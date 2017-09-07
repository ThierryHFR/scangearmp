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

#ifndef	_COLORADJUST_C_
#define	_COLORADJUST_C_

#define	_CNMS_CA_GLOBALS_
#include "coloradjust.h"
#undef	_CNMS_CA_GLOBALS_

/* #define __COLOR_ADJUST_DEBUG__ */

#include "prev_main.h"
#include "cnmsstr.h"

#include "coloradjust_bc.h"
#include "coloradjust_hg.h"
#include "coloradjust_tc.h"
#include "coloradjust_fr.h"
#include "coloradjust_mc.h"

#include "w1.h"

static CNMSVoid CnmsColAdjCurveValueNormalize(
						CNMSUInt32			*curve,
						CNMSInt32			max_len,
						CNMSInt32			max_value				);

static CNMSInt32 CnmsColAdjCopyHistogram(
						CNMSUInt32			masterSrc[],
						CNMSUInt32			redSrc[],
						CNMSUInt32			greenSrc[],
						CNMSUInt32			blueSrc[],
						CNMSUInt32			graySrc[],
						CNMSUInt32			masterDst[],
						CNMSUInt32			redDst[],
						CNMSUInt32			greenDst[],
						CNMSUInt32			blueDst[],
						CNMSUInt32			grayDst[]				);

static CNMSInt32 CnmsColAdjGetDialogInfo(
						CNMSInt32			id,
						CNMSCADialog		**dlg					);

/***   ColorAdjust Dialog/widget Create & Initialize   ***/
CNMSInt32 CnmsColAdjInit(
						CNMSVoid									)
{
	CNMSInt32		ret				= CNMS_ERR;
					
	if( main_window == CNMSNULL ){
		DBGMSG( "[CnmsColAdjInit] Pointer is NULL. main_window[%p]\n",main_window );
		goto EXIT;
	}		
		
	/* Create & Initialize Brightness/Contrast Dialog	*/
	if( bc_dlg.widget == CNMSNULL ){
		if(      ( bc_dlg.widget			= create_dialog_bright_contrast() ) == CNMSNULL )							goto EXIT;
		else if( ( bc_dlg.cbox 				= lookup_widget( bc_dlg.widget, "bc_combobox" )					) == NULL )	goto EXIT;
		else if( ( bc_dlg.darea				= lookup_widget( bc_dlg.widget, "bc_drawingarea" )				) == NULL )	goto EXIT;
		else if( ( bc_scale_bright 			= lookup_widget( bc_dlg.widget, "bc_scale_bright" ) 			) == NULL )	goto EXIT;
		else if( ( bc_spinbutton_bright 	= lookup_widget( bc_dlg.widget, "bc_spinbutton_bright" )		) == NULL )	goto EXIT;
		else if( ( bc_scale_contrast 		= lookup_widget( bc_dlg.widget, "bc_scale_contrast" )			) == NULL )	goto EXIT;
		else if( ( bc_spinbutton_contrast 	= lookup_widget( bc_dlg.widget, "bc_spinbutton_contrast" )		) == NULL )	goto EXIT;
		else{
			CnmsColAdjDialogInfoInit( CNMS_CA_DIALOG_BC );
			CnmsBCInit();
		}
	}
	/* Set background color */
	ca_bg_color = bc_dlg.widget->style->bg[GTK_WIDGET_STATE(bc_dlg.widget)];

	/* Create & Initialize Histogram Dialog	*/
	if( hg_dlg.widget == CNMSNULL ){
		if(      ( hg_dlg.widget	 		= create_dialog_histogram() ) == CNMSNULL )									goto EXIT;
		else if( ( hg_dlg.cbox	 			= lookup_widget( hg_dlg.widget, "hg_combobox" ) 				) == NULL )	goto EXIT;
		else if( ( hg_dlg.darea	 			= lookup_widget( hg_dlg.widget, "hg_drawingarea" ) 				) == NULL )	goto EXIT;
		else if( ( hg_entry_shadow	 		= lookup_widget( hg_dlg.widget, "hg_spinbutton_shadow" ) 		) == NULL )	goto EXIT;
		else if( ( hg_entry_mid	 			= lookup_widget( hg_dlg.widget, "hg_spinbutton_mid" ) 			) == NULL )	goto EXIT;
		else if( ( hg_entry_highlight		= lookup_widget( hg_dlg.widget, "hg_spinbutton_highlight" )		) == NULL )	goto EXIT;
		else if( ( hg_scalebar_drawarea		= lookup_widget( hg_dlg.widget, "hg_drawingarea_scalebar" ) 	) == NULL )	goto EXIT;
		else if( ( hg_label_str_1	 		= lookup_widget( hg_dlg.widget, "hg_label_str_1" ) 				) == NULL )	goto EXIT;
		else if( ( hg_label_str_2	 		= lookup_widget( hg_dlg.widget, "hg_label_str_2" ) 				) == NULL )	goto EXIT;
		else if( ( hg_label_str_3	 		= lookup_widget( hg_dlg.widget, "hg_label_str_3" ) 				) == NULL )	goto EXIT;
		else if( ( hg_label_value_1 		= lookup_widget( hg_dlg.widget, "hg_label_value_1" ) 			) == NULL )	goto EXIT;
		else if( ( hg_label_value_2 		= lookup_widget( hg_dlg.widget, "hg_label_value_2" ) 			) == NULL )	goto EXIT;
		else if( ( hg_label_value_3 		= lookup_widget( hg_dlg.widget, "hg_label_value_3" ) 			) == NULL )	goto EXIT;
		else if( ( hg_togglebutton_gbal		= lookup_widget( hg_dlg.widget, "hg_togglebutton_graybalance" ) ) == NULL )	goto EXIT;
		else{
			CnmsColAdjDialogInfoInit( CNMS_CA_DIALOG_HG );
			CnmsHGInit();
		}
	}

	/* Create & Initialize ToneCurve Dialog	*/
	if( tc_dlg.widget == CNMSNULL ){
		if(      ( tc_dlg.widget			= create_dialog_tonecurve() ) == CNMSNULL )									goto EXIT;
		else if( ( tc_dlg.cbox				= lookup_widget( tc_dlg.widget, "tc_combobox" ) 				) == NULL )	goto EXIT;
		else if( ( tc_dlg.darea				= lookup_widget( tc_dlg.widget, "tc_drawingarea" ) 				) == NULL )	goto EXIT;
		else if( ( tc_combobox_tone_select	= lookup_widget( tc_dlg.widget, "tc_combobox_tc_select" ) 		) == NULL )	goto EXIT;
		else{
			CnmsColAdjDialogInfoInit( CNMS_CA_DIALOG_TC );
			CnmsTCInit();
		}
	}

	/* Create & Initialize FinalReview Dialog	*/
	if( fr_dlg.widget == CNMSNULL ){
		if(      ( fr_dlg.widget	= create_dialog_final_review() ) == CNMSNULL )										goto EXIT;
		else if( ( fr_dlg.cbox		 	= lookup_widget( fr_dlg.widget, "fr_combobox" ) 					) == NULL )	goto EXIT;
		else if( ( fr_dlg.darea	 		= lookup_widget( fr_dlg.widget, "fr_drawingarea" ) 					) == NULL )	goto EXIT;
		else if( ( fr_label_str_a1	 	= lookup_widget( fr_dlg.widget, "fr_label_str_a_1" ) 				) == NULL )	goto EXIT;
		else if( ( fr_label_str_b1	 	= lookup_widget( fr_dlg.widget, "fr_label_str_b_1" ) 				) == NULL )	goto EXIT;
		else if( ( fr_label_str_a2	 	= lookup_widget( fr_dlg.widget, "fr_label_str_a_2" ) 				) == NULL )	goto EXIT;
		else if( ( fr_label_str_b2	 	= lookup_widget( fr_dlg.widget, "fr_label_str_b_2" ) 				) == NULL )	goto EXIT;
		else if( ( fr_label_str_a3	 	= lookup_widget( fr_dlg.widget, "fr_label_str_a_3" ) 				) == NULL )	goto EXIT;
		else if( ( fr_label_str_b3	 	= lookup_widget( fr_dlg.widget, "fr_label_str_b_3" ) 				) == NULL )	goto EXIT;
		else if( ( fr_label_value_a1 	= lookup_widget( fr_dlg.widget, "fr_label_value_a_1" ) 				) == NULL )	goto EXIT;
		else if( ( fr_label_value_b1 	= lookup_widget( fr_dlg.widget, "fr_label_value_b_1" ) 				) == NULL )	goto EXIT;
		else if( ( fr_label_value_a2 	= lookup_widget( fr_dlg.widget, "fr_label_value_a_2" ) 				) == NULL )	goto EXIT;
		else if( ( fr_label_value_b2 	= lookup_widget( fr_dlg.widget, "fr_label_value_b_2" ) 				) == NULL )	goto EXIT;
		else if( ( fr_label_value_a3 	= lookup_widget( fr_dlg.widget, "fr_label_value_a_3" ) 				) == NULL )	goto EXIT;
		else if( ( fr_label_value_b3 	= lookup_widget( fr_dlg.widget, "fr_label_value_b_3" ) 				) == NULL )	goto EXIT;
		else{
			CnmsColAdjDialogInfoInit( CNMS_CA_DIALOG_FR );
			CnmsFRReset();
		}
	}

	/* Create & Initialize Monochrome Dialog	*/
	if( mc_dlg.widget == CNMSNULL ){
		if(      ( mc_dlg.widget 	= create_dialog_monochrome() ) == CNMSNULL )										goto EXIT;
		else if( ( mc_dlg.darea	 		= lookup_widget( mc_dlg.widget, "mc_drawingarea" ) 					) == NULL )	goto EXIT;
		else if( ( mc_spinbutton	 	= lookup_widget( mc_dlg.widget, "mc_spinbutton" ) 					) == NULL )	goto EXIT;
		else if( ( mc_scale	 		= lookup_widget( mc_dlg.widget, "mc_scale" ) 							) == NULL )	goto EXIT;
		else{
			CnmsColAdjDialogInfoInit( CNMS_CA_DIALOG_MC );
			CnmsMCInit();
		}
	}

	CnmsColAdjParamInit( CNMSNULL, CNMSNULL );
	ca_info.colorgroup	= CNMS_CA_COLORGROUP_COLOR;
	ca_info.page		= 0;
	ca_info.preview		= CNMS_FALSE;
	ca_info.clearcbox	= CNMS_TRUE;
	ca_info.reloadhist	= CNMS_TRUE;

	ret = CNMS_NO_ERR;
EXIT:

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjInit ]\n" );
#endif

	return ret;
}	


/***   Drawing Graph Base ( Frame and Line )   ***/
CNMSInt32 CnmsColAdjDrawAreaBaseDraw(
						GtkWidget			*widget,
						GdkPixmap			*pixmap,
						GdkGC				*gc						)
{
	CNMSInt32	ret		= CNMS_ERR,
				Left	= CNMS_CA_GRAPH_FRAME_LEFT,
				Top		= CNMS_CA_GRAPH_FRAME_TOP,
				Right	= CNMS_CA_GRAPH_FRAME_RIGHT,
				Bottom	= CNMS_CA_GRAPH_FRAME_BOTTOM;

	if( ( widget == CNMSNULL ) || ( pixmap == CNMSNULL ) || ( gc == CNMSNULL ) ){
		DBGMSG( "[CnmsColAdjDrawAreaBaseDraw] Arg Pointer is NULL. widget[%p], pixmap[%p], gc[%p]\n",widget,pixmap,gc );
		goto EXIT;
	}		

	/* Drawing Back ground		*/
	CnmsColAdjGdkGCSetColor( gc, ca_bg_color.red, ca_bg_color.green, ca_bg_color.blue );
	gdk_draw_rectangle( pixmap, gc, CNMS_TRUE, 0, 0, widget->allocation.width, widget->allocation.height );

	/* Drawing Frame outside	*/
	CnmsColAdjGdkGCSetColor( gc, CNMS_CA_GC_FRAME_R, CNMS_CA_GC_FRAME_G, CNMS_CA_GC_FRAME_B );
	gdk_draw_rectangle( pixmap, gc, CNMS_FALSE, Left, Top, Right - Left, Bottom - Top );
								
	/* Drawing Frame inside	*/
	gdk_draw_line( pixmap, gc, ( ( Right - Left ) * 1 / 4 ) + Left, Top, ( ( Right - Left ) * 1 / 4 ) + Left, Bottom ); 	/* Vertical line 1st from leftside	*/
	gdk_draw_line( pixmap, gc, ( ( Right - Left ) * 2 / 4 ) + Left, Top, ( ( Right - Left ) * 2 / 4 ) + Left, Bottom ); 	/* Vertical line 2nd from leftside	*/
	gdk_draw_line( pixmap, gc, ( ( Right - Left ) * 3 / 4 ) + Left, Top, ( ( Right - Left ) * 3 / 4 ) + Left, Bottom ); 	/* Vertical line 3rd from leftside	*/
	gdk_draw_line( pixmap, gc,   Left,  ( ( Bottom - Top ) * 1 / 4 ) + Top, Right, ( ( Bottom - Top ) * 1 / 4 ) +Top ); 	/* Horizontal line 1st from topside	*/
	gdk_draw_line( pixmap, gc, 	 Left,  ( ( Bottom - Top ) * 2 / 4 ) + Top, Right, ( ( Bottom - Top ) * 2 / 4 ) + Top ); 	/* Horizontal line 2nd from topside	*/
	gdk_draw_line( pixmap, gc,	 Left,  ( ( Bottom - Top ) * 3 / 4 ) + Top, Right, ( ( Bottom - Top ) * 3 / 4 ) + Top ); 	/* Horizontal line 3rd from topside	*/

	ret = CNMS_NO_ERR;
EXIT:

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjDrawAreaBaseDraw ]\n" );
#endif

	return ret;
}


/***   Drawing ToneCurve on Graph without Graph Base   ***/
CNMSInt32 CnmsColAdjDrawAreaCurveDraw(
						GdkPixmap			*pixmap,
						GdkGC				*gc,
						CNMSInt32			curve[],
						CNMSInt32			color					)
{
	CNMSInt32	i		= 0,
				ret		= CNMS_ERR,
				VecLen	= CNMS_CA_VECTOR_LEN,
				Left	= CNMS_CA_GRAPH_FRAME_LEFT,
				Top		= CNMS_CA_GRAPH_FRAME_TOP,
				Right	= CNMS_CA_GRAPH_FRAME_RIGHT,
				Bottom	= CNMS_CA_GRAPH_FRAME_BOTTOM;

	CNMSInt32	DispCurve[VecLen],
				DispCurveMaxX = Right - Left - 2,
				X,
				Xp1,
				Y,
				Yp1;
	CNMSDec32	Xdash,
				Ydash;
	
	if( ( pixmap == CNMSNULL ) || ( gc == CNMSNULL ) ){
		DBGMSG( "[CnmsColAdjDrawAreaCurveDraw] Arg Pointer is NULL. pixmap[%p], gc[%p]\n",pixmap,gc );
		goto EXIT;
	}		

	/* Set line color */
	switch( color ){
		case CNMS_CA_COLOR_BLACK:	CnmsColAdjGdkGCSetColor( gc, CNMS_CA_GC_VALUE_ZERO, CNMS_CA_GC_VALUE_ZERO, CNMS_CA_GC_VALUE_ZERO );	break;		/* Draw black line	*/
		case CNMS_CA_COLOR_RED:		CnmsColAdjGdkGCSetColor( gc,  CNMS_CA_GC_VALUE_MAX, CNMS_CA_GC_VALUE_ZERO, CNMS_CA_GC_VALUE_ZERO );	break;		/* Draw   red line	*/
		case CNMS_CA_COLOR_GREEN:	CnmsColAdjGdkGCSetColor( gc, CNMS_CA_GC_VALUE_ZERO,  CNMS_CA_GC_VALUE_MAX, CNMS_CA_GC_VALUE_ZERO );	break;		/* Draw green line	*/
		case CNMS_CA_COLOR_BLUE:	CnmsColAdjGdkGCSetColor( gc, CNMS_CA_GC_VALUE_ZERO, CNMS_CA_GC_VALUE_ZERO,  CNMS_CA_GC_VALUE_MAX );	break;		/* Draw  blue line	*/
		case CNMS_CA_COLOR_GRAY:	CnmsColAdjGdkGCSetColor( gc, CNMS_CA_GC_VALUE_GRAY, CNMS_CA_GC_VALUE_GRAY, CNMS_CA_GC_VALUE_GRAY );	break;		/* Draw  gray line	*/
		default:					DBGMSG("[CnmsColAdjDrawAreaCurveDraw] Arg color is invalid. color[%d]\n",color);			goto	EXIT;
	}
	
	/* Converting Data range from Real level(256) into Display level */
	for( i = 0 ; i <= DispCurveMaxX ; i++ ){
		Xdash = ( CNMSDec32 )( i * ( VecLen - 1 ) ) / ( CNMSDec32 )( DispCurveMaxX );

		X = ( CNMSInt32 )( Xdash );
		Xp1 = X + 1;
		Y = curve[X];
		if( Xp1 < VecLen ){
			Yp1 = curve[Xp1];
		}
		else{
			Yp1 = Y;
		}

		Ydash = ( CNMSDec32 )( ( ( CNMSDec32 )( Yp1 - Y ) * ( Xdash - ( CNMSDec32 )X ) ) + ( CNMSDec32 )( Y ) );
		DispCurve[i] = ( CNMSInt32 )( ( Ydash * ( CNMSDec32 )( DispCurveMaxX ) / ( CNMSDec32 )( VecLen - 1 ) ) + 0.5 );
	}

	/* Draw curve line */
	for( i = 0 ; i < DispCurveMaxX ; i++ ){
		gdk_draw_line( pixmap, gc, i + Left + 1, abs( DispCurve[ i ] - Bottom + 1 ), i + Left + 2, abs( DispCurve[ i + 1 ] - Bottom + 1 ) );
	}
							
	ret = CNMS_NO_ERR;
EXIT:

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjDrawAreaCurveDraw ]\n" );
#endif

	return ret;
}


/***   Drawing Histogram on Graph without Graph Base   ***/
CNMSInt32 CnmsColAdjDrawAreaHistogramDraw(
						GdkPixmap			*pixmap,
						GdkGC				*gc,
						CNMSUInt32			histogram[]				)
{
	CNMSInt32	i		= 0,
				ret		= CNMS_ERR,
				VecLen	= CNMS_CA_VECTOR_LEN,
				Left	= CNMS_CA_GRAPH_FRAME_LEFT,
				Top		= CNMS_CA_GRAPH_FRAME_TOP,
				Right	= CNMS_CA_GRAPH_FRAME_RIGHT,
				Bottom	= CNMS_CA_GRAPH_FRAME_BOTTOM;

	if( ca_info.preview == CNMS_FALSE ){
		ret = CNMS_NO_ERR;
		goto EXIT;
	}

	if( ( pixmap == CNMSNULL ) || ( gc == CNMSNULL ) ){
		DBGMSG( "[CnmsColAdjDrawAreaHistogramDraw] Arg Pointer is NULL. pixmap[%p], gc[%p]\n",pixmap,gc );
		goto EXIT;
	}		

	/* Set Histogram color (Black only) */
	CnmsColAdjGdkGCSetColor( gc, CNMS_CA_GC_VALUE_ZERO, CNMS_CA_GC_VALUE_ZERO, CNMS_CA_GC_VALUE_ZERO );

	/* Draw histogram line */
	for( i = 0 ; i < VecLen ; i++ ){
		if( histogram[i] != 0 )
		{
			CnmsColAdjCurveValueNormalize( histogram, VecLen, VecLen-1 );

			for( i = 0 ; i < VecLen ; i++ )
			{
				gdk_draw_line( pixmap, gc,
								( CNMSInt32 )( ( ( CNMSDec32 )i / ( VecLen - 1 ) * ( Right - Left - 1 ) ) - 0.5 ) + ( Left + 1 ),						/* Start X */
								( Bottom - 1 ),																											/* Start Y */
								( CNMSInt32 )( ( ( CNMSDec32 )i / ( VecLen - 1 ) * ( Right - Left - 1 ) ) - 0.5 ) + ( Left + 1 ),						/* End   X */
								abs( ( CNMSInt32 )( ( ( CNMSDec32 )histogram[i] * ( Bottom - Top - 1 ) / ( VecLen - 1 ) ) - 0.5 ) - ( Bottom - 1 ) ) );	/* End   Y */
			}
			break;
		}
	}

	ret = CNMS_NO_ERR;
EXIT:

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjDrawAreaHistogramDraw ]\n" );
#endif

	return ret;
}


/***   ColorMode Changed ( & Move Page to Advanced )   ***/
CNMSInt32 CnmsColAdjColorModeChanged(
						CNMSVoid									)
{
	static CNMSInt32 	colorBefore		= CNMS_CA_COLORGROUP_COLOR;		/* ColorGroup Before ColorMode Changed	*/
	CNMSInt32			colorAfter		= CNMS_CA_COLORGROUP_ERROR,		/* ColorGroup Current ColorMode Changed	*/
						ret				= CNMS_ERR;

	ca_info.reloadhist = CNMS_FALSE;

	if( ( colorAfter = CnmsColAdjGetColorGroupNum() ) == CNMS_CA_COLORGROUP_ERROR ){
		DBGMSG( "[CnmsColAdjColorModeChanged] colorAfter value [%d] is invalid.\n",colorAfter );
		goto EXIT;
	}		
	if( ( bc_dlg.widget == CNMSNULL ) || ( hg_dlg.widget == CNMSNULL ) || ( tc_dlg.widget == CNMSNULL ) || ( fr_dlg.widget == CNMSNULL ) || ( mc_dlg.widget == CNMSNULL ) ){
		DBGMSG( "[CnmsColAdjColorModeChanged] Dialog Pointer is NULL.\n" );
		goto EXIT;
	}		
		
	/* Case Changed between Same ColorGroup, No Reset Value and Disp	*/
	if( colorBefore == colorAfter ){
		ret = CNMS_NO_ERR;
		goto EXIT;
	}
	ca_info.colorgroup = colorAfter;

	/* If Changed between differnce Group, Data Reset */
	CnmsColAdjAllDataReset();

	/* Advanced mode only */
	if( ca_info.page == NOTEBOOK_PAGE_ADVANCED )
	{
		switch( colorAfter )
		{
			case	CNMS_CA_COLORGROUP_COLOR:
			case	CNMS_CA_COLORGROUP_GRAY:
						/* Not MC Dialog Show if the Dialog with showing status is Hiden */
						if( bc_dlg.show == CNMS_TRUE ){
							CnmsBCDialogShow( colorAfter, GTK_WIDGET_VISIBLE( bc_dlg.widget ) );
						}
						if( hg_dlg.show == CNMS_TRUE ){
							CnmsHGDialogShow( colorAfter, GTK_WIDGET_VISIBLE( hg_dlg.widget ) );
						}
						if( tc_dlg.show == CNMS_TRUE ){
							CnmsTCDialogShow( colorAfter, GTK_WIDGET_VISIBLE( tc_dlg.widget ) );
						}
						if( fr_dlg.show == CNMS_TRUE ){
							CnmsFRDialogShow( colorAfter, GTK_WIDGET_VISIBLE( fr_dlg.widget ) );
						}

						/* Dialog Hide and Get showing status if MC dialog is Shown */
						if( ( mc_dlg.show == CNMS_TRUE ) && ( GTK_WIDGET_VISIBLE( mc_dlg.widget ) == CNMS_TRUE ) ){
							CnmsMCDialogShow( colorAfter, CNMS_CA_DIALOG_NON_KEEP );
							mc_dlg.show = CNMS_TRUE;
						}
						break;
		
			case	CNMS_CA_COLORGROUP_MONO:
						if( ( bc_dlg.show == CNMS_TRUE ) ){
							CnmsBCDialogShow( colorAfter, CNMS_CA_DIALOG_NON_KEEP );
							bc_dlg.show = CNMS_TRUE;
						}
						if( ( hg_dlg.show == CNMS_TRUE ) ){
							CnmsHGDialogShow( colorAfter, CNMS_CA_DIALOG_NON_KEEP );
							hg_dlg.show = CNMS_TRUE;
						}
						if( ( tc_dlg.show == CNMS_TRUE ) ){
							CnmsTCDialogShow( colorAfter, CNMS_CA_DIALOG_NON_KEEP );
							tc_dlg.show = CNMS_TRUE;
						}
						if( ( fr_dlg.show == CNMS_TRUE ) ){
							CnmsFRDialogShow( colorAfter, CNMS_CA_DIALOG_NON_KEEP );
							fr_dlg.show = CNMS_TRUE;
						}
						if( ( mc_dlg.show == CNMS_TRUE ) ){
							CnmsMCDialogShow( colorAfter, CNMS_CA_DIALOG_NON_KEEP );
						}
						break;
		
			default:
						DBGMSG( "[CnmsColAdjColorModeChanged] colorAfter value [%d] is invalid.\n",colorAfter );
						goto EXIT;
		}
	}

	/* Keep current color group */
	colorBefore = colorAfter;
	
	ret = CNMS_NO_ERR;
EXIT:
	if( ca_info.page == NOTEBOOK_PAGE_SIMPLE ){
		bc_dlg.show = hg_dlg.show = tc_dlg.show = fr_dlg.show = mc_dlg.show = CNMS_FALSE;
	}

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjColorModeChanged ]\n" );
#endif
	ca_info.reloadhist = CNMS_TRUE;

	return ret;
}


/***   Dialog Button on Main Window Clicked   ***/
CNMSVoid CnmsColAdjButtonClicked(
						CNMSInt32			ButtonNum				)
{
	CNMSInt32		colorGroup	= CNMS_CA_COLORGROUP_ERROR;
	
	if( ( colorGroup = CnmsColAdjGetColorGroupNum() ) != CNMS_CA_COLORGROUP_ERROR )
	{
		switch( ButtonNum ){
			case	CNMS_CA_DIALOG_BC:		CnmsBCDialogShow( colorGroup, CNMS_CA_DIALOG_NON_KEEP );	break;
			case	CNMS_CA_DIALOG_HG:		CnmsHGDialogShow( colorGroup, CNMS_CA_DIALOG_NON_KEEP );	break;
			case	CNMS_CA_DIALOG_TC:		CnmsTCDialogShow( colorGroup, CNMS_CA_DIALOG_NON_KEEP );	break;
			case	CNMS_CA_DIALOG_FR:		CnmsFRDialogShow( colorGroup, CNMS_CA_DIALOG_NON_KEEP );	break;
			case	CNMS_CA_DIALOG_MC:		CnmsMCDialogShow( colorGroup, CNMS_CA_DIALOG_NON_KEEP );	break;
			default:						DBGMSG( "[CnmsColAdjButtonClicked] ButtonNum value [%d] is invalid.\n",ButtonNum );
											break;
		}
	}

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjButtonClicked ]\n" );
#endif

	return;
}


/***   All Dialog Parameter Reset only ( dialog No hide )   ***/
CNMSVoid CnmsColAdjAllDataReset(
						CNMSVoid									)
{
	if( ca_info.colorgroup != CNMS_CA_COLORGROUP_MONO ){
		CnmsBCReset();
		CnmsHGReset();
		CnmsTCReset();
		CnmsFRReset();
	}
	else{
		CnmsMCReset();
	}

	CnmsColAdjPreviewImageSet();
	
#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjAllDataReset ]\n" );
#endif

	return;
}


/***   All Dialog Parameter Reset with Dialog Hide   ***/
CNMSVoid CnmsColAdjResetComplete(
						CNMSVoid									)
{
	if( GTK_WIDGET_VISIBLE( bc_dlg.widget ) == CNMS_TRUE ){
		CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_BC, CNMS_FALSE );
	}
	if( GTK_WIDGET_VISIBLE( hg_dlg.widget ) == CNMS_TRUE ){
		CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_HG, CNMS_FALSE );
	}
	if( GTK_WIDGET_VISIBLE( tc_dlg.widget ) == CNMS_TRUE ){
		CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_TC, CNMS_FALSE );
	}
	if( GTK_WIDGET_VISIBLE( fr_dlg.widget ) == CNMS_TRUE ){
		CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_FR, CNMS_FALSE );
	}
	if( GTK_WIDGET_VISIBLE( mc_dlg.widget ) == CNMS_TRUE ){
		CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_MC, CNMS_FALSE );
	}

	CnmsColAdjAllDataReset();

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjResetComplete ]\n" );
#endif

	return;
}


/***   Dialog Show or Hide with Keep Position (True is show, False is hide )   ***/
CNMSInt32 CnmsColAdjDialogShowHide(
						CNMSInt32			id,
						CNMSBool			show					)
{
	CNMSInt32		ret			= CNMS_ERR;
	CNMSCADialog	*dlg		= CNMSNULL;

	if( ( CnmsColAdjGetDialogInfo( id, &dlg ) ) != CNMS_NO_ERR ){
		DBGMSG( "[CnmsColAdjDialogShowHide] Arg Value is Invalid. id[%d]\n", id );
		goto EXIT;
	}

	if( show == CNMS_TRUE ){
		gtk_window_move( GTK_WINDOW( dlg->widget ), dlg->x, dlg->y );
		gtk_widget_show( dlg->widget );
		dlg->show = show;
	}
	else{
		gtk_window_get_position( GTK_WINDOW( dlg->widget ), &(dlg->x), &(dlg->y) );
		gtk_widget_hide( dlg->widget );
		dlg->show = show;
		W1_HideAndShowAllButton();
	}

	ret = CNMS_NO_ERR;
EXIT:

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjDialogShowHide ]\n" );
#endif

	return ret;
}


/***   Histogram Value Set   ***/
CNMSInt32 CnmsColAdjSetHistogram(
						CNMSUInt32			type,
						CNMSUInt32			masterSrc[],
						CNMSUInt32			redSrc[],
						CNMSUInt32			greenSrc[],
						CNMSUInt32			blueSrc[],
						CNMSUInt32			graySrc[]				)
{
	CNMSInt32	i		= 0,
				ret		= CNMS_ERR;

	if( masterSrc == CNMSNULL ){
		DBGMSG( "[CnmsColAdjSetHistogram] Arg Pointer is NULL. masterSrc[%p]\n",masterSrc );
		goto EXIT;
	}		

	switch( type ){
		case CNMS_CA_DIALOG_HG:
					CnmsColAdjCopyHistogram( masterSrc, redSrc, greenSrc, blueSrc, graySrc,
							hg_dlg.hist[CNMS_CA_MASTER], hg_dlg.hist[CNMS_CA_RED], hg_dlg.hist[CNMS_CA_GREEN], hg_dlg.hist[CNMS_CA_BLUE], hg_dlg.hist[CNMS_CA_GRAY] );
					break;
					
		case CNMS_CA_DIALOG_FR:
					CnmsColAdjCopyHistogram( masterSrc, redSrc, greenSrc, blueSrc, graySrc,
							fr_dlg.hist[CNMS_CA_MASTER], fr_dlg.hist[CNMS_CA_RED], fr_dlg.hist[CNMS_CA_GREEN], fr_dlg.hist[CNMS_CA_BLUE], fr_dlg.hist[CNMS_CA_GRAY] );
					break;
					
		case CNMS_CA_DIALOG_MC:
					CnmsColAdjCopyHistogram( masterSrc, CNMSNULL, CNMSNULL, CNMSNULL, CNMSNULL, mc_dlg.hist[CNMS_CA_MASTER], CNMSNULL, CNMSNULL, CNMSNULL, CNMSNULL	);
					break;
					
		default:
			DBGMSG( "[CnmsColAdjSetHistogram] Arg value is invalid. type[%d]\n",type );
			goto EXIT;
	}		

	ret = CNMS_NO_ERR;
EXIT:

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjSetHistogram ]\n" );
#endif

	return ret;
}


/***   Histogram Value Set   ***/
static CNMSInt32 CnmsColAdjCopyHistogram(
						CNMSUInt32			masterSrc[],
						CNMSUInt32			redSrc[],
						CNMSUInt32			greenSrc[],
						CNMSUInt32			blueSrc[],
						CNMSUInt32			graySrc[],
						CNMSUInt32			masterDst[],
						CNMSUInt32			redDst[],
						CNMSUInt32			greenDst[],
						CNMSUInt32			blueDst[],
						CNMSUInt32			grayDst[]				)
{
	CNMSInt32	i		= 0,
				ret		= CNMS_ERR;

	if( ( masterSrc == CNMSNULL ) || ( masterDst == CNMSNULL ) ){
		DBGMSG( "[CnmsColAdjCopyHistogram] Arg Pointer is NULL. masterSrc[%p], masterDst[%p]\n",masterSrc,masterDst );
		goto EXIT;
	}		

	/* Master/Monochrome Value Copy Src to Dst	*/
	for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ){
		masterDst[i] = masterSrc[i];
	}

	/* Other Value Copy Src to Dst	*/
	if( ( redDst != CNMSNULL ) && ( greenDst != CNMSNULL ) && ( blueDst != CNMSNULL ) && ( grayDst != CNMSNULL ) &&
		( redSrc != CNMSNULL ) && ( greenSrc != CNMSNULL ) && ( blueSrc != CNMSNULL ) && ( graySrc != CNMSNULL ) )
	{
		for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ )
		{
			redDst[i]	= redSrc[i];
			greenDst[i]	= greenSrc[i];
			blueDst[i]	= blueSrc[i];
			grayDst[i]	= graySrc[i];
		}
	}
	else if( ( redDst != CNMSNULL ) || ( greenDst != CNMSNULL ) || ( blueDst != CNMSNULL ) || ( grayDst != CNMSNULL ) ||
			 ( redSrc != CNMSNULL ) || ( greenSrc != CNMSNULL ) || ( blueSrc != CNMSNULL ) || ( graySrc != CNMSNULL ) )
	{
		DBGMSG( "[CnmsColAdjCopyHistogram] Some Pointer is not NULL. redDst[%p], greenDst[%p], blueDst[%p], grayDst[%p], ",redDst,greenDst, blueDst, grayDst );
		DBGMSG( "redSrc[%p], greenSrc[%p], blueSrc[%p], graySrc[%p]\n",redSrc,greenSrc, blueSrc, graySrc );
		goto EXIT;
	}		

	ret = CNMS_NO_ERR;
EXIT:

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjCopyHistogram ]\n" );
#endif

	return ret;
}


/***   Get ColorGroup Number from Advanced ColorMode   ***/
CNMSInt32 CnmsColAdjGetColorGroupNum(
						CNMSVoid									)
{
	CNMSByte		*str		= CNMSNULL;
	CNMSInt32		ret			= CNMS_ERR;
	GtkWidget		*ComboBox	= CNMSNULL;

	if( main_window == CNMSNULL ){
		DBGMSG( "[CnmsColAdjGetColorGroupNum] Pointer[main_window] is NULL.\n" );
		goto	EXIT;
	}
	if( ( ComboBox = lookup_widget( main_window, "advance_color_combo" ) ) == CNMSNULL ){
		DBGMSG( "[CnmsColAdjGetColorGroupNum] Pointer[ComboBox] is NULL.\n" );
		goto	EXIT;
	}
	if( ( str = gtk_combo_box_get_active_text( GTK_COMBO_BOX( ComboBox ) ) ) == CNMSNULL ){
		DBGMSG( "[CnmsColAdjGetColorGroupNum] Pointer[str] is NULL.\n" );
		goto	EXIT;
	}

	/* Select Group from ColorMode str */
	     if( CnmsStrCompare( str, CnmsChgValToStr( CNMS_A_COLOR_MODE_COLOR          , CNMS_OBJ_A_COLOR_MODE ) ) == CNMS_NO_ERR ){
		ret = CNMS_CA_COLORGROUP_COLOR;
	}
	else if( CnmsStrCompare( str, CnmsChgValToStr( CNMS_A_COLOR_MODE_COLOR_DOCUMENTS, CNMS_OBJ_A_COLOR_MODE ) ) == CNMS_NO_ERR ){
		ret = CNMS_CA_COLORGROUP_COLOR;
	}
	else if( CnmsStrCompare( str, CnmsChgValToStr( CNMS_A_COLOR_MODE_GRAY           , CNMS_OBJ_A_COLOR_MODE ) ) == CNMS_NO_ERR ){
		ret = CNMS_CA_COLORGROUP_GRAY;
	}
	else if( CnmsStrCompare( str, CnmsChgValToStr( CNMS_A_COLOR_MODE_MONO           , CNMS_OBJ_A_COLOR_MODE ) ) == CNMS_NO_ERR ){
		ret = CNMS_CA_COLORGROUP_MONO;
	}
	else{
		ret = CNMS_CA_COLORGROUP_ERROR;
	}

EXIT:

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjGetColorGroupNum ]\n" );
#endif

	return ret;
}


/***   ComboBox Item All Clear   ***/
CNMSVoid CnmsColAdjComboBoxItemAllClear(
						GtkComboBox			*combobox				)
{
	CNMSInt32		i		= 0;
	
	ca_info.clearcbox = CNMS_TRUE;
	ComGtk_ClearCombo( combobox );
	ca_info.clearcbox = CNMS_FALSE;

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjComboBoxItemAllClear ]\n" );
#endif

	return;
}


/***   Set GdkGC Color from RGB Value   ***/
CNMSVoid CnmsColAdjGdkGCSetColor(
						GdkGC				*gc,
						CNMSUInt16			r,
						CNMSUInt16			g,
						CNMSUInt16			b						)
{
	GdkColor		color;

	if( gc == CNMSNULL ){
		DBGMSG( "[CnmsColAdjGdkGCSetColor] Arg Pointer[gc] is NULL.\n" );
		return;
	}

	color.red = r;
	color.green = g;
	color.blue = b;
	if( ( gdk_colormap_alloc_color( gdk_colormap_get_system(), &color, CNMS_TRUE, CNMS_FALSE ) ) != CNMS_TRUE )	return;
	gdk_gc_set_foreground( gc, &color );

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjGdkGCSetColor ]\n" );
#endif

	return;
}


/***   Calculate ToneCurve from Brigtness/Contrast Value   ***/
CNMSVoid CnmsColAdjCurveCalcBC(
						CNMSMAKETONEPARAM	*base_param				)
{
	CNMSMAKETONEPARAM	param;
	CNMSRGB32			usertone[CNMS_CA_VECTOR_LEN];
	CNMSRGB32			resulttone[CNMS_CA_VECTOR_LEN];
	CNMSInt32			i = 0;
	
	if( base_param == CNMSNULL ){
		DBGMSG( "[CnmsColAdjCurveCalcBC] Arg Pointer is NULL. base_param[%p]\n", base_param );
		return;
	}

	CnmsColAdjParamInit( &param, usertone );

	param.brightness.r = base_param->brightness.r;
	param.brightness.g = base_param->brightness.g;
	param.brightness.b = base_param->brightness.b;

	param.contrast.r = base_param->contrast.r;
	param.contrast.g = base_param->contrast.g;
	param.contrast.b = base_param->contrast.b;

	lpCnmsImgApi->p_CnmsImgMakeToneCurve( &param, usertone, resulttone );
	
	for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ){
		bc_dlg.curve[CNMS_CA_RED][i]	= resulttone[i].r;
		bc_dlg.curve[CNMS_CA_GREEN][i]	= resulttone[i].g;
		bc_dlg.curve[CNMS_CA_BLUE][i]	= resulttone[i].b;
	}
	
	CnmsColAdjParamInit( &param, usertone );
	
	param.brightness.m = base_param->brightness.m;
	param.contrast.m   = base_param->contrast.m;

	lpCnmsImgApi->p_CnmsImgMakeToneCurve( &param, usertone, resulttone );

	for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ){
		bc_dlg.curve[CNMS_CA_MASTER][i] = bc_dlg.curve[CNMS_CA_GRAY][i] = resulttone[i].r;
	}

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjCurveCalcBC ]\n" );
#endif

	return;
}


/***   Calculate ToneCurve from Shadow/Mid/Highlight Value   ***/
CNMSVoid CnmsColAdjCurveCalcHG(
						CNMSMAKETONEPARAM	*base_param				)
{
	CNMSMAKETONEPARAM	param;
	CNMSRGB32			usertone[CNMS_CA_VECTOR_LEN];
	CNMSRGB32			resulttone[CNMS_CA_VECTOR_LEN];
	CNMSInt32			i = 0;

	if( base_param == CNMSNULL ){
		DBGMSG( "[CnmsColAdjCurveCalcHG] Arg Pointer is NULL. base_param[%p]\n", base_param );
		return;
	}

	CnmsColAdjParamInit( &param, usertone );
	
	param.blackpoint.r = base_param->blackpoint.r;
	param.blackpoint.g = base_param->blackpoint.g;
	param.blackpoint.b = base_param->blackpoint.b;

	param.midpoint.r = base_param->midpoint.r;
	param.midpoint.g = base_param->midpoint.g;
	param.midpoint.b = base_param->midpoint.b;

	param.whitepoint.r = base_param->whitepoint.r;
	param.whitepoint.g = base_param->whitepoint.g;
	param.whitepoint.b = base_param->whitepoint.b;

	param.graypoint.r = base_param->graypoint.r;
	param.graypoint.g = base_param->graypoint.g;
	param.graypoint.b = base_param->graypoint.b;

	lpCnmsImgApi->p_CnmsImgMakeToneCurve( &param, usertone, resulttone );
	
	for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ){
		hg_dlg.curve[CNMS_CA_RED][i]	= resulttone[i].r;
		hg_dlg.curve[CNMS_CA_GREEN][i]	= resulttone[i].g;
		hg_dlg.curve[CNMS_CA_BLUE][i]	= resulttone[i].b;
	}
	
	CnmsColAdjParamInit( &param, usertone );
	
	param.blackpoint.m = base_param->blackpoint.m;
	param.midpoint.m = base_param->midpoint.m;
	param.whitepoint.m = base_param->whitepoint.m;

	lpCnmsImgApi->p_CnmsImgMakeToneCurve( &param, usertone, resulttone );

	for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ){
		hg_dlg.curve[CNMS_CA_MASTER][i] = hg_dlg.curve[CNMS_CA_GRAY][i] = resulttone[i].r;
	}

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjCurveCalcHG ]\n" );
#endif

	return;
}


/***   Calculate ToneCurve from All ColorAdjust Parameter Value   ***/
CNMSVoid CnmsColAdjCurveCalcFR(
						CNMSVoid									)
{
	CNMSInt32			i			= 0;
	CNMSMAKETONEPARAM	*param		= &ca_param_all;
	CNMSRGB32			*usertone	= ca_usertone,
						*resulttone	= ca_resulttone;

	if( ( param == CNMSNULL ) && ( usertone == CNMSNULL ) && ( resulttone == CNMSNULL ) ){
		DBGMSG( "[CnmsColAdjCurveCalcFR] Arg Pointer is NULL. param[%p], usertone[%p], resulttone[%p]\n", param, usertone, resulttone );
		return;
	}

	lpCnmsImgApi->p_CnmsImgMakeToneCurve( param, usertone, resulttone );

	for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ){
		fr_dlg.curve[CNMS_CA_RED][i]	= resulttone[i].r;
		fr_dlg.curve[CNMS_CA_GREEN][i]	= resulttone[i].g;
		fr_dlg.curve[CNMS_CA_BLUE][i]	= resulttone[i].b;	
		fr_dlg.curve[CNMS_CA_GRAY][i]	= resulttone[i].r;
	}

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjCurveCalcFR ]\n" );
#endif

	return;
}


/***   Initialize All ColorAdjust Parameter Value   ***/
CNMSVoid CnmsColAdjParamInit(
						CNMSMAKETONEPARAM	*param,
						CNMSRGB32			*usertone				)
{
	CNMSInt32			i 						= 0;
	CNMSUInt8			tc_curve_out[CNMS_CA_VECTOR_LEN];
	CNMSUInt16			tc_curve_out16[CNMS_CA_VECTOR_LONGLEN];

	/* All arg pointer NULL is Static Prame Init	*/
	if( ( param == CNMSNULL ) && ( usertone == CNMSNULL ) ){
		param		= &ca_param_all;
		usertone	= ca_usertone;

		for( i = 0 ; i < CNMS_CA_VECTOR_LONGLEN ; i++ ){
			ca_usertone16[i].m   = ca_usertone16[i].r   = ca_usertone16[i].g   = ca_usertone16[i].b   = i;
		}
		lpCnmsImgApi->p_CnmsImgMakeToneCurve16( param, ca_usertone16, ca_resulttone16 );
	}
	/* Someone arg pointer NULL is Error			*/
	else if( !( ( param != CNMSNULL ) && ( usertone != CNMSNULL ) ) ){
		DBGMSG( "[CnmsColAdjParamInit] Arg Pointer is NULL. param[%p], usertone[%p]\n", param, usertone );
		return;
	}

	param->blackpoint.m = param->blackpoint.r = param->blackpoint.g = param->blackpoint.b =   0;
	param->midpoint.m   = param->midpoint.r   = param->midpoint.g   = param->midpoint.b   = 128;
	param->whitepoint.m = param->whitepoint.r = param->whitepoint.g = param->whitepoint.b = 255;
	param->graypoint.m  = param->graypoint.r  = param->graypoint.g  = param->graypoint.b  = 128;

	param->brightness.m = param->brightness.r = param->brightness.g = param->brightness.b = 128;
	param->contrast.m   = param->contrast.r   = param->contrast.g   = param->contrast.b   = 128;
	param->balance.m    = param->balance.r    = param->balance.g    = param->balance.b    = 128;
	param->gamma.m      = param->gamma.r      = param->gamma.g      = param->gamma.b      = 1.0;

	for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ){
		usertone[i].m   = usertone[i].r   = usertone[i].g   = usertone[i].b   = i;
	}

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjParamInit ]\n" );
#endif

	return;
}


/***   Preview Image Set From Change ColorAdjust Parameter   ***/
CNMSVoid CnmsColAdjPreviewImageSet(
						CNMSVoid									)
{
	Preview_ChangeStatus( CNMSSCPROC_ACTION_PARAM );

#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjPreviewImageSet ]\n" );
#endif

	return;
}


/***   Set UserTone Called From ScanMain   ***/
CNMSVoid CnmsColAdjSetUserTone(
						CNMSUInt16			*R,
						CNMSUInt16			*G,
						CNMSUInt16			*B						)
{
	CNMSInt32	i	= 0;
	
	if( ( ca_usertone16 == CNMSNULL ) || ( ca_resulttone16 == CNMSNULL ) ){
		DBGMSG( "[CnmsColAdjSetUserTone] Pointer is NULL. ca_usertone16[%p], ca_resulttone16[%p]\n", ca_usertone16, ca_resulttone16 );
		return;
	}
	if( ( R == CNMSNULL ) || ( G == CNMSNULL ) || ( B == CNMSNULL ) ){
		DBGMSG( "[CnmsColAdjSetUserTone] Arg Pointer is NULL. R[%p], G[%p], B[%p]\n",R,G,B );
		return;
	}
	
	lpCnmsImgApi->p_CnmsImgMakeToneCurve16( &ca_param_all, ca_usertone16, ca_resulttone16 );

	for( i = 0 ; i < CNMS_CA_VECTOR_LONGLEN ; i++ ){
		R[ i ] = ( CNMSUInt16 )ca_resulttone16[ i ].r;
		G[ i ] = ( CNMSUInt16 )ca_resulttone16[ i ].g;
		B[ i ] = ( CNMSUInt16 )ca_resulttone16[ i ].b;
	}
	
#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjSetUserTone ]\n" );
#endif

	return;
}


/***   Initialized Default Threshold Value   ***/
CNMSInt32 CnmsColAdjInitThresholdDefault(
						CNMSVoid									)
{
	CNMSInt32	ret = CNMS_ERR,
				ldata;

	if( ( ldata = W1_JudgeFormatType( CNMS_OBJ_A_THRESHOLD_DEFAULT ) ) == CNMS_ERR ){
		DBGMSG( "[CnmsColAdjInitThresholdDefault] Function error [W1_JudgeFormatType() = %d].\n", ldata );
		goto EXIT;
	}

	if( ldata == CNMS_TRUE ){
		if( ( ldata = CnmsUiGetRealValue( CNMS_OBJ_A_THRESHOLD_DEFAULT, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
			DBGMSG( "[CnmsColAdjInitThresholdDefault] Function error [CnmsUiGetRealValue() = %d].\n", ldata );
			goto EXIT;
		}
		else{
			ret = ldata;
		}
	}
	else{
		ret = CNMS_CA_MC_DEFAULT;
	}
EXIT:
	return	ret;
}


/***   Dialog Position Initialize   ***/
CNMSVoid CnmsColAdjDialogInfoInit(
						CNMSInt32			id						)
{
	CNMSInt			WindowWidth		= 0,
					WindowHeight	= 0,
					DialogWidth		= 0,
					DialogHeight	= 0,
					WindowX			= 0,
					WindowY			= 0;
	CNMSInt32		DialogNum		= 0,
					i				= 0;
	CNMSCADialog	*dlg			= CNMSNULL;

	switch( id ){
		case CNMS_CA_DIALOG_BC:		dlg = &bc_dlg;		DialogNum = 1;		dlg->fcurve = CNMS_CA_COLOR_MAX;	dlg->fhist = 0;					break;
		case CNMS_CA_DIALOG_HG:		dlg = &hg_dlg;		DialogNum = 2;		dlg->fcurve = CNMS_CA_COLOR_MAX;	dlg->fhist = CNMS_CA_COLOR_MAX;	break;
		case CNMS_CA_DIALOG_TC:		dlg = &tc_dlg;		DialogNum = 3;		dlg->fcurve = CNMS_CA_COLOR_MAX;	dlg->fhist = 0;					break;
		case CNMS_CA_DIALOG_FR:		dlg = &fr_dlg;		DialogNum = 4;		dlg->fcurve = CNMS_CA_COLOR_MAX;	dlg->fhist = CNMS_CA_COLOR_MAX;	break;
		case CNMS_CA_DIALOG_MC:		dlg = &mc_dlg;		DialogNum = 1;		dlg->fcurve = 0;					dlg->fhist = 1;					break;
	}

	/* Get Main window Size & Position	*/
	gtk_window_get_position( GTK_WINDOW( main_window ), &WindowX, &WindowY );
	gtk_window_get_size( GTK_WINDOW( main_window ), &WindowWidth, &WindowHeight );
	
	gtk_window_get_size( GTK_WINDOW( dlg->widget ), &DialogWidth, &DialogHeight );
	dlg->x = WindowX + WindowWidth - DialogWidth - ( CNMS_CA_HEIGHT_TITLE_BAR * DialogNum );
	dlg->y = WindowY + ( CNMS_CA_HEIGHT_TITLE_BAR * DialogNum );

	dlg->id			= id;
	dlg->channel	= CNMS_CA_MASTER;
	dlg->pmap 		= CNMSNULL;
	dlg->gc 		= CNMSNULL;
	dlg->show		= CNMS_FALSE;

	for( i = 0 ; i < CNMS_CA_COLOR_MAX ; i++ ){
		dlg->curve[i] = CNMSNULL;
		dlg->hist[i] = CNMSNULL;
	}
	for( i = 0 ; i < dlg->fcurve ; i++ ){
		dlg->curve[i] = ( CNMSInt32* )CnmsGetMem( CNMS_CA_VECTOR_LEN * sizeof( CNMSInt32 ) );
	}
	for( i = 0 ; i < dlg->fhist ; i++ ){
		dlg->hist[i] = ( CNMSUInt32* )CnmsGetMem( CNMS_CA_VECTOR_LEN * sizeof( CNMSUInt32 ) );
	}

	return;
}

CNMSVoid CnmsColAdjClose(
						CNMSVoid									)
{
	CNMSInt32		id		= 0,
					i		= 0;
	CNMSCADialog	*dlg	= CNMSNULL;

	for( id = 0 ; id < CNMS_CA_DIALOG_MAX ; id++ ){
		if( ( CnmsColAdjGetDialogInfo( id, &dlg ) ) != CNMS_NO_ERR ){
			DBGMSG( "[CnmsColAdjClose] Arg Value is Invalid. id[%d]\n", id );
		}
		for( i = 0 ; i < CNMS_CA_COLOR_MAX ; i++ ){
			if( dlg->hist[i] != CNMSNULL ){
				CnmsFreeMem( (CNMSLPSTR)dlg->hist[i] );
			}
			if( dlg->curve[i] != CNMSNULL ){
				CnmsFreeMem( (CNMSLPSTR)dlg->curve[i] );
			}
		}
	}
	return;
}

/***   Normalize Histogram Value To Graph Height Max From Raw Value   ***/
static CNMSVoid CnmsColAdjCurveValueNormalize(
						CNMSUInt32			*curve,
						CNMSInt32			max_len,
						CNMSInt32			max_value				)
{
	CNMSInt32		max		= 0,
					i		= 0;

	if( curve == CNMSNULL ){
		DBGMSG( "[CnmsColAdjCurveValueNormalize] Arg Pointer[curve] is NULL.\n" );
		return;
	}

	for( i = 0 ; i < max_len ; i++ ){
		if( max < abs(curve[i]) ){
			max = abs(curve[i]);
		}
	}
	
	for( i = 0 ; i < max_len ; i++ ){
		curve[i] = ( CNMSUInt32 )( ( CNMSDec32 )abs( curve[i] ) * ( CNMSDec32 )max_value / ( CNMSDec32 )max );	
	}
	
#ifdef __COLOR_ADJUST_DEBUG__
	DBGMSG( "\t[ CnmsColAdjCurveValueNormalize ]\n" );
#endif

	return;
}


/***   Avoid Show Value "-0"   ***/
CNMSVoid CnmsColAdjSetZeroIfMinusZeroForSpinbutton(
						GtkWidget			*spinbutton				)
{

	if( spinbutton == CNMSNULL ){
		DBGMSG( "[CnmsColAdjSetZeroIfMinusZeroForSpinbutton] Arg Widget Pointer[spinbutton] is NULL.\n" );
		return;
	}
	
	if( strncmp( gtk_entry_get_text( GTK_ENTRY( spinbutton ) ), "-0", 2 ) == 0 ){		/* Avoid Show Value "-0"	*/
		gtk_spin_button_set_value( GTK_SPIN_BUTTON( spinbutton ), 1 );
		gtk_spin_button_set_value( GTK_SPIN_BUTTON( spinbutton ), 0 );
	}

	return;
}


CNMSVoid CnmsColAdjSetCurrentPage(
						CNMSInt32			pagenum					)
{
	ca_info.page = pagenum;
	if( pagenum < 0 ){
		ca_info.page = 0;
	}
	return;
}


/***   Drawing Pixmap Event for Hiden Pixmap Area   ***/
CNMSVoid CnmsColAdjDrawingAreaExposeEvent(
						GtkWidget			*widget,
						GdkEventExpose		*event,
						CNMSInt32			id						)
{
	CNMSCADialog	*dlg = CNMSNULL;

	if( ( widget == CNMSNULL ) || ( event == CNMSNULL ) ){
		DBGMSG( "[CnmsColAdjDrawingAreaExposeEvent] Arg Pointer is MULL. widget[%p], event[%p]\n", widget, event );
		return;
	}

	if( ( CnmsColAdjGetDialogInfo( id, &dlg ) ) != CNMS_NO_ERR ){
		DBGMSG( "[CnmsColAdjDrawingAreaExposeEvent] Arg Value is Invalid. id[%d]\n", id );
		return;
	}

	/* Drawing Pixmap	*/
	gdk_draw_pixmap( widget->window, widget->style->fg_gc[ GTK_WIDGET_STATE( widget ) ],
		dlg->pmap, event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height );
		
	return;
}


/***   Event for Drawing Graph   ***/
CNMSVoid CnmsColAdjDrawingAreaConfigureEvent(
						GtkWidget			*widget,
						GdkEventConfigure	*event,
						CNMSInt32			id						)
{
	CNMSCADialog	*dlg = CNMSNULL;

	if( ( widget == CNMSNULL ) || ( event == CNMSNULL ) ){
		DBGMSG( "[CnmsColAdjDrawingAreaConfigureEvent] Arg Pointer is MULL. widget[%p], event[%p]\n", widget, event );
		return;
	}

	if( ( CnmsColAdjGetDialogInfo( id, &dlg ) ) != CNMS_NO_ERR ){
		DBGMSG( "[CnmsColAdjDrawingAreaConfigureEvent] Arg Value is Invalid. id[%d]\n", id );
		return;
	}

	/* Create Pixmap			*/
	if ( dlg->pmap != CNMSNULL ){
		gdk_pixmap_unref( dlg->pmap );
	}
	dlg->pmap = gdk_pixmap_new( widget->window, widget->allocation.width, widget->allocation.height, -1 );

	/* Create Graphic Context	*/
	if( dlg->gc == CNMSNULL ){
		dlg->gc = gdk_gc_new( widget->window );
	}

	/* Draw ToneCurve			*/
	CnmsColAdjToneCurveDraw( id );

	return;
}


/***   Set Channel Combobox Item   ***/
CNMSBool CnmsColAdjChannelItemSet(
						CNMSInt32			color,
						CNMSInt32			id						)
{
	CNMSBool		ret		= CNMS_TRUE;
	CNMSCADialog	*dlg 	= CNMSNULL;

	if( ( CnmsColAdjGetDialogInfo( id, &dlg ) ) != CNMS_NO_ERR ){
		DBGMSG( "[CnmsColAdjChannelItemSet] Arg Value is Invalid. id[%d]\n", id );
		return;
	}
	if( ( color < 0 ) || ( color >= CNMS_CA_COLORGROUP_MAX ) ){
		DBGMSG( "[CnmsColAdjChannelItemSet] Arg Value is invalid. color[%d]\n",color );
		return;
	}

	switch( id ){
		case CNMS_CA_DIALOG_BC:
		case CNMS_CA_DIALOG_HG:
		case CNMS_CA_DIALOG_TC:
		case CNMS_CA_DIALOG_FR:
			switch( color ){
				case CNMS_CA_COLORGROUP_COLOR:
						CnmsColAdjComboBoxItemAllClear( GTK_COMBO_BOX( dlg->cbox ) );
						gtk_combo_box_append_text( GTK_COMBO_BOX( dlg->cbox ), gettext(ca_ch_litem_master) );
						gtk_combo_box_append_text( GTK_COMBO_BOX( dlg->cbox ), gettext(ca_ch_litem_red) );
						gtk_combo_box_append_text( GTK_COMBO_BOX( dlg->cbox ), gettext(ca_ch_litem_green) );
						gtk_combo_box_append_text( GTK_COMBO_BOX( dlg->cbox ), gettext(ca_ch_litem_blue) );
						break;
				case CNMS_CA_COLORGROUP_GRAY:
						CnmsColAdjComboBoxItemAllClear( GTK_COMBO_BOX( dlg->cbox ) );
						gtk_combo_box_append_text( GTK_COMBO_BOX( dlg->cbox ), gettext(ca_ch_litem_gray) );
						break;
				case CNMS_CA_COLORGROUP_MONO:
						if( GTK_WIDGET_VISIBLE( dlg->widget ) == CNMS_TRUE )
							CnmsColAdjDialogShowHide( id, CNMS_FALSE );
						ret = CNMS_FALSE;
						break;
			}
			break;

		case CNMS_CA_DIALOG_MC:
			switch( color ){
				case CNMS_CA_COLORGROUP_COLOR:
				case CNMS_CA_COLORGROUP_GRAY:
						if( GTK_WIDGET_VISIBLE( dlg->widget ) == CNMS_TRUE )
							CnmsColAdjDialogShowHide( id, CNMS_FALSE );
						ret = CNMS_FALSE;
						break;
				case CNMS_CA_COLORGROUP_MONO:
						break;
			}
			break;
	}

	return	ret;
}


/***   Return Dialog Information   ***/
static CNMSInt32 CnmsColAdjGetDialogInfo(
						CNMSInt32			id,
						CNMSCADialog		**dlg					)
{
	CNMSInt32		ret = CNMS_ERR;

	switch( id ){
		case CNMS_CA_DIALOG_BC	:	*dlg = &bc_dlg;					break;
		case CNMS_CA_DIALOG_HG	:	*dlg = &hg_dlg;					break;
		case CNMS_CA_DIALOG_TC	:	*dlg = &tc_dlg;					break;
		case CNMS_CA_DIALOG_FR	:	*dlg = &fr_dlg;					break;
		case CNMS_CA_DIALOG_MC	:	*dlg = &mc_dlg;					break;
						default	:	dlg = CNMSNULL;	goto EXIT_ERR;	break;
	}

	ret = CNMS_NO_ERR;
EXIT_ERR:
	return	ret;
}


CNMSVoid CnmsColAdjToneCurveDraw(
						CNMSInt32			id						)
{
	CNMSCADialog	*dlg		= CNMSNULL;

	if( ( CnmsColAdjGetDialogInfo( id, &dlg ) ) != CNMS_NO_ERR ){
		DBGMSG( "[CnmsColAdjToneCurveDraw] Arg Value is Invalid. id[%d]\n", id );
		return;
	}
	if( GTK_WIDGET_VISIBLE( dlg->widget ) != CNMS_TRUE ){
		return;
	}
	if( ( dlg->fhist > 0 ) && ( ca_info.reloadhist == CNMS_FALSE ) ){
		return;
	}

	CnmsColAdjDrawAreaBaseDraw( dlg->widget, dlg->pmap, dlg->gc );

	switch( ca_info.colorgroup )
	{
		case CNMS_CA_COLORGROUP_COLOR:
			if( dlg->fhist > 0 ){
				CnmsColAdjDrawAreaHistogramDraw( dlg->pmap, dlg->gc, dlg->hist[dlg->channel] );
			}
			switch( dlg->channel )
			{
				case CNMS_CA_MASTER:
					if( dlg->fcurve > 0 ){
						if( dlg->id == CNMS_CA_DIALOG_FR ){
							CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_RED], CNMS_CA_COLOR_RED );
							CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_GREEN], CNMS_CA_COLOR_GREEN );
							CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_BLUE], CNMS_CA_COLOR_BLUE );
						}
						else{
							CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_MASTER], CNMS_CA_COLOR_GRAY );
						}
					}
					break;
				
				case CNMS_CA_RED:
					if( dlg->fcurve > 0 ){
						CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_GREEN], CNMS_CA_COLOR_GREEN );
						CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_BLUE], CNMS_CA_COLOR_BLUE );
						CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_RED], CNMS_CA_COLOR_RED );
					}
					break;
				
				case CNMS_CA_GREEN:
					if( dlg->fcurve > 0 ){
						CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_RED], CNMS_CA_COLOR_RED );
						CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_BLUE], CNMS_CA_COLOR_BLUE );
						CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_GREEN], CNMS_CA_COLOR_GREEN );
					}
					break;
				
				case CNMS_CA_BLUE:
					if( dlg->fcurve > 0 ){
						CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_RED], CNMS_CA_COLOR_RED );
						CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_GREEN], CNMS_CA_COLOR_GREEN );
						CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_BLUE], CNMS_CA_COLOR_BLUE );
					}
					break;
				
				default:
					DBGMSG( "[CnmsColAdjToneCurveDraw] Arg Value is invalid. channel[%d]\n", dlg->channel );
					break;
			}
			break;
			
		case CNMS_CA_COLORGROUP_GRAY:
			if( dlg->fhist > 0 ){
				CnmsColAdjDrawAreaHistogramDraw( dlg->pmap, dlg->gc, dlg->hist[CNMS_CA_GRAY] );
			}
			if( dlg->fcurve > 0 ){
				CnmsColAdjDrawAreaCurveDraw( dlg->pmap, dlg->gc, dlg->curve[CNMS_CA_GRAY], CNMS_CA_COLOR_GRAY );
			}
			break;
			
		case CNMS_CA_COLORGROUP_MONO:
			CnmsMCDrawThreshold( dlg->darea, dlg->hist[CNMS_CA_MASTER], mc_mono.threshold );
			break;

		default:
			DBGMSG( "[CnmsColAdjToneCurveDraw] Arg Value is invalid. colorGroup[%d]\n", ca_info.colorgroup );
			break;
	}

	gdk_draw_pixmap( dlg->darea->window, dlg->darea->style->fg_gc[GTK_WIDGET_STATE (dlg->darea)],
		dlg->pmap, 0, 0, 0, 0, dlg->darea->allocation.width, dlg->darea->allocation.height);

	if( id < CNMS_CA_DIALOG_FR ){
		CnmsColAdjCurveCalcFR();
		CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_FR );
	}

	return;
}


CNMSVoid CnmsColAdjInitCurveAndHistogram(
						CNMSCADialog		*dlg					)
{
	CNMSInt32	channel, i;

	for( channel = 0 ; channel < CNMS_CA_COLOR_MAX ; channel++ ){
		if( dlg->curve[channel] != CNMSNULL ){
			for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ){
				dlg->curve[channel][i] = i;
			}
		}
		if( ca_info.preview == CNMS_FALSE ){
			if( dlg->hist[channel] != CNMSNULL ){
				for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ){
					dlg->hist[channel][i] = 0;
				}
			}
		}
	}
	return;
}


CNMSVoid CnmsColAdjClearHistogram(
						CNMSVoid									)
{
	CNMSInt32	channel;

	for( channel = 0 ; channel < CNMS_CA_COLOR_MAX ; channel++ ){
		if( hg_dlg.hist[channel] != CNMSNULL ){
			CnmsSetMem( (CNMSLPSTR)( hg_dlg.hist[channel] ), 0, ( sizeof(CNMSUInt32) * CNMS_CA_VECTOR_LEN ) );
		}
		if( fr_dlg.hist[channel] != CNMSNULL ){
			CnmsSetMem( (CNMSLPSTR)( fr_dlg.hist[channel] ), 0, ( sizeof(CNMSUInt32) * CNMS_CA_VECTOR_LEN ) );
		}
		if( mc_dlg.hist[channel] != CNMSNULL ){
			CnmsSetMem( (CNMSLPSTR)( mc_dlg.hist[channel] ), 0, ( sizeof(CNMSUInt32) * CNMS_CA_VECTOR_LEN ) );
		}
	}

	return;
}

CNMSVoid CnmsColAdjGetPreviewStatus(
						CNMSBool			prevFlag				)
{
	ca_info.preview = prevFlag;
	return;
}


CNMSVoid CnmsColAdjUpDateHistogram(
						CNMSVoid									)
{
	if( GTK_WIDGET_VISIBLE( hg_dlg.widget ) == CNMS_TRUE ){
		CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_HG );
		CnmsHGPreviewChangedAfter();
	}
	if( GTK_WIDGET_VISIBLE( fr_dlg.widget ) == CNMS_TRUE ){
		CnmsColAdjCurveCalcFR();
		CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_FR );
	}
	if( GTK_WIDGET_VISIBLE( mc_dlg.widget ) == CNMS_TRUE ){
		CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_MC );
	}

	return;	
}


CNMSVoid CnmsColAdjSetChannelItem(
						CNMSCADialog		*dlg					)
{
	CNMSInt32	channel;

	channel = dlg->channel;
	if( dlg->channel == CNMS_CA_GRAY ){
		channel = CNMS_CA_MASTER;
	}

	gtk_combo_box_set_active( GTK_COMBO_BOX( dlg->cbox ), channel );

	return;
}


CNMSInt32 CnmsColAdjGetChannelIDFromCombobox(
						GtkComboBox			*combobox				)
{
	CNMSInt8	*channel	= CNMSNULL;
	CNMSInt32	ret			= CNMS_ERR;

	if( combobox == CNMSNULL ){
		DBGMSG( "[CnmsTCChannelComboBoxChanged] Arg Pointer[combobox] is NULL.\n" );
		goto EXIT;
	}

	/* Get Current Channel as String */
	if( ( channel = gtk_combo_box_get_active_text( combobox ) ) == CNMSNULL ){
		DBGMSG( "[CnmsTCChannelComboBoxChanged] Pointer[channel] is NULL.\n" );
		goto EXIT;
	}

	if		( CnmsStrCompare( channel, gettext( ca_ch_litem_master ) ) == CNMS_NO_ERR ){
		ret = CNMS_CA_MASTER;																/* Convert Current Channel to Number from String	*/
	}
	else if	( CnmsStrCompare( channel, gettext( ca_ch_litem_red ) )    == CNMS_NO_ERR ){
		ret = CNMS_CA_RED;
	}
	else if	( CnmsStrCompare( channel, gettext( ca_ch_litem_green ) )  == CNMS_NO_ERR ){
		ret = CNMS_CA_GREEN;
	}
	else if	( CnmsStrCompare( channel, gettext( ca_ch_litem_blue ) )   == CNMS_NO_ERR ){
		ret = CNMS_CA_BLUE;
	}
	else if	( CnmsStrCompare( channel, gettext( ca_ch_litem_gray ) )   == CNMS_NO_ERR ){
		ret = CNMS_CA_GRAY;
	}
	else{
		DBGMSG( "[CnmsTCChannelComboBoxChanged] Channel string[%s] is invalid.\n",channel );
		goto EXIT;
	}

EXIT:
	return	ret;
}

#endif	/* _COLORADJUST_C_ */

