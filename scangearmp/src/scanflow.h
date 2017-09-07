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

#ifndef _SCANFLOW_H_
#define _SCANFLOW_H_

#include <gtk/gtk.h>
#include <sane/sane.h>

#include "cnmstype.h"
#include "cnmsimg.h"
#include "scanopts.h"
#include "cnmsfunc.h"

#ifdef	_SCANFLOW_GLOBALS_
	#define	SFGLOBAL
#else
	#define	SFGLOBAL	extern
#endif

SFGLOBAL CNMSIMGAPI *lpCnmsImgApi
#ifdef	_SCANFLOW_GLOBALS_
 = CNMSNULL
#endif
;

#define CNMS_CLEAR_PREVIEW_VALUE	(235)
#define CNMS_CANCELED				(2006)
#define CNMS_SKIP_SETBACKENDERROR	(2009)

typedef struct{
	CNMSLPSTR			ModelName;			/* MP160/MP510/MP600...										*/
	CNMSInt32			Method;				/* 															*/
	CNMSInt32			Preview;			/* CNMSSCPROC_PREVIEWIMG_XXXXX								*/
	CNMSInt32			MediaType;			/* CNMSSCPROC_MTYPE_XXXXX									*/
	CNMSInt32			MediaSize;			/* CNMSSCPROC_MSIZE_XXXXX									*/
	CNMSInt32			ColorMode;			/* CNMSSCPROC_CMODE_XXXXX									*/
	CNMSInt32			OutRes;				/* CNMSSCPROC_OUTPUTRES_XXXXX								*/
	CNMSSize			InSize;				/* input size(pixel)										*/
	CNMSSize			OutSize;			/* output size(pixel)										*/
	CNMSPoint			InOffset;			/* scan offset(OutRes)										*/
	CNMSImgSetFlagUi	ImgSetFlagUi;		/* Image setting flag										*/
	CNMSImgSetFlagUi2	ImgSetFlagUi2;		/* Image setting flag 2										*/
	CNMSTable			UserTone;			/* user tone												*/
	CNMSInt32			Threshold;			/* threshold												*/
	CNMSInt32			ThresholdDefault;	/* default threshold(outside of crop area)					*/
	CNMSInt32			ColSetting;			/* CNMSSCPROC_CSET_XXXXX									*/
	CNMSDec32			MonGamma;			/* monitor gamma											*/
	GtkWidget			*Calibration;		/* calibration dialog										*/
	SANE_Handle			Hand;				/* sane handle												*/
	CNMSOPTSNUM			OptsNum;			/* option number list										*/
	CANON_ABILITY_INFO	Ability;			/* ability of device										*/
	CNMSInt32			SilentMode;			/* silent mode												*/
	CNMSInt32			EveryCalibration;	/* every calibration										*/
	CNMSInt32			BindingLocation;	/* binding location											*/
}CNMSUIInfo;

typedef struct{
	CNMSSize			ResultImgSize;		/* image size												*/
	CNMSHistogram		Histogram;			/* histogram of original image data							*/
	CNMSHistogram		FinalReview;		/* histogram of final image data							*/
	CNMSHistogram		MonoChrome;			/* histogram of monochrome image data						*/
}CNMSUIReturn;	


CNMSInt32 CnmsScanFlowExec(
									CNMSFd 			dstFd,
									LPCNMS_ROOT		root,
									CNMSUIInfo		*UiInfo,
									CNMSInt32 		ActionMode,
									CNMSUIReturn 	*UiReturn			);

CNMSVoid CnmsScanFlowScanCancel(
									CNMSVoid							);

CNMSVoid CnmsScanFlowPreviewFdInfoDispose(
									CNMSVoid							);

CNMSInt32 CnmsScanFlowGetOriginalRGB(
									CNMSInt32			*lpCurPosR,
									CNMSInt32			*lpR,
									CNMSInt32			*lpG,
									CNMSInt32			*lpB			);

CNMSInt32 CnmsImgApiOpen(
									CNMSLPSTR							);
CNMSVoid CnmsImgApiClose(
									CNMSVoid							);

CNMSInt32 CnmsGetNetworkModelInstalled(
									CNMSVoid							);
CNMSInt32 CnmsGetNetworkSupport(
									CNMSLPSTR			lpModel			);

CNMSInt32 CnmsScanFlowSetOptionResolution(
									CNMSScanInfo		*info,
									CANON_ABILITY_INFO	*ability		);

CNMSInt32 CnmsGetBytesPerPixel(
									CNMSVoid							);


#endif	/* _SCANFLOW_H_ */
