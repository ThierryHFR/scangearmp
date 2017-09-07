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

#ifndef _CNMSDEF_H_
#define _CNMSDEF_H_

#if defined(__cplusplus)
extern "C"
{
#endif	/* __cplusplus */

enum{
	CNMS_OBJ_S_SOURCE = 0,
	CNMS_OBJ_S_DESTINATION,
	CNMS_OBJ_S_OUTPUT_SIZE,
	CNMS_OBJ_A_SOURCE,
	CNMS_OBJ_A_PAPER_SIZE,
	CNMS_OBJ_A_COLOR_MODE,
	CNMS_OBJ_A_INPUT_WIDTH,
	CNMS_OBJ_A_INPUT_HEIGHT,
	CNMS_OBJ_A_RATIO_FIX,
	CNMS_OBJ_A_UNIT,
	CNMS_OBJ_A_OUTPUT_RESOLUTION,
	CNMS_OBJ_A_OUTPUT_SIZE,
	CNMS_OBJ_A_OUTPUT_WIDTH,
	CNMS_OBJ_A_OUTPUT_HEIGHT,
	CNMS_OBJ_A_SCALE,
	CNMS_OBJ_A_DATA_SIZE,
	CNMS_OBJ_A_COLOR_ADJUST,
	CNMS_OBJ_A_COLOR_CONFIG,
	CNMS_OBJ_A_AUTO_TONE,
	CNMS_OBJ_A_UNSHARP_MASK,
	CNMS_OBJ_A_DESCREEN,
	CNMS_OBJ_A_REDUCE_DUST_SCRATCHES,
	CNMS_OBJ_A_REMOVE_DUST_SCRATCHES,
	CNMS_OBJ_A_FADING_CORRECTION,
	CNMS_OBJ_A_GRAIN_CORRECTION,
	CNMS_OBJ_A_BACKLIGHT_CORRECTION,
	/* Add ver.1.20 */
	CNMS_OBJ_A_THRESHOLD_DEFAULT,
	CNMS_OBJ_P_TAB_SCANNER,
	CNMS_OBJ_P_EVERY_CALIBRATION,
	CNMS_OBJ_P_SILENT_MODE,
	CNMS_OBJ_A_IMAGE_CORRECT,
	/* Add ver.1.30 */
	CNMS_OBJ_A_BINDING_LOCATION,
	CNMS_OBJ_UI,
	/* Add ver.2.00 */
	CNMS_OBJ_P_AUTO_POWER_SETTINGS,
	CNMS_OBJ_P_AUTO_POWER_ON,
	CNMS_OBJ_P_AUTO_POWER_OFF,
	CNMS_OBJ_MAX,
};

/* Select Source (Simple Mode) */
enum{
	CNMS_S_SOURCE_PHOTO = 0,		/* Photo(Color) */
	CNMS_S_SOURCE_MAGAZINE,			/* Magazine(Color) */
	CNMS_S_SOURCE_NEWSPAPER,		/* Newspaper(B&W) */
	CNMS_S_SOURCE_DOCUMENT,			/* Document(Grayscale) */
	CNMS_S_SOURCE_NEGAFILM,			/* Negative Film(Color) */
	CNMS_S_SOURCE_POSIFILM,			/* Positive Film(Color) */
	CNMS_S_SOURCE_GRAYNEWSPAPER,	/* Newspaper(Grayscale) */
	CNMS_S_SOURCE_TEXT_COL_SADF,	/* Document(Color) ADF Simplex     */
	CNMS_S_SOURCE_TEXT_GRAY_SADF,	/* Document(Grayscale) ADF Simplex */
	CNMS_S_SOURCE_TEXT_COL_DADF,	/* Document(Color) ADF Duplex      */
	CNMS_S_SOURCE_TEXT_GRAY_DADF,	/* Document(Grayscale) ADF Duplex  */
	CNMS_S_SOURCE_COLDOCUMENT,		/* Document(Color)  */
	CNMS_S_SOURCE_MAX,
};

/* Destination (Simple Mode) */
enum{
	CNMS_S_DESTINATION_PRINT = 0,	/* Print(300dpi) */
	CNMS_S_DESTINATION_DISPLAY,		/* Image display(150dpi) */
	CNMS_S_DESTINATION_OCR,			/* OCR(300dpi) */
	CNMS_S_DESTINATION_PRINT_NDPI,	/* Print */
	CNMS_S_DESTINATION_DISPLAY_NDPI,/* Image display */
	CNMS_S_DESTINATION_OCR_NDPI,	/* OCR */
	CNMS_S_DESTINATION_MAX,
};

/* Output Size (Simple Mode) */
enum{
	CNMS_S_OUTPUT_SIZE_FREE = 0,	/* Flexible */
	CNMS_S_OUTPUT_SIZE_L_LA,		/* L Landscape */
	CNMS_S_OUTPUT_SIZE_L_PO,		/* L Portrait */
	CNMS_S_OUTPUT_SIZE_LL_LA,		/* 2L Landscape */
	CNMS_S_OUTPUT_SIZE_LL_PO,		/* 2L Portrait */
	CNMS_S_OUTPUT_SIZE_4_6_LA,		/* 4"x6" Landscape */
	CNMS_S_OUTPUT_SIZE_4_6_PO,		/* 4"x6" Portrait */
	CNMS_S_OUTPUT_SIZE_POSTCARD,	/* Postcard */
	CNMS_S_OUTPUT_SIZE_A4,			/* A4 */
	CNMS_S_OUTPUT_SIZE_LETTER,		/* Letter */
	CNMS_S_OUTPUT_SIZE_640_480,		/* 640 x 480 pixels */
	CNMS_S_OUTPUT_SIZE_800_600,		/* 800 x 600 pixels */
	CNMS_S_OUTPUT_SIZE_1024_768,	/* 1024 x 768 pixels */
	CNMS_S_OUTPUT_SIZE_1600_1200,	/* 1600 x 1200 pixels */
	CNMS_S_OUTPUT_SIZE_2048_1536,	/* 2048 x 1536 pixels */
	CNMS_S_OUTPUT_SIZE_MAX,
};

/* Select Source : (Advanced Mode) */
enum{
	CNMS_A_SOURCE_PLATEN = 0,		/* Platen */
	CNMS_A_SOURCE_COLOR_NEGA,		/* Color Negative Film */
	CNMS_A_SOURCE_COLOR_POSI,		/* Color Positive Film */
	CNMS_A_SOURCE_MONO_NEGA,		/* Monochrome Negative Film */
	CNMS_A_SOURCE_MONO_POSI,		/* Monochrome Positive Film */
	CNMS_A_SOURCE_TEXT_SADF,		/* Document (ADF Simplex)   */
	CNMS_A_SOURCE_TEXT_DADF,		/* Document (ADF Duplex)    */
	CNMS_A_SOURCE_MAX,
};

/* Paper Size : (Advanced Mode) */
enum{
	CNMS_A_PAPER_SIZE_CARD = 0,		/* Business Card */
	CNMS_A_PAPER_SIZE_L_LA,			/* L Landscape */
	CNMS_A_PAPER_SIZE_L_PO,			/* L Portrait */
	CNMS_A_PAPER_SIZE_POSTCARD_LA,	/* Postcard Landscape */
	CNMS_A_PAPER_SIZE_POSTCARD_PO,	/* Postcard Portrait */
	CNMS_A_PAPER_SIZE_LL_LA,		/* 2L Landscape */
	CNMS_A_PAPER_SIZE_LL_PO,		/* 2L Portrait */
	CNMS_A_PAPER_SIZE_A5_LA,		/* A5 Landscape */
	CNMS_A_PAPER_SIZE_A5_PO,		/* A5 Portrait */
	CNMS_A_PAPER_SIZE_B5,			/* B5 */
	CNMS_A_PAPER_SIZE_A4,			/* A4 */
	CNMS_A_PAPER_SIZE_LETTER,		/* Letter */
	CNMS_A_PAPER_SIZE_FULL_PAGE,	/* Full Platen */
	CNMS_A_PAPER_SIZE_FILM_35MM,	/* 35mm Strip */
	CNMS_A_PAPER_SIZE_A5,			/* A5 */
	CNMS_A_PAPER_SIZE_LEGAL,		/* Legal */
	CNMS_A_PAPER_SIZE_MAX,
};

/* Binding Location */
enum{
	CNMS_A_BINDING_LOCATION_LONG = 0,	/* Long Edge  */
	CNMS_A_BINDING_LOCATION_SHORT,		/* Short Edge */
	CNMS_A_BINDING_LOCATION_MAX,
};

/* Color Mode : (Advanced Mode) */
enum{
	CNMS_A_COLOR_MODE_COLOR = 0,		/* Color */
	CNMS_A_COLOR_MODE_COLOR_DOCUMENTS,	/* Color(Documents) */
	CNMS_A_COLOR_MODE_GRAY,				/* Grayscale */
	CNMS_A_COLOR_MODE_MONO,				/* Black and White */
	CNMS_A_COLOR_MODE_TEXT_ENHANCED,	/* Text Enhanced */
	CNMS_A_COLOR_MODE_MAX,
};

/* Input Width : (Advanced Mode) */
enum{
	CNMS_VALUE_SHOW = 0,
	CNMS_VALUE_NULL,
	CNMS_VALUE_MAX,
};

/* Input Height : (Advanced Mode) */
/* use same value of Input Width */

/* Ratio Fix : (Advanced Mode) */
enum{
	CNMS_TOGGLE_OFF = 0,
	CNMS_TOGGLE_ON,
	CNMS_TOGGLE_MAX,
};

/* Unit : (Advanced Mode) */
enum{
	CNMS_A_UNIT_PIXEL = 0,
	CNMS_A_UNIT_INCH,
	CNMS_A_UNIT_CENTIMETER,
	CNMS_A_UNIT_MILLIMETER,
	CNMS_A_UNIT_MAX,
};

/* Output Resolution : (Advanced Mode) */
enum{
	CNMS_A_OUTPUT_RESOLUTION_50 = 0,	/* 50 */
	CNMS_A_OUTPUT_RESOLUTION_75,		/* 75 */
	CNMS_A_OUTPUT_RESOLUTION_100,		/* 100 */
	CNMS_A_OUTPUT_RESOLUTION_150,		/* 150 */
	CNMS_A_OUTPUT_RESOLUTION_200,		/* 200 */
	CNMS_A_OUTPUT_RESOLUTION_300,		/* 300 */
	CNMS_A_OUTPUT_RESOLUTION_400,		/* 400 */
	CNMS_A_OUTPUT_RESOLUTION_600,		/* 600 */
	CNMS_A_OUTPUT_RESOLUTION_800,		/* 800 */
	CNMS_A_OUTPUT_RESOLUTION_1200,		/* 1200 */
	CNMS_A_OUTPUT_RESOLUTION_1600,		/* 1600 */
	CNMS_A_OUTPUT_RESOLUTION_2400,		/* 2400 */
	CNMS_A_OUTPUT_RESOLUTION_4800,		/* 4800 */
	CNMS_A_OUTPUT_RESOLUTION_MAX,
};

/* Output Size : (Advanced Mode) */
enum{
	CNMS_A_OUTPUT_SIZE_FREE = 0,	/* Flexible */
	CNMS_A_OUTPUT_SIZE_CARD,		/* Card */
	CNMS_A_OUTPUT_SIZE_L_LA,		/* L Landscape */
	CNMS_A_OUTPUT_SIZE_L_PO,		/* L Portrait */
	CNMS_A_OUTPUT_SIZE_LL_LA,		/* 2L Landscape */
	CNMS_A_OUTPUT_SIZE_LL_PO,		/* 2L Portrait */
	CNMS_A_OUTPUT_SIZE_4_6_LA,		/* 4"x6" Landscape */
	CNMS_A_OUTPUT_SIZE_4_6_PO,		/* 4"x6" Portrait */
	CNMS_A_OUTPUT_SIZE_POSTCARD,	/* Postcard */
	CNMS_A_OUTPUT_SIZE_A5,			/* A5 */
	CNMS_A_OUTPUT_SIZE_B5,			/* B5 */
	CNMS_A_OUTPUT_SIZE_A4,			/* A4 */
	CNMS_A_OUTPUT_SIZE_LETTER,		/* Letter */
	CNMS_A_OUTPUT_SIZE_LEGAL,		/* Legal */
	CNMS_A_OUTPUT_SIZE_B4,			/* B4 */
	CNMS_A_OUTPUT_SIZE_A3,			/* A3 */
	CNMS_A_OUTPUT_SIZE_128_128,		/* 128 x 128 pixels */
	CNMS_A_OUTPUT_SIZE_640_480,		/* 640 x 480 pixels */
	CNMS_A_OUTPUT_SIZE_800_600,		/* 800 x 600 pixels */
	CNMS_A_OUTPUT_SIZE_1024_768,	/* 1024 x 768 pixels */
	CNMS_A_OUTPUT_SIZE_1280_1024,	/* 1280 x 1024 pixels */
	CNMS_A_OUTPUT_SIZE_1600_1200,	/* 1600 x 1200 pixels */
	CNMS_A_OUTPUT_SIZE_2048_1536,	/* 2048 x 1536 pixels */
	CNMS_A_OUTPUT_SIZE_2272_1704,	/* 2272 x 1704 pixels */
	CNMS_A_OUTPUT_SIZE_2592_1944,	/* 2592 x 1944 pixels */
	CNMS_A_OUTPUT_SIZE_MAX,
};

/* Output Width : (Advanced Mode) */
/* use same value of Input Width */

/* Output Height : (Advanced Mode) */
/* use same value of Input Width */

/* Scale : (Advanced Mode) */
/* use same value of Input Width */

/* Data Size : (Advanced Mode) */
/* use same value of Input Width */

/* Color Adjust : (Advanced Mode) */
enum{
	CNMS_A_COLOR_ADJUST_BC = 0,	/* Brightness/Contrast */
	CNMS_A_COLOR_ADJUST_HG,		/* Histogram */
	CNMS_A_COLOR_ADJUST_TC,		/* Tone Curve Settings */
	CNMS_A_COLOR_ADJUST_FR,		/* Final Review */
	CNMS_A_COLOR_ADJUST_MC,		/* Threshold */
/*	CNMS_A_COLOR_ADJUST_RESET,*/	/* Reset */
	CNMS_A_COLOR_ADJUST_MAX,
};

/* Auto Tone : (Advanced Mode) */
enum{
	CNMS_A_AUTO_TONE_OFF = 0,	/* OFF */
	CNMS_A_AUTO_TONE_ON,		/* ON */
	CNMS_A_AUTO_TONE_MAX,
};

/* Color Config : (Advanced Mode) */
enum{
	CNMS_A_COLOR_CONFIG_RECOM = 0,	/* Recommended */
	CNMS_A_COLOR_CONFIG_MATCH,		/* Color Matching */
	CNMS_A_COLOR_CONFIG_NON,		/* None */
	CNMS_A_COLOR_CONFIG_MAX,
};

/* Image Correction : (Advanced Mode) */
enum{
	CNMS_A_IMAGE_CORRECT_AUSM = 0,	/* Unsharp Mask */
	CNMS_A_IMAGE_CORRECT_MOIRE,		/* Descreen */
	CNMS_A_IMAGE_CORRECT_MAX,
};

/* Unsharp Mask : (Advanced Mode) */
enum{
	CNMS_A_UNSHARP_MASK_OFF = 0,	/* OFF */
	CNMS_A_UNSHARP_MASK_ON,			/* ON */
	CNMS_A_UNSHARP_MASK_MAX,
};

/* Descreen : (Advanced Mode) */
enum{
	CNMS_A_DESCREEN_OFF = 0,	/* OFF */
	CNMS_A_DESCREEN_ON,			/* ON */
	CNMS_A_DESCREEN_MAX,
};

/* Reduce Dust and Scratches : (Advanced Mode) */
enum{
	CNMS_A_REDUCE_DUST_SCRATCHES_OFF = 0,	/* None */
	CNMS_A_REDUCE_DUST_SCRATCHES_LOW,		/* Low */
	CNMS_A_REDUCE_DUST_SCRATCHES_MEDIUM,	/* Medium */
	CNMS_A_REDUCE_DUST_SCRATCHES_HIGH,		/* High */
	CNMS_A_REDUCE_DUST_SCRATCHES_MAX,
};

/* Remove Dust and Scratches : (Advanced Mode) */
enum{
	CNMS_A_REMOVE_DUST_SCRATCHES_OFF = 0,	/* OFF */
	CNMS_A_REMOVE_DUST_SCRATCHES_ON,		/* ON */
	CNMS_A_REMOVE_DUST_SCRATCHES_MAX,
};

/* Fading Correction : (Advanced Mode) */
enum{
	CNMS_A_FADING_CORRECTION_OFF = 0,	/* None */
	CNMS_A_FADING_CORRECTION_LOW,		/* Low */
	CNMS_A_FADING_CORRECTION_NORMAL,	/* Medium */
	CNMS_A_FADING_CORRECTION_HIGH,		/* High */
	CNMS_A_FADING_CORRECTION_MAX,
};

/* Grain Correction : (Advanced Mode) */
enum{
	CNMS_A_GRAIN_CORRECTION_OFF = 0,	/* None */
	CNMS_A_GRAIN_CORRECTION_LOW,		/* Low */
	CNMS_A_GRAIN_CORRECTION_NORMAL,		/* Medium */
	CNMS_A_GRAIN_CORRECTION_HIGH,		/* High */
	CNMS_A_GRAIN_CORRECTION_MAX,
};

/* Backlight Correction : (Advanced Mode) */
enum{
	CNMS_A_BACKLIGHT_CORRECTION_OFF = 0,	/* None */
	CNMS_A_BACKLIGHT_CORRECTION_LOW,		/* Low */
	CNMS_A_BACKLIGHT_CORRECTION_NORMAL,		/* Medium */
	CNMS_A_BACKLIGHT_CORRECTION_HIGH,		/* High */
	CNMS_A_BACKLIGHT_CORRECTION_MAX,
};

/* Default Threshold : (Advanced Mode) */
enum{
	CNMS_A_THRESHOLD_DEFAULT_72 = 0,	/* 72 */
	CNMS_A_THRESHOLD_DEFAULT_128,		/* 128 */
	CNMS_A_THRESHOLD_DEFAULT_MAX,
};

/* Scanner Tab : (Preference) */
/* use same value of Input Width */

/* Calibration Setting : (Preference) */
enum{
	CNMS_P_EVERY_CALIBRATION_OFF = 0,	/* OFF */
	CNMS_P_EVERY_CALIBRATION_ON,		/* ON */
	CNMS_P_EVERY_CALIBRATION_MAX,
};

/* Silent Mode : (Preference) */
enum{
	CNMS_P_SILENT_MODE_OFF = 0,			/* OFF */
	CNMS_P_SILENT_MODE_ON,				/* ON */
	CNMS_P_SILENT_MODE_NULL,			/* hide */
	CNMS_P_SILENT_MODE_TYPE2,			/* quiet mode (no message) */
	CNMS_P_SILENT_MODE_TYPE3,			/* quiet mode */
	CNMS_P_SILENT_MODE_MAX,
};

/* Auto Power Settings : (Preference) */
enum{
	CNMS_P_AUTO_POWER_SETTINGS_NULL = 0,	/* hide */
	CNMS_P_AUTO_POWER_SETTINGS_TYPE1,		/* Auto Power ON/OFF support */
	CNMS_P_AUTO_POWER_SETTINGS_MAX,
};

/* Auto Power On : (Preference) */
enum{
	CNMS_P_AUTO_POWER_ON_DISABLE = 0,	/* disable */
	CNMS_P_AUTO_POWER_ON_ENABLE,		/* enable */
	CNMS_P_AUTO_POWER_ON_MAX,
};

/* Auto Power Off : (Preference) */
enum{
	CNMS_P_AUTO_POWER_OFF_DISABLE = 0,	/* disable */
	CNMS_P_AUTO_POWER_OFF_15MIN,		/* 15min. */
	CNMS_P_AUTO_POWER_OFF_30MIN,		/* 30min. */
	CNMS_P_AUTO_POWER_OFF_60MIN,		/* 60min. */
	CNMS_P_AUTO_POWER_OFF_120MIN,		/* 120min. */
	CNMS_P_AUTO_POWER_OFF_240MIN,		/* 240min. */
	CNMS_P_AUTO_POWER_OFF_MAX,
};


/* UI Information */
enum{
	CNMS_UI_S_PREVIEW = 0,				/* Preview button (Basic Mode)       */
	CNMS_UI_S_OUTPUT_SIZE,				/* Output size (Basic Mode)          */
	CNMS_UI_S_CROP_ADJUST,				/* Crop Area Adjustment (Basic Mode) */
	CNMS_UI_S_SCAN_STR2,				/* Number [2] of Scan (Basic Mode)   */
	CNMS_UI_S_SCAN_STR3,				/* Number [3] of Scan (Basic Mode)   */
	CNMS_UI_A_BINDING_LOCATION,			/* Binding Location (Advanced Mode)  */
	CNMS_UI_A_OUTPUT_SIZE,				/* Output size (Advanced Mode)       */
	CNMS_UI_A_PREVIEW,					/* Preview button (Advanced Mode)    */
	CNMS_UI_MAX,
};

enum{
	CNMS_MODE_DELETE = 0,
	CNMS_MODE_SELECT,
	CNMS_MODE_ADD,
	CNMS_MODE_CHANGE,
	CNMS_MODE_HIDE,
};

enum{
	CNMS_ASPECT_VARIED = 0,
	CNMS_ASPECT_FIXED,
	CNMS_ASPECT_STANDERD,
	CNMS_ASPECT_MAX,
};

/* Cursor ID */
#define	CNMS_CURSOR_ID_LT_RB	(14)
#define	CNMS_CURSOR_ID_T_B		(16)
#define	CNMS_CURSOR_ID_RT_LB	(12)
#define	CNMS_CURSOR_ID_OUTSIDE	(31)
#define	CNMS_CURSOR_ID_L_R		(70)
#define	CNMS_CURSOR_ID_INSIDE	(52)
#define	CNMS_CURSOR_ID_PEN		(86)
#define	CNMS_CURSOR_ID_CROSS	(66)
#define	CNMS_CURSOR_ID_NORMAL	(68)

enum{
	CNMS_FORMATTYPE_VER_100 = 7,
	CNMS_FORMATTYPE_VER_110,
	CNMS_FORMATTYPE_VER_120,
	CNMS_FORMATTYPE_VER_130,
	CNMS_FORMATTYPE_VER_MAX,
};

#if defined(__cplusplus)
}
#endif	/* __cplusplus */

#endif	/* _CNMSDEF_H_ */
