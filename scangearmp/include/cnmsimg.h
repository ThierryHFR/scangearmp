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

#ifndef _CNMSIMG_H_
#define _CNMSIMG_H_

#include "cnmstype.h"
#include "cnmsability.h"

#define		CNMSSCPROC_MODELNAME_MAX		(64)

#define		CNMSSCPROC_FLAG_OFF				(0)
#define		CNMSSCPROC_FLAG_ON				(1)

#define		CNMSSCPROC_PREVIEWIMG_OFF		(0)
#define		CNMSSCPROC_PREVIEWIMG_ON		(1)

#define		CNMSSCPROC_ACTION_SCAN			(0)
#define		CNMSSCPROC_ACTION_PREVIEW		(1)
#define		CNMSSCPROC_ACTION_PARAM			(2)
#define		CNMSSCPROC_ACTION_CROP_CORRECT	(3)
#define		CNMSSCPROC_ACTION_PREV_CLEAR	(4)
#define		CNMSSCPROC_ACTION_MAX			(5)

#define		CNMSSCPROC_MTYPE_PAPER			(0)

#define		CNMSSCPROC_MSIZE_FULLPAGE		(0)

#define		CNMSSCPROC_CMODE_COLOR			(0)
#define		CNMSSCPROC_CMODE_COLOR_TEXT		(1)
#define		CNMSSCPROC_CMODE_GRAY			(2)
#define		CNMSSCPROC_CMODE_MONO			(3)

#define		CNMSSCPROC_OUTPUTRES_0075		(  75)
#define		CNMSSCPROC_OUTPUTRES_0100		( 100)
#define		CNMSSCPROC_OUTPUTRES_0150		( 150)
#define		CNMSSCPROC_OUTPUTRES_0200		( 200)
#define		CNMSSCPROC_OUTPUTRES_0300		( 300)
#define		CNMSSCPROC_OUTPUTRES_0400		( 400)
#define		CNMSSCPROC_OUTPUTRES_0600		( 600)
#define		CNMSSCPROC_OUTPUTRES_1200		(1200)

#define		CNMSSCPROC_CSET_RECOMMEND		(0)
#define		CNMSSCPROC_CSET_NON_CORRECT		(2)

#define		CNMSSCPROC_OPT_COLOR_COLOR		(0)
#define		CNMSSCPROC_OPT_COLOR_GRAY		(1)

#define		CNMSSCPROC_GAMMA_LINEAR			(1.0)
#define		CNMSSCPROC_GAMMA_DEFAULT		(2.2)

enum{
	CNMS_SCAN_STATUS_NONE = 0,
	CNMS_SCAN_STATUS_INITED,
	CNMS_SCAN_STATUS_OPENED,
	CNMS_SCAN_STATUS_STARTED,
};

#if defined(__cplusplus)
extern "C"
{
#endif	/* __cplusplus */


typedef struct{
	CNMSInt32			Width;
	CNMSInt32			Height;
}CNMSSize;

typedef struct{
	CNMSInt32			X;
	CNMSInt32			Y;
}CNMSPoint;

typedef struct{
	CNMSInt32			USM;				/* unsharp mask */
}CNMSImgSetFlagUi;

typedef struct{
	CNMSInt32			USM;				/* unsharp mask 											*/
	CNMSInt32			Descreen;			/* moire reduction  										*/
	CNMSInt32			reserve[16];		/* reserve area 											*/
}CNMSImgSetFlagUi2;

typedef struct{
	CNMSUInt16			R[65536];			/* user gamma R												*/
	CNMSUInt16			G[65536];			/* user gamma G												*/
	CNMSUInt16			B[65536];			/* user gamma B												*/
}CNMSTable;	

typedef struct{
	CNMSUInt32			Master[256];		/* histogram Master											*/
	CNMSUInt32			Red[256];			/* histogram Red											*/
	CNMSUInt32			Green[256];			/* histogram Green											*/
	CNMSUInt32			Blue[256];			/* histogram Blue											*/
	CNMSUInt32			Gray[256];			/* histogram Gray											*/
}CNMSHistogram;	

typedef struct{
	CNMSLPSTR			ModelName;			/* MP160/MP510/MP600...										*/
	CNMSLPSTR			DatFilePath;		/* dat file path											*/
	CNMSInt32			Method;				/* 															*/
	CNMSInt32			Preview;			/* CNMSSCPROC_PREVIEWIMG_XXXXX								*/
	CNMSInt32			ActionMode;			/* CNMSSCPROC_ACTION_XXXXX									*/
	CNMSInt32			MediaType;			/* CNMSSCPROC_MTYPE_XXXXX									*/
	CNMSInt32			MediaSize;			/* CNMSSCPROC_MSIZE_XXXXX									*/
	CNMSInt32			ColorMode;			/* CNMSSCPROC_CMODE_XXXXX									*/
	CNMSInt32			OutRes;				/* CNMSSCPROC_OUTPUTRES_XXXXX								*/
	CNMSPoint			InRes;				/* scanning resolution										*/
	CNMSSize			InSize;				/* input size(pixel)										*/
	CNMSSize			OutSize;			/* output size(pixel)										*/
	CNMSSize			BaseSize;			/* scan size(pixel)											*/
											/* SCAN：InSize(InRes), PREVIEW：FullPlaten(InRes)			*/
	CNMSSize			InFullSize;			/* SCAN：not use, PREVIEW,PARAM,CROP：=BaseSize				*/
	CNMSSize			OutFullSize;		/* SCAN：not use, PREVIEW,PARAM,CROP：FullPlaten(OutRess)	*/
	CNMSSize			InCropSize;			/* SCAN：BaseSize,	PARAM,CROP：InSize(InRes)				*/
	CNMSSize			OutCropSize;		/* SCAN,PARAM,CROP：OutSize = InSize(OutRes)				*/
	CNMSPoint			InOffset;			/* scan offset(OutRes)										*/
	CNMSPoint			ScanOffset;			/* SCAN,PREVIEW：scan offset(InRes)							*/
	CNMSPoint			OutOffset;			/* PARAM,CROP： = InOffset									*/
	CNMSImgSetFlagUi	ImgSetFlagUi;		/* Image setting flag										*/
	CNMSTable			UserTone;			/* user tone												*/
	CNMSInt32			Threshold;			/* threshold												*/
	CNMSInt32			ThresholdDefault;	/* default threshold(outside of crop area)					*/
	CNMSInt32			ColSetting;			/* CNMSSCPROC_CSET_XXXXX									*/
	CNMSDec32			MonGamma;			/* monitor gamma											*/
	CNMSDec32			GammaValue;			/* gamma value for backend									*/
	CNMSUInt16			GammaTable[65536];	/* gamma table for backend									*/
	CNMSInt32			Samples;			/* Bytes/Pixel												*/
	CNMSInt32			Depth;				/* Bit/Pixel												*/
	CNMSInt32			ScanStatus;			/* status													*/
	
	CNMSHistogram		histogram;			/* histogram table											*/
	CNMSHistogram		final;				/* final review table										*/

	CNMSVoid			*UiInfo;			/* CNMSUIInfo												*/

	CNMSLPSTR			DatPFilePath;		/* datP file path											*/

	CNMSImgSetFlagUi2	ImgSetFlagUi2;		/* Image setting flag										*/
	CNMSInt32			SensorType;			/* Sensor type												*/
}CNMSScanInfo;	

/*---------------------  parameter    -------------------*/
CNMSInt32 CnmsParamSetOptionColor(
									CNMSScanInfo			*info		);

CNMSInt32 CnmsParamSetOptionResolution(
									CNMSScanInfo			*info,
									CANON_ABILITY_INFO		*ability	);

CNMSInt32 CnmsParamSetOptionScanArea(
									CNMSScanInfo			*info,
									CANON_ABILITY_INFO		*ability	);

CNMSInt32 CnmsParamSetOptionGammaTable(
									CNMSScanInfo			*info		);


/*------------------------   image correction   ------------------------*/
CNMSInt32 CnmsImgProcWorkInit(
									CNMSScanInfo			*info,
									CNMSVoid				**work		);

CNMSInt32 CnmsImgProcModInit(
									CNMSVoid				*work		);

CNMSInt32 CnmsImgProcCorrectSetBase(
									CNMSUInt8				*buff,
									CNMSVoid				*work		);

CNMSInt32 CnmsImgProcCorrectSetFull(
									CNMSUInt8				*buff,
									CNMSVoid				*work		);

CNMSInt32 CnmsImgProcCorrectSetCrop(
									CNMSUInt8				*buff,
									CNMSVoid				*work		);

CNMSInt32 CnmsImgProcCorrectGet(
									CNMSUInt8				**buff,
									CNMSVoid				*work		);

CNMSVoid CnmsImgProcCorrectComplete(
									CNMSVoid				*work		);

CNMSVoid CnmsImgProcTerminate(
									CNMSVoid				*work		);

CNMSInt32 CnmsImgProcGetHistogram(
									CNMSHistogram			*histogram,
									CNMSHistogram			*final,
									CNMSVoid				*work		);
									
CNMSVoid *CnmsImgProcOriginalDataPGet(
									CNMSVoid				*work		);
CNMSVoid CnmsImgProcOriginalDataPDispose(
									CNMSVoid				**work		);

CNMSInt32 CnmsImgProcGetOriginalRGB(
									CNMSVoid				*work,
									CNMSInt32				x,
									CNMSInt32				y,
									CNMSInt32				*r,
									CNMSInt32				*g,
									CNMSInt32				*b			);



/*------------------------------  Make Tone Curve   ----------------------------------*/

enum{
	CNMS_SPLINE_NO_ADJUST = 0,
	CNMS_SPLINE_EXP_OVER,
	CNMS_SPLINE_EXP_UNDER,
	CNMS_SPLINE_HIGH_CONT,
};


typedef struct {
	CNMSUInt32	m;
	CNMSUInt32	r;
	CNMSUInt32	g;
	CNMSUInt32	b;
}CNMSRGB32;

typedef struct {
	CNMSInt8	m;
	CNMSInt8	r;
	CNMSInt8	g;
	CNMSInt8	b;
}CNMSRGB08;

typedef struct {
	float		m;
	float		r;
	float		g;
	float		b;
}CNMSRGBFLOAT;

typedef struct {
	CNMSRGB32		blackpoint;
	CNMSRGB32		midpoint;
	CNMSRGB32		whitepoint;
	CNMSRGB32		graypoint;
	CNMSRGB08		brightness;
	CNMSRGB08		contrast;
	CNMSRGB08		balance;
	CNMSRGBFLOAT	gamma;
}CNMSMAKETONEPARAM;


void CnmsImgMakeToneCurve(
				    CNMSMAKETONEPARAM	*CnmsImgMakeToneParam,		/* input parameter */
				    CNMSRGB32			*CnmsImgUserCurve,			/* input user tone curve */
					CNMSRGB32			*CnmsImgToneCurve );		/* output tone curve */

void CnmsImgMakeToneCurve16(
				    CNMSMAKETONEPARAM	*CnmsImgMakeToneParam,		/* input parameter */
				    CNMSRGB32			*CnmsImgUserCurve,			/* input user tone curve */
					CNMSRGB32			*CnmsImgToneCurve );		/* output tone curve */


/*-----------------------------  Make Spline Curve   ---------------------------------*/

typedef struct {
	CNMSInt16	h;
	CNMSInt16	v;
}CNMSSPOINT16;


void CnmsImgSplineCalc(
					CNMSUInt8			*curve,						/* output tone curve (8-bit) */
					CNMSInt32			type					);	/* tone curve type */

void CnmsImgSplineCalc16(
					CNMSUInt16			*curve,						/* output tone curve (16-bit) */
					CNMSInt32			type					);	/* tone curve type */



/*-----------------------------  Scaling Image   ---------------------------------*/

CNMSInt32 CnmsImgScaleFromMemory(
					CNMSUInt8			*SrcImg,					/* input image */
					CNMSUInt8			*DstImg,					/* scaled image */
					CNMSInt32			bpp,						/* bit per pixels */
					CNMSInt32			*SrcSize,					/* original size */
					CNMSInt32			*DstSize				);	/* scaled size */



typedef CNMSInt32 ( *FP_CnmsParamSetOptionColor )( CNMSScanInfo * );
typedef CNMSInt32 ( *FP_CnmsParamSetOptionResolution )( CNMSScanInfo *, CANON_ABILITY_INFO * );
typedef CNMSInt32 ( *FP_CnmsParamSetOptionScanArea )( CNMSScanInfo *, CANON_ABILITY_INFO * );
typedef CNMSInt32 ( *FP_CnmsParamSetOptionGammaTable )( CNMSScanInfo * );

typedef CNMSInt32 ( *FP_CnmsImgProcWorkInit )( CNMSScanInfo *, CNMSVoid ** );
typedef CNMSInt32 ( *FP_CnmsImgProcModInit )( CNMSVoid * );
typedef CNMSInt32 ( *FP_CnmsImgProcCorrectSetBase )( CNMSUInt8 *, CNMSVoid * );
typedef CNMSInt32 ( *FP_CnmsImgProcCorrectSetFull )( CNMSUInt8 *, CNMSVoid * );
typedef CNMSInt32 ( *FP_CnmsImgProcCorrectSetCrop )( CNMSUInt8 *, CNMSVoid * );
typedef CNMSInt32 ( *FP_CnmsImgProcCorrectGet )( CNMSUInt8 **, CNMSVoid * );
typedef CNMSVoid  ( *FP_CnmsImgProcCorrectComplete )( CNMSVoid * );
typedef CNMSVoid  ( *FP_CnmsImgProcTerminate )( CNMSVoid * );

typedef CNMSInt32 ( *FP_CnmsImgProcGetHistogram )( CNMSHistogram *, CNMSHistogram *, CNMSVoid * );
typedef CNMSVoid* ( *FP_CnmsImgProcOriginalDataPGet )( CNMSVoid * );
typedef CNMSVoid  ( *FP_CnmsImgProcOriginalDataPDispose )( CNMSVoid ** );
typedef CNMSInt32 ( *FP_CnmsImgProcGetOriginalRGB )( CNMSVoid *, CNMSInt32, CNMSInt32, CNMSInt32 *, CNMSInt32 *, CNMSInt32 * );

typedef CNMSVoid  ( *FP_CnmsImgMakeToneCurve )( CNMSMAKETONEPARAM *, CNMSRGB32 *, CNMSRGB32 * );
typedef CNMSVoid  ( *FP_CnmsImgMakeToneCurve16 )( CNMSMAKETONEPARAM *, CNMSRGB32 *, CNMSRGB32 * );
typedef CNMSVoid  ( *FP_CnmsImgSplineCalc )( CNMSUInt8 *, CNMSInt32 );
typedef CNMSVoid  ( *FP_CnmsImgSplineCalc16 )( CNMSUInt16 *, CNMSInt32 );

typedef CNMSInt32 ( *FP_CnmsImgScaleFromMemory )( CNMSUInt8 *, CNMSUInt8 *, CNMSInt32, CNMSInt32 *, CNMSInt32 * );


/* cnmsimg api struct */
typedef struct {
	void	*handle;
	
	FP_CnmsParamSetOptionColor			p_CnmsParamSetOptionColor;
	FP_CnmsParamSetOptionResolution		p_CnmsParamSetOptionResolution;
	FP_CnmsParamSetOptionScanArea		p_CnmsParamSetOptionScanArea;
	FP_CnmsParamSetOptionGammaTable		p_CnmsParamSetOptionGammaTable;
	
	FP_CnmsImgProcWorkInit				p_CnmsImgProcWorkInit;
	FP_CnmsImgProcModInit				p_CnmsImgProcModInit;
	FP_CnmsImgProcCorrectSetBase		p_CnmsImgProcCorrectSetBase;
	FP_CnmsImgProcCorrectSetFull		p_CnmsImgProcCorrectSetFull;
	FP_CnmsImgProcCorrectSetCrop		p_CnmsImgProcCorrectSetCrop;
	FP_CnmsImgProcCorrectGet			p_CnmsImgProcCorrectGet;
	FP_CnmsImgProcCorrectComplete		p_CnmsImgProcCorrectComplete;
	FP_CnmsImgProcTerminate				p_CnmsImgProcTerminate;
	
	FP_CnmsImgProcGetHistogram			p_CnmsImgProcGetHistogram;
	FP_CnmsImgProcOriginalDataPGet		p_CnmsImgProcOriginalDataPGet;
	FP_CnmsImgProcOriginalDataPDispose	p_CnmsImgProcOriginalDataPDispose;
	FP_CnmsImgProcGetOriginalRGB		p_CnmsImgProcGetOriginalRGB;
	
	FP_CnmsImgMakeToneCurve				p_CnmsImgMakeToneCurve;
	FP_CnmsImgMakeToneCurve16			p_CnmsImgMakeToneCurve16;
	FP_CnmsImgSplineCalc				p_CnmsImgSplineCalc;
	FP_CnmsImgSplineCalc16				p_CnmsImgSplineCalc16;

	FP_CnmsImgScaleFromMemory			p_CnmsImgScaleFromMemory;
}CNMSIMGAPI;


#if defined(__cplusplus)
}
#endif	/* __cplusplus */

#endif	/* _CNMSSCANPROC_H_ */
