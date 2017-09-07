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

#ifndef	_COLORADJUST_HG_C_
#define	_COLORADJUST_HG_C_

#include "coloradjust_hg.h"
#include "scanflow.h"
#include "cnmsdef.h"
#include "com_gtk.h"

#define		HG_DEF_SHADOW			(  0)		/* Default Value for Shadow							*/
#define		HG_DEF_MID				(128)		/* Default Value for Mid							*/
#define		HG_DEF_HIGHLIGHT		(255)		/* Default Value for Highlight						*/

#define		HG_SCALE_AREA_OFFSET	( 20)		/* Offset position for triangle scale area			*/
#define		HG_SCALE_AREA_WIDTH		(223)		/* Width for triangle scale area					*/
#define		HG_SCALE_TRI_TOP		(  0)		/* Top position for triangle scale icon				*/
#define		HG_SCALE_TRI_HEIGHT		( 10)		/* Height size for triangle scale icon				*/

#define		HG_GC_TRIICON_FRAME		(0x0000)	/*   0 GC color Triangle icon OutFrame				*/
#define		HG_GC_TRIICON_BG_R		(0xEFEF)	/* 239 GC color Triangle icon BackGround Red		*/
#define		HG_GC_TRIICON_BG_G		(0xEBEB)	/* 235 GC color Triangle icon BackGround Green		*/
#define		HG_GC_TRIICON_BG_B		(0xE7E7)	/* 231 GC color Triangle icon BackGround Blue		*/
#define		HG_GC_TRIICON_SHADOW	(0x0000)	/*   0 GC color Triangle icon Shadow				*/
#define		HG_GC_TRIICON_MID		(0xC0C0)	/* 192 GC color Triangle icon Mid					*/
#define		HG_GC_TRIICON_HIGH		(0xFFFF)	/* 255 GC color Triangle icon Highlight				*/

typedef struct{
		CNMSInt32		center;
		CNMSInt32		left;
		CNMSInt32		top;
		CNMSInt32		right;
		CNMSInt32		bottom;
}CNMSHGTriIconRect;

typedef struct{
		CNMSInt32			cVal[CNMS_CA_MAX];			/* Color Value on Cursor Position					*/
		CNMSInt32			grab;						/* For Keep Current Grab Triangle Icon				*/
		CNMSInt32			btnType;					/* For Keep Current Toggle Spite Button				*/
}CNMSHGInfo;

	static	CNMSHGInfo			hg_info;
	static	CNMSInt32			hg_flag_spinbutton				= CNMS_FALSE;				/* Flag for Value Set Signal Timing					*/
				
	static	GtkAdjustment		*hg_adjust_shadow				= CNMSNULL,					/* SpinButton Adjustment for Shadow					*/
								*hg_adjust_mid					= CNMSNULL,					/* SpinButton Adjustment for Mid					*/
								*hg_adjust_highlight			= CNMSNULL;					/* SpinButton Adjustment for Highlight				*/
	static	GtkToggleButton		*hg_toggle						= CNMSNULL;					/* Selecting Toggle Button							*/
	static	GdkPixmap			*hg_pmap						= CNMSNULL;					/* Pixmap For Triangle Scale Drawing Area			*/
	static	GdkGC				*hg_gc							= CNMSNULL;					/* Graphic Context For Triangle Scale Drawing Area	*/

	static	CNMSHGTriIconRect	hg_area_shadow					= { 0, 0, 0, 0, 0 };		/* Triangle Icon Information for Shadow				*/
	static	CNMSHGTriIconRect	hg_area_mid						= { 0, 0, 0, 0, 0 };		/* Triangle Icon Information for Mid				*/
	static	CNMSHGTriIconRect	hg_area_highlight				= { 0, 0, 0, 0, 0 };		/* Triangle Icon Information for Highlight			*/

static CNMSInt32 CnmsHGSpuitButtonStatusChange(
						CNMSLPSTR			buttonStr				);
						
static CNMSVoid CnmsHGDrawTriangle(
						CNMSHGTriIconRect	*icon,
						CNMSInt32			center					);

static CNMSVoid CnmsHGDrawHGandFRToneCurve(
						CNMSVoid									);

static CNMSVoid CnmsHGDrawOnlyScaleBar(
						CNMSVoid									);

static CNMSVoid CnmsHGDispSpinButtonValue(
						CNMSVoid									);

static CNMSVoid CnmsHGCalcurateParameter(
						CNMSHGValue			*current,
						CNMSInt32			grab,
						CNMSInt32			value					);

static CNMSVoid CnmsHGCalcurateValue(
						CNMSInt32			x,
						CNMSInt32			grab					);


/***   Histogram Dialog Show   ***/
CNMSVoid CnmsHGDialogShow(
						CNMSInt32			color,
						CNMSInt32			keep					)
{
	/* Channel ComboBox Item Change depending on ColorGroup */
	if( ( CnmsColAdjChannelItemSet( color, CNMS_CA_DIALOG_HG ) ) == CNMS_FALSE )	return;
	switch( color ){
		case CNMS_CA_COLORGROUP_COLOR:
				gtk_label_set_text( GTK_LABEL( hg_label_str_1 ), gettext(ca_label_red) );
				gtk_label_set_text( GTK_LABEL( hg_label_str_2 ), gettext(ca_label_green) );
				gtk_label_set_text( GTK_LABEL( hg_label_str_3 ), gettext(ca_label_blue) );
				CnmsHGPreviewRGBKValueUnDisp();
				break;
		case CNMS_CA_COLORGROUP_GRAY:
				gtk_label_set_text( GTK_LABEL( hg_label_str_1 ), gettext(ca_label_gray) );
				gtk_label_set_text( GTK_LABEL( hg_label_str_2 ), ca_label_null );
				gtk_label_set_text( GTK_LABEL( hg_label_str_3 ), ca_label_null );
				CnmsHGPreviewRGBKValueUnDisp();
				break;
		default:
				break;
	}
	
	/* Show/UnShow Change */
	if( keep == CNMS_CA_DIALOG_NON_KEEP ){
		CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_HG, !( GTK_WIDGET_VISIBLE( hg_dlg.widget ) ) );
		CnmsHGPreviewChangedAfter();
	}
	CnmsColAdjSetChannelItem( &hg_dlg );
	

	return;
}


/***   Data Initialize   ***/
CNMSVoid CnmsHGInit(
						CNMSVoid									)
{
	CNMSInt32	i = 0;
	
	/* Initialize to Color Value and Max/Min Color Value */
	for( i = 0 ; i < CNMS_CA_MAX ; i++ ){
		hg[i].shadow.raw	= hg[i].shadow.disp		= HG_DEF_SHADOW;
		hg[i].mid.raw		= hg[i].mid.disp		= HG_DEF_MID;
		hg[i].highlight.raw	= hg[i].highlight.disp	= HG_DEF_HIGHLIGHT;

		hg[i].shadow.lower		= HG_DEF_SHADOW;
		hg[i].shadow.upper		= HG_DEF_HIGHLIGHT - 10;
		hg[i].mid.lower			= HG_DEF_SHADOW + 5;
		hg[i].mid.upper			= HG_DEF_HIGHLIGHT - 5;
		hg[i].highlight.lower	= HG_DEF_SHADOW + 10;
		hg[i].highlight.upper	= HG_DEF_HIGHLIGHT;

		hg_info.cVal[i] = 0;
	}
	hg_info.grab	= CNMS_HG_GRAB_NO_GRAB;
	hg_info.btnType	= CNMS_HG_SPUIT_ERR;

	/* Initialize to Parameter Value		*/
	ca_param_all.blackpoint.m	= ca_param_all.blackpoint.r	= ca_param_all.blackpoint.g	= ca_param_all.blackpoint.b	= HG_DEF_SHADOW;
	ca_param_all.midpoint.m		= ca_param_all.midpoint.r	= ca_param_all.midpoint.g	= ca_param_all.midpoint.b	= HG_DEF_MID;
	ca_param_all.whitepoint.m	= ca_param_all.whitepoint.r	= ca_param_all.whitepoint.g	= ca_param_all.whitepoint.b	= HG_DEF_HIGHLIGHT;
	ca_param_all.graypoint.m	= ca_param_all.graypoint.r	= ca_param_all.graypoint.g	= ca_param_all.graypoint.b	= HG_DEF_MID;
	
	/* Get SpinButton Adjustment			*/
	if( ( hg_adjust_shadow == CNMSNULL ) || ( hg_adjust_mid == CNMSNULL ) || ( hg_adjust_highlight == CNMSNULL ) ){
		hg_adjust_shadow = gtk_spin_button_get_adjustment( GTK_SPIN_BUTTON( hg_entry_shadow ) );
		hg_adjust_mid = gtk_spin_button_get_adjustment( GTK_SPIN_BUTTON( hg_entry_mid ) );
		hg_adjust_highlight = gtk_spin_button_get_adjustment( GTK_SPIN_BUTTON( hg_entry_highlight ) );
	}

	/* Initialize to ToneCurve Value		*/
	CnmsColAdjInitCurveAndHistogram( &hg_dlg );
	CnmsHGPreviewChangedAfter();
	return;
}


/***   Reset   ***/
CNMSVoid CnmsHGReset(
						CNMSVoid									)
{
	/* Data Initialize						*/
	CnmsHGInit();
	CnmsHGDispSpinButtonValue();
	CnmsHGDrawOnlyScaleBar();

	/* Channel Set ComboBox Top				*/
	if( hg_dlg.channel != CNMS_CA_GRAY ){
		hg_dlg.channel = CNMS_CA_MASTER;
	}
	if( GTK_WIDGET_VISIBLE( hg_dlg.widget ) != 0 ){
		CnmsColAdjSetChannelItem( &hg_dlg );
		CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_HG );
	}

	return;
}


/***   Call after Preview Image Changed   ***/
CNMSInt32 CnmsHGPreviewChangedAfter(
						CNMSVoid									)
{
	CNMSInt32		ldata			= CNMS_ERR;

	if( ( ( ldata = CnmsHGSpuitButtonStatusChange( "hg_togglebutton_shadow" ) ) != CNMS_NO_ERR ) ||
		( ( ldata = CnmsHGSpuitButtonStatusChange( "hg_togglebutton_mid" ) ) != CNMS_NO_ERR ) ||
		( ( ldata = CnmsHGSpuitButtonStatusChange( "hg_togglebutton_highlight" ) ) != CNMS_NO_ERR ) ||
		( ( ldata = CnmsHGSpuitButtonStatusChange( "hg_togglebutton_graybalance" ) ) != CNMS_NO_ERR ) )
	{
		goto	EXIT;
	}
	
EXIT:
	return	ldata;
}


/***   Channel ComboBox Changed   ***/
CNMSVoid CnmsHGChannelComboBoxChanged(
						GtkComboBox			*combobox				)
{
	CNMSInt8	*channel	= CNMSNULL;

	/* If flag is True, Called by Combobox Clear func. The case, No action the process  */
	if( ca_info.clearcbox == CNMS_TRUE ){
		return;
	}

	/* Get Current Channel ID from Combobox String */ 
	if( ( hg_dlg.channel = CnmsColAdjGetChannelIDFromCombobox( combobox ) ) == CNMS_ERR ){
		DBGMSG( "[CnmsHGChannelComboBoxChanged] value[hg_dlg.channel] is invalid.\n" );
		return;
	}

	/* Set Data			*/
	CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_HG );
	CnmsHGDispSpinButtonValue();
	CnmsHGDrawOnlyScaleBar();

	return;
}


/***   Configure for Draw Triangle Icon   ***/
CNMSVoid CnmsHGScaleBarConfigureEvent(
						GtkWidget			*widget,
						GdkEventConfigure	*event					)
{
	if( ( widget == CNMSNULL ) || ( event == CNMSNULL ) ){
		DBGMSG( "[CnmsHGScaleBarConfigureEvent] Arg Pointer is NULL. widget[%p], event[%p]\n", widget, event );
		return;
	}

	/* Create Pixmap			*/
	if ( hg_pmap != CNMSNULL ){
		gdk_pixmap_unref( hg_pmap );
	}
	hg_pmap = gdk_pixmap_new( widget->window, widget->allocation.width, widget->allocation.height, -1 );

	/* Create Graphic Context	*/
	if( hg_gc == CNMSNULL ){
		hg_gc = gdk_gc_new( widget->window );
	}

	CnmsHGDrawOnlyScaleBar();

	return;
}


/***   Expose for Draw Triangle Icon   ***/
CNMSVoid CnmsHGScaleBarExposeEvent(
						GtkWidget			*widget,
						GdkEventExpose		*event					)
{
	if( ( widget == CNMSNULL ) || ( event == CNMSNULL ) ){
		DBGMSG( "[CnmsHGScaleBarExposeEvent] Arg Pointer is NULL. widget[%p], event[%p]\n", widget, event );
		return;
	}

	gdk_draw_pixmap( widget->window, widget->style->fg_gc[GTK_WIDGET_STATE (widget)], hg_pmap,
		event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height );
		
	return;
}


/***   Mouse clicked on triangle icon area   ***/
CNMSVoid CnmsHGScaleBarButtonPressEvent(
						GdkEventButton		*event					)
{
	CNMSInt32	x = event->x,
				y = event->y;

	if( event == CNMSNULL ){
		DBGMSG( "[CnmsHGScaleBarButtonPressEvent] Arg Pointer is NULL. event[%p]\n", event );
		return;
	}

	/* Judge and Set Flag depending on Grabing Icon	*/
	if( event->button == 1 ){
		if( ( x >= hg_area_highlight.left ) && ( x <= hg_area_highlight.right ) && ( y >= hg_area_highlight.top ) && ( y <= hg_area_highlight.bottom ) ){
			hg_info.grab = CNMS_HG_GRAB_HIGHLIGHT;
		}
		else if( ( x >= hg_area_mid.left ) && ( x <= hg_area_mid.right ) && ( y >= hg_area_mid.top ) && ( y <= hg_area_mid.bottom ) ){
			hg_info.grab = CNMS_HG_GRAB_MID;
		}
		else if( ( x >= hg_area_shadow.left ) && ( x <= hg_area_shadow.right ) && ( y >= hg_area_shadow.top ) && ( y <= hg_area_shadow.bottom ) ){
			hg_info.grab = CNMS_HG_GRAB_SHADOW;
		}
	}

	return;
}


/***   Mouse released on triangle icon area   ***/
CNMSVoid CnmsHGScaleBarButtonReleaseEvent(
						GdkEventButton		*event					)
{
	if( event == CNMSNULL ){
		DBGMSG( "[CnmsHGScaleBarButtonReleaseEvent] Arg Pointer is NULL. event[%p]\n", event );
		return;
	}

	/* Reset hg_current_grab just on Button released	*/
	if( event->button == 1 ){
		hg_info.grab = CNMS_HG_GRAB_NO_GRAB;
	}

	return;
}


/***   Moving cursor on triangle icon area   ***/
CNMSVoid CnmsHGScaleBarMotionNotifyEvent(
						GdkEventMotion		*event					)
{
	static	CNMSInt				xBase		= 0,
								yBase		= 0,
								xFix		= 0;
			GdkModifierType		state;

	if( event == CNMSNULL ){
		DBGMSG( "[CnmsHGScaleBarMotionNotifyEvent] Arg Pointer is NULL. event[%p]\n", event );
		return;
	}

	if ( event->is_hint ){
		gdk_window_get_pointer ( event->window, &xBase, &yBase, &state );
	}
	else{
		xBase = event->x;
		yBase = event->y;
		state = event->state;
	}

	if ( ( state & GDK_BUTTON1_MASK ) && ( hg_info.grab != CNMS_HG_GRAB_ERR ) && ( hg_info.grab != CNMS_HG_GRAB_NO_GRAB ) )
	{
		/* Fix to 255 from Base, Calcurate Param, Set value to SpinButton, Draw Triangle Icon, and Draw ToneCurve */
		xFix = ( CNMSInt )( ( ( CNMSDec32 )( ( xBase - HG_SCALE_AREA_OFFSET ) * 255 ) ) / HG_SCALE_AREA_WIDTH );
		CnmsHGCalcurateValue( xFix, hg_info.grab );
		CnmsHGDispSpinButtonValue();
		CnmsHGDrawOnlyScaleBar();
		CnmsHGDrawHGandFRToneCurve();
	}
	
	return;
}


/***   Valid to SpinButton Value Changed Direct   ***/
CNMSVoid CnmsHGSpinButtonValueChanged(
						GtkSpinButton		*spinbutton,
						CNMSInt32			grab					)
{
	CNMSInt32 			x_value		= 0;							/* Value range is 0 to 255				*/

	if( spinbutton == CNMSNULL ){
		DBGMSG( "[CnmsHGSpinButtonValueChanged] Arg Pointer is NULL. spinbutton[%p]\n", spinbutton );
		return;
	}

	/* Flag for Avoid Infinite Calling Loop	*/
	if( hg_flag_spinbutton == CNMS_FALSE ){
		CnmsColAdjSetZeroIfMinusZeroForSpinbutton( GTK_WIDGET( spinbutton ) );

		x_value = gtk_spin_button_get_value_as_int( spinbutton );	/* Get SpinButton Value					*/
		CnmsHGCalcurateValue( x_value, grab );
		CnmsHGDispSpinButtonValue();
		CnmsHGDrawOnlyScaleBar();
		CnmsHGDrawHGandFRToneCurve();
	}
	
	return;
}


/***   Disp RGBK Value   ***/
CNMSVoid CnmsHGPreviewRGBKValueDisp(
						CNMSInt32			rVal,
						CNMSInt32			gVal,
						CNMSInt32			bVal					)
{
	CNMSInt8		value_1[ CNMS_CA_STRING_LEN ],
					value_2[ CNMS_CA_STRING_LEN ],
					value_3[ CNMS_CA_STRING_LEN ];

	if( ( rVal < 0 ) || ( gVal < 0 ) || ( bVal < 0 ) ){
		hg_info.cVal[CNMS_CA_RED] = hg_info.cVal[CNMS_CA_GREEN] = hg_info.cVal[CNMS_CA_BLUE] = CNMS_ERR;
		CnmsHGPreviewRGBKValueUnDisp();
		DBGMSG( "[CnmsHGPreviewRGBKValueDisp] Value is invalid. rVal[%d], gVal[%d], bVal[%d]\n", rVal, gVal, bVal );
		return;
	}

	hg_info.cVal[CNMS_CA_RED]	= rVal;
	hg_info.cVal[CNMS_CA_GREEN]	= gVal;
	hg_info.cVal[CNMS_CA_BLUE]	= bVal;
	hg_info.cVal[CNMS_CA_GRAY]	= gVal;			/* Case Gray, use gVal	*/

	switch( ca_info.colorgroup ){
		case	CNMS_CA_COLORGROUP_COLOR:
			sprintf( value_1, "%d\0", hg_info.cVal[CNMS_CA_RED] );
			gtk_label_set_text( GTK_LABEL( hg_label_value_1 ), value_1 );
			sprintf( value_2, "%d\0", hg_info.cVal[CNMS_CA_GREEN] );
			gtk_label_set_text( GTK_LABEL( hg_label_value_2 ), value_2 );
			sprintf( value_3, "%d\0", hg_info.cVal[CNMS_CA_BLUE] );
			gtk_label_set_text( GTK_LABEL( hg_label_value_3 ), value_3 );
			break;

		case CNMS_CA_COLORGROUP_GRAY:
			sprintf( value_1, "%d\0", hg_info.cVal[CNMS_CA_GRAY] );
			gtk_label_set_text( GTK_LABEL( hg_label_value_1 ), value_1 );
			break;
		default:
			DBGMSG( "[CnmsHGPreviewRGBKValueDisp] Value is invalid. ca_info.colorgroup[%d]\n", ca_info.colorgroup );
			break;
	}
	DBGMSG( "[CnmsHGPreviewRGBKValueDisp] value_1[%s], value_2[%s], value_3[%s]\n", value_1, value_2, value_3 );

EXIT:
	return;
}


/***   UnDisp RGBK Value   ***/
CNMSVoid CnmsHGPreviewRGBKValueUnDisp(
						CNMSVoid									)
{
	if( ( hg_label_value_1 == CNMSNULL ) || ( hg_label_value_2 == CNMSNULL ) || ( hg_label_value_3 == CNMSNULL ) ){
		DBGMSG( "[CnmsHGPreviewRGBKValueUnDisp] Pointer is NULL. hg_label_value_1[%p], hg_label_value_2[%p], hg_label_value_3[%p]\n",
																	 					hg_label_value_1, hg_label_value_2, hg_label_value_3 );
		return;
	}

	gtk_label_set_text( GTK_LABEL( hg_label_value_1 ), ca_label_null );
	gtk_label_set_text( GTK_LABEL( hg_label_value_2 ), ca_label_null );
	gtk_label_set_text( GTK_LABEL( hg_label_value_3 ), ca_label_null );

	return;
}


/***   Spuit ToggleButton Clicked   ***/
CNMSVoid CnmsHGSpuitButtonToggled(
						GtkToggleButton		*togglebutton,
						CNMSInt32			button_type				)
{
	if( ( togglebutton == CNMSNULL ) || ( CNMS_HG_SPUIT_ERR >= button_type ) || ( button_type >= CNMS_HG_SPUIT_MAX ) ){
		DBGMSG( "[CnmsHGSpuitButtonToggled] Arg is invalid. togglebutton[%p], button_type[%p]\n", togglebutton, button_type );
		goto	EXIT;
	}

	if( gtk_toggle_button_get_active( togglebutton ) == CNMS_TRUE ){
		ComGtk_CursorImageSet( hg_dlg.widget, CNMS_CURSOR_ID_CROSS );
		if( GTK_WIDGET_VISIBLE( bc_dlg.widget ) == CNMS_TRUE ){
			ComGtk_CursorImageSet( bc_dlg.widget, CNMS_CURSOR_ID_CROSS );
		}
		if( GTK_WIDGET_VISIBLE( tc_dlg.widget ) == CNMS_TRUE ){
			ComGtk_CursorImageSet( tc_dlg.widget, CNMS_CURSOR_ID_CROSS );
		}
		if( GTK_WIDGET_VISIBLE( fr_dlg.widget ) == CNMS_TRUE ){
			ComGtk_CursorImageSet( fr_dlg.widget, CNMS_CURSOR_ID_CROSS );
		}

		/* GrayOut to Without Preview Area In Main Window	*/
		gtk_widget_set_sensitive( lookup_widget( main_window, "main_box" ), CNMS_FALSE );
		gtk_widget_set_sensitive( lookup_widget( main_window, "toolbar_frame" ), CNMS_FALSE );

		hg_toggle = togglebutton;
		hg_info.btnType = button_type;
	}
	else{
		hg_toggle = CNMSNULL;
		hg_info.btnType = CNMS_HG_SPUIT_ERR;

		ComGtk_CursorImageSet( hg_dlg.widget, CNMS_CURSOR_ID_NORMAL );
		if( GTK_WIDGET_VISIBLE( bc_dlg.widget ) == CNMS_TRUE ){
			ComGtk_CursorImageSet( bc_dlg.widget, CNMS_CURSOR_ID_NORMAL );
		}
		if( GTK_WIDGET_VISIBLE( tc_dlg.widget ) == CNMS_TRUE ){
			ComGtk_CursorImageSet( tc_dlg.widget, CNMS_CURSOR_ID_NORMAL );
		}
		if( GTK_WIDGET_VISIBLE( fr_dlg.widget ) == CNMS_TRUE ){
			ComGtk_CursorImageSet( fr_dlg.widget, CNMS_CURSOR_ID_NORMAL );
		}

		/* Sensitive to Without Preview Area In Main Window	*/
		gtk_widget_set_sensitive( lookup_widget( main_window, "main_box" ), CNMS_TRUE );
		gtk_widget_set_sensitive( lookup_widget( main_window, "toolbar_frame" ), CNMS_TRUE );
	}

EXIT:
	return;
}


/***   Release SpuitButton Toggle   ***/
CNMSInt32 CnmsHGSpuitButtonToggleRelease(
						CNMSVoid									)
{
	CNMSInt32	ret = CNMS_ERR;

	gtk_toggle_button_set_active( hg_toggle, CNMS_FALSE );
	hg_toggle = CNMSNULL;
	CnmsHGPreviewRGBKValueUnDisp();
	ComGtk_CursorImageSet( hg_dlg.widget, CNMS_CURSOR_ID_NORMAL );
	ret = CNMS_NO_ERR;
EXIT:
	return	ret;
}


/***   Judge and Get Status Toggle/UnToggle   ***/
CNMSInt32 CnmsHGGetToggleStatus(
						CNMSVoid									)
{
	if( hg_toggle == CNMSNULL ){
		return	CNMS_FALSE;
	}
	else{
		return	CNMS_TRUE;
	}
}


/***   Process for SpuitButton Action depending on Toggle Status   ***/
CNMSVoid CnmsHGSpuitToolAction(
						CNMSVoid									)
{
	CNMSInt32		i							= 0;
	CNMSInt32		defVal[CNMS_HG_SPUIT_MAX]	= { CNMS_ERR, HG_DEF_SHADOW, HG_DEF_MID, HG_DEF_HIGHLIGHT };
	CNMSUInt32		*pblack[CNMS_CA_MAX]		= { &(ca_param_all.blackpoint.m), &(ca_param_all.blackpoint.r), &(ca_param_all.blackpoint.g), &(ca_param_all.blackpoint.b) },
					*pmid[CNMS_CA_MAX]			= { &(ca_param_all.midpoint.m), &(ca_param_all.midpoint.r), &(ca_param_all.midpoint.g), &(ca_param_all.midpoint.b) },
					*pwhite[CNMS_CA_MAX]		= { &(ca_param_all.whitepoint.m), &(ca_param_all.whitepoint.r), &(ca_param_all.whitepoint.g), &(ca_param_all.whitepoint.b) };

	if( ( hg_info.cVal[CNMS_CA_RED] < 0 ) || ( hg_info.cVal[CNMS_CA_GREEN] < 0 ) || ( hg_info.cVal[CNMS_CA_BLUE] < 0 ) || ( hg_info.cVal[CNMS_CA_GRAY] < 0 ) ){
		DBGMSG( "[CnmsHGSpuitToolAction] RGBKValue is invalid. cVal[CNMS_CA_RED][%d], cVal[CNMS_CA_GREEN][%d], cVal[CNMS_CA_BLUE][%d], cVal[CNMS_CA_GRAY][%d]\n",
													hg_info.cVal[CNMS_CA_RED], hg_info.cVal[CNMS_CA_GREEN], hg_info.cVal[CNMS_CA_BLUE], hg_info.cVal[CNMS_CA_GRAY] );
		return;
	}

	switch( ca_info.colorgroup ){
		case CNMS_CA_COLORGROUP_COLOR:
				if( hg_info.btnType == CNMS_HG_SPUIT_GRAYLEVEL ){
					ca_param_all.graypoint.r = hg_info.cVal[CNMS_CA_RED];
					ca_param_all.graypoint.g = hg_info.cVal[CNMS_CA_GREEN];
					ca_param_all.graypoint.b = hg_info.cVal[CNMS_CA_BLUE];
				}
				else{
					switch( hg_dlg.channel ){
						case CNMS_CA_MASTER:
								if( ( CNMS_HG_SPUIT_SHADOW <= hg_info.btnType ) || ( hg_info.btnType <= CNMS_HG_SPUIT_HIGHLIGHT ) ){
									CnmsHGCalcurateParameter( &(hg[CNMS_CA_MASTER]), hg_info.btnType, defVal[hg_info.btnType] );
									CnmsHGCalcurateParameter( &(hg[CNMS_CA_RED]), hg_info.btnType, hg_info.cVal[CNMS_CA_RED] );
									CnmsHGCalcurateParameter( &(hg[CNMS_CA_GREEN]), hg_info.btnType, hg_info.cVal[CNMS_CA_GREEN] );
									CnmsHGCalcurateParameter( &(hg[CNMS_CA_BLUE]), hg_info.btnType, hg_info.cVal[CNMS_CA_BLUE] );
								}
								else{
									DBGMSG( "[CnmsHGSpuitToolAction] Value is invalid. channel[%d], button_type[%d]\n", hg_dlg.channel, hg_info.btnType );
									break;
								}
								for( i = CNMS_CA_MASTER ; i <= CNMS_CA_BLUE ; i++ ){
									*(pblack[i]) = hg[i].shadow.disp;
									*(pmid[i]) = hg[i].mid.disp;
									*(pwhite[i]) = hg[i].highlight.disp;
								}
								break;

						case CNMS_CA_RED:
						case CNMS_CA_GREEN:
						case CNMS_CA_BLUE:
								if( ( CNMS_HG_SPUIT_SHADOW <= hg_info.btnType ) && ( hg_info.btnType <= CNMS_HG_SPUIT_HIGHLIGHT ) ){
									CnmsHGCalcurateParameter( &(hg[hg_dlg.channel]), hg_info.btnType, hg_info.cVal[hg_dlg.channel] );
								}
								else{
									DBGMSG( "[CnmsHGSpuitToolAction] Value is invalid. channel[%d], button_type[%d]\n", hg_dlg.channel, hg_info.btnType );
									break;
								}
								*(pblack[hg_dlg.channel]) = hg[hg_dlg.channel].shadow.disp;
								*(pmid[hg_dlg.channel]) = hg[hg_dlg.channel].mid.disp;
								*(pwhite[hg_dlg.channel]) = hg[hg_dlg.channel].highlight.disp;
								break;

						default:
								DBGMSG( "[CnmsHGSpuitToolAction] Value is invalid. channel[%d], button_type[%d]\n", hg_dlg.channel, hg_info.btnType );
								break;
					}
				}
				break;

		case CNMS_CA_COLORGROUP_GRAY:
				if( ( hg_info.btnType >= CNMS_HG_SPUIT_SHADOW ) && ( hg_info.btnType <= CNMS_HG_SPUIT_HIGHLIGHT ) ){
					CnmsHGCalcurateParameter( &(hg[hg_dlg.channel]), hg_info.btnType, hg_info.cVal[hg_dlg.channel] );
				}
				else{
					DBGMSG( "[CnmsHGSpuitToolAction] Value is invalid. channel[%d], button_type[%d]\n", hg_dlg.channel, hg_info.btnType );
					break;
				}
				*(pblack[CNMS_CA_MASTER]) = hg[hg_dlg.channel].shadow.disp;
				*(pmid[CNMS_CA_MASTER]) = hg[hg_dlg.channel].mid.disp;
				*(pwhite[CNMS_CA_MASTER]) = hg[hg_dlg.channel].highlight.disp;
				break;

		default:
				DBGMSG( "[CnmsHGSpuitToolAction] Value is invalid. ca_info.colorgroup[%d]\n", ca_info.colorgroup );
				break;

	}

	CnmsHGDispSpinButtonValue();
	CnmsHGDrawOnlyScaleBar();
	CnmsHGDrawHGandFRToneCurve();

	return;
}


/***   Changeing status about SpuitButton Sansitive ON/OFF   ***/
static CNMSInt32 CnmsHGSpuitButtonStatusChange(
						CNMSLPSTR			buttonStr				)
{
	CNMSInt32		ret				= CNMS_ERR;
	GtkWidget		*toggleButton	= CNMSNULL;

	if( buttonStr == CNMSNULL ){
		goto	EXIT;
	}
	else if( ( toggleButton = lookup_widget( hg_dlg.widget, buttonStr ) ) == CNMSNULL ){
		DBGMSG( "[CnmsHGSpuitButtonStatusChange]Can't look up widget( %s ).\n", buttonStr );
		goto	EXIT;
	}

	/* Preview On with toggle sensitive, Off with toggle not sensitive */
	gtk_widget_set_sensitive( toggleButton, ca_info.preview );

	/* Case Grayscale, Always GrayBalance Button is Not Sensitive	*/
	if( ( toggleButton == hg_togglebutton_gbal ) && ( ca_info.colorgroup == CNMS_CA_COLORGROUP_GRAY ) ){
		gtk_widget_set_sensitive( toggleButton, CNMS_FALSE );
	}

	ret = CNMS_NO_ERR;
EXIT:
	return	ret;
}


/***   Draw Icon depending on Triangle Icon Position   ***/
static CNMSVoid CnmsHGDrawTriangle(
						CNMSHGTriIconRect	*icon,
						CNMSInt32			center					)
{
	GdkPoint		point[4];
	GdkDrawable		*drawable = hg_pmap;
	GdkGC			*gc = hg_gc;
	
	if( ( drawable == CNMSNULL ) || ( gc == CNMSNULL ) || ( icon == CNMSNULL ) ){
		DBGMSG( "[CnmsHGDrawTriangle] Pointer is NULL. drawable[%p], gc[%p], icon[%p]\n",drawable,gc,icon );
		return;
	}		

	icon->center	= center;

	/* Create Triangle Icon Coordinate	*/
	point[0].x = icon->center;							point[0].y = HG_SCALE_TRI_TOP;
	point[1].x = icon->center - HG_SCALE_TRI_HEIGHT;	point[1].y = HG_SCALE_TRI_TOP + HG_SCALE_TRI_HEIGHT;
	point[2].x = icon->center + HG_SCALE_TRI_HEIGHT;	point[2].y = HG_SCALE_TRI_TOP + HG_SCALE_TRI_HEIGHT;
	point[3].x = point[0].x;							point[3].y = point[0].y;

	/* Set Triangle Icon Information	*/
	icon->left		= icon->center - ( HG_SCALE_TRI_HEIGHT / 2 ); 
	icon->top		= HG_SCALE_TRI_TOP;
	icon->right		= icon->center + ( HG_SCALE_TRI_HEIGHT / 2 );
	icon->bottom	= HG_SCALE_TRI_TOP + HG_SCALE_TRI_HEIGHT;

	/* Draw Triangle Icon Inside		*/
	gdk_draw_polygon( drawable, gc, CNMS_TRUE, point, 4 );

	/* Draw Triangle Icon Outframe		*/
	CnmsColAdjGdkGCSetColor( gc, HG_GC_TRIICON_FRAME, HG_GC_TRIICON_FRAME, HG_GC_TRIICON_FRAME );	
	gdk_draw_polygon( drawable, gc, CNMS_FALSE, point, 4 );
	
	return;
}


/***   Calcurate Value Shadow/Mid/Highlight depending on Channel   ***/
static CNMSVoid CnmsHGCalcurateValue(
						CNMSInt32			x,
						CNMSInt32			grab					)
{
	CNMSUInt32		*pblack[CNMS_CA_MAX]	= { &(ca_param_all.blackpoint.m), &(ca_param_all.blackpoint.r), &(ca_param_all.blackpoint.g), &(ca_param_all.blackpoint.b) },
					*pmid[CNMS_CA_MAX]		= { &(ca_param_all.midpoint.m), &(ca_param_all.midpoint.r), &(ca_param_all.midpoint.g), &(ca_param_all.midpoint.b) },
					*pwhite[CNMS_CA_MAX]	= { &(ca_param_all.whitepoint.m), &(ca_param_all.whitepoint.r), &(ca_param_all.whitepoint.g), &(ca_param_all.whitepoint.b) };

	/* Calcurate parameter depending on Channel	*/
	if( ( ( CNMS_CA_MASTER <= hg_dlg.channel ) && ( hg_dlg.channel <= CNMS_CA_BLUE ) ) || ( hg_dlg.channel == CNMS_CA_GRAY ) ){
		CnmsHGCalcurateParameter( &(hg[hg_dlg.channel]), grab, x );
	}
	else{
		DBGMSG( "[CnmsHGCalcurateValue] Value is invalid. ca_info.channel[%d]\n", hg_dlg.channel );
		return;
	}

	/* Set value depending on Channel			*/
	if( ( CNMS_CA_MASTER <= hg_dlg.channel ) && ( hg_dlg.channel <= CNMS_CA_BLUE ) ){
		*pblack[hg_dlg.channel] = hg[hg_dlg.channel].shadow.disp;
		*pmid[hg_dlg.channel] = hg[hg_dlg.channel].mid.disp;
		*pwhite[hg_dlg.channel] = hg[hg_dlg.channel].highlight.disp;
	}
	else if( hg_dlg.channel == CNMS_CA_GRAY ){
		*pblack[CNMS_CA_MASTER] = hg[hg_dlg.channel].shadow.disp;
		*pmid[CNMS_CA_MASTER] = hg[hg_dlg.channel].mid.disp;
		*pwhite[CNMS_CA_MASTER] = hg[hg_dlg.channel].highlight.disp;
	}
	else{
		DBGMSG( "[CnmsHGCalcurateValue] Pointer[hg_current] is invalid.\n" );
		return;
	}

	return;
}


/***   Calcurate Value Shadow/Mid/Highlight depending on Grabing Icon Type   ***/
static CNMSVoid CnmsHGCalcurateParameter(
						CNMSHGValue			*current,
						CNMSInt32			grab,
						CNMSInt32			value					)
{
	CNMSDec32			ratio		= 1.0;
	CNMSHGScaleBarPos	*shadow		= CNMSNULL,
						*mid		= CNMSNULL,
						*highlight	= CNMSNULL;
			
	if( current == CNMSNULL ){
		DBGMSG( "[CnmsHGCalcurateParameter] Arg Pointer is NULL. current[%p]\n", current );
		return;
	}

	shadow = &(current->shadow),
	mid = &(current->mid),
	highlight = &(current->highlight);
	ratio = ( mid->raw - shadow->raw ) / ( highlight->raw - shadow->raw );		/* Calcurate Ratio	*/

	/* raw ( float type ): For up ratio accuracy, disp ( integer type ): For display to Spinbutton	*/ 
	switch( grab )
	{
		case CNMS_HG_GRAB_SHADOW:
				if( value < HG_DEF_SHADOW ){											/* Case Shadow Value under Min, Value Set Min	*/
					shadow->raw = HG_DEF_SHADOW;
					shadow->disp = HG_DEF_SHADOW;
				}
				else if( value > highlight->disp - 10 ){								/* Case Shadow Value over Max, Value Set Max	*/
					shadow->raw = highlight->raw - 10;
					shadow->disp = highlight->disp - 10;
				}
				else{
					shadow->raw = ( CNMSDec32 )value;
					shadow->disp = value;
				}
				
				mid->raw = shadow->raw + ( ( highlight->raw - shadow->raw )*ratio );	/* Calcurate Mid Value based Ratio				*/
				if( ( CNMSInt32 )mid->raw > ( CNMSInt32 )(highlight->raw - 5) ){		/* Case Calcurated Mid Value over Max, Set Max	*/
					mid->disp = highlight->disp - 5;
				}
				else if( ( CNMSInt32 )mid->raw < ( CNMSInt32 )(shadow->raw + 5) ){		/* Case Calcurated Mid Value under Min, Set Min	*/
					mid->disp = shadow->disp + 5;
				}
				else{																	/* Only Round Off Result Value					*/
					mid->disp = ( CNMSInt32 )(mid->raw + 0.5);
				}
				
				/* Set Max/Min Value	*/
				hg_adjust_highlight->lower =  shadow->disp + 10;
				highlight->lower = ( CNMSInt32 )shadow->disp + 10;

				hg_adjust_mid->lower =  shadow->disp + 5;
				mid->lower = ( CNMSInt32 )shadow->disp + 5;
				
				break;
				
		case CNMS_HG_GRAB_MID:
				if( value < shadow->disp + 5 ){
					mid->raw = shadow->raw + 5;
					mid->disp = shadow->disp + 5;
				}
				else if( value > highlight->disp - 5 ){
					mid->raw = highlight->raw - 5;
					mid->disp = highlight->disp - 5;
				}
				else{
					mid->raw = ( CNMSDec32 )value;
					mid->disp = value;
				}
				break;

		case CNMS_HG_GRAB_HIGHLIGHT:
				if( value < shadow->disp + 10 ){
					highlight->raw = shadow->raw + 10;
					highlight->disp = shadow->disp + 10;
				}
				else if( value > HG_DEF_HIGHLIGHT ){
					highlight->raw = HG_DEF_HIGHLIGHT;
					highlight->disp = HG_DEF_HIGHLIGHT;
				}
				else{
					highlight->raw = ( CNMSDec32 )value;
					highlight->disp = value;
				}

				mid->raw = shadow->raw + ( ( highlight->raw - shadow->raw )*ratio );
				if( ( CNMSInt32 )mid->raw < ( CNMSInt32 )(shadow->raw + 5) ){
					mid->disp = shadow->disp + 5;
				}
				else if( ( CNMSInt32 )mid->raw > ( CNMSInt32 )(highlight->raw - 5) ){
					mid->disp = highlight->disp - 5;
				}
				else{
					mid->disp = ( CNMSInt32 )(mid->raw + 0.5);
				}
				
				/* Set Max/Min Value	*/
				hg_adjust_shadow->upper = highlight->disp - 10;
				shadow->upper = ( CNMSInt32 )highlight->disp - 10;

				hg_adjust_mid->upper = highlight->disp - 5;
				mid->upper = ( CNMSInt32 )highlight->disp - 5;
				
				break;
	}
	
	return;
}


/***   Set Value to SpinButton ( But No Signal by SpinButton Value Changed )   ***/
static CNMSVoid CnmsHGDispSpinButtonValue(
						CNMSVoid									)
{
	if( ( hg_adjust_shadow == CNMSNULL ) || ( hg_adjust_mid == CNMSNULL ) || ( hg_adjust_highlight == CNMSNULL ) ){
		DBGMSG( "[CnmsHGDispSpinButtonValue] Pointer is NULL. hg_adjust_shadow[%p], hg_adjust_mid[%p], hg_adjust_highlight[%p]\n",
																					hg_adjust_shadow, hg_adjust_mid, hg_adjust_highlight );
		return;
	}
	if( ( hg_entry_shadow == CNMSNULL ) || ( hg_entry_mid == CNMSNULL ) || ( hg_entry_highlight == CNMSNULL ) ){
		DBGMSG( "[CnmsHGDispSpinButtonValue] Pointer is NULL. hg_entry_shadow[%p], hg_entry_mid[%p], hg_entry_highlight[%p]\n",
																	 					hg_entry_shadow, hg_entry_mid, hg_entry_highlight );
		return;
	}

	/* Set Flag for No Signal			*/
	hg_flag_spinbutton = CNMS_TRUE;

	/* First, Set Limit Value			*/
	hg_adjust_shadow->lower = hg[hg_dlg.channel].shadow.lower;
	hg_adjust_shadow->upper = hg[hg_dlg.channel].shadow.upper;
	hg_adjust_mid->lower = hg[hg_dlg.channel].mid.lower;
	hg_adjust_mid->upper = hg[hg_dlg.channel].mid.upper;
	hg_adjust_highlight->lower = hg[hg_dlg.channel].highlight.lower;
	hg_adjust_highlight->upper = hg[hg_dlg.channel].highlight.upper;

	/* Next, Set Value to SpinButton	*/
	gtk_spin_button_set_value( GTK_SPIN_BUTTON( hg_entry_shadow ), hg[hg_dlg.channel].shadow.disp );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON( hg_entry_mid ), hg[hg_dlg.channel].mid.disp );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON( hg_entry_highlight ), hg[hg_dlg.channel].highlight.disp );

	/* ReSet Flag						*/
	hg_flag_spinbutton = CNMS_FALSE;
	
	return;
}


/***   Draw Triangle Icons depending on Values   ***/
static CNMSVoid CnmsHGDrawOnlyScaleBar(
						CNMSVoid									)
{
	CNMSInt32			center_s	= 0,
						center_m	= 0,
						center_h	= 0;

	GdkPixmap			*pixmap		= hg_pmap;
	GdkGC				*gc			= hg_gc;
	CNMSHGTriIconRect	*highlight	= &hg_area_highlight,
						*mid		= &hg_area_mid,
						*shadow		= &hg_area_shadow;

	if( ( pixmap == CNMSNULL ) || ( gc == CNMSNULL ) && ( highlight == CNMSNULL ) && ( mid == CNMSNULL ) && ( shadow == CNMSNULL ) ){
		DBGMSG( "[CnmsHGDrawOnlyScaleBar] Arg Pointer is NULL. pixmap[%p], gc[%p], highlight[%p], mid[%p], shadow[%p]\n", pixmap, gc, highlight, mid, shadow );
		return;
	}
		
	/* Draw BackGround 				*/
	CnmsColAdjGdkGCSetColor( gc, ca_bg_color.red, ca_bg_color.green, ca_bg_color.blue );
	gdk_draw_rectangle (pixmap, gc, CNMS_TRUE, 0, 0, hg_scalebar_drawarea->allocation.width, hg_scalebar_drawarea->allocation.height);

	/* Get Value, Set Center		*/
	center_s = ( ( CNMSInt32 )( ( ( CNMSDec32 )hg[hg_dlg.channel].shadow.disp * HG_SCALE_AREA_WIDTH / 255 ) + 0.5 ) ) + HG_SCALE_AREA_OFFSET;
	center_m = ( ( CNMSInt32 )( ( ( CNMSDec32 )hg[hg_dlg.channel].mid.disp * HG_SCALE_AREA_WIDTH / 255 ) + 0.5 ) ) + HG_SCALE_AREA_OFFSET;
	center_h = ( ( CNMSInt32 )( ( ( CNMSDec32 )hg[hg_dlg.channel].highlight.disp * HG_SCALE_AREA_WIDTH / 255 ) + 0.5 ) ) + HG_SCALE_AREA_OFFSET;

	/* Draw Shadow Triangle Icon	*/
	CnmsColAdjGdkGCSetColor( gc, HG_GC_TRIICON_SHADOW, HG_GC_TRIICON_SHADOW, HG_GC_TRIICON_SHADOW );
	CnmsHGDrawTriangle( shadow, center_s );

	/* Draw Mid Triangle Icon		*/
	CnmsColAdjGdkGCSetColor( gc, HG_GC_TRIICON_MID, HG_GC_TRIICON_MID, HG_GC_TRIICON_MID );
	CnmsHGDrawTriangle( mid, center_m );

	/* Draw Highlight Triangle Icon	*/
	CnmsColAdjGdkGCSetColor( gc, HG_GC_TRIICON_HIGH, HG_GC_TRIICON_HIGH, HG_GC_TRIICON_HIGH );
	CnmsHGDrawTriangle( highlight, center_h );

	gdk_draw_pixmap( hg_scalebar_drawarea->window, hg_scalebar_drawarea->style->fg_gc[GTK_WIDGET_STATE (hg_scalebar_drawarea)],
															pixmap, 0, 0, 0, 0,	hg_scalebar_drawarea->allocation.width, hg_scalebar_drawarea->allocation.height );
	return;
}


/***   Culcurate Histogram and FinalReview ToneCurve, and Draw   ***/
static CNMSVoid CnmsHGDrawHGandFRToneCurve(
						CNMSVoid									)
{
	CnmsColAdjCurveCalcHG( &ca_param_all );												/* Calcurate Histogram ToneCurve for Histogram		*/
	CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_HG );

	/* Case Flag OFF, Redraw Preview Data by Parameter Changed */
	CnmsColAdjPreviewImageSet();

	return;
}

#endif	/* _COLORADJUST_HG_C_ */
