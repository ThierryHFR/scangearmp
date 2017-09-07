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

/* #define __USE_LIBSANE__	*/

#ifdef __USE_LIBSANE__
	#define SCAN_BITS_APIV2		(8)
#else
	#define SCAN_BITS_APIV2		(16)
#endif

/*	#define _CMT_DEBUG	*/

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include <sane/sane.h>

#define CANON_CONFIG_FILE "canon_mfp.conf"

#define MM_PER_INCH	 25.4

#define BACKEND_NAME canon_mfp
#define _CMT_DEBUG_NAME_ BACKEND_NAME

#include "canon_mfp_tools.h"
#include "canon_mfp.h"
#include "cnmslld.h"
#include "canon_mfp_opts.h"


/*---------------------------------------------------------------------------------------*/
/*    definitions for SANE ---->                                                         */
/*---------------------------------------------------------------------------------------*/
typedef struct {
	SANE_Word			product;
	SANE_Word			api_ver;
	SANE_String_Const	model;
	const char 			*libname;
} CANON_PRODUCT_INFO;

static int num_devices = 0;
static CANON_Device *first_dev = NULL;
static CANON_Scanner *opened_handle = NULL;
static const SANE_Device **devlist = NULL;

static CANON_PRODUCT_INFO canon_mfp_product_ids[] = {
	{ 0x1714,	0,	"MP160", "libcncpmslld.so" },
	{ 0x1717,	0,	"MP510", "libcncpmslld.so" },
	{ 0x1718,	0,	"MP600", "libcncpmslld.so" },
	/* Ver.1.10 */
	{ 0x172B,	0,	"MP140 series", "libcncpmslld315.so" },
	{ 0x1721,	1,	"MP210 series", "libcncpmslld316.so" },
	{ 0x1724,	1,	"MP520 series", "libcncpmslld328.so" },
	{ 0x1725,	1,	"MP610 series", "libcncpmslld327.so" },
	/* Ver.1.20 */
	{ 0x1734,	1,	"MP190 series", "libcncpmslld342.so" },
	{ 0x1732,	2,	"MP240 series", "libcncpmslld341.so" },
	{ 0x1730,	2,	"MP540 series", "libcncpmslld338.so" },
	{ 0x172E,	2,	"MP630 series", "libcncpmslld336.so" },
	/* Ver.1.30 */
	{ 0x1736,	2,	"MX320 series", "libcncpmslld348.so" },
	{ 0x1737,	2,	"MX330 series", "libcncpmslld349.so" },
	{ 0x1735,	2,	"MX860 series", "libcncpmslld347.so" },
	/* Ver.1.40 */
	{ 0x173A,	2,	"MP250 series", "libcncpmslld356.so" },
	{ 0x173B,	2,	"MP270 series", "libcncpmslld357.so" },
	{ 0x173C,	2,	"MP490 series", "libcncpmslld358.so" },
	{ 0x173D,	2,	"MP550 series", "libcncpmslld359.so" },
	{ 0x173E,	2,	"MP560 series", "libcncpmslld360.so" },
	{ 0x173F,	2,	"MP640 series", "libcncpmslld362.so" },
	/* Ver.1.50 */
	{ 0x1741,	2,	"MX340 series", "libcncpmslld365.so" },
	{ 0x1742,	2,	"MX350 series", "libcncpmslld366.so" },
	{ 0x1743,	2,	"MX870 series", "libcncpmslld367.so" },
	/* Ver.1.60 */
	{ 0x1746,	2,	"MP280 series",		"libcncpmslld370.so" },
	{ 0x1747,	2,	"MP495 series",		"libcncpmslld369.so" },
	{ 0x1748,	2,	"MG5100 series",	"libcncpmslld373.so" },
	{ 0x1749,	2,	"MG5200 series",	"libcncpmslld374.so" },
	{ 0x174A,	2,	"MG6100 series",	"libcncpmslld376.so" },
	{ 0x174B,	2,	"MG8100 series",	"libcncpmslld377.so" },
	/* Ver.1.70 */
	{ 0x174D,	2,	"MX360 series", "libcncpmslld380.so" },
	{ 0x174E,	2,	"MX410 series", "libcncpmslld381.so" },
	{ 0x174F,	2,	"MX420 series", "libcncpmslld382.so" },
	{ 0x1750,	2,	"MX880 series", "libcncpmslld383.so" },
	/* Ver.1.80 */
	{ 0x1751,	2,	"MG2100 series",	"libcncpmslld386.so" },
	{ 0x1752,	2,	"MG3100 series",	"libcncpmslld387.so" },
	{ 0x1753,	2,	"MG4100 series",	"libcncpmslld388.so" },
	{ 0x1754,	2,	"MG5300 series",	"libcncpmslld389.so" },
	{ 0x1755,	2,	"MG6200 series",	"libcncpmslld390.so" },
	{ 0x1756,	2,	"MG8200 series",	"libcncpmslld391.so" },
	{ 0x1758,	2,	"E500 series",		"libcncpmslld393.so" },
	/* Ver.1.90 */
	{ 0x175D,	3,	"MX710 series",	"libcncpmslld394.so" },
	{ 0x175E,	3,	"MX890 series",	"libcncpmslld395.so" },
	{ 0x1759,	3,	"MX370 series",	"libcncpmslld396.so" },
	{ 0x175B,	3,	"MX430 series",	"libcncpmslld397.so" },
	{ 0x175C,	3,	"MX510 series",	"libcncpmslld398.so" },
	{ 0x175A,	3,	"E600 series",	"libcncpmslld399.so" },
	/* Ver.2.00 */
	{ 0x175F,	3,	"MP230 series",		"libcncpmslld401.so" },
	{ 0x1760,	3,	"MG2200 series",	"libcncpmslld402.so" },
	{ 0x1761,	3,	"E510 series",		"libcncpmslld403.so" },
	{ 0x1762,	3,	"MG3200 series",	"libcncpmslld404.so" },
	{ 0x1763,	3,	"MG4200 series",	"libcncpmslld405.so" },
	{ 0x1764,	3,	"MG5400 series",	"libcncpmslld407.so" },
	{ 0x1765,	3,	"MG6300 series",	"libcncpmslld408.so" },
	/* Ver.2.10 */
	{ 0x176A,	3,	"MX720 series",	"libcncpmslld416.so" },
	{ 0x176B,	3,	"MX920 series",	"libcncpmslld417.so" },
	{ 0x1766,	3,	"MX390 series",	"libcncpmslld418.so" },
	{ 0x1768,	3,	"MX450 series",	"libcncpmslld419.so" },
	{ 0x1769,	3,	"MX520 series",	"libcncpmslld420.so" },
	{ 0x1767,	3,	"E610 series",	"libcncpmslld421.so" },
	/* Ver.2.20 */
	{ 0x1772,	3,	"MG7100 series",	"libcncpmslld423.so" },
	{ 0x176F,	3,	"MG6500 series",	"libcncpmslld424.so" },
	{ 0x1770,	3,	"MG6400 series",	"libcncpmslld425.so" },
	{ 0x1771,	3,	"MG5500 series",	"libcncpmslld426.so" },
	{ 0x176E,	3,	"MG3500 series",	"libcncpmslld427.so" },
	{ 0x176C,	3,	"MG2400 series",	"libcncpmslld428.so" },
	{ 0x176D,	3,	"MG2500 series",	"libcncpmslld429.so" },
	{ 0x1773,	3,	"P200 series",		"libcncpmslld430.so" },
	/* Ver.2.30 */
	{ 0x1774,	3,	"MX470 series",	"libcncpmslld434.so" },
	{ 0x1775,	3,	"MX530 series",	"libcncpmslld435.so" },
	{ 0x177B,	3,	"E560 series",	"libcncpmslld437.so" },
	{ 0x177A,	3,	"E400 series",	"libcncpmslld438.so" },
	{ -1, -1, "", "", },
};

/*---------------------------------------------------------------------------------------*/
/*    scanner info for SANE ---->                                                        */
/*---------------------------------------------------------------------------------------*/
static const SANE_String_Const mode_list[] =	/* "ScanMode" */
{
	"Gray",
	"Color",
	0
};

/*--- resolution list ---*/
static SANE_Int xres_list[16];
static SANE_Int yres_list[16];

/*---------------------------------------------------------------------------------------*/
/*    internal functions for SANE ---->                                                  */
/*---------------------------------------------------------------------------------------*/
static void *canon_mfp_lib = NULL;	/* canon_mfp_lib handle */
static SANE_Int CANON_fd = -1;		/* file descriptor of canon_mfp device */

static CNNLHANDLE CANON_hnd = NULL;		/* handle of canon_mfp network device */


/*---------------------------------------------------------------------------------------*/
/*    internal functions for Network ---->                                                  */
/*---------------------------------------------------------------------------------------*/
static int canon_get_network_installed( void );

#include "canon_mfp_debug.h"

static SANE_Status canon_usb_open( SANE_String_Const devname, CANON_Scanner *s )
{
	SANE_Status status;

	cmd_init_usb_open();
	
	if ( !s ) {
		return SANE_STATUS_INVAL;
	}
	if ( CANON_fd < 0 ) {
		if ( ( status = cmt_libusb_open ((const char *)devname, &s->fd) ) != SANE_STATUS_GOOD ) {
			return status;
		}
		CANON_fd = s->fd;
		
		return SANE_STATUS_GOOD;
	}
	else { /* already open. */
		return SANE_STATUS_IO_ERROR;
	}
}

static void canon_usb_close( CANON_Scanner *s )
{
	if ( !s ) {
		return ;
	}
	if ( CANON_fd < 0 ) {
		return ;
	}
	else {
		cmt_libusb_close( s->fd );
		CANON_fd = -1;
	}
}

static int canon_usb_write( unsigned char * buffer, unsigned long size )
{
	unsigned long n = size;
	SANE_Status status;
	
	cmd_init_usb_write();
	
	if ( CANON_fd < 0 ) {
		return -1; /* error */
	}
	status = cmt_libusb_bulk_write ( CANON_fd, buffer, &n );
	
	if ( status != SANE_STATUS_GOOD ) {
		return -1; /* error */
	}
	
	if ( size != n ) {
		return n; /* error */
	}
	return 0; /* no error */
}

static int canon_usb_read( unsigned char * buffer, unsigned long * size )
{
	unsigned long n = *size;
	SANE_Status status;
	
	cmd_init_usb_read();
	
	if ( CANON_fd < 0 ) {
		return -1; /* error */
	}
	status = cmt_libusb_bulk_read ( CANON_fd, buffer, &n );
		
	if ( status != SANE_STATUS_GOOD ) {
		return -1; /* error */
	}
	
	*size = n;
	
	if ( *size != n ) {
		return n; /* error */
	}
	return 0; /* no error */
}

static SANE_Status canon_network_open( SANE_String_Const devname, CANON_Scanner *s )
{
	SANE_Status status;
	CNNLHANDLE handle = NULL;
	
	if ( !s ) {
		return SANE_STATUS_INVAL;
	}
	if ( CANON_hnd == NULL ) {
		if ( ( status = cmt_network_open ((const char *)devname, &handle) ) != SANE_STATUS_GOOD ) {
			return status;
		}
		CANON_hnd = handle;
		return SANE_STATUS_GOOD;
	}
	else { /* already open. */
		return SANE_STATUS_IO_ERROR;
	}

}

static void canon_network_close( CANON_Scanner *s )
{
	if ( !s ) {
		return ;
	}
	if ( CANON_hnd == NULL ) {
		return ;
	}
	else {
		cmt_network_close( &CANON_hnd );
		CANON_hnd = NULL;
	}
}

static int canon_network_write( unsigned char * buffer, unsigned long size )
{
	unsigned long n = size;
	SANE_Status status;
	
	if ( CANON_hnd == NULL ) {
		return -1; /* error */
	}
	status = cmt_network_write ( CANON_hnd, buffer, &n );
	
	if ( status != SANE_STATUS_GOOD ) {
		return -1; /* error */
	}
	
	if ( size != n ) {
		return n; /* error */
	}
	return 0; /* no error */
}

static int canon_network_read( unsigned char * buffer, unsigned long * size )
{
	unsigned long n = *size;
	SANE_Status status;
	
	if ( CANON_hnd == NULL ) {
		return -1; /* error */
	}
	status = cmt_network_read ( CANON_hnd, buffer, &n );
		
	if ( status != SANE_STATUS_GOOD ) {
		return -1; /* error */
	}
	
	*size = n;
	
	if ( *size != n ) {
		return n; /* error */
	}
	return 0; /* no error */
}


/*-------------------------------------------------
	dispose_canon_dev()
-------------------------------------------------*/
static void dispose_canon_dev( CANON_Device *dev )
{
	CNMSLLDAPI *api = NULL;

	if ( dev ) {
		if ( dev->sane.name ) {
			free( (void *)dev->sane.name );
		}
		if ( dev->sane.model ) {
			free( (void *)dev->sane.model );
		}
		if ( dev->cnmslldapi ) {
			api = (CNMSLLDAPI *)dev->cnmslldapi;
			if ( api->handle ) {
				dlclose( api->handle );
			}
			free( api );
		}
		free( dev );
	}
}

/*-------------------------------------------------
	create_cnmslld_api()
-------------------------------------------------*/
static void* create_cnmslld_api( char *libname, SANE_Word api_ver )
{
	CNMSLLDAPI		*api = NULL;

	api = (CNMSLLDAPI *)malloc( sizeof( CNMSLLDAPI ) );
	if ( !api ) {
		goto _ERROR;
	}
	memset( api, 0, sizeof(*api) );
	
	DBGMSG("<attach> dlopen( \"%s\", RTLD_LAZY )\n", libname );
	if ( ( api->handle = dlopen( libname, RTLD_LAZY ) ) == NULL ) {
		DBGMSG("ERROR : dlopen( \"%s\", RTLD_LAZY ) -> %s\n", libname, dlerror() );
		goto _ERROR;
	}
	
	/* api_ver : 0 */
	api->p_canon_init_driver			= ( FPCANON_INIT_DRIVER				)dlsym( api->handle, "canon_init_driver" );
	api->p_canon_init_scanner			= ( FPCANON_INIT_SCANNER			)dlsym( api->handle, "canon_init_scanner" );
	api->p_canon_terminate_scanner		= ( FPCANON_TERMINATE_SCANNER		)dlsym( api->handle, "canon_terminate_scanner" );
	api->p_canon_set_parameter			= ( FPCANON_SET_PARAMETER			)dlsym( api->handle, "canon_set_parameter" );
	api->p_canon_start_scan				= ( FPCANON_START_SCAN				)dlsym( api->handle, "canon_start_scan" );
	api->p_canon_do_cancel				= ( FPCANON_DO_CANCEL				)dlsym( api->handle, "canon_do_cancel" );
	api->p_canon_read_scan				= ( FPCANON_READ_SCAN				)dlsym( api->handle, "canon_read_scan" );
	api->p_canon_get_ability_ver		= ( FPCANON_GET_ABILITY_VER			)dlsym( api->handle, "canon_get_ability_ver" );
	api->p_canon_get_ability			= ( FPCANON_GET_ABILITY				)dlsym( api->handle, "canon_get_ability" );
	api->p_canon_get_status				= ( FPCANON_GET_STATUS				)dlsym( api->handle, "canon_get_status" );
	api->p_canon_get_calibration_status	= ( FPCANON_GET_CALIBLATION_STATUS	)dlsym( api->handle, "canon_get_calibration_status" );

	if (	api->p_canon_init_driver			== NULL ||
			api->p_canon_init_scanner			== NULL ||
			api->p_canon_terminate_scanner		== NULL ||
			api->p_canon_set_parameter			== NULL ||
			api->p_canon_start_scan				== NULL ||
			api->p_canon_do_cancel				== NULL ||
			api->p_canon_read_scan				== NULL ||
			api->p_canon_get_ability			== NULL ||
			api->p_canon_get_ability_ver		== NULL ||
			api->p_canon_get_status				== NULL ||
			api->p_canon_get_calibration_status	== NULL ) {

		DBGMSG("<create_cnmslld_api> ERROR : dlsym 0\n");
		goto _ERROR;
	}
	
	/* api_ver : 1 */
	api->p_canon_get_parameters = NULL;
	if ( api_ver > 0 ) {
		api->p_canon_get_parameters	= ( FPCANON_GET_PARAMETERS )dlsym( api->handle, "canon_get_parameters" );
		if ( api->p_canon_get_parameters == NULL ) {
			DBGMSG("<create_cnmslld_api> ERROR : dlsym 1\n");
			goto _ERROR;
		}
	}
	
	/* api_ver : 2 */
	api->p_canon_end_scan = NULL;
	api->p_canon_set_tblpath = NULL;
	api->p_canon_set_parameter_ex = NULL;
	if ( api_ver > 1 ) {
		api->p_canon_end_scan			= ( FPCANON_END_SCAN			)dlsym( api->handle, "canon_end_scan" );
		api->p_canon_set_tblpath		= ( FPCANON_SET_TBLPATH			)dlsym( api->handle, "canon_set_tblpath" );
		api->p_canon_set_parameter_ex	= ( FPCANON_SET_PARAMETER_EX	)dlsym( api->handle, "canon_set_parameter_ex" );
		if (	api->p_canon_end_scan			== NULL ||
				api->p_canon_set_parameter_ex	== NULL ||
				api->p_canon_set_tblpath		== NULL ) {
			DBGMSG("<create_cnmslld_api> ERROR : dlsym 2\n");
			goto _ERROR;
		}
	}
	
	/* api_ver : 3 */
	api->p_canon_get_devicesettings = NULL;
	api->p_canon_set_devicesettings = NULL;
	if ( api_ver > 2 ) {
		api->p_canon_get_devicesettings		= ( FPCANON_GET_DEVICESETTINGS	)dlsym( api->handle, "canon_get_devicesettings" );
		api->p_canon_set_devicesettings		= ( FPCANON_SET_DEVICESETTINGS	)dlsym( api->handle, "canon_set_devicesettings" );
		if (	api->p_canon_get_devicesettings	== NULL ||
				api->p_canon_set_devicesettings	== NULL ) {
			DBGMSG("<create_cnmslld_api> ERROR : dlsym 3\n");
			goto _ERROR;
		}
	}
	
	return (void *)api; /* no error */
	
_ERROR:
	if ( api ) {
		if ( api->handle ) {
			dlclose( api->handle );
		}
		free( api );
	}
	return NULL;
}


/*-------------------------------------------------
	get_max_len()
-------------------------------------------------*/
static size_t get_max_len(const SANE_String_Const strs[])
{
	size_t size, max = 0;
	int i;

	for( i = 0; strs[i] != NULL; i++ ) {
		size = strlen(strs[i]) + 1;
		if ( size > max ) {
			max = size;
		}
	}

	return max;
}

/*-------------------------------------------------
	attach()
-------------------------------------------------*/
static SANE_Status attach(
	const char *name,
	const char *model )
{
	CANON_Device	*dev = NULL, *devloop = NULL;
	CANON_Scanner	*s = NULL;
	SANE_Status		status;
	SANE_Word		vendor;
	SANE_Word		product;
	SANE_Word		speed = 2;
	CANON_DEVICE_INFO	canon_dev_info;
	CNMSLLDAPI		*api = NULL;
	char			*libname = NULL;
	SANE_Word		api_ver;
	SANE_Word		usb_opend = 0;
	const char		*path = DEFTOSTR( CNMSLIBPATH );	/* tblpath : Ver.1.20 */
	int				flag_usb = 0, i = 0;

	if( name == NULL ){
		flag_usb = 1;
	}
	else{
		if( strncmp( name, "libusb:", 7 ) >= 0 ){
			flag_usb = 1;
		}
	}
	
	for (dev = first_dev; dev; dev = dev->next) {
		if ( strcmp(dev->sane.name, name) == 0 ) {
			return (SANE_STATUS_GOOD);		/* already exists. */
		}
	}
	
	dev = malloc( sizeof( CANON_Device ) );
	if ( !dev ) {
		status = SANE_STATUS_NO_MEM;
		goto _ERROR;
	}
	memset( dev, 0, sizeof(*dev) );
	
	s = malloc ( sizeof( CANON_Scanner ) );
	if ( !s ) {
		status = SANE_STATUS_NO_MEM;
		goto _ERROR;
	}
	
	memset( s, 0, sizeof(*s) );
	
	if(flag_usb){
		/* open succeed -> append to list */
		if ( ( status = canon_usb_open(name, s) ) != SANE_STATUS_GOOD )
		{
			DBGMSG("<attach> name = \"%s\" not found.\n", name);
			goto _ERROR;
		}
		usb_opend = 1;

		/* get vendor & product id */
		if ( ( status = cmt_libusb_get_id( name, &vendor, &product, &speed ) ) != SANE_STATUS_GOOD )
		{
			cmd_no_goto_error;
		}
	}
	else{
		for(i = 0;canon_mfp_product_ids[i].product != -1; i++) {
			if( strncmp( model, canon_mfp_product_ids[i].model, strlen(model) ) == 0 ){
				product = canon_mfp_product_ids[i].product;
				speed = -1;
				break;
			}
		}
	}

	/*--- SANE_Device ---*/
	cmd_set_usb( &name, canon_mfp_product_ids, &product, &speed );
	dev->sane.name =	strdup(name);
	dev->sane.vendor =	"CANON";
	dev->sane.model =	NULL;
	dev->sane.type =	"flatbed scanner";
	
	{
		/* search product id */
		for(i = 0;canon_mfp_product_ids[i].product != -1; i++) {
			if( product == canon_mfp_product_ids[i].product ) {
				dev->sane.model = strdup( canon_mfp_product_ids[i].model );
				libname = strdup( canon_mfp_product_ids[i].libname );
				api_ver = canon_mfp_product_ids[i].api_ver;
				break;
			}
		}
		if ( canon_mfp_product_ids[i].product == -1 ) {
			status = SANE_STATUS_INVAL;
			goto _ERROR;
		}
	}
	
	dev->cnmslldapi = create_cnmslld_api( libname, api_ver );
	if ( libname ) {
		free( libname );
	}
	
	if ( !dev->cnmslldapi ) {
		status = SANE_STATUS_INVAL;
		goto _ERROR;
	}
	api = (CNMSLLDAPI *)dev->cnmslldapi;
	
	memset( &canon_dev_info, 0, sizeof (canon_dev_info) );
	
	if(flag_usb){
		canon_usb_close( s );
		usb_opend = 0;
	}
	
	if ( s ) {
		free( s );
	}
	
	/*
		initialize CANON_Info struct.
	*/
	dev->info.product_id = product;
	dev->info.speed = speed;
	
	num_devices++;

	dev->api_ver = api_ver;	/* add Ver.1.20 */
	if( first_dev == NULL ){
		first_dev = dev;
	}
	else{
		for( devloop = first_dev ; devloop ; devloop = devloop->next ){
			if( devloop->next == NULL ){
				devloop->next = dev;
				break;
			}
		}
	}
	
	return status;

_ERROR:
	dispose_canon_dev( dev );
	
	if(flag_usb){
		if ( usb_opend ) {
			canon_usb_close( s );
		}
		if ( s ) {
			free( s );
		}
	}
	return status;
}

/*-------------------------------------------------
	init_opts()
-------------------------------------------------*/
static SANE_Status init_opts(CANON_Scanner * s)
{
	int i;
	
	memset (s->opt, 0, sizeof (s->opt));
	memset (s->val, 0, sizeof (s->val));
	
	for (i = 0; i < NUM_OPTIONS; ++i) {
		s->opt[i].size = sizeof (SANE_Word);
		s->opt[i].cap = SANE_CAP_SOFT_SELECT | SANE_CAP_SOFT_DETECT;
	}
	
	/*--- Number of options  ---*/
	s->opt[OPT_NUM_OPTS].title = CANON_TITLE_NUM_OPTS;		/* "" */
	s->opt[OPT_NUM_OPTS].desc = CANON_DESC_NULL;		/* "Number of options" */
	s->opt[OPT_NUM_OPTS].cap = SANE_CAP_SOFT_DETECT;
	s->val[OPT_NUM_OPTS].w = NUM_OPTIONS;
	
	/*--- preview ---*/
	s->opt[OPT_PREVIEW].name  = CANON_NAME_PREVIEW;
	s->opt[OPT_PREVIEW].title = CANON_TITLE_PREVIEW;
	s->opt[OPT_PREVIEW].desc  = CANON_DESC_NULL;
	s->opt[OPT_PREVIEW].type  = SANE_TYPE_BOOL;
	s->opt[OPT_PREVIEW].cap   = SANE_CAP_SOFT_DETECT | SANE_CAP_SOFT_SELECT;
	s->val[OPT_PREVIEW].w     = SANE_FALSE;
	
	/*--- scan mode ---*/
	s->opt[OPT_MODE].name = CANON_NAME_SCAN_MODE;
	s->opt[OPT_MODE].title = CANON_TITLE_SCAN_MODE;
	s->opt[OPT_MODE].desc = CANON_DESC_NULL;
	s->opt[OPT_MODE].type = SANE_TYPE_STRING;
	s->opt[OPT_MODE].size = get_max_len(mode_list);	/* "Gray","Color" */
	s->opt[OPT_MODE].constraint_type = SANE_CONSTRAINT_STRING_LIST;
	s->opt[OPT_MODE].constraint.string_list = mode_list;
	s->val[OPT_MODE].s = strdup (mode_list[1]);	/* default : "Color" */
	
	
	/*--- Resolution Group ---*/
	s->opt[OPT_RESOLUTION_GROUP].title = CANON_TITLE_RES;
	s->opt[OPT_RESOLUTION_GROUP].desc = CANON_DESC_NULL;
	s->opt[OPT_RESOLUTION_GROUP].type = SANE_TYPE_GROUP;
	s->opt[OPT_RESOLUTION_GROUP].cap = 0;;
	s->opt[OPT_RESOLUTION_GROUP].constraint_type = SANE_CONSTRAINT_NONE;
	
	/*--- bind resolution ---*/
	s->opt[OPT_RESOLUTION_BIND].name  = CANON_NAME_RES_BIND;
	s->opt[OPT_RESOLUTION_BIND].title = CANON_TITLE_RES_BIND;
	s->opt[OPT_RESOLUTION_BIND].desc  = CANON_DESC_NULL;
	s->opt[OPT_RESOLUTION_BIND].type  = SANE_TYPE_BOOL;
	s->val[OPT_RESOLUTION_BIND].w     = SANE_TRUE;
	
	/*--- x-resolution ---*/
	s->opt[OPT_X_RESOLUTION].name  = CANON_NAME_RES;
	s->opt[OPT_X_RESOLUTION].title = CANON_TITLE_RES;
	s->opt[OPT_X_RESOLUTION].desc  = CANON_DESC_NULL;
	s->opt[OPT_X_RESOLUTION].type  = SANE_TYPE_INT;
	s->opt[OPT_X_RESOLUTION].unit  = SANE_UNIT_DPI;
	s->opt[OPT_X_RESOLUTION].constraint_type  = SANE_CONSTRAINT_WORD_LIST;
	s->opt[OPT_X_RESOLUTION].constraint.word_list = xres_list;
	s->val[OPT_X_RESOLUTION].w = s->hw->info.xres_default;
	
	/*--- y-resolution ---*/
	s->opt[OPT_Y_RESOLUTION].name  = CANON_NAME_RES_Y;
	s->opt[OPT_Y_RESOLUTION].title = CANON_TITLE_RES_Y;
	s->opt[OPT_Y_RESOLUTION].desc  = CANON_DESC_NULL;
	s->opt[OPT_Y_RESOLUTION].type  = SANE_TYPE_INT;
	s->opt[OPT_Y_RESOLUTION].unit  = SANE_UNIT_DPI;
	s->opt[OPT_Y_RESOLUTION].constraint_type  = SANE_CONSTRAINT_WORD_LIST;
	s->opt[OPT_Y_RESOLUTION].constraint.word_list = yres_list;
	s->val[OPT_Y_RESOLUTION].w = s->hw->info.yres_default;
	s->opt[OPT_Y_RESOLUTION].cap  |= SANE_CAP_INACTIVE;		/* bind = true -> inactive */
	

	/*--- Margins Group ---*/
	s->opt[OPT_MARGINS_GROUP].title = CANON_TITLE_MARGINS;
	s->opt[OPT_MARGINS_GROUP].desc = CANON_DESC_NULL;
	s->opt[OPT_MARGINS_GROUP].type = SANE_TYPE_GROUP;
	s->opt[OPT_MARGINS_GROUP].cap = SANE_CAP_ADVANCED;
	s->opt[OPT_MARGINS_GROUP].constraint_type = SANE_CONSTRAINT_NONE;
	
	/*--- top-left x ---*/
	s->opt[OPT_TL_X].name = CANON_NAME_MARGINS_TL_X;
	s->opt[OPT_TL_X].title = CANON_TITLE_MARGINS_TL_X;
	s->opt[OPT_TL_X].desc = CANON_DESC_NULL;
	s->opt[OPT_TL_X].type = SANE_TYPE_FIXED;
	s->opt[OPT_TL_X].unit = SANE_UNIT_MM;
	s->opt[OPT_TL_X].constraint_type = SANE_CONSTRAINT_RANGE;
	s->opt[OPT_TL_X].constraint.range = &s->hw->info.x_range;
	s->val[OPT_TL_X].w = 0;
	
	/*--- top-left y ---*/
	s->opt[OPT_TL_Y].name = CANON_NAME_MARGINS_TL_Y;
	s->opt[OPT_TL_Y].title = CANON_TITLE_MARGINS_TL_Y;
	s->opt[OPT_TL_Y].desc = CANON_DESC_NULL;
	s->opt[OPT_TL_Y].type = SANE_TYPE_FIXED;
	s->opt[OPT_TL_Y].unit = SANE_UNIT_MM;
	s->opt[OPT_TL_Y].constraint_type = SANE_CONSTRAINT_RANGE;
	s->opt[OPT_TL_Y].constraint.range = &s->hw->info.y_range;
	s->val[OPT_TL_Y].w = 0;
	
	/*--- bottom-right x ---*/
	s->opt[OPT_BR_X].name = CANON_NAME_MARGINS_BR_X;
	s->opt[OPT_BR_X].title = CANON_TITLE_MARGINS_BR_X;
	s->opt[OPT_BR_X].desc = CANON_DESC_NULL;
	s->opt[OPT_BR_X].type = SANE_TYPE_FIXED;
	s->opt[OPT_BR_X].unit = SANE_UNIT_MM;
	s->opt[OPT_BR_X].constraint_type = SANE_CONSTRAINT_RANGE;
	s->opt[OPT_BR_X].constraint.range = &s->hw->info.x_range;
	s->val[OPT_BR_X].w = s->hw->info.x_range.max;
	
	/*--- bottom-right y ---*/
	s->opt[OPT_BR_Y].name = CANON_NAME_MARGINS_BR_Y;
	s->opt[OPT_BR_Y].title = CANON_TITLE_MARGINS_BR_Y;
	s->opt[OPT_BR_Y].desc = CANON_DESC_NULL;
	s->opt[OPT_BR_Y].type = SANE_TYPE_FIXED;
	s->opt[OPT_BR_Y].unit = SANE_UNIT_MM;
	s->opt[OPT_BR_Y].constraint_type = SANE_CONSTRAINT_RANGE;
	s->opt[OPT_BR_Y].constraint.range = &s->hw->info.y_range;
	s->val[OPT_BR_Y].w = s->hw->info.y_range.max;
	
	
	/*--- Extend Group ---*/
	s->opt[OPT_EXTEND_GROUP].title = CANON_TITLE_EXTEND;
	s->opt[OPT_EXTEND_GROUP].desc = CANON_DESC_NULL;
	s->opt[OPT_EXTEND_GROUP].type = SANE_TYPE_GROUP;
	s->opt[OPT_EXTEND_GROUP].cap |= SANE_CAP_INACTIVE;
	s->opt[OPT_EXTEND_GROUP].constraint_type = SANE_CONSTRAINT_NONE;
	
	/*--- get scanner status ---*/
	s->opt[OPT_EX_STATUS].name = CANON_NAME_EX_GET_STATUS;
	s->opt[OPT_EX_STATUS].title = CANON_TITLE_EX_GET_STATUS;
	s->opt[OPT_EX_STATUS].desc = CANON_DESC_NULL;
	s->opt[OPT_EX_STATUS].type = SANE_TYPE_INT;
	s->opt[OPT_EX_STATUS].cap &= ~SANE_CAP_SOFT_SELECT;
	s->opt[OPT_EX_STATUS].cap |= SANE_CAP_INACTIVE;
	s->opt[OPT_EX_STATUS].unit = SANE_UNIT_NONE;
	s->opt[OPT_EX_STATUS].constraint_type = SANE_CONSTRAINT_NONE;
	
	/*--- get calibration status ---*/
	s->opt[OPT_EX_CALIBRATION_STATUS].name = CANON_NAME_EX_GET_CAL_STATUS;
	s->opt[OPT_EX_CALIBRATION_STATUS].title = CANON_TITLE_EX_GET_CAL_STATUS;
	s->opt[OPT_EX_CALIBRATION_STATUS].desc = CANON_DESC_NULL;
	s->opt[OPT_EX_CALIBRATION_STATUS].type = SANE_TYPE_INT;
	s->opt[OPT_EX_CALIBRATION_STATUS].cap &= ~SANE_CAP_SOFT_SELECT;
	s->opt[OPT_EX_CALIBRATION_STATUS].cap |= SANE_CAP_INACTIVE;
	s->opt[OPT_EX_CALIBRATION_STATUS].unit = SANE_UNIT_NONE;
	s->opt[OPT_EX_CALIBRATION_STATUS].constraint_type = SANE_CONSTRAINT_NONE;
	
	/*--- get ability struct version ---*/
	s->opt[OPT_EX_ABILITY_VER].name = CANON_NAME_EX_GET_ABILITY_VER;
	s->opt[OPT_EX_ABILITY_VER].title = CANON_TITLE_EX_GET_ABILITY_VER;
	s->opt[OPT_EX_ABILITY_VER].desc = CANON_DESC_NULL;
	s->opt[OPT_EX_ABILITY_VER].type = SANE_TYPE_INT;
	s->opt[OPT_EX_ABILITY_VER].cap &= ~SANE_CAP_SOFT_SELECT;
	s->opt[OPT_EX_ABILITY_VER].cap |= SANE_CAP_INACTIVE;
	s->opt[OPT_EX_ABILITY_VER].unit = SANE_UNIT_NONE;
	s->opt[OPT_EX_ABILITY_VER].constraint_type = SANE_CONSTRAINT_NONE;
	
	/*--- get scanner ability ---*/
	s->opt[OPT_EX_ABILITY].name = CANON_NAME_EX_GET_ABILITY;
	s->opt[OPT_EX_ABILITY].title = CANON_TITLE_EX_GET_ABILITY;
	s->opt[OPT_EX_ABILITY].desc = CANON_DESC_NULL;
	s->opt[OPT_EX_ABILITY].type = SANE_TYPE_INT;
	s->opt[OPT_EX_ABILITY].cap &= ~SANE_CAP_SOFT_SELECT;
	s->opt[OPT_EX_ABILITY].cap |= SANE_CAP_INACTIVE;
	s->opt[OPT_EX_ABILITY].unit = SANE_UNIT_NONE;
	s->opt[OPT_EX_ABILITY].size = 0;						/* -> sizeof(CANON_ABILITY_INFO) */
	s->opt[OPT_EX_ABILITY].constraint_type = SANE_CONSTRAINT_NONE;
	
	/*--- use unit pixel ---*/
	s->opt[OPT_EX_USE_AREA_PIXEL].name  = CANON_NAME_EX_USE_AREA_PIXEL;
	s->opt[OPT_EX_USE_AREA_PIXEL].title = CANON_TITLE_EX_USE_AREA_PIXEL;
	s->opt[OPT_EX_USE_AREA_PIXEL].desc  = CANON_DESC_NULL;
	s->opt[OPT_EX_USE_AREA_PIXEL].type  = SANE_TYPE_BOOL;
	s->val[OPT_EX_USE_AREA_PIXEL].w     = SANE_FALSE;		/* FALSE -> use OPT_TL_X/Y,BR_X/Y (mm) */
	s->opt[OPT_EX_USE_AREA_PIXEL].cap  |= SANE_CAP_INACTIVE;
	
	/*--- top-left x pixel ---*/
	s->opt[OPT_EX_TL_X_PIXEL].name = CANON_NAME_EX_TL_X_PIXEL;
	s->opt[OPT_EX_TL_X_PIXEL].title = CANON_TITLE_EX_TL_X_PIXEL;
	s->opt[OPT_EX_TL_X_PIXEL].desc = CANON_DESC_NULL;
	s->opt[OPT_EX_TL_X_PIXEL].type = SANE_TYPE_INT;
	s->opt[OPT_EX_TL_X_PIXEL].unit = SANE_UNIT_PIXEL;
	s->opt[OPT_EX_TL_X_PIXEL].constraint_type = SANE_CONSTRAINT_NONE;
	s->val[OPT_EX_TL_X_PIXEL].w = 0;
	s->opt[OPT_EX_TL_X_PIXEL].cap |= SANE_CAP_INACTIVE;
	
	/*--- top-left y pixel ---*/
	s->opt[OPT_EX_TL_Y_PIXEL].name = CANON_NAME_EX_TL_Y_PIXEL;
	s->opt[OPT_EX_TL_Y_PIXEL].title = CANON_TITLE_EX_TL_Y_PIXEL;
	s->opt[OPT_EX_TL_Y_PIXEL].desc = CANON_DESC_NULL;
	s->opt[OPT_EX_TL_Y_PIXEL].type = SANE_TYPE_INT;
	s->opt[OPT_EX_TL_Y_PIXEL].unit = SANE_UNIT_PIXEL;
	s->opt[OPT_EX_TL_Y_PIXEL].constraint_type = SANE_CONSTRAINT_NONE;
	s->val[OPT_EX_TL_Y_PIXEL].w = 0;
	s->opt[OPT_EX_TL_Y_PIXEL].cap |= SANE_CAP_INACTIVE;
	
	/*--- bottom-right x pixel ---*/
	s->opt[OPT_EX_BR_X_PIXEL].name = CANON_NAME_EX_BR_X_PIXEL;
	s->opt[OPT_EX_BR_X_PIXEL].title = CANON_TITLE_EX_BR_X_PIXEL;
	s->opt[OPT_EX_BR_X_PIXEL].desc = CANON_DESC_NULL;
	s->opt[OPT_EX_BR_X_PIXEL].type = SANE_TYPE_INT;
	s->opt[OPT_EX_BR_X_PIXEL].unit = SANE_UNIT_PIXEL;
	s->opt[OPT_EX_BR_X_PIXEL].constraint_type = SANE_CONSTRAINT_NONE;
	s->val[OPT_EX_BR_X_PIXEL].w = 0;
	s->opt[OPT_EX_BR_X_PIXEL].cap |= SANE_CAP_INACTIVE;
	
	/*--- bottom-right y pixel ---*/
	s->opt[OPT_EX_BR_Y_PIXEL].name = CANON_NAME_EX_BR_Y_PIXEL;
	s->opt[OPT_EX_BR_Y_PIXEL].title = CANON_TITLE_EX_BR_Y_PIXEL;
	s->opt[OPT_EX_BR_Y_PIXEL].desc = CANON_DESC_NULL;
	s->opt[OPT_EX_BR_Y_PIXEL].type = SANE_TYPE_INT;
	s->opt[OPT_EX_BR_Y_PIXEL].unit = SANE_UNIT_PIXEL;
	s->opt[OPT_EX_BR_Y_PIXEL].constraint_type = SANE_CONSTRAINT_NONE;
	s->val[OPT_EX_BR_Y_PIXEL].w = 0;
	s->opt[OPT_EX_BR_Y_PIXEL].cap |= SANE_CAP_INACTIVE;
	
	/*--- set gamma ---*/
	for( i = 0; i < 65536; i++ ) s->gamma_table[i] = i;

	s->opt[OPT_EX_GAMMA].name = CANON_NAME_EX_GAMMA;
	s->opt[OPT_EX_GAMMA].title = CANON_TITLE_EX_GAMMA;
	s->opt[OPT_EX_GAMMA].desc = CANON_DESC_NULL;
	s->opt[OPT_EX_GAMMA].type = SANE_TYPE_INT;
	s->opt[OPT_EX_GAMMA].unit = SANE_UNIT_NONE;
	s->opt[OPT_EX_GAMMA].size = sizeof( s->gamma_table );
	s->opt[OPT_EX_GAMMA].constraint_type = SANE_CONSTRAINT_NONE;
	s->val[OPT_EX_GAMMA].wa = (SANE_Word *)s->gamma_table;
	s->opt[OPT_EX_GAMMA].cap |= SANE_CAP_INACTIVE;
	
	/* Ver.1.20 */
	/*--- Calibration setting ---*/
	s->opt[OPT_EX_EVERY_CALIBRATION].name  = CANON_NAME_EX_EVERY_CALIBRATION;
	s->opt[OPT_EX_EVERY_CALIBRATION].title = CANON_TITLE_EX_EVERY_CALIBRATION;
	s->opt[OPT_EX_EVERY_CALIBRATION].desc  = CANON_DESC_NULL;
	s->opt[OPT_EX_EVERY_CALIBRATION].type  = SANE_TYPE_BOOL;
	s->val[OPT_EX_EVERY_CALIBRATION].w     = SANE_FALSE;		/* FALSE -> Every Calibration OFF */
	s->opt[OPT_EX_EVERY_CALIBRATION].cap  |= SANE_CAP_INACTIVE;
	
	/*--- Silent scan setting ---*/
	s->opt[OPT_EX_SILENT].name  = CANON_NAME_EX_SILENT;
	s->opt[OPT_EX_SILENT].title = CANON_TITLE_EX_SILENT;
	s->opt[OPT_EX_SILENT].desc  = CANON_DESC_NULL;
	s->opt[OPT_EX_SILENT].type  = SANE_TYPE_BOOL;
	s->val[OPT_EX_SILENT].w     = SANE_FALSE;		/* FALSE -> Silent scan OFF */
	s->opt[OPT_EX_SILENT].cap  |= SANE_CAP_INACTIVE;
	
	/* Ver.1.30 */
	/*--- Scan Method ---*/
	s->opt[OPT_EX_SCAN_METHOD].name  = CANON_NAME_EX_SCAN_METHOD;
	s->opt[OPT_EX_SCAN_METHOD].title = CANON_TITLE_EX_SCAN_METHOD;
	s->opt[OPT_EX_SCAN_METHOD].desc  = CANON_DESC_NULL;
	s->opt[OPT_EX_SCAN_METHOD].type  = SANE_TYPE_INT;
	s->opt[OPT_EX_SCAN_METHOD].unit  = SANE_UNIT_NONE;
	s->opt[OPT_EX_SCAN_METHOD].constraint_type = SANE_CONSTRAINT_NONE;
	s->val[OPT_EX_SCAN_METHOD].w     = 0;		/* 0 : flatbed */
	s->opt[OPT_EX_SCAN_METHOD].cap  |= SANE_CAP_INACTIVE;
	
	/* Ver.1.90 */
	/*--- get,set device settings ---*/
	s->opt[OPT_EX_DEVICE_SETTINGS].name = CANON_NAME_EX_DEVICE_SETTINGS;
	s->opt[OPT_EX_DEVICE_SETTINGS].title = CANON_TITLE_EX_DEVICE_SETTINGS;
	s->opt[OPT_EX_DEVICE_SETTINGS].desc = CANON_DESC_NULL;
	s->opt[OPT_EX_DEVICE_SETTINGS].type = SANE_TYPE_INT;
	s->opt[OPT_EX_DEVICE_SETTINGS].cap |= SANE_CAP_INACTIVE;
	s->opt[OPT_EX_DEVICE_SETTINGS].unit = SANE_UNIT_NONE;
	s->opt[OPT_EX_DEVICE_SETTINGS].size = 0;						/* -> sizeof(CANON_DEVICE_SETTINGS) */
	s->opt[OPT_EX_DEVICE_SETTINGS].constraint_type = SANE_CONSTRAINT_NONE;
	
	return SANE_STATUS_GOOD;
}

/*-------------------------------------------------
	check_ex_opts()
-------------------------------------------------*/
static SANE_Int check_ex_opts( SANE_Int opt )
{
	SANE_Int ret = 0;
	
	switch( opt ) {
		case OPT_EX_USE_AREA_PIXEL:
		case OPT_EX_TL_X_PIXEL:
		case OPT_EX_TL_Y_PIXEL:
		case OPT_EX_BR_X_PIXEL:
		case OPT_EX_BR_Y_PIXEL:
		case OPT_EX_STATUS:
		case OPT_EX_CALIBRATION_STATUS:
		case OPT_EX_ABILITY_VER:
		case OPT_EX_ABILITY:
		case OPT_EX_GAMMA:
		/* Ver.1.20 */
		case OPT_EX_EVERY_CALIBRATION:
		case OPT_EX_SILENT:
		/* Ver.1.30 */
		case OPT_EX_SCAN_METHOD:
		/* Ver.1.90 */
		case OPT_EX_DEVICE_SETTINGS:
			ret = 1;
			break;
		default :
			break;
	}
	return ret;
}

/*-------------------------------------------------
	check_opts_val()
-------------------------------------------------*/
static SANE_Status check_opts_val(const SANE_Option_Descriptor *opt, void *v )
{
	const SANE_Range *range;
	const SANE_Word *word_list;
	SANE_Word val;
	SANE_Word num;
	int i, found = 0;
	
	const SANE_String_Const *string_list;
	SANE_String str;
	int len, len2;
	
	switch ( opt->constraint_type ) {
		case SANE_CONSTRAINT_RANGE:
			val = *( SANE_Word * ) v;
			range = opt->constraint.range;
			
			if ( range ) {
				if ( val > range->max ) {
					*( SANE_Word * ) v = range->max;
				} else if ( val < range->min ) {
					*( SANE_Word * ) v = range->min;
				}
			}
			else {
				return SANE_STATUS_INVAL;
			}
			
			break;
			
		case SANE_CONSTRAINT_WORD_LIST:
			val = *( SANE_Word * ) v;
			word_list = opt->constraint.word_list;
			
			if ( word_list ) {
				num = (int)word_list[0];
				if ( !num ) {
					return SANE_STATUS_INVAL;
				}
				
				for ( i = 1; i < num + 1; i++ ) {
					if ( word_list[i] == val ) {
						found = 1;
						break;
					}
				}
				/* set first value */
				if ( !found ) {
					*( SANE_Word * ) v = word_list[1];
				}
			}
			else {
				return SANE_STATUS_INVAL;
			}
			
			break;
		
		case SANE_CONSTRAINT_STRING_LIST:
			str = ( SANE_String ) v;
			len = strlen( str );
			string_list = opt->constraint.string_list;
			
			if ( string_list ) {
				for ( i = 0; *string_list[i] ; i++ ) {
					len2 = strlen( string_list[i] );
					
					if( len != len2 ) {
						continue;
					}
					if ( !strncmp( str, string_list[i], len ) ) {
						found = 1;
						break;
					}
				}
				if ( !found ) {
					return SANE_STATUS_INVAL;
				}
			}
			else {
				return SANE_STATUS_INVAL;
			}
			
			break;
		
		default:
			break;
	}

	return SANE_STATUS_GOOD;
}


/*---------------------------------------------------------------------------------------*/
/*    SANE API ---->                                                                     */
/*---------------------------------------------------------------------------------------*/

/*-------------------------------------------------
	sane_init()
-------------------------------------------------*/
#ifdef __USE_LIBSANE__
SANE_Status sane_canon_mfp_init(
#else
SANE_Status sane_init(
#endif
	SANE_Int * version_code,
	SANE_Auth_Callback authorize)
{
	FILE *fp;
	SANE_Status status = SANE_STATUS_GOOD;
	int devnum = 0;

	authorize = authorize;

	if ( version_code ) {
		*version_code = SANE_VERSION_CODE (V_MAJOR, V_MINOR, 0);
	}
	/*
		initialize libUSB
	*/
	cmt_libusb_init();
	
	/*
		initialize Network
	*/
	cmt_network_init( canon_get_network_installed() );
	
	/*--- read Configuration file. ---*/
	fp = cmt_conf_file_open( CANON_CONFIG_FILE );
	if ( fp ) {
		char line[PATH_MAX];
		int len;
		char *dev;
		int index;
		
		/* Set USB/Network device list */
		while ( ( len = cmt_conf_file_read_line (line, sizeof (line), fp) ) >= 0 ) {
			int idProduct;
			index = 0;

			idProduct = cmt_get_device_id( line, len );	/* read product id only. */
			/* USB search */
			if ( idProduct > 0 ) {
				while( ( dev = cmt_find_device( idProduct, &index ) ) != NULL ) {
					attach( (const char *)dev, NULL );
					index++; /* find next device */
					devnum++;
				}
			}
			/* Network search */
			else if( idProduct == 0 ){
				char *model = NULL;
				while( ( dev = cmt_find_device_net( line, len, &index, &model ) ) != NULL ){
					attach( (const char *)dev, model );
					index++;
					devnum++;
				}
			}
		}
		
		fclose ( fp );
	}
	cmd_exit_sane_init( devnum );

	return status;
}

/*-------------------------------------------------
	sane_exit()
-------------------------------------------------*/
#ifdef __USE_LIBSANE__
void sane_canon_mfp_exit(void)
#else
void sane_exit(void)
#endif
{
	CANON_Device *dev, *next;

	for (dev = first_dev; dev; dev = next) {
		next = dev->next;
		dispose_canon_dev( dev );
	}
	first_dev = NULL;
	
	if( devlist ){
		free( devlist );
	}
	devlist = NULL;
	
	num_devices = 0;
	cmt_libusb_exit();
	cmt_network_exit();

}

/*-------------------------------------------------
	sane_get_devices()
-------------------------------------------------*/
#ifdef __USE_LIBSANE__
SANE_Status sane_canon_mfp_get_devices(
#else
SANE_Status sane_get_devices(
#endif
	const SANE_Device ***device_list,
	SANE_Bool local_only)
{
	CANON_Device *dev;
	const SANE_Device **devlisttemp;

	local_only = local_only;
	
	if( devlist ){
		free( devlist );
	}
	
	devlist = malloc( ( num_devices + 1 ) * sizeof( SANE_Device * ) );
	if( !devlist ) {
		return (SANE_STATUS_NO_MEM);
	}
	for( dev = first_dev, devlisttemp = devlist; dev; dev = dev->next ) {
		*devlisttemp++ = &dev->sane;
	}
	
	*devlisttemp = NULL;
	*device_list = devlist;

	return SANE_STATUS_GOOD;
}

/*-------------------------------------------------
	sane_open()
-------------------------------------------------*/
#ifdef __USE_LIBSANE__
SANE_Status sane_canon_mfp_open(
#else
SANE_Status sane_open(
#endif
	SANE_String_Const name,		/* libusb:00X:00Y */
	SANE_Handle *h )
{
	SANE_Status status;
	CANON_Device *dev;
	CANON_Scanner *s;
	CNMSLLDAPI *api = NULL;
	SANE_Word	api_ver ;	/* Ver.1.20 */
	const char *path = DEFTOSTR( CNMSLIBPATH );	/* tblpath : Ver.1.20 */
	int flag_usb = 0, i;
	CANON_DEVICE_INFO	canon_dev_info;
	
	if ( opened_handle ) {
		DBGMSG("ERROR : Another CANON MFP Deviece has opened already.\n");
		return (SANE_STATUS_INVAL);
	}
	
	if ( !name ) {
		return (SANE_STATUS_INVAL);
	}
	
	if( strncmp( name, "libusb:", 7 ) >= 0 ){
		flag_usb = 1;
	}
	if ( name[0] != '\0' ) {
		for (dev = first_dev; dev; dev = dev->next) {
			if (strcmp (dev->sane.name, name) == 0) {
				break;
			}
		}
		
		if (!dev) {
			return (SANE_STATUS_INVAL);
		}
	}
	else {
		dev = first_dev;
	}

	s = malloc (sizeof (*s));
	if ( !s ) {
		return SANE_STATUS_NO_MEM;
	}
	memset( s, 0, sizeof(*s) );
	memset( &canon_dev_info, 0, sizeof (canon_dev_info) );
	
	s->fd = -1;
	s->hw = dev;
	
	api = (CNMSLLDAPI *)dev->cnmslldapi;
	
	if( flag_usb ){
		if ( ( status = canon_usb_open (name, s) ) != SANE_STATUS_GOOD ) {
			return status;
		}
	}
	else{
		if( ( status = canon_network_open ( name, s ) ) != SANE_STATUS_GOOD ){
			return status;
		}
	}
	
	status = SANE_STATUS_INVAL;
	cmt_network_mutex_lock();
	
	/* set func-pointer (read, write) */
	if(flag_usb){
		if ( api->p_canon_init_driver( canon_usb_read, canon_usb_write ) < 0 ) {
			DBGMSG("<attach> ERROR : p_canon_init_driver() \n");
			goto EXIT_CMT_NETWORK_MUTEX_UNLOCK;
		}
	}
	else{
		if ( api->p_canon_init_driver( canon_network_read, canon_network_write ) < 0 ) {
			DBGMSG("<attach> ERROR : p_canon_init_driver() \n");
			goto EXIT_CMT_NETWORK_MUTEX_UNLOCK;
		}
	}
	
	
	/* set tblpath : Ver.1.20 */
	api_ver = s->hw->api_ver ;
	if ( api_ver > 1 ) {
		DBGMSG("<sane_open> p_canon_set_tblpath() path = %s\n", path );
		if ( api->p_canon_set_tblpath( path ) < 0 ) {
			DBGMSG("ERROR : p_canon_set_tblpath() path = %s\n", path );
			goto EXIT_CMT_NETWORK_MUTEX_UNLOCK;
		}
	}
	
	/* set product id. */
	DBGMSG("<sane_open> p_canon_init_scanner() product = %X\n", dev->info.product_id);
	if ( api->p_canon_init_scanner( dev->info.product_id, dev->info.speed, &canon_dev_info ) < 0 ) {
		DBGMSG("ERROR : p_canon_init_scanner() product = %d\n", dev->info.product_id);
		goto EXIT_CMT_NETWORK_MUTEX_UNLOCK;
	}
	
	/* copies list of resolution. */
	memcpy( xres_list, canon_dev_info.xres_list, sizeof( xres_list ) );
	memcpy( yres_list, canon_dev_info.yres_list, sizeof( yres_list ) );
	
	dev->info.mud = canon_dev_info.mud;	/* Measurement Unit Divisor */
	
	/* X resolution */
	dev->info.xres_default = canon_dev_info.xres_default;
	dev->info.xres_range.max = canon_dev_info.xres_max;
	dev->info.xres_range.min = canon_dev_info.xres_min;
	dev->info.xres_range.quant = 0;
	
	/* Y resolution */
	dev->info.yres_default = canon_dev_info.yres_default;
	dev->info.yres_range.max = canon_dev_info.yres_max;
	dev->info.yres_range.min = canon_dev_info.yres_min;
	dev->info.yres_range.quant = 0;
	
	/* set max/min pixels */
	dev->info.x_range_pixel.min = 0;
	dev->info.x_range_pixel.max = canon_dev_info.max_width - 1;
	dev->info.x_range_pixel.quant = 0;
	
	dev->info.y_range_pixel.min = 0;
	dev->info.y_range_pixel.max = canon_dev_info.max_length - 1;
	dev->info.y_range_pixel.quant = 0;
	
	/* set max/min mm */
	dev->info.x_range.min = SANE_FIX(0.0);
	dev->info.x_range.max = SANE_FIX(dev->info.x_range_pixel.max * MM_PER_INCH / dev->info.mud);
	dev->info.x_range.quant = 0;
	
	dev->info.y_range.min = SANE_FIX(0.0);
	dev->info.y_range.max = SANE_FIX(dev->info.y_range_pixel.max * MM_PER_INCH / dev->info.mud);
	dev->info.y_range.quant = 0;
	
	init_opts( s );
	
	s->scanMethod = 0;
	s->sane_start_status = SANE_STATUS_NO_DOCS;
	
	opened_handle = s;
	*h = (SANE_Handle)s;
	
	status = SANE_STATUS_GOOD;
	
EXIT_CMT_NETWORK_MUTEX_UNLOCK:
	
	cmt_network_mutex_unlock();
	return status;
}

/*-------------------------------------------------
	sane_close()
-------------------------------------------------*/
#ifdef __USE_LIBSANE__
void sane_canon_mfp_close(
#else
void sane_close(
#endif
	SANE_Handle h )
{
	CANON_Scanner *s = (CANON_Scanner *) h;
	CNMSLLDAPI *api = ( CNMSLLDAPI * )s->hw->cnmslldapi;

	api->p_canon_terminate_scanner();
	
	canon_usb_close ( s );
	canon_network_close( s );

	opened_handle = NULL;
	
	if ( s ){
		if ( s->val[OPT_MODE].s ) free (s->val[OPT_MODE].s);
		free (s);
	}
}

/*-------------------------------------------------
	sane_get_option_descriptor()
-------------------------------------------------*/
#ifdef __USE_LIBSANE__
const SANE_Option_Descriptor * sane_canon_mfp_get_option_descriptor(
#else
const SANE_Option_Descriptor * sane_get_option_descriptor(
#endif
	SANE_Handle	h,		/* handle */
	SANE_Int	n )		/* option no. */
{
	CANON_Scanner *s = h;

	if ((unsigned) n >= NUM_OPTIONS) {
		return NULL;
	}
	return (s->opt + n);
}

/*-------------------------------------------------
	sane_control_option()
-------------------------------------------------*/
#ifdef __USE_LIBSANE__
SANE_Status sane_canon_mfp_control_option(
#else
SANE_Status sane_control_option(
#endif
	SANE_Handle h, 			/* handle */
	SANE_Int n,				/* option no. */
	SANE_Action a,
	void *v,
	SANE_Int *i)
{
	CANON_Scanner *s = h;
	SANE_Status status;
	SANE_Word cap;
	unsigned int ErrorCode;
	CNMSLLDAPI *api = ( CNMSLLDAPI * )s->hw->cnmslldapi;

	DBGMSG("<sane_control_option> opt=%s (%s)\n", option_name[n], ((a==SANE_ACTION_GET_VALUE)?"GET":"SET"));

	
	if (i)
		*i = 0;

	/*--- check BUSY status ---*/
	if ( s->scanning == SANE_TRUE &&
			 n != OPT_EX_CALIBRATION_STATUS &&
			 n != OPT_EX_STATUS ) {
		DBGMSG("<sane_control_option> SANE_STATUS_DEVICE_BUSY\n");
		return (SANE_STATUS_DEVICE_BUSY);
	}
	
	if (n >= NUM_OPTIONS) {
		return (SANE_STATUS_INVAL);
	}
	cap = s->opt[n].cap;
	if (!SANE_OPTION_IS_ACTIVE (cap))
		if ( !check_ex_opts( n ) )
			return (SANE_STATUS_INVAL);
	
	/* action : get value */
	if (a == SANE_ACTION_GET_VALUE)
	{
		if ( !v ) {
			return (SANE_STATUS_INVAL);
		}
		switch( n )
		{
			/*--- Word(Int, Fix, Bool) ---*/
			case OPT_RESOLUTION_BIND:
			case OPT_X_RESOLUTION:
			case OPT_Y_RESOLUTION:
			case OPT_TL_X:
			case OPT_TL_Y:
			case OPT_BR_X:
			case OPT_BR_Y:
			case OPT_NUM_OPTS:
			case OPT_PREVIEW:

			case OPT_EX_USE_AREA_PIXEL:
			case OPT_EX_TL_X_PIXEL:
			case OPT_EX_TL_Y_PIXEL:
			case OPT_EX_BR_X_PIXEL:
			case OPT_EX_BR_Y_PIXEL:
			/* Ver.1.20 */
			case OPT_EX_EVERY_CALIBRATION:
			case OPT_EX_SILENT:
			/* Ver.1.30 */
			case OPT_EX_SCAN_METHOD:

				*(SANE_Word *) v = s->val[n].w;
				if (i)
					*i |= SANE_INFO_RELOAD_PARAMS;
		
				return (SANE_STATUS_GOOD);

			/*--- get scanner status ---*/
			case OPT_EX_STATUS:
				cmd_sane_ctlopt_status();
				if ( api->p_canon_get_status( &ErrorCode ) != 0 )
					return (SANE_STATUS_INVAL);
			
				/* Ver.1.40 */
				if ( s->hw->info.product_id == 0x1735 && ErrorCode == 145 ) {
					ErrorCode = 155;
				}
				
				*(SANE_Word *) v = ErrorCode;
				if ( i )
					*i |= SANE_INFO_RELOAD_PARAMS;
	
				return (SANE_STATUS_GOOD);
	
			/*--- get ability version ---*/
			case OPT_EX_ABILITY_VER:
				DBGMSG("<sane_control_option> p_canon_get_ability_ver()\n");
				if ( api->p_canon_get_ability_ver( (int *)v ) != 0 )
					return (SANE_STATUS_INVAL);

				if (i)
					*i |= SANE_INFO_RELOAD_PARAMS;
				return (SANE_STATUS_GOOD);
			
			/*--- get ability ---*/
			case OPT_EX_ABILITY:
				DBGMSG("<sane_control_option> p_canon_get_ability()\n");
				if ( api->p_canon_get_ability( v ) != 0 )
					return (SANE_STATUS_INVAL);

				if (i)
					*i |= SANE_INFO_RELOAD_PARAMS;
				return (SANE_STATUS_GOOD);
			
			/*--- get calibration status ---*/
			case OPT_EX_CALIBRATION_STATUS:
				cmd_sane_ctlopt_calibration();
				if ( api->p_canon_get_calibration_status( &ErrorCode ) != 0 )
					return (SANE_STATUS_INVAL);
			
				*(SANE_Word *) v = ErrorCode;
				if (i)
					*i |= SANE_INFO_RELOAD_PARAMS;
	
				return (SANE_STATUS_GOOD);
			
			/*--- set gamma ---*/
			case OPT_EX_GAMMA:
				memcpy( v, s->val[n].wa, s->opt[n].size );
				
				if (i)
					*i |= SANE_INFO_RELOAD_PARAMS;
				return (SANE_STATUS_GOOD);
	
			/*--- String ---*/
			case OPT_MODE:
				strcpy (v, s->val[n].s);
				if (i)
					*i |= SANE_INFO_RELOAD_PARAMS;
				return (SANE_STATUS_GOOD);
			
			
			/*--- get device settings ---*/
			case OPT_EX_DEVICE_SETTINGS:
				DBGMSG("<sane_control_option> p_canon_get_devicesettings()\n");
				cmd_sane_ctlopt_getsettings();
				cmt_network_mutex_lock();
				if ( api->p_canon_get_devicesettings( v ) != 0 ){
					cmt_network_mutex_unlock();
					return (SANE_STATUS_INVAL);
				}
				cmt_network_mutex_unlock();
					
				if (i)
					*i |= SANE_INFO_RELOAD_PARAMS;
				return (SANE_STATUS_GOOD);
			
			default:
				return (SANE_STATUS_INVAL);
		}
	}

	/* action : set value */
	else if (a == SANE_ACTION_SET_VALUE)
	{
		if (!SANE_OPTION_IS_SETTABLE (cap))
			return (SANE_STATUS_INVAL);
		
		status = check_opts_val ( (const SANE_Option_Descriptor * )s->opt + n, v );
		if (status != SANE_STATUS_GOOD)
			return status;
	
		switch( n )
		{
			/*--- Word(Int, Fix, Bool) ---*/
			case OPT_X_RESOLUTION:
			case OPT_Y_RESOLUTION:
			case OPT_TL_X:
			case OPT_TL_Y:
			case OPT_BR_X:
			case OPT_BR_Y:
			
			case OPT_EX_USE_AREA_PIXEL:
			case OPT_EX_TL_X_PIXEL:
			case OPT_EX_TL_Y_PIXEL:
			case OPT_EX_BR_X_PIXEL:
			case OPT_EX_BR_Y_PIXEL:

			/* Ver.1.20 */
			case OPT_EX_EVERY_CALIBRATION:
			case OPT_EX_SILENT:
			/* Ver.1.30 */
			case OPT_EX_SCAN_METHOD:
				if ( i && s->val[n].w != *(SANE_Word *) v )
					*i |= SANE_INFO_RELOAD_PARAMS;
			
			case OPT_NUM_OPTS:
			case OPT_PREVIEW:
				s->val[n].w = *(SANE_Word *) v;
				return (SANE_STATUS_GOOD);
			
			case OPT_EX_GAMMA:
				memcpy( s->val[n].wa, v, s->opt[n].size );
				return (SANE_STATUS_GOOD);
			
			case OPT_RESOLUTION_BIND:
				if (s->val[n].w != *(SANE_Word *) v )
				{
					s->val[n].w = *(SANE_Word *) v;
		
					if (i) { *i |= SANE_INFO_RELOAD_OPTIONS; }
					
					if (s->val[n].w == SANE_FALSE)
					{
						s->opt[OPT_Y_RESOLUTION].cap &= ~SANE_CAP_INACTIVE;
						
						s->opt[OPT_X_RESOLUTION].title = CANON_TITLE_RES_X;
						s->opt[OPT_X_RESOLUTION].name  = CANON_NAME_RES_X;
						s->opt[OPT_X_RESOLUTION].desc  = CANON_DESC_NULL;
					}
					else
					{
						s->opt[OPT_Y_RESOLUTION].cap |= SANE_CAP_INACTIVE;
						
						s->opt[OPT_X_RESOLUTION].title = CANON_TITLE_RES;
						s->opt[OPT_X_RESOLUTION].name  = CANON_NAME_RES;
						s->opt[OPT_X_RESOLUTION].desc  = CANON_DESC_NULL;
					}
				}
				return SANE_STATUS_GOOD;
	
			case OPT_MODE:
				if (i && strcmp( s->val[n].s, (SANE_String) v ) )
				*i |= SANE_INFO_RELOAD_OPTIONS | SANE_INFO_RELOAD_PARAMS;
			
				if (s->val[n].s)
				free (s->val[n].s);
		
				s->val[n].s = strdup( v );

				return (SANE_STATUS_GOOD);
	
			/*--- set device settings ---*/
			case OPT_EX_DEVICE_SETTINGS:
				DBGMSG("<sane_control_option> p_canon_set_devicesettings()\n");
				cmd_sane_ctlopt_setsettings();
				cmt_network_mutex_lock();
				if ( api->p_canon_set_devicesettings( v ) != 0 ){
					cmt_network_mutex_unlock();
					return (SANE_STATUS_INVAL);
				}
				cmt_network_mutex_unlock();
					
				if (i)
					*i |= SANE_INFO_RELOAD_PARAMS;
				return (SANE_STATUS_GOOD);
			
			default:
				return (SANE_STATUS_INVAL);
		}
	}

	return (SANE_STATUS_INVAL);

}

/*-------------------------------------------------
	sane_start()
-------------------------------------------------*/
#ifdef __USE_LIBSANE__
SANE_Status sane_canon_mfp_start(
#else
SANE_Status sane_start(
#endif
	SANE_Handle h )
{
	char 			*mode_str;

	const	char	*mode;
	int				scanmode;
	CANON_SCANDATA	scandata;
	CANON_Scanner	*s = h;
	CNMSLLDAPI *api = ( CNMSLLDAPI * )s->hw->cnmslldapi;
	int status;
	SANE_Word	api_ver = s->hw->api_ver ;	/* Ver.1.20 */
	CANON_SCANOPTS	scanopts;
	
	DBGMSG("<sane_start>\n");
	
	if ( s->sane_start_status == SANE_STATUS_NO_DOCS ) {
		cmt_network_mutex_lock();
	}
	
	s->scanning = SANE_TRUE;
	
	mode_str = s->val[OPT_MODE].s;
	s->xres = s->val[OPT_X_RESOLUTION].w;
	s->yres = s->val[OPT_Y_RESOLUTION].w;
	
	if ( (s->val[OPT_RESOLUTION_BIND].w == SANE_TRUE)  || (s->val[OPT_PREVIEW].w == SANE_TRUE) )
	{
		s->yres = s->xres;
	}
	
	/* use pixels */
	if ( s->val[OPT_EX_USE_AREA_PIXEL].w ) {
		s->ulx = ( s->val[OPT_EX_TL_X_PIXEL].w * s->hw->info.mud ) / s->xres;
		s->uly = ( s->val[OPT_EX_TL_Y_PIXEL].w * s->hw->info.mud ) / s->yres;
		
		s->width = ( ( s->val[OPT_EX_BR_X_PIXEL].w - s->val[OPT_EX_TL_X_PIXEL].w ) * s->hw->info.mud ) / s->xres;
		s->length = ( ( s->val[OPT_EX_BR_Y_PIXEL].w - s->val[OPT_EX_TL_Y_PIXEL].w ) * s->hw->info.mud ) / s->yres;
	}
	else {
		s->ulx = SANE_UNFIX(s->val[OPT_TL_X].w) * s->hw->info.mud / MM_PER_INCH;
		s->uly = SANE_UNFIX(s->val[OPT_TL_Y].w) * s->hw->info.mud / MM_PER_INCH;
		
		s->width = SANE_UNFIX(s->val[OPT_BR_X].w - s->val[OPT_TL_X].w) * s->hw->info.mud / MM_PER_INCH;
		s->length = SANE_UNFIX(s->val[OPT_BR_Y].w - s->val[OPT_TL_Y].w) * s->hw->info.mud / MM_PER_INCH;
	}
	
	/* Ver.1.20 */
	s->params.depth = ( api_ver > 1 ) ? SCAN_BITS_APIV2 : 8 ;
	
	s->bpp = s->params.depth;
	
	mode = s->val[OPT_MODE].s;
	if (strcmp(mode, "Gray")==0) {
		s->params.format = SANE_FRAME_GRAY;
		scanmode = 2;
	} else {
		s->params.format = SANE_FRAME_RGB;
		scanmode = 4;
	}
	
	/* Ver.1.20 */
	memset( &scanopts, 0, sizeof( scanopts ) );
	scanopts.every_calibration	= ( s->val[OPT_EX_EVERY_CALIBRATION].w )	? CNMS_SCANOPTS_ON : CNMS_SCANOPTS_OFF;
	scanopts.silent				= ( s->val[OPT_EX_SILENT].w )				? CNMS_SCANOPTS_ON : CNMS_SCANOPTS_OFF;
	
	/* Ver.1.30 */
	s->scanMethod = s->val[OPT_EX_SCAN_METHOD].w;
	
	cmd_sane_start();
	if ( api_ver > 1 ) {
		status = api->p_canon_set_parameter_ex(
			s->xres,			/* XRes */
			s->yres,			/* YRes */
			s->ulx,				/* Left */
			s->uly,				/* Top */
			s->width,			/* Width */
			s->length,			/* Length */
			scanmode,			/* ScanMode */
			s->params.depth,	/* BitsPerPixel */
			s->scanMethod,		/* ScanMethod */
			s->gamma_table,		/* gamma table */
			&scandata,			/* CANON_SCANDATA */
			&scanopts			/* CANON_SCANOPTS */
		);
	}
	else {
		/* ignore scanopts ( every calibration, silent, ... ) */
		status = api->p_canon_set_parameter(
			s->xres,			/* XRes */
			s->yres,			/* YRes */
			s->ulx,				/* Left */
			s->uly,				/* Top */
			s->width,			/* Width */
			s->length,			/* Length */
			scanmode,			/* ScanMode */
			s->params.depth,	/* BitsPerPixel */
			0,					/* ScanMethod */
			s->gamma_table,		/* gamma table */
			&scandata			/* CANON_SCANDATA */
		);
	}
	
	if ( status < 0 ) {
		DBGMSG("<sane_start> error in canon_set_parameter/canon_set_parameter_ex()\n");
		status = SANE_STATUS_INVAL;
		goto _EXIT;
	}
	else if ( status == CNMP_ST_NO_PAPER ) {	/* No paper */
		DBGMSG("<sane_start> error in canon_set_parameter/canon_set_parameter_ex()\n");
		status = SANE_STATUS_NO_DOCS;
		cmt_network_mutex_unlock();
		goto _EXIT;
	}
	
	if ( api->p_canon_get_parameters == NULL ) {	/* api_ver == 0 */
		s->params.bytes_per_line = scandata.bytes_per_line;
		s->params.pixels_per_line = scandata.pixels_per_line;
		s->params.lines = scandata.lines;
		s->bytes_to_read = scandata.available;
	}
	
	s->params.last_frame = SANE_TRUE;

	if ( api->p_canon_start_scan() < 0 ) {
		DBGMSG("<sane_start> error in canon_start_scan()\n");
		status = SANE_STATUS_INVAL;
		goto _EXIT;
	}
	
	status = SANE_STATUS_GOOD;

_EXIT:
	s->sane_start_status = status;
	return status;
}

/*-------------------------------------------------
	sane_get_parameters()
-------------------------------------------------*/
#ifdef __USE_LIBSANE__
SANE_Status sane_canon_mfp_get_parameters(
#else
SANE_Status sane_get_parameters(
#endif
	SANE_Handle		h,
	SANE_Parameters	*p )
{
	CANON_Scanner *s = h;
	CNMSLLDAPI *api = ( CNMSLLDAPI * )s->hw->cnmslldapi;
	int status;
	CANON_SCANDATA scandata;
	SANE_Word	api_ver = s->hw->api_ver ;	/* Ver.1.20 */
	
	cmd_sane_set_scanning();
	if( !s->scanning )	/* before sane_start() */
	{
		int width, length, xres, yres;
		const char *mode;

		memset( &s->params, 0, sizeof (s->params) );
	
		xres = s->val[OPT_X_RESOLUTION].w;
		yres = s->val[OPT_Y_RESOLUTION].w;
		if ( ( s->val[OPT_RESOLUTION_BIND].w == SANE_TRUE ) || ( s->val[OPT_PREVIEW].w == SANE_TRUE ) ) {
			yres = xres;
		}
		
		/* use pixels */
		if ( s->val[OPT_EX_USE_AREA_PIXEL].w ) {
			width = ( ( s->val[OPT_EX_BR_X_PIXEL].w - s->val[OPT_EX_TL_X_PIXEL].w ) * s->hw->info.mud ) / xres;
			length = ( ( s->val[OPT_EX_BR_Y_PIXEL].w - s->val[OPT_EX_TL_Y_PIXEL].w ) * s->hw->info.mud ) / yres;
		}
		else {
			width = SANE_UNFIX(s->val[OPT_BR_X].w - s->val[OPT_TL_X].w) * s->hw->info.mud / MM_PER_INCH;
			length = SANE_UNFIX(s->val[OPT_BR_Y].w - s->val[OPT_TL_Y].w) * s->hw->info.mud / MM_PER_INCH;
		}
		
		/* Ver.1.20 */
		s->params.depth = ( api_ver > 1 ) ? SCAN_BITS_APIV2 : 8 ;
		
		if (xres > 0 && yres > 0 && width > 0 && length > 0)
		{
			s->params.pixels_per_line = width * xres / s->hw->info.mud;
			s->params.lines = length * yres / s->hw->info.mud;
		}
		
		mode = s->val[OPT_MODE].s;
		if (strcmp(mode, "Gray") == 0) {
			s->params.format = SANE_FRAME_GRAY;
			s->params.bytes_per_line = s->params.pixels_per_line;
		} else {
			s->params.format = SANE_FRAME_RGB;
			s->params.bytes_per_line = 3 * s->params.pixels_per_line;
		}
		s->params.bytes_per_line *= ( s->params.depth / 8 );	/* Ver.1.20 */
		s->params.last_frame = SANE_TRUE;
		
		cmd_sane_get_parameters();
	}
	else{
		if ( api->p_canon_get_parameters != NULL ) {	/* api_ver > 0 */
			status = api->p_canon_get_parameters( &scandata );
			if ( status < 0 ) {
				DBGMSG("<sane_get_parameters> error in p_canon_get_parameters()\n");
				status = SANE_STATUS_INVAL;
				goto _EXIT;
			}
			s->params.bytes_per_line = scandata.bytes_per_line;
			s->params.pixels_per_line = scandata.pixels_per_line;
			s->params.lines = scandata.lines;
			s->bytes_to_read = scandata.available;
		}
	}
	
	if( p ) {
		*p = s->params;
	}
	
	status = SANE_STATUS_GOOD;
	
_EXIT:
	return status;
}

/*-------------------------------------------------
	sane_read()
-------------------------------------------------*/
#ifdef __USE_LIBSANE__
SANE_Status sane_canon_mfp_read(
#else
SANE_Status sane_read(
#endif
	SANE_Handle	h,
	SANE_Byte	*buf,
	SANE_Int	maxlen,
	SANE_Int	*len)
{
	SANE_Status status;
	SANE_Int nread;
	CANON_Scanner	*s = h;
	CNMSLLDAPI *api = ( CNMSLLDAPI * )s->hw->cnmslldapi;
	SANE_Word	api_ver = s->hw->api_ver ;	/* Ver.1.20 */
	
	*len = 0;
	maxlen = maxlen;

	if (s->params.lines <= 0) {
		s->scanning = SANE_FALSE;
		if ( api_ver < 2 ) {
			status = api->p_canon_do_cancel();
		}
		else {
			status = api->p_canon_end_scan();	/* Ver.1.20 */
		}
		status = SANE_STATUS_EOF;
		goto _EXIT;
	}

	if (!s->scanning) {
		status = SANE_STATUS_CANCELLED;
		goto _EXIT;
	}
	nread = 1;
	nread *= (SANE_Int)s->params.bytes_per_line;
	
	cmd_sane_read();
	if ( api->p_canon_read_scan ( buf, nread ) < 0 ) {
		status = SANE_STATUS_IO_ERROR;
		goto _EXIT;
	}
	*len = nread;
	s->params.lines--;
	
	status = SANE_STATUS_GOOD;

_EXIT:
	return status;
}

/*-------------------------------------------------
	sane_cancel()
-------------------------------------------------*/
#ifdef __USE_LIBSANE__
void sane_canon_mfp_cancel(
#else
void sane_cancel(
#endif
	SANE_Handle h )
{
	CANON_Scanner *s = h;
	CNMSLLDAPI *api = ( CNMSLLDAPI * )s->hw->cnmslldapi;
	SANE_Word	api_ver = s->hw->api_ver ;	/* Ver.1.20 */
	int			skip_unlock = 0;
	
	DBGMSG("<sane_cancel>\n");
	
	s->scanning = SANE_FALSE;
	
	if ( api_ver < 2 ) {
		api->p_canon_do_cancel();
	}
	/* api_ver >= 2 */
	else {
		if ( s->params.lines <= 0 ) {
			/* end scan */
			if ( s->scanMethod ) {	/* ADF */
				if ( s->sane_start_status == SANE_STATUS_NO_DOCS ) {
					api->p_canon_end_scan();
					skip_unlock = 1;
					DBGMSG("<sane_cancel> skip cmt_network_mutex_unlock()\n");
				}
				else {
					api->p_canon_do_cancel();
				}
			}
			else {
				api->p_canon_end_scan();
			}
		}
		else {
			/* user cancel */
			api->p_canon_do_cancel();
		}
	}
	
	if( !skip_unlock ) {
		cmt_network_mutex_unlock();
	}
	
	/* for next sane_start */
	s->sane_start_status = SANE_STATUS_NO_DOCS;
}

/*-------------------------------------------------
	sane_set_io_mode()
-------------------------------------------------*/
#ifdef __USE_LIBSANE__
SANE_Status sane_canon_mfp_set_io_mode(
#else
SANE_Status sane_set_io_mode(
#endif
	SANE_Handle	h,
	SANE_Bool	m )
{
	h = h;
	m = m;
	
	return SANE_STATUS_UNSUPPORTED;
}

/*-------------------------------------------------
	sane_get_select_fd()
-------------------------------------------------*/
#ifdef __USE_LIBSANE__
SANE_Status sane_canon_mfp_get_select_fd(
#else
SANE_Status sane_get_select_fd(
#endif
	SANE_Handle	h,
	SANE_Int	*fd)
{
	h = h;
	fd = fd;
	return SANE_STATUS_UNSUPPORTED;
}


#ifdef __USE_LIBSANE__
SANE_Status sane_init(
	SANE_Int * version_code,
	SANE_Auth_Callback authorize)
{
	return sane_canon_mfp_init( version_code, authorize );
}

void sane_exit(void)
{
	sane_canon_mfp_exit();
}

SANE_Status sane_get_devices(
	const SANE_Device *** device_list,
	SANE_Bool local_only)
{
	return sane_canon_mfp_get_devices( device_list, local_only );
}

SANE_Status sane_open(
	SANE_String_Const name,
	SANE_Handle *h )
{
	return sane_canon_mfp_open( name, h );
}

void sane_close(
	SANE_Handle h )
{
	sane_canon_mfp_close( h );
}

const SANE_Option_Descriptor * sane_get_option_descriptor(
	SANE_Handle h,
	SANE_Int n)
{
	return sane_canon_mfp_get_option_descriptor( h, n );
}

SANE_Status sane_control_option(
	SANE_Handle h,
	SANE_Int n,
	SANE_Action a,
	void *v,
	SANE_Int * info)
{
	return sane_canon_mfp_control_option( h, n, a, v, info );
}

SANE_Status sane_get_parameters(
	SANE_Handle h,
	SANE_Parameters *p )
{
	return sane_canon_mfp_get_parameters( h, p );
}

SANE_Status sane_start(
	SANE_Handle h )
{
	return sane_canon_mfp_start( h );
}

SANE_Status sane_read(
	SANE_Handle h,
	SANE_Byte *buf,
	SANE_Int maxlen,
	SANE_Int *len)
{
	return sane_canon_mfp_read( h, buf, maxlen, len );
}

void sane_cancel(SANE_Handle h )
{
	sane_canon_mfp_cancel( h );
}

SANE_Status sane_set_io_mode(
	SANE_Handle h,
	SANE_Bool m)
{
	return sane_canon_mfp_set_io_mode( h, m );
}

SANE_Status sane_get_select_fd(
	SANE_Handle h,
	SANE_Int * fd)
{
	return sane_canon_mfp_get_select_fd( h, fd );
}
#endif

/*-------------------------------------------------
	Check network machine installed
-------------------------------------------------*/
static int canon_get_network_installed( void )
{
	char	line[PATH_MAX], *dev = NULL;
	int		len, i, ret = 0;
	FILE	*fp;
	void	*handle = NULL;

	static int	network = -1;

	if( network == -1 ){
		network = 0;
		/*--- read Configuration file. ---*/
		fp = cmt_conf_file_open( CANON_CONFIG_FILE );
		if ( fp ) {

			/* Set USB/Network device list */
			while ( ( len = cmt_conf_file_read_line (line, sizeof (line), fp) ) >= 0 ) {
				if( strncmp( line, "net", 3 ) == 0 ){
					dev = &line[4];
					i = 0;
					while( canon_mfp_product_ids[i].product != -1 ){
						if( strncmp( canon_mfp_product_ids[i].model, dev, strlen( canon_mfp_product_ids[i].model ) ) == 0 ){
							if( ( handle = dlopen( canon_mfp_product_ids[i].libname, RTLD_LAZY ) ) != NULL ){
								dlclose( handle );
								network = 1;
								break;
							}
						}
						i++;
					}
					if( network != 0 )	break;
				}
			}

			fclose ( fp );
		}
	}
	ret = network;

	return ret;
}

