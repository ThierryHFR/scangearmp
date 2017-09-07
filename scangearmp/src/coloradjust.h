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


#ifndef	_COLORADJUST_H_
#define	_COLORADJUST_H_

#ifdef	_CNMS_CA_GLOBALS_
	#define	CNMS_CA_GLOBALS
#else
	#define	CNMS_CA_GLOBALS	extern
#endif

#include <gtk/gtk.h>
#include <string.h>
#include <math.h>

#include "interface.h"
#include "support.h"
#include "scanflow.h"

#include "w1.h"
#include "cnmstype.h"

#include "coloradjuststr.h"

#define		CNMS_CA_DIALOG_NON_KEEP		(	0)		/* Use for XX_dialog_show()							*/
#define		CNMS_CA_DIALOG_KEEP			(	1)		/* Ditto											*/

#define		CNMS_CA_VECTOR_LEN			(  256)		/* Table length  8bit (Number of member)			*/
#define		CNMS_CA_VECTOR_LONGLEN		(65536)		/* Table length 16bit (Number of member)			*/
#define		CNMS_CA_STRING_LEN			(  128)		/* Standard string length							*/

#define		CNMS_CA_GRAPH_FRAME_LEFT	(	3)		/* Graph drawing area frame coordinate 	: Left		*/
#define		CNMS_CA_GRAPH_FRAME_TOP		(	3)		/* Ditto								: Top		*/
#define		CNMS_CA_GRAPH_FRAME_RIGHT	( 227)		/* Ditto								: Right		*/
#define		CNMS_CA_GRAPH_FRAME_BOTTOM	( 227)		/* Ditto								: Bottom	*/

#define		CNMS_CA_HEIGHT_TITLE_BAR	(  20)		/* Title Bar Height for Dialog show position		*/

#define		CNMS_CA_MC_DEFAULT			(  72)		/* Default threshold								*/

#define		CNMS_CA_GC_BACKGROUND_R		(0xEFEF)	/* 239 GC color R for graph background				*/
#define		CNMS_CA_GC_BACKGROUND_G		(0xEBEB)	/* 235 GC color G for graph background				*/
#define		CNMS_CA_GC_BACKGROUND_B		(0xE7E7)	/* 231 GC color B for graph background				*/

#define		CNMS_CA_GC_FRAME_R			(0xB1B1)	/* 177 GC color R for graph frame					*/
#define		CNMS_CA_GC_FRAME_G			(0xA5A5)	/* 165 GC color G for graph frame					*/
#define		CNMS_CA_GC_FRAME_B			(0x9898)	/* 152 GC color B for graph frame					*/

#define		CNMS_CA_GC_VALUE_MAX		(0xFFFF)	/* 255 GC color value max							*/
#define		CNMS_CA_GC_VALUE_ZERO		(0x0000)	/*   0 GC color value zero							*/
#define		CNMS_CA_GC_VALUE_GRAY		(0x7777)	/* 119 GC color value gray							*/


/*******************************************/
/***        Define for ColorAdjust       ***/
/*******************************************/

	CNMS_CA_GLOBALS	CNMSMAKETONEPARAM	ca_param_all;
	CNMS_CA_GLOBALS	CNMSRGB32			ca_usertone[CNMS_CA_VECTOR_LEN];
	CNMS_CA_GLOBALS	CNMSRGB32			ca_resulttone[CNMS_CA_VECTOR_LEN];
	CNMS_CA_GLOBALS	CNMSRGB32			ca_usertone16[CNMS_CA_VECTOR_LONGLEN];
	CNMS_CA_GLOBALS	CNMSRGB32			ca_resulttone16[CNMS_CA_VECTOR_LONGLEN];
	
	CNMS_CA_GLOBALS	GdkColor			ca_bg_color;

typedef struct{
		CNMSInt32			colorgroup;					/* Current Color Group						*/
		CNMSInt32			page;						/* Current Page								*/
		CNMSBool			preview;					/* TRUE:Preview ON, FALSE:Preview OFF		*/
		CNMSBool			clearcbox;					/* TRUE:Clear, FALSE;Other					*/
		CNMSBool			reloadhist;					/* TRUE:Need reload, FALSE:Not Need reload	*/
}CNMSCAInfo;

	CNMS_CA_GLOBALS CNMSCAInfo			ca_info;		/* ColorAdjust Information					*/

enum{	/* Channel ID */
	CNMS_CA_MASTER = 0,				/* Master		*/
	CNMS_CA_RED,					/* Red			*/
	CNMS_CA_GREEN,					/* Green		*/
	CNMS_CA_BLUE,					/* Blue		 	*/
	CNMS_CA_GRAY,					/* Gray		 	*/
	CNMS_CA_MAX,
};

enum{	/* Color Group */
	CNMS_CA_COLORGROUP_ERROR = -1,	/* Error		*/
	CNMS_CA_COLORGROUP_COLOR,		/* Color		*/
	CNMS_CA_COLORGROUP_GRAY,		/* Grayscale	*/
	CNMS_CA_COLORGROUP_MONO,		/* Monochrome 	*/
	CNMS_CA_COLORGROUP_MAX,
};

enum{	/* Color number, Use for gc set color */
	CNMS_CA_COLOR_BLACK = 0,		/* Black		*/
	CNMS_CA_COLOR_RED,				/* Red			*/
	CNMS_CA_COLOR_GREEN,			/* Green		*/
	CNMS_CA_COLOR_BLUE,				/* Blue			*/
	CNMS_CA_COLOR_GRAY,				/* Gray			*/
	CNMS_CA_COLOR_MAX,
};

enum{	/* Dialog number */
	CNMS_CA_DIALOG_BC = 0,			/* Brightness/Contrast	*/
	CNMS_CA_DIALOG_HG,				/* Histogram			*/
	CNMS_CA_DIALOG_TC,				/* ToneCurve Setting	*/
	CNMS_CA_DIALOG_FR,				/* Final Review			*/
	CNMS_CA_DIALOG_MC,				/* Monochrome			*/
	CNMS_CA_DIALOG_MAX,
};

typedef struct{
		GtkWidget			*widget;					/* Dialog Widget						*/
		CNMSInt32			id;							/* Dialog ID							*/
		GtkWidget			*cbox;						/* Channel Conbobox on Dialog			*/
		CNMSInt32			channel;					/* Current Channel						*/
		GtkWidget			*darea;						/* Drawing Area Widget					*/
		GdkPixmap			*pmap;						/* Pixmap in Drawing Area				*/
		GdkGC				*gc;						/* Graphic Context in Drawing Area		*/
		CNMSInt 			x;							/* Dialog X position					*/ 
		CNMSInt 			y;							/* Dialog Y position					*/ 
		CNMSBool			show;						/* Flag for Dialog show/hide			*/
		CNMSInt32			*curve[CNMS_CA_MAX];		/* Tone Curve Data						*/
		CNMSInt32			fcurve;						/* Number of Using Curve				*/
		CNMSUInt32			*hist[CNMS_CA_MAX];			/* Histogram Data						*/
		CNMSInt32			fhist;						/* Number of Using Histogram			*/
}CNMSCADialog;

typedef struct{
		CNMSInt32	x;
		CNMSInt32	value;
		CNMSDec32	range;
		CNMSBool	flag;
}CNMSCAScaleBarInfo;

/*******************************************/
/***    Define for Brightness/Contrast   ***/
/*******************************************/

enum{
	CNMS_CA_ID_BRIGHT = 0,			/* Brightness		*/
	CNMS_CA_ID_CONTRAST,			/* Contrast			*/
};

typedef struct{
		CNMSDec32 			bright;								/* Brightness value					*/
		CNMSDec32 			contrast;							/* Contrast value					*/
}CNMSBCValue;

	CNMS_CA_GLOBALS GtkWidget		*bc_scale_bright,			/* ScaleBar   for brightness		*/
	                                *bc_spinbutton_bright,		/* SpinBottun for brightness		*/
	                                *bc_scale_contrast,			/* ScaleBar   for contrast			*/
	                                *bc_spinbutton_contrast;	/* SpinBottun for contrast			*/

	CNMS_CA_GLOBALS	CNMSCADialog 	bc_dlg;						/* Pixmap For Graph Drawing Area 	*/
	CNMS_CA_GLOBALS	CNMSBCValue		bc[CNMS_CA_MAX];

/*******************************************/
/***         Define for Histogram        ***/
/*******************************************/

enum{	/* Spuit button number */
	CNMS_HG_SPUIT_ERR = -1,			/* Error								*/
	CNMS_HG_SPUIT_GRAYLEVEL,		/* Gray level balance					*/
	CNMS_HG_SPUIT_SHADOW,			/* Shadow								*/
	CNMS_HG_SPUIT_MID,				/* Mid									*/
	CNMS_HG_SPUIT_HIGHLIGHT,		/* Highlight							*/
	CNMS_HG_SPUIT_MAX,
};

enum{
	CNMS_HG_GRAB_ERR = -1,			/* Error								*/
	CNMS_HG_GRAB_NO_GRAB,			/* Not Grab								*/
	CNMS_HG_GRAB_SHADOW,			/* Grab on Shadow						*/
	CNMS_HG_GRAB_MID,				/* Grab on Mid							*/
	CNMS_HG_GRAB_HIGHLIGHT,			/* Grab on Highlight					*/
	CNMS_HG_GRAB_MAX,
};

typedef struct{
		CNMSDec32 			raw;
		CNMSInt32			disp;
		CNMSInt32			lower;
		CNMSInt32			upper;
		CNMSInt32			center;
}CNMSHGScaleBarPos;

typedef struct{
		CNMSHGScaleBarPos	shadow;
		CNMSHGScaleBarPos	mid;
		CNMSHGScaleBarPos	highlight;
}CNMSHGValue;

	CNMS_CA_GLOBALS GtkWidget		*hg_entry_shadow,			/* TextEntry for Shadow				*/
	                                *hg_entry_mid,				/* TextEntry for Mid				*/
	                                *hg_entry_highlight,		/* TextEntry for Highlight			*/
	                                *hg_scalebar_drawarea,		/* DrawingArea for ScaleBar			*/
	                                *hg_label_str_1,			/* Label 1 for R/K					*/
	                                *hg_label_str_2,			/* Label 2 for G					*/
	                                *hg_label_str_3,			/* Label 3 for B					*/
	                                *hg_label_value_1,			/* Value 1 for R/K					*/
	                                *hg_label_value_2,			/* Value 2 for G					*/
	                                *hg_label_value_3,			/* Value 3 for B					*/
	                                *hg_togglebutton_gbal;		/* Gray Balance Toggle Button		*/

	CNMS_CA_GLOBALS	CNMSCADialog 	hg_dlg;						/* Pixmap For Graph Drawing Area 	*/
	CNMS_CA_GLOBALS	CNMSHGValue		hg[CNMS_CA_MAX];

/*******************************************/
/***         Define for ToneCurve        ***/
/*******************************************/

typedef struct{
		CNMSInt32			tc_num;								/* ToneCurve number					*/
}CNMSTCValue;

#define		CNMS_TC_NO_ADJUST	CNMS_SPLINE_NO_ADJUST			/*	0 	*/
#define		CNMS_TC_EXP_OVER	CNMS_SPLINE_EXP_OVER			/*  1 	*/
#define		CNMS_TC_EXP_UNDER	CNMS_SPLINE_EXP_UNDER			/*  2 	*/
#define		CNMS_TC_HIGH_CONT	CNMS_SPLINE_HIGH_CONT			/*  3 	*/

	CNMS_CA_GLOBALS GtkWidget		*tc_combobox_tone_select;	/* ToneCurve Select ComboBox		*/

	CNMS_CA_GLOBALS	CNMSCADialog 	tc_dlg;						/* Pixmap For Graph Drawing Area 	*/
	CNMS_CA_GLOBALS	CNMSTCValue		tc[CNMS_CA_MAX];

/*******************************************/
/***        Define for FinalReview       ***/
/*******************************************/

	CNMS_CA_GLOBALS GtkWidget		*fr_label_str_a1,			/* Label 1 After  for R/K			*/
	                                *fr_label_str_b1,	        /* Label 1 Before for R/K			*/
	                                *fr_label_str_a2,	        /* Label 2 After  for G				*/
	                                *fr_label_str_b2,	        /* Label 2 Before for G				*/
	                                *fr_label_str_a3,	        /* Label 3 After  for B				*/
	                                *fr_label_str_b3,	        /* Label 3 Before for B				*/
	                                *fr_label_value_a1,			/* Value 1 After  for R/K			*/
	                                *fr_label_value_b1,			/* Value 1 Before for R/K			*/
	                                *fr_label_value_a2,			/* Value 2 After  for G				*/
	                                *fr_label_value_b2,			/* Value 2 Before for G				*/
	                                *fr_label_value_a3,			/* Value 3 After  for B				*/
	                                *fr_label_value_b3;			/* Value 3 Before for B				*/

	CNMS_CA_GLOBALS	CNMSCADialog 	fr_dlg;						/* Pixmap For Graph Drawing Area 	*/

/*******************************************/
/***        Define for Monochrome        ***/
/*******************************************/

typedef struct{
		CNMSInt32			threshold;							/* Threshold						*/
}CNMSMCValue;

	CNMS_CA_GLOBALS GtkWidget		*mc_spinbutton,				/* SpinButton for Threshold			*/
	                                *mc_scale;					/* ScaleBar for Threshold			*/

	CNMS_CA_GLOBALS	CNMSCADialog 	mc_dlg;						/* Pixmap For Graph Drawing Area 	*/
	CNMS_CA_GLOBALS	CNMSMCValue		mc_mono;
	

/*******************************************/
/***              Function               ***/
/*******************************************/


CNMSInt32 CnmsColAdjDrawAreaBaseDraw(
						GtkWidget			*widget,
						GdkPixmap			*pixmap,
						GdkGC				*gc						);

CNMSInt32 CnmsColAdjDrawAreaCurveDraw(
						GdkPixmap			*pixmap,
						GdkGC				*gc,
						CNMSInt32			curve[],
						CNMSInt32			color					);

CNMSInt32 CnmsColAdjDrawAreaHistogramDraw(
						GdkPixmap			*pixmap,
						GdkGC				*gc,
						CNMSUInt32			histogram[]				);

CNMSInt32 CnmsColAdjGetColorGroupNum(
						CNMSVoid									);

CNMSVoid CnmsColAdjComboBoxItemAllClear(
						GtkComboBox			*combobox				);

CNMSVoid CnmsColAdjGdkGCSetColor(
						GdkGC				*gc,
						CNMSUInt16			r,
						CNMSUInt16			g,
						CNMSUInt16			b						);

CNMSVoid CnmsColAdjCurveCalcBC(
						CNMSMAKETONEPARAM	*base_param				);

CNMSVoid CnmsColAdjCurveCalcHG(
						CNMSMAKETONEPARAM	*base_param				);

CNMSVoid CnmsColAdjCurveCalcFR(
						CNMSVoid									);

CNMSVoid CnmsColAdjPreviewImageSet(
						CNMSVoid									);

CNMSVoid CnmsColAdjDialogInfoInit(
						CNMSInt32			id						);

CNMSVoid CnmsColAdjSetZeroIfMinusZeroForSpinbutton(
						GtkWidget			*spinbutton				);

CNMSVoid CnmsColAdjSetCurrentPage(
						CNMSInt32			pagenum					);

CNMSBool CnmsColAdjChannelItemSet(
						CNMSInt32			color,
						CNMSInt32			id						);

CNMSVoid CnmsColAdjToneCurveDraw(
						CNMSInt32			id						);

CNMSVoid CnmsColAdjInitCurveAndHistogram(
						CNMSCADialog		*dlg					);

CNMSVoid CnmsColAdjClearHistogram(
						CNMSVoid									);

CNMSVoid CnmsColAdjGetPreviewStatus(
						CNMSBool			prevFlag				);

CNMSVoid CnmsColAdjUpDateHistogram(
						CNMSVoid									);

CNMSVoid CnmsColAdjSetChannelItem(
						CNMSCADialog		*dlg					);

CNMSInt32 CnmsColAdjGetChannelIDFromCombobox(
						GtkComboBox			*combobox				);

CNMSInt32 CnmsColAdjInit(
						CNMSVoid									);

CNMSInt32 CnmsColAdjColorModeChanged(
						CNMSVoid									);

CNMSVoid CnmsColAdjButtonClicked(
						CNMSInt32			ButtonNum				);

CNMSVoid CnmsColAdjAllDataReset(
						CNMSVoid									);

CNMSVoid CnmsColAdjResetComplete(
						CNMSVoid									);

CNMSInt32 CnmsColAdjSetHistogram(
						CNMSUInt32			type,
						CNMSUInt32			masterSrc[],
						CNMSUInt32			redSrc[],
						CNMSUInt32			greenSrc[],
						CNMSUInt32			blueSrc[],
						CNMSUInt32			graySrc[]				);

CNMSVoid CnmsColAdjParamInit(
						CNMSMAKETONEPARAM	*param,
						CNMSRGB32			*usertone				);

CNMSVoid CnmsColAdjSetUserTone(
						CNMSUInt16			*R,
						CNMSUInt16			*G,
						CNMSUInt16			*B						);

CNMSInt32 CnmsColAdjInitThresholdDefault(
						CNMSVoid									);

CNMSVoid CnmsColAdjDrawingAreaExposeEvent(
						GtkWidget			*widget,
						GdkEventExpose		*event,
						CNMSInt32			id						);

CNMSVoid CnmsColAdjDrawingAreaConfigureEvent(
						GtkWidget			*widget,
						GdkEventConfigure	*event,
						CNMSInt32			id						);

CNMSInt32 CnmsColAdjDialogShowHide(
						CNMSInt32			id,
						CNMSBool			show					);

#endif	/* _COLORADJUST_H_ */
