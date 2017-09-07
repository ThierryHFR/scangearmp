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

#ifndef _SCANMAIN_C_
#define _SCANMAIN_C_

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sane/sane.h>

#include "scanmain.h"
#include "cnmsfunc.h"
#include "scanstdlg.h"
#include "preference.h"
#include "scanflow.h"
#include "w1.h"
#include "coloradjust.h"
#include "prev_main.h"
#include "progress_bar.h"
#include "file_control.h"
#include "keep_setting.h"

#include "errors.h"

/* #define	__CNMS_DEBUG_SCAN__ */

#define	SCANMAIN_DEV_MODEL_LEN	( 32 )
#define	SCANMAIN_DEV_NAME_LEN	( 32 )
#define	SCANMAIN_DEV_FULL_LEN	( (SCANMAIN_DEV_MODEL_LEN) + (SCANMAIN_DEV_NAME_LEN) )

#define	CNMS_SCAN_MIN_WIDTH		(96)
#define	CNMS_SCAN_MIN_HEIGHT	(96)
#define	CNMS_SCAN_MIN_BASE_RES	(600)

typedef struct{
	CNMSInt32			dev;
	SANE_Handle			hand;
	CNMSInt32			scanStatus;
	CNMSInt32			devNum;
	CNMSLPSTR			lpModelStr;
	CNMSLPSTR			lpNameStr;
	CNMSLPSTR			lpFullStr;
	CNMSOPTSNUM			optsNum;
	CANON_ABILITY_INFO	ability;
}CNMSSCANWORK, *LPCNMSSCANWORK;

static CNMSLPSTR	lpWork = CNMSNULL;

#ifdef	__CNMS_DEBUG__
static CNMSLPSTR ActionModeStr[ CNMSSCPROC_ACTION_MAX ] = { "SCAN", "PREVIEW", "PARAM", "CROP_CORRECT", "PREV_CLEAR" };

CNMSVoid DebugCanonAbilityInfo( CANON_ABILITY_INFO *lpCanon );
CNMSVoid DebugUiInfo( CNMSInt32 ActionMode, CNMSUIInfo *lpInfo );
CNMSVoid DebugUiReturn( CNMSInt32 ActionMode, CNMSUIReturn *lpRtn );
#endif

typedef struct{
	CNMSInt32		uiMediaType;
	CNMSInt32		abilitySubInfo;
}CNMS_SCANSOURCE, *LPCNMS_SCANSOURCE;

static const CNMS_SCANSOURCE scanSource[] = {
	{ CNMS_A_SOURCE_PLATEN		, CANON_ABILITY_SUB_INFO_FLATBED	},
	{ CNMS_A_SOURCE_TEXT_SADF	, CANON_ABILITY_SUB_INFO_ADF		},
	{ CNMS_A_SOURCE_TEXT_DADF	, CANON_ABILITY_SUB_INFO_ADF2		},
	{ CNMS_ERR					, CNMS_ERR							},
};


CNMSInt32 CnmsGetScanSourceAbility(
	CNMSInt32			uiMediaType				)
{
	CNMSInt32		ret = CNMS_ERR, i;
	
	for( i = 0 ; scanSource[i].uiMediaType != CNMS_ERR ; i++ ){
		if( uiMediaType == scanSource[i].uiMediaType ){
			ret = scanSource[i].abilitySubInfo;
			break;
		}
	}

#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsGetScanSourceAbility( uiMediaType )] = %d.\n",ret );
#endif
	return ret;
}

CNMSInt32 CnmsScanInit(
	CNMSBool			init					)
{
	CNMSInt32			ret = CNMS_ERR, ldata, i;
	LPCNMSSCANWORK		lpScanWork;
	SANE_Int			idata;
	CNMSByte			bracketsF[] = " (\0", bracketsR[] = ")\0";
	const SANE_Device	**device_list = CNMSNULL;

	if( init ){
		if( lpWork != CNMSNULL ){
			goto	EXIT_ERR;
		}
	}
	else{
		if( lpWork != CNMSNULL ){
			CnmsFreeMem( lpWork );
		}
	}
		
	/***** sane_init *****/
	if( ( ldata = sane_init( &idata, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG( "[CnmsScanInit]Error is occured in sane_init.\n" );
		goto	EXIT_ERR;
	}
	/***** sane_get_devices *****/
	if( ( ldata = sane_get_devices( &device_list, SANE_FALSE ) ) != SANE_STATUS_GOOD ){
		DBGMSG( "[CnmsScanInitGetDevList]Error is occured in sane_get_devices.\n" );
		goto	EXIT_ERR_2;
	}
	for( i = 0 ; ; i ++ ){
		if( device_list[ i ] == CNMSNULL ){
			break;
		}
	}

	if( KeepSettingCommonOpen() != CNMS_NO_ERR ){
		DBGMSG( "[CnmsScanInit]Error is occured in KeepSettingCommonOpen.\n" );
		goto	EXIT;
	}

	/* Get Work Memory */
	ldata = sizeof( CNMSSCANWORK ) + ( SCANMAIN_DEV_MODEL_LEN + SCANMAIN_DEV_NAME_LEN + SCANMAIN_DEV_FULL_LEN ) * ( i + 1 );
	if( ( lpWork = CnmsGetMem( ldata ) ) == CNMSNULL ){
		DBGMSG( "[CnmsScanInit]Can't get work memory.\n" );
		goto	EXIT_ERR_2;
	}
	lpScanWork = (LPCNMSSCANWORK)lpWork;
	lpScanWork->scanStatus = CNMS_SCAN_STATUS_INITED;
	lpScanWork->devNum = i;
	lpScanWork->lpModelStr = lpWork + sizeof( CNMSSCANWORK );
	lpScanWork->lpNameStr = lpScanWork->lpModelStr + SCANMAIN_DEV_MODEL_LEN * lpScanWork->devNum;
	lpScanWork->lpFullStr = lpScanWork->lpNameStr + SCANMAIN_DEV_NAME_LEN * lpScanWork->devNum;
	for( i = 0 ; i < lpScanWork->devNum ; i ++ ){
		/* copy model(ex.MP800) */
		if( ( ldata = CnmsStrCopy( ( CNMSLPSTR )device_list[ i ]->model, lpScanWork->lpModelStr + i * SCANMAIN_DEV_MODEL_LEN, SCANMAIN_DEV_MODEL_LEN ) ) <= 0 ){
			DBGMSG( "[CnmsScanInit]Can't copy model string.\n" );
			goto	EXIT_ERR;
		}
		/* copy name(ex./dev/usb/001/001) */
		if( ( ldata = CnmsStrCopy( ( CNMSLPSTR )device_list[ i ]->name, lpScanWork->lpNameStr + i * SCANMAIN_DEV_NAME_LEN, SCANMAIN_DEV_NAME_LEN ) ) <= 0 ){
			DBGMSG( "[CnmsScanInit]Can't copy name string.\n" );
			goto	EXIT_ERR;
		}
		/* copy model(ex.MP800) + name(ex./dev/usb/001/001) */
		if( ( ldata = CnmsStrCopy( ( CNMSLPSTR )device_list[ i ]->model, lpScanWork->lpFullStr + i * SCANMAIN_DEV_FULL_LEN, SCANMAIN_DEV_FULL_LEN ) ) <= 0 ){
			DBGMSG( "[CnmsScanInit]Can't copy model + name string.\n" );
			goto	EXIT_ERR;
		}
		else if( ( ldata = CnmsStrCat( bracketsF, lpScanWork->lpFullStr + i * SCANMAIN_DEV_FULL_LEN, SCANMAIN_DEV_FULL_LEN ) ) <= 0 ){
			DBGMSG( "[CnmsScanInit]Can't copy model + name string.\n" );
			goto	EXIT_ERR;
		}
		else if( ( ldata = CnmsStrCat( ( CNMSLPSTR )device_list[ i ]->name, lpScanWork->lpFullStr + i * SCANMAIN_DEV_FULL_LEN, SCANMAIN_DEV_FULL_LEN ) ) <= 0 ){
			DBGMSG( "[CnmsScanInit]Can't copy model + name string.\n" );
			goto	EXIT_ERR;
		}
		else if( ( ldata = CnmsStrCat( bracketsR, lpScanWork->lpFullStr + i * SCANMAIN_DEV_FULL_LEN, SCANMAIN_DEV_FULL_LEN ) ) <= 0 ){
			DBGMSG( "[CnmsScanInit]Can't copy model + name string.\n" );
			goto	EXIT_ERR;
		}
	}
	ret = lpScanWork->devNum;
EXIT:
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsScanInit()]=%d.\n", ret );
#endif
	return	ret;

EXIT_ERR_2:
	sane_exit();
EXIT_ERR:
	CnmsScanClose();
	goto	EXIT;
}

CNMSInt32 CnmsGetDeviceIndexSelectedLatest( CNMSVoid )
{
	LPCNMSSCANWORK	lpScanWork = (LPCNMSSCANWORK)lpWork;
	CNMSInt32		i, ret = 0;
	CNMSLPSTR		latest = CNMSNULL;

	if( ( latest = KeepSettingCommonGetString( KEEPSETTING_COMMON_ID_MACADDRESS ) ) == CNMSNULL ){
		DBGMSG( "[CnmsGetDeviceIndexSelectedLatest]Error is occured in KeepSettingCommonGetString.\n" );
		goto	EXIT_ERR;
	}

	for( i = 0 ; i < lpScanWork->devNum ; i ++ ){
		if( CnmsStrCompare( latest, lpScanWork->lpNameStr + i * SCANMAIN_DEV_NAME_LEN ) == 0 ){
			ret = i;
			break;
		}
	}
EXIT_ERR:
	return	ret;
}

CNMSVoid CnmsScanClose( CNMSVoid )
{
	LPCNMSSCANWORK		lpScanWork;

	if( ( lpScanWork = (LPCNMSSCANWORK)lpWork ) == CNMSNULL ){
		goto	EXIT;
	}

	switch( lpScanWork->scanStatus ){
		case	CNMS_SCAN_STATUS_INITED:
			goto	EXIT_2;
			break;
		case	CNMS_SCAN_STATUS_OPENED:
			goto	EXIT_3;
			break;
	}
EXIT_3:
	sane_close( lpScanWork->hand );
EXIT_2:
	sane_exit();
	
	CnmsFreeMem( lpWork );
	lpWork = CNMSNULL;
EXIT:
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsScanClose()].\n" );
#endif
	return;
}

CNMSLPSTR CnmsScanGetDevModelStr(
		CNMSInt32		type, 
		CNMSInt32		devIndex )
{
	CNMSLPSTR			lpRet = CNMSNULL;
	LPCNMSSCANWORK		lpScanWork;

	if( ( lpScanWork = (LPCNMSSCANWORK)lpWork ) == CNMSNULL ){
		DBGMSG( "[CnmsScanGetDevModelStr]Status is error.\n" );
		goto	EXIT;
	}
	else if( lpScanWork->devNum <= devIndex ){
		DBGMSG( "[CnmsScanGetDevModelStr]Device index( %d ) is above max value( %d ).\n", devIndex, lpScanWork->devNum );
		goto	EXIT;
	}
	
	if( type == CNMS_SCAN_GETDEVMODELSTR_FULL ){
		lpRet = lpScanWork->lpFullStr + SCANMAIN_DEV_FULL_LEN * devIndex;
	}
	else{
		lpRet = lpScanWork->lpModelStr + SCANMAIN_DEV_MODEL_LEN * devIndex;
	}
EXIT:
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsScanGetDevModelStr(type:%d, devIndex:%d)]=%s.\n", type, devIndex, lpRet );
#endif
	return	lpRet;
}

CNMSInt32 CnmsScanOpen(
		CNMSInt32			devIndex )
{
	CNMSInt32			ret = CNMS_ERR, ldata;
	LPCNMSSCANWORK		lpScanWork;
	SANE_Fixed			val;
	SANE_Status			status;
	LPCNMSOPTSNUM		lpOpts;
	CNMSByte			address[SCANMAIN_DEV_NAME_LEN];

	if( ( ( lpScanWork = (LPCNMSSCANWORK)lpWork ) == CNMSNULL ) || ( lpScanWork->scanStatus != CNMS_SCAN_STATUS_INITED ) ){
		DBGMSG( "[CnmsScanOpen]Status is error.\n" );
		goto	EXIT;
	}
	else if( lpScanWork->devNum <= devIndex ){
		DBGMSG( "[CnmsScanOpen]Device index( %d ) is above max value( %d ).\n", devIndex, lpScanWork->devNum );
		goto	EXIT;
	}
	lpScanWork->dev = devIndex;

	/***** sane_open *****/
	if( ( ldata = sane_open( lpScanWork->lpNameStr + devIndex * SCANMAIN_DEV_NAME_LEN, &( lpScanWork->hand ) ) ) != SANE_STATUS_GOOD ){
		DBGMSG( "[CnmsScanOpen]Error is occured in sane_open.\n" );

		/*
			SANE_STATUS_IO_ERROR : network open error (BERRCODE_DEVICE_NOT_AVAILABLE_LAN)
			SANE_STATUS_DEVICE_BUSY : network busy error (BERRCODE_DEVICE_NOT_AVAILABLE)
			other : USB open error (BERRCODE_CONNECT_FAILED)
		*/
		switch ( ldata ) {
			case SANE_STATUS_IO_ERROR :
				lastBackendErrCode = BERRCODE_DEVICE_NOT_AVAILABLE_LAN;
				break;
			case SANE_STATUS_DEVICE_BUSY :
				lastBackendErrCode = BERRCODE_DEVICE_NOT_AVAILABLE;
				break;
			default :
				lastBackendErrCode = BERRCODE_CONNECT_FAILED;
				break;
		}
		
		goto	EXIT;
	}
	lpOpts = &( lpScanWork->optsNum );

	/***** Set MAC Address *****/
	if( CnmsGetNetworkModelInstalled() == 1 ){
		if( CnmsStrCopy( lpScanWork->lpNameStr + devIndex * SCANMAIN_DEV_NAME_LEN, address, SCANMAIN_DEV_NAME_LEN ) == CNMS_ERR ){
			DBGMSG( "[CnmsScanOpen]Error is occured in CnmsStrCopy.\n" );
			goto	EXIT;
		}
		if( strncmp( address, "libusb", 6 ) == 0 ){
			if( CnmsStrCopy( KEEPSETTING_MAC_ADDRESS_USB, address, SCANMAIN_DEV_NAME_LEN ) == CNMS_ERR ){
				DBGMSG( "[CnmsScanOpen]Error is occured in CnmsStrCopy.\n" );
				goto	EXIT;
			}
		}
		if( KeepSettingCommonSetString( KEEPSETTING_COMMON_ID_MACADDRESS, address ) != CNMS_NO_ERR ){
			DBGMSG( "[CnmsScanOpen]Error is occured in KeepSettingCommonSetString.\n" );
			goto	EXIT;
		}
	}

	/***** set opts num *****/
	if( ( ldata = CnmsSetOptsNum( lpScanWork->hand, lpOpts ) ) != CNMS_NO_ERR ) {
		DBGMSG( "[CnmsScanOpen]Error is occured in CnmsSetOptsNum.\n" );
		goto	EXIT_ERR;
	}
	/***** get ability version *****/
	if( ( status = sane_control_option( lpScanWork->hand, lpOpts->getAbilityVer, SANE_ACTION_GET_VALUE, &( lpScanWork->ability.version ), CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG( "[CnmsScanOpen]Error is occured in sane_control_option(GET)( ability version ).\n" );
		goto	EXIT_ERR;
	}

	if( CANON_AB_VER_MEJOR( lpScanWork->ability.version ) != 1 ){
		DBGMSG( "[CnmsScanOpen]ability struct version error.\n" );
		goto	EXIT_ERR;
	}

	/***** get ability *****/
	if( ( status = sane_control_option( lpScanWork->hand, lpOpts->getAbility, SANE_ACTION_GET_VALUE, &( lpScanWork->ability ), CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG( "[CnmsScanOpen]Error is occured in sane_control_option(GET)( ability ).\n" );
		goto	EXIT_ERR;
	}

#ifdef	__CNMS_DEBUG_SCAN__
	DebugCanonAbilityInfo( &( lpScanWork->ability ) );
#endif
	/***** set use pixel *****/
	val = SANE_TRUE;
	if( ( status = sane_control_option( lpScanWork->hand, lpOpts->useAreaPixel, SANE_ACTION_SET_VALUE, &val, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG( "[CnmsScanOpen]Error is occured in sane_control_option(SET)( useAreaPixel ).\n" );
		goto	EXIT_ERR;
	}
	else if( ( status = sane_control_option( lpScanWork->hand, lpOpts->useAreaPixel, SANE_ACTION_GET_VALUE, &val, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG( "[CnmsScanOpen]Error is occured in sane_control_option(GET)( useAreaPixel ).\n" );
		goto	EXIT_ERR;
	}
	else if( val != SANE_TRUE ){
		DBGMSG( "[CnmsScanOpen]Can't set sane_control_option( useAreaPixel ) value.\n" );
		goto	EXIT_ERR;
	}
	lpScanWork->scanStatus = CNMS_SCAN_STATUS_OPENED;
	
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsScanOpen(devIndex:%d)]=%d.\n", devIndex, ret );
#endif
	return	ret;

EXIT_ERR:
	sane_close( lpScanWork->hand );
	goto	EXIT;
}

CNMSInt32 CnmsScanGetPreviewResolution(
		CNMSInt32		source )
{
	CNMSInt32			ret = CNMS_ERR;
	LPCNMSSCANWORK		lpScanWork;

	if( ( ( lpScanWork = (LPCNMSSCANWORK)lpWork ) == CNMSNULL ) || ( lpScanWork->scanStatus < CNMS_SCAN_STATUS_OPENED ) ){
		DBGMSG( "[CnmsScanGetPreviewResolution]Status is error.\n" );
		goto	EXIT;
	}
	else if( source >= CANON_ABILITY_SUB_INFO_MAX ){
		DBGMSG( "[CnmsScanGetPreviewResolution]Parameter is error.\n" );
		goto	EXIT;
	}
	else if( lpScanWork->ability.info[ source ].support == 0 ){
		DBGMSG( "[CnmsScanGetPreviewResolution]This scan source(%d) is not supported.\n", source );
		goto	EXIT;
	}
	ret = (CNMSInt32)( lpScanWork->ability.xres[ lpScanWork->ability.info[ source ].res_index_preview ] );
EXIT:
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsScanGetPreviewResolution(source:%d)]=%d.\n", source, ret );
#endif
	return	ret;
}

CNMSInt32 CnmsScanGetRealMinSize(
		CNMSInt32		source,
		CNMSInt32		*lpSize )
{
	CNMSInt32			ret = CNMS_ERR;
	LPCNMSSCANWORK		lpScanWork;

	if( ( ( lpScanWork = (LPCNMSSCANWORK)lpWork ) == CNMSNULL ) || ( lpScanWork->scanStatus < CNMS_SCAN_STATUS_OPENED ) ){
		DBGMSG( "[CnmsScanGetRealMinSize]Status is error.\n" );
		goto	EXIT;
	}
	else if( ( source >= CANON_ABILITY_SUB_INFO_MAX ) || ( lpSize == CNMSNULL ) ){
		DBGMSG( "[CnmsScanGetRealMinSize]Parameter is error.\n" );
		goto	EXIT;
	}
	else if( lpScanWork->ability.info[ source ].support == 0 ){
		DBGMSG( "[CnmsScanGetRealMinSize]Scan source(%d) is not supported.\n", source );
		goto	EXIT;
	}

	lpSize[ CNMS_DIM_H ] = CNMS_SCAN_MIN_WIDTH;
	lpSize[ CNMS_DIM_V ] = CNMS_SCAN_MIN_HEIGHT;

	ret = CNMS_SCAN_MIN_BASE_RES;
EXIT:
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsScanGetRealMinSize(source:%d)]=%d, minSize:%dx%d.\n", source, ret, lpSize[ CNMS_DIM_H ], lpSize[ CNMS_DIM_V ] );
#endif
	return	ret;

}

CNMSInt32 CnmsScanGetRealMaxSize(
		CNMSInt32		source,
		CNMSInt32		*lpSize )
{
	CNMSInt32			ret = CNMS_ERR;
	LPCNMSSCANWORK		lpScanWork;

	if( ( ( lpScanWork = (LPCNMSSCANWORK)lpWork ) == CNMSNULL ) || ( lpScanWork->scanStatus < CNMS_SCAN_STATUS_OPENED ) ){
		DBGMSG( "[CnmsScanGetRealMaxSize]Status is error.\n" );
		goto	EXIT;
	}
	else if( ( source >= CANON_ABILITY_SUB_INFO_MAX ) || ( lpSize == CNMSNULL ) ){
		DBGMSG( "[CnmsScanGetRealMaxSize]Parameter is error.\n" );
		goto	EXIT;
	}
	else if( lpScanWork->ability.info[ source ].support == 0 ){
		DBGMSG( "[CnmsScanGetRealMaxSize]Scan source(%d) is not supported.\n", source );
		goto	EXIT;
	}
	lpSize[ CNMS_DIM_H ] = lpScanWork->ability.info[ source ].max_width;
	lpSize[ CNMS_DIM_V ] = lpScanWork->ability.info[ source ].max_length;

	ret = (CNMSInt32)( lpScanWork->ability.opt_res );
EXIT:
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsScanGetRealMaxSize(source:%d)]=%d, maxSize:%dx%d\n", source, ret, lpSize[ CNMS_DIM_H ], lpSize[ CNMS_DIM_V ] );
#endif
	return	ret;
}

CNMSInt32 CnmsScanGetRealMinMaxSize(
		CNMSInt32		source,
		CNMSInt32		*lpMinSize,
		CNMSInt32		*lpMaxSize )
{
	CNMSInt32		ret = CNMS_ERR, i, minRes, maxRes, minSize[ CNMS_DIM_MAX ], maxSize[ CNMS_DIM_MAX ];

	if( ( lpMinSize == CNMSNULL ) || ( lpMaxSize == CNMSNULL ) ){
		DBGMSG( "[CnmsScanGetRealMinMaxSize]Parameter is error.\n" );
		goto	EXIT;
	}
	else if( ( minRes = CnmsScanGetRealMinSize( source, minSize ) ) <= 0 ){
		DBGMSG( "[CnmsScanGetRealMinMaxSize]Error is occured in CnmsScanGetRealMinSize.\n" );
		goto	EXIT;
	}
	else if( ( maxRes = CnmsScanGetRealMaxSize( source, maxSize ) ) <= 0 ){
		DBGMSG( "[CnmsScanGetRealMinMaxSize]Error is occured in CnmsScanGetRealMaxSize.\n" );
		goto	EXIT;
	}
	
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( minRes < maxRes ){
			lpMinSize[ i ] = minSize[ i ] * ( maxRes / minRes );
			lpMaxSize[ i ] = maxSize[ i ];
			ret = maxRes;
		}
		else{
			lpMinSize[ i ] = minSize[ i ];
			lpMaxSize[ i ] = maxSize[ i ] * ( minRes / maxRes );
			ret = minRes;
		}
	}
EXIT:
	return	ret;
}

CNMSInt32 CnmsScanGetMinMaxScale(
		CNMSInt32		res,
		CNMSInt32		*lpMinMax )
{
	CNMSInt32		ret = CNMS_ERR;

	if( ( res == 0 ) || ( lpMinMax == CNMSNULL ) ){
		DBGMSG( "[CnmsScanGetMaxScale]Parameter is error.\n" );
		goto	EXIT;
	}
	lpMinMax[ 0 ] = 25;
	lpMinMax[ 1 ] = 1920000 / res;

	if( lpMinMax[ 1 ] > 76800 ){
		lpMinMax[ 1 ] = 76800;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsScanGetMinMaxScale(res:%d)]=%d, min:%d, max:%d\n", res, ret, lpMinMax[ 0 ], lpMinMax[ 1 ] );
#endif
	return	ret;
}

CNMSInt32 CnmsScanChangeStatus(
		CNMSInt32 		ActionMode,
		CNMSFd			dstFd,
		LPCNMS_ROOT		root,
		CNMSUIReturn	*lpRtn )
{
	CNMSInt32			ret = CNMS_ERR, i, ldata, source, mediatype;
	CNMSInt32			outRes, tmpSize[ CNMS_DIM_MAX ];
	LPCNMSSCANWORK		lpScanWork;
	CNMSUIInfo			*lpUiInfo = CNMSNULL;
	CNMSPREFCOMP		prefComp;

	if( ( ( lpScanWork = (LPCNMSSCANWORK)lpWork ) == CNMSNULL ) || ( lpScanWork->scanStatus != CNMS_SCAN_STATUS_OPENED ) ){
		DBGMSG( "[CnmsScanChangeStatus]Status is error!\n" );
		goto	EXIT;
	}
	else if( ( ActionMode == CNMSSCPROC_ACTION_SCAN ) && ( ( dstFd != CNMS_FILE_ERR ) || ( root == CNMSNULL ) ) ){
		DBGMSG( "[CnmsScanChangeStatus]Parameter is error!\n" );
		goto	EXIT;
	}
	else if( ( ActionMode != CNMSSCPROC_ACTION_SCAN ) && ( ( dstFd == CNMS_FILE_ERR ) || ( root != CNMSNULL ) ) ){
		DBGMSG( "[CnmsScanChangeStatus]Parameter is error!\n" );
		goto	EXIT;
	}
	else if( lpRtn == CNMSNULL ){
		DBGMSG( "[CnmsScanChangeStatus]Parameter is error!\n" );
		goto	EXIT;
	}
	else if( ( lpUiInfo = (CNMSUIInfo *)CnmsGetMem( sizeof( CNMSUIInfo ) ) ) == CNMSNULL ){
		DBGMSG( "[CnmsScanChangeStatus]Can't get work memory!\n" );
		goto	EXIT;
	}
	else if( ( ldata = Preference_Get( lpW1Comp->lpLink, lpW1Comp->linkNum, &prefComp ) ) != CNMS_NO_ERR ){
		DBGMSG( "[CnmsScanChangeStatus]Error is occured in Preference_Get!\n" );
		goto	EXIT;
	}
	else if( ( mediatype = CnmsUiGetValue( CNMS_OBJ_A_SOURCE, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[CnmsScanChangeStatus]Error is occured in CnmsUiGetValue( CNMS_OBJ_A_SOURCE ).\n" );
		goto	EXIT;
	}

	if( ( source = CnmsGetScanSourceAbility( mediatype ) ) == CNMS_ERR ){
		DBGMSG( "[CnmsScanChangeStatus]Error is occured in CnmsGetScanSourceAbility( mediatype ).\n" );
		goto	EXIT;
	}

	if( ActionMode == CNMSSCPROC_ACTION_SCAN ){
		if( ( outRes = CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) <= 0 ){
			DBGMSG( "[CnmsScanChangeStatus]Error is occured in CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION ).\n" );
			goto	EXIT;
		}
	}
	else{
		if( ( outRes = CnmsScanGetPreviewResolution( source ) ) <= 0 ){
			DBGMSG( "[CnmsScanChangeStatus]Error is occured in CnmsScanGetPreviewResolution.\n" );
			goto	EXIT;
		}
	}
	/* calc offset + inSize */
	if( ( ldata = CnmsScanGetRealMaxSize( source, tmpSize ) ) <= 0 ){
		DBGMSG( "[CnmsScanChangeStatus]Error is occured in CnmsScanGetRealMaxSize.\n" );
		goto	EXIT;
	}
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		ldata = ( tmpSize[ i ] < ( lpW1Comp->offset[ i ] + lpW1Comp->inSize[ i ] ) ) ? tmpSize[ i ] : ( lpW1Comp->offset[ i ] + lpW1Comp->inSize[ i ] );
		tmpSize[ i ] = (CNMSInt32)CnmsConvUnit( (CNMSDec32)ldata, CNMS_A_UNIT_PIXEL, CNMS_A_UNIT_PIXEL, lpW1Comp->base_res, outRes );
	}

	lpUiInfo->ModelName = lpScanWork->lpModelStr + lpScanWork->dev * SCANMAIN_DEV_MODEL_LEN;
	lpUiInfo->Method = source;
	lpUiInfo->Preview = ( lpW1Comp->previewFlag == CNMS_TRUE ) ? CNMSSCPROC_PREVIEWIMG_ON : CNMSSCPROC_PREVIEWIMG_OFF;
	lpUiInfo->MediaType = CNMSSCPROC_MTYPE_PAPER;
	lpUiInfo->MediaSize = CNMSSCPROC_MSIZE_FULLPAGE;
	lpUiInfo->ColorMode = CnmsUiGetValue( CNMS_OBJ_A_COLOR_MODE, lpW1Comp->lpLink, lpW1Comp->linkNum );
	lpUiInfo->OutRes = outRes;
	lpUiInfo->InSize.Width  = (CNMSInt32)CnmsConvUnit( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_H ] ), CNMS_A_UNIT_PIXEL, CNMS_A_UNIT_PIXEL, lpW1Comp->base_res, outRes );
	lpUiInfo->InSize.Height  = (CNMSInt32)CnmsConvUnit( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_V ] ), CNMS_A_UNIT_PIXEL, CNMS_A_UNIT_PIXEL, lpW1Comp->base_res, outRes );
	if( ActionMode == CNMSSCPROC_ACTION_SCAN ){
		if( lpW1Comp->group == CNMSUI_SIZE_GROUP_DISPLAY ){
			lpUiInfo->OutSize.Width  = (CNMSInt32)CnmsConvUnit( (CNMSDec32)( lpW1Comp->outSize[ CNMS_DIM_H ] ), CNMS_A_UNIT_PIXEL, CNMS_A_UNIT_PIXEL, lpW1Comp->base_res, lpW1Comp->size_res );
			lpUiInfo->OutSize.Height  = (CNMSInt32)CnmsConvUnit( (CNMSDec32)( lpW1Comp->outSize[ CNMS_DIM_V ] ), CNMS_A_UNIT_PIXEL, CNMS_A_UNIT_PIXEL, lpW1Comp->base_res, lpW1Comp->size_res );
		}
		else{
			lpUiInfo->OutSize.Width  = (CNMSInt32)CnmsConvUnit( (CNMSDec32)( lpW1Comp->outSize[ CNMS_DIM_H ] ), CNMS_A_UNIT_PIXEL, CNMS_A_UNIT_PIXEL, lpW1Comp->base_res, outRes );
			lpUiInfo->OutSize.Height  = (CNMSInt32)CnmsConvUnit( (CNMSDec32)( lpW1Comp->outSize[ CNMS_DIM_V ] ), CNMS_A_UNIT_PIXEL, CNMS_A_UNIT_PIXEL, lpW1Comp->base_res, outRes );
		}
	}
	else{
		lpUiInfo->OutSize.Width  = lpUiInfo->InSize.Width;
		lpUiInfo->OutSize.Height = lpUiInfo->InSize.Height;
	}
	lpUiInfo->InOffset.X  = ( tmpSize[ CNMS_DIM_H ] - lpUiInfo->InSize.Width < 0 ) ? 0 : ( tmpSize[ CNMS_DIM_H ] - lpUiInfo->InSize.Width );
	lpUiInfo->InOffset.Y  = ( tmpSize[ CNMS_DIM_V ] - lpUiInfo->InSize.Height < 0 ) ? 0 : ( tmpSize[ CNMS_DIM_V ] - lpUiInfo->InSize.Height );
	lpUiInfo->ImgSetFlagUi.USM = CnmsUiGetValue( CNMS_OBJ_A_UNSHARP_MASK, lpW1Comp->lpLink, lpW1Comp->linkNum );
	lpUiInfo->ImgSetFlagUi2.USM = CnmsUiGetValue( CNMS_OBJ_A_UNSHARP_MASK, lpW1Comp->lpLink, lpW1Comp->linkNum );
	if( W1_JudgeFormatType( CNMS_OBJ_A_DESCREEN ) == CNMS_TRUE ){
		lpUiInfo->ImgSetFlagUi2.Descreen = CnmsUiGetValue( CNMS_OBJ_A_DESCREEN, lpW1Comp->lpLink, lpW1Comp->linkNum );
	}
	if( ( ( lpUiInfo->Preview == CNMSSCPROC_PREVIEWIMG_ON ) && ( lpUiInfo->ColorMode != CNMSSCPROC_CMODE_MONO ) ) ||
		( ( lpUiInfo->Preview == CNMSSCPROC_PREVIEWIMG_OFF ) && ( ( ActionMode == CNMSSCPROC_ACTION_SCAN ) || ( ActionMode == CNMSSCPROC_ACTION_PREVIEW ) ) ) )
	{
		CnmsColAdjSetUserTone( lpUiInfo->UserTone.R, lpUiInfo->UserTone.G, lpUiInfo->UserTone.B );
	}
	lpUiInfo->Threshold = mc_mono.threshold;
	lpUiInfo->ThresholdDefault = lpW1Comp->thresholdDef;
	lpUiInfo->ColSetting = prefComp.color_setting;
	lpUiInfo->MonGamma = prefComp.monitor_gamma;
	lpUiInfo->SilentMode = prefComp.silent_mode;
	lpUiInfo->EveryCalibration = prefComp.every_calibration;
	lpUiInfo->Calibration = calibration_dialog;
	lpUiInfo->Hand = lpScanWork->hand;
	lpUiInfo->OptsNum = lpScanWork->optsNum;
	lpUiInfo->Ability = lpScanWork->ability;

	lpUiInfo->BindingLocation = CNMS_A_BINDING_LOCATION_SHORT;
	if( W1_JudgeFormatType( CNMS_OBJ_A_BINDING_LOCATION ) == CNMS_TRUE ){
		lpUiInfo->BindingLocation = CnmsUiGetValue( CNMS_OBJ_A_BINDING_LOCATION, lpW1Comp->lpLink, lpW1Comp->linkNum );
	}

#ifdef	__CNMS_DEBUG_SCAN__
//DebugUiInfo( ActionMode, lpUiInfo );
#endif

	if( ActionMode <= CNMSSCPROC_ACTION_PREVIEW ){	/* Preview or Scan */
		CNMSInt32 page;
		page = ( source == CANON_ABILITY_SUB_INFO_FLATBED ) ? 0 : 1;
		if( ( ldata = ProgressBarStart( PROGRESSBAR_ID_SCAN, page ) ) != CNMS_NO_ERR ){
			DBGMSG( "[CnmsScanChangeStatus]Error is occured in ProgressBarStart.\n" );
			goto	EXIT;
		}
	}

	ldata = CnmsScanFlowExec( dstFd, root, lpUiInfo, ActionMode, lpRtn );

	if( ActionMode == CNMSSCPROC_ACTION_SCAN ){	/* Scan */
		ProgressBarEnd();
	}
	
	if( ldata != CNMS_NO_ERR ){
		/* Preview & STOP key */
		if ( ActionMode == CNMSSCPROC_ACTION_PREVIEW && ldata != CNMS_CANCELED ) {
			ProgressBarEnd();
		}
	
		DBGMSG( "[CnmsScanChangeStatus]Error is occured in CnmsScanFlowExec.\n" );
		goto	EXIT;
	}

	if( ActionMode == CNMSSCPROC_ACTION_PREVIEW ){
		lpUiInfo->Preview = CNMSSCPROC_PREVIEWIMG_ON;
		if( ( ldata = FileControlSeekFile( dstFd, 0, FILECONTROL_SEEK_FROM_TOP ) ) < 0 ) {
			DBGMSG( "[CnmsScanChangeStatus]Error is occured in FileControlSeekFile.\n" );
			goto	EXIT;
		}
		if( ( ldata = Preview_RectGet( tmpSize ) ) != CNMS_NO_ERR ){
			DBGMSG( "[CnmsScanChangeStatus]Error is occured in Preview_RectGet.\n" );
			goto	EXIT;
		}
		if( tmpSize[ CNMS_DIM_H ] == 0 ){
			lpUiInfo->InSize.Width = lpRtn->ResultImgSize.Width;
			lpUiInfo->InSize.Height = lpRtn->ResultImgSize.Height;
			lpUiInfo->OutSize.Width = lpRtn->ResultImgSize.Width;
			lpUiInfo->OutSize.Height = lpRtn->ResultImgSize.Height;
		}
		if( ( ldata = CnmsScanFlowExec( dstFd, CNMSNULL, lpUiInfo, CNMSSCPROC_ACTION_CROP_CORRECT, lpRtn ) ) != CNMS_NO_ERR ){
			DBGMSG( "[CnmsScanChangeStatus]Error is occured in CnmsScanFlowExec( Preview at Crop On ).\n" );
			goto	EXIT;
		}
	}
	
	/* Change Preview On/Off Status for Coloradjust Dialog */
	CnmsColAdjGetPreviewStatus( lpUiInfo->Preview );
	if( ( ( ActionMode == CNMSSCPROC_ACTION_CROP_CORRECT ) || ( ActionMode == CNMSSCPROC_ACTION_PREVIEW ) || ( ActionMode == CNMSSCPROC_ACTION_PARAM ) ) &&
		( lpUiInfo->Preview == CNMSSCPROC_PREVIEWIMG_ON ) )
	{
		if( lpUiInfo->ColorMode != CNMSSCPROC_CMODE_MONO )
		{
			CnmsColAdjSetHistogram( CNMS_CA_DIALOG_HG,
									lpRtn->Histogram.Master, lpRtn->Histogram.Red, lpRtn->Histogram.Green, lpRtn->Histogram.Blue, lpRtn->Histogram.Gray );
			CnmsColAdjSetHistogram( CNMS_CA_DIALOG_FR,
									lpRtn->FinalReview.Master, lpRtn->FinalReview.Red, lpRtn->FinalReview.Green, lpRtn->FinalReview.Blue, lpRtn->FinalReview.Gray );
		}
		else{
			CnmsColAdjSetHistogram( CNMS_CA_DIALOG_MC,
									lpRtn->MonoChrome.Gray, CNMSNULL, CNMSNULL, CNMSNULL, CNMSNULL );
		}

	}
	if( ActionMode == CNMSSCPROC_ACTION_PREV_CLEAR ){
		CnmsColAdjGetPreviewStatus( CNMS_TRUE );
		CnmsColAdjClearHistogram();
		CnmsColAdjUpDateHistogram();
		CnmsColAdjGetPreviewStatus( lpUiInfo->Preview );
	}
	else{
		CnmsColAdjUpDateHistogram();
	}

	if( ActionMode == CNMSSCPROC_ACTION_PREVIEW ){	/* Preview */
		ProgressBarEnd();
	}
	
#ifdef	__CNMS_DEBUG_SCAN__
//DebugUiReturn( ActionMode, lpRtn );
#endif

	ret = CNMS_NO_ERR;
EXIT:
	if( lpUiInfo != CNMSNULL ){
		CnmsFreeMem( (CNMSLPSTR)lpUiInfo );
	}
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsScanChangeStatus(ActionMode:%d)]=%d.\n", ActionMode, ret );
#endif
	return	ret;
}

CNMSVoid CnmsScanCancel( CNMSVoid )
{
	LPCNMSSCANWORK		lpScanWork;
	
	if( ( lpScanWork = (LPCNMSSCANWORK)lpWork ) == CNMSNULL ){
		goto	EXIT;
	}
	else if( lpScanWork->scanStatus != CNMS_SCAN_STATUS_STARTED ){
		goto	EXIT;
	}
	lpScanWork->scanStatus = CNMS_SCAN_STATUS_OPENED;
EXIT:
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsScanCancel()].\n" );
#endif
	return;
}

CNMSVoid CnmsAllScanDataDispose(
		LPCNMS_ROOT			root		)
{
	LPCNMS_NODE		node;

	while( CnmsDisposeRoot( &root ) > 0 ) {
		node = root->head;
		DBGMSG( "[CnmsAllScanDataDispose] delete file in list [%s]\n", node->file_path );
		FileControlDeleteFile( node->file_path, CNMS_FILE_ERR );
		CnmsDisposeQueue( root, CNMS_NODE_HEAD );
	}

EXIT:
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsAllScanDataDispose(root:%p)].\n",root );
#endif
	return;
}

CNMSInt32 CnmsGetScanInfoParameter(
		CNMSScanInfo 	*info )
{
	CNMSInt32			ret = CNMS_ERR;
	LPCNMSSCANWORK		lpScanWork;

	if( ( info == CNMSNULL ) || ( ( lpScanWork = (LPCNMSSCANWORK)lpWork ) == CNMSNULL ) || ( lpScanWork->scanStatus != CNMS_SCAN_STATUS_OPENED ) ){
		DBGMSG( "[CnmsGetScanInfoParameter]Status is error!\n" );
		goto	EXIT;
	}
	CnmsSetMem( (CNMSLPSTR)info, 0, sizeof( CNMSScanInfo ) );

	if( ( info->OutRes = CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) <= 0 ){
		DBGMSG( "[CnmsGetScanInfoParameter]Error is occured in CnmsUiGetRealValue() = %d.\n", info->OutRes );
		goto	EXIT;
	}
	
	info->InSize.Width  = (CNMSInt32)CnmsConvUnit( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_H ] ), CNMS_A_UNIT_PIXEL, CNMS_A_UNIT_PIXEL, lpW1Comp->base_res, info->OutRes );
	info->InSize.Height  = (CNMSInt32)CnmsConvUnit( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_V ] ), CNMS_A_UNIT_PIXEL, CNMS_A_UNIT_PIXEL, lpW1Comp->base_res, info->OutRes );

	info->OutSize.Width  = (CNMSInt32)CnmsConvUnit( (CNMSDec32)( lpW1Comp->outSize[ CNMS_DIM_H ] ), CNMS_A_UNIT_PIXEL, CNMS_A_UNIT_PIXEL, lpW1Comp->base_res, info->OutRes );
	info->OutSize.Height  = (CNMSInt32)CnmsConvUnit( (CNMSDec32)( lpW1Comp->outSize[ CNMS_DIM_V ] ), CNMS_A_UNIT_PIXEL, CNMS_A_UNIT_PIXEL, lpW1Comp->base_res, info->OutRes );

	info->ActionMode = CNMSSCPROC_ACTION_MAX;
	
	if( CnmsScanFlowSetOptionResolution( info, &(lpScanWork->ability) ) != CNMS_NO_ERR ){
		DBGMSG( "[CnmsGetScanInfoParameter]Error is occured in CnmsScanFlowSetOptionResolution.\n" );
		goto	EXIT;
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsGetScanInfoParameter(info:%p)]=%d.\n",info, ret );
#endif
	return ret;
}

/* Ver.1.90- */
CNMSInt32 CnmsGetDeviceSettings(
	CANON_DEVICE_SETTINGS	*devsets )
{
	CNMSInt32			ret = CNMS_ERR;
	LPCNMSSCANWORK		lpScanWork;
	LPCNMSOPTSNUM		lpOpts;
	SANE_Status			status;

	if( ( ( lpScanWork = (LPCNMSSCANWORK)lpWork ) == CNMSNULL ) || ( lpScanWork->scanStatus != CNMS_SCAN_STATUS_OPENED ) ){
		DBGMSG( "[CnmsGetDeviceSettings]Status is error!\n" );
		goto	EXIT;
	}
	lpOpts = &( lpScanWork->optsNum );
	/***** get device settings *****/
	if( ( status = sane_control_option( lpScanWork->hand, lpOpts->deviceSettings, SANE_ACTION_GET_VALUE, devsets, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG( "[CnmsGetDeviceSettings]Error is occured in sane_control_option(GET)( deviceSettings ).\n" );
		if( ( status = sane_control_option( lpScanWork->hand, lpOpts->getStatus, SANE_ACTION_GET_VALUE, &lastBackendErrCode, CNMSNULL ) ) != SANE_STATUS_GOOD ){
			DBGMSG( "[CnmsGetDeviceSettings] Can't get last error code.\n" );
			goto	EXIT;
		}
		goto	EXIT;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsGetDeviceSettings()]=%d, lastBackendErrCode=%d\n", ret, lastBackendErrCode );
#endif
	return ret;
}

CNMSInt32 CnmsSetDeviceSettings(
	CANON_DEVICE_SETTINGS	*devsets )
{
	CNMSInt32			ret = CNMS_ERR;
	LPCNMSSCANWORK		lpScanWork;
	LPCNMSOPTSNUM		lpOpts;
	SANE_Status			status;

	if( ( ( lpScanWork = (LPCNMSSCANWORK)lpWork ) == CNMSNULL ) || ( lpScanWork->scanStatus != CNMS_SCAN_STATUS_OPENED ) ){
		DBGMSG( "[CnmsSetDeviceSettings]Status is error!\n" );
		goto	EXIT;
	}
	lpOpts = &( lpScanWork->optsNum );
	/***** set device settings *****/
	if( ( status = sane_control_option( lpScanWork->hand, lpOpts->deviceSettings, SANE_ACTION_SET_VALUE, devsets, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG( "[CnmsSetDeviceSettings]Error is occured in sane_control_option(SET)( deviceSettings ).\n" );
		if( ( status = sane_control_option( lpScanWork->hand, lpOpts->getStatus, SANE_ACTION_GET_VALUE, &lastBackendErrCode, CNMSNULL ) ) != SANE_STATUS_GOOD ){
			DBGMSG( "[CnmsSetDeviceSettings] Can't get last error code.\n" );
			goto	EXIT;
		}
		goto	EXIT;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SCAN__
	DBGMSG( "[CnmsSetDeviceSettings()]=%d, lastBackendErrCode=%d\n", ret, lastBackendErrCode );
#endif
	return ret;
}


#ifdef	__CNMS_DEBUG__
CNMSVoid DebugCanonAbilityInfo(
		CANON_ABILITY_INFO		*lpCanon )
{
	CNMSInt32		i;

	if( lpCanon == CNMSNULL ){
		goto	EXIT;
	}

	DBGMSG( "[CANON_ABILITY_INFO]version         :%d\n", lpCanon->version );
	DBGMSG( "[CANON_ABILITY_INFO]sensor_type     :%d\n", lpCanon->sensor_type );
	DBGMSG( "[CANON_ABILITY_INFO]scan_method     :%d\n", lpCanon->scan_method );
	DBGMSG( "[CANON_ABILITY_INFO]wait_calibration:%d\n", lpCanon->wait_calibration );
	DBGMSG( "[CANON_ABILITY_INFO]wait_warmup     :%d\n", lpCanon->wait_warmup );
	DBGMSG( "[CANON_ABILITY_INFO]res_num         :%d\n", lpCanon->res_num );
	for( i = 0 ; i < lpCanon->res_num ; i ++ ){
		DBGMSG( "[CANON_ABILITY_INFO]xres(%2d)        :%d\n", i, lpCanon->xres[ i ] );
		DBGMSG( "[CANON_ABILITY_INFO]yres(%2d)        :%d\n", i, lpCanon->yres[ i ] );
	}
	DBGMSG( "[CANON_ABILITY_INFO]opt_res         :%d\n", lpCanon->opt_res );
	DBGMSG( "[CANON_ABILITY_INFO]res_max         :%d\n", lpCanon->res_max );
	for( i = 0 ; i < 3 ; i ++ ){
		DBGMSG( "[CANON_ABILITY_SUB_INFO(%d)]support          :%d\n", i, lpCanon->info[ i ].support );
		DBGMSG( "[CANON_ABILITY_SUB_INFO(%d)]max_width        :%d\n", i, lpCanon->info[ i ].max_width );
		DBGMSG( "[CANON_ABILITY_SUB_INFO(%d)]max_length       :%d\n", i, lpCanon->info[ i ].max_length );
		DBGMSG( "[CANON_ABILITY_SUB_INFO(%d)]res_index_preview:%d\n", i, lpCanon->info[ i ].res_index_preview );
		DBGMSG( "[CANON_ABILITY_SUB_INFO(%d)]res_index_max    :%d\n", i, lpCanon->info[ i ].res_index_max );
		DBGMSG( "[CANON_ABILITY_SUB_INFO(%d)]res_index_min    :%d\n", i, lpCanon->info[ i ].res_index_min );
		DBGMSG( "[CANON_ABILITY_SUB_INFO(%d)]ref              :%d\n", i, lpCanon->info[ i ].ref );
	}
EXIT:
	return;
}

CNMSVoid DebugUiInfo(
		CNMSInt32		ActionMode,
		CNMSUIInfo		*lpInfo )
{

	if( lpInfo == CNMSNULL ){
		goto	EXIT;
	}
	DBGMSG( "[CNMSUIInfo(%s)]ModelName   :%s\n", ActionModeStr[ ActionMode ], lpInfo->ModelName );
	DBGMSG( "[CNMSUIInfo(%s)]Method      :%d\n", ActionModeStr[ ActionMode ], lpInfo->Method );
	DBGMSG( "[CNMSUIInfo(%s)]Preview     :%d\n", ActionModeStr[ ActionMode ], lpInfo->Preview );
	DBGMSG( "[CNMSUIInfo(%s)]MediaType   :%d\n", ActionModeStr[ ActionMode ], lpInfo->MediaType );
	DBGMSG( "[CNMSUIInfo(%s)]MediaSize   :%d\n", ActionModeStr[ ActionMode ], lpInfo->MediaSize );
	DBGMSG( "[CNMSUIInfo(%s)]ColorMode   :%d\n", ActionModeStr[ ActionMode ], lpInfo->ColorMode );
	DBGMSG( "[CNMSUIInfo(%s)]OutRes      :%d\n", ActionModeStr[ ActionMode ], lpInfo->OutRes );
	DBGMSG( "[CNMSUIInfo(%s)]InSize      :%dx%d\n", ActionModeStr[ ActionMode ], lpInfo->InSize.Width, lpInfo->InSize.Height );
	DBGMSG( "[CNMSUIInfo(%s)]OutSize     :%dx%d\n", ActionModeStr[ ActionMode ], lpInfo->OutSize.Width, lpInfo->OutSize.Height );
	DBGMSG( "[CNMSUIInfo(%s)]InOffset    :%dx%d\n", ActionModeStr[ ActionMode ], lpInfo->InOffset.X, lpInfo->InOffset.Y );
	DBGMSG( "[CNMSUIInfo(%s)]Unsharp Mask:%d\n", ActionModeStr[ ActionMode ], lpInfo->ImgSetFlagUi.USM );
/*	DBGMSG( "[CNMSUIInfo(%s)]UserTone    :%d\n", ActionModeStr[ ActionMode ], lpInfo->UserTone ); */
	DBGMSG( "[CNMSUIInfo(%s)]Threshold   :%d\n", ActionModeStr[ ActionMode ], lpInfo->Threshold );
	DBGMSG( "[CNMSUIInfo(%s)]ColSetting  :%d\n", ActionModeStr[ ActionMode ], lpInfo->ColSetting );
	DBGMSG( "[CNMSUIInfo(%s)]MonGamma    :%f\n", ActionModeStr[ ActionMode ], lpInfo->MonGamma );
	DBGMSG( "[CNMSUIInfo(%s)]*Calibration:%d\n", ActionModeStr[ ActionMode ], lpInfo->Calibration );
	DBGMSG( "[CNMSUIInfo(%s)]Hand        :%d\n", ActionModeStr[ ActionMode ], lpInfo->Hand );
/*	DBGMSG( "[CNMSUIInfo(%s)]OptsNum     :%d\n", lpInfo->OptsNum ); */
/*	DebugCanonAbilityInfo( &( lpInfo->Ability ) ); */

EXIT:
	return;
}

CNMSVoid DebugUiReturn(
		CNMSInt32		ActionMode,
		CNMSUIReturn	*lpRtn )
{

	if( lpRtn == CNMSNULL ){
		goto	EXIT;
	}
	DBGMSG( "[CNMSUIReturn(%s)]ResultImgSize   :%dx%d\n", ActionModeStr[ ActionMode ], lpRtn->ResultImgSize.Width, lpRtn->ResultImgSize.Height );
/*	DBGMSG( "[CNMSUIReturn(%s)]Histogram   :\n", ActionModeStr[ ActionMode ] ); */
/*	DBGMSG( "[CNMSUIReturn(%s)]Histogram   :\n", ActionModeStr[ ActionMode ] ); */

EXIT:
	return;
}


#endif	/* __CNMS_DEBUG__ */


#endif	/* _SCANMAIN_C_ */

