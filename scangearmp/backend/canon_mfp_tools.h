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


#ifndef _canon_mfp_tools_h_
#define _canon_mfp_tools_h_

#include <sane/sane.h>
#include <inttypes.h>

#include "libcnnet.h"

#define _CANON_VENDOR_ID (0x04a9)

#define DEFTOSTR1(def) #def
#define DEFTOSTR(def) DEFTOSTR1(def)

#ifdef _CMT_DEBUG
#include <stdarg.h>
/* debug message */
static void DBGMSG( const char *format, ... )
{
	va_list ap;
	va_start( ap, format );
	fprintf( stderr, "[%12s] ",DEFTOSTR(_CMT_DEBUG_NAME_) );
	vfprintf( stderr, format, ap );
	va_end( ap );
}
#else
/* do nothing. */
#define DBGMSG( x, ... )
#endif

FILE *cmt_conf_file_open(const char *conf);
int cmt_conf_file_read_line(char *line, int size, FILE *fp);

int cmt_get_device_id( char *line, int len );
char *cmt_find_device( int id, int *index );
char *cmt_find_device_net( char *line, int len, int *index, char **model );

void cmt_libusb_init(void);
void cmt_libusb_exit(void);
SANE_Status cmt_libusb_open(const char *devname, int *index);
void cmt_libusb_close(int index);

SANE_Status cmt_libusb_get_id( const char *devname, int *idVendor, int *idProduct, int *speed );

SANE_Status cmt_libusb_bulk_write( int index, unsigned char *buffer, unsigned long *size );
SANE_Status cmt_libusb_bulk_read( int index, unsigned char *buffer, unsigned long *size );


void cmt_network_init( int network );
void cmt_network_exit( void );
SANE_Status cmt_network_open(const char *macaddr, CNNLHANDLE *handle);
void cmt_network_close(CNNLHANDLE *handle);

SANE_Status cmt_network_write( CNNLHANDLE handle, unsigned char *buffer, unsigned long *size );
SANE_Status cmt_network_read( CNNLHANDLE handle, unsigned char *buffer, unsigned long *size );

void cmt_network_mutex_lock( void );
void cmt_network_mutex_unlock( void );

int cmt_serach_network_device( int mode );

#endif /*_canon_mfp_tools_h_ */
