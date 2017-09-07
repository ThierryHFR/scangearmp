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

#ifndef	_COLORADJUST_MC_C_
#define	_COLORADJUST_MC_C_

#include "coloradjust_mc.h"

#define		MC_GC_UNDER_THRESHOLD		(0xD0D0)									/* 208 GC color under Threshold Background	*/

	static	CNMSCAScaleBarInfo	mc_scalebar;

static CNMSInt32 CnmsWidthConvertFrom255(
						CNMSInt32			co_input,
						CNMSInt32			co_max					);

/***   Monochrome Dialog Show   ***/
CNMSVoid CnmsMCDialogShow(
						CNMSInt32			color,
						CNMSInt32			keep					)
{
	/* Channel ComboBox Item Change depending on ColorGroup */
	if( ( CnmsColAdjChannelItemSet( color, CNMS_CA_DIALOG_MC ) ) == CNMS_FALSE )	return;

	/* Show/UnShow Change */
	if( keep == CNMS_CA_DIALOG_NON_KEEP ){
		CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_MC, !( GTK_WIDGET_VISIBLE( mc_dlg.widget ) ) );
	}
	gtk_range_set_value( GTK_RANGE( mc_scale ), ( CNMSDec32 )mc_mono.threshold );
	if( GTK_WIDGET_VISIBLE( mc_dlg.widget ) ){
		CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_MC );
	}

	return;
}


/***   Data Initialize   ***/
CNMSVoid CnmsMCInit(
						CNMSVoid									)
{
	/* Set Default Threshold Value	*/
	mc_mono.threshold = lpW1Comp->thresholdDef;

	/* Initialize HistoGram Value	*/
	CnmsColAdjInitCurveAndHistogram( &mc_dlg );

	/* Initialize ScaleBar Info		*/
	mc_scalebar.x = mc_scalebar.value = mc_scalebar.range = mc_scalebar.flag = 0;

	return;
}


/***   Reset   ***/
CNMSVoid CnmsMCReset(
						CNMSVoid									)
{
	/* Data Initialize						*/
	CnmsMCInit();
	if( GTK_WIDGET_VISIBLE( mc_dlg.widget ) == CNMS_TRUE ){
		gtk_range_set_value( GTK_RANGE( mc_scale ), ( CNMSDec32 )mc_mono.threshold );
		CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_MC );
	}

	return;
}


/***   Threshold Value Changed with ScaleBar   ***/
CNMSVoid CnmsMCScaleThresholdValueChanged(
						GtkRange			*range					)
{
	if( range == CNMSNULL ){
		DBGMSG( "[CnmsMCScaleThresholdValueChanged] Pointer is NULL. range[%p]\n", range );
		return;
	}

	/* Set Value to SpinButton			*/
	if( mc_scalebar.flag == TRUE ){
		gtk_spin_button_set_value( GTK_SPIN_BUTTON( mc_spinbutton ), mc_scalebar.value );
		gtk_range_set_value( GTK_RANGE( mc_scale ), mc_scalebar.range );
		mc_scalebar.flag = FALSE;
	}
	else{
		gtk_spin_button_set_value( GTK_SPIN_BUTTON( mc_spinbutton ), gtk_range_get_value( range ) );
	}
	CnmsColAdjSetZeroIfMinusZeroForSpinbutton( mc_spinbutton );

	/* Draw Threshold					*/
	CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_MC );

	/* Draw Preview Image Set Threshold	*/
	CnmsColAdjPreviewImageSet();
	
	return;
}


/***   Threshold Value Changed with SpinButton   ***/
CNMSVoid CnmsMCSpinButtonThresholdValueChanged(
						GtkSpinButton		*spinbutton				)
{
	if( spinbutton == CNMSNULL ){
		DBGMSG( "[CnmsMCSpinButtonThresholdValueChanged] Arg pointer [spinbutton] is NULL.\n" );
		return;
	}

	/* Set Value to ScaleBar	*/
	mc_mono.threshold = gtk_spin_button_get_value_as_int( spinbutton );
	gtk_range_set_value( GTK_RANGE( mc_scale ), ( CNMSDec32 )mc_mono.threshold );
	
	return;
}


/***   Draw Threshold Bar   ***/
CNMSVoid CnmsMCDrawThreshold(
						GtkWidget			*widget,
						CNMSUInt32			histogram[],
						CNMSInt32			value					)
{
	CNMSInt32	Left			= CNMS_CA_GRAPH_FRAME_LEFT,
				Top				= CNMS_CA_GRAPH_FRAME_TOP,
				Right			= CNMS_CA_GRAPH_FRAME_RIGHT,
				Bottom			= CNMS_CA_GRAPH_FRAME_BOTTOM,
				XPosRedBar		= CnmsWidthConvertFrom255( value, ( Right - Left - 1 ) - 0.5 ) + ( Left + 1 ),
				XPosUnderThre	= CnmsWidthConvertFrom255( value, ( Right - Left - 1 ) - 0.5 );
	GdkPixmap	*Pixmap			= mc_dlg.pmap;
	GdkGC		*Gc				= mc_dlg.gc;
				
	if( ( widget == CNMSNULL ) || ( histogram == CNMSNULL ) || ( Pixmap == CNMSNULL ) || ( Gc == CNMSNULL ) ){
		DBGMSG( "[CnmsMCDrawThreshold] Pointer is NULL. widget[%p], histogram[%p], Pixmap[%p], Gc[%p]\n", widget, histogram, Pixmap, Gc );
		return;
	}

	/* Draw	Graph Frame									*/
	CnmsColAdjDrawAreaBaseDraw( widget, Pixmap, Gc );

	/* Draw Background for Under Threshold				*/
	CnmsColAdjGdkGCSetColor( Gc, MC_GC_UNDER_THRESHOLD, MC_GC_UNDER_THRESHOLD, MC_GC_UNDER_THRESHOLD );
	gdk_draw_rectangle (Pixmap, Gc, CNMS_TRUE, ( Left + 1 ), ( Top + 1 ), XPosUnderThre, ( Bottom - Top - 1 ) );

	/* Process above Hide Base Frame, ReDraw Base Frame	*/ 
	CnmsColAdjGdkGCSetColor( Gc, CNMS_CA_GC_FRAME_R, CNMS_CA_GC_FRAME_G, CNMS_CA_GC_FRAME_B );
	gdk_draw_line( Pixmap, Gc, 	( ( Right - Left ) * 1 / 4 ) + Left, Top, ( ( Right - Left ) * 1 / 4 ) + Left, Bottom ); 
	gdk_draw_line( Pixmap, Gc, 	( ( Right - Left ) * 2 / 4 ) + Left, Top, ( ( Right - Left ) * 2 / 4 ) + Left, Bottom ); 
	gdk_draw_line( Pixmap, Gc, 	( ( Right - Left ) * 3 / 4 ) + Left, Top, ( ( Right - Left ) * 3 / 4 ) + Left, Bottom ); 
	gdk_draw_line( Pixmap, Gc,	Left, ( ( Bottom - Top ) * 1 / 4 ) + Top, Right,( ( Bottom - Top ) * 1 / 4 ) + Top ); 
	gdk_draw_line( Pixmap, Gc,	Left, ( ( Bottom - Top ) * 2 / 4 ) + Top, Right,( ( Bottom - Top ) * 2 / 4 ) + Top ); 
	gdk_draw_line( Pixmap, Gc,	Left, ( ( Bottom - Top ) * 3 / 4 ) + Top, Right,( ( Bottom - Top ) * 3 / 4 ) + Top ); 

	/* Draw	Histogram									*/
	CnmsColAdjDrawAreaHistogramDraw( Pixmap, Gc, histogram );

	/* Draw	Red Bar for Threshold						*/
	CnmsColAdjGdkGCSetColor( Gc, CNMS_CA_GC_VALUE_MAX, CNMS_CA_GC_VALUE_ZERO, CNMS_CA_GC_VALUE_ZERO );
	gdk_draw_line( Pixmap, Gc, XPosRedBar, ( Top + 1 ), XPosRedBar, ( Bottom - 1 ) ); 

	return;
}


CNMSVoid CnmsMCScaleBarPress(
						GdkEventButton		*event					)
{
	mc_scalebar.x = event->x;
	mc_scalebar.value = (CNMSInt32)atoi( gtk_entry_get_text( GTK_ENTRY( mc_spinbutton ) ) );
	mc_scalebar.range = gtk_range_get_value( GTK_RANGE( mc_scale ) );
	return;
}


CNMSVoid CnmsMCScaleBarRelease(
						GdkEventButton		*event					)
{
	if( event->x == mc_scalebar.x ){
		mc_scalebar.flag = TRUE;
	}
	return;
}


/***   Coordinate Convert to Red Bar Width from 255   ***/
static CNMSInt32 CnmsWidthConvertFrom255(
						CNMSInt32			co_input,
						CNMSInt32			co_max					)
{
	return ( CNMSInt32 )( ( ( CNMSDec32 )co_input * ( CNMSDec32 )co_max / 255 ) + 0.5 );
}


#endif	/* _COLORADJUST_MC_C_ */
