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

#ifndef	_W1_MAIN_C_
#define	_W1_MAIN_C_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif


/* #define	__CNMS_DEBUG_UI__ */


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <gtk/gtk.h>
#include <gtk/gtkpreview.h>

#include "interface.h"
#include "support.h"

#include "cnmstype.h"
#include "cnmsdef.h"
#include "cnmsstr.h"
#include "cnmsfunc.h"
#include "com_gtk.h"

#include "w1.h"
#include "prev_main.h"
#include "preference.h"
#include "scanmain.h"
#include "save_dialog.h"
#include "cnmsability.h"
#include "progress_bar.h"
#include "child_dialog.h"
#include "scanmsg.h"
#include "keep_setting.h"
#include "scanflow.h"
#include "cnmsnetwork.h"

static CNMSBool	ui_flag = CNMS_FALSE;
static CNMSBool	swPaA_flag = CNMS_FALSE;

enum{
	CNMS_LOCALE_EN = 0,
	CNMS_LOCALE_TR,
	CNMS_LOCALE_OTHER,
	CNMS_LOCALE_MAX,
};

typedef struct{
	CNMSInt32	id;
	CNMSLPSTR	str;
}CNMS_CLEARPREVIEW_MSG, *LPCNMS_CLEARPREVIEW_MSG;

static const CNMS_CLEARPREVIEW_MSG clearPrevMsg[] = {
	{ CLEARPREVIEW_TYPE_NORMAL			, MSG_CLEAR_PREVIEW_NORMAL			},
	{ CLEARPREVIEW_TYPE_CHANGE_SOURCE	, MSG_CLEAR_PREVIEW_SOURCE_CHANGE	},
	{ CNMS_ERR							, CNMSNULL							},
};

typedef struct{
	CNMSInt32	object;
	CNMSInt32	formatType;
}CNMS_FORMATTYPEINFO, *LPCNMS_FORMATTYPEINFO;

static const CNMS_FORMATTYPEINFO ftypeInfo[] = {
	{ CNMS_OBJ_A_THRESHOLD_DEFAULT	, CNMS_FORMATTYPE_VER_110 },
	{ CNMS_OBJ_A_DESCREEN			, CNMS_FORMATTYPE_VER_110 },
	{ CNMS_OBJ_P_TAB_SCANNER		, CNMS_FORMATTYPE_VER_110 },
	{ CNMS_OBJ_P_EVERY_CALIBRATION	, CNMS_FORMATTYPE_VER_110 },
	{ CNMS_OBJ_P_SILENT_MODE		, CNMS_FORMATTYPE_VER_110 },
	{ CNMS_OBJ_A_IMAGE_CORRECT		, CNMS_FORMATTYPE_VER_110 },
	{ CNMS_OBJ_A_BINDING_LOCATION	, CNMS_FORMATTYPE_VER_120 },
	{ CNMS_OBJ_UI					, CNMS_FORMATTYPE_VER_120 },
	{ CNMS_OBJ_P_AUTO_POWER_SETTINGS	, CNMS_FORMATTYPE_VER_130 },
	{ CNMS_OBJ_P_AUTO_POWER_ON			, CNMS_FORMATTYPE_VER_130 },
	{ CNMS_OBJ_P_AUTO_POWER_OFF			, CNMS_FORMATTYPE_VER_130 },
	{ CNMS_ERR						, CNMS_ERR				  },
};

#define W1_PUTBACKOBJ_MAX	(5)

typedef struct{
	CNMSInt32	object;
	CNMSInt32	putback[W1_PUTBACKOBJ_MAX];
}CNMS_UIPUTBACKOBJECT, *LPCNMS_UIPUTBACKOBJECT;

static const CNMS_UIPUTBACKOBJECT uiPutBackObj[] = {
	{ CNMS_OBJ_S_SOURCE			, { CNMS_OBJ_S_DESTINATION	, CNMS_OBJ_S_OUTPUT_SIZE		, CNMS_OBJ_A_PAPER_SIZE			, CNMS_ERR				, CNMS_ERR	} },
	{ CNMS_OBJ_S_OUTPUT_SIZE	, { CNMS_OBJ_A_UNIT			, CNMS_OBJ_A_OUTPUT_RESOLUTION	, CNMS_ERR						, CNMS_ERR				, CNMS_ERR	} },
	{ CNMS_OBJ_A_SOURCE			, { CNMS_OBJ_A_UNIT			, CNMS_OBJ_A_BINDING_LOCATION	, CNMS_OBJ_A_OUTPUT_RESOLUTION	, CNMS_OBJ_A_PAPER_SIZE	, CNMS_ERR	} },
	{ CNMS_OBJ_A_OUTPUT_SIZE	, { CNMS_OBJ_A_UNIT			, CNMS_OBJ_A_OUTPUT_RESOLUTION	, CNMS_ERR						, CNMS_ERR				, CNMS_ERR	} },
	{ CNMS_ERR					, { CNMS_ERR				, CNMS_ERR						, CNMS_ERR						, CNMS_ERR				, CNMS_ERR	} },
};

static const CNMS_UIPUTBACKOBJECT swPaAPutBackObj = {
	  CNMS_OBJ_A_SOURCE			, { CNMS_OBJ_A_COLOR_MODE	, CNMS_OBJ_A_UNSHARP_MASK		, CNMS_OBJ_A_DESCREEN			, CNMS_ERR				, CNMS_ERR	}
};

typedef struct{
	CNMSInt32	object;
	CNMSInt32	value;
	CNMSInt32	group;
}CNMS_UIOBJECTGROUP, *LPCNMS_UIOBJECTGROUP;

static CNMS_UIOBJECTGROUP currentUOGroup[] = {
	{ CNMS_OBJ_S_SOURCE			, CNMS_ERR	, CNMS_ERR },
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_ERR	, CNMS_ERR },
	{ CNMS_OBJ_A_SOURCE			, CNMS_ERR	, CNMS_ERR },
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_ERR	, CNMS_ERR },
	{ CNMS_ERR					, CNMS_ERR	, CNMS_ERR },
};

typedef struct{
	CNMSInt32	object;
	CNMSInt32	num;
	CNMSLPSTR	str;
}CNMS_COMBOSET, *LPCNMS_COMBOSET;

static CNMS_COMBOSET oldComboSet[] = {
	{ CNMS_OBJ_S_SOURCE			, 0			, "simple_src_combo"	},
	{ CNMS_OBJ_A_SOURCE			, 0			, "advance_src_combo"	},
	{ CNMS_ERR					, CNMS_ERR	, CNMSNULL 				},
};

static CNMS_UIOBJECTGROUP oldValue[] = {
	{ CNMS_OBJ_S_DESTINATION		, CNMS_ERR	, CNMS_ERR },
	{ CNMS_OBJ_S_OUTPUT_SIZE		, CNMS_ERR	, CNMS_ERR },
	{ CNMS_OBJ_A_PAPER_SIZE			, CNMS_ERR	, CNMS_ERR },
	{ CNMS_OBJ_A_BINDING_LOCATION	, CNMS_ERR	, CNMS_ERR },
	{ CNMS_OBJ_A_UNIT				, CNMS_ERR	, CNMS_ERR },
	{ CNMS_OBJ_A_OUTPUT_RESOLUTION	, CNMS_ERR	, CNMS_ERR },
	{ CNMS_ERR						, CNMS_ERR	, CNMS_ERR },
};

static CNMS_UIOBJECTGROUP currentValue[] = {
	{ CNMS_OBJ_A_COLOR_MODE			, CNMS_ERR	, CNMS_ERR },
	{ CNMS_OBJ_A_UNSHARP_MASK		, CNMS_ERR	, CNMS_ERR },
	{ CNMS_OBJ_A_DESCREEN			, CNMS_ERR	, CNMS_ERR },
	{ CNMS_ERR						, CNMS_ERR	, CNMS_ERR },
};

static const CNMS_UIOBJECTGROUP uiObjGroup[] = {
	{ CNMS_OBJ_S_SOURCE			, CNMS_S_SOURCE_PHOTO			, CNMS_S_SOURCE_PHOTO			},
	{ CNMS_OBJ_S_SOURCE			, CNMS_S_SOURCE_MAGAZINE		, CNMS_S_SOURCE_PHOTO			},
	{ CNMS_OBJ_S_SOURCE			, CNMS_S_SOURCE_NEWSPAPER		, CNMS_S_SOURCE_PHOTO			},
	{ CNMS_OBJ_S_SOURCE			, CNMS_S_SOURCE_DOCUMENT		, CNMS_S_SOURCE_PHOTO			},
	{ CNMS_OBJ_S_SOURCE			, CNMS_S_SOURCE_GRAYNEWSPAPER	, CNMS_S_SOURCE_PHOTO			},
	{ CNMS_OBJ_S_SOURCE			, CNMS_S_SOURCE_COLDOCUMENT		, CNMS_S_SOURCE_PHOTO			},	/* Ver.1.80 */
	{ CNMS_OBJ_S_SOURCE			, CNMS_S_SOURCE_TEXT_COL_SADF	, CNMS_S_SOURCE_TEXT_COL_SADF	},
	{ CNMS_OBJ_S_SOURCE			, CNMS_S_SOURCE_TEXT_GRAY_SADF	, CNMS_S_SOURCE_TEXT_COL_SADF	},
	{ CNMS_OBJ_S_SOURCE			, CNMS_S_SOURCE_TEXT_COL_DADF	, CNMS_S_SOURCE_TEXT_COL_SADF	},
	{ CNMS_OBJ_S_SOURCE			, CNMS_S_SOURCE_TEXT_GRAY_DADF	, CNMS_S_SOURCE_TEXT_COL_SADF	},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_FREE		, CNMS_S_OUTPUT_SIZE_FREE		},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_L_LA		, CNMS_S_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_L_PO		, CNMS_S_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_LL_LA		, CNMS_S_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_LL_PO		, CNMS_S_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_4_6_LA		, CNMS_S_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_4_6_PO		, CNMS_S_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_POSTCARD	, CNMS_S_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_A4			, CNMS_S_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_LETTER		, CNMS_S_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_640_480	, CNMS_S_OUTPUT_SIZE_640_480	},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_800_600	, CNMS_S_OUTPUT_SIZE_640_480	},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_1024_768	, CNMS_S_OUTPUT_SIZE_640_480	},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_1600_1200	, CNMS_S_OUTPUT_SIZE_640_480	},
	{ CNMS_OBJ_S_OUTPUT_SIZE	, CNMS_S_OUTPUT_SIZE_2048_1536	, CNMS_S_OUTPUT_SIZE_640_480	},
	{ CNMS_OBJ_A_SOURCE			, CNMS_A_SOURCE_PLATEN			, CNMS_A_SOURCE_PLATEN			},
	{ CNMS_OBJ_A_SOURCE			, CNMS_A_SOURCE_TEXT_SADF		, CNMS_A_SOURCE_TEXT_SADF		},
	{ CNMS_OBJ_A_SOURCE			, CNMS_A_SOURCE_TEXT_DADF		, CNMS_A_SOURCE_TEXT_SADF		},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_FREE		, CNMS_A_OUTPUT_SIZE_FREE		},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_CARD		, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_L_LA		, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_L_PO		, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_LL_LA		, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_LL_PO		, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_4_6_LA		, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_4_6_PO		, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_POSTCARD	, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_A5			, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_B5			, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_A4			, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_LETTER		, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_LEGAL		, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_B4			, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_A3			, CNMS_A_OUTPUT_SIZE_A4			},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_128_128	, CNMS_A_OUTPUT_SIZE_640_480	},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_640_480	, CNMS_A_OUTPUT_SIZE_640_480	},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_800_600	, CNMS_A_OUTPUT_SIZE_640_480	},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_1024_768	, CNMS_A_OUTPUT_SIZE_640_480	},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_1280_1024	, CNMS_A_OUTPUT_SIZE_640_480	},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_1600_1200	, CNMS_A_OUTPUT_SIZE_640_480	},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_2048_1536	, CNMS_A_OUTPUT_SIZE_640_480	},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_2272_1704	, CNMS_A_OUTPUT_SIZE_640_480	},
	{ CNMS_OBJ_A_OUTPUT_SIZE	, CNMS_A_OUTPUT_SIZE_2592_1944	, CNMS_A_OUTPUT_SIZE_640_480	},
	{ CNMS_ERR					, CNMS_ERR						, CNMS_ERR						},
};

static CNMSInt32 GetEnvDefaultUnit( LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum );
static CNMSInt32 GetEnvLocaleID( CNMSVoid );
static CNMSInt32 W1_InitObjectFormat( CNMSVoid );
static CNMSInt32 W1_InitUIFormat( CNMSVoid );
static CNMSVoid W1_CheckMoireWarning( CNMSVoid );
static CNMSInt32 W1_GetUIObjGroup( CNMSInt32 object, CNMSInt32 value );
static CNMSInt32 W1_KeepOldValue( CNMSVoid );
static CNMSInt32 W1_KeepCurrentValue( CNMSVoid );
static CNMSInt32 W1_PutBackObject( CNMSInt32 object, CNMSInt32 value );
static CNMSInt32 W1_SwitchPaAPutBack( CNMSVoid );
static LPCNMS_UIOBJECTGROUP W1_GetObjCurrentValue( CNMSInt32 object );

static CNMSBool DisplayDeviceFullName( CNMSVoid );

#define DEFTOSTR1(def) #def
#define DEFTOSTR(def) DEFTOSTR1(def)

CNMSInt32 W1_Open( CNMSInt32 devIndex )
{
	CNMSInt32	ret = CNMS_ERR, ldata;
	CNMSLPSTR	lpStr, lpFullName;
	CNMSByte	lpLibPath[] = DEFTOSTR( CNMSLIBPATH );
	GtkWidget	*widget;
	CNMSByte	window_title[512];

	if( main_window == CNMSNULL ){
		DBGMSG( "[W1_Open]Status is error.\n" );
		goto	EXIT;
	}
	else if( ( lpFullName = CnmsScanGetDevModelStr( CNMS_SCAN_GETDEVMODELSTR_FULL, devIndex ) ) == CNMSNULL ){
		DBGMSG( "[W1_Open]Error is occured in CnmsScanGetDevModelStr.\n" );
		goto	EXIT;
	}
	/* init backend */
	else if( ( lpStr = CnmsScanGetDevModelStr( CNMS_SCAN_GETDEVMODELSTR_MODEL, devIndex ) ) == CNMSNULL ){
		DBGMSG( "[W1_Open]Error is occured in CnmsScanGetDevModelStr.\n" );
		goto	EXIT;
	}
	else if( ( ldata = CnmsScanOpen( devIndex ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Open]Error is occured in CnmsScanOpen.\n" );
		goto	EXIT;
	}
	/* init cnmsimg */
	else if( ( ldata = CnmsImgApiOpen( lpStr ) ) != CNMS_NO_ERR ) {
		DBGMSG( "[W1_Open]Error is occured in CnmsImgApiOpen.\n" );
		goto	EXIT;
	}
	/* init cnmslink */
	else if( ( ldata = CnmsUiLinkOpen( lpStr, lpLibPath ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Open]Error is occured in CnmsUiLinkOpen.\n" );
		goto	EXIT;
	}
	else if( ( ldata = CnmsUiLinkGetSize() ) <= 0 ){
		DBGMSG( "[W1_Open]Error is occured in CnmsUiLinkGetSize.\n" );
		goto	EXIT;
	}
	/* get mem */
	else if( ( lpW1Comp = (LPW1COMP)CnmsGetMem( sizeof( W1COMP ) + ldata * sizeof( CNMSUILINKCOMP ) ) ) == CNMSNULL ){
		DBGMSG( "[W1_Open]Can't get work memory.\n" );
		goto	EXIT;
	}
	/* init link table */
	lpW1Comp->linkNum = ldata;
	lpW1Comp->lpLink = (LPCNMSUILINKCOMP)( (CNMSLPSTR)lpW1Comp + sizeof( W1COMP ) );
	if( ( ldata = CnmsUiLinkDef( lpW1Comp->lpLink ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Open]Error is occured in CnmsUiLinkDef.\n" );
		goto	EXIT;
	}

	/* get format type */
	if( ( lpW1Comp->formatType = CnmsGetFormatType( lpStr, lpLibPath ) ) <= 0 ){
		DBGMSG( "[W1_Open]Error is occured in CnmsUiLinkGetSize.\n" );
		goto	EXIT;
	}
	if( ( ldata = W1_InitObjectFormat() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Open]Error is occured in W1_InitObjectFormat.\n" );
		goto	EXIT;
	}

	/* get default unit */
	if( ( lpW1Comp->env_unit = GetEnvDefaultUnit( lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[W1_Open]Error is occured in GetEnvDefaultUnit.\n" );
		goto	EXIT;
	}
	/* init preview area */
	lpW1Comp->previewFlag = CNMS_FALSE;

	if ( CnmsColAdjInit() == CNMS_ERR ){	/* Color Adjust Dialog Create & Initialize */
		DBGMSG( "[W1_Open]Error is occured in CnmsColAdjInit.\n" );
		goto	EXIT;
	}
	
	if ( DisplayDeviceFullName() ) {
		snprintf( window_title, sizeof( window_title ) - 1 , "ScanGear : Canon %s", lpFullName );
	}
	else {
		snprintf( window_title, sizeof( window_title ) - 1 , "ScanGear" );
	}
	gtk_window_set_title (GTK_WINDOW (main_window), window_title );
	
	gtk_widget_show( main_window );
	if( ( ldata = W1Ui_HideNoUseBox() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Open]Error is occured in W1Ui_HideNoUseBox.\n" );
		goto	EXIT;
	}
	else if( ( ldata = Preference_Open( lpStr ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Open]Error is occured in Preference_Open.\n" );
		goto	EXIT;
	}
	else if( ( ldata = Preview_Open( main_window ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Open]Error is occured in Preview_Open.\n" );
		goto	EXIT;
	}
	else if( ( ldata = W1_Reset( PREVIEW_RESET_ALL ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Open]Error is occured in W1_Reset.\n" );
		goto	EXIT;
	}
	else if( ( ldata = KeepSettingOpen( lpStr ) ) == CNMS_ERR ){
		DBGMSG( "[W1_Open]Error is occured in KeepSettingOpen.\n" );
		goto	EXIT;
	}
	else if( ( ldata = SaveDialog_Open() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Open]Error is occured in SaveDialog_Open.\n" );
		goto	EXIT;
	}

	if( ( ldata = W1_InitUIFormat() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Open]Error is occured in W1_InitUIFormat.\n" );
		goto	EXIT;
	}

	if( ( widget = lookup_widget( main_window, "main_notebook" ) ) == CNMSNULL ){
		DBGMSG( "[W1_Open]Can't look up widget.\n" );
		goto	EXIT;
	}
	gtk_notebook_set_current_page( GTK_NOTEBOOK( widget ), 1 );
	gtk_notebook_set_current_page( GTK_NOTEBOOK( widget ), 0 );

	ret = CNMS_NO_ERR;
EXIT:
	if( ret != CNMS_NO_ERR ){
		W1_Close();
	}
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1_Open(devIndex:%d)]=%d.\n", devIndex, ret );
#endif
	return	ret;
}

static CNMSInt32 GetEnvDefaultUnit(
		LPCNMSUILINKCOMP		lpLink,
		CNMSInt32				linkNum )
{
	CNMSInt32	ret = CNMS_ERR, unit;
	CNMSLPSTR	env_locale = CNMSNULL;

	if( ( unit = CnmsUiGetValue( CNMS_OBJ_A_UNIT, lpLink, linkNum ) ) < 0 ){
		DBGMSG( "[GetEnvDefaultUnit]Error is occured in CnmsUiGetValue( CNMS_OBJ_A_UNIT ).\n" );
		goto	EXIT;
	}

	/* select unit */
	if( GetEnvLocaleID() == CNMS_LOCALE_EN ){
		unit = CNMS_A_UNIT_INCH;
	}

	ret = unit;
EXIT:
	return	ret;
}

static CNMSInt32 GetEnvLocaleID( CNMSVoid )
{
	CNMSInt32	ret = CNMS_LOCALE_OTHER;
	CNMSLPSTR	env_locale = CNMSNULL;

	/* get locale string */
#ifdef USE_PO_LOCALE
	env_locale = _( "Locale" );
#else
	if( ( env_locale = ( CNMSLPSTR )getenv( "LANG" ) ) == CNMSNULL ){
		env_locale = "";
	}
	env_locale = (CNMSLPSTR)strsep( &env_locale, "." );
#endif
	/* return local */
	if( strcasecmp( env_locale, "en_US" ) == 0 ){
		ret = CNMS_LOCALE_EN;
	}
	else if( strcasecmp( env_locale, "tr_TR" ) == 0 ){
		ret = CNMS_LOCALE_TR;
	}

	return	ret;
}

CNMSVoid W1_Close( CNMSVoid )
{
	CnmsScanFlowPreviewFdInfoDispose();

	SaveDialog_Close();
	ChildDialogClose();
	KeepSettingClose();
	KeepSettingCommonClose();
	ProgressBarClose();
	Preview_Close();
	Preference_Close();
	CnmsUiLinkClose();
	CnmsImgApiClose();
	CnmsScanClose();
	CnmsColAdjClose();

	if( lpW1Comp != CNMSNULL ){
		CnmsFreeMem( (CNMSLPSTR)lpW1Comp );
	}
	lpW1Comp = CNMSNULL;
	gtk_widget_hide( main_window );

	gtk_main_quit();

#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1_Close()].\n" );
#endif
	return;
}

CNMSInt32 W1_SwitchPlatenAndADFReset( CNMSInt32 prevKey )
{
	CNMSInt32	ret = CNMS_ERR,
				ui_flag_tmp = ui_flag;

	if( ui_flag_tmp == CNMS_TRUE )	ui_flag = CNMS_FALSE;
	W1_KeepCurrentValue();
	swPaA_flag = CNMS_TRUE;
	if( ( ret = W1_Reset( prevKey ) ) == CNMS_ERR ){
		DBGMSG( "[W1_SwitchPlatenAndADFReset]Status is error.\n" );
	}
	swPaA_flag = CNMS_FALSE;
	if( ui_flag_tmp == CNMS_TRUE )	ui_flag = CNMS_TRUE;
	
	return	ret;
}

CNMSInt32 W1_Reset( CNMSInt32 prevKey )
{
	CNMSInt32			ret = CNMS_ERR, i, j, ldata, source, cropRes;
	CNMSUISIZECOMP		sizeComp;
	LPCNMSUILINKCOMP	lplink;
	GtkWidget*			widget = CNMSNULL;

	if( ui_flag == CNMS_TRUE ){
		goto	EXIT;
	}
	ui_flag = CNMS_TRUE;
	/* check */
	if( ( lpW1Comp == CNMSNULL ) || ( main_window == CNMSNULL ) ){
		DBGMSG( "[W1_Reset]Status is error.\n" );
		goto	EXIT_ERR;
	}
	/* get default ui info */
	if( ( ldata = CnmsUiLinkDef( lpW1Comp->lpLink ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Reset]Error is occured in CnmsUiLinkDef.\n" );
		goto	EXIT_ERR;
	}
	else if( ( source = CnmsUiGetValue( CNMS_OBJ_A_SOURCE, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[W1_Reset]Error is occured in CnmsUiGetValue( CNMS_OBJ_A_SOURCE ).\n" );
		goto	EXIT_ERR;
	}
	else if( ( lpW1Comp->crop_res = CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) <= 0 ){
		DBGMSG( "[W1_Reset]Error is occured in CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION ).\n" );
		goto	EXIT_ERR;
	}
	else if( ( ldata = CnmsUiGetValue( CNMS_OBJ_A_UNIT, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_A_UNIT_PIXEL ){
		if( ( ldata = CnmsUiSetValue( CNMS_OBJ_A_UNIT, lpW1Comp->env_unit, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_NO_ERR ){
			DBGMSG( "[W1_Reset]Error is occured in CnmsUiSetValue( CNMS_OBJ_A_UNIT ).\n" );
			goto	EXIT_ERR;
		}
	}
	
	/* putback colormode for switch Platen and ADF */
	if( swPaA_flag == CNMS_TRUE ){
		if( W1_SwitchPaAPutBack() == CNMS_ERR ){
			DBGMSG( "[W1_Reset]Error is occured in W1_SwitchPaAPutBack().\n" );
			goto	EXIT_ERR;
		}
	}

	/* get platen size */
	if( ( ldata = CnmsUiGetRealSize( CNMS_OBJ_A_OUTPUT_SIZE, lpW1Comp->lpLink, lpW1Comp->linkNum, &sizeComp ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Reset]Error is occured in CnmsUiGetRealSize( CNMS_OBJ_A_OUTPUT_SIZE ).\n" );
		goto	EXIT_ERR;
	}
	lpW1Comp->group = sizeComp.group;
	lpW1Comp->scale = CNMS_SCALE_100_PERCENT;
	if( ( lpW1Comp->base_res = CnmsScanGetRealMinMaxSize( source, lpW1Comp->minSize, lpW1Comp->maxSize ) ) <= 0 ){
		DBGMSG( "[W1_Reset]Error is occured in CnmsScanGetRealMinMaxSize.\n" );
		goto	EXIT_ERR;
	}
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		lpW1Comp->outSize[ i ] = lpW1Comp->inSize[ i ] = lpW1Comp->maxSize[ i ];
		lpW1Comp->offset[ i ] = 0;
	}
	/* Reset Preview */
	if( ( ldata = Preview_Init( source ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Reset]Error is occured in Preview_Init.\n" );
		goto	EXIT_ERR;
	}
	else if( ( ldata = Preview_Reset( prevKey ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Reset]Error is occured in Preview_Reset.\n" );
		goto	EXIT_ERR;
	}
	
	if( ( prevKey & PREVIEW_RESET_IMAGE ) == 0 ) {
		if( swPaA_flag == CNMS_TRUE ){
			CnmsColAdjAllDataReset();
		}
		else{
			CnmsColAdjResetComplete();
		}
	}		

	/* Init ui */
	if( ( ldata = W1Ui_ShowSize() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Reset]Error is occured in W1Ui_ShowSize.\n" );
		goto	EXIT_ERR;
	}
	else if( ( ldata = W1Ui_Init() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Reset]Error is occured in W1Ui_Init.\n" );
		goto	EXIT_ERR;
	}

	for( j = 0 ; currentUOGroup[j].object != CNMS_ERR ; j++ ){
		lplink = lpW1Comp->lpLink;
		for( i = 0 ; i < lpW1Comp->linkNum ; i++, lplink++ ){
			if( ( currentUOGroup[j].object == lplink->object ) && ( lplink->mode == CNMS_MODE_SELECT ) ){
				currentUOGroup[j].value = lplink->value;
				if( ( currentUOGroup[j].group = W1_GetUIObjGroup( lplink->object, lplink->value ) ) == CNMS_ERR ){
					DBGMSG( "[W1_Reset]Error is occured in W1_GetUIObjGroup.\n" );
					goto	EXIT_ERR;
				}
					
			}
		}
	}
	for( i = 0 ; oldComboSet[i].object != CNMS_ERR ; i++ ){
		if( ( widget = lookup_widget( main_window, (const gchar *)( oldComboSet[i].str ) ) ) == CNMSNULL ){
			DBGMSG( "[W1_Reset]Can't get widget( %s ).\n", oldComboSet[i].str );
			goto	EXIT_ERR;
		}
		oldComboSet[i].num = gtk_combo_box_get_active( GTK_COMBO_BOX( widget ) );
	}

	ui_flag = CNMS_FALSE;
EXIT_NO_ERR:
	CnmsColAdjColorModeChanged();
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	if( ui_flag == CNMS_FALSE ){
		DBGMSG( "[W1_Reset()]= %d.\n", ret );
	}
#endif
	return	ret;

EXIT_ERR:
	ui_flag = CNMS_FALSE;
	goto	EXIT;
}

CNMSInt32 W1_ChangeToggle(
		GtkToggleButton	*togglebutton,
		CNMSInt32		objectID )
{
	CNMSInt32	ret = CNMS_ERR, ldata, aspect;

	/* called when Fixed -> Varied or Varied -> Fixed only */
	/* check */
	if( ui_flag == CNMS_TRUE ){
		goto	EXIT;
	}
	ui_flag = CNMS_TRUE;
	if( lpW1Comp == CNMSNULL ){
		DBGMSG( "[W1_ChangeToggle]Status is error.\n" );
		goto	EXIT;
	}
	else if( ( togglebutton == CNMSNULL ) || ( objectID != CNMS_OBJ_A_RATIO_FIX ) ){
		DBGMSG( "[W1_ChangeToggle]Initialize parameter is error.\n" );
		goto	EXIT;
	}
	/* set */
	aspect = ( gtk_toggle_button_get_active( togglebutton ) != 0 ) ? CNMS_ASPECT_FIXED : CNMS_ASPECT_VARIED;

	if( ( ldata = W1Ui_AspectInfoSet( aspect ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ChangeToggle]Error is occured in W1Ui_AspectInfoSet.\n" );
		goto	EXIT;
	}

	ui_flag = CNMS_FALSE;
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1_ChangeToggle()]=%d.\n", ret );
#endif
	return	ret;

EXIT_ERR:
	ui_flag = CNMS_FALSE;
	goto	EXIT;
}

CNMSInt32 W1_ChangeSpin(
		GtkSpinButton		*spin_button,
		CNMSInt32			objectID )
{
	CNMSInt32		ret = CNMS_ERR, ldata, outRes, unit, value, tmpSize[ CNMS_DIM_MAX ];
	CNMSDec32		decVal;

	if( ui_flag == CNMS_TRUE ){
		goto	EXIT;
	}
	ui_flag = CNMS_TRUE;
	/* check */
	if( ( lpW1Comp == CNMSNULL ) || ( main_window == CNMSNULL ) ){
		DBGMSG( "[W1_ChangeSpin]Status is error.\n" );
		goto	EXIT_ERR;
	}
	/* get value */
	decVal = (CNMSDec32)gtk_spin_button_get_value( spin_button );
	if( ( outRes = CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) <= 0 ){
		DBGMSG( "[W1_ChangeSpin]Error is occured in CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION ).\n" );
		goto	EXIT_ERR;
	}
	else if( ( unit = CnmsUiGetValue( CNMS_OBJ_A_UNIT, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[W1_ChangeSpin]Error is occured in CnmsUiGetValue( CNMS_OBJ_A_UNIT ).\n" );
		goto	EXIT_ERR;
	}
	/* change ( no error message ) */
	if( objectID == CNMS_OBJ_A_SCALE ){
		value = (CNMSInt32)CnmsRoundValue( decVal * ( CNMS_SCALE_100_PERCENT / 100 ), 0, CNMS_ROUND_CUT );
	}
	else{
		value = (CNMSInt32)CnmsConvUnit( decVal, unit, CNMS_A_UNIT_PIXEL, outRes, lpW1Comp->base_res );
		switch( objectID ){
			case	CNMS_OBJ_A_INPUT_WIDTH:
				value = ( value < lpW1Comp->minSize[ CNMS_DIM_H ] ) ? lpW1Comp->minSize[ CNMS_DIM_H ] : ( ( lpW1Comp->maxSize[ CNMS_DIM_H ] < value ) ? lpW1Comp->maxSize[ CNMS_DIM_H ] : value );
				break;
			case	CNMS_OBJ_A_INPUT_HEIGHT:
				value = ( value < lpW1Comp->minSize[ CNMS_DIM_V ] ) ? lpW1Comp->minSize[ CNMS_DIM_V ] : ( ( lpW1Comp->maxSize[ CNMS_DIM_V ] < value ) ? lpW1Comp->maxSize[ CNMS_DIM_V ] : value );
				break;
		}
	}
	if( ( ldata = W1Size_ChangeSpin( value, objectID ) ) != CNMS_NO_ERR ){	/* Don't change spin */
		goto	EXIT_NO_ERR;
	}
	/* show */
	else if( ( ldata = W1Ui_ShowSize() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ChangeSpin]Error is occured in W1Ui_ShowSize.\n" );
		goto	EXIT_ERR;
	}
	else if( ( objectID != CNMS_OBJ_A_INPUT_WIDTH ) && ( objectID != CNMS_OBJ_A_INPUT_HEIGHT ) ) {
		goto	EXIT_NO_ERR;
	}
	/* scan flow */
	if( lpW1Comp->previewFlag == CNMS_TRUE ){
		if( ( ldata = Preview_ChangeStatus( CNMSSCPROC_ACTION_CROP_CORRECT ) ) != CNMS_NO_ERR ){
			DBGMSG( "[W1_ChangeSpin]Error is occured in Preview_ChangeStatus( CNMSSCPROC_ACTION_CROP_CORRECT ).\n" );
			goto	EXIT_ERR;
		}
	}
	if( ( ldata = Preview_RectGet( tmpSize ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ChangeSpin]Error is occured in Preview_RectGet.\n" );
		goto	EXIT_ERR;
	}
	/* set rect */
	if( ( ldata = Preview_RectSet( lpW1Comp->offset, lpW1Comp->inSize ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ChangeSpin]Error is occured in Preview_RectSet.\n" );
		goto	EXIT_ERR;
	}
	/* change aspect toggle button */
	if( tmpSize[ CNMS_DIM_H ] == 0 ){	/* no rect */
		if( ( ldata = W1Ui_AspectInfoSet( CNMS_ASPECT_VARIED ) ) != CNMS_NO_ERR ){
			DBGMSG( "[W1_ChangeSpin]Error is occured in W1Ui_AspectInfoSet!\n" );
			goto	EXIT;
		}
	}
EXIT_NO_ERR:
	ui_flag = CNMS_FALSE;
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	if( ui_flag == CNMS_FALSE ){
		DBGMSG( "[W1_ChangeSpin(objectID:%d)]= %d. decVal:%f(%d).\n", objectID, ret, decVal, value );
	}
#endif
	return	ret;

EXIT_ERR:
	ui_flag = CNMS_FALSE;
	goto	EXIT;
}

CNMSInt32 W1_ChangeCombo(
		GtkComboBox			*combobox,
		CNMSInt32			objectID )
{
	CNMSInt32				ret = CNMS_ERR, i, ldata, value, oldRes, oldUnit, unit, intSize[ CNMS_DIM_MAX ], group, source;
	CNMSDec32				ddata;
	CNMSLPSTR				str;
	CNMSUISIZECOMP			sizeComp;
	LPCNMS_UIOBJECTGROUP	lpCurrentObj = CNMSNULL;
	static	CNMSInt32		oldSSrc = 0;
	static	CNMSInt32		oldASrc = 0;
	GtkWidget				*widget = CNMSNULL;

	if( ui_flag == CNMS_TRUE ){
		goto	EXIT;
	}
	ui_flag = CNMS_TRUE;

	if( ( lpW1Comp == CNMSNULL ) || ( combobox == CNMSNULL ) ){
		DBGMSG( "[W1_ChangeCombo]Status is error.\n" );
		goto	EXIT_ERR;
	}
	else if( ( str = (CNMSLPSTR)gtk_combo_box_get_active_text( combobox ) ) == CNMSNULL ){
		DBGMSG( "[W1_ChangeCombo]Can't get activetext from combobox.\n" );
		goto	EXIT_ERR;
	}
	else if( ( value = CnmsChgStrToVal( str, objectID ) ) == CNMS_ERR ){
		DBGMSG( "[W1_ChangeCombo]Can't change str to value.\n" );
		goto	EXIT_ERR;
	}

	/* UI set for ADF */
	if( ( objectID == CNMS_OBJ_S_SOURCE ) || ( objectID == CNMS_OBJ_A_SOURCE ) ){
		if( ( lpCurrentObj = W1_GetObjCurrentValue( objectID ) ) == CNMSNULL ){
			DBGMSG( "[W1_ChangeCombo]Error is occured in W1_GetObjCurrentValue( %d ).\n",objectID );
			goto	EXIT_ERR;
		}
		for( i = 0 ; oldComboSet[i].object != CNMS_ERR ; i++ ){
			if( oldComboSet[i].object == objectID ){
				if( ( widget = lookup_widget( main_window, (const gchar *)( oldComboSet[i].str ) ) ) == CNMSNULL ){
					DBGMSG( "[W1_ChangeCombo]Can't get widget( %s ).\n", oldComboSet[i].str );
					goto	EXIT_ERR;
				}
				break;
			}
		}
		if( ( lpW1Comp->previewFlag == CNMS_TRUE ) && ( W1_GetUIObjGroup( objectID, value ) != lpCurrentObj->group ) ){
			if( W1_PreviewClear( CLEARPREVIEW_TYPE_CHANGE_SOURCE ) == COMGTK_GTKMAIN_CANCEL ){
				gtk_combo_box_set_active( GTK_COMBO_BOX( widget ), oldComboSet[i].num );
				ui_flag = CNMS_FALSE;
				ret = CNMS_NO_ERR;
				goto	EXIT;	

			}
		}
		oldComboSet[i].num = gtk_combo_box_get_active( GTK_COMBO_BOX( widget ) );
		
		if( W1Ui_SwitchPlatenAndADF( objectID, lpCurrentObj->group, W1_GetUIObjGroup( objectID, value ) ) == CNMS_ERR ){
			DBGMSG( "[W1_ChangeCombo]Error is occured in W1Ui_SwitchPlatenAndADF().\n" );
			goto	EXIT_ERR;
		}
		
		if ( objectID == CNMS_OBJ_A_SOURCE ){
			if( ( source = CnmsGetScanSourceAbility( value ) ) == CNMS_ERR ){
				DBGMSG( "[W1_ChangeCombo]Error is occured in CnmsGetScanSourceAbility().\n" );
				goto	EXIT_ERR;
			}
			if ( CnmsScanGetRealMaxSize( source, lpW1Comp->maxSize ) == CNMS_ERR ){
				DBGMSG( "[W1_ChangeCombo]Error is occured in CnmsScanGetRealMaxSize().\n" );
				goto	EXIT_ERR;
			}
			Preview_ResetMaxSize( lpW1Comp->maxSize );
			CnmsColAdjAllDataReset();
		}
	}
	
	/* keep old value */
	if( W1_KeepOldValue() == CNMS_ERR ){
		DBGMSG( "[W1_ChangeCombo]Error is occured in W1_KeepOldValue().\n" );
		goto	EXIT_ERR;
	}

	/* menu link */
	if( ( ldata = CnmsUiSetChange( objectID, value, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ChangeCombo]Error is occured in CnmsUiSetChange.\n" );
		goto	EXIT_ERR;
	}
	else if( ( ldata = CnmsUiLinkMode( lpW1Comp->lpLink ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ChangeCombo]Error is occured in CnmsUiLinkMode.\n" );
		goto	EXIT_ERR;
	}
	
	/* set locale default paper size */		
	if( ( objectID == CNMS_OBJ_S_SOURCE ) || ( objectID == CNMS_OBJ_A_SOURCE ) ){
		if( GetEnvLocaleID() == CNMS_LOCALE_EN ){
			if( ( ldata = CnmsUiSetValue( CNMS_OBJ_A_PAPER_SIZE, CNMS_A_PAPER_SIZE_LETTER, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_NO_ERR ){
				DBGMSG( "[W1_ChangeCombo]Error is occured in CnmsUiSetValue( CNMS_OBJ_A_PAPER_SIZE ).\n" );
				goto	EXIT_ERR;
			}
		}		
	}
	/* set locale default unit */		
	if( ( objectID == CNMS_OBJ_S_SOURCE ) || ( objectID == CNMS_OBJ_A_SOURCE ) || ( objectID == CNMS_OBJ_S_OUTPUT_SIZE ) || ( objectID == CNMS_OBJ_A_OUTPUT_SIZE ) ){
		if( ( ldata = CnmsUiGetValue( CNMS_OBJ_A_UNIT, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_A_UNIT_PIXEL ){
			if( ( ldata = CnmsUiSetValue( CNMS_OBJ_A_UNIT, lpW1Comp->env_unit, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_NO_ERR ){
				DBGMSG( "[W1_ChangeCombo]Error is occured in CnmsUiSetConvValue( CNMS_OBJ_A_UNIT ).\n" );
				goto	EXIT_ERR;
			}
		}
	}

	/* change destination (simple mode only) */
	if ( objectID == CNMS_OBJ_S_DESTINATION ) {
		/* reset scale to 100% */
		lpW1Comp->scale = CNMS_SCALE_100_PERCENT;
		for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
			lpW1Comp->inSize[i] = lpW1Comp->outSize[i];
		}
	}
	
	/* same group -> put back old value */
	if( W1_PutBackObject( objectID, value ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ChangeCombo]Error is occured in W1_PutBackObject.\n" );
		goto	EXIT_ERR;
	}
	
	group = W1_GetUIObjGroup( objectID, value );
	/* change paper size ( or source ) */
	/* CNMS_S_SOURCE_PHOTO -> CNMS_S_SOURCE_PHOTO : do nothing */
	if( ( objectID == CNMS_OBJ_S_SOURCE && group == CNMS_S_SOURCE_TEXT_COL_SADF ) || ( objectID == CNMS_OBJ_A_SOURCE ) || ( objectID == CNMS_OBJ_A_PAPER_SIZE ) ){
		lpW1Comp->scale = CNMS_SCALE_100_PERCENT;
		CnmsUiGetRealSize( CNMS_OBJ_A_PAPER_SIZE, lpW1Comp->lpLink, lpW1Comp->linkNum, &sizeComp );
		for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
			intSize[ i ] = (CNMSInt32)CnmsConvUnit( sizeComp.size[ i ], sizeComp.unit, CNMS_A_UNIT_PIXEL, lpW1Comp->size_res, lpW1Comp->base_res );
		}
		Preview_GetNewInOutSize( intSize, lpW1Comp->inSize, lpW1Comp->outSize, &( lpW1Comp->scale ) );
	}

	/* change out size */
	if( ( objectID == CNMS_OBJ_S_OUTPUT_SIZE ) || ( objectID == CNMS_OBJ_A_OUTPUT_SIZE ) ){
		/* get outSize */
		if( ( ldata = CnmsUiGetRealSize( CNMS_OBJ_A_OUTPUT_SIZE, lpW1Comp->lpLink, lpW1Comp->linkNum, &sizeComp ) ) != CNMS_NO_ERR ){
			DBGMSG( "[W1_ChangeCombo]Error is occured in CnmsUiGetRealSize( CNMS_OBJ_A_OUTPUT_SIZE ).\n" );
			goto	EXIT_ERR;
		}
		/* get param */
		if( ( lpW1Comp->size_res = CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) <= 0 ){
			DBGMSG( "[W1_ChangeCombo]Error is occured in CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION ).\n" );
			goto	EXIT_ERR;
		}
		else if( ( unit = CnmsUiGetValue( CNMS_OBJ_A_UNIT, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
			DBGMSG( "[W1_ChangeCombo]Error is occured in CnmsUiGetValue( CNMS_OBJ_A_UNIT ).\n" );
			goto	EXIT_ERR;
		}
		/* get real size */
		for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
			intSize[ i ] = (CNMSInt32)CnmsConvUnit( sizeComp.size[ i ], sizeComp.unit, CNMS_A_UNIT_PIXEL, lpW1Comp->size_res, lpW1Comp->base_res );
		}
		lpW1Comp->scale = ( sizeComp.unit == CNMS_A_UNIT_PIXEL ) ? (CNMSInt32)( CNMS_SCALE_100_PERCENT * (CNMSDec32)( lpW1Comp->crop_res ) / (CNMSDec32)( lpW1Comp->size_res ) ) : CNMS_SCALE_100_PERCENT;
		/* for pixel size */
		if( ( ldata = Preview_GetNewInOutSize( intSize, lpW1Comp->inSize, lpW1Comp->outSize, &( lpW1Comp->scale ) ) ) != CNMS_NO_ERR ){
			DBGMSG( "[W1_ChangeCombo]Error is occured in Preview_GetNewInOutSize.\n" );
			goto	EXIT_ERR;
		}
		lpW1Comp->group = sizeComp.group;
		if( sizeComp.size[ CNMS_DIM_H ] != 0 ){
			if( ( ldata = Preview_RectSet( lpW1Comp->offset, lpW1Comp->inSize ) ) != CNMS_NO_ERR ){
				DBGMSG( "[W1_ChangeCombo]Error is occured in Preview_RectSet.\n" );
				goto	EXIT_ERR;
			}
		}
	}
	/* show ui */
	if( ( ldata = W1Ui_ShowSize() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ChangeCombo]Error is occured in W1Ui_ShowSize.\n" );
		goto	EXIT_ERR;
	}
	else if( ( ldata = W1Ui_Init() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ChangeCombo]Error is occured in W1Ui_Init.\n" );
		goto	EXIT_ERR;
	}

	if( lpW1Comp->previewFlag == CNMS_TRUE ){
		switch( objectID ){
			case	CNMS_OBJ_S_OUTPUT_SIZE:
			case	CNMS_OBJ_A_OUTPUT_SIZE:
				value = CNMSSCPROC_ACTION_CROP_CORRECT;
				break;
			case	CNMS_OBJ_S_SOURCE:
			case	CNMS_OBJ_A_COLOR_MODE:
			case	CNMS_OBJ_A_UNIT:
			case	CNMS_OBJ_A_OUTPUT_RESOLUTION:
				value = CNMS_ERR;
				break;
			default:
				value = CNMSSCPROC_ACTION_PARAM;
				break;
		}
		if( value != CNMS_ERR ){
			if( ( ldata = Preview_ChangeStatus( value ) ) != CNMS_NO_ERR ){
				DBGMSG( "[W1_ChangeCombo]Error is occured in Preview_ChangeStatus( CNMSSCPROC_ACTION_CROP_CORRECT ).\n" );
				goto	EXIT_ERR;
			}
		}
	}

	if( ( objectID == CNMS_OBJ_A_UNSHARP_MASK ) || ( objectID == CNMS_OBJ_A_DESCREEN ) ){
		W1_CheckMoireWarning();
	}

	ui_flag = CNMS_FALSE;
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	if( ui_flag == CNMS_FALSE ){
		DBGMSG( "[W1_ChangeCombo(objectID:%d, value:%d)]=%d.\n", objectID, value, ret );
	}
#endif
	return	ret;

EXIT_ERR:
	ui_flag = CNMS_FALSE;
	goto	EXIT;
}

CNMSInt32 W1_ResetSize( CNMSVoid )	/* called by RectReset( in prev_sub.c ) only */
{
	CNMSInt32		ret = CNMS_ERR, ldata, aspect, source, unit, res, destination;
	CNMSUISIZECOMP	sizeComp;
	GtkWidget	*widget;
	LPCNMS_UIOBJECTGROUP	lpCurrentObj = CNMSNULL;

	/* check */
	if( ui_flag == CNMS_TRUE ){
		goto	EXIT;
	}
	ui_flag = CNMS_TRUE;
	if( lpW1Comp == CNMSNULL ){
		DBGMSG( "[W1_ResetSize]Status is wrong.\n" );
		goto	EXIT_ERR;
	}
	/* keep old value */
	else if( ( aspect = CnmsUiAspectGet( lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[W1_ResetSize]Error is occured in CnmsUiAspectGet.\n" );
		goto	EXIT_ERR;
	}
	else if( ( res = CnmsUiGetValue( CNMS_OBJ_A_OUTPUT_RESOLUTION, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[W1_ResetSize]Error is occured in CnmsUiGetValue( CNMS_OBJ_A_OUTPUT_RESOLUTION ).\n" );
		goto	EXIT_ERR;
	}
	else if( ( unit = CnmsUiGetValue( CNMS_OBJ_A_UNIT, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[W1_ResetSize]Error is occured in CnmsUiGetValue( CNMS_OBJ_A_UNIT ).\n" );
		goto	EXIT_ERR;
	}
	/* menu link ( to Free Size )*/
	else if( ( ldata = CnmsUiSetChange( CNMS_OBJ_S_OUTPUT_SIZE, CNMS_S_OUTPUT_SIZE_FREE, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ResetSize]Error is occured in CnmsUiSetChange.\n" );
		goto	EXIT_ERR;
	}
	else if( ( ldata = CnmsUiLinkMode( lpW1Comp->lpLink ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ResetSize]Error is occured in CnmsUiLinkMode.\n" );
		goto	EXIT_ERR;
	}
	
	/* reset destination (simple mode only) */
	if( ( widget = lookup_widget( main_window, "main_notebook" ) ) == CNMSNULL ){
		DBGMSG( "[W1_ResetSize]Can't look up widget(main_notebook).\n" );
		goto	EXIT_ERR;
	}
	if( gtk_notebook_get_current_page( GTK_NOTEBOOK( widget ) ) == 0 ){
		/* get current destination */
		if( ( destination = CnmsUiGetValue( CNMS_OBJ_S_DESTINATION, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
			DBGMSG( "[W1_ResetSize]Error is occured in CnmsUiGetValue( CNMS_OBJ_A_UNIT ).\n" );
			goto	EXIT_ERR;
		}
		/* menu link */
		else if( ( ldata = CnmsUiSetChange( CNMS_OBJ_S_DESTINATION, destination, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_NO_ERR ){
			DBGMSG( "[W1_ResetSize]Error is occured in CnmsUiSetChange.\n" );
			goto	EXIT_ERR;
		}
		else if( ( ldata = CnmsUiLinkMode( lpW1Comp->lpLink ) ) != CNMS_NO_ERR ){
			DBGMSG( "[W1_ResetSize]Error is occured in CnmsUiLinkMode.\n" );
			goto	EXIT_ERR;
		}
	}
	
	/* make new ui */
	if( aspect == CNMS_ASPECT_STANDERD ){
		/* Stardard -> Free scale:100%, res:new, unit:default */
		lpW1Comp->scale = CNMS_SCALE_100_PERCENT;
		if( ( ldata = CnmsUiGetValue( CNMS_OBJ_A_UNIT, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_A_UNIT_PIXEL ){
			if( ( ldata = CnmsUiSetValue( CNMS_OBJ_A_UNIT, lpW1Comp->env_unit, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_NO_ERR ){
				DBGMSG( "[W1_ResetSize]Error is occured in CnmsUiSetConvValue( CNMS_OBJ_A_UNIT ).\n" );
				goto	EXIT_ERR;
			}
		}
	}
	else{
		/* Free -> Free scale:keep, res:oldRes, unit:oldUnit ) */
		if( ( ldata = CnmsUiSetValue( CNMS_OBJ_A_OUTPUT_RESOLUTION, res, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_NO_ERR ){
			DBGMSG( "[W1_ResetSize]Error is occured in CnmsUiSetValue( CNMS_OBJ_A_OUTPUT_RESOLUTION ).\n" );
			goto	EXIT_ERR;
		}
		if( ( ldata = CnmsUiSetValue( CNMS_OBJ_A_UNIT, unit, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_NO_ERR ){
			DBGMSG( "[W1_ResetSize]Error is occured in CnmsUiSetValue( CNMS_OBJ_A_UNIT ).\n" );
			goto	EXIT_ERR;
		}
	}
	/* get value */
	if( ( source = CnmsUiGetValue( CNMS_OBJ_A_SOURCE, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[W1_ResetSize]Error is occured in CnmsUiGetValue( CNMS_OBJ_A_SOURCE ).\n" );
		goto	EXIT_ERR;
	}
	else if( ( ldata = CnmsUiGetRealSize( CNMS_OBJ_A_OUTPUT_SIZE, lpW1Comp->lpLink, lpW1Comp->linkNum, &sizeComp ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ResetSize]Error is occured in CnmsUiGetRealSize( CNMS_OBJ_A_OUTPUT_SIZE ).\n" );
		goto	EXIT_ERR;
	}
	/* set */
	lpW1Comp->group = sizeComp.group;
	lpW1Comp->offset[ CNMS_DIM_H ] = lpW1Comp->offset[ CNMS_DIM_V ] = 0;
	/* set currentUOGroup (for W1_PutBackObject) */
	if( ( lpCurrentObj = W1_GetObjCurrentValue( CNMS_OBJ_A_OUTPUT_SIZE ) ) == CNMSNULL ){
		DBGMSG( "[W1_ResetSize]Error is occured in W1_GetObjCurrentValue( %d ).\n",CNMS_OBJ_A_OUTPUT_SIZE );
		goto	EXIT_ERR;
	}
	lpCurrentObj->value = CNMS_A_OUTPUT_SIZE_FREE;
	lpCurrentObj->group = W1_GetUIObjGroup( CNMS_OBJ_A_OUTPUT_SIZE, CNMS_A_OUTPUT_SIZE_FREE );
	/* inSize = fullSize */
	if( ( lpW1Comp->base_res = CnmsScanGetRealMaxSize( source, lpW1Comp->inSize ) ) <= 0 ){
		DBGMSG( "[W1_ResetSize]Error is occured in CnmsScanGetRealMaxSize.\n" );
		goto	EXIT_ERR;
	}
	/* conv outSize = inSize * scale */
	lpW1Comp->outSize[ CNMS_DIM_H ] = (CNMSInt32)( ( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_H ] ) * (CNMSDec32)( lpW1Comp->scale ) ) / CNMS_SCALE_100_PERCENT );
	lpW1Comp->outSize[ CNMS_DIM_V ] = (CNMSInt32)( ( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_V ] ) * (CNMSDec32)( lpW1Comp->scale ) ) / CNMS_SCALE_100_PERCENT );
	/* show */
	if( ( ldata = W1Ui_ShowSize() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ResetSize]Error is occured in W1Ui_ShowSize.\n" );
		goto	EXIT_ERR;
	}
	else if( ( ldata = W1Ui_Init() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ResetSize]Error is occured in W1Ui_Init.\n" );
		goto	EXIT_ERR;
	}
	else if( ( ldata = W1Ui_AspectReflect() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_ResetSize]Error is occured in W1Ui_AspectReflect.\n" );
		goto	EXIT_ERR;
	}
	ui_flag = CNMS_FALSE;
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1_ResetSize()]=%d.\n", ret );
#endif
	return	ret;

EXIT_ERR:
	ui_flag = CNMS_FALSE;
	goto	EXIT;
}

CNMSInt32 W1_Preview( CNMSVoid )
{
	CNMSInt32	ret = CNMS_ERR, ldata, errorQuit = ERROR_QUIT_NO_ERR;

	/* clear preview */
	dialog_clear_preview_ok_clicked();

	/* check */
	if( ( lpW1Comp == CNMSNULL ) || ( main_window == CNMSNULL ) ){
		DBGMSG( "[W1_Preview]Status is error.\n" );
		goto	EXIT;
	}
	else if( ( ldata = Preview_ChangeStatus( CNMSSCPROC_ACTION_PREVIEW ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Preview]Error is occured in Preview_ChangeStatus( CNMSSCPROC_ACTION_PREVIEW ).\n" );
		goto	EXIT;
	}
	/* no error */
	lpW1Comp->previewFlag = CNMS_TRUE;
	if( ( ldata = W1Ui_ChangeClearPrevButtonVisible() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_Preview]Error is occured in W1Ui_ChangeClearPrevButtonVisible.\n" );
		goto	EXIT;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1_Preview()]=%d.\n", ret );
#endif

	if( ( errorQuit = ShowErrorDialog() ) == ERROR_QUIT_TRUE ){
		W1_Close();
	}

	return	ret;
}

CNMSInt32 W1_PreviewCancel( CNMSVoid )
{
	CNMSInt32	ret = CNMS_ERR, ldata;

	/* check */
	if( ( lpW1Comp == CNMSNULL ) || ( main_window == CNMSNULL ) ){
		DBGMSG( "[W1_PreviewCancel]Status is error.\n" );
		goto	EXIT;
	}
	CnmsScanFlowScanCancel();

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1_PreviewCancel()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSInt32 W1_PreviewClear(
		CNMSInt32			id	)
{
	CNMSInt32	ret = CNMS_ERR, i;
	GtkWidget	*label = CNMSNULL;

	if ( clear_preview_dialog != NULL ) {
		if( ( label = lookup_widget( clear_preview_dialog, "dialog_clear_preview_label" ) ) == CNMSNULL ){
			DBGMSG( "[W1_PreviewClear]Can't get widget( dialog_clear_preview_label ).\n");
			goto	EXIT;
		}
		for( i = 0 ; clearPrevMsg[i].id != CNMS_ERR ; i++ ){
			if( clearPrevMsg[i].id == id )	break;
		}
		if( clearPrevMsg[i].id == CNMS_ERR ){
			DBGMSG( "[W1_PreviewClear]Can't find ID( %d ).\n",id);
			goto	EXIT;
		}
		gtk_label_set_text( GTK_LABEL( label ), gettext( clearPrevMsg[i].str ) );

		/* set focus -> OK */
		W1_WidgetGrabFocus( clear_preview_dialog, "dialog_clear_preview_ok" );
		W1_ModalDialogShowAction( clear_preview_dialog, main_window );
	}
EXIT:
	if( ( ret = ComGtk_GtkMain() ) == CNMS_ERR ){
		DBGMSG( "[W1_PreviewClear]Function error by ComGtk_GtkMain() = %d.\n",ret );
	}

	return	ret;
}

void dialog_clear_preview_ok_clicked(void)
{
	CNMSInt32	ldata;
	
	/* check */
	if( ( lpW1Comp == CNMSNULL ) || ( main_window == CNMSNULL ) ){
		DBGMSG( "[W1_PreviewClear]Status is error.\n" );
		goto	EXIT;
	}
	if( lpW1Comp->previewFlag == CNMS_FALSE ){
		goto	EXIT;
	}
	if( ( ldata = Preview_Reset( PREVIEW_RESET_IMAGE ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_PreviewClear]Error is occured in Preview_Reset.\n" );
		goto	EXIT;
	}
	/* no error */
	lpW1Comp->previewFlag = CNMS_FALSE;
	if( ( ldata = W1Ui_ChangeClearPrevButtonVisible() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1_PreviewClear]Error is occured in W1Ui_ChangeClearPrevButtonVisible.\n" );
		goto	EXIT;
	}
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[dialog_clear_preview_ok_clicked].\n" );
#endif
	return;
}

CNMSInt32 W1_SetModal(
		CNMSBool		flag )
{
	CNMSInt32		ret = CNMS_ERR;
	GdkCursor		*cursor;

	/* check */
	if( ( lpW1Comp == CNMSNULL ) || ( main_window == CNMSNULL ) ){
		DBGMSG( "[W1_SetModal]Initialize parameter is error.\n" );
		goto	EXIT;
	}
	if( flag == CNMS_TRUE ){
		gtk_window_set_modal( GTK_WINDOW( main_window ), TRUE );
		cursor = gdk_cursor_new( CNMS_CURSOR_ID_CROSS );
	}
	else{
		gtk_window_set_modal( GTK_WINDOW( main_window ), FALSE );
		cursor = gdk_cursor_new( CNMS_CURSOR_ID_NORMAL );
		gtk_window_set_keep_above( (GtkWindow*)hg_dlg.widget, CNMS_TRUE );		
		gtk_window_set_keep_above( (GtkWindow*)hg_dlg.widget, CNMS_FALSE );		
	}
	gdk_window_set_cursor( main_window->window, cursor );
	gdk_cursor_unref (cursor);

	ret = CNMS_NO_ERR;
EXIT:
	return	ret;
}

CNMSVoid W1_HideAndShowAllButton(
		CNMSVoid			 )
{
	/* do nothing. */
	return;
/*
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "main_preview" 		 		 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "toolbar_clear_preview_button" ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "toolbar_about_button" 		 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "preference_button" 	 		 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "close_button" 	 			 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "simple_preview_button" 		 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "simple_scan_button" 			 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "advance_size_lock_toggle" 	 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "advance_preview_button" 		 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "advance_scan_button" 		 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "advance_BC_button" 			 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "advance_HG_button" 			 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "advance_TC_button" 			 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "advance_FR_button" 			 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "advance_MC_button" 			 ) );
	ComGtk_WidgetHideAndShow( lookup_widget( main_window, "advance_reset_button" 		 ) );

	ComGtk_WidgetHideAndShow( lookup_widget( select_model_dialog, "select_model_search_button" ) );
	ComGtk_WidgetHideAndShow( lookup_widget( select_model_dialog, "select_model_ok_button" 	   ) );
	ComGtk_WidgetHideAndShow( lookup_widget( select_model_dialog, "select_model_cancel_button" ) );

	SaveDialog_HideAndShowButton();

	return;
*/
}

CNMSVoid W1_ModalDialogShowAction(
		GtkWidget		*widget,
		GtkWidget		*parent			)
{
	if( ( widget == CNMSNULL ) || ( main_window == CNMSNULL ) || ( select_model_dialog == CNMSNULL ) ){
		DBGMSG( "[W1_ModalDialogHideAction]widget is NULL.\n" );
		return;
	}
	
	if( parent == CNMSNULL ) {
		if( GTK_WIDGET_VISIBLE( main_window ) ){
			gtk_window_set_transient_for( GTK_WINDOW( widget ), GTK_WINDOW( main_window ) );
			if( GTK_WIDGET_SENSITIVE( main_window ) ){
				gtk_widget_set_sensitive( main_window, FALSE );
			}
		}
		if( GTK_WIDGET_VISIBLE( select_model_dialog ) ){
			gtk_window_set_transient_for( GTK_WINDOW( widget ), GTK_WINDOW( select_model_dialog ) );
			if( GTK_WIDGET_SENSITIVE( select_model_dialog ) ){
				gtk_widget_set_sensitive( select_model_dialog, FALSE );
			}
		}
	}
	else {
		if( GTK_WIDGET_VISIBLE( parent ) ){
			gtk_window_set_transient_for( GTK_WINDOW( widget ), GTK_WINDOW( parent ) );
			if( GTK_WIDGET_SENSITIVE( parent ) ){
				gtk_widget_set_sensitive( parent, FALSE );
			}
		}
	}
	
	gtk_widget_show( widget );

	return;	
}

CNMSVoid W1_ModalDialogHideAction(
		GtkWidget		*widget,
		GtkWidget		*parent			)
{
	if( ( widget == CNMSNULL ) || ( main_window == CNMSNULL ) || ( select_model_dialog == CNMSNULL ) ){
		DBGMSG( "[W1_ModalDialogHideAction]widget is NULL.\n" );
		return;
	}
	
	gtk_widget_hide( widget );

	if( parent == CNMSNULL ) {
		if( GTK_WIDGET_VISIBLE( main_window ) ){
			if( main_window_sensitive ) {
				if( !GTK_WIDGET_SENSITIVE( main_window ) ){
					gtk_widget_set_sensitive( main_window, TRUE );
				}
			}
		}
		if( GTK_WIDGET_VISIBLE( select_model_dialog ) ){
			if( !GTK_WIDGET_SENSITIVE( select_model_dialog ) ){
				gtk_widget_set_sensitive( select_model_dialog, TRUE );
			}
		}
	}
	else {
		if( GTK_WIDGET_VISIBLE( parent ) ){
			if( !GTK_WIDGET_SENSITIVE( parent ) ){
				gtk_widget_set_sensitive( parent, TRUE );
			}
		}
	}

	W1_HideAndShowAllButton();

	return;	
}

CNMSVoid W1_MainWindowSetSensitiveFalse( CNMSVoid )
{
	main_window_sensitive = CNMS_FALSE;
	gtk_widget_set_sensitive( main_window, FALSE );
}

CNMSVoid W1_MainWindowSetSensitiveTrue( CNMSVoid )
{
	main_window_sensitive = CNMS_TRUE;
	gtk_widget_set_sensitive( main_window, TRUE );
	W1_HideAndShowAllButton();
}

CNMSVoid W1_WidgetGrabFocus( GtkWidget *parent, const gchar *widget_name )
{
	GtkWidget *widget = CNMSNULL;
	
	if( ( widget = (GtkWidget *)lookup_widget( parent, widget_name ) ) == CNMSNULL ){
		DBGMSG( "[W1_WidgetGrabFocus]Can't look up widget(%s).\n", widget_name );
		return;
	}
	else {
		gtk_widget_grab_focus(widget);
	}
}

CNMSInt32 W1_JudgeFormatType(
		CNMSInt32		object			)
{
	CNMSInt32	ret = CNMS_ERR,
				i,
				ftype = CNMS_ERR;

	/* check */
	if( lpW1Comp == CNMSNULL ){
		DBGMSG( "[W1_JudgeFormatType]Parameter[lpW1Comp] is NULL.\n" );
		goto EXIT;
	}

	/* search object */
	for( i = 0 ; ftypeInfo[i].object != CNMS_ERR ; i++ ){
		if( ftypeInfo[i].object == object ){
			ftype = ftypeInfo[i].formatType;
			break;
		}
	}
	if( ftype == CNMS_ERR ){
		DBGMSG( "[W1_JudgeFormatType]Parameter[object] is Invalid.\n",object );
		goto EXIT;
	}

	/* judge format type */
	if( lpW1Comp->formatType < ftype ){
		ret = CNMS_FALSE;
	}
	else{
		ret = CNMS_TRUE;
	}

EXIT:
	return ret;
}

static CNMSVoid W1_CheckMoireWarning(
		CNMSVoid			)
{
	GtkWidget	*notebook	= CNMSNULL;
	CNMSInt32	unsharp		= CNMS_FALSE,
				descreen	= CNMS_FALSE;

	/* check */
	if( lpW1Comp == CNMSNULL ){
		DBGMSG( "[W1_CheckMoireWarning]Parameter[lpW1Comp] is NULL.\n" );
		goto EXIT;
	}
	if( ( notebook = lookup_widget( main_window, "main_notebook" ) ) == CNMSNULL ){
		DBGMSG( "[W1_CheckMoireWarning]Can't look up widget(main_notebook).\n" );
		goto EXIT;
	}

	/* Only Advanced Mode */
	if( gtk_notebook_get_current_page( GTK_NOTEBOOK( notebook ) ) == NOTEBOOK_PAGE_ADVANCED ){
		if( W1_JudgeFormatType( CNMS_OBJ_A_DESCREEN ) == CNMS_TRUE ){
			unsharp		= CnmsUiGetValue( CNMS_OBJ_A_UNSHARP_MASK, lpW1Comp->lpLink, lpW1Comp->linkNum );
			descreen	= CnmsUiGetValue( CNMS_OBJ_A_DESCREEN, lpW1Comp->lpLink, lpW1Comp->linkNum );

			if( ( unsharp == CNMS_TRUE ) && ( descreen == CNMS_TRUE ) ){
				ChildDialogShow( CHILDDIALOG_ID_WARNING_MOIRE, main_window );
			}
		}
	}
EXIT:
	return;
}

static CNMSInt32 W1_InitObjectFormat(
		CNMSVoid			)
{
	CNMSInt32	ret = CNMS_ERR;

	if( ( lpW1Comp->thresholdDef = CnmsColAdjInitThresholdDefault() ) < 0 ){
		DBGMSG( "[W1_InitObjectFormat]Parameter is Invalid.\n" );
		goto EXIT;
	}

	ret = CNMS_NO_ERR;
EXIT:
	return ret;
}

static CNMSInt32 W1_InitUIFormat(
		CNMSVoid			)
{
	CNMSInt32	ret = CNMS_ERR,
				ldata;

	/* Descreen : (Advanced Mode)*/
	if( ( ldata = W1_JudgeFormatType( CNMS_OBJ_A_DESCREEN ) ) == CNMS_ERR ){
		DBGMSG( "[W1_InitUIFormat] Function error [W1_JudgeFormatType() = %d].\n", ldata );
		goto EXIT;
	}
	if( ldata == CNMS_FALSE ){
		gtk_widget_hide( lookup_widget( main_window, "advance_descreen_label" ) );
		gtk_widget_hide( lookup_widget( main_window, "advance_descreen_combo" ) );
	}

	/* Scanner Tab : (Preference) */
	if( ( ldata = W1_JudgeFormatType( CNMS_OBJ_P_TAB_SCANNER ) ) == CNMS_ERR ){
		DBGMSG( "[W1_InitUIFormat] Function error [W1_JudgeFormatType() = %d].\n", ldata );
		goto EXIT;
	}
	if( ldata == CNMS_FALSE ){
		Preference_HideTab( PREF_TAB_SCANNER );
	}
	else {
		/* Silent Mode , Quiet Settings : (Preference) */
		ldata = CnmsUiGetValue( CNMS_OBJ_P_SILENT_MODE, lpW1Comp->lpLink, lpW1Comp->linkNum );
		Preference_InitSilent( ldata );
		
		/* Auto Power Settings : (Preference) */
		if( ( ldata = W1_JudgeFormatType( CNMS_OBJ_P_AUTO_POWER_SETTINGS ) ) == CNMS_ERR ){
			DBGMSG( "[W1_InitUIFormat] Function error [W1_JudgeFormatType() = %d].\n", ldata );
			goto EXIT;
		}
		if( ldata == CNMS_FALSE ){
			/* hide auto power settings */
			ldata = CNMS_P_AUTO_POWER_SETTINGS_NULL;
		}
		else {
			/* get auto power settings type */
			ldata = CnmsUiGetValue( CNMS_OBJ_P_AUTO_POWER_SETTINGS, lpW1Comp->lpLink, lpW1Comp->linkNum );
		}
		Preference_InitAutoPower( ldata );
	}

	/* Turkish % */
	if( GetEnvLocaleID() == CNMS_LOCALE_TR ){
		gtk_widget_hide( lookup_widget( main_window, "advance_scale_label" ) );
	}
	else{
		gtk_widget_hide( lookup_widget( main_window, "advance_scale_label_left" ) );
	}		

	W1Ui_InitUIFormat();

	ret = CNMS_NO_ERR;
EXIT:
	return ret;
}

static CNMSInt32 W1_GetUIObjGroup(
		CNMSInt32		object,
		CNMSInt32		value			)
{
	CNMSInt32	ret	= CNMS_ERR, i;

	for( i = 0 ; uiObjGroup[i].object != CNMS_ERR ; i++ ){
		if( ( uiObjGroup[i].object == object ) && ( uiObjGroup[i].value == value ) ){
			ret = uiObjGroup[i].group;
			break;
		}
	}
	return ret;
}

static CNMSInt32 W1_KeepOldValue(
		CNMSVoid			)
{
	CNMSInt32	ret	= CNMS_NO_ERR, i;

	for( i = 0 ; oldValue[i].object != CNMS_ERR ; i++ ){
		if( oldValue[i].object == CNMS_OBJ_A_OUTPUT_RESOLUTION ){
			if( ( oldValue[i].value = CnmsUiGetRealValue( oldValue[i].object, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) <= 0 ){
				DBGMSG( "[W1_KeepOldValue]Error is occured in CnmsUiGetRealValue( %d ).\n",oldValue[i].object );
			}
		}
		else{
			if( ( oldValue[i].value = CnmsUiGetValue( oldValue[i].object, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
				DBGMSG( "[W1_KeepOldValue]Error is occured in CnmsUiGetValue( %d ).\n",oldValue[i].value );
			}
		}
	}
	
	ret = CNMS_NO_ERR;
EXIT_ERR:
	return	ret;
}

static CNMSInt32 W1_KeepCurrentValue(
		CNMSVoid			)
{
	CNMSInt32	ret	= CNMS_NO_ERR, i;

	for( i = 0 ; currentValue[i].object != CNMS_ERR ; i++ ){
		if( ( currentValue[i].value = CnmsUiGetValue( currentValue[i].object, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
			DBGMSG( "[W1_KeepCurrentValue]Error is occured in CnmsUiGetValue( %d ).\n",currentValue[i].value );
		}
	}
	
	ret = CNMS_NO_ERR;
EXIT_ERR:
	return	ret;
}

static CNMSInt32 W1_PutBackObject(
		CNMSInt32		object,
		CNMSInt32		value			)
{
	CNMSInt32				ret	= CNMS_ERR, i, j, k, putback;
	LPCNMS_UIOBJECTGROUP	lpCurrentObj = CNMSNULL;
	
	if( ( lpCurrentObj = W1_GetObjCurrentValue( object ) ) == CNMSNULL ){
		DBGMSG( "[W1_PutBackObject]Error is occured in W1_GetObjCurrentValue( %d ).\n",object );
		goto	EXIT_NO_ERR;
	}
	for( i = 0 ; uiPutBackObj[i].object != CNMS_ERR ; i++ ){
		if( uiPutBackObj[i].object == object )	break;
	}
	if( lpCurrentObj->group == W1_GetUIObjGroup( object, value ) ){
		for( j = 0 ; j < W1_PUTBACKOBJ_MAX ; j++ ){
			if( ( putback = uiPutBackObj[i].putback[j] ) != CNMS_ERR ){
				for( k = 0 ; oldValue[k].object != CNMS_ERR ; k++ ){
					if( oldValue[k].object == putback )	break;
				}
				if( oldValue[k].value == CNMS_ERR ) {
					continue;
				}
				if( putback == CNMS_OBJ_A_OUTPUT_RESOLUTION ){
					if( CnmsUiSetRealValue( putback, oldValue[k].value, lpW1Comp->lpLink, lpW1Comp->linkNum ) != CNMS_NO_ERR ){
						DBGMSG( "[W1_PutBackObject]Error is occured in CnmsUiSetRealValue( %d ).\n",putback );
						goto	EXIT_ERR;
					}
				}
				else{
					if( CnmsUiSetValue( putback, oldValue[k].value, lpW1Comp->lpLink, lpW1Comp->linkNum ) != CNMS_NO_ERR ){
						DBGMSG( "[W1_PutBackObject]Error is occured in CnmsUiSetValue( %d ).\n",putback );
						goto	EXIT_ERR;
					}
					/* CNMS_S_SOURCE_PHOTO -> CNMS_S_SOURCE_PHOTO : menu link */
					if( lpCurrentObj->group == CNMS_S_SOURCE_PHOTO ) {
						/* menu link */
						if( ( CnmsUiSetChange( putback, oldValue[k].value, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_NO_ERR ){
							DBGMSG( "[W1_PutBackObject]Error is occured in CnmsUiSetChange.\n" );
							goto	EXIT_ERR;
						}
						else if( ( CnmsUiLinkMode( lpW1Comp->lpLink ) ) != CNMS_NO_ERR ){
							DBGMSG( "[W1_PutBackObject]Error is occured in CnmsUiLinkMode.\n" );
							goto	EXIT_ERR;
						}
					}
				}
			}
		}
	}
	else{
		lpCurrentObj->value = value;
		lpCurrentObj->group = W1_GetUIObjGroup( object, value );
	}

EXIT_NO_ERR:
	ret = CNMS_NO_ERR;
EXIT_ERR:
	for( i = 0 ; oldValue[i].object != CNMS_ERR ; i++ ){
		oldValue[i].value = CNMS_ERR;
		oldValue[i].group = CNMS_ERR;
	}
	return	ret;
}

static CNMSInt32 W1_SwitchPaAPutBack(
		CNMSVoid						)
{
	CNMSInt32	ret	= CNMS_ERR, i, j, k, putback, ui_flag_tmp = ui_flag;
	GtkComboBox		*combobox = CNMSNULL;

	for( j = 0 ; j < W1_PUTBACKOBJ_MAX ; j++ ){
		if( ( putback = swPaAPutBackObj.putback[j] ) != CNMS_ERR ){
			for( k = 0 ; currentValue[k].object != CNMS_ERR ; k++ ){
				if( currentValue[k].object == putback )	break;
			}
			if( currentValue[k].value == CNMS_ERR ) {
				continue;
			}
			if( putback == CNMS_OBJ_A_COLOR_MODE ){
				if( ( combobox = (GtkComboBox*)lookup_widget( main_window, "advance_color_combo" ) ) == CNMSNULL ){
					DBGMSG( "[W1_PutBackObject]Error is occured in lookup_widget( advance_color_combo ).\n" );
					goto	EXIT_ERR;
				}
				if( ui_flag_tmp == CNMS_TRUE )	ui_flag = CNMS_FALSE;
				W1_ChangeCombo( combobox, CNMS_OBJ_A_COLOR_MODE );				
				if( ui_flag_tmp == CNMS_TRUE )	ui_flag = CNMS_TRUE;
			}
			if( CnmsUiSetValue( putback, currentValue[k].value, lpW1Comp->lpLink, lpW1Comp->linkNum ) != CNMS_NO_ERR ){
				DBGMSG( "[W1_PutBackObject]Error is occured in CnmsUiSetValue( %d ).\n",putback );
				goto	EXIT_ERR;
			}
		}
	}

	ret = CNMS_NO_ERR;
EXIT_ERR:
	for( i = 0 ; currentValue[i].object != CNMS_ERR ; i++ ){
		currentValue[i].value = CNMS_ERR;
		currentValue[i].group = CNMS_ERR;
	}
	return	ret;
}

static LPCNMS_UIOBJECTGROUP W1_GetObjCurrentValue(
		CNMSInt32		object			)
{
	CNMSInt32				i;
	LPCNMS_UIOBJECTGROUP	ret = CNMSNULL;

	for( i = 0 ; currentUOGroup[i].object != CNMS_ERR ; i++ ){
		if( currentUOGroup[i].object == object ){
			ret = &currentUOGroup[i];
			break;
		}			
	}

	return	ret;
}

CNMSInt32 W1_GetSelectSourceADF(
		CNMSVoid						)
{
	CNMSInt32				ret	= CNMS_ERR;
	LPCNMS_UIOBJECTGROUP	uogroup = CNMSNULL;
	GtkWidget				*widget = CNMSNULL;
	
	if( ( widget = lookup_widget( main_window, "main_notebook" ) ) == CNMSNULL ){
		DBGMSG( "[W1_GetSelectSourceADF]Can't look up widget.\n" );
		goto	EXIT;
	}

	switch( gtk_notebook_get_current_page( GTK_NOTEBOOK( widget ) ) ){
		case	NOTEBOOK_PAGE_SIMPLE:
			if( ( uogroup = W1_GetObjCurrentValue( CNMS_OBJ_S_SOURCE ) ) == CNMSNULL ){
				DBGMSG( "[W1_GetSelectSourceADF]Error is occured in W1_GetObjCurrentValue().\n" );
				goto	EXIT;
			}
			switch( uogroup->group ){
				case	CNMS_S_SOURCE_TEXT_COL_SADF:
					ret = CNMS_TRUE;
					break;
				default:
					ret = CNMS_FALSE;
					break;
			}
			break;
		case	NOTEBOOK_PAGE_ADVANCED:
			if( ( uogroup = W1_GetObjCurrentValue( CNMS_OBJ_A_SOURCE ) ) == CNMSNULL ){
				DBGMSG( "[W1_GetSelectSourceADF]Error is occured in W1_GetObjCurrentValue().\n" );
				goto	EXIT;
			}
			switch( uogroup->group ){
				case	CNMS_A_SOURCE_TEXT_SADF:
					ret = CNMS_TRUE;
					break;
				default:
					ret = CNMS_FALSE;
					break;
			}
			break;
	}
EXIT:
	return	ret;
}

static CNMSBool DisplayDeviceFullName( CNMSVoid )
{
	char *display_name = CNMSNULL;
	CNMSBool ret = CNMS_TRUE;
	
	display_name = getenv( "CANON_SGMP_TITLE" );
	
	if( display_name == CNMSNULL ) {
		goto EXIT;	/* Display DeviceFullName */
	}
	else if ( strcmp( display_name, "OFF" ) == 0 ) {
		ret = CNMS_FALSE;	/* Hide DeviceFullName */
	}
EXIT:
	return ret;
}

CNMSInt32 W1_ShowSelectModelForNetwork(
		CNMSInt32			devnum		)
{
	CNMSInt32		ret = CNMS_ERR, i;
	GtkWidget		*widget = NULL;
	CNMSLPSTR		lpStr;
	CNMSByte		listname[PATH_MAX];
	CNMSInt32		network_model = CnmsGetNetworkModelInstalled();
	static CNMSBool	first_call = CNMS_TRUE;

	if( !network_model && first_call ) {
		if( devnum == 0 ){
			/* show "device not found" */
			W1Ui_SwitchSelectModelFromDevnum( devnum );
			goto	EXIT;
		}			
		else if( devnum == 1 ){
			/* main ui open */
			if( W1_Open( 0 ) != CNMS_NO_ERR ) {
				/* show error message. */
				ShowErrorDialog();
				goto EXIT;
			}
			goto EXIT_NO_ERROR;	/* -> init -> gtk_main */
		}
	}
	
	if( W1Ui_SwitchSelectModelFromDevnum( devnum ) != CNMS_NO_ERR ){
		goto	EXIT;
	}

	gtk_widget_set_sensitive( select_model_dialog, TRUE );

	if( devnum == 0 ) {
		/* focus -> cancel */
		W1_WidgetGrabFocus( select_model_dialog, "select_model_cancel_button" );
	}
	else {
		/* focus -> ok */
		W1_WidgetGrabFocus( select_model_dialog, "select_model_ok_button" );
		
		/* append scanner list */
		if( ( widget = lookup_widget( select_model_dialog, "select_model_combo" ) ) == CNMSNULL ){
			goto	EXIT;
		}
		for( i = 0 ; i < devnum ; i ++ ){
			if( ( lpStr = CnmsScanGetDevModelStr( CNMS_SCAN_GETDEVMODELSTR_FULL, i ) ) == CNMSNULL ){
				goto	EXIT;
			}
			if( snprintf( listname, PATH_MAX, "Canon %s", lpStr ) <= 0 ){
				goto	EXIT;
			}
			gtk_combo_box_append_text( GTK_COMBO_BOX( widget ), _( listname ) );
		}
		gtk_combo_box_set_active( GTK_COMBO_BOX( widget ), CnmsGetDeviceIndexSelectedLatest() );
	}
	
	gtk_widget_show( select_model_dialog );
	
EXIT_NO_ERROR:
	ret = CNMS_NO_ERR;
EXIT:
	first_call = CNMS_FALSE;
	return	ret;
}

CNMSInt32 W1_ReloadSelectModel( CNMSVoid )
{
	CNMSInt32	ret = CNMS_ERR, devNum, status;
	GtkWidget	*widget = NULL;
	CNMSInt32	network_model = CnmsGetNetworkModelInstalled();

	if( ( widget = lookup_widget( select_model_dialog, "select_model_combo" ) ) == CNMSNULL ){
		goto	EXIT_ERR;
	}
	ComGtk_ClearCombo( (GtkComboBox*)widget );

	CnmsScanClose();
	
	if( network_model ){
		if( ProgressBarStart( PROGRESSBAR_ID_SEARCH, -1 ) != CNMS_NO_ERR ) {
			goto EXIT_ERR;
		}
		CnmsReloadNetworkCache();
	}
	
	if( ( devNum = CnmsScanInit( CNMS_FALSE ) ) < 0 ){
		goto	EXIT_ERR;
	}
	
	if( network_model ){
		ProgressBarEnd();
	}
	
	if( W1_ShowSelectModelForNetwork( devNum ) == CNMS_ERR ){
		goto	EXIT_ERR;
	}
	
	ret = CNMS_TRUE;
EXIT_ERR:
	return	ret;
}

CNMSVoid  W1_ChangeSpinAdjustMinusZero( GtkSpinButton *spinbutton )
{
	if( spinbutton == CNMSNULL ){
		DBGMSG( "[W1_ChangeSpinAdjustMinusZero] Arg Widget Pointer[spinbutton] is NULL.\n" );
		return;
	}
	
	if( strncmp( gtk_entry_get_text( GTK_ENTRY( spinbutton ) ), "-0", 2 ) == 0 ){		/* Avoid Show Value "-0"	*/
		gtk_spin_button_set_value( GTK_SPIN_BUTTON( spinbutton ), 1 );
		gtk_spin_button_set_value( GTK_SPIN_BUTTON( spinbutton ), 0 );
	}
}


#endif	/* _W1_MAIN_C_ */
