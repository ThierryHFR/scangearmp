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

#ifndef _CNMSNETWORK_C_
#define _CNMSNETWORK_C_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include "interface.h"
#include "support.h"

#include "cnmstype.h"
#include "cnmsfunc.h"
#include "w1.h"
#include "cnmsnetwork.h"
#include "cnmsstrings.h"
#include "scanflow.h"
#include "libcnnet.h"

#include "progress_bar.h"


/*
	definitions for network
*/
#define CACHE_PATH				DEFTOSTR( CNMSLIBPATH ) "/canon_mfp_net.ini"
#define NETWORK_DEV_MAX			(64)
#define TIMEOUT_MSEC (80)


static CNMSInt32 CnmsSerachNetworkDevice( CNMSVoid )
{
	CNNLHANDLE hmdl=NULL;
	CNMSInt32	ret = CNMS_ERR,
				max = NETWORK_DEV_MAX;
	int			found = 0;
	CNNLNICINFO *nic = CNMSNULL;

	unsigned long	cnnl_ver = 110L, cnnl_ver_size = sizeof( cnnl_ver );
	void 			*cnnl_callback = (void *)ProgressBarUpdatePulse;
	unsigned long	cnnl_callback_size = sizeof( cnnl_callback );
	unsigned long	cnnl_interval = TIMEOUT_MSEC, cnnl_interval_size = sizeof( cnnl_interval );
	
	if( CNNL_Init( &hmdl ) != CNNL_RET_SUCCESS ) goto EXIT;
	
	if (CNNL_Config( hmdl, CNNL_CONFIG_SET_VERSION, &cnnl_ver, &cnnl_ver_size ) != CNNL_RET_SUCCESS) goto EXIT;
	if (CNNL_Config( hmdl, CNNL_CONFIG_SET_CALLBACK_FUNCTION, cnnl_callback, &cnnl_callback_size ) != CNNL_RET_SUCCESS) goto EXIT;
	if (CNNL_Config( hmdl, CNNL_CONFIG_SET_CALLBACK_INTERVAL, &cnnl_interval, &cnnl_interval_size ) != CNNL_RET_SUCCESS) goto EXIT;

	if( ( nic = (CNNLNICINFO *)CnmsGetMem( sizeof( CNNLNICINFO ) * max ) ) == CNMSNULL ) goto EXIT;
	if( CNNL_SearchPrintersEx( hmdl, nic, CACHE_PATH, max, &found, CNNET_SEARCH_AUTO, 1, 5000 ) != CNNL_RET_SUCCESS ){
		DBGMSG( "CnmsSerachNetworkDevice() : Error is occured in CNNL_SearchPrintersEx().\n" );
		goto EXIT;
	}
	
	ret = CNMS_NO_ERR;
EXIT:
	ProgressBarWaitFinish();
	if( nic )			CnmsFreeMem( (CNMSLPSTR)nic );
	if( hmdl != NULL)	CNNL_Terminate( &hmdl );

	return ret;
}

CNMSInt32 CnmsReloadNetworkCache( CNMSVoid )
{
	CNMSInt32		ret = CNMS_ERR;

	if( ( ret = CnmsSerachNetworkDevice() ) != CNMS_NO_ERR ){
		DBGMSG( "[CnmsReloadNetworkCache]Error is occured in canon_reload_network_cache().\n" );
		goto	EXIT;
	}

EXIT:
	return	ret;
}

#endif	/* _CNMSNETWORK_C_ */
