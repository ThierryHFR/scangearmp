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

#ifndef _SCANFLOW_C_
#define _SCANFLOW_C_

/*#define	__CNMS_DEBUG__*/
/*	#define __CNMS_DEBUG_SCANFLOW__	*/
#define __CNMS_DEBUG_SCANFLOW__
#define __CNMS_DEBUG_SCANFLOW_DETAIL__


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sane/sane.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <limits.h>

#define _SCANFLOW_GLOBALS_ /* for cnmsimgapi */
#include "cnmsfunc.h"
#include "scanflow.h"
#include "cnmsdef.h"
#include "progress_bar.h"
#include "file_control.h"
#include "child_dialog.h"
#include "com_gtk.h"
#include "w1.h"

#include "errors.h"

#include "cnmsstrings.h"

#define	CnmsProgressValue	80		/* Separated Value Scanning from ImageProcessing */

enum {
	CNMS_SCANFLOW_INIT = 0,
	CNMS_SCANFLOW_SET_OPT,
	CNMS_SCANFLOW_FILE_INIT,
	CNMS_SCANFLOW_MAKE_DST_FILE,	/* Ver.1.30 */
	CNMS_SCANFLOW_SANE_START,
	CNMS_SCANFLOW_CALIBRATION,
	CNMS_SCANFLOW_SANE_GET_PARAM,
	CNMS_SCANFLOW_PROCMOD_INIT,
	CNMS_SCANFLOW_CLEAR_PREVIEW_DATA,
	CNMS_SCANFLOW_SCAN_DATA,
	CNMS_SCANFLOW_IMG_CORRECT_FULL,
	CNMS_SCANFLOW_IMG_CORRECT_CROP,
	CNMS_SCANFLOW_GET_HISTOGRAM,
	CNMS_SCANFLOW_TERMINATE_PAGE,	/* Ver.1.30 */
	CNMS_SCANFLOW_ADD_LIST,			/* Ver.1.30 */
	CNMS_SCANFLOW_EXIT_LOOP,		/* Ver.1.30 */
	CNMS_SCANFLOW_SET_ERROR,
	CNMS_SCANFLOW_DISPOSE_DST_FILE,	/* Ver.1.30 */
	CNMS_SCANFLOW_TERMINATE,
	CNMS_SCANFLOW_MAX,
};

static CNMSBool CnmsScanFlowTable[CNMS_SCANFLOW_MAX][CNMSSCPROC_ACTION_MAX] = {
/*		SCAN		PREVIEW		PARAM		CROP		PREV_CLS			FUNCTION							*/
	{	CNMS_TRUE,	CNMS_TRUE,	CNMS_TRUE,	CNMS_TRUE,	CNMS_TRUE,	},	/*	CNMS_SCANFLOW_INIT					*/
	{	CNMS_TRUE,	CNMS_TRUE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_SET_OPT				*/
	{	CNMS_TRUE,	CNMS_TRUE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_FILE_INIT				*/
	{	CNMS_TRUE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_MAKE_DST_FILE				Ver.1.30 */
	{	CNMS_TRUE,	CNMS_TRUE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_SANE_START			*/
	{	CNMS_TRUE,	CNMS_TRUE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_CALIBRATION			*/
	{	CNMS_TRUE,	CNMS_TRUE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_SANE_GET_PARAM		*/
	{	CNMS_TRUE,	CNMS_TRUE,	CNMS_TRUE,	CNMS_TRUE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_PROCMOD_INIT			*/
	{	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_TRUE,	},	/*	CNMS_SCANFLOW_CLEAR_PREVIEW_DATA	*/
	{	CNMS_TRUE,	CNMS_TRUE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_SCAN_DATA				*/
	{	CNMS_FALSE,	CNMS_FALSE,	/* delete useless process ( CNMS_TRUE -> CNMS_FALSE ) */
								CNMS_TRUE,	CNMS_TRUE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_IMG_CORRECT_FULL		*/
	{	CNMS_TRUE,	CNMS_FALSE,	CNMS_TRUE,	CNMS_TRUE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_IMG_CORRECT_CROP		*/
	{	CNMS_FALSE,	CNMS_FALSE,	CNMS_TRUE,	CNMS_TRUE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_GET_HISTOGRAM			*/
	{	CNMS_TRUE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_TERMINATE_PAGE			Ver.1.30 */
	{	CNMS_TRUE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_ADD_LIST					Ver.1.30 */
	{	CNMS_TRUE,	CNMS_TRUE,	CNMS_TRUE,	CNMS_TRUE,	CNMS_TRUE,	},	/*	CNMS_SCANFLOW_EXIT_LOOP					Ver.1.30 */
	{	CNMS_TRUE,	CNMS_TRUE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_SET_ERROR				*/
	{	CNMS_TRUE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	CNMS_FALSE,	},	/*	CNMS_SCANFLOW_DISPOSE_DST_FILE			Ver.1.30 */
	{	CNMS_TRUE,	CNMS_TRUE,	CNMS_TRUE,	CNMS_TRUE,	CNMS_TRUE,	},	/*	CNMS_SCANFLOW_TERMINATE				*/
};


typedef struct{
	CNMSUInt8			*Data;				/* base data												*/
	CNMSUInt8			*Current;			/* base data work pointer									*/
	CNMSFd				Fd;					/* fd														*/
	CNMSByte			Path[PATH_MAX];		/* file path												*/
	CNMSInt32			PathLen;			/* length of path											*/
	CNMSPoint			InRes;				/* scanning resolution										*/
	CNMSInt32			Samples;			/* Bytes/Pixel												*/
	CNMSSize			BaseSize;			/* scanning size											*/
	CNMSInt32			Depth;				/* Bit/Pixel												*/
	CNMSDec32			GammaValue;			/* gamma value for backend									*/
	CNMSInt32			OutRes;				/* output resolution										*/

	CNMSVoid			*HistogramWork;		/* Work area for Histogram									*/
	CNMSPoint			ScanOffset;			/* SCAN,PREVIEWFscan offset (InRes)						*/

	CNMSBool			created_tmpfile;
}CNMSFdInfo;


static	CNMSFdInfo		*CnmsPrevFdInfo	= CNMSNULL;		
static	CNMSInt32		CnmsScanStatus	= CNMS_SCAN_STATUS_OPENED;
static	CNMSInt32		CnmsImgProcFlag	= FALSE;
static	CNMSInt32		cnmsimgInfoNo	= -1;

#define	CNMS_WARMINGUP_10MSEC_WAIT (4)

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__

/* #define __DEBUG_SCAN_DATA__ */
/* #define __DEBUG_HISTOGRAM__ */

static CNMSVoid DebugScanData( CNMSScanInfo *info, CNMSFd fd, CNMSInt32 bytePerLineIn, CNMSInt32 lineBufSize );
static CNMSVoid DebugHistogram( CNMSScanInfo *info, CNMSUIReturn *UiReturn );
#endif


/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowInit(
									CNMSScanInfo		**ScanInfo,
									CNMSUIInfo			*UiInfo,
									CNMSInt32			ActionMode,
									CNMSVoid			**CnmsScanWork		);

static CNMSInt32 CnmsScanInfoInit(
									CNMSScanInfo		**info				);
									
static CNMSInt32 CnmsScanInfoCpy(
									CNMSScanInfo		*dst,
									CNMSUIInfo			*src,
									CNMSInt32			ActionMode			);
static CNMSInt32 CnmsScanInfoCheckParam(
									CNMSScanInfo		*info				);


/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowSetOptionParam(
									CNMSScanInfo		*info				);

static CNMSInt32 CnmsScanFlowSubSetOptionColor(
									CNMSScanInfo		*info				);
									
static CNMSInt32 CnmsScanFlowSubSetOptionResolution(
									CNMSScanInfo		*info				);
									
static CNMSInt32 CnmsScanFlowSubCheckDataSize(
									CNMSScanInfo		*info,
									CANON_ABILITY_INFO	*ability			);
									
static CNMSInt32 CnmsScanFlowSubSetOptionScanArea(
									CNMSScanInfo		*info				);

static CNMSInt32 CnmsScanFlowSubSetOptionGammaTable(
									CNMSScanInfo		*info				);

static CNMSInt32 CnmsScanFlowSubSetOptionScannerSetting(
									CNMSScanInfo		*info				);

static CNMSInt32 CnmsScanFlowSubSetScanMethod(
									CNMSScanInfo		*info				);

/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowFileInfoInit(
									CNMSScanInfo		*info,
									CNMSFdInfo 			**ScanFdInfo		);

/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowSaneStart(
									CNMSInt32			*pages,
									CNMSScanInfo		*info				);

/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowShowCalibrationDialog(
									CNMSInt32			pages,
									CNMSScanInfo		*info				);

/*---------------------------------------------------------------------------------------*/
static CNMSVoid CnmsScanFlowGtkMainIteration(
									CNMSInt32 wait10msecCount				);

/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowWarmingUpDialog(
									CNMSBool			show,
									CNMSInt32			pages,
									CNMSScanInfo		*info				);

/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowSaneGetParameters(
									CNMSScanInfo		*info,
									SANE_Parameters		*parm				);

/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowImgProcModInit(
									CNMSScanInfo		*info,
									CNMSVoid			**CnmsScanWork		);

/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowClearPreviewData(
									CNMSFd				dstFd,
									CNMSUIReturn 		*UiReturn,
									CNMSScanInfo		*info				);

/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowScanData(
									CNMSInt32			pages,
									CNMSScanInfo		*info,
									SANE_Parameters		*parm,
									CNMSUIReturn 		*UiReturn,
									CNMSVoid			*CnmsScanWork,		
									CNMSFdInfo			*ScanFdInfo,
									CNMSFdInfo			*PrevFdInfo			);


/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowImgCorrectFull(
									CNMSFd				dstFd,
									CNMSScanInfo		*info,
									CNMSUIReturn 		*UiReturn,
									CNMSVoid			*CnmsScanWork,		
									CNMSFdInfo			*PrevFdInfo			);


/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowImgCorrectCrop(
									CNMSInt32			pages,
									CNMSFd				dstFd,
									CNMSScanInfo		*info,
									CNMSUIReturn 		*UiReturn,
									CNMSVoid			*CnmsScanWork,		
									CNMSFdInfo			*ScanFdInfo,
									CNMSFdInfo			*PrevFdInfo			);

/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowGetHistogram(
									CNMSScanInfo		*info,
									CNMSUIReturn 		*UiReturn,
									CNMSVoid			*CnmsScanWork		);

/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowSetBackendError(
									CNMSScanInfo		*info				);

/*---------------------------------------------------------------------------------------*/
static CNMSVoid CnmsScanFlowTerminate(
									CNMSScanInfo		*ScanInfo,
									CNMSVoid			**CnmsScanWork		);		/* CNMSVoid * -> CNMSVoid ** : Ver.1.30 */

/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowMakeDstFile(
									LPCNMS_NODE			*pnode				);

/*---------------------------------------------------------------------------------------*/
static CNMSInt32 CnmsScanFlowAddList(
									CNMSScanInfo		*info,
									CNMSInt32			pages,
									LPCNMS_NODE			*pnode,
									LPCNMS_ROOT			root				);

/*---------------------------------------------------------------------------------------*/
static CNMSBool CnmsScanFlowExitLoop(
									CNMSScanInfo		*info				);

/*---------------------------------------------------------------------------------------*/
static CNMSVoid CnmsScanFlowDisposeDstFile(
									LPCNMS_NODE			*pnode,
									CNMSFdInfo			*ScanFdInfo			);

/*---------------------------------------------------------------------------------------*/


typedef struct {
	CNMSInt32	api_ver;
	CNMSLPSTR	model;
	CNMSLPSTR	libname;
	CNMSInt32	net_support;		/* Ver.1.40 */
	CNMSInt32	bytes_per_pixel;	/* Ver.1.40 */
	CNMSInt32	warmingup_mess;		/* Ver.1.60 */
} CNMSIMGAPI_INFO;

static CNMSIMGAPI_INFO cnmsimg_info[] = {
	/* Ver.1.00 */
	{ 0, "MP160", "libcncpmsimg.so", 0, 3, 0 },
	{ 0, "MP510", "libcncpmsimg.so", 0, 3, 0 },
	{ 0, "MP600", "libcncpmsimg.so", 0, 3, 0 },
	/* Ver.1.10 */
	{ 0, "MP140 series", "libcncpmsimg315.so", 0, 3, 0 },
	{ 0, "MP210 series", "libcncpmsimg316.so", 0, 3, 0 },
	{ 0, "MP520 series", "libcncpmsimg328.so", 0, 3, 0 },
	{ 0, "MP610 series", "libcncpmsimg327.so", 0, 3, 0 },
	/* Ver.1.20 */
	{ 0, "MP190 series", "libcncpmsimg342.so", 0, 3, 0 },
	{ 0, "MP240 series", "libcncpmsimg341.so", 0, 6, 0 },
	{ 0, "MP540 series", "libcncpmsimg338.so", 0, 6, 0 },
	{ 0, "MP630 series", "libcncpmsimg336.so", 0, 6, 0 },
	/* Ver.1.30 */
	{ 0, "MX320 series", "libcncpmsimg348.so", 0, 6, 0 },
	{ 0, "MX330 series", "libcncpmsimg349.so", 0, 6, 0 },
	{ 0, "MX860 series", "libcncpmsimg347.so", 0, 6, 0 },
	/* Ver.1.40 */
	{ 0, "MP250 series", "libcncpmsimg356.so", 0, 6, 0 },
	{ 0, "MP270 series", "libcncpmsimg357.so", 0, 6, 0 },
	{ 0, "MP490 series", "libcncpmsimg358.so", 0, 6, 0 },
	{ 0, "MP550 series", "libcncpmsimg359.so", 0, 6, 0 },
	{ 0, "MP560 series", "libcncpmsimg360.so", 1, 6, 0 },	/* network support */
	{ 0, "MP640 series", "libcncpmsimg362.so", 1, 6, 0 },	/* network support */
	/* Ver.1.50 */
	{ 0, "MX340 series", "libcncpmsimg365.so", 1, 6, 0 },	/* network support */
	{ 0, "MX350 series", "libcncpmsimg366.so", 1, 6, 0 },	/* network support */
	{ 0, "MX870 series", "libcncpmsimg367.so", 1, 6, 0 },	/* network support */
	/* Ver.1.60 */
	{ 0, "MP280 series",	"libcncpmsimg370.so", 0, 6, 1 },
	{ 0, "MP495 series",	"libcncpmsimg369.so", 1, 6, 1 },	/* network support */
	{ 0, "MG5100 series",	"libcncpmsimg373.so", 0, 6, 1 },
	{ 0, "MG5200 series",	"libcncpmsimg374.so", 1, 6, 1 },	/* network support */
	{ 0, "MG6100 series",	"libcncpmsimg376.so", 1, 6, 1 },	/* network support */
	{ 0, "MG8100 series",	"libcncpmsimg377.so", 1, 6, 1 },	/* network support */
	/* Ver.1.70 */
	{ 0, "MX360 series", "libcncpmsimg380.so", 0, 6, 1 },
	{ 0, "MX410 series", "libcncpmsimg381.so", 1, 6, 1 },	/* network support */
	{ 0, "MX420 series", "libcncpmsimg382.so", 1, 6, 1 },	/* network support */
	{ 0, "MX880 series", "libcncpmsimg383.so", 1, 6, 1 },	/* network support */
	/* Ver.1.80 */
	{ 0, "MG2100 series",	"libcncpmsimg386.so", 0, 6, 1 },
	{ 0, "MG3100 series",	"libcncpmsimg387.so", 1, 6, 1 },	/* network support */
	{ 0, "MG4100 series",	"libcncpmsimg388.so", 1, 6, 1 },	/* network support */
	{ 0, "MG5300 series",	"libcncpmsimg389.so", 1, 6, 1 },	/* network support */
	{ 0, "MG6200 series",	"libcncpmsimg390.so", 1, 6, 1 },	/* network support */
	{ 0, "MG8200 series",	"libcncpmsimg391.so", 1, 6, 1 },	/* network support */
	{ 0, "E500 series",		"libcncpmsimg393.so", 0, 6, 1 },
	/* Ver.1.90 */
	{ 0, "MX710 series",	"libcncpmsimg394.so", 1, 6, 1 },	/* network support */
	{ 0, "MX890 series",	"libcncpmsimg395.so", 1, 6, 1 },	/* network support */
	{ 0, "MX370 series",	"libcncpmsimg396.so", 0, 6, 1 },
	{ 0, "MX430 series",	"libcncpmsimg397.so", 1, 6, 1 },	/* network support */
	{ 0, "MX510 series",	"libcncpmsimg398.so", 1, 6, 1 },	/* network support */
	{ 0, "E600 series",		"libcncpmsimg399.so", 0, 6, 1 },
	/* Ver.2.00 */
	{ 0, "MP230 series",	"libcncpmsimg401.so", 0, 6, 1 },
	{ 0, "MG2200 series",	"libcncpmsimg402.so", 0, 6, 1 },
	{ 0, "E510 series",		"libcncpmsimg403.so", 0, 6, 1 },
	{ 0, "MG3200 series",	"libcncpmsimg404.so", 1, 6, 1 },	/* network support */
	{ 0, "MG4200 series",	"libcncpmsimg405.so", 1, 6, 1 },	/* network support */
	{ 0, "MG5400 series",	"libcncpmsimg407.so", 1, 6, 1 },	/* network support */
	{ 0, "MG6300 series",	"libcncpmsimg408.so", 1, 6, 1 },	/* network support */
	/* Ver.2.10 */
	{ 0, "MX720 series",	"libcncpmsimg416.so", 1, 6, 1 },	/* network support */
	{ 0, "MX920 series",	"libcncpmsimg417.so", 1, 6, 1 },	/* network support */
	{ 0, "MX390 series",	"libcncpmsimg418.so", 0, 6, 1 },
	{ 0, "MX450 series",	"libcncpmsimg419.so", 1, 6, 1 },	/* network support */
	{ 0, "MX520 series",	"libcncpmsimg420.so", 1, 6, 1 },	/* network support */
	{ 0, "E610 series",	"libcncpmsimg421.so", 0, 6, 1 },
	/* Ver.2.20 */
	{ 0, "MG7100 series",	"libcncpmsimg423.so", 1, 6, 1 },	/* network support */
	{ 0, "MG6500 series",	"libcncpmsimg424.so", 1, 6, 1 },	/* network support */
	{ 0, "MG6400 series",	"libcncpmsimg425.so", 1, 6, 1 },	/* network support */
	{ 0, "MG5500 series",	"libcncpmsimg426.so", 1, 6, 1 },	/* network support */
	{ 0, "MG3500 series",	"libcncpmsimg427.so", 1, 6, 1 },	/* network support */
	{ 0, "MG2400 series",	"libcncpmsimg428.so", 0, 6, 1 },
	{ 0, "MG2500 series",	"libcncpmsimg429.so", 0, 6, 1 },
	{ 0, "P200 series",		"libcncpmsimg430.so", 0, 6, 1 },
	/* Ver.2.30 */
	{ 0, "MX470 series",	"libcncpmsimg434.so", 1, 6, 1 },	/* network support */
	{ 0, "MX530 series",	"libcncpmsimg435.so", 1, 6, 1 },	/* network support */
	{ 0, "E560 series",		"libcncpmsimg437.so", 1, 6, 1 },	/* network support */
	{ 0, "E400 series",		"libcncpmsimg438.so", 0, 6, 1 },
	{ -1, NULL, NULL, -1, -1, -1 },
};

CNMSInt32 CnmsGetNetworkModelInstalled( CNMSVoid )
{
	void				*handle = NULL;
	CNMSInt32			i;
	static CNMSInt32	ret = -1;
	
	if ( ret != -1 ) return ret;
	
	ret = 0;
	for(i = 0; cnmsimg_info[i].api_ver >= 0; i++) {
		if( cnmsimg_info[i].net_support ) {
			/* try dlopen */
			if( ( handle = dlopen( cnmsimg_info[i].libname, RTLD_LAZY ) ) != NULL ){
				dlclose( handle );
				ret = 1;
				break;
			}
		}
	}
	
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "[CnmsGetNetworkModelInstalled()]=%d.\n", ret );
#endif
	return ret;
}

CNMSInt32 CnmsGetNetworkSupport(
		CNMSLPSTR	lpModel )
{
	CNMSInt32			i, ret = CNMS_ERR;
	
	if( lpModel == CNMSNULL ) {
		goto EXIT;
	}
	for(i = 0; cnmsimg_info[i].api_ver >= 0; i++) {
		if( CnmsStrCompare( lpModel, cnmsimg_info[i].model ) == CNMS_NO_ERR ) {
			ret = cnmsimg_info[i].net_support;
			break;
		}
	}
	
EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "[CnmsGetNetworkModelInstalled()]=%d.\n", ret );
#endif
	return ret;
}


CNMSInt32 CnmsGetBytesPerPixel( CNMSVoid )
{
	CNMSInt32	ret = CNMS_ERR;
	
	if( cnmsimgInfoNo < 0 ){
		goto	EXIT;
	}
	ret = cnmsimg_info[cnmsimgInfoNo].bytes_per_pixel;
EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "[CnmsGetBytesPerPixel()]=%d.\n", ret );
#endif
	return ret;
}

CNMSInt32 CnmsImgApiOpen( CNMSLPSTR lpModel )
{
	CNMSInt32 ret = CNMS_ERR, api_ver = -1, i;
	CNMSIMGAPI *api = NULL;

	if( lpCnmsImgApi ) {
		DBGMSG("Error[CnmsImgApiOpen] lpCnmsImgApi != NULL\n" );
		goto _ERROR;
	}
	
	if( ( api = (CNMSIMGAPI *)CnmsGetMem( sizeof( CNMSIMGAPI ) ) ) == CNMSNULL ){
		DBGMSG( "[CnmsImgApiOpen]Error is occured in CnmsGetMem.\n" );
		goto	_ERROR;
	}
	
	for(i = 0; cnmsimg_info[i].api_ver >= 0; i++) {
		if( CnmsStrCompare( (const CNMSLPSTR)lpModel, (const CNMSLPSTR)cnmsimg_info[i].model ) == CNMS_NO_ERR ) {
			cnmsimgInfoNo = i;
			api_ver = cnmsimg_info[i].api_ver;
			break;
		}
	}
	if( api_ver < 0 ) {
		DBGMSG("Error[CnmsImgApiOpen] Can't find libname for [%s].\n", lpModel );
		goto _ERROR;
	}
	
	if ( ( api->handle = dlopen( cnmsimg_info[i].libname, RTLD_LAZY ) ) == NULL ) {
		DBGMSG("Error[CnmsImgApiOpen] dlopen( \"%s\", RTLD_LAZY ) -> %s\n", cnmsimg_info[i].libname, dlerror() );
		goto _ERROR;
	}
	
	/* dlsym */
	switch ( api_ver ) {
		case 0:
			api->p_CnmsParamSetOptionColor			= ( FP_CnmsParamSetOptionColor			)dlsym( api->handle, "CnmsParamSetOptionColor" );
			api->p_CnmsParamSetOptionResolution		= ( FP_CnmsParamSetOptionResolution		)dlsym( api->handle, "CnmsParamSetOptionResolution" );
			api->p_CnmsParamSetOptionScanArea		= ( FP_CnmsParamSetOptionScanArea		)dlsym( api->handle, "CnmsParamSetOptionScanArea" );
			api->p_CnmsParamSetOptionGammaTable		= ( FP_CnmsParamSetOptionGammaTable		)dlsym( api->handle, "CnmsParamSetOptionGammaTable" );
			
			api->p_CnmsImgProcWorkInit				= ( FP_CnmsImgProcWorkInit				)dlsym( api->handle, "CnmsImgProcWorkInit" );
			api->p_CnmsImgProcModInit				= ( FP_CnmsImgProcModInit				)dlsym( api->handle, "CnmsImgProcModInit" );
			api->p_CnmsImgProcCorrectSetBase		= ( FP_CnmsImgProcCorrectSetBase		)dlsym( api->handle, "CnmsImgProcCorrectSetBase" );
			api->p_CnmsImgProcCorrectSetFull		= ( FP_CnmsImgProcCorrectSetFull		)dlsym( api->handle, "CnmsImgProcCorrectSetFull" );
			api->p_CnmsImgProcCorrectSetCrop		= ( FP_CnmsImgProcCorrectSetCrop		)dlsym( api->handle, "CnmsImgProcCorrectSetCrop" );
			api->p_CnmsImgProcCorrectGet			= ( FP_CnmsImgProcCorrectGet			)dlsym( api->handle, "CnmsImgProcCorrectGet" );
			api->p_CnmsImgProcCorrectComplete		= ( FP_CnmsImgProcCorrectComplete		)dlsym( api->handle, "CnmsImgProcCorrectComplete" );
			api->p_CnmsImgProcTerminate				= ( FP_CnmsImgProcTerminate				)dlsym( api->handle, "CnmsImgProcTerminate" );
			
			api->p_CnmsImgProcGetHistogram			= ( FP_CnmsImgProcGetHistogram			)dlsym( api->handle, "CnmsImgProcGetHistogram" );
			api->p_CnmsImgProcOriginalDataPGet		= ( FP_CnmsImgProcOriginalDataPGet		)dlsym( api->handle, "CnmsImgProcOriginalDataPGet" );
			api->p_CnmsImgProcOriginalDataPDispose	= ( FP_CnmsImgProcOriginalDataPDispose	)dlsym( api->handle, "CnmsImgProcOriginalDataPDispose" );
			api->p_CnmsImgProcGetOriginalRGB		= ( FP_CnmsImgProcGetOriginalRGB		)dlsym( api->handle, "CnmsImgProcGetOriginalRGB" );
			
			api->p_CnmsImgMakeToneCurve				= ( FP_CnmsImgMakeToneCurve				)dlsym( api->handle, "CnmsImgMakeToneCurve" );
			api->p_CnmsImgMakeToneCurve16			= ( FP_CnmsImgMakeToneCurve16			)dlsym( api->handle, "CnmsImgMakeToneCurve16" );
			api->p_CnmsImgSplineCalc				= ( FP_CnmsImgSplineCalc				)dlsym( api->handle, "CnmsImgSplineCalc" );
			api->p_CnmsImgSplineCalc16				= ( FP_CnmsImgSplineCalc16				)dlsym( api->handle, "CnmsImgSplineCalc16" );
			
			api->p_CnmsImgScaleFromMemory			= ( FP_CnmsImgScaleFromMemory			)dlsym( api->handle, "CnmsImgScaleFromMemory" );
			break;
		default:
			goto _ERROR;
	}
	
	/* check dlsym */
	switch ( api_ver ) {
		case 0:
			if (	api->p_CnmsParamSetOptionColor			== NULL ||
					api->p_CnmsParamSetOptionResolution		== NULL ||
					api->p_CnmsParamSetOptionScanArea		== NULL ||
					api->p_CnmsParamSetOptionGammaTable		== NULL ||
					
					api->p_CnmsImgProcWorkInit				== NULL ||
					api->p_CnmsImgProcModInit				== NULL ||
					api->p_CnmsImgProcCorrectSetBase		== NULL ||
					api->p_CnmsImgProcCorrectSetFull		== NULL ||
					api->p_CnmsImgProcCorrectSetCrop		== NULL ||
					api->p_CnmsImgProcCorrectGet			== NULL ||
					api->p_CnmsImgProcCorrectComplete		== NULL ||
					api->p_CnmsImgProcTerminate				== NULL ||
					
					api->p_CnmsImgProcGetHistogram			== NULL ||
					api->p_CnmsImgProcOriginalDataPGet		== NULL ||
					api->p_CnmsImgProcOriginalDataPDispose	== NULL ||
					api->p_CnmsImgProcGetOriginalRGB		== NULL ||
					
					api->p_CnmsImgMakeToneCurve				== NULL ||
					api->p_CnmsImgMakeToneCurve16			== NULL ||
					api->p_CnmsImgSplineCalc				== NULL ||
					api->p_CnmsImgSplineCalc16				== NULL ||
					
					api->p_CnmsImgScaleFromMemory			== NULL ) {
		
				DBGMSG("Error[CnmsImgApiOpen] dlsym error.\n");
				goto _ERROR;
			}
			break;
		default:
			goto _ERROR;
	}
	
	lpCnmsImgApi = api;
	ret = CNMS_NO_ERR;

EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "[CnmsImgApiOpen(lpModel:%s)]=%d.\n", lpModel, ret );
#endif
	return ret;

_ERROR:
	if ( api ) {
		if ( api->handle ) {
			dlclose( api->handle );
		}
		CnmsFreeMem( (CNMSLPSTR)api );
	}

	goto	EXIT;
}

CNMSVoid CnmsImgApiClose( CNMSVoid )
{
	if ( lpCnmsImgApi ) {
		if ( lpCnmsImgApi->handle ) {
			dlclose( lpCnmsImgApi->handle );
		}
		CnmsFreeMem( (CNMSLPSTR)lpCnmsImgApi );
		lpCnmsImgApi = CNMSNULL;
	}
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "[CnmsImgApiClose()].\n" );
#endif
	return;
}


CNMSVoid CnmsScanFlowScanCancel(
									CNMSVoid								)
{
	CnmsScanStatus = CNMS_SCAN_STATUS_OPENED;
	CnmsImgProcFlag = FALSE;
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "[CnmsScanFlowScanCancel()].\n" );
#endif
	return;
}

CNMSVoid CnmsScanFlowPreviewFdInfoDispose(
									CNMSVoid								)
{
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowPreviewFdInfoDispose\n" );
#endif
	if( CnmsPrevFdInfo != CNMSNULL ){
		lpCnmsImgApi->p_CnmsImgProcOriginalDataPDispose( &CnmsPrevFdInfo->HistogramWork );
		CnmsFreeMem( (CNMSLPSTR)( CnmsPrevFdInfo->Data ) );
		CnmsPrevFdInfo->Data = CNMSNULL;
		CnmsFreeMem( (CNMSLPSTR)CnmsPrevFdInfo );
		
		CnmsPrevFdInfo = CNMSNULL;
	}
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "[CnmsScanFlowPreviewFdInfoDispose()].\n" );
#endif
	return;
}

CNMSInt32 CnmsScanFlowGetOriginalRGB(
									CNMSInt32			*lpCurPosR,
									CNMSInt32			*lpR,
									CNMSInt32			*lpG,
									CNMSInt32			*lpB				)
{
	CNMSInt32			x, y, ret = CNMS_ERR;
	
	x = (CNMSInt32)lpCurPosR[ CNMS_DIM_H ];
	y = (CNMSInt32)lpCurPosR[ CNMS_DIM_V ];
	
	x = ( x * CnmsPrevFdInfo->InRes.X + CnmsPrevFdInfo->OutRes / 2 ) / CnmsPrevFdInfo->OutRes;
	y = ( y * CnmsPrevFdInfo->InRes.Y + CnmsPrevFdInfo->OutRes / 2 ) / CnmsPrevFdInfo->OutRes;
	
	x -= CnmsPrevFdInfo->ScanOffset.X;
	y -= CnmsPrevFdInfo->ScanOffset.Y;
	
	ret = lpCnmsImgApi->p_CnmsImgProcGetOriginalRGB( CnmsPrevFdInfo->HistogramWork, x, y, lpR, lpG, lpB );

#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "[CnmsScanFlowGetOriginalRGB()].\n" );
#endif
	return ret;
}

CNMSInt32 CnmsScanFlowExec(
									CNMSFd				dstFd,
									LPCNMS_ROOT			root,
									CNMSUIInfo			*UiInfo,
									CNMSInt32			ActionMode,
									CNMSUIReturn 		*UiReturn			)
{
	CNMSInt32			ret 			= CNMS_NO_ERR;
	CNMSInt32			ret2 			= CNMS_NO_ERR;
	CNMSScanInfo		*ScanInfo 		= CNMSNULL;
	CNMSVoid			*CnmsScanWork 	= CNMSNULL;
	SANE_Parameters		parm;
	CNMSFdInfo			*ScanFdInfo		= CNMSNULL;
	
	LPCNMS_NODE			node			= CNMSNULL;
	CNMSFd				fd				= CNMS_FILE_ERR;
	CNMSInt32			pages			= 0;

	
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "<CnmsScanFlowExec>  ActionMode = %d\n", ActionMode );
#endif

	pages = 0;

	/* no preview data -> do nothing. */
	if( ( ActionMode == CNMSSCPROC_ACTION_PARAM ) && ( UiInfo->Preview == CNMSSCPROC_PREVIEWIMG_OFF )
		|| ( ActionMode == CNMSSCPROC_ACTION_CROP_CORRECT ) && ( UiInfo->Preview == CNMSSCPROC_PREVIEWIMG_OFF ) ){

		UiReturn->ResultImgSize.Width = 0;
		UiReturn->ResultImgSize.Height = 0;

		return ret;
	}

	/*----------------------   initialize   --------------------*/
	if ( CnmsScanFlowTable[ CNMS_SCANFLOW_INIT ][ ActionMode ] ) {
		if( ( ret = CnmsScanFlowInit( &ScanInfo, UiInfo, ActionMode, &CnmsScanWork ) ) != CNMS_NO_ERR ){
			DBGMSG("Error[CnmsScanFlowExec] CnmsUIInfoInit \n");
			goto	EXIT;
		}
	}
	
	/*----------------   set parameters   --------------*/
	if ( CnmsScanFlowTable[ CNMS_SCANFLOW_SET_OPT ][ ActionMode ] ) {
		if( ( ret = CnmsScanFlowSetOptionParam( ScanInfo ) ) != CNMS_NO_ERR ){
			DBGMSG("Error[CnmsScanFlowExec] CnmsScanFlowSetOptionParam \n");
			goto	EXIT;
		}
	}

	while(1) {
		/*------------------   initialize temp file or memory   --------------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_FILE_INIT ][ ActionMode ] ) {
			if( ( ret = CnmsScanFlowFileInfoInit( ScanInfo, &ScanFdInfo ) ) != CNMS_NO_ERR ){
				DBGMSG("Error[CnmsScanFlowExec] CnmsScanFlowFileInfoInit \n");
				goto	EXIT;
			}
		}
		
		/*------------------   make dst file   --------------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_MAKE_DST_FILE ][ ActionMode ] ) {
			if( ( ret = CnmsScanFlowMakeDstFile( &node ) ) != CNMS_NO_ERR ){
				DBGMSG("Error[CnmsScanFlowExec] CnmsScanFlowMakeDstFile \n");
				goto	EXIT;
			}
		}
		
		/*---------------------  sane_start  -------------------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_SANE_START ][ ActionMode ] ) {
			if( ( ret = CnmsScanFlowSaneStart( &pages, ScanInfo ) ) != SANE_STATUS_GOOD ){
				DBGMSG("[CnmsScanFlowExec] CnmsScanFlowSaneStart = %d\n", ret);
				if( ret == SANE_STATUS_NO_DOCS ) { /* ADF scan finished */
					ret = CNMS_NO_ERR;
				}
				goto EXIT;
			}
		}
		
		/*---------------  show calibration dialog  -----------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_CALIBRATION ][ ActionMode ] ) {
			if( ( ret = CnmsScanFlowShowCalibrationDialog( pages, ScanInfo ) ) != CNMS_NO_ERR ){
				DBGMSG("Error[CnmsScanFlowExec] CnmsScanFlowShowCalibrationDialog \n");
				goto	EXIT;
			}
		}
		
		/*-----------------  sane_get_parameters  --------------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_SANE_GET_PARAM ][ ActionMode ] ) {
			if( ( ret = CnmsScanFlowSaneGetParameters( ScanInfo, &parm ) ) != SANE_STATUS_GOOD ){
				DBGMSG("Error[CnmsScanFlowExec] CnmsScanFlowSaneGetParameters \n");
				goto	EXIT;
			}
		}
		
		/*----------------   initialize library   --------------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_PROCMOD_INIT ][ ActionMode ] ) {
			if( ( ret = CnmsScanFlowImgProcModInit( ScanInfo, &CnmsScanWork ) ) != CNMS_NO_ERR ){
				DBGMSG("Error[CnmsScanFlowExec] CnmsScanFlowImgProcModInit \n");
				goto	EXIT;
			}
		}
		
		/*------------------  create image for "clear preview" --------------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_CLEAR_PREVIEW_DATA ][ ActionMode ] ) {
			if( ( ret = CnmsScanFlowClearPreviewData( dstFd, UiReturn, ScanInfo ) ) != CNMS_NO_ERR ){
				DBGMSG("Error[CnmsScanFlowExec] CnmsScanFlowClearPreviewData \n");
				goto	EXIT;
			}
		}
		
		/*-------------------  scan image   ---------------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_SCAN_DATA ][ ActionMode ] ) {
			if( ( ret = CnmsScanFlowScanData( pages, ScanInfo, &parm, UiReturn, CnmsScanWork, ScanFdInfo, CnmsPrevFdInfo ) ) != CNMS_NO_ERR ){
				if ( ret == CNMS_SKIP_SETBACKENDERROR ) {
					/* skip CnmsScanFlowSetBackendError() */
					goto EXIT1;
				}
				else if ( ret != CNMS_CANCELED ) {
					DBGMSG("Error[CnmsScanFlowExec] CnmsScanFlowScanData \n");
				}
				goto	EXIT;
			}
		}
		
		/*----------------------  correct image (full)  ------------------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_IMG_CORRECT_FULL ][ ActionMode ] ) {
			if( ( ret = CnmsScanFlowImgCorrectFull( dstFd, ScanInfo, UiReturn, CnmsScanWork, CnmsPrevFdInfo ) ) != CNMS_NO_ERR ){
				DBGMSG("Error[CnmsScanFlowExec] CnmsScanFlowImgCorrectFull \n");
				goto	EXIT;
			}
		}
		
		/*----------------------  correct image (crop)  ------------------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_IMG_CORRECT_CROP ][ ActionMode ] ) {
			fd = ( node == CNMSNULL ) ? dstFd : node->fd;
			if( ( ret = CnmsScanFlowImgCorrectCrop( pages, fd, ScanInfo, UiReturn, CnmsScanWork, ScanFdInfo, CnmsPrevFdInfo ) ) != CNMS_NO_ERR ){
				if ( ret == CNMS_SKIP_SETBACKENDERROR ) {
					/* skip CnmsScanFlowSetBackendError() */
					goto EXIT1;
				}
				else {
					DBGMSG("Error[CnmsScanFlowExec] CnmsScanFlowImgCorrectCrop \n");
					goto	EXIT;
				}
			}
		}
		
		/*----------------------  set histogram table  ------------------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_GET_HISTOGRAM ][ ActionMode ] ) {
			if( ( ret = CnmsScanFlowGetHistogram( ScanInfo, UiReturn, CnmsScanWork ) ) != CNMS_NO_ERR ){
				DBGMSG("Error[CnmsScanFlowExec] CnmsScanFlowImgGetHistogram \n");
				goto	EXIT;
			}
		}
		
		/*----------------------  terminate page  ------------------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_TERMINATE_PAGE ][ ActionMode ] ) {
			CnmsScanFlowTerminate( CNMSNULL, &CnmsScanWork );	/* keep [ScanInfo] */
		}
		
		/*----------------------  add dst file to list  ------------------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_ADD_LIST ][ ActionMode ] ) {
			if( ( ret = CnmsScanFlowAddList( ScanInfo, pages, &node, root ) ) != CNMS_NO_ERR ){
				DBGMSG("Error[CnmsScanFlowExec] CnmsScanFlowAddList \n");
				goto	EXIT;
			}
		}
		
		/*----------------------  check exit loop  ------------------*/
		if ( CnmsScanFlowTable[ CNMS_SCANFLOW_EXIT_LOOP ][ ActionMode ] ) {
			if( CnmsScanFlowExitLoop( ScanInfo ) ) {
				break;	/* while loop exit */
			}
		}
		
	}/* end of [while(1)] */

EXIT:
	/*--------  get error of backend   ------*/
	if ( CnmsScanFlowTable[ CNMS_SCANFLOW_SET_ERROR ][ ActionMode ] ) {
		if( ( ret2 = CnmsScanFlowSetBackendError( ScanInfo ) ) != CNMS_NO_ERR ) {
			DBGMSG("Error[CnmsScanFlowExec] CnmsScanFlowGetBackendError \n");
		}
	}
EXIT1:
	/*----------------------  dispose dst file (on error)  --------------------*/
	if ( CnmsScanFlowTable[ CNMS_SCANFLOW_DISPOSE_DST_FILE ][ ActionMode ] ) {
		CnmsScanFlowDisposeDstFile( &node, ScanFdInfo );
	}
	/*----------------------  terminate  --------------------*/
	if ( CnmsScanFlowTable[ CNMS_SCANFLOW_TERMINATE ][ ActionMode ] ) {
		CnmsScanFlowTerminate( ScanInfo, &CnmsScanWork );
	}

#ifdef __CNMS_DEBUG_SCANFLOW__
	DBGMSG("[CnmsScanFlowExec()]=%d\n", ret );
#endif

	return	ret;
}



static CNMSInt32 CnmsScanFlowInit(
									CNMSScanInfo		**ScanInfo,
									CNMSUIInfo			*UiInfo,
									CNMSInt32			ActionMode,
									CNMSVoid			**CnmsScanWork		)
{
	CNMSInt32		ret = CNMS_ERR;

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowInit\n");
#endif

	/*----------------  initialize ScanInfo struct   ----------------*/
	if( ( ret = CnmsScanInfoInit( ScanInfo ) ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowInit] CnmsScanInfoInit \n");
		goto	EXIT;
	}

	/*----------  copies UiInfo -> ScanInfo  -----------*/
	if( ( ret = CnmsScanInfoCpy( *ScanInfo, UiInfo, ActionMode ) ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowInit] CnmsScanInfoCpy \n");
		goto	EXIT;
	}

	/*-------------   check ScanInfo parameters   ---------------*/
	if( ( ret = CnmsScanInfoCheckParam( *ScanInfo ) ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowInit] CnmsScanInfoCheckParam \n");
		goto	EXIT;
	}

EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowInit()]=%d.\n", ret );
#endif
	return ret;
}	

static CNMSInt32 CnmsScanInfoInit(
									CNMSScanInfo		**info				)
{
	CNMSInt32		ret = CNMS_ERR;

	if( ( *info = ( CNMSScanInfo* )CnmsGetMem( sizeof( CNMSScanInfo ) ) ) == CNMSNULL ){
		goto	EXIT;
	}
	if( ( (*info)->ModelName = ( CNMSLPSTR )CnmsGetMem( CNMSSCPROC_MODELNAME_MAX ) ) == CNMSNULL ){
		goto	EXIT;
	}

	if( ( (*info)->DatFilePath = ( CNMSLPSTR )CnmsGetMem( PATH_MAX ) ) == CNMSNULL ){
		goto	EXIT;
	}
	CnmsStrCopy( DEFTOSTR( CNMSLIBPATH ), (*info)->DatFilePath, PATH_MAX );

	if( ( (*info)->DatPFilePath = ( CNMSLPSTR )CnmsGetMem( PATH_MAX ) ) == CNMSNULL ){
		goto	EXIT;
	}
	CnmsStrCopy( DEFTOSTR( CNMSLIBPATH ), (*info)->DatPFilePath, PATH_MAX );

	if( ( (*info)->UiInfo = (CNMSVoid *)CnmsGetMem( sizeof( CNMSUIInfo ) ) ) == CNMSNULL ){
		goto	EXIT;
	}
	ret = CNMS_NO_ERR;

EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanInfoInit()]=%d.\n", ret );
#endif
	return	ret;
}

static CNMSInt32 CnmsScanInfoCpy(
									CNMSScanInfo		*dst,
									CNMSUIInfo			*src,
									CNMSInt32			ActionMode			)
{
	CNMSUIInfo		*dstUiInfo = ( CNMSUIInfo * )dst->UiInfo;
	
	CnmsStrCopy( src->ModelName, dst->ModelName, CNMSSCPROC_MODELNAME_MAX );
	
	dst->Method				= src->Method;
	dst->Preview			= src->Preview;
	
	dst->ActionMode			= ActionMode;
	
	dst->MediaType 			= src->MediaType;
	dst->MediaSize 			= src->MediaSize;
	dst->ColorMode 			= src->ColorMode;
	dst->OutRes 			= src->OutRes;
	dst->InSize				= src->InSize;
	dst->OutSize		 	= src->OutSize;
	dst->InOffset	 		= src->InOffset;
	dst->ImgSetFlagUi		= src->ImgSetFlagUi;
	dst->ImgSetFlagUi2		= src->ImgSetFlagUi2;
	dst->UserTone			= src->UserTone;
	dst->ColSetting			= src->ColSetting;
	dst->Threshold			= src->Threshold;
	dst->ThresholdDefault	= src->ThresholdDefault;
	dst->MonGamma			= src->MonGamma;
	dst->SensorType			= src->Ability.sensor_type;	/* add Ver.1.60 */
	
	*dstUiInfo				= *src;
	
	return CNMS_NO_ERR;
}

static CNMSInt32 CnmsScanInfoCheckParam(
									CNMSScanInfo		*info				)
{
	CNMSUIInfo			*uiInfo		= ( CNMSUIInfo * )info->UiInfo;
	CANON_ABILITY_INFO	*ability	= &uiInfo->Ability;
 
	if( info->ModelName == CNMSNULL )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] ModelName (%p)\n",info->ModelName);
		return 	CNMS_ERR;
	}

	if( info->Method < 0 )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] Method (%d)\n",info->Method);
		return 	CNMS_ERR;
	}
	
	if(		( info->Preview != CNMSSCPROC_PREVIEWIMG_OFF )
		 && ( info->Preview != CNMSSCPROC_PREVIEWIMG_ON ) )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] Preview (%d)\n",info->Preview);
		return 	CNMS_ERR;
	}
	
	if(		( info->ActionMode != CNMSSCPROC_ACTION_SCAN )
		 && ( info->ActionMode != CNMSSCPROC_ACTION_PREVIEW )
		 && ( info->ActionMode != CNMSSCPROC_ACTION_PARAM )
		 && ( info->ActionMode != CNMSSCPROC_ACTION_CROP_CORRECT )
		 && ( info->ActionMode != CNMSSCPROC_ACTION_PREV_CLEAR ) )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] ActionMode (%d)\n",info->ActionMode);
		return 	CNMS_ERR;
	}
	
	if( ( info->MediaType != CNMSSCPROC_MTYPE_PAPER ) )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] MediaType (%d)\n",info->MediaType);
		return 	CNMS_ERR;
	}
	
	if( ( info->MediaSize != CNMSSCPROC_MSIZE_FULLPAGE ) )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] MediaSize (%d)\n",info->MediaSize);
		return 	CNMS_ERR;
	}
	
	if(		( info->ColorMode != CNMSSCPROC_CMODE_COLOR )
		 && ( info->ColorMode != CNMSSCPROC_CMODE_COLOR_TEXT )
		 && ( info->ColorMode != CNMSSCPROC_CMODE_GRAY )
		 && ( info->ColorMode != CNMSSCPROC_CMODE_MONO ) )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] ColorMode (%d)\n",info->ColorMode);
		return 	CNMS_ERR;
	}
	
	if(		( info->OutRes != CNMSSCPROC_OUTPUTRES_0075 )
		 && ( info->OutRes != CNMSSCPROC_OUTPUTRES_0100 )
		 && ( info->OutRes != CNMSSCPROC_OUTPUTRES_0150 )
		 && ( info->OutRes != CNMSSCPROC_OUTPUTRES_0200 )
		 && ( info->OutRes != CNMSSCPROC_OUTPUTRES_0300 )
		 && ( info->OutRes != CNMSSCPROC_OUTPUTRES_0400 )
		 && ( info->OutRes != CNMSSCPROC_OUTPUTRES_0600 )
		 && ( info->OutRes != CNMSSCPROC_OUTPUTRES_1200 ) )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] OutRes (%d)\n",info->OutRes);
		return 	CNMS_ERR;
	}

	if( ( info->InSize.Width < 0 ) || ( info->InSize.Height < 0 ) )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] InSize.Width (%d) .Height (%d)\n",info->InSize.Width,info->InSize.Height);
		return 	CNMS_ERR;
	}

	if( ( info->OutSize.Width < 0 ) || ( info->OutSize.Height < 0 ) )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] OutSize.Width (%d) .Height (%d)\n",info->OutSize.Width,info->OutSize.Height);
		return 	CNMS_ERR;
	}

	if( ( info->InOffset.X < 0 ) || ( info->InOffset.Y < 0 ) )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] OutSize.X (%d) .Y (%d)\n",info->InOffset.X,info->InOffset.Y);
		return 	CNMS_ERR;
	}

	if ( info->ColorMode != CNMSSCPROC_CMODE_MONO ) {
		if( ( info->ImgSetFlagUi.USM != CNMSSCPROC_FLAG_OFF ) && ( info->ImgSetFlagUi.USM != CNMSSCPROC_FLAG_ON ) )
		{
			DBGMSG("Error[CnmsScanInfoCheckParam] ImgSetFlagUi.USM (%d)\n",info->ImgSetFlagUi.USM);
			return 	CNMS_ERR;
		}
		if( ( info->ImgSetFlagUi2.USM != CNMSSCPROC_FLAG_OFF ) && ( info->ImgSetFlagUi2.USM != CNMSSCPROC_FLAG_ON ) )
		{
			DBGMSG("Error[CnmsScanInfoCheckParam] ImgSetFlagUi2.USM (%d)\n",info->ImgSetFlagUi2.USM);
			return 	CNMS_ERR;
		}
		if( W1_JudgeFormatType( CNMS_OBJ_A_DESCREEN ) == CNMS_TRUE ){
			if( ( info->ImgSetFlagUi2.Descreen != CNMSSCPROC_FLAG_OFF ) && ( info->ImgSetFlagUi2.Descreen != CNMSSCPROC_FLAG_ON ) )
			{
				DBGMSG("Error[CnmsScanInfoCheckParam] ImgSetFlagUi2.Descreen (%d)\n",info->ImgSetFlagUi2.Descreen);
				return 	CNMS_ERR;
			}
		}
	}

	if( ( info->Threshold < 0 ) || ( info->Threshold > 255 ) )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] Threshold (%d)\n",info->Threshold);
		return 	CNMS_ERR;
	}

	if( info->MonGamma < 0 )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] MonGamma (%f)\n",info->MonGamma);
		return 	CNMS_ERR;
	}

	if(		( info->ColSetting != CNMSSCPROC_CSET_RECOMMEND )
		 && ( info->ColSetting != CNMSSCPROC_CSET_NON_CORRECT ) )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] ColSetting (%d)\n",info->ColSetting);
		return 	CNMS_ERR;
	}
	
	if ( uiInfo == CNMSNULL )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] UiInfo (%p)\n",uiInfo);
		return 	CNMS_ERR;
	}
	
	if( uiInfo->Calibration == CNMSNULL )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] Calibration (%p)\n",uiInfo->Calibration);
		return 	CNMS_ERR;
	}
	
	if( uiInfo->Hand < 0 )
	{
		DBGMSG("Error[CnmsScanInfoCheckParam] Hand (%p)\n",uiInfo->Hand);
		return 	CNMS_ERR;
	}

	/* check useable funtion of preference */
	if( ( W1_JudgeFormatType( CNMS_OBJ_P_EVERY_CALIBRATION ) == CNMS_TRUE ) && ( ability->every_calibration == CANON_ABILITY_DISABLE ) ){
		DBGMSG("Error[CnmsScanInfoCheckParam] JudgeFormatType (%d), every_calibration (%d)\n",W1_JudgeFormatType( CNMS_OBJ_P_EVERY_CALIBRATION ),ability->every_calibration );
		return 	CNMS_ERR;
	}
	/* Ver.1.80 skip check silent */
	/*
	if( ( W1_JudgeFormatType( CNMS_OBJ_P_SILENT_MODE ) == CNMS_TRUE ) && ( ability->silent_mode == CANON_ABILITY_DISABLE ) ){
		DBGMSG("Error[CnmsScanInfoCheckParam] JudgeFormatType (%d), silent_mode (%d)\n",W1_JudgeFormatType( CNMS_OBJ_P_SILENT_MODE ),ability->silent_mode );
		return 	CNMS_ERR;
	}
	*/

	return CNMS_NO_ERR;
}


static CNMSInt32 CnmsScanFlowSetOptionParam(
									CNMSScanInfo		*info				)
{
	CNMSInt32	ret = CNMS_ERR;

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowSetOptionParam\n");
#endif

	/* set scanning color */
	if( ( ret = CnmsScanFlowSubSetOptionColor( info ) ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowSetOptionParam] CnmsScanFlowSubSetOptionColor \n");
		goto	EXIT;
	}

	/* set scanning resolution */
	if( ( ret = CnmsScanFlowSubSetOptionResolution( info ) ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowSetOptionParam] CnmsScanFlowSubSetOptionResolution \n");
		goto	EXIT;
	}
	
	/* set scanning area */
	if( ( ret = CnmsScanFlowSubSetOptionScanArea( info ) ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowSetOptionParam] CnmsScanFlowSubSetOptionScanArea \n");
		goto	EXIT;
	}
		
	/* set gamma table for backend */
	if( ( ret = CnmsScanFlowSubSetOptionGammaTable( info ) ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowSetOptionParam] CnmsScanFlowSubSetOptionGammaTable \n");
		goto	EXIT;
	}

	/* set scanner setting */
	if( ( ret = CnmsScanFlowSubSetOptionScannerSetting( info ) ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowSetOptionParam] CnmsScanFlowSubSetOptionScannerSetting \n");
		goto	EXIT;
	}

	/* set scan method */
	if( ( ret = CnmsScanFlowSubSetScanMethod( info ) ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowSetOptionParam] CnmsScanFlowSubSetScanMethod \n");
		goto	EXIT;
	}

EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowSetOptionParam()]=%d.\n", ret );
#endif
	return ret;
}

static CNMSInt32 CnmsScanFlowSubSetOptionColor(
									CNMSScanInfo		*info				)
{
	CNMSInt32		ret = CNMS_ERR, ldata;
	SANE_Status		status;
	CNMSLPSTR		lpStr = CNMSNULL, lpStr2;
	CNMSUIInfo		*uiInfo = ( CNMSUIInfo * )info->UiInfo;
	LPCNMSOPTSNUM	lpOpts = &( uiInfo->OptsNum );
	
	if( ( lpStr = ( CNMSLPSTR )CnmsGetMem( 128 ) ) == CNMSNULL ){
		DBGMSG( "[CnmsScanFlowSubSetOptionColor]Error is occured in CnmsGetMem.\n" );
		goto	EXIT;
	}
	lpStr2 = lpStr + 64;
	
	/* color or gray */
	switch( lpCnmsImgApi->p_CnmsParamSetOptionColor( info ) )
	{
		case	CNMSSCPROC_OPT_COLOR_COLOR:
					if( ( ldata = CnmsStrCopy( "Color\0", lpStr, 64 ) ) < 0 ){
						goto	EXIT;
					}
					info->Samples = 3;
					break;
		default:	/* CNMSSCPROC_OPT_COLOR_GRAY */
					if( ( ldata = CnmsStrCopy( "Gray\0", lpStr, 64 ) ) < 0 ){
						goto	EXIT;
					}
					info->Samples = 1;
					break;
	}

	/* set */
	if( ( status = sane_control_option( uiInfo->Hand, lpOpts->scanMode, SANE_ACTION_SET_VALUE, lpStr, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionColor] Can't set color( %s ).\n", lpStr );
		goto	EXIT;
	}
	else if( ( status = sane_control_option( uiInfo->Hand, lpOpts->scanMode, SANE_ACTION_GET_VALUE, lpStr2, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionColor] Can't get color( %s ).\n", lpStr2 );
		goto	EXIT;
	}
	else if( ( ldata = CnmsStrCompare( lpStr, lpStr2 ) ) != CNMS_NO_ERR ){
		DBGMSG("[CnmsScanFlowSubSetOptionColor]Can't set color( set:%s, get:%s).\n", lpStr, lpStr2 );
		goto	EXIT;
	}
	ret = CNMS_NO_ERR;
EXIT:
	if( lpStr != CNMSNULL ){
		CnmsFreeMem( lpStr );
	}
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowSubSetOptionColor()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSInt32 CnmsScanFlowSetOptionResolution(
									CNMSScanInfo		*info,
									CANON_ABILITY_INFO	*ability			)
{
	CNMSInt32		ret = CNMS_ERR;
	
	/* get scanning resolution */
	if( lpCnmsImgApi->p_CnmsParamSetOptionResolution( info, ability ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowSetOptionResolution] CnmsParamSetOptionResolution\n");
		goto	EXIT;
	}
	
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG("[CnmsScanFlowSetOptionResolution] resX,Y = ( %d, %d ).\n", info->InRes.X, info->InRes.Y );
#endif

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowSetOptionResolution()]=%d.\n", ret );
#endif
	return	ret;
}


static CNMSInt32 CnmsScanFlowSubSetOptionResolution(
									CNMSScanInfo		*info				)
{
	CNMSInt32		ret = CNMS_ERR;
	SANE_Int		bind = SANE_FALSE, in, out;
	SANE_Status		status;
	CNMSUIInfo		*uiInfo = ( CNMSUIInfo * )info->UiInfo;
	LPCNMSOPTSNUM	lpOpts = &( uiInfo->OptsNum );
	CANON_ABILITY_SUB_INFO	*lpInfo;
	CANON_ABILITY_INFO		*ability = &uiInfo->Ability;

	/* set bind */
	if( ( status = sane_control_option( uiInfo->Hand, lpOpts->resBind, SANE_ACTION_SET_VALUE, &bind, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionResolution]Can't set bind(FALSE).\n" );
		goto	EXIT;
	}
	else if( ( status = sane_control_option( uiInfo->Hand, lpOpts->resBind, SANE_ACTION_GET_VALUE, &bind, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionResolution]Can't get bind.\n" );
		goto	EXIT;
	}
	
	/* get scanning resolution */
	if( lpCnmsImgApi->p_CnmsParamSetOptionResolution( info, ability ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowSubSetOptionResolution] CnmsParamSetOptionResolution\n");
		goto	EXIT;
	}
	if( CnmsScanFlowSubCheckDataSize( info, ability ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowSubSetOptionResolution] CnmsScanFlowSubCheckDataSize\n");
		goto	EXIT;
	}

	/* set horizontal resolution */
	in = (SANE_Int)( info->InRes.X );
	if( ( status = sane_control_option( uiInfo->Hand, lpOpts->resX, SANE_ACTION_SET_VALUE, &in, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionResolution]Can't set resX.\n" );
		goto	EXIT;
	}
	else if( ( status = sane_control_option( uiInfo->Hand, lpOpts->resX, SANE_ACTION_GET_VALUE, &out, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionResolution]Can't get resX.\n" );
		goto	EXIT;
	}
	if( in != out ){
		DBGMSG("[CnmsScanFlowSubSetOptionResolution]Can't set resX( set:%d, get:%d ).\n", in, out );
		goto	EXIT;
	}
	
	/* set vertical resolution */
	if( bind == SANE_FALSE )
	{
		in = (SANE_Int)( info->InRes.Y );
		if( ( status = sane_control_option( uiInfo->Hand, lpOpts->resY, SANE_ACTION_SET_VALUE, &in, CNMSNULL ) ) != SANE_STATUS_GOOD ){
			DBGMSG("[CnmsScanFlowSubSetOptionResolution]Can't set resY.\n" );
			goto	EXIT;
		}
		else if( ( status = sane_control_option( uiInfo->Hand, lpOpts->resY, SANE_ACTION_GET_VALUE, &out, CNMSNULL ) ) != SANE_STATUS_GOOD ){
			DBGMSG("[CnmsScanFlowSubSetOptionResolution]Can't get resY.\n" );
			goto	EXIT;
		}
		if( in != out ){
			DBGMSG("[CnmsScanFlowSubSetOptionResolution]Can't set resY( set:%d, get:%d ).\n", in, out );
			goto	EXIT;
		}
	}
	
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG("[CnmsScanFlowSubSetOptionResolution] resX,Y = ( %d, %d ).\n", info->InRes.X, info->InRes.Y );
#endif


	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowSubSetOptionResolution()]=%d.\n", ret );
#endif
	return	ret;
}

static CNMSInt32 CnmsScanFlowSubCheckDataSize(
									CNMSScanInfo		*info,
									CANON_ABILITY_INFO	*ability			)
{
	CNMSInt32		ret = CNMS_ERR,
					i;

	if( ( info == CNMSNULL ) || ( ability == CNMSNULL ) ){
		DBGMSG("[CnmsScanFlowSubCheckDataSize]Input parameter error. info = %p, ability = %p.\n", info, ability );
		goto	EXIT;
	}

	/* Check file size limit 2GB */
	if( ( ( ( info->InRes.X * info->InSize.Width / info->OutRes ) * 3 ) >= ( 0x40000000L ) / ( info->InRes.X * info->InSize.Height / info->OutRes ) ) ||
		( ( ( info->InRes.Y * info->InSize.Width / info->OutRes ) * 3 ) >= ( 0x40000000L ) / ( info->InRes.Y * info->InSize.Height / info->OutRes ) ) ){
		/* do nothing. */
	}
	else{
		ret = CNMS_NO_ERR;
	}
	
EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowSubCheckDataSize()]=%d.\n", ret );
#endif
	return ret;
}

static CNMSInt32 CnmsScanFlowSubSetOptionScanArea(
									CNMSScanInfo		*info				)
{
	CNMSInt32				ret = CNMS_ERR, ldata;
	SANE_Int				right, bottom, left, top, out;
	SANE_Status				status;
	CNMSUIInfo				*uiInfo = ( CNMSUIInfo * )info->UiInfo;
	LPCNMSOPTSNUM			lpOpts = &( uiInfo->OptsNum );
	CANON_ABILITY_INFO		*ability = &uiInfo->Ability;

	/* check */
	if( ( info->Method >= CANON_ABILITY_SUB_INFO_MAX ) || ( info->InSize.Width <= 0 ) || ( info->InSize.Height <= 0 )
		|| ( info->InOffset.X < 0 ) || ( info->InOffset.Y < 0 ) ){
		DBGMSG( "[CnmsScanFlowSubSetOptionScanArea]parameter is error.\n" );
		goto	EXIT;
	}

	if( lpCnmsImgApi->p_CnmsParamSetOptionScanArea( info, ability ) != CNMS_NO_ERR ){
		DBGMSG("[CnmsScanFlowSubSetOptionScanArea] error in CnmsParamSetOptionScanArea\n");
		goto	EXIT;
	}
	
	left	= (SANE_Int)( info->ScanOffset.X );
	top		= (SANE_Int)( info->ScanOffset.Y );
	right	= (SANE_Int)( info->BaseSize.Width + info->ScanOffset.X );
	bottom	= (SANE_Int)( info->BaseSize.Height + info->ScanOffset.Y );

	/* left */
	if( ( status = sane_control_option( uiInfo->Hand, lpOpts->pixel_tl_X, SANE_ACTION_SET_VALUE, &left, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionScanArea] Can't set horizontal offset.\n" );
		goto	EXIT;
	}
	else if( ( status = sane_control_option( uiInfo->Hand, lpOpts->pixel_tl_X, SANE_ACTION_GET_VALUE, &out, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionScanArea] Can't get horizontal offset.\n" );
		goto	EXIT;
	}
	else if( left != out ){
		DBGMSG("[CnmsScanFlowSubSetOptionScanArea] Can't set horizontal offset( set:%d, get:%d ).\n", left, out );
		goto	EXIT;
	}
	/* top */
	if( ( status = sane_control_option( uiInfo->Hand, lpOpts->pixel_tl_Y, SANE_ACTION_SET_VALUE, &top, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionScanArea] Can't set vertical offset.\n" );
		goto	EXIT;
	}
	else if( ( status = sane_control_option( uiInfo->Hand, lpOpts->pixel_tl_Y, SANE_ACTION_GET_VALUE, &out, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionScanArea] Can't get vertical offset.\n" );
		goto	EXIT;
	}
	else if( top != out ){
		DBGMSG("[CnmsScanFlowSubSetOptionScanArea] Can't set vertical offset( set:%d, get:%d ).\n", top, out );
		goto	EXIT;
	}
	/* right */
	if( ( status = sane_control_option( uiInfo->Hand, lpOpts->pixel_br_X, SANE_ACTION_SET_VALUE, &right, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionScanArea] Can't set right end.\n" );
		goto	EXIT;
	}
	else if( ( status = sane_control_option( uiInfo->Hand, lpOpts->pixel_br_X, SANE_ACTION_GET_VALUE, &out, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionScanArea] Can't get right end.\n" );
		goto	EXIT;
	}
	else if( right != out ){
		DBGMSG("[CnmsScanFlowSubSetOptionScanArea] Can't set right( set:%d, get:%d ).\n", right, out );
		goto	EXIT;
	}
	/* bottom */
	if( ( status = sane_control_option( uiInfo->Hand, lpOpts->pixel_br_Y, SANE_ACTION_SET_VALUE, &bottom, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionScanArea] Can't get bottom end.\n" );
		goto	EXIT;
	}
	else if( ( status = sane_control_option( uiInfo->Hand, lpOpts->pixel_br_Y, SANE_ACTION_GET_VALUE, &out, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionScanArea] Can't get bottom end.\n" );
		goto	EXIT;
	}
	else if( bottom != out ){
		DBGMSG("[CnmsScanFlowSubSetOptionScanArea] Can't set bottom( set:%d, get:%d ).\n", bottom, out );
		goto	EXIT;
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowSubSetOptionScanArea()]=%d.\n", ret );
#endif
	return	ret;
}

static CNMSInt32 CnmsScanFlowSubSetOptionGammaTable(
									CNMSScanInfo		*info				)
{
	CNMSInt32		ret = CNMS_ERR;
	SANE_Status		status;
	CNMSUIInfo		*uiInfo = ( CNMSUIInfo * )info->UiInfo;
	LPCNMSOPTSNUM	lpOpts = &( uiInfo->OptsNum );

	/* get gamma table */
	if( lpCnmsImgApi->p_CnmsParamSetOptionGammaTable( info ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowSubSetOptionGammaTable] CnmsParamSetOptionGammaTable\n");
		goto	EXIT;
	}
	
	/* set gamma */
	if( ( status = sane_control_option( uiInfo->Hand, lpOpts->setGamma, SANE_ACTION_SET_VALUE, info->GammaTable, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionGammaTable]Can't set setGamma.\n" );
		goto	EXIT;
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowSubSetOptionGammaTable()]=%d.\n", ret );
#endif
	return	ret;
}


static CNMSInt32 CnmsScanFlowSubSetOptionScannerSetting(
									CNMSScanInfo		*info				)
{
	CNMSInt32		ret = CNMS_ERR;
	SANE_Status		status;
	CNMSUIInfo		*uiInfo = ( CNMSUIInfo * )info->UiInfo;
	LPCNMSOPTSNUM	lpOpts = &( uiInfo->OptsNum );

	/* set silent mode */
	if( W1_JudgeFormatType( CNMS_OBJ_P_SILENT_MODE ) == CNMS_TRUE ){
		if( ( status = sane_control_option( uiInfo->Hand, lpOpts->setSilent, SANE_ACTION_SET_VALUE, &(uiInfo->SilentMode), CNMSNULL ) ) != SANE_STATUS_GOOD ){
			DBGMSG("[CnmsScanFlowSubSetOptionScannerSetting]Can't set SilentMode.\n" );
			goto	EXIT;
		}
	}

	/* set every calibration setting */
	if( W1_JudgeFormatType( CNMS_OBJ_P_EVERY_CALIBRATION ) == CNMS_TRUE ){
		if( ( status = sane_control_option( uiInfo->Hand, lpOpts->setEveryCalibration, SANE_ACTION_SET_VALUE, &(uiInfo->EveryCalibration), CNMSNULL ) ) != SANE_STATUS_GOOD ){
			DBGMSG("[CnmsScanFlowSubSetOptionScannerSetting]Can't set CalibrationSetting.\n" );
			goto	EXIT;
		}
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowSubSetOptionScannerSetting()]=%d.\n", ret );
#endif
	return	ret;
}


static CNMSInt32 CnmsScanFlowSubSetScanMethod(
									CNMSScanInfo		*info				)
{
	CNMSInt32		ret = CNMS_ERR;
	SANE_Status		status;
	CNMSUIInfo		*uiInfo = ( CNMSUIInfo * )info->UiInfo;
	LPCNMSOPTSNUM	lpOpts = &( uiInfo->OptsNum );

	/* set ScanMethod */
	if( ( status = sane_control_option( uiInfo->Hand, lpOpts->scanMethod, SANE_ACTION_SET_VALUE, &(uiInfo->Method), CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowSubSetOptionScannerSetting]Can't set CalibrationSetting.\n" );
		goto	EXIT;
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowSubSetScanMethod()]=%d.\n", ret );
#endif
	return	ret;
}


static CNMSInt32 CnmsScanFlowFileInfoInit(
									CNMSScanInfo		*info,
									CNMSFdInfo 			**ScanFdInfo				)
{
	CNMSInt32		ret = CNMS_ERR;

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowFileInfoInit\n");
#endif

	if( info->ActionMode == CNMSSCPROC_ACTION_SCAN ){
		if( *ScanFdInfo == CNMSNULL ) {
			if( ( *ScanFdInfo = ( CNMSFdInfo* )CnmsGetMem( sizeof( CNMSFdInfo ) ) ) == CNMSNULL ){
				DBGMSG("Error[CnmsScanFlowFileInfoInit] CnmsGetMem [ScanFdInfo] is error!\n");
				goto	EXIT;
			}
		}

		(*ScanFdInfo)->created_tmpfile = CNMS_FALSE;
		if( ( (*ScanFdInfo)->Fd = FileControlMakeTempFile( (*ScanFdInfo)->Path, PATH_MAX ) ) == CNMS_FILE_ERR ){
			DBGMSG( "[CnmsScanFlowFileInfoInit]Error is occured in FileControlMakeTempFile.\n" );
			goto	EXIT;
		}

		(*ScanFdInfo)->InRes = info->InRes;
		(*ScanFdInfo)->BaseSize = info->BaseSize;
		(*ScanFdInfo)->Samples = info->Samples;
		(*ScanFdInfo)->GammaValue = info->GammaValue;
		(*ScanFdInfo)->created_tmpfile = CNMS_TRUE;

	}
	else if( info->ActionMode == CNMSSCPROC_ACTION_PREVIEW ){
		CnmsScanFlowPreviewFdInfoDispose();
		
		if( ( CnmsPrevFdInfo = ( CNMSFdInfo* )CnmsGetMem( sizeof( CNMSFdInfo ) ) ) == CNMSNULL ){
			DBGMSG("Error[CnmsScanFlowFileInfoInit] CnmsGetMem [CnmsPrevFdInfo] is error!\n");
			goto	EXIT;
		}
		CnmsPrevFdInfo->InRes = info->InRes;
		CnmsPrevFdInfo->BaseSize = info->BaseSize;
		CnmsPrevFdInfo->Samples = info->Samples;
		CnmsPrevFdInfo->GammaValue = info->GammaValue;
		
		CnmsPrevFdInfo->HistogramWork = CNMSNULL;
		CnmsPrevFdInfo->created_tmpfile = CNMS_FALSE;
	}
	
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowFileInfoInit()]=%d.\n", ret );
#endif
	return	ret;
}


static CNMSInt32 CnmsScanFlowSaneStart(
									CNMSInt32			*pages,
									CNMSScanInfo		*info				)
{
	CNMSUIInfo		*uiInfo = ( CNMSUIInfo * )info->UiInfo;
	CNMSInt32		ret = SANE_STATUS_GOOD;
	CNMSInt32		ret2 = -1;
	
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowSaneStart\n");
#endif


	do {
		/* hide progress dialog and show warmingup dialog */
		CnmsScanFlowWarmingUpDialog( CNMS_TRUE, *pages, info );
	
		if( ( ret = sane_start( uiInfo->Hand ) ) != SANE_STATUS_GOOD ) {
			/* NO_DOCS check */
			if( ret == SANE_STATUS_NO_DOCS ) {
				/* first page */
				if ( *pages == 0 ) {
					/* hide warmingup dialog */
					if ( ( ret2 = CnmsScanFlowWarmingUpDialog( CNMS_FALSE, *pages, info ) ) == CNMS_FALSE ) {
						/* hide progress dialog */
						ProgressBarHide();
					}
					/* show NO PAPER dialog */
					if( ( ret2 = ChildDialogShow( CHILDDIALOG_ID_NO_PAPER, CNMSNULL ) ) != COMGTK_GTKMAIN_OK ) {
						/* scan canceled */
						sane_cancel( uiInfo->Hand );
						ret = CNMS_CANCELED;
						goto EXIT;
					}
					/* show progress dialog */
					ProgressBarShow();
				}
				else {
					/* *pages > 1 : end of document -> scan finished */
					sane_cancel( uiInfo->Hand );
					goto EXIT;
				}
			}
			/* other error */
			else {
				/* hide warmingup dialog */
				CnmsScanFlowWarmingUpDialog( CNMS_FALSE, *pages, info );
				
				/* need to call sane_cancel in CnmsScanFlowSetBackendError */
				CnmsScanStatus = CNMS_SCAN_STATUS_STARTED;
				DBGMSG("[CnmsScanFlowSaneStart] error in sane_start \n");
				goto	EXIT;
			}
		}
		else {	/* ret == SANE_STATUS_GOOD */
			*pages += 1;
			break;
		}
	} while( ret == SANE_STATUS_NO_DOCS && *pages == 0 );

	/* ret = SANE_STATUS_GOOD */
	CnmsScanStatus = CNMS_SCAN_STATUS_STARTED;
EXIT:
	return	ret;
}

static CNMSInt32 CnmsScanFlowShowCalibrationDialog(
									CNMSInt32			pages,
									CNMSScanInfo		*info				)
{
	CNMSInt32		ret = CNMS_ERR;
	CNMSUIInfo		*uiInfo = ( CNMSUIInfo * )info->UiInfo;
	LPCNMSOPTSNUM	lpOpts = &( uiInfo->OptsNum );

	GtkWidget		*calibration_label = CNMSNULL;
	char			*calibration_label_str = MSG_EVERY_CALIBRATION_ON;
	
	CNMSInt32		calibration;
	SANE_Status		status;
	
	CNMSInt32		ret2 = -1;
	CNMSInt32		CCD = ( uiInfo->Ability.sensor_type == CANON_ABILITY_SENSOR_TYPE_CCD ) ? CNMS_TRUE : CNMS_FALSE ;
	
	struct	timespec mytime;
	CnmsSetMem( (CNMSLPSTR)&mytime, 0, sizeof(mytime) );
	mytime.tv_sec   = 0;
	mytime.tv_nsec  = 100000000;	/* 100msec */

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowShowCalibrationDialog\n");
#endif

	if ( uiInfo->Calibration == NULL ) {
		DBGMSG("Error[CnmsScanFlowShowCalibrationDialog] calibration_dialog is NULL\n");
		goto	EXIT;
	}
	else if ( ( calibration_label = (GtkWidget *)lookup_widget( uiInfo->Calibration, "dialog_calibration_label" ) ) == CNMSNULL ){
		DBGMSG( "[CnmsScanFlowShowCalibrationDialog]Can't look up widget(dialog_calibration_label).\n" );
		goto	EXIT;
	}

	/* set dialog_calibration_label */
	if( cnmsimg_info[cnmsimgInfoNo].warmingup_mess ) {
		calibration_label_str = MSG_EVERY_CALIBRATION_OFF;
	}
	else if( W1_JudgeFormatType( CNMS_OBJ_P_EVERY_CALIBRATION ) == CNMS_TRUE ){
		if ( uiInfo->EveryCalibration == CNMS_FALSE ) {		/* Every calibration is OFF. */
			calibration_label_str = MSG_EVERY_CALIBRATION_OFF;
		}
	}
	
	/* wait 100msec */
	nanosleep( &mytime, NULL );
	
	/* check calibration status */
	if( ( status = sane_control_option( uiInfo->Hand, lpOpts->getCalStatus, SANE_ACTION_GET_VALUE, &calibration, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG("[CnmsScanFlowShowCalibrationDialog] Can't get calibration status.\n" );
		goto EXIT;
	}
	
	if ( !calibration ) {
		/* not necessary to show calibration dialog -> hide warmingup dialog */
		if ( ( ret2 = CnmsScanFlowWarmingUpDialog( CNMS_FALSE, pages - 1, info ) ) == CNMS_TRUE ) {
			/* show progress dialog */
			ProgressBarShow();
		}
		ret = CNMS_NO_ERR;
		goto EXIT;
	}
	
	if ( pages == 1 ) {
		/* CCD : warmingup dialog(= calibration dialog) is already showed */
		if ( !CCD ) {
			/* hide progress dialog */
			ProgressBarHide();
			
			gtk_label_set_text( GTK_LABEL( calibration_label ), gettext( calibration_label_str ) );
			/* show calibration dialog */
			W1_ModalDialogShowAction( uiInfo->Calibration, main_window );
		}
	}
	else {
		ProgressBarUpdate( 0, pages );
	}
	
	/* wait calibration */
	while(1) {
		if ( pages == 1 ) {
			while( gtk_events_pending() ){
				gtk_main_iteration();
			}
		}
		nanosleep( &mytime, NULL );
		
		if( ( status = sane_control_option( uiInfo->Hand, lpOpts->getCalStatus, SANE_ACTION_GET_VALUE, &calibration, CNMSNULL ) ) != SANE_STATUS_GOOD ){
			DBGMSG("[CnmsScanFlowShowCalibrationDialog] Can't get calibration status.\n" );
			break;
		}
		if ( !calibration ) {
			ret = CNMS_NO_ERR;
			break;
		}
	}

	if ( pages == 1 ) {
		/* hide calibration dialog */
		W1_ModalDialogHideAction( uiInfo->Calibration, CNMSNULL );
		while( gtk_events_pending() ){
			gtk_main_iteration();
		}
		/* show progress dialog */
		ProgressBarShow();
	}

EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowShowCalibrationDialog()]=%d.\n", ret );
#endif
	return	ret;
}

static CNMSVoid CnmsScanFlowGtkMainIteration(
									CNMSInt32 wait10msecCount				)
{
	struct	timespec mytime;
	CnmsSetMem( (CNMSLPSTR)&mytime, 0, sizeof(mytime) );
	mytime.tv_sec   = 0;
	mytime.tv_nsec  = 10000000;	/* 10msec */
	
	if ( wait10msecCount > 0 ) {
		while( wait10msecCount-- ) {
			nanosleep( &mytime, NULL );
			while( gtk_events_pending() ){
				gtk_main_iteration();
			}
		}
	}
}

static CNMSInt32 CnmsScanFlowWarmingUpDialog(
									CNMSBool			show,
									CNMSInt32			pages,
									CNMSScanInfo		*info				)
{
	CNMSUIInfo		*uiInfo = ( CNMSUIInfo * )info->UiInfo;

	GtkWidget		*calibration_label = CNMSNULL;
	char			*calibration_label_str = MSG_EVERY_CALIBRATION_OFF;
	
	CNMSInt32		CCD = ( uiInfo->Ability.sensor_type == CANON_ABILITY_SENSOR_TYPE_CCD ) ? CNMS_TRUE : CNMS_FALSE ;
	
	if( !CCD || pages != 0 ) {
		/* do nothing. */
		return CNMS_FALSE;
	}
	
	/* if ( CCD && pages == 0 ) */

	if ( uiInfo->Calibration == NULL ) {
		DBGMSG("Error[CnmsScanFlowShowAdjustingLampDialog] calibration_dialog is NULL\n");
		return CNMS_ERR;
	}
	else if ( ( calibration_label = (GtkWidget *)lookup_widget( uiInfo->Calibration, "dialog_calibration_label" ) ) == CNMSNULL ){
		DBGMSG( "[CnmsScanFlowShowAdjustingLampDialog]Can't look up widget(dialog_calibration_label).\n" );
		return CNMS_ERR;
	}

	if ( show ) {
		/* set dialog_calibration_label */
		gtk_label_set_text( GTK_LABEL( calibration_label ), gettext( calibration_label_str ) );
		
		ProgressBarHide();
		W1_ModalDialogShowAction( uiInfo->Calibration, main_window );
		CnmsScanFlowGtkMainIteration( CNMS_WARMINGUP_10MSEC_WAIT );
	}
	else {
		/* hide */
		W1_ModalDialogHideAction( uiInfo->Calibration, CNMSNULL );
		while( gtk_events_pending() ){
			gtk_main_iteration();
		}
	}
	
	return CNMS_TRUE;
}


static CNMSInt32 CnmsScanFlowSaneGetParameters(
									CNMSScanInfo		*info,
									SANE_Parameters		*parm				)
{
	CNMSUIInfo		*uiInfo = ( CNMSUIInfo * )info->UiInfo;
	SANE_Status		ret;
	
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowSaneGetParameters\n");
#endif

	if( ( ret = sane_get_parameters( uiInfo->Hand, parm ) ) != SANE_STATUS_GOOD ){
		DBGMSG("Error[CnmsScanFlowSaneGetParameters] sane_get_parameters \n");
		return	CNMS_ERR;	
	}
	/*
		SCAN, PREVIEWF update BaseSize
	*/
	info->Depth = parm->depth;
	info->BaseSize.Width = ( parm->bytes_per_line * 8 ) / ( info->Depth * info->Samples);
	info->BaseSize.Height = parm->lines;
	
	if( info->ActionMode == CNMSSCPROC_ACTION_PREVIEW ){
		CnmsPrevFdInfo->BaseSize = info->BaseSize;
		CnmsPrevFdInfo->Depth = info->Depth;
		
		/* get work memory for preview data */
		if ( ( CnmsPrevFdInfo->Data = ( CNMSUInt8 * )CnmsGetMem( info->BaseSize.Width * info->BaseSize.Height * info->Samples * ( info->Depth / 8 ) ) ) == CNMSNULL ) {
			DBGMSG("Error[CnmsScanFlowSaneGetParameters] CnmsGetMem [CnmsPrevFdInfo->Data] is error!\n");
			return CNMS_ERR;
		}
	}
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "     BaseSize.W = %d, BaseSize.H = %d, Depth = %d\n", info->BaseSize.Width, info->BaseSize.Height, info->Depth);
#endif

	return	CNMS_NO_ERR;
}


static CNMSInt32 CnmsScanFlowImgProcModInit(
									CNMSScanInfo		*info,
									CNMSVoid			**CnmsScanWork		)
{
	CNMSInt32			ret				= CNMS_ERR;
	
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowImgProcModInit\n");
#endif

	if( info->ActionMode == CNMSSCPROC_ACTION_SCAN ) {
		info->InCropSize = info->BaseSize;
		info->OutCropSize = info->OutSize;
	}
	else {
		info->BaseSize = CnmsPrevFdInfo->BaseSize;
		
		info->InFullSize = CnmsPrevFdInfo->BaseSize;
		
		info->OutFullSize.Width = ( CnmsPrevFdInfo->BaseSize.Width * info->OutRes ) / CnmsPrevFdInfo->InRes.X;
		info->OutFullSize.Height = ( CnmsPrevFdInfo->BaseSize.Height * info->OutRes ) / CnmsPrevFdInfo->InRes.Y;
		
		info->InCropSize.Width = ( info->InSize.Width * CnmsPrevFdInfo->InRes.X ) / info->OutRes;
		info->InCropSize.Height = ( info->InSize.Height * CnmsPrevFdInfo->InRes.Y ) / info->OutRes;

		info->OutCropSize = info->OutSize;
		
		/* ScanOffset(InRes) <- InOffset(OutRes) */
		info->ScanOffset.X = ( info->InOffset.X * CnmsPrevFdInfo->InRes.X ) / info->OutRes;
		info->ScanOffset.Y = ( info->InOffset.Y * CnmsPrevFdInfo->InRes.Y ) / info->OutRes;

		info->OutOffset = info->InOffset;
		
		info->Samples = CnmsPrevFdInfo->Samples;
		info->Depth = CnmsPrevFdInfo->Depth;
		info->InRes = CnmsPrevFdInfo->InRes;
		info->GammaValue = CnmsPrevFdInfo->GammaValue;
		
		CnmsPrevFdInfo->OutRes = info->OutRes;
		CnmsPrevFdInfo->ScanOffset = info->ScanOffset;
	}

	if( ( ret = lpCnmsImgApi->p_CnmsImgProcWorkInit( info, CnmsScanWork ) ) != CNMS_NO_ERR ){
		DBGMSG("[CnmsScanFlowImgProcModInit] CnmsImgProcWorkInit \n");
		set_module_error();
		goto EXIT;
	}

	if( ( ret = lpCnmsImgApi->p_CnmsImgProcModInit( *CnmsScanWork ) ) != CNMS_NO_ERR ){
		DBGMSG("Error[CnmsScanFlowImgProcModInit] CnmsImgProcModInit \n");
		set_module_error();
		goto EXIT;
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowImgProcModInit()]=%d.\n", ret );
#endif
	return	ret;	
}


static CNMSInt32 CnmsScanFlowClearPreviewData(
									CNMSFd				dstFd,
									CNMSUIReturn 		*UiReturn,
									CNMSScanInfo		*info				)
{
	CNMSInt32	ret = CNMS_ERR;
	CNMSInt32	i, size;
	CNMSUInt8	*white_data = CNMSNULL;
	CNMSSize	OutFullSize;
	CNMSUIInfo				*uiInfo = ( CNMSUIInfo * )info->UiInfo;
	CANON_ABILITY_SUB_INFO	*lpInfo;
	CANON_ABILITY_INFO		*ability = &uiInfo->Ability;

	
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowClearPreviewData\n");
#endif

	CnmsScanFlowPreviewFdInfoDispose();

	/* check */
	if( ( info->Method >= CANON_ABILITY_SUB_INFO_MAX ) || ( info->InSize.Width <= 0 ) || ( info->InSize.Height <= 0 )
		|| ( info->InOffset.X < 0 ) || ( info->InOffset.Y < 0 ) ){
		DBGMSG( "[CnmsScanFlowClearPreviewData]parameter is error.\n" );
		goto	EXIT;
	}
	lpInfo = &( ability->info[info->Method] );
	
	OutFullSize.Width = ( lpInfo->max_width * info->OutRes + ability->opt_res / 2 ) / ability->opt_res;
	OutFullSize.Height = ( lpInfo->max_length * info->OutRes + ability->opt_res / 2 ) / ability->opt_res;

	size = OutFullSize.Width * OutFullSize.Height * 3;
	
	if ( ( white_data = (CNMSUInt8*)CnmsGetMem( size ) ) == CNMSNULL ) {
		DBGMSG( "[CnmsScanFlowClearPreviewData]Can't get [white_data] memory( bytes = %d ).\n\n", size );
		goto	EXIT;
	}
	CnmsSetMem( (CNMSLPSTR)white_data, CNMS_CLEAR_PREVIEW_VALUE, size );
	
	if( FileControlWriteFile( dstFd, (CNMSLPSTR)white_data, size ) != CNMS_NO_ERR ){
		DBGMSG( "[CnmsScanFlowClearPreviewData]Error is occured in FileControlWriteFile.\n" );
		goto	EXIT;
	}
	
	UiReturn->ResultImgSize = OutFullSize;

	ret = CNMS_NO_ERR;
EXIT:
	if( white_data != CNMSNULL ){
		CnmsFreeMem( (CNMSLPSTR)white_data );
	}
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowClearPreviewData()]=%d.\n", ret );
#endif
	return	ret;
}


static CNMSInt32 CnmsScanFlowScanData(
									CNMSInt32			pages,
									CNMSScanInfo		*info,
									SANE_Parameters		*parm,
									CNMSUIReturn 		*UiReturn,
									CNMSVoid			*CnmsScanWork,		
									CNMSFdInfo			*ScanFdInfo,
									CNMSFdInfo			*PrevFdInfo			)
{
	CNMSInt32			ret 			= CNMS_ERR,
						readByte 		= 0,
						ldata			= 0;
	CNMSDec32			totalSize 		= 0,
						curSize 		= 0;
	CNMSUInt8			*lineBuf 		= CNMSNULL;
	CNMSUInt8			*lineBufDst 	= CNMSNULL;
	CNMSFd				fd				= CNMS_FILE_ERR;
	CNMSByte			*path			= CNMSNULL;
	CNMSUIInfo			*uiInfo = ( CNMSUIInfo * )info->UiInfo;
	SANE_Status			status;
	CNMSInt32			lret			= -1;
	CNMSInt32			mret			= CNMS_ERR;
	LPCNMSOPTSNUM		lpOpts = &( uiInfo->OptsNum );
	
	CNMSInt32			tmpPage			= pages;
	
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowScanData\n");
#endif

	if( ( lineBuf = ( CNMSUInt8 * )CnmsGetMem( parm->bytes_per_line ) ) == CNMSNULL ){
		DBGMSG("Error[CnmsScanFlowScanData] Can't get [lineBuf] memory( bytes = %d ).\n", parm->bytes_per_line);
		goto	EXIT;
	}

	totalSize = (CNMSDec32)parm->bytes_per_line * (CNMSDec32)parm->lines;

	if( info->ActionMode == CNMSSCPROC_ACTION_SCAN ){
		fd = ScanFdInfo->Fd;
		path = ScanFdInfo->Path;
		if( info->Method == CANON_ABILITY_SUB_INFO_FLATBED ) {
			tmpPage = 0;
		}
	}
	else{	/* CNMSSCPROC_ACTION_PREVIEW */
		PrevFdInfo->Current = PrevFdInfo->Data;
		CnmsImgProcFlag = TRUE;		/* ImageProcessing progress bar -> on */
		tmpPage = 0;
	}
	ProgressBarUpdate( 0, tmpPage );
	
	while( curSize < totalSize )
	{
		if ( CnmsScanStatus == CNMS_SCAN_STATUS_OPENED ) {
			status = SANE_STATUS_CANCELLED;
			ProgressBarWaitFinish();
			/*ProgressBarHide();*/
			break;
		}
		/* get raster data */
		if( ( status = sane_read( uiInfo->Hand, ( SANE_Byte* )lineBuf, parm->bytes_per_line, ( SANE_Int* )&readByte ) ) == SANE_STATUS_EOF ){
			break;
		}
		else if ( status == SANE_STATUS_CANCELLED ) {
			break;
		}
		else if( status != SANE_STATUS_GOOD ){
			goto EXIT_CANCEL;
		}
		if ( ( mret = lpCnmsImgApi->p_CnmsImgProcCorrectSetBase( lineBuf, CnmsScanWork ) ) != CNMS_NO_ERR ) {
			set_module_error();
			DBGMSG( "[CnmsScanFlowScanData] error in CnmsImgProcCorrectSetBase.\n" );
			goto EXIT_CANCEL;
		}
		
		while ( 1 ) {
			if ( ( mret = lpCnmsImgApi->p_CnmsImgProcCorrectGet( &lineBufDst, CnmsScanWork ) ) != CNMS_NO_ERR ) {
				set_module_error();
				DBGMSG( "[CnmsScanFlowScanData] error in CnmsImgProcCorrectGet.\n" );
				goto EXIT_CANCEL;
			}
			if ( lineBufDst == CNMSNULL ) {
				break;
			}
			else {
				if ( fd == CNMS_FILE_ERR ) {
					/* CNMSSCPROC_ACTION_PREVIEW */
					CnmsCopyMem( (CNMSLPSTR)lineBufDst, (CNMSLPSTR)( PrevFdInfo->Current ), readByte );
					PrevFdInfo->Current += readByte;
				}
				else {
					/* CNMSSCPROC_ACTION_SCAN */
					if( FileControlWriteFile( fd, (CNMSLPSTR)lineBufDst, readByte ) != CNMS_NO_ERR ){
						DBGMSG( "[CnmsScanFlowScanData]Error is occured in FileControlWriteFile.\n" );
						goto	EXIT_CANCEL;
					}
				}
				curSize += readByte;
				if ( curSize >= totalSize ) {
					status = SANE_STATUS_EOF;
					if ( curSize > totalSize )DBGMSG("    CnmsScanFlowScanData curSize != totalSize\n");
					break;
				}
			}
		}
		lpCnmsImgApi->p_CnmsImgProcCorrectComplete( CnmsScanWork );
		ProgressBarUpdate( ( CNMSInt32 )( ( curSize / totalSize ) * ( CnmsProgressValue ) ), tmpPage );
	}
	
	if ( status != SANE_STATUS_EOF ) {
		if ( info->ActionMode == CNMSSCPROC_ACTION_SCAN ) {
			ret = CNMS_CANCELED;
			goto EXIT_CANCEL;
		}
		else {
			/* fill black */
			CNMSInt32 i;
			readByte = parm->bytes_per_line;
			for( i = 0;i < readByte;i++) lineBuf[i] = 0;
			while( curSize < totalSize ) {
				CnmsCopyMem( (CNMSLPSTR)lineBuf, (CNMSLPSTR)( PrevFdInfo->Current ), readByte );
				PrevFdInfo->Current += readByte;
				curSize += readByte;
			}
		}
	}
	
	ret = CNMS_NO_ERR;

EXIT_CANCEL:
	lpCnmsImgApi->p_CnmsImgProcCorrectComplete( CnmsScanWork );
	
	if ( fd != CNMS_FILE_ERR ) {
		if( ( lret = FileControlSeekFile( fd, 0, FILECONTROL_SEEK_FROM_TOP ) ) < 0 ){
			DBGMSG( "[CnmsScanFlowScanData]Error is occured in FileControlWriteFile.\n" );
			goto	EXIT;
		}
	}

	if( info->Method == CANON_ABILITY_SUB_INFO_FLATBED ) {
		/* Ver.1.70 */
		/* get backend error code */
		if( ( ldata = sane_control_option( uiInfo->Hand, lpOpts->getStatus, SANE_ACTION_GET_VALUE, &lastBackendErrCode, CNMSNULL ) ) != SANE_STATUS_GOOD ){
			DBGMSG( "[CnmsScanFlowGetBackendError] Can't get last error code.\n" );
		}
		sane_cancel( uiInfo->Hand );
	}
	else if( status != SANE_STATUS_GOOD && status != SANE_STATUS_EOF ) {	/* ADF error/cancel */
		/* skip CnmsScanFlowSetBackendError in CnmsScanFlowExec */
		ret = CNMS_SKIP_SETBACKENDERROR;
		/* PC canceled */
		if( status == SANE_STATUS_CANCELLED ) {
			lastBackendErrCode = BERRCODE_CANCELED_ADF;
		}
		/* other error */
		else {
			/* get backend error code */
			if( ( ldata = sane_control_option( uiInfo->Hand, lpOpts->getStatus, SANE_ACTION_GET_VALUE, &lastBackendErrCode, CNMSNULL ) ) != SANE_STATUS_GOOD ){
				DBGMSG( "[CnmsScanFlowGetBackendError] Can't get last error code.\n" );
			}
		}
		DBGMSG("[CnmsScanFlowScanData]lastBackendErrCode = %d, status = %d\n", lastBackendErrCode, status );
		sane_cancel( uiInfo->Hand );
	}

	CnmsScanStatus = CNMS_SCAN_STATUS_OPENED;
	
EXIT:

	UiReturn->ResultImgSize.Width = ( parm->bytes_per_line * 8 ) / ( info->Depth * info->Samples);
	UiReturn->ResultImgSize.Height = parm->lines;

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "   <UiReturn>  Width = %d, Height = %d\n", UiReturn->ResultImgSize.Width, UiReturn->ResultImgSize.Height);
#endif
	if( lineBuf != CNMSNULL ){
		CnmsFreeMem( (CNMSLPSTR)lineBuf );
	}
	if( fd != CNMS_FILE_ERR ) {
		if ( ret != CNMS_NO_ERR ) {
			if ( path ) {
				/* unlink( path ); */
				FileControlDeleteFile( path, fd );
				ScanFdInfo->Path[0] = '\0';
				ScanFdInfo->created_tmpfile = CNMS_FALSE;
			}
		}
		else {	/* ret == CNMS_NO_ERR */
			FileControlCloseFile( fd );
		}
		ScanFdInfo->Fd = CNMS_FILE_ERR;
	}
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowScanData()]=%d.\n", ret );
#endif
	return	ret;
}


static CNMSInt32 CnmsScanFlowImgCorrectFull(
									CNMSFd				dstFd,
									CNMSScanInfo		*info,
									CNMSUIReturn 		*UiReturn,
									CNMSVoid			*CnmsScanWork,		
									CNMSFdInfo			*PrevFdInfo			)
{
	CNMSInt32			ret				= CNMS_ERR,
						lineBufSize		= 0,
						readByte		= 0,
						bytePerLineIn	= info->InFullSize.Width * info->Samples * PrevFdInfo->Depth / 8,
						bytePerLineOut	= info->OutFullSize.Width * info->Samples,
						lineNumIn		= 0,
						lineNumOut		= 0,
						i				= 0;
	CNMSUInt8			*lineBufDst 	= CNMSNULL;
	CNMSByte			*path			= CNMSNULL;
	CNMSInt32			lret			= -1;
	CNMSInt32			mret			= CNMS_ERR;

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowImgCorrectFull action = %d\n", info->ActionMode);
#endif

	lineBufSize = bytePerLineIn;

	PrevFdInfo->Current = PrevFdInfo->Data;
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "  InFullSize.Width, InFullSize.Height = (%d, %d),  OutFullSize.Width, OutFullSize.Height = (%d, %d), Samples = %d, Depth = %d\n", 
	info->InFullSize.Width, info->InFullSize.Height, 
	info->OutFullSize.Width, info->OutFullSize.Height, info->Samples, PrevFdInfo->Depth );
#endif

	for( lineNumIn = 0, lineNumOut = 0; ( lineNumIn < info->InFullSize.Height && lineNumOut < info->OutFullSize.Height ); lineNumIn++)
	{
		if ( ( mret = lpCnmsImgApi->p_CnmsImgProcCorrectSetFull( PrevFdInfo->Current, CnmsScanWork ) ) != CNMS_NO_ERR ) {
			set_module_error();
			DBGMSG( "[CnmsScanFlowImgCorrectFull] error in CnmsImgProcCorrectSetFull.\n" );
			goto EXIT;
		}
		PrevFdInfo->Current += bytePerLineIn;
		if( CnmsImgProcFlag == TRUE ){	/* after CNMSSCPROC_ACTION_PREVIEW */
			ProgressBarUpdate( ( CNMSInt32 )( ( ( lineNumIn * 10 ) / info->InFullSize.Height ) + CnmsProgressValue ), 0 );
		}

		while ( 1 ) {
			if ( ( mret = lpCnmsImgApi->p_CnmsImgProcCorrectGet( &lineBufDst, CnmsScanWork ) ) != CNMS_NO_ERR ) {
				set_module_error();
				DBGMSG( "[CnmsScanFlowImgCorrectFull] error in CnmsImgProcCorrectGet.\n" );
				goto EXIT;
			}
			if ( lineBufDst == CNMSNULL ) {
				break;
			}
			else {
				if( FileControlWriteFile( dstFd, (CNMSLPSTR)lineBufDst, bytePerLineOut ) != CNMS_NO_ERR ){
					DBGMSG( "[CnmsScanFlowImgCorrectFull]Error is occured in FileControlWriteFile.\n" );
					goto	EXIT;
				}
				lineNumOut++;
				
				if( lineNumOut >= info->OutFullSize.Height ){
					break;
				}
			}
		}
		lpCnmsImgApi->p_CnmsImgProcCorrectComplete( CnmsScanWork );
	}
	lpCnmsImgApi->p_CnmsImgProcCorrectComplete( CnmsScanWork );
	
	if( ( lret = FileControlSeekFile( dstFd, 0, FILECONTROL_SEEK_FROM_TOP ) ) < 0 ){
		DBGMSG( "[CnmsScanFlowImgCorrectFull]Error is occured in FileControlSeekFile.\n" );
		goto	EXIT;
	}

	UiReturn->ResultImgSize = info->OutFullSize;

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "   <UiReturn>  Width = %d, Height = %d\n", UiReturn->ResultImgSize.Width, UiReturn->ResultImgSize.Height);
#endif

	ret = CNMS_NO_ERR;

EXIT:

#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowImgCorrectFull()]=%d.\n", ret );
#endif
	return	ret;
}


static CNMSInt32 CnmsScanFlowImgCorrectCropScan(
									CNMSInt32			pages,
									CNMSFd				dstFd,
									CNMSScanInfo		*info,
									CNMSUIReturn 		*UiReturn,
									CNMSVoid			*CnmsScanWork,		
									CNMSFdInfo			*ScanFdInfo,
									CNMSFdInfo			*PrevFdInfo			);

static CNMSInt32 CnmsScanFlowImgCorrectCropPreview(
									CNMSFd				dstFd,
									CNMSScanInfo		*info,
									CNMSUIReturn 		*UiReturn,
									CNMSVoid			*CnmsScanWork,		
									CNMSFdInfo			*ScanFdInfo,
									CNMSFdInfo			*PrevFdInfo			);


static CNMSInt32 CnmsScanFlowImgCorrectCrop(
									CNMSInt32			pages,
									CNMSFd				dstFd,
									CNMSScanInfo		*info,
									CNMSUIReturn 		*UiReturn,
									CNMSVoid			*CnmsScanWork,		
									CNMSFdInfo			*ScanFdInfo,
									CNMSFdInfo			*PrevFdInfo			)
{
	CNMSInt32			ret				= CNMS_ERR;

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowImgCorrectCrop action = %d\n", info->ActionMode);
#endif

	if ( info->ActionMode == CNMSSCPROC_ACTION_SCAN ) {
		if( ( ret = CnmsScanFlowImgCorrectCropScan( pages, dstFd, info, UiReturn, CnmsScanWork, ScanFdInfo, PrevFdInfo ) ) != CNMS_NO_ERR ){
			DBGMSG("Error[CnmsScanFlowImgCorrectCrop] CnmsScanFlowImgCorrectCropScan \n");
			return ret;
		}
	}
	else {
		if( ( ret = CnmsScanFlowImgCorrectCropPreview( dstFd, info, UiReturn, CnmsScanWork, ScanFdInfo, PrevFdInfo ) ) != CNMS_NO_ERR ){
			DBGMSG("Error[CnmsScanFlowImgCorrectCrop] CnmsScanFlowImgCorrectCropPreview \n");
			return ret;
		}
	}

	return ret;
}


static CNMSInt32 CnmsScanFlowImgCorrectCropScan(
									CNMSInt32			pages,
									CNMSFd				dstFd,
									CNMSScanInfo		*info,
									CNMSUIReturn 		*UiReturn,
									CNMSVoid			*CnmsScanWork,		
									CNMSFdInfo			*ScanFdInfo,
									CNMSFdInfo			*PrevFdInfo			)
{
	CNMSInt32			ret				= CNMS_ERR,
						lineBufSize		= 0,
						bytePerLineIn	= info->InCropSize.Width * info->Samples * info->Depth / 8,
						bytePerLineOut	= info->OutCropSize.Width * info->Samples,
						lineNumIn		= 0,
						lineNumOut		= 0,
						i				= 0;
	CNMSUInt8			*lineBuf 		= CNMSNULL;
	CNMSUInt8			*lineBufDst 	= CNMSNULL;
	CNMSFd				fd				= CNMS_FILE_ERR;
	CNMSByte			*path			= CNMSNULL;
	CNMSInt32			lret			= -1;
	CNMSInt32			mret			= CNMS_ERR;
	
	CNMSInt32			tmpPage			= pages;
	CNMSUIInfo			*uiInfo = ( CNMSUIInfo * )info->UiInfo;

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "  -> CnmsScanFlowImgCorrectCropScan action = %d\n", info->ActionMode);
#endif

	if( info->ColorMode == CNMSSCPROC_CMODE_GRAY || info->ColorMode == CNMSSCPROC_CMODE_MONO ) {
		bytePerLineOut	= info->OutCropSize.Width;
	}
	
	path = ScanFdInfo->Path;
	if( ( fd = FileControlOpenFile( FILECONTROL_OPEN_TYPE_READ, path ) ) == CNMS_FILE_ERR ){
		DBGMSG( "[CnmsScanFlowImgCorrectCropScan]Error is occured in FileControlOpenFile.\n" );
		goto	EXIT;
	}
	ScanFdInfo->Fd = fd;
	
	if( ProgressBarGetCanceledStatus() ) {
		/* already canceled. */
		ret = CNMS_CANCELED;
		goto EXIT;
	}

	CnmsImgProcFlag = TRUE;		/* ImageProcessing Start	*/

	if( bytePerLineIn < bytePerLineOut ){
		lineBufSize = bytePerLineOut;
	}
	else{
		lineBufSize = bytePerLineIn;
	}

	if( ( lineBuf = ( CNMSUInt8 * )CnmsGetMem( lineBufSize ) ) == CNMSNULL ){
		DBGMSG("Error[CnmsScanFlowImgCorrectCropScan] Can't get [lineBuf] memory( bytes = %d ).\n", lineBufSize);
		goto	EXIT;
	}

	if( info->Method == CANON_ABILITY_SUB_INFO_FLATBED ) {
		tmpPage = 0;
	}

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__  /* output raw data */
DebugScanData( info, fd, bytePerLineIn, lineBufSize);
#endif

	for( lineNumIn = 0, lineNumOut = 0; (lineNumIn < info->InCropSize.Height && lineNumOut < info->OutCropSize.Height) ; lineNumIn++)
	{
		if( CnmsImgProcFlag == FALSE ){	/* ImageProcessing Cancel?	*/
			ret = CNMS_CANCELED;
			goto EXIT;
		}
		ProgressBarUpdate( ( CNMSInt32 )( ( ( lineNumIn * 20 ) / info->InCropSize.Height ) + CnmsProgressValue ), tmpPage );

		if( FileControlReadFile( fd, (CNMSLPSTR)lineBuf, bytePerLineIn ) == CNMS_ERR ){
			DBGMSG( "[CnmsScanFlowImgCorrectCropScan]Error is occured in FileControlReadFile.\n" );
			break;
		}

		if ( ( mret = lpCnmsImgApi->p_CnmsImgProcCorrectSetCrop( lineBuf, CnmsScanWork ) ) != CNMS_NO_ERR ) {
			set_module_error();
			DBGMSG( "[CnmsScanFlowImgCorrectCropScan] error in CnmsImgProcCorrectSetCrop.\n" );
			goto EXIT;
		}
		while ( 1 ) {
			if ( ( mret = lpCnmsImgApi->p_CnmsImgProcCorrectGet( &lineBufDst, CnmsScanWork ) ) != CNMS_NO_ERR ) {
				set_module_error();
				DBGMSG( "[CnmsScanFlowImgCorrectCropScan] error in CnmsImgProcCorrectGet.\n" );
				goto EXIT;
			}
			if ( lineBufDst == CNMSNULL ) {
				break;
			}
			else {
				if( FileControlWriteFile( dstFd, (CNMSLPSTR)lineBufDst, bytePerLineOut ) != CNMS_NO_ERR ){
					DBGMSG( "[CnmsScanFlowImgCorrectCropScan]Error is occured in FileControlWriteFile.\n" );
					goto	EXIT;
				}
				lineNumOut++;
				
				if( lineNumOut >= info->OutCropSize.Height ){
					break;
				}
			}
		}
		lpCnmsImgApi->p_CnmsImgProcCorrectComplete( CnmsScanWork );
	}
	lpCnmsImgApi->p_CnmsImgProcCorrectComplete( CnmsScanWork );
	
	ProgressBarUpdate( 100, tmpPage );
	
	if( ( lret = FileControlSeekFile( fd, 0, FILECONTROL_SEEK_FROM_TOP ) ) < 0 ){
		DBGMSG( "[CnmsScanFlowImgCorrectCropScan]Error is occured in FileControlSeekFile.\n" );
		goto	EXIT;
	}
	if( ( lret = FileControlSeekFile( dstFd, 0, FILECONTROL_SEEK_FROM_TOP ) ) < 0 ){
		DBGMSG( "[CnmsScanFlowImgCorrectCropScan]Error is occured in FileControlSeekFile.\n" );
		goto	EXIT;
	}

	UiReturn->ResultImgSize = info->OutCropSize;

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "   <UiReturn>  Width = %d, Height = %d\n", UiReturn->ResultImgSize.Width, UiReturn->ResultImgSize.Height);
#endif

	ret = CNMS_NO_ERR;

EXIT:
	if( CnmsImgProcFlag == FALSE ) {	/* ImageProcessing Canceled	*/
		if( info->Method != CANON_ABILITY_SUB_INFO_FLATBED ) {
			/* skip CnmsScanFlowSetBackendError in CnmsScanFlowExec */
			ret = CNMS_SKIP_SETBACKENDERROR;
			lastBackendErrCode = BERRCODE_CANCELED_ADF;
			/* PC canceled */
			sane_cancel( uiInfo->Hand );
		}
	}

	if( lineBuf != NULL )		free( lineBuf );

	if( fd != CNMS_FILE_ERR ){
		FileControlDeleteFile( path, fd );
		ScanFdInfo->Fd = CNMS_FILE_ERR;
		ScanFdInfo->Path[0] = '\0';
		ScanFdInfo->created_tmpfile = CNMS_FALSE;
	}

	CnmsImgProcFlag = FALSE;	/* ImageProcessing End		*/
	
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowImgCorrectCropScan()]=%d.\n", ret );
#endif
	return	ret;
}


static CNMSInt32 CnmsScanFlowImgCorrectCropPreview(
									CNMSFd				dstFd,
									CNMSScanInfo		*info,
									CNMSUIReturn 		*UiReturn,
									CNMSVoid			*CnmsScanWork,		
									CNMSFdInfo			*ScanFdInfo,
									CNMSFdInfo			*PrevFdInfo			)
{
	CNMSInt32			ret				= CNMS_ERR,
						lineBufSize		= 0,
						writeByte		= 0,
						bytePerLineIn	= info->InCropSize.Width * info->Samples * PrevFdInfo->Depth / 8,
						bytePerLineOut	= info->OutCropSize.Width * info->Samples,
						lineNumIn		= 0,
						lineNumOut		= 0,
						i				= 0,
						byteInOffset, byteOutOffset,
						lret			= -1;
	CNMSInt32			byteInLeftSkip	= 0,
						byteInRightSkip	= 0,
						byteOutLeftSkip	= 0,
						byteOutRightSkip= 0;
	CNMSPoint			inOffset, outOffset;
	CNMSUInt8			*lineBufDst 	= CNMSNULL;
	CNMSByte			*path			= CNMSNULL;
	CNMSInt32			mret			= CNMS_ERR;

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "  -> CnmsScanFlowImgCorrectCropPreview action = %d\n", info->ActionMode);
DBGMSG( "    info->InCropSize.Width = %d, info->InCropSize.Height = %d\n", info->InCropSize.Width, info->InCropSize.Height);
#endif

	lpCnmsImgApi->p_CnmsImgProcOriginalDataPDispose( &CnmsPrevFdInfo->HistogramWork );
	lineBufSize = bytePerLineIn;

	PrevFdInfo->Current = PrevFdInfo->Data;
	inOffset = info->ScanOffset;
	outOffset = info->OutOffset;

	/* in offset Y */
	byteInOffset = info->InFullSize.Width * info->Samples * inOffset.Y * PrevFdInfo->Depth / 8;
	PrevFdInfo->Current += byteInOffset;
	/* out offset Y */
	byteOutOffset = info->OutFullSize.Width * info->Samples * outOffset.Y;
	if( ( lret = FileControlSeekFile( dstFd, byteOutOffset, FILECONTROL_SEEK_FROM_TOP ) ) != byteOutOffset ){
		DBGMSG( "[CnmsScanFlowImgCorrectCropPreview]Error is occured in FileControlSeekFile.\n" );
		goto	EXIT;
	}

	/* skip (in) */
	byteInLeftSkip		= inOffset.X * info->Samples * PrevFdInfo->Depth / 8;
	byteInRightSkip		= ( info->InFullSize.Width - inOffset.X - info->InCropSize.Width ) * info->Samples * PrevFdInfo->Depth / 8;
	/* skip (out) */
	byteOutLeftSkip		= outOffset.X * info->Samples;
	byteOutRightSkip	= ( info->OutFullSize.Width - outOffset.X - info->OutCropSize.Width ) * info->Samples;

	for( lineNumIn = 0, lineNumOut = 0; (lineNumIn < info->InCropSize.Height && lineNumOut < info->OutCropSize.Height ); lineNumIn++)
	{
		if( CnmsImgProcFlag == TRUE ){	/* after CNMSSCPROC_ACTION_PREVIEW */
			ProgressBarUpdate( ( CNMSInt32 )( ( ( lineNumIn * 10 ) / info->InCropSize.Height ) + CnmsProgressValue + 10 ), 0 );
		}

		PrevFdInfo->Current += byteInLeftSkip;
		
		if ( ( mret = lpCnmsImgApi->p_CnmsImgProcCorrectSetCrop( PrevFdInfo->Current, CnmsScanWork ) ) != CNMS_NO_ERR ) {
			set_module_error();
			DBGMSG( "[CnmsScanFlowImgCorrectCropPreview] error in CnmsImgProcCorrectSetCrop.\n" );
			goto EXIT;
		}
		PrevFdInfo->Current += bytePerLineIn;
		while ( 1 ) {
			if ( ( mret = lpCnmsImgApi->p_CnmsImgProcCorrectGet( &lineBufDst, CnmsScanWork ) ) != CNMS_NO_ERR ) {
				set_module_error();
				DBGMSG( "[CnmsScanFlowImgCorrectCropPreview] error in CnmsImgProcCorrectGet.\n" );
				goto EXIT;
			}
			if ( lineBufDst == CNMSNULL ) {
				break;
			}
			else {
				if( ( lret = FileControlSeekFile( dstFd, byteOutLeftSkip, FILECONTROL_SEEK_FROM_CURRENT ) ) < 0 ){
					DBGMSG( "[CnmsScanFlowImgCorrectCropPreview]Error is occured in FileControlSeekFile.\n" );
					goto	EXIT;
				}
				if( FileControlWriteFile( dstFd, (CNMSLPSTR)lineBufDst, bytePerLineOut ) != CNMS_NO_ERR ){
					DBGMSG( "[CnmsScanFlowImgCorrectCropPreview]Error is occured in FileControlWriteFile.\n" );
					goto	EXIT;
				}
				if( ( lret = FileControlSeekFile( dstFd, byteOutRightSkip, FILECONTROL_SEEK_FROM_CURRENT ) ) < 0 ){
					DBGMSG( "[CnmsScanFlowImgCorrectCropPreview]Error is occured in FileControlSeekFile.\n" );
					goto	EXIT;
				}
				lineNumOut++;
				
				if( lineNumOut >= info->OutCropSize.Height ){
					break;
				}
			}
		}
		lpCnmsImgApi->p_CnmsImgProcCorrectComplete( CnmsScanWork );
		
		PrevFdInfo->Current += byteInRightSkip;
	}
	lpCnmsImgApi->p_CnmsImgProcCorrectComplete( CnmsScanWork );
	
	ProgressBarUpdate( 100, 0 );
	
	if( ( lret = FileControlSeekFile( dstFd, 0, FILECONTROL_SEEK_FROM_TOP ) ) < 0 ){
		DBGMSG( "[CnmsScanFlowImgCorrectCropPreview]Error is occured in FileControlSeekFile.\n" );
		goto	EXIT;
	}

	UiReturn->ResultImgSize = info->OutFullSize;

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "   <UiReturn>  Width = %d, Height = %d\n", UiReturn->ResultImgSize.Width, UiReturn->ResultImgSize.Height);
#endif

	CnmsPrevFdInfo->HistogramWork = lpCnmsImgApi->p_CnmsImgProcOriginalDataPGet( CnmsScanWork );
	
	CnmsImgProcFlag = FALSE;	/* ImageProcessing End		*/

	ret = CNMS_NO_ERR;

EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowImgCorrectCropPreview()]=%d.\n", ret );
#endif
	return	ret;
}


static CNMSInt32 CnmsScanFlowGetHistogram(
									CNMSScanInfo		*info,
									CNMSUIReturn 		*UiReturn,
									CNMSVoid			*CnmsScanWork		)
{
	CNMSInt32		ret = CNMS_ERR;
	CNMSHistogram	*histogram = CNMSNULL;
	CNMSHistogram	*final = CNMSNULL;
	
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowGetHistogram\n");
#endif

	if( info->ColorMode != CNMSSCPROC_CMODE_MONO ) {
		histogram = &UiReturn->Histogram;
	}
	else {
		histogram = &UiReturn->MonoChrome;
	}
	final = &UiReturn->FinalReview;
	
	if ( ( ret = lpCnmsImgApi->p_CnmsImgProcGetHistogram( histogram, final, CnmsScanWork ) ) != CNMS_NO_ERR ) {
		set_module_error();
		DBGMSG( "[CnmsScanFlowGetHistogram] error in CnmsImgProcGetHistogram.\n" );
		goto EXIT;
	}
	ret = CNMS_NO_ERR;

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DebugHistogram( info, UiReturn );
#endif


EXIT:
#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowGetHistogram()]=%d.\n", ret );
#endif
	return ret;
}

static CNMSInt32 CnmsScanFlowSetBackendError(
									CNMSScanInfo		*info				)
{
	CNMSInt32		ret = CNMS_ERR;
	CNMSUIInfo		*uiInfo = ( CNMSUIInfo * )info->UiInfo;
	LPCNMSOPTSNUM	lpOpts = &( uiInfo->OptsNum );
	CNMSInt32		ldata = SANE_STATUS_GOOD;
	
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowSetBackendError\n");
#endif

	if ( ! lastModuleErrCode ) {
		/* Ver.1.70 */
		if ( !lastBackendErrCode ) {
			if( ( ldata = sane_control_option( uiInfo->Hand, lpOpts->getStatus, SANE_ACTION_GET_VALUE, &lastBackendErrCode, CNMSNULL ) ) != SANE_STATUS_GOOD ){
				DBGMSG( "[CnmsScanFlowGetBackendError] Can't get last error code.\n" );
			}
		}
		if ( lastBackendErrCode == 0 ) {
			ret = CNMS_NO_ERR;
		}
	}

	if ( CnmsScanStatus == CNMS_SCAN_STATUS_STARTED ) {
		DBGMSG( "[CnmsScanFlowGetBackendError] call sane_cancel().\n" );
		sane_cancel( uiInfo->Hand );
		CnmsScanStatus = CNMS_SCAN_STATUS_OPENED;
	}

#ifdef	__CNMS_DEBUG_SCANFLOW__
	DBGMSG( "\t[CnmsScanFlowSetBackendError()]=%d. lastBackendErrCode = %d\n", ret, lastBackendErrCode );
#endif
	return	ret;
}


static CNMSVoid CnmsScanFlowTerminate(
									CNMSScanInfo		*ScanInfo,
									CNMSVoid			**CnmsScanWork		)
{
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
DBGMSG( "-> CnmsScanFlowTerminate\n");
#endif
	if( ScanInfo != CNMSNULL ){
		if( ScanInfo->ModelName != CNMSNULL ){
			CnmsFreeMem( (CNMSLPSTR)( ScanInfo->ModelName ) );
		}
		if( ScanInfo->DatFilePath != CNMSNULL ){
			CnmsFreeMem( (CNMSLPSTR)( ScanInfo->DatFilePath ) );
		}
		if( ScanInfo->DatPFilePath != CNMSNULL ){
			CnmsFreeMem( (CNMSLPSTR)( ScanInfo->DatPFilePath ) );
		}
		if( ScanInfo->UiInfo != CNMSNULL ){
			CnmsFreeMem( (CNMSLPSTR)( ScanInfo->UiInfo ) );
		}
		CnmsFreeMem( (CNMSLPSTR)ScanInfo );
		ScanInfo = CNMSNULL;
	}
	
	if( *CnmsScanWork != CNMSNULL ) {
		lpCnmsImgApi->p_CnmsImgProcTerminate( *CnmsScanWork );
		*CnmsScanWork = CNMSNULL;
	}
}

static CNMSInt32 CnmsScanFlowMakeDstFile(
									LPCNMS_NODE			*pnode				)
{
	CNMSInt32		ret = CNMS_ERR;
	CNMSFd			fd = CNMS_FILE_ERR;
	CNMSByte		file_path[ PATH_MAX ];
	
	if( pnode == CNMSNULL ) {
		DBGMSG( "[CnmsScanFlowMakeDstFile]pnode is CNMSNULL.\n" );
		goto	EXIT;
	}
	
	CnmsSetMem( file_path, 0, sizeof( file_path ) );
	
	if( ( fd = FileControlMakeTempFile( file_path, PATH_MAX ) ) == CNMS_FILE_ERR ){
		DBGMSG( "[CnmsScanFlowMakeDstFile]Error is occured in FileControlMakeTempFile.\n" );
		goto	EXIT;
	}
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
	DBGMSG( "[CnmsScanFlowMakeDstFile] create file [%s]\n", file_path );
#endif
	if( ( *pnode = CnmsNewNode( file_path ) ) == CNMSNULL ) {
		DBGMSG( "[CnmsScanFlowMakeDstFile]Error is occured in CnmsNewNode.\n" );
		goto 	EXIT;
	}
	(*pnode)->fd = fd;
	
	ret = CNMS_NO_ERR;

EXIT:
	return ret;
}

static CNMSInt32 CnmsScanFlowAddList(
									CNMSScanInfo		*info,
									CNMSInt32			pages,
									LPCNMS_NODE			*pnode,
									LPCNMS_ROOT			root				)

{
	CNMSInt32		ret = CNMS_ERR;
	CNMSUIInfo		*uiInfo = ( CNMSUIInfo * )info->UiInfo;

	if( pnode == CNMSNULL || root == CNMSNULL ) {
		DBGMSG( "[CnmsScanFlowAddList]parameter is error.\n" );
		goto	EXIT;
	}
	else if ( *pnode == CNMSNULL ) {
		DBGMSG( "[CnmsScanFlowAddList]parameter is error.\n" );
		goto	EXIT;
	}
	
	FileControlCloseFile( (*pnode)->fd );
	(*pnode)->fd = CNMS_FILE_ERR;
	
	(*pnode)->page = pages;
	(*pnode)->show_page = ( uiInfo->Method == CANON_ABILITY_SUB_INFO_FLATBED ) ? CNMS_FALSE : CNMS_TRUE;
	(*pnode)->rotate = ( ( pages % 2 ) || uiInfo->Method != CANON_ABILITY_SUB_INFO_ADF2 || uiInfo->BindingLocation == CNMS_A_BINDING_LOCATION_SHORT ) ? CNMS_FALSE : CNMS_TRUE ;
	CnmsPutQueue( root, *pnode );
#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__
	DBGMSG( "[CnmsScanFlowAddList] <%d> file [%s]\n", (*pnode)->page, (*pnode)->file_path );
#endif
	*pnode = CNMSNULL;
	
	ret = CNMS_NO_ERR;

EXIT:
	return ret;
}

static CNMSBool CnmsScanFlowExitLoop(
									CNMSScanInfo		*info				)
{
	if ( info->ActionMode != CNMSSCPROC_ACTION_SCAN ) {
		return CNMS_TRUE;
	}
	if ( info->Method == CANON_ABILITY_SUB_INFO_FLATBED ) {
		return CNMS_TRUE;
	}
	return CNMS_FALSE;
}

static CNMSVoid CnmsScanFlowDisposeDstFile(
									LPCNMS_NODE			*pnode,
									CNMSFdInfo			*ScanFdInfo			)
{

	if( pnode == CNMSNULL ) {
		DBGMSG( "[CnmsScanFlowDisposeDstFile]pnode is CNMSNULL.\n" );
		return ;
	}
	else if ( *pnode != CNMSNULL ) {
		if( (*pnode)->fd != CNMS_FILE_ERR ) {
			DBGMSG( "[CnmsScanFlowDisposeDstFile] delete file(crop) [%s]\n", (*pnode)->file_path );
			FileControlDeleteFile( (*pnode)->file_path, (*pnode)->fd );
		}
		CnmsDisposeNode( pnode );	/* *pnode -> CNMSNULL */
	}
	if( ScanFdInfo != CNMSNULL ){
		if( ScanFdInfo->created_tmpfile ){
	/*	if( ScanFdInfo->Fd != CNMS_FILE_ERR ){*/
			DBGMSG( "[CnmsScanFlowDisposeDstFile] delete file(base) [%s]\n", ScanFdInfo->Path );
			FileControlDeleteFile( ScanFdInfo->Path, ScanFdInfo->Fd );
			ScanFdInfo->Fd = CNMS_FILE_ERR;
			ScanFdInfo->Path[0] = '\0';
		}
		CnmsFreeMem( (CNMSLPSTR)ScanFdInfo );
	}
}

#ifdef __CNMS_DEBUG_SCANFLOW_DETAIL__

static CNMSVoid DebugScanData( CNMSScanInfo *info, CNMSFd fd, CNMSInt32 bytePerLineIn, CNMSInt32 lineBufSize )
{
#ifdef __DEBUG_SCAN_DATA__

	CNMSFd tempfd;
	CNMSByte log[512];
	CNMSInt32 lineNumIn, writeByte, lret;
	CNMSUInt8			*lineBuf 		= CNMSNULL;
	
	CnmsSetMem( (CNMSLPSTR)log, 0, sizeof(log) );
	
	/* make dst file */
	if( ( tempfd = FileControlOpenFile( FILECONTROL_OPEN_TYPE_NEW, "/tmp/scandata.log" ) ) == CNMS_FILE_ERR ){
		DBGMSG( "[DebugScanData]Error is occured in FileControlOpenFile.\n" );
		goto	EXIT;
	}
	sprintf( log, "%d x %d\n", info->InCropSize.Width, info->InCropSize.Height);
	if( FileControlWriteFile( tempfd, log, sizeof( log ) ) != CNMS_NO_ERR ){
		DBGMSG( "[DebugScanData]Error is occured in FileControlWriteFile.\n" );
		goto	EXIT;
	}
	FileControlCloseFile( tempfd );
	
	/* make dst file */
	if( ( tempfd = FileControlOpenFile( FILECONTROL_OPEN_TYPE_NEW, "/tmp/scandata.raw" ) ) == CNMS_FILE_ERR ){
		DBGMSG( "[DebugScanData]Error is occured in FileControlOpenFile.\n" );
		goto	EXIT;
	}

	DBGMSG( "   <scandata>  Width = %d, Height = %d\n", info->InCropSize.Width, info->InCropSize.Height);
	
	if( ( lineBuf = ( CNMSUInt8 * )CnmsGetMem( lineBufSize ) ) == CNMSNULL ){
		DBGMSG("Error[CnmsScanFlowImgCorrectCropScan] Can't get [lineBuf] memory( bytes = %d ).\n", lineBufSize);
		goto	EXIT;
	}

	for( lineNumIn = 0; lineNumIn < info->InCropSize.Height; lineNumIn++)
	{
		if( FileControlReadFile( fd, lineBuf, bytePerLineIn ) == CNMS_ERR ){
			DBGMSG( "[DebugScanData]Error is occured in FileControlReadFile.\n" );
			break;
		}
		if( FileControlWriteFile( tempfd, lineBuf, bytePerLineIn ) != CNMS_NO_ERR ){
			DBGMSG( "[DebugScanData]Error is occured in FileControlWriteFile.\n" );
			goto	EXIT;
		}
	}
	FileControlCloseFile( tempfd );
	if( ( lret = FileControlSeekFile( fd, 0, FILECONTROL_SEEK_FROM_TOP ) ) < 0 ){
		DBGMSG( "[DebugScanData]Error is occured in FileControlOpenFile.\n" );
		goto	EXIT;
	}
EXIT:
	if( lineBuf != CNMSNULL ){
		CnmsFreeMem( lineBuf );
	}
	return;

#endif
}

static CNMSVoid DebugHistogram( CNMSScanInfo *info, CNMSUIReturn *UiReturn )
{
#ifdef __DEBUG_HISTOGRAM__

	CNMSInt32		i,j;
	CNMSHistogram	*histogram = CNMSNULL;
	CNMSHistogram	*final = CNMSNULL;
	
	if( info->ColorMode != CNMSSCPROC_CMODE_MONO ) {
		histogram = &UiReturn->Histogram;
	}
	else {
		histogram = &UiReturn->MonoChrome;
	}
	final = &UiReturn->FinalReview;
	
	DBGMSG( "------- DebugHistogram <Histogram Master>-------\n" );
	for(i = 0;i < 16;i++) {
		for(j = 0;j < 16;j++) {
			DBGMSG( " %d", histogram->Master[i*16+j] );
		}
		DBGMSG( "\n" );
	}
	DBGMSG( "------- DebugHistogram <Final Master>-------\n" );
	for(i = 0;i < 16;i++) {
		for(j = 0;j < 16;j++) {
			DBGMSG( " %d", final->Master[i*16+j] );
		}
		DBGMSG( "\n" );
	}

#endif
}



#endif /* __CNMS_DEBUG_SCANFLOW_DETAIL__ */

#endif	/* _SCANFLOW_C_ */

