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

#ifndef	_W1_UI_C_
#define	_W1_UI_C_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <gtk/gtk.h>
#include <gtk/gtkpreview.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "cnmstype.h"
#include "cnmsdef.h"
#include "cnmsstr.h"
#include "cnmsfunc.h"
#include "w1.h"
#include "prev_main.h"
#include "scanmain.h"
#include "com_gtk.h"
#include "child_dialog.h"

#ifdef	__CNMS_DEBUG_UI__
#ifdef	__CNMS_DEBUG_DETAIL__
	#define	__CNMS_DEBUG_UI_DETAIL__
#endif
#endif

#define	W1_WIDGET_MAX		(6)
#define	W1UI_PAGE_MUL		(20)
#define W1UI_WIDGETNAME_MAX	(5)

enum{
	W1_TYPE_COMBO = 0,
	W1_TYPE_SPIN,
	W1_TYPE_ENTRY,
	W1_TYPE_TOGGLE,
	W1_TYPE_BUTTONS,
	W1_TYPE_ETC,
	W1_TYPE_MAX,
};

typedef struct{
	CNMSInt32			obj;
	CNMSInt32			type;
	CNMSLPSTR			str[ W1_WIDGET_MAX ];
}W1WIDGETCOMP, *LPW1WIDGETCOMP;

static	CNMSLPSTR	advance_unsharp_list[]	= {	"advance_unsharp_combo",
												"advance_unsharp_label",
												CNMSNULL						};

static	CNMSLPSTR	advance_descreen_list[]	= {	"advance_descreen_combo",
												"advance_descreen_label",
												CNMSNULL						};

static W1WIDGETCOMP	widgetCompW1[] = {
	{ CNMS_OBJ_S_SOURCE,			W1_TYPE_COMBO,		{ "simple_src_combo",					"simple_src_label",					CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_S_DESTINATION,		W1_TYPE_COMBO,		{ "simple_dst_combo",					"simple_dst_label",					CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_S_OUTPUT_SIZE,		W1_TYPE_COMBO,		{ "simple_output_size_combo",			"simple_output_size_label",			CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_SOURCE,			W1_TYPE_COMBO,		{ "advance_src_combo",					"advance_src_label",				CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_PAPER_SIZE,		W1_TYPE_COMBO,		{ "advance_paper_combo",				"advance_paper_label",				CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_COLOR_MODE,		W1_TYPE_COMBO,		{ "advance_color_combo",				"advance_color_label",				CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_INPUT_WIDTH,		W1_TYPE_SPIN,		{ "advance_input_width_spin",			CNMSNULL,							CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_INPUT_HEIGHT,		W1_TYPE_SPIN,		{ "advance_input_height_spin",			CNMSNULL,							CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_RATIO_FIX,			W1_TYPE_TOGGLE,		{ "advance_size_lock_toggle",			CNMSNULL,							CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_UNIT,				W1_TYPE_COMBO,		{ "advance_unit_combo",					CNMSNULL,							CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_OUTPUT_RESOLUTION,	W1_TYPE_COMBO,		{ "advance_output_resolution_combo",	"advance_output_resolution_label",	CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_OUTPUT_SIZE,		W1_TYPE_COMBO,		{ "advance_output_size_combo",			"advance_output_size_label",		CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_OUTPUT_WIDTH,		W1_TYPE_SPIN,		{ "advance_output_width_spin",			CNMSNULL,							CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_OUTPUT_HEIGHT,		W1_TYPE_SPIN,		{ "advance_output_height_spin",			CNMSNULL,							CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_SCALE,				W1_TYPE_SPIN,		{ "advance_scale_spin",					CNMSNULL,							CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_DATA_SIZE,			W1_TYPE_ENTRY,		{ "advance_data_size_entry",			CNMSNULL,							CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_COLOR_ADJUST,		W1_TYPE_BUTTONS,	{ "advance_BC_button", "advance_HG_button", "advance_TC_button", "advance_FR_button", "advance_MC_button", "advance_reset_button" } },
	{ CNMS_OBJ_A_UNSHARP_MASK,		W1_TYPE_COMBO,		{ "advance_unsharp_combo",				"advance_unsharp_label",			CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_DESCREEN,			W1_TYPE_COMBO,		{ "advance_descreen_combo",				"advance_descreen_label",			CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_IMAGE_CORRECT,		W1_TYPE_ETC,		{ (CNMSLPSTR)advance_unsharp_list, 		(CNMSLPSTR)advance_descreen_list,	CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_OBJ_A_BINDING_LOCATION,	W1_TYPE_COMBO,		{ "advance_binding_location_combo",		"advance_binding_location_label",		CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
	{ CNMS_ERR,						CNMS_ERR,			{ CNMSNULL,								CNMSNULL,							CNMSNULL, CNMSNULL,	CNMSNULL, CNMSNULL } },
};

typedef struct{
	CNMSInt32	object;
	CNMSInt32	def;
	CNMSLPSTR	widgetname[W1UI_WIDGETNAME_MAX];
}CNMS_UISETINFO, *LPCNMS_UISETINFO;

static const CNMS_UISETINFO uiSetInfo[] = {
	{ CNMS_UI_S_PREVIEW			, CNMS_MODE_ADD		, { "simple_preview_table"			, CNMSNULL						, CNMSNULL, CNMSNULL, CNMSNULL }	},
	{ CNMS_UI_S_OUTPUT_SIZE		, CNMS_MODE_ADD		, { "simple_output_size_table"		, CNMSNULL						, CNMSNULL, CNMSNULL, CNMSNULL }	},
	{ CNMS_UI_S_CROP_ADJUST		, CNMS_MODE_ADD		, { "simple_crop_adjust_table"		, CNMSNULL						, CNMSNULL, CNMSNULL, CNMSNULL }	},
	{ CNMS_UI_S_SCAN_STR2		, CNMS_MODE_DELETE	, { "simple_no2_scan_label"			, CNMSNULL						, CNMSNULL, CNMSNULL, CNMSNULL }	},
	{ CNMS_UI_S_SCAN_STR3		, CNMS_MODE_ADD		, { "simple_no3_scan_label"			, CNMSNULL						, CNMSNULL, CNMSNULL, CNMSNULL }	},
	{ CNMS_UI_A_BINDING_LOCATION, CNMS_MODE_DELETE	, { "advance_binding_location_label"	, "advance_binding_location_combo"	, CNMSNULL, CNMSNULL, CNMSNULL }	},
	{ CNMS_UI_A_OUTPUT_SIZE		, CNMS_MODE_ADD		, { "advance_output_size_label"		, "advance_output_size_combo"	, CNMSNULL, CNMSNULL, CNMSNULL }	},
	{ CNMS_UI_A_PREVIEW			, CNMS_MODE_ADD		, { "advance_preview_button"		, CNMSNULL						, CNMSNULL, CNMSNULL, CNMSNULL }	},
	{ CNMS_ERR					, CNMS_ERR			, { CNMSNULL						, CNMSNULL						, CNMSNULL, CNMSNULL, CNMSNULL }	},
};


static LPW1WIDGETCOMP GetWidgetCompW1( CNMSInt32 obj );
static GtkWidget * GetWidgetW1( CNMSInt32 obj );

static CNMSInt32 SubInitUI( LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum, LPW1WIDGETCOMP lpWidgetComp );
static CNMSInt32 SubInitCombo( CNMSInt32 object, LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum, CNMSLPSTR lpWidgetStr );
static CNMSInt32 SubInitSpinBox( CNMSInt32 object, LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum, CNMSLPSTR lpWidgetStr );
static CNMSInt32 SubInitToggleButton( CNMSInt32 object, LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum, CNMSLPSTR lpWidgetStr );
static CNMSInt32 SubInitButtons( CNMSInt32 object, LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum, CNMSLPSTR *lplpWidgetStr );
static CNMSInt32 SubInitETC( CNMSInt32 object, LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum, CNMSLPSTR *lplpWidgetStr );

static CNMSInt32 RelocateUI( CNMSInt32 key, LPW1WIDGETCOMP lpWidgetComp );

static CNMSVoid ClipSize( CNMSInt32 *lpScale );

static CNMSInt32 ShowScale( CNMSInt32 *lpScale );
static CNMSInt32 ShowInSize( CNMSInt32 unit, CNMSInt32 outRes, CNMSInt32 decFig, CNMSDec32 step );
static CNMSInt32 SubSetInSizeLowerUpper( CNMSInt32 unit, CNMSInt32 outRes, CNMSInt32 decFig );

static CNMSInt32 ShowOutSize( CNMSInt32 unit, CNMSInt32 outRes, CNMSInt32 *lpScale, CNMSInt32 decFig, CNMSDec32 step );
static CNMSInt32 ShowDataSize( CNMSVoid );

static CNMSInt32 SubAspectToggleSet( CNMSInt32 aspect, CNMSInt32 rectSizeH );

#ifdef	__CNMS_DEBUG__
static CNMSVoid DebugAdjustment( CNMSLPSTR lpStr, GtkAdjustment *adjust_ment );
#endif

static CNMSInt32	inObjArray[ CNMS_DIM_MAX ] = { CNMS_OBJ_A_INPUT_WIDTH, CNMS_OBJ_A_INPUT_HEIGHT };
static CNMSInt32	outObjArray[ CNMS_DIM_MAX ] = { CNMS_OBJ_A_OUTPUT_WIDTH, CNMS_OBJ_A_OUTPUT_HEIGHT };

static LPW1WIDGETCOMP GetWidgetCompW1(
		CNMSInt32		obj )
{
	LPW1WIDGETCOMP	lpRet = CNMSNULL;
	CNMSInt32		i;
	
	for( i = 0 ; widgetCompW1[ i ].obj != CNMS_ERR ; i ++ ){
		if( widgetCompW1[ i ].obj == obj ){
			lpRet = widgetCompW1 + i;
			break;
		}
	}
EXIT:
	return	lpRet;
}

static GtkWidget * GetWidgetW1(
		CNMSInt32	obj )
{
	GtkWidget		*widget = CNMSNULL;
	LPW1WIDGETCOMP	lpWidgetComp;
	
	if( main_window == CNMSNULL ){
		DBGMSG( "[GetWidgetW1]Status is error.\n" );
		goto	EXIT;
	}
	else if( ( lpWidgetComp = GetWidgetCompW1( obj ) ) == CNMSNULL ){
		DBGMSG( "[GetWidgetW1]Error is occured in GetWidgetCompW1!\n" );
		goto	EXIT;
	}
	else if( ( widget = lookup_widget( main_window, (const gchar *)( lpWidgetComp->str[ 0 ] ) ) ) == CNMSNULL ){
		DBGMSG( "[GetWidgetW1]Can't get widget( %s ).\n", lpWidgetComp->str[ 0 ] );
		goto	EXIT;
	}
EXIT:
	return	widget;
}

CNMSInt32 W1Ui_HideNoUseBox( CNMSVoid )
{
	CNMSInt32		ret = CNMS_ERR, obj, i, ldata;
	GtkWidget		*widget;
	LPW1WIDGETCOMP	lpWidgetComp;

	if( ( lpW1Comp == CNMSNULL ) || ( main_window == CNMSNULL ) ){
		DBGMSG( "[W1Ui_HideNoUseBox]Status is error.\n" );
		goto	EXIT;
	}
	for( obj = 0 ; obj < CNMS_OBJ_MAX ; obj ++ ){
		if( ( ( lpWidgetComp = GetWidgetCompW1( obj ) ) == CNMSNULL ) || ( lpWidgetComp->str[ 0 ] == CNMSNULL ) ){
			continue;
		}
		for( i = 0 ; i < lpW1Comp->linkNum ; i ++ ){
			if( lpW1Comp->lpLink[ i ].object == obj ){
				break;
			}
		}
		if( i == lpW1Comp->linkNum ){
			for( i = 0 ; i < W1_WIDGET_MAX ; i ++ ){
				if( lpWidgetComp->type != W1_TYPE_ETC ){
					if( lpWidgetComp->str[ i ] != CNMSNULL ){
						if( ( widget = lookup_widget( main_window, (const gchar *)lpWidgetComp->str[ i ] ) ) == CNMSNULL ){
							DBGMSG( "[W1Ui_HideNoUseBox]Can't look up widget( %s ).\n", lpWidgetComp->str[ i ] );
							goto	EXIT;
						}
						gtk_widget_hide( widget );
					}
				}
			}
		}
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1Ui_HideNoUseBox()]= %d.\n", ret );
#endif
	return	ret;
}

CNMSInt32 W1Ui_Init( CNMSVoid )
{
	CNMSInt32			ret = CNMS_ERR, i, ldata, count = 0, hideCnt = 0, key,
						usiCnt, wnCnt;
	LPCNMSUILINKCOMP	lpLink, lpLink2;
	LPW1WIDGETCOMP		lpWidgetComp;
	LPCNMSUILINKCOMP	lplinktop	= CNMSNULL;
	GtkWidget			*widget		= CNMSNULL;

	if( ( lpW1Comp == CNMSNULL ) || ( main_window == CNMSNULL ) ){
		DBGMSG( "[W1Ui_Init]Initialize parameter is error.\n" );
		goto	EXIT;
	}
	lpLink = lpLink2 = lpW1Comp->lpLink;
	for( i = 0 ; i < lpW1Comp->linkNum ; i ++, lpLink ++ ){
		if( lpLink->mode == CNMS_MODE_DELETE ){
			count ++;
		}
		else if( lpLink->mode == CNMS_MODE_HIDE ){
			hideCnt ++;
		}
		if( ( i == lpW1Comp->linkNum - 1 ) || ( lpLink->object != ( lpLink + 1 )->object ) ){
			if( ( lpWidgetComp = GetWidgetCompW1( lpLink2->object ) ) != CNMSNULL ){
				if( ( ldata = SubInitUI( lpLink2, lpLink - lpLink2 + 1, lpWidgetComp ) ) != CNMS_NO_ERR ){
					DBGMSG( "[W1Ui_Init]Error is occured in SubInitUI.\n" );
					goto	EXIT;
				}
				/* delete or hide or add */
				if( count == lpLink - lpLink2 + 1 ){
					key = CNMS_MODE_DELETE;
				}
				else if( hideCnt != 0 ){
					key = CNMS_MODE_HIDE;
				}
				else{
					key = CNMS_MODE_ADD;
				}
				if( ( ldata = RelocateUI( key, lpWidgetComp ) ) != CNMS_NO_ERR ){
					DBGMSG( "[W1Ui_Init]Error is occured in RelocateUI.\n" );
					goto	EXIT;
				}
			}
			if( i != lpW1Comp->linkNum -1 ){
				lpLink2 = lpLink + 1;
				count = hideCnt = 0;
			}
		}
	}

	/* User Interface switching (ex. Platen <=> ADF) */
	if( W1_JudgeFormatType( CNMS_OBJ_UI ) == CNMS_TRUE ){
		usiCnt = 0;
		while( uiSetInfo[usiCnt].object != CNMS_ERR ){
			lplinktop = lpW1Comp->lpLink;
			for( i = 0 ; i < lpW1Comp->linkNum ; i++, lplinktop++ ){
				if( ( lplinktop->object == CNMS_OBJ_UI ) && ( lplinktop->value == uiSetInfo[usiCnt].object ) ){
					for( wnCnt = 0 ; wnCnt < W1UI_WIDGETNAME_MAX ; wnCnt++ ){
						if( uiSetInfo[usiCnt].widgetname[wnCnt] != CNMSNULL ){
							if( ( widget = lookup_widget( main_window, uiSetInfo[usiCnt].widgetname[wnCnt] ) ) == CNMSNULL ){
								goto EXIT;
							}
							switch( lplinktop->mode ){
								case CNMS_MODE_SELECT:
								case CNMS_MODE_ADD:
										gtk_widget_show( widget );
										gtk_widget_set_sensitive( widget, CNMS_TRUE );	break;

								case CNMS_MODE_DELETE:
										gtk_widget_hide( widget );						break;

								case CNMS_MODE_HIDE:
										gtk_widget_set_sensitive( widget, CNMS_FALSE );	break;

								default: 												break;
							}
						}
					}
				}
			}
			usiCnt++;
		}
	}


EXIT_NO_ERR:
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1Ui_Init()]=%d\n", ret );
#endif
	return	ret;
}

static CNMSInt32 SubInitUI(
		LPCNMSUILINKCOMP	lpLink,
		CNMSInt32			linkNum,
		LPW1WIDGETCOMP		lpWidgetComp )
{
	CNMSInt32		ret = CNMS_ERR, ldata;

	switch( lpWidgetComp->type ){
		case	W1_TYPE_COMBO:
			ldata = SubInitCombo( lpLink->object, lpLink, linkNum, lpWidgetComp->str[ 0 ] );
			break;
		case	W1_TYPE_SPIN:
			ldata = SubInitSpinBox( lpLink->object, lpLink, linkNum, lpWidgetComp->str[ 0 ] );
			break;
		case	W1_TYPE_TOGGLE:
			ldata = SubInitToggleButton( lpLink->object, lpLink, linkNum, lpWidgetComp->str[ 0 ] );
			break;
		case	W1_TYPE_BUTTONS:
			ldata = SubInitButtons( lpLink->object, lpLink, linkNum, lpWidgetComp->str );
			break;
		case	W1_TYPE_ETC:
			ldata = SubInitETC( lpLink->object, lpLink, linkNum, lpWidgetComp->str );
			break;
		default:
			ldata = CNMS_NO_ERR;
			break;
	}

	if( ldata != CNMS_NO_ERR ){
		goto	EXIT;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI_DETAIL__
	DBGMSG( "\t[SubInitUI()]=%d.\n", ret );
#endif
	return	ret;
}

static CNMSInt32 SubInitCombo(
		CNMSInt32			obj,
		LPCNMSUILINKCOMP	lpLink,
		CNMSInt32			linkNum,
		CNMSLPSTR			lpWidgetStr )
{
	GtkComboBox		*combobox;
	CNMSInt32		ret = CNMS_ERR, i, count = 0, defCnt = CNMS_ERR, ldata, bits;
	CNMSLPSTR		lpStr;

	if( ( combobox = (GtkComboBox *)lookup_widget( main_window, (const gchar *)lpWidgetStr ) ) == CNMSNULL ){
		DBGMSG( "[SubInitCombo]Can't get combobox( %s ).\n", lpWidgetStr );
		goto	EXIT;
	}
	ComGtk_ClearCombo( combobox );
	
	for( i = 0 ; i < linkNum ; i ++, lpLink ++ ){
		if( ( lpLink->mode == CNMS_MODE_ADD ) || ( lpLink->mode == CNMS_MODE_SELECT ) || ( lpLink->mode == CNMS_MODE_HIDE ) ){
			if( ( lpStr = CnmsChgValToStr( lpLink->value, obj ) ) == CNMSNULL ){
				DBGMSG( "[SubInitCombo]Can't get string( value = %d ).\n", lpLink->value );
				goto	EXIT;
			}
			if( ( lpLink->mode == CNMS_MODE_SELECT ) || ( lpLink->mode == CNMS_MODE_HIDE ) ){
				defCnt = count;
			}
			gtk_combo_box_append_text( combobox, lpStr );
			count ++;
		}
	}
	if( defCnt != CNMS_ERR ){
		gtk_combo_box_set_active( combobox, defCnt );
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI_DETAIL__
	DBGMSG( "\t[SubInitCombo(obj:%d)]=%d.\n", obj, ret );
#endif
	return	ret;
}

static CNMSInt32 SubInitSpinBox(
		CNMSInt32			obj,
		LPCNMSUILINKCOMP	lpLink,
		CNMSInt32			linkNum,
		CNMSLPSTR			lpWidgetStr )
{
	GtkEntry		*entry;
	CNMSInt32		ret = CNMS_ERR, ldata, i;

	if( ( entry = (GtkEntry *)lookup_widget( main_window, (const gchar *)lpWidgetStr ) ) == CNMSNULL ){
		DBGMSG( "[SubInitSpinBox]Can't get entry( %s ).\n", lpWidgetStr );
		goto	EXIT;
	}
	
	for( i = 0 ; i < linkNum ; i ++, lpLink ++ ){
		if( ( ( lpLink->mode == CNMS_MODE_SELECT ) || ( lpLink->mode == CNMS_MODE_HIDE ) ) && ( lpLink->value == CNMS_VALUE_NULL ) ){
			gtk_entry_set_text( entry, "" );
		}
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI_DETAIL__
	DBGMSG( "\t[SubInitSpinBox(obj:%d)]=%d.\n", obj, ret );
#endif
	return	ret;
}

static CNMSInt32 SubInitToggleButton(
		CNMSInt32			obj,
		LPCNMSUILINKCOMP	lpLink,
		CNMSInt32			linkNum,
		CNMSLPSTR			lpWidgetStr )
{
	CNMSInt32		ret = CNMS_ERR, ldata;

	switch( obj ){
		case	CNMS_OBJ_A_RATIO_FIX:
			if( ( ldata = W1Ui_AspectReflect() ) != CNMS_NO_ERR ){
				DBGMSG( "[SubInitToggleButton]Error is occured in W1Ui_AspectReflect.\n" );
				goto	EXIT;
			}
			break;
		default:
			DBGMSG( "[SubInitToggleButton]Object(%d) is not supported.\n", obj );
			goto	EXIT;
			break;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI_DETAIL__
	DBGMSG( "\t[SubInitToggleButton(obj:%d)]=%d.\n", obj, ret );
#endif
	return	ret;
}

static CNMSInt32 SubInitButtons(
		CNMSInt32			obj,
		LPCNMSUILINKCOMP	lpLink,
		CNMSInt32			linkNum,
		CNMSLPSTR			*lplpWidgetStr )
{
	GtkWidget		*widget;
	CNMSLPSTR		lpStr;
	CNMSInt32		ret = CNMS_ERR, i;

	if( linkNum > W1_WIDGET_MAX ){
		DBGMSG( "[SubInitButtons]Link number is too much( %d ).\n", linkNum );
		goto	EXIT;
	}

	for( i = 0 ; i < linkNum ; i ++, lpLink ++ ){
		if( ( lpStr = lplpWidgetStr[ i ] ) == CNMSNULL ){
			continue;
		}
		else if( ( widget = lookup_widget( main_window, (const gchar *)lpStr ) ) == CNMSNULL ){
			DBGMSG( "[SubInitButtons]Can't look up widget( %s ).\n", lpStr );
			goto	EXIT;
		}

		switch( lpLink->mode ){
			case	CNMS_MODE_SELECT:
			case	CNMS_MODE_ADD:
				if( GTK_WIDGET_SENSITIVE( widget ) == 0 ){
					gtk_widget_set_sensitive( widget, TRUE );
				}
				if( GTK_WIDGET_VISIBLE( widget ) == 0 ){
					gtk_widget_show( widget );
				}
				break;
			case	CNMS_MODE_HIDE:
				if( GTK_WIDGET_SENSITIVE( widget ) != 0 ){
					gtk_widget_set_sensitive( widget, FALSE );
				}
				if( GTK_WIDGET_VISIBLE( widget ) == 0 ){
					gtk_widget_show( widget );
				}
				break;
			
			case	CNMS_MODE_DELETE:
				if( GTK_WIDGET_VISIBLE( widget ) != 0 ){
					gtk_widget_hide( widget );
				}
				break;
			default:
				goto	EXIT;
				break;
		}
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI_DETAIL__
	DBGMSG( "\t[SubInitButtons(obj:%d)]=%d.\n", obj, ret );
#endif
	return	ret;
}

static CNMSInt32 SubInitETC(
		CNMSInt32			obj,
		LPCNMSUILINKCOMP	lpLink,
		CNMSInt32			linkNum,
		CNMSLPSTR			*lplpWidgetStr )
{
	GtkWidget		*widget;
	CNMSLPSTR		lpStr;
	CNMSInt32		ret = CNMS_ERR, i;
	CNMSLPSTR		*lplpStr;

	if( linkNum > W1_WIDGET_MAX ){
		DBGMSG( "[SubInitButtons]Link number is too much( %d ).\n", linkNum );
		goto	EXIT;
	}

	for( i = 0 ; i < linkNum ; i ++, lpLink ++ ){
		if( ( lpStr = lplpWidgetStr[ i ] ) == CNMSNULL ){
			continue;
		}
		lplpStr = ( CNMSLPSTR* )lplpWidgetStr[ i ];
		while( *lplpStr != CNMSNULL ){
			if( ( widget = lookup_widget( main_window, (const gchar *)*lplpStr ) ) == CNMSNULL ){
				DBGMSG( "[SubInitButtons]Can't look up widget( %s ).\n", lplpStr );
				goto	EXIT;
			}

			switch( lpLink->mode ){
				case	CNMS_MODE_SELECT:
				case	CNMS_MODE_ADD:
					if( GTK_WIDGET_SENSITIVE( widget ) == 0 ){
						gtk_widget_set_sensitive( widget, TRUE );
					}
					if( GTK_WIDGET_VISIBLE( widget ) == 0 ){
						gtk_widget_show( widget );
					}
					break;
				case	CNMS_MODE_HIDE:
					if( GTK_WIDGET_SENSITIVE( widget ) != 0 ){
						gtk_widget_set_sensitive( widget, FALSE );
					}
					if( GTK_WIDGET_VISIBLE( widget ) == 0 ){
						gtk_widget_show( widget );
					}
					break;
				
				case	CNMS_MODE_DELETE:
					if( GTK_WIDGET_VISIBLE( widget ) != 0 ){
						gtk_widget_hide( widget );
					}
					break;
				default:
					goto	EXIT;
					break;
			}
			lplpStr++;
		}
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI_DETAIL__
	DBGMSG( "\t[SubInitButtons(obj:%d)]=%d.\n", obj, ret );
#endif
	return	ret;
}

static CNMSInt32 RelocateUI(
		CNMSInt32		key,
		LPW1WIDGETCOMP	lpWidgetComp )
{
	CNMSInt32	ret = CNMS_ERR, i;
	CNMSLPSTR	lpStr;
	GtkWidget	*widget;

	if( ( lpWidgetComp->type == W1_TYPE_BUTTONS ) || ( lpWidgetComp->type == W1_TYPE_TOGGLE ) || ( lpWidgetComp->type == W1_TYPE_ETC ) ){
		goto	EXIT_NO_ERR;
	}
	for( i = 0 ; i < W1_WIDGET_MAX ; i ++ ){
		if( ( lpStr = lpWidgetComp->str[ i ] ) == CNMSNULL ){
			continue;
		}
		else if( ( widget = lookup_widget( main_window, (const gchar *)lpStr ) ) == CNMSNULL ){
			DBGMSG( "[RelocateUI]Can't look up widget( %s ).\n", lpStr );
			goto	EXIT;
		}

		switch( key ){
			case	CNMS_MODE_ADD:
				if( GTK_WIDGET_SENSITIVE( widget ) == 0 ){
					gtk_widget_set_sensitive( widget, TRUE );
				}
				if( GTK_WIDGET_VISIBLE( widget ) == 0 ){
					gtk_widget_show( widget );
				}
				break;
			case	CNMS_MODE_HIDE:
				if( GTK_WIDGET_SENSITIVE( widget ) != 0 ){
					gtk_widget_set_sensitive( widget, FALSE );
				}
				if( GTK_WIDGET_VISIBLE( widget ) == 0 ){
					gtk_widget_show( widget );
				}
				break;
			case	CNMS_MODE_DELETE:
				if( GTK_WIDGET_VISIBLE( widget ) != 0 ){
					gtk_widget_hide( widget );
				}
				break;
			default:
				goto	EXIT;
				break;
		}
	}
EXIT_NO_ERR:
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI_DETAIL__
	DBGMSG( "\t[RelocateUI(mode:%d)]=%d.\n", key, ret );
#endif
	return	ret;
}

CNMSInt32 W1Ui_ShowSize( CNMSVoid )
{
	CNMSInt32		ret = CNMS_ERR, ldata, unit, decFig, outRes, minmaxScale[ 2 ];
	CNMSDec32		step;

	if( ( lpW1Comp == CNMSNULL ) || ( main_window == CNMSNULL ) ){
		DBGMSG( "[W1Ui_ShowSize]Initialize parameter is error.\n" );
		goto	EXIT;
	}
	/* get param */
	else if( ( unit = CnmsUiGetValue( CNMS_OBJ_A_UNIT, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[W1Ui_ShowSize]Error is occured in CnmsUiGetValue( CNMS_OBJ_A_UNIT ).\n" );
		goto	EXIT;
	}
	else if( ( outRes = CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) <= 0 ){
		DBGMSG( "[W1Ui_ShowSize]Error is occured in CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION ).\n" );
		goto	EXIT;
	}
	/* get min and max scale (unit:percent) */
	if( ( ldata = CnmsScanGetMinMaxScale( outRes, minmaxScale ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1Ui_ShowSize]Can't get min and max scale.\n" );
		goto	EXIT;
	}
	/* get decimal figure */
	else if( ( decFig = CnmsGetDecFig( unit ) ) < 0 ){
		DBGMSG( "[W1Ui_ShowSize]Error is occured in CnmsGetDecFig.\n" );
		goto	EXIT;
	}
	/* clip scale and output size */
	ClipSize( minmaxScale );
	/* get step */
	step = 1;
	for( ldata = 0 ; ldata < decFig ; ldata ++ ){
		step = step / 10;
	}
	/* change ui */
	/* scale */
	if( ( ldata = ShowScale( minmaxScale ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1Ui_ShowSize]Error is occured in ShowScale.\n" );
		goto	EXIT;
	}
	/* input size */
	if( ( ldata = ShowInSize( unit, outRes, decFig, step ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1Ui_ShowSize]Error is occured in ShowInSize.\n" );
		goto	EXIT;
	}
	/* output size */
	if( ( ldata = ShowOutSize( unit, outRes, minmaxScale, decFig, step ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1Ui_ShowSize]Error is occured in ShowOutSize.\n" );
		goto	EXIT;
	}
	/* data size */
	if( ( ldata = ShowDataSize() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1Ui_ShowSize]Error is occured in ShowDataSize.\n" );
		goto	EXIT;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1Ui_ShowSize()]=%d.\n", ret );
#endif
	return	ret;
}

static CNMSVoid ClipSize(
		CNMSInt32		*lpScale )
{
	CNMSInt32		i, min, max, scale;

	/* clip scale (%) */
	if( lpW1Comp->group == CNMSUI_SIZE_GROUP_DISPLAY ){
		goto	EXIT;
	}
	
	scale = lpW1Comp->scale / ( CNMS_SCALE_100_PERCENT / 100 );
	/* min */
	if( scale < lpScale[ 0 ] ){
		lpW1Comp->scale = lpScale[ 0 ] * ( CNMS_SCALE_100_PERCENT / 100 );
	}
	/* max */
	else if( lpScale[ 1 ] < scale ){
		lpW1Comp->scale = lpScale[ 1 ] * ( CNMS_SCALE_100_PERCENT / 100 );
	}

	/* clip outSize */
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		/* min */
		min = (CNMSInt32)( (CNMSDec32)( lpW1Comp->inSize[ i ] ) * (CNMSDec32)( lpScale[ 0 ] ) / 100 );
		/* max */
		max = (CNMSInt32)( (CNMSDec32)( lpW1Comp->inSize[ i ] ) * (CNMSDec32)( lpScale[ 1 ] ) / 100 );
		/* clip */
		lpW1Comp->outSize[ i ] = ( lpW1Comp->outSize[ i ] < min ) ? min : ( ( max < lpW1Comp->outSize[ i ] ) ? max : lpW1Comp->outSize[ i ] );
	}
EXIT:
	return;
}

static CNMSInt32 ShowScale(
		CNMSInt32		*lpScale )
{
	CNMSInt32		ret = CNMS_ERR;
	GtkSpinButton	*spin_button;
	GtkAdjustment	*adjust_ment;

	if( lpW1Comp->group != CNMSUI_SIZE_GROUP_FREE ){
		goto	EXIT_NO_ERR;
	}

	if( ( spin_button = (GtkSpinButton *)GetWidgetW1( CNMS_OBJ_A_SCALE ) ) == CNMSNULL ){
		DBGMSG( "[ShowScale]Can't get widget( CNMS_OBJ_A_SCALE ).\n" );
		goto	EXIT;
	}
	else if( ( adjust_ment = gtk_spin_button_get_adjustment( spin_button ) ) == CNMSNULL ){
		DBGMSG( "[ShowScale]Can't get adjustment of spin button at scale.\n" );
		goto	EXIT;
	}

	adjust_ment->lower = (gdouble)( lpScale[ 0 ] );
	adjust_ment->upper = (gdouble)( lpScale[ 1 ] );
	adjust_ment->value = (gdouble)( lpW1Comp->scale / ( CNMS_SCALE_100_PERCENT / 100 ) );
	gtk_spin_button_set_adjustment( spin_button, adjust_ment );
	gtk_spin_button_set_value( spin_button, adjust_ment->value );
EXIT_NO_ERR:
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "\t[ShowScale(minScale:%d, maxScale%d, scale:%d)]=%d.\n", lpScale[ 0 ], lpScale[ 1 ], lpW1Comp->scale, ret );
#endif
	return	ret;
}

static CNMSInt32 ShowInSize(
		CNMSInt32		unit,
		CNMSInt32		outRes,
		CNMSInt32		decFig,
		CNMSDec32		step )
{
	CNMSInt32		ret = CNMS_ERR, i;
	gdouble			gval;
	GtkSpinButton	*spin_button;
	GtkAdjustment	*adjust_ment;

	if( lpW1Comp->group != CNMSUI_SIZE_GROUP_FREE ){
		goto	EXIT_NO_ERR;
	}
	/* get lower/upper */
	if( ( i = SubSetInSizeLowerUpper( unit, outRes, decFig ) ) != CNMS_NO_ERR ){
		DBGMSG( "[ShowInSize]Error is occured in SubSetInSizeLowerUpper.\n" );
		goto	EXIT;
	}

	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( ( spin_button = (GtkSpinButton *)GetWidgetW1( inObjArray[ i ] ) ) == CNMSNULL ){
			DBGMSG( "[ShowInSize]Can't get widget(inputSize:%d).\n", i );
			goto	EXIT;
		}
		else if( ( adjust_ment = gtk_spin_button_get_adjustment( spin_button ) ) == CNMSNULL ){
			DBGMSG( "[ShowInSize]Can't get adjustment of spin button(inputSize:%d).\n", i );
			goto	EXIT;
		}
		/* set value */
		adjust_ment->step_increment = (gdouble)step;
		adjust_ment->page_increment = (gdouble)step * W1UI_PAGE_MUL;
		gval = (gdouble)CnmsConvUnit( (CNMSDec32)( lpW1Comp->inSize[ i ] ), CNMS_A_UNIT_PIXEL, unit, lpW1Comp->base_res, outRes );
		adjust_ment->value = ( gval < adjust_ment->lower ) ? adjust_ment->lower : ( ( adjust_ment->upper < gval ) ? adjust_ment->upper : gval );
		gtk_spin_button_set_adjustment( spin_button, adjust_ment );
		gtk_spin_button_set_digits( spin_button, decFig );
		gtk_spin_button_set_value( spin_button, adjust_ment->value );
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "\t[ShowInSize(%d)]lower:%f, upper:%f, value:%f.\n", i, adjust_ment->lower, adjust_ment->upper, adjust_ment->value );
#endif
	}
EXIT_NO_ERR:
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "\t[ShowInSize(unit:%d, outRes:%d, decFig:%d, step:%f)]=%d.\n", unit, outRes, decFig, step, ret );
#endif

	return	ret;
}

static CNMSInt32 SubSetInSizeLowerUpper(
		CNMSInt32		unit,
		CNMSInt32		outRes,
		CNMSInt32		decFig )
{
	CNMSInt32		ret = CNMS_ERR, i, aspect;
	CNMSInt32		minSize[ CNMS_DIM_MAX ], maxSize[ CNMS_DIM_MAX ];
	GtkSpinButton	*spin_button;
	GtkAdjustment	*adjust_ment;

	/* get minSize/maxSize */
	if( ( i = Preview_GetMinMaxSize( minSize, maxSize ) ) != CNMS_NO_ERR ){
		DBGMSG( "[SubSetInSizeLowerUpper]Error is occured in Preview_GetMinMaxSize.\n" );
		goto	EXIT;
	}
	else if( ( aspect = CnmsUiAspectGet( lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[SubSetInSizeLowerUpper]Error is occured in CnmsUiAspectGet.\n" );
		goto	EXIT;
	}

	if( aspect == CNMS_ASPECT_FIXED ){
		if( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_H ] ) * (CNMSDec32)( minSize[ CNMS_DIM_V ] ) < (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_V ] ) * (CNMSDec32)( minSize[ CNMS_DIM_H ] ) ){
			minSize[ CNMS_DIM_V ] = (CNMSInt32)( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_V ] ) * (CNMSDec32)( minSize[ CNMS_DIM_H ] ) / (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_H ] ) );
		}
		else{
			minSize[ CNMS_DIM_H ] = (CNMSInt32)( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_H ] ) * (CNMSDec32)( minSize[ CNMS_DIM_V ] ) / (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_V ] ) );
		}
		/* max size */
		if( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_V ] ) * (CNMSDec32)( maxSize[ CNMS_DIM_H ] ) < (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_H ] ) * (CNMSDec32)( maxSize[ CNMS_DIM_V ] ) ){
			maxSize[ CNMS_DIM_V ] = (CNMSInt32)( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_V ] ) * (CNMSDec32)( maxSize[ CNMS_DIM_H ] ) / (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_H ] ) );
		}
		else{
			maxSize[ CNMS_DIM_H ] = (CNMSInt32)( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_H ] ) * (CNMSDec32)( maxSize[ CNMS_DIM_V ] ) / (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_V ] ) );
		}
	}

	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( ( spin_button = (GtkSpinButton *)GetWidgetW1( inObjArray[ i ] ) ) == CNMSNULL ){
			DBGMSG( "[SubSetInSizeLowerUpper]Can't get widget(inputSize:%d).\n", i );
			goto	EXIT;
		}
		else if( ( adjust_ment = gtk_spin_button_get_adjustment( spin_button ) ) == CNMSNULL ){
			DBGMSG( "[SubSetInSizeLowerUpper]Can't get adjustment of spin button(inputSize:%d).\n", i );
			goto	EXIT;
		}
		/* set value */
		adjust_ment->lower = (gdouble)CnmsConvUnit( (CNMSDec32)( minSize[ i ] ), CNMS_A_UNIT_PIXEL, unit, lpW1Comp->base_res, outRes );
		adjust_ment->upper = (gdouble)CnmsConvUnit( (CNMSDec32)( maxSize[ i ] ), CNMS_A_UNIT_PIXEL, unit, lpW1Comp->base_res, outRes );
		gtk_spin_button_set_adjustment( spin_button, adjust_ment );
	}
EXIT_NO_ERR:
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "\t[SubSetInSizeLowerUpper(unit:%d, outRes:%d, decFig:%d)]=%d.\n", unit, outRes, decFig, ret );
#endif
	return	ret;
}

static CNMSInt32 ShowOutSize(
		CNMSInt32		unit,
		CNMSInt32		outRes,
		CNMSInt32		*lpScale,
		CNMSInt32		decFig,
		CNMSDec32		step )
{
	CNMSInt32		ret = CNMS_ERR, i, ldata;
	GtkSpinButton	*spin_button;
	GtkAdjustment	*adjust_ment;

	/*** output ***/
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( ( spin_button = (GtkSpinButton *)GetWidgetW1( outObjArray[ i ] ) ) == CNMSNULL ){
			DBGMSG( "[ShowOutSize]Can't get widget string(outputSize:%d ).\n", i );
			goto	EXIT;
		}
		else if( ( adjust_ment = gtk_spin_button_get_adjustment( spin_button ) ) == CNMSNULL ){
			DBGMSG( "[ShowOutSize]Can't get adjustment from spinbutton of outputSize(%d).\n", i );
			goto	EXIT;
		}
		if( lpW1Comp->group == CNMSUI_SIZE_GROUP_DISPLAY ){
			/* set value */
			adjust_ment->value = (gdouble)CnmsConvUnit( (CNMSDec32)( lpW1Comp->outSize[ i ] ), CNMS_A_UNIT_PIXEL, CNMS_A_UNIT_PIXEL, lpW1Comp->base_res, lpW1Comp->size_res );
			if( unit != CNMS_A_UNIT_PIXEL ){
				adjust_ment->value = (gdouble)CnmsConvUnit( (CNMSDec32)( adjust_ment->value ), CNMS_A_UNIT_PIXEL, unit, outRes, outRes );
			}
		}
		else{
			/* lower */
			ldata = (CNMSInt32)( (CNMSDec32)( lpW1Comp->inSize[ i ] ) * (CNMSDec32)( lpScale[ 0 ] ) / 100 );
			adjust_ment->lower = (gdouble)CnmsConvUnit( (CNMSDec32)ldata, CNMS_A_UNIT_PIXEL, unit, lpW1Comp->base_res, outRes );
			/* upper */
			ldata = (CNMSInt32)( (CNMSDec32)( lpW1Comp->inSize[ i ] ) * (CNMSDec32)( lpScale[ 1 ] ) / 100 );
			adjust_ment->upper = (gdouble)CnmsConvUnit( (CNMSDec32)ldata, CNMS_A_UNIT_PIXEL, unit, lpW1Comp->base_res, outRes );
			/* set value */
			adjust_ment->value = (gdouble)CnmsConvUnit( (CNMSDec32)( lpW1Comp->outSize[ i ] ), CNMS_A_UNIT_PIXEL, unit, lpW1Comp->base_res, outRes );
		}
		adjust_ment->step_increment = (gdouble)step;
		adjust_ment->page_increment = (gdouble)step * W1UI_PAGE_MUL;
		gtk_spin_button_set_adjustment( spin_button, adjust_ment );
		gtk_spin_button_set_digits( spin_button, decFig );
		gtk_spin_button_set_value( spin_button, adjust_ment->value );
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "\t[ShowOutSize(%d)]lower:%f, upper:%f, value:%f.\n", i, adjust_ment->lower, adjust_ment->upper, adjust_ment->value );
#endif
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "\t[ShowOutSize(unit:%d, outRes:%d, minScale:%d, maxScale:%d)]=%d.\n", unit, outRes, lpScale[ 0 ], lpScale[ 1 ], ret );
#endif

	return	ret;
}

static CNMSInt32 ShowDataSize( CNMSVoid )
{
	CNMSByte		buf[ 64 ];
	CNMSInt32		ret = CNMS_ERR, ldata;
	GtkWidget		*widget;

	if( ( lpW1Comp == CNMSNULL ) || ( main_window == CNMSNULL ) ){
		DBGMSG( "[ShowDataSize]Initialize parameter is error.\n" );
		goto	EXIT;
	}
	/*** Show data size ***/
	else if( ( ldata = W1Size_GetEstimatedSizeStr( buf, sizeof( buf ) ) ) != CNMS_NO_ERR ){
		DBGMSG( "[ShowDataSize]Error is occured in W1Size_GetEstimatedSizeStr.\n" );
		goto	EXIT;
	}
	else if( ( widget = GetWidgetW1( CNMS_OBJ_A_DATA_SIZE ) ) == CNMSNULL ){
		DBGMSG( "[ShowDataSize]Can't get widget( CNMS_OBJ_A_DATA_SIZE ).\n" );
		goto	EXIT;
	}
	gtk_entry_set_text( (GtkEntry *)widget, buf );

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "\t[ShowDataSize()]=%d.\n", ret );
#endif
	return	ret;
}

/*** Aspect ***/
/* call after set new crop area */
CNMSInt32 W1Ui_AspectInfoSet( CNMSInt32 aspect )
{
	CNMSInt32		ret = CNMS_ERR, ldata;

	/* check */
	if( lpW1Comp == CNMSNULL ){
		DBGMSG( "[W1Ui_AspectInfoSet]Initialize parameter is error.\n" );
		goto	EXIT;
	}
	else if( ( ldata = CnmsUiAspectSet( aspect, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1Ui_AspectInfoSet]Error is occured in CnmsUiAspectSet.\n" );
		goto	EXIT;
	}
	else if( ( ldata = W1Ui_AspectReflect() ) != CNMS_NO_ERR ){
		DBGMSG( "[W1Ui_AspectInfoSet]Error is occured in W1Ui_AspectReflect.\n" );
		goto	EXIT;
	}
EXIT_NO_ERR:
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1Ui_AspectInfoSet(aspect:%d)]=%d.\n", aspect, ret );
#endif
	return	ret;
}

/* call after link table and rect area changed */
CNMSInt32 W1Ui_AspectReflect( CNMSVoid )
{
	CNMSInt32		ret = CNMS_ERR, ldata, aspect, unit, outRes, decFig;
	CNMSInt32		rectSize[ CNMS_DIM_MAX ];

	/* check */
	if( lpW1Comp == CNMSNULL ){
		DBGMSG( "[W1Ui_AspectReflect]Initialize parameter is error.\n" );
		goto	EXIT;
	}
	/* get info */
	else if( ( aspect = CnmsUiAspectGet( lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[W1Ui_AspectReflect]Error is occured in CnmsUiAspectGet.\n" );
		goto	EXIT;
	}
	else if( ( unit = CnmsUiGetValue( CNMS_OBJ_A_UNIT, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[W1Ui_AspectReflect]Error is occured in CnmsUiGetValue( CNMS_OBJ_A_UNIT ).\n" );
		goto	EXIT;
	}
	else if( ( outRes = CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) <= 0 ){
		DBGMSG( "[W1Ui_AspectReflect]Error is occured in CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION ).\n" );
		goto	EXIT;
	}
	else if( ( decFig = CnmsGetDecFig( unit ) ) < 0 ){
		DBGMSG( "[W1Ui_AspectReflect]Error is occured in CnmsGetDecFig.\n" );
		goto	EXIT;
	}
	else if( ( ldata = Preview_RectGet( rectSize ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1Ui_AspectReflect]Error is occured in Preview_RectGet.\n" );
		goto	EXIT;
	}
	/* set toggle button */
	if( ( ldata = SubAspectToggleSet( aspect, rectSize[ CNMS_DIM_H ] ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1Ui_AspectReflect]Error is occured in SubAspectToggleSet.\n" );
		goto	EXIT;
	}
	/* set adjustment */
	else if( ( ldata = SubSetInSizeLowerUpper( unit, outRes, decFig ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1Ui_AspectReflect]Error is occured in SubSetInSizeLowerUpper.\n" );
		goto	EXIT;
	}
	Preview_AspectSet( aspect );
EXIT_NO_ERR:
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1Ui_AspectReflect()]=%d.\n", ret );
#endif
	return	ret;
}

static CNMSInt32 SubAspectToggleSet(
		CNMSInt32		aspect,
		CNMSInt32		rectSizeH )
{
	CNMSInt32		ret = CNMS_ERR;
	GtkWidget		*widget;

	if( ( widget = GetWidgetW1( CNMS_OBJ_A_RATIO_FIX ) ) == CNMSNULL ){
		DBGMSG( "[SubAspectToggleSet]Can't get widget( CNMS_OBJ_A_RATIO_FIX ).\n" );
		goto	EXIT;
	}

	if( GTK_WIDGET_VISIBLE( widget ) == 0 ){
		gtk_widget_show( widget );
	}

	switch( aspect ){
		case	CNMS_ASPECT_VARIED:
			/* toggle off */
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), CNMS_FALSE );
			if( rectSizeH == 0 ){
				/* No rect : not sensitive */
				if( GTK_WIDGET_SENSITIVE( widget ) != 0 ){
					gtk_widget_set_sensitive( widget, FALSE );
				}
			}
			else{
				/* Rect : sensitive */
				if( GTK_WIDGET_SENSITIVE( widget ) == 0 ){
					gtk_widget_set_sensitive( widget, TRUE );
				}
			}
			break;
		
		case	CNMS_ASPECT_FIXED:
			/* toggle on */
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), CNMS_TRUE );
			/* sensitive */
			if( GTK_WIDGET_SENSITIVE( widget ) == 0 ){
				gtk_widget_set_sensitive( widget, TRUE );
			}
			break;
		
		case	CNMS_ASPECT_STANDERD:
			/* Fixed size */
			/* toggle on */
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), CNMS_TRUE );
			/* not sensitive */
			if( GTK_WIDGET_SENSITIVE( widget ) != 0 ){
				gtk_widget_set_sensitive( widget, FALSE );
			}
			break;
	}
EXIT_NO_ERR:
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI_DETAIL__
	DBGMSG( "\t[SubAspectToggleSet(aspect:%d, rectSizeH:%d)]=%d.\n", aspect, rectSizeH, ret );
#endif
	return	ret;
}

CNMSInt32 W1Ui_ChangeClearPrevButtonVisible( CNMSVoid )
{
	CNMSInt32	ret = CNMS_ERR;
	GtkWidget	*widget;

	if( ( lpW1Comp == CNMSNULL ) || ( main_window == CNMSNULL ) ){
		DBGMSG( "[W1Ui_ChangeClearPrevButtonVisible]Initialize parameter is error.\n" );
		goto	EXIT;
	}
	else if( ( widget = lookup_widget( main_window, "toolbar_clear_preview_button" ) ) == CNMSNULL ){
		DBGMSG( "[W1Ui_ChangeClearPrevButtonVisible]Can't look up clear preview button.\n" );
		goto	EXIT;
	}

	if( lpW1Comp->previewFlag == CNMS_TRUE ){
		if( GTK_WIDGET_SENSITIVE( widget ) == 0 ){
			gtk_widget_set_sensitive( widget, TRUE );
			gtk_widget_hide( widget );
			gtk_widget_show( widget );
		}
		if( GTK_WIDGET_VISIBLE( widget ) == 0 ){
			gtk_widget_show( widget );
		}
	}
	else{
		if( GTK_WIDGET_SENSITIVE( widget ) != 0 ){
			gtk_widget_set_sensitive( widget, FALSE );
		}
		if( GTK_WIDGET_VISIBLE( widget ) == 0 ){
			gtk_widget_show( widget );
		}
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1Ui_ChangeClearPrevButtonVisible()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSInt32 W1Ui_SetComboDefVal(
		GtkWidget			*combobox,
		CNMSInt32			obj )
{
	CNMSInt32			ret = CNMS_ERR,
						i,
						defCnt = CNMS_ERR,
						count = 0,
						linkNum = 0;
	CNMSLPSTR			lpStr;
	LPCNMSUILINKCOMP	lpLink = lpW1Comp->lpLink,
						lpLinkTop = CNMSNULL;

	if( ( lpLink == CNMSNULL ) || ( combobox == CNMSNULL ) ){
		DBGMSG( "[CnmsSetComboDefVal]Parameter is error.\n" );
		goto	EXIT;
	}

	for( i = 0 ; i < lpW1Comp->linkNum ; i++, lpLink++ ){
		if( lpLink->object == obj ){
			if( lpLinkTop == CNMSNULL ){
				lpLinkTop = lpLink;
			}
			linkNum++;
		}
	}

	lpLink = lpLinkTop;
	ComGtk_ClearCombo( GTK_COMBO_BOX( combobox ) );
	for( i = 0 ; i < linkNum ; i ++, lpLink ++ ){
		if( ( lpLink->mode == CNMS_MODE_ADD ) || ( lpLink->mode == CNMS_MODE_SELECT ) || ( lpLink->mode == CNMS_MODE_HIDE ) ){
			if( ( lpStr = CnmsChgValToStr( lpLink->value, obj ) ) == CNMSNULL ){
				DBGMSG( "[CnmsSetComboDefVal]Can't get string( value = %d ).\n", lpLink->value );
				break;
			}
			if( ( lpLink->mode == CNMS_MODE_SELECT ) || ( lpLink->mode == CNMS_MODE_HIDE ) ){
				defCnt = count;
			}
			gtk_combo_box_append_text( GTK_COMBO_BOX( combobox ), lpStr );
			count ++;
		}
	}
	if( defCnt != CNMS_ERR ){
		gtk_combo_box_set_active( GTK_COMBO_BOX( combobox ), defCnt );
	}

	ret = CNMS_NO_ERR;
EXIT:
	return	ret;
}

CNMSInt32 W1Ui_SetComboSelectedVal(
		GtkWidget			*combobox,
		CNMSInt32			obj,
		CNMSLPSTR			str )
{
	CNMSInt32			ret = CNMS_ERR,
						i,
						defCnt = CNMS_ERR,
						count = 0,
						linkNum = 0;
	CNMSLPSTR			lpStr;
	LPCNMSUILINKCOMP	lpLink = lpW1Comp->lpLink,
						lpLinkTop = CNMSNULL;

	if( ( lpLink == CNMSNULL ) || ( combobox == CNMSNULL ) ){
		DBGMSG( "[CnmsSetComboDefVal]Parameter is error.\n" );
		goto	EXIT;
	}

	for( i = 0 ; i < lpW1Comp->linkNum ; i++, lpLink++ ){
		if( lpLink->object == obj ){
			if( lpLinkTop == CNMSNULL ){
				lpLinkTop = lpLink;
			}
			linkNum++;
		}
	}

	lpLink = lpLinkTop;
	ComGtk_ClearCombo( GTK_COMBO_BOX( combobox ) );
	for( i = 0 ; i < linkNum ; i ++, lpLink ++ ){
		if( ( lpLink->mode == CNMS_MODE_ADD ) || ( lpLink->mode == CNMS_MODE_SELECT ) || ( lpLink->mode == CNMS_MODE_HIDE ) ){
			if( ( lpStr = CnmsChgValToStr( lpLink->value, obj ) ) == CNMSNULL ){
				DBGMSG( "[CnmsSetComboDefVal]Can't get string( value = %d ).\n", lpLink->value );
				break;
			}
			if( CnmsStrCompare( str, lpStr ) == CNMS_NO_ERR ) {
				defCnt = count;
			}
			gtk_combo_box_append_text( GTK_COMBO_BOX( combobox ), lpStr );
			count ++;
		}
	}
	if( defCnt != CNMS_ERR ){
		gtk_combo_box_set_active( GTK_COMBO_BOX( combobox ), defCnt );
	}

	ret = CNMS_NO_ERR;
EXIT:
	return	ret;
}

CNMSInt32 W1Ui_InitUIFormat( CNMSVoid )
{
	CNMSInt32			ret			= CNMS_ERR,
						usiCnt,
						wnCnt;
	LPCNMSUILINKCOMP	lplinktop	= CNMSNULL;
	GtkWidget			*widget		= CNMSNULL;

	/* User Interface Initialize */
	if( W1_JudgeFormatType( CNMS_OBJ_UI ) != CNMS_TRUE ){
		usiCnt = 0;
		while( uiSetInfo[usiCnt].object != CNMS_ERR ){
			for( wnCnt = 0 ; wnCnt < W1UI_WIDGETNAME_MAX ; wnCnt++ ){
				if( uiSetInfo[usiCnt].widgetname[wnCnt] != CNMSNULL ){
					if( ( widget = lookup_widget( main_window, uiSetInfo[usiCnt].widgetname[wnCnt] ) ) == CNMSNULL ){
						goto EXIT;
					}
					switch( uiSetInfo[usiCnt].def ){
						case CNMS_MODE_SELECT:
						case CNMS_MODE_ADD:
								gtk_widget_show( widget );
								gtk_widget_set_sensitive( widget, CNMS_TRUE );	break;

						case CNMS_MODE_DELETE:
								gtk_widget_hide( widget );						break;

						case CNMS_MODE_HIDE:
								gtk_widget_set_sensitive( widget, CNMS_FALSE );	break;

						default: 												break;
					}
				}
			}
			usiCnt++;
		}
	}


EXIT_NO_ERR:
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1Ui_InitUIFormat()]=%d\n", ret );
#endif
	return	ret;
}

CNMSInt32 W1Ui_SwitchPlatenAndADF(
		CNMSInt32		object,
		CNMSInt32		currentGp,
		CNMSInt32		changedGp				)
{
	CNMSInt32	ret	= CNMS_ERR;

	if( ( ( object != CNMS_OBJ_S_SOURCE )&&( object != CNMS_OBJ_A_SOURCE ) ) || ( currentGp == changedGp ) ){
		goto EXIT_NO_ERR;
	}

	/* Reset Crop Area */
	W1_SwitchPlatenAndADFReset( PREVIEW_RESET_RECT );
	if( ( ( object == CNMS_OBJ_S_SOURCE ) && ( changedGp == CNMS_S_SOURCE_TEXT_COL_SADF ) ) || ( ( object == CNMS_OBJ_A_SOURCE ) && ( changedGp == CNMS_A_SOURCE_TEXT_SADF ) ) ){
		dialog_clear_preview_ok_clicked();
	}

EXIT_NO_ERR:
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1Ui_SwitchADFandPlaten()]=%d\n", ret );
#endif
	return	ret;
}

CNMSInt32 W1Ui_SwitchSelectModelFromDevnum(
		CNMSInt32		devnum				)
{
	CNMSInt32	ret	= CNMS_ERR;
	GtkWidget	*combo = NULL, *label = NULL, *okbutton = NULL;

	if( ( combo = lookup_widget( select_model_dialog, "select_model_combo" ) ) == CNMSNULL ){
		goto	EXIT;
	}
	if( ( label = lookup_widget( select_model_dialog, "select_model_label" ) ) == CNMSNULL ){
		goto	EXIT;
	}
	if( ( okbutton = lookup_widget( select_model_dialog, "select_model_ok_button" ) ) == CNMSNULL ){
		goto	EXIT;
	}

	if( devnum == 0 ){
		gtk_widget_hide( combo );
		gtk_widget_show( label );
		gtk_widget_set_sensitive( okbutton, CNMS_FALSE );
		ChildDialogShow( CHILDDIALOG_ID_NOT_FOUND, select_model_dialog );
	}
	else{
		gtk_widget_show( combo );
		gtk_widget_hide( label );
		gtk_widget_set_sensitive( okbutton, CNMS_TRUE );
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1Ui_SwitchADFandPlaten()]=%d\n", ret );
#endif
	return	ret;
}

#ifdef	__CNMS_DEBUG__
CNMSVoid DebugW1Comp( CNMSVoid )
{
	CNMSLPSTR	unitStr[ CNMS_A_UNIT_MAX ] = { "pixel\0", "inch \0", "cm   \0", "mm   \0" };

	if( lpW1Comp == CNMSNULL ){
		DBGMSG( "[DebugW1Comp]lpW1Comp is null pointer.\n" );
		goto	EXIT;
	}
	DBGMSG( "[DebugW1Comp]previewFlag : %d\n", lpW1Comp->previewFlag );
	DBGMSG( "[DebugW1Comp]group       : %d\n", lpW1Comp->group );
	DBGMSG( "[DebugW1Comp]env_unit    : %s\n", unitStr[ lpW1Comp->env_unit ] );
	DBGMSG( "[DebugW1Comp]base_res    : %d\n", lpW1Comp->base_res );
	DBGMSG( "[DebugW1Comp]size_res    : %d\n", lpW1Comp->size_res );
	DBGMSG( "[DebugW1Comp]crop_res    : %d\n", lpW1Comp->crop_res );
	DBGMSG( "[DebugW1Comp]scale       : %d\n", lpW1Comp->scale );
	DBGMSG( "[DebugW1Comp]offset      : %d x %d\n", lpW1Comp->offset[ CNMS_DIM_H ], lpW1Comp->offset[ CNMS_DIM_V ] );
	DBGMSG( "[DebugW1Comp]inSize      : %d x %d\n", lpW1Comp->inSize[ CNMS_DIM_H ], lpW1Comp->inSize[ CNMS_DIM_V ] );
	DBGMSG( "[DebugW1Comp]outSize     : %d x %d\n", lpW1Comp->outSize[ CNMS_DIM_H ], lpW1Comp->outSize[ CNMS_DIM_V ] );
EXIT:
	return;
}

static CNMSVoid DebugAdjustment(
		CNMSLPSTR		lpStr,
		GtkAdjustment	*adjust_ment )
{
	DBGMSG( "[DebugAdjustment(%s)]lower         :%f\n", lpStr, adjust_ment->lower );
	DBGMSG( "[DebugAdjustment(%s)]upper         :%f\n", lpStr, adjust_ment->upper );
	DBGMSG( "[DebugAdjustment(%s)]value         :%f\n", lpStr, adjust_ment->value );
	DBGMSG( "[DebugAdjustment(%s)]step_increment:%f\n", lpStr, adjust_ment->step_increment );
	DBGMSG( "[DebugAdjustment(%s)]page_increment:%f\n\n", lpStr, adjust_ment->page_increment );

	return;
}
#endif	/* __CNMS_DEBUG__ */

#endif	/* _W1_UI_C_ */

