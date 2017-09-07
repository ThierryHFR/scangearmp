/*
 *  ScanGear MP for Linux
 *  Copyright CANON INC. 2007-2014
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA.
 *
 * NOTE:
 *  - As a special exception, this program is permissible to link with the
 *    libraries released as the binary modules.
 *  - If you write modifications of your own for these programs, it is your
 *    choice whether to permit this exception to apply to your modifications.
 *    If you do not wish that, delete this exception.
*/


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/*	#define _CMT_DEBUG */

#define _CMT_DEBUG_NAME_ cm_tools

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <usb.h>
#include <errno.h>
#include <pthread.h>

#include "canon_mfp_tools.h"

#define LIBUSB_DEV_MAX (16)
typedef struct {
	struct usb_device *dev;
	usb_dev_handle *handle;
	unsigned int bcdUSB;
	int idVendor;
	int idProduct;
	char *devname;
	int interface;
	int ep_bulk_in_address;
	int ep_bulk_out_address;
	int opened;
} LIB_USB_DEV;

static int libusb_inited = 0;
static int network_inited = 0;

static LIB_USB_DEV libusbdev[LIBUSB_DEV_MAX];



/*
	definitions for network
*/
#define CACHE_PATH				DEFTOSTR( CNMSLIBPATH ) "/canon_mfp_net.ini"
#define NETWORK_DEV_MAX			(64)
#define STRING_SHORT			(32)

#define WAIT_SECOND				{usleep(500000);usleep(500000);}
#define WAIT_300MSEC			{usleep(300000);}
#define WAIT_50MSEC				{usleep( 50000);}

#define SESSION_TIMEOUT			(25)

#define	RETRY_COUNT_START_SESSION		(3)
#define	RETRY_COUNT_KEEP_SESSION_CANCEL	(50)

int					cmt_network_mode = 0;
pthread_mutex_t		cmt_net_mutex;
pthread_t			cmt_network_thread = (pthread_t)NULL;

static int			cmt_net_aborted = 0;

typedef struct {
	char 		modelName[STRING_SHORT];
	char 		ipAddStr[STRING_SHORT];
	char 		macAddStr[STRING_SHORT];
	CNNLNICINFO	nic;
} NETWORK_DEV;

static NETWORK_DEV	networkdev[NETWORK_DEV_MAX];



/*
	open conf file
*/
FILE *cmt_conf_file_open(const char *conf)
{
	char *path = DEFTOSTR( CNMSSANECONFPATH );
	char dst[PATH_MAX];
	FILE *fp = NULL;
	
	if ( !conf ) return NULL;
	
	memset( dst, 0, sizeof(dst) );
	snprintf( dst, sizeof(dst), "%s/%s", path, conf );
	DBGMSG( "cmt_conf_file_open() : conf file \"%s\".\n", dst );
	fp = fopen( dst, "r" );
	if (fp) {
		DBGMSG( "cmt_conf_file_open() : use conf file \"%s\".\n", dst );
	}
	else {
		DBGMSG( "cmt_conf_file_open() : could not open conf file \"%s\".\n", (char *)conf );
	}

	return fp;
}

/*
	return : length of line or -1(EOF).
*/
int cmt_conf_file_read_line(char *line, int size, FILE *fp)
{
	int length = 0;
	char *c;
	
	if ( fp ) {
		memset( line, 0, size );
		
		c = fgets( line, size, fp );
		
		if ( !c ) {
			/* EOF */
			return -1;
		}
		/* comment? */
		if ( line[0] == '#' ) {
			length = 0;
		} else {
			length = strlen( line );
		}
	}
	else {
		return -1;
	}
	return length;
}

int cmt_get_device_id( char *line, int len )
{
	int idVendor, idProduct = -1;
	char *start_v = NULL;
	char *start_p = NULL;
	int len_v, len_p;
	
	/* check "usb" or "net"*/
	if( len < 3 ) goto _EXIT;
	if ( strncmp( line, "usb", 3 ) ) {
		if ( strncmp( line, "net", 3 ) ) {
			goto _EXIT;
		}
		else {
			idProduct = 0;	/* "net" */
			goto _EXIT;
		}
	}
	/* "usb" */
	line +=3;
	len -= 3;
	
	/* skip space */
	while( isspace( *line ) ) {
		line++;
		len--;
		if( !len ) {
			goto _EXIT;
		}
	}
	/* get vendor id */
	start_v = line;
	len_v = len;
	while( !isspace( *line ) ) {
		line++;
		len--;
		if( !len ) {
			goto _EXIT;
		}
	}
	len_v -= len;
	/* skip space */
	while( isspace( *line ) ) {
		line++;
		len--;
		if( !len ) {
			goto _EXIT;
		}
	}
	/* get product id */
	start_p = line;
	len_p = len;
	while( !isspace( *line ) ) {
		line++;
		len--;
		if( !len ) {
			break;
		}
	}
	len_p -= len;
	
	*(start_v + len_v) = '\0';
	*(start_p + len_p) = '\0';
	
	idVendor = strtol( start_v, 0, 0 );
	
	if ( idVendor == _CANON_VENDOR_ID ) {
		idProduct = strtol( start_p, 0, 0 );
	}

_EXIT:
	return idProduct;
}

/*
	find canon_mfp device.
*/
char *cmt_find_device( int id, int *index )
{
	int i;
	
	if ( *index < 0 ) {
		return NULL; /* error */
	}
	DBGMSG( "cmt_find_device() : [0x%4X]\n", id );
	for ( i = *index ; i < LIBUSB_DEV_MAX; i++) {
		if ( libusbdev[i].idVendor == _CANON_VENDOR_ID && libusbdev[i].idProduct == id ) {
			if ( libusbdev[i].devname ) {
				DBGMSG( "cmt_find_device() : [0x%4X(%s)]\n", id, libusbdev[i].devname );
				*index = i;
				return libusbdev[i].devname;
			}
		}
	}
	return NULL;
}

char *cmt_find_device_net( char *line, int len, int *index, char **model )
{
	int i, modelName_len;
	
	if( !network_inited ) return NULL;
	
	if ( *index < 0 ) {
		return NULL; /* error */
	}
	*model = NULL;
	
	/* skip "net" */
	line += 3;
	len -= 3;
	/* skip space */
	while( isspace( *line ) ) {
		line++;
		len--;
		if( !len ) {
			return NULL;
		}
	}
	
	for ( i = *index ; i < NETWORK_DEV_MAX; i++) {
		modelName_len = strlen(networkdev[i].modelName);
		if ( len < modelName_len || !modelName_len ) {
			continue;
		}
		if ( strncmp( networkdev[i].modelName, line, modelName_len ) == 0 ){
			*index = i;
			*model = networkdev[i].modelName;
			return networkdev[i].macAddStr;
		}
	}
	
	return NULL;
}


/*
	initialize libusb.
*/
void cmt_libusb_init(void)
{
	struct usb_bus *bus;
	struct usb_device *dev;
	char name[256];
	int scanner_num = 0;

	/* inited already. */
	if ( libusb_inited ) return;
	
	memset( libusbdev, 0, sizeof(libusbdev) );
	
	libusb_inited = 1;
	
	usb_init();
	
	usb_find_busses();
	usb_find_devices();

	for ( bus = usb_busses; bus; bus = bus->next ) {
		for ( dev = bus->devices; dev; dev = dev->next ) {
			int interface;
			int is_scanner = 0;
			
			/* find CANON device */
			if ( dev->descriptor.idVendor != _CANON_VENDOR_ID ) {
				DBGMSG( "cmt_libusb_init() : [0x%04X:0x%04X] is not CANON device.\n",
					 dev->descriptor.idVendor, dev->descriptor.idProduct );
				continue;
			}
			
			if ( !dev->config ) {
				DBGMSG( "cmt_libusb_init() : could not retrieve descriptors\n" );
				continue;
			}
			/* find scanner device */
			if ( dev->descriptor.bDeviceClass == USB_CLASS_PER_INTERFACE ) {
				for ( interface = 0; interface < dev->config[0].bNumInterfaces; interface++) {
					if ( dev->config[0].interface[interface].altsetting[0].bInterfaceClass == USB_CLASS_VENDOR_SPEC ) {
						is_scanner = 1; /* CANON MFP scanner */
						break;
					}
				}
			}
			if ( !is_scanner ) {
				DBGMSG( "cmt_libusb_init() : [0x%04X:0x%04X] is not CANON MFP. (CANON printer maybe)\n",
					 dev->descriptor.idVendor, dev->descriptor.idProduct );
				continue;
			}
			
			snprintf( name, sizeof(name), "libusb:%s:%s", dev->bus->dirname, dev->filename );
			libusbdev[scanner_num].dev			= dev;
			libusbdev[scanner_num].bcdUSB		= dev->descriptor.bcdUSB;
			libusbdev[scanner_num].idVendor		= _CANON_VENDOR_ID;
			libusbdev[scanner_num].idProduct	= dev->descriptor.idProduct;
			libusbdev[scanner_num].devname		= strdup( name );
			libusbdev[scanner_num].interface	= interface;
			libusbdev[scanner_num].opened		= 0;
			
			DBGMSG( "cmt_libusb_init() : libusbdev[scanner_num].bcdUSB = %d.%02d\n", ( libusbdev[scanner_num].bcdUSB >> 8 ), ( libusbdev[scanner_num].bcdUSB & 0xFF )  );
			
			DBGMSG( "cmt_libusb_init() : CANON MFP found! [0x%04X:0x%04X] %s\n",
				 dev->descriptor.idVendor, libusbdev[scanner_num].idProduct, libusbdev[scanner_num].devname );
			
			if ( ++scanner_num == LIBUSB_DEV_MAX ) {
				DBGMSG( "cmt_libusb_init() : can not add device anymore.\n" );
				return;
			}
		}
	}
	if( scanner_num ) {
		DBGMSG( "cmt_libusb_init() : CANON MFP num = %d\n", scanner_num );
	}
}


/*
	dispose LIB_USB_DEV.
*/
void cmt_libusb_exit(void)
{
	int i;
	
	if ( !libusb_inited ) return;
	
	for ( i = 0; i < LIBUSB_DEV_MAX ; i++ ) {
		if ( libusbdev[i].devname ) {
			free( libusbdev[i].devname );
		}
	}
	libusb_inited = 0;
}


/*
	open libusb devices.
*/
SANE_Status cmt_libusb_open(const char *devname, int *index)
{
	int len;
	struct usb_device *dev;
	int ret;

	int dev_index, ep_no, numEndpoints;
	struct usb_endpoint_descriptor *endpoint;
	
	if ( !libusb_inited ) return SANE_STATUS_INVAL;
	if ( !devname ) return SANE_STATUS_INVAL;
	if ( !index ) return SANE_STATUS_INVAL;

	len = strlen( devname );
	for ( dev_index = 0; dev_index < LIBUSB_DEV_MAX ; dev_index++ ) {
		if ( libusbdev[dev_index].devname ) {
			if ( strncmp( devname, libusbdev[dev_index].devname, len ) == 0 ) {
				DBGMSG("cmt_libusb_open() : found [%s].\n", devname);
				break; /* found! */
			}
		}
	}
	if ( dev_index == LIBUSB_DEV_MAX ) {
		DBGMSG( "cmt_libusb_open() : could not find [%s].\n", devname );
		return SANE_STATUS_INVAL;
	}
	if ( libusbdev[dev_index].opened ) {
		DBGMSG( "cmt_libusb_open() : [%s] already open.\n", devname );
		return SANE_STATUS_INVAL;
	}
	
	/* open device */
	if ( ( libusbdev[dev_index].handle = usb_open( libusbdev[dev_index].dev ) ) == NULL ) {
		DBGMSG( "cmt_libusb_open() : usb_open error [%s].\n", devname );
		return SANE_STATUS_INVAL;
	}
	
	dev = usb_device( libusbdev[dev_index].handle );
	if ( !dev->config ) {
		DBGMSG( "cmt_libusb_open() : could not retrieve descriptors\n" );
		return SANE_STATUS_INVAL;
	}
	/* set configuration */
	ret = usb_set_configuration( libusbdev[dev_index].handle, dev->config[0].bConfigurationValue );
	
	if ( ret ) {
		if ( errno == EBUSY ) {
			DBGMSG( "cmt_libusb_open() : Though ioctl() returns EBUSY, go to next step.\n" );
		}
		else {
			DBGMSG( "cmt_libusb_open() : %s\n", strerror(errno) );
			
			if ( errno == EPERM ) {
				ret = SANE_STATUS_ACCESS_DENIED;
			}
			else {
				ret = SANE_STATUS_INVAL;
			}
			usb_close( libusbdev[dev_index].handle );
			libusbdev[dev_index].handle = NULL;
			
			return ret;
		}
	}
	/* claim interface */
	ret = usb_claim_interface( libusbdev[dev_index].handle, libusbdev[dev_index].interface );
	if ( ret ) {
		DBGMSG( "cmt_libusb_open() : could not claim interface\n" );
		usb_close( libusbdev[dev_index].handle );
		libusbdev[dev_index].handle = NULL;
		
		return SANE_STATUS_INVAL;
	}
	/* set endpoints (bulk-in, bulk-out) */
	numEndpoints = dev->config[0].interface->altsetting[0].bNumEndpoints;
	endpoint = dev->config[0].interface->altsetting[0].endpoint;
	for ( ep_no = 0; ep_no < numEndpoints; ep_no++, endpoint++) {
		if ( ( endpoint->bmAttributes & USB_ENDPOINT_TYPE_MASK ) != USB_ENDPOINT_TYPE_BULK ) {
			continue;
		}
		/* bulk-in or bulk-out */
		if ( ( endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK ) == USB_ENDPOINT_IN ) {
			libusbdev[dev_index].ep_bulk_in_address = endpoint->bEndpointAddress & USB_ENDPOINT_ADDRESS_MASK;
		}
		else if ( ( endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK ) == USB_ENDPOINT_OUT ) {
			libusbdev[dev_index].ep_bulk_out_address = endpoint->bEndpointAddress & USB_ENDPOINT_ADDRESS_MASK;
		}
	}
	
	*index = dev_index;
	libusbdev[dev_index].opened = 1;
	return SANE_STATUS_GOOD;
}


/*
	close libusb devices.
*/
void cmt_libusb_close(int index)
{
	if ( index > LIBUSB_DEV_MAX ) {
		DBGMSG( "cmt_libusb_close() : can not close this device.\n" );
	}
	if ( libusbdev[index].opened ) {
		libusbdev[index].opened = 0;
		if ( libusbdev[index].handle ) {
			usb_release_interface( libusbdev[index].handle, libusbdev[index].interface );
			usb_close( libusbdev[index].handle );
			/* bzero */
			libusbdev[index].handle = NULL;
		}
	}
}

SANE_Status cmt_libusb_get_id( const char *devname, int *idVendor, int *idProduct, int *speed )
{
	int len;
	int dev_index;
	
	if ( !libusb_inited ) return SANE_STATUS_INVAL;
	if ( !devname ) return SANE_STATUS_INVAL;

	len = strlen( devname );
	for ( dev_index = 0; dev_index < LIBUSB_DEV_MAX ; dev_index++ ) {
		if ( libusbdev[dev_index].devname ) {
			if ( strncmp( devname, libusbdev[dev_index].devname, len ) == 0 ) {
				DBGMSG("cmt_libusb_get_id() : found [%s].\n", devname);
				break; /* found! */
			}
		}
	}
	if ( dev_index == LIBUSB_DEV_MAX ) {
		DBGMSG( "cmt_libusb_get_id() : could not find [%s].\n", devname );
		return SANE_STATUS_INVAL;
	}
	*idVendor = libusbdev[dev_index].idVendor;
	*idProduct = libusbdev[dev_index].idProduct;
	*speed = libusbdev[dev_index].bcdUSB >> 8; /* USB2.0->2, USB1.1->1 */
	
	return SANE_STATUS_GOOD;
}

#define LIBUSB_TIMEOUT 20000	/* 20sec */
/*
	bulk-write to libusb devices.
*/
SANE_Status cmt_libusb_bulk_write( int index, unsigned char *buffer, unsigned long *size )
{
	int ret_bytes, request_bytes;
	
	if ( !buffer || !size ) {
		return SANE_STATUS_INVAL;
	}
	if ( index > LIBUSB_DEV_MAX || index < 0 ) {
		return SANE_STATUS_INVAL;
	}
	if ( libusbdev[index].handle == NULL || !libusbdev[index].ep_bulk_in_address ) {
		return SANE_STATUS_INVAL;
	}
	
	request_bytes = *size;
	ret_bytes = usb_bulk_write( libusbdev[index].handle, libusbdev[index].ep_bulk_out_address,
					(char *)buffer, request_bytes, LIBUSB_TIMEOUT );
	
	if ( ret_bytes < 0 ) { /* error happend. */
		usb_clear_halt( libusbdev[index].handle, libusbdev[index].ep_bulk_out_address );
		*size = 0;
		DBGMSG( "cmt_libusb_bulk_write() : IO error.(%d)\n", ret_bytes );
		return SANE_STATUS_IO_ERROR;
	}
	
	*size = ret_bytes;
	
	return SANE_STATUS_GOOD;
}


#define LIBUSB_READ_MAX_SIZE	0x4000	/* 16k */
/*
	bulk-read from libusb devices.
*/
SANE_Status cmt_libusb_bulk_read( int index, unsigned char *buffer, unsigned long *size )
{
	int ret_bytes, request_bytes;
	
	if ( !buffer || !size ) {
		return SANE_STATUS_INVAL;
	}
	if ( index > LIBUSB_DEV_MAX || index < 0 ) {
		return SANE_STATUS_INVAL;
	}
	if ( libusbdev[index].handle == NULL || !libusbdev[index].ep_bulk_in_address ) {
		return SANE_STATUS_INVAL;
	}
	
	request_bytes = ( *size > LIBUSB_READ_MAX_SIZE ) ? LIBUSB_READ_MAX_SIZE : *size;
	ret_bytes = usb_bulk_read( libusbdev[index].handle, libusbdev[index].ep_bulk_in_address,
					(char *)buffer, request_bytes, LIBUSB_TIMEOUT );
	
	if ( ret_bytes < 0 ) { /* error happend. */
		usb_clear_halt( libusbdev[index].handle, libusbdev[index].ep_bulk_in_address );
		*size = 0;
		DBGMSG( "cmt_libusb_bulk_read() : IO error.(%d)\n", ret_bytes );
		return SANE_STATUS_IO_ERROR;
	}
	
	*size = ret_bytes;
	
	return SANE_STATUS_GOOD;
}


/*
	initialize network devices list.
*/
void cmt_network_init( int network )
{
	CNNLHANDLE hmdl=NULL;
	int i=0, j=0, k=0, max = NETWORK_DEV_MAX, found=0, found_cache=0, timeout_msec = 0;
	CNNLNICINFO *nic;
	char model[STRING_SHORT], ipaddr[STRING_SHORT];
	unsigned long version = 110, versize;

#ifdef	__USE_LIBSANE__
	int cnnl_mode = CNNET_SEARCH_AUTO;
#else
	int cnnl_mode = CNNET_SEARCH_CACHE_ACTIVEONLY;
#endif

	if( !network ) return;

	if( network_inited ) return;
	network_inited = 1;

	versize = sizeof( unsigned long );
	
	memset( networkdev, 0, sizeof(networkdev) );

	if( CNNL_Init( &hmdl ) != CNNL_RET_SUCCESS ) goto error;
	if( CNNL_Config( hmdl, CNNL_CONFIG_SET_VERSION, &version, &versize ) != CNNL_RET_SUCCESS) goto error;
	
	memset(model, 0x00, sizeof(model));
	
	if( ( nic = (CNNLNICINFO *)malloc(sizeof(CNNLNICINFO)*max) ) == NULL ) goto error;
	
	// count cache num
	if ( CNNL_SearchPrintersEx( hmdl, nic, CACHE_PATH, max, &found_cache, CNNET_SEARCH_CACHE_ALL, 1, 5000 ) != CNNL_RET_SUCCESS ){
		free(nic);
		goto error;
	}
	timeout_msec = ( found_cache ) ? found_cache * 1500 : 5000;
	DBGMSG( "cmt_network_init() : cache num = %d, timeout = %d msec\n", found_cache, timeout_msec );
	
	// find printers
	memset(nic, 0x00, sizeof(CNNLNICINFO)*max);
	if( CNNL_SearchPrintersEx( hmdl, nic, CACHE_PATH, max, &found, cnnl_mode, 1, timeout_msec ) == CNNL_RET_SUCCESS ){
		for (j=0; j<found; j++){
			
			memset(ipaddr, 0x00, STRING_SHORT);
			snprintf(ipaddr, STRING_SHORT-1, "%d.%d.%d.%d", 
					nic[j].ipaddr[0],nic[j].ipaddr[1],nic[j].ipaddr[2],nic[j].ipaddr[3]);
			
			if( CNNL_OpenEx( hmdl, ipaddr, CNNET_TYPE_MULTIPASS, 1, 1000 ) == CNNL_RET_SUCCESS ){
				if( CNNL_GetModelName( hmdl, model, STRING_SHORT, 3, 3000) == CNNL_RET_SUCCESS){
					
					strncpy( networkdev[j].modelName, model, STRING_SHORT-1 );
					strncpy( networkdev[j].ipAddStr, ipaddr, STRING_SHORT-1 );
					snprintf( networkdev[j].macAddStr, STRING_SHORT-1, "%02X-%02X-%02X-%02X-%02X-%02X",
						nic[j].macaddr[0],nic[j].macaddr[1],nic[j].macaddr[2], nic[j].macaddr[3],nic[j].macaddr[4],nic[j].macaddr[5] );
					
					for( k = 0 ; k < 4 ; k++ ){
						networkdev[j].nic.ipaddr[k] = nic[j].ipaddr[k];
					}
					for( k = 0 ; k < 6 ; k++ ){
						networkdev[j].nic.macaddr[k] = nic[j].macaddr[k];
					}
					
					DBGMSG("cmt_network_init() : CANON MFP found! %s (%02X-%02X-%02X-%02X-%02X-%02X)\n", 
						model,
						nic[j].macaddr[0],nic[j].macaddr[1],nic[j].macaddr[2],
						nic[j].macaddr[3],nic[j].macaddr[4],nic[j].macaddr[5]);

				}
				CNNL_Close( hmdl );
			}
		}
	} else {
		free(nic);
		goto error;
	}
	
	free(nic);
	CNNL_Terminate( &hmdl );
	
	return;
	
error:
	if (hmdl!= NULL) CNNL_Terminate( &hmdl );

	return;
}

void cmt_network_exit( void )
{
	if ( !network_inited ) return;
	network_inited = 0;
}

void cmt_network_mutex_lock( void )
{
	if( cmt_network_mode ) {
		DBGMSG( "cmt_network_mutex_lock()...\n" );
		pthread_mutex_lock( &cmt_net_mutex );
	}
}

static int cmt_network_mutex_trylock( void )
{
	if( cmt_network_mode ) {
		DBGMSG( "cmt_network_mutex_trylock()...\n" );
		return pthread_mutex_trylock( &cmt_net_mutex );
	}
	else {
		return -1;
	}
}

void cmt_network_mutex_unlock( void )
{
	if( cmt_network_mode ) {
		DBGMSG( "cmt_network_mutex_unlock()...\n" );
		pthread_mutex_unlock( &cmt_net_mutex );
	}
}

void cmt_network_keep_session( CNNLHANDLE hnd )
{
	int				i;
	unsigned long	d_time;
	
	while(1) {
		/* sleep and wait pthread_testcancel */
		for( i = 0; i < RETRY_COUNT_KEEP_SESSION_CANCEL; i++ ) {
			pthread_testcancel();
			DBGMSG( "cmt_network_keep_session() : wait 300 msec... (%02d/%d)\n", i+1, RETRY_COUNT_KEEP_SESSION_CANCEL );
			WAIT_300MSEC;
		}
		
		DBGMSG( "<cmt_network_keep_session>\n" );
		if( cmt_network_mutex_trylock() ) {
			DBGMSG( "cmt_network_keep_session() : cmt_network_mutex_trylock() failed.\n" );
		}
		else {
			if( !cmt_net_aborted ) {
				DBGMSG( "cmt_network_keep_session() : call CNNL_GetTimeout()\n" );
				CNNL_GetTimeout( hnd, &d_time, 3, 3000);
			}
			cmt_network_mutex_unlock();
		}
	}
}


/*
	open network devices.
*/
SANE_Status cmt_network_open(const char *macaddr, CNNLHANDLE *handle)
{
	char		ipaddr[64];
	int			ret, j;
	SANE_Status	status = SANE_STATUS_IO_ERROR;
	CNNLHANDLE	hnd = NULL;

	if( CNNL_Init( &hnd ) != CNNL_RET_SUCCESS ){
		goto error;
	}
	if( CNNL_GetIPAddressEx( hnd, CACHE_PATH, macaddr, ipaddr, STRING_SHORT, CNNET_SEARCH_CACHE_ALL, 1, 5000 ) != CNNL_RET_SUCCESS ){
		goto	error;
	}
	if( CNNL_OpenEx( hnd, ipaddr, CNNET_TYPE_MULTIPASS, 1, 5000 ) != CNNL_RET_SUCCESS ){
		goto	error1;
	}
	if( CNNL_CheckVersion( hnd, 3, 5000 ) != CNNL_RET_SUCCESS ){
		goto	error1;
	}
	
	for( j = 0; j < RETRY_COUNT_START_SESSION; j++) {
		if( ( ret = CNNL_SessionStart( hnd, "\0", "\0", "\0", 3, 9000 ) ) == CNNL_RET_SUCCESS ){
			/* succeed to start session */
			DBGMSG( "cmt_network_open() : succeed to start session.\n" );
			break;
		}
		/* failed to start session */
		if (ret == CNNL_RET_BUSY){
			DBGMSG( "cmt_network_open() : wait 1 sec...\n" );
			WAIT_SECOND;
		}
		else {
			goto error2;
		}
	}
	/* failed to start session (busy error) */
	if( j == RETRY_COUNT_START_SESSION ) {
		status = SANE_STATUS_DEVICE_BUSY;
		goto error2;
	}
	
	/* set timeout */
	CNNL_SetTimeout( hnd, SESSION_TIMEOUT, 3, 30000 );
	
	/* keep TCP session thread */
	pthread_mutex_init( &cmt_net_mutex, NULL );
	if( ( ret = pthread_create( &cmt_network_thread, NULL, (void *(*)(void*))cmt_network_keep_session, hnd ) ) ) {
		goto error2;
	}
	cmt_network_mode = 1;
	cmt_net_aborted = 0;
	
	*handle = hnd;
	return SANE_STATUS_GOOD;

error2:
		CNNL_SessionEnd( hnd, 3, 9000 );
error1:
		CNNL_Close( hnd );
error:
	if( hnd ){
		CNNL_Terminate( &hnd );
	}
	*handle = hnd;
	return status;	/* SANE_STATUS_IO_ERROR or SANE_STATUS_DEVICE_BUSY */
}

/*
	close network devices.
*/
void cmt_network_close(CNNLHANDLE *handle)
{
	CNNLHANDLE	hnd = *handle;

	if( hnd ){
		if ( cmt_network_thread ) {
			cmt_network_mutex_lock();
			pthread_cancel( cmt_network_thread );
			pthread_join( cmt_network_thread, NULL );
			cmt_network_thread = (pthread_t)NULL;
			DBGMSG( "cmt_network_close() : cmt_network_thread canceled.\n" );
			cmt_network_mutex_unlock();
		}
		pthread_mutex_destroy( &cmt_net_mutex );
		
		CNNL_SessionEnd( hnd, 3, 1000 );
		CNNL_Close( hnd );
		CNNL_Terminate( &hnd );
	}

	*handle = hnd;	
	return;
}

/*
	write to canon network devices.
*/
SANE_Status cmt_network_write( CNNLHANDLE handle, unsigned char *buffer, unsigned long *size )
{
	int request_bytes, status = -1;
	unsigned long	ret_bytes = 0;

	if ( !buffer || !size ) {
		return SANE_STATUS_INVAL;
	}
	if ( handle == NULL ) {
		return SANE_STATUS_INVAL;
	}
	if( cmt_net_aborted ) {
		*size = 0;
		return SANE_STATUS_IO_ERROR;
	}
	
	request_bytes = *size;
	status = CNNL_DataWrite( handle, (char*)buffer, request_bytes, &ret_bytes, 3, 30000 );
	
	if ( status != CNNL_RET_SUCCESS ) { /* error happend. */
		DBGMSG( "cmt_network_write() : IO error.\n" );
		*size = 0;
		CNNL_Abort( handle );
		cmt_net_aborted = -1;
		return SANE_STATUS_IO_ERROR;
	}
	
	*size = ret_bytes;
	
	return SANE_STATUS_GOOD;
}


/*
	read from canon network devices.
*/
SANE_Status cmt_network_read( CNNLHANDLE handle, unsigned char *buffer, unsigned long *size )
{
	int request_bytes, status = -1;
	unsigned long	ret_bytes = 0;
	
	
	if ( !buffer || !size ) {
		return SANE_STATUS_INVAL;
	}
	if ( handle == NULL ) {
		return SANE_STATUS_INVAL;
	}
	if( cmt_net_aborted ) {
		*size = 0;
		return SANE_STATUS_IO_ERROR;
	}
	
	request_bytes = *size;
	status = CNNL_DataRead( handle, buffer, &ret_bytes, request_bytes, 3, 30000 );
	
	if ( status != CNNL_RET_SUCCESS ) { /* error happend. */
		DBGMSG( "cmt_network_read() : IO error.\n" );
		*size = 0;
		CNNL_Abort( handle );
		cmt_net_aborted = -1;
		return SANE_STATUS_IO_ERROR;
	}
	
	*size = ret_bytes;
	
	return SANE_STATUS_GOOD;
}

