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

#ifndef	_CALLBACKS_C_
#define	_CALLBACKS_C_

/*	#define	__CNMS_DEBUG_EVENT__	*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "cnmstype.h"
#include "cnmsdef.h"

#include "w1.h"
#include "prev_main.h"
#include "preference.h"
#include "save_dialog.h"
#include "com_gtk.h"

#include "progress_bar.h"
#include "coloradjust.h"
#include "child_dialog.h"

#ifdef	__GIMP_PLUGIN_ENABLE__
	#include "scangimp.h"
#endif

static CNMSBool	note_flag = CNMS_FALSE;

  /************************/
 /***** Select Model *****/
/************************/
void on_select_model_ok_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_select_model_ok_button_clicked ]]\n" );
#endif
	CNMSInt32	devIndex, ldata;
	GtkComboBox	*combobox;

	combobox = (GtkComboBox *)lookup_widget( select_model_dialog, "select_model_combo" );
	devIndex = gtk_combo_box_get_active( combobox );

	gtk_widget_hide( select_model_dialog );

	if( ( ldata = W1_Open( devIndex ) ) != CNMS_NO_ERR ) {
		/* show error message. */
		/*fprintf( stderr, "Usage :\n  Cannot find modules for selected Canon MFP scanner device.\n  Please install \"the model-specific package\".\n" );*/
		ShowErrorDialog();
		W1_Close();
	}
	return;
}

gboolean on_select_model_dialog_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_select_model_dialog_delete_event ]]\n" );
#endif
	if( GTK_WIDGET_SENSITIVE( select_model_dialog ) ){
		W1_Close();
	}
	
	return	TRUE;
}

void on_select_model_cancel_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_select_model_cancel_button_clicked ]]\n" );
#endif
	W1_Close();

	return;
}

void on_select_model_search_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_select_model_search_button_clicked ]]\n" );
#endif
	W1_ReloadSelectModel();
	
	return;
}


/***********************/
/***** main window *****/
/***********************/
void main_window_destroy(
		GtkObject		*object,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ main_window_destroy ]]\n" );
#endif

	return;
}

gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	if( GTK_WIDGET_SENSITIVE( main_window ) ){
		W1_Close();
  		return FALSE;
	}
	else{
		return TRUE;
	}
}


gboolean on_main_window_button_press_event(
		GtkWidget		*widget,
		GdkEventButton	*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_main_window_button_press_event ]]\n" );
#endif
	CNMSInt32	ldata;
	CNMSBool	hgKey;

	if( ( hgKey = CnmsHGGetToggleStatus() ) != CNMS_TRUE ){
		goto	EXIT;
	}

	if( ( ldata = CnmsHGSpuitButtonToggleRelease() ) == CNMS_NO_ERR ){
		W1_SetModal( CNMS_FALSE );
		if( bc_dlg.widget != CNMSNULL ){
			if( GTK_WIDGET_VISIBLE( bc_dlg.widget ) != 0 ){
				ComGtk_CursorImageSet( bc_dlg.widget, CNMS_CURSOR_ID_NORMAL );
			}
		}
		if( tc_dlg.widget != CNMSNULL ){
			if( GTK_WIDGET_VISIBLE( tc_dlg.widget ) != 0 ){
				ComGtk_CursorImageSet( tc_dlg.widget, CNMS_CURSOR_ID_NORMAL );
			}
		}
		if( fr_dlg.widget != CNMSNULL ){
			if( GTK_WIDGET_VISIBLE( fr_dlg.widget ) != 0 ){
				ComGtk_CursorImageSet( fr_dlg.widget, CNMS_CURSOR_ID_NORMAL );
			}
		}
	}
EXIT:
	return	FALSE;
}

void
on_preference_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
	CNMSInt32	ldata;
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_preference_button_clicked ]]\n" );
#endif
	if( ( ldata = Preference_Show( lpW1Comp->lpLink, lpW1Comp->linkNum ) ) != CNMS_NO_ERR ){
		goto	EXIT;
	}

EXIT:
	return;
}


void
on_close_button_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_close_button_clicked ]]\n" );
#endif
	W1_Close();

	return;
}


  /*****************/
 /***** About *****/
/*****************/
void on_toolbar_about_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_toolbar_about_button_clicked ]]\n" );
#endif
	if( about_dialog != CNMSNULL ) {
		W1_ModalDialogShowAction( about_dialog, main_window );
	}

	return;
}

void on_dialog_about_ok_button_clicked(
		GtkButton	*button,
		gpointer	user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_dialog_about_ok_button_clicked ]]\n" );
#endif
	W1_ModalDialogHideAction( about_dialog, main_window );

	return;
}

gboolean on_dialog_about_delete_event(
		GtkWidget	*widget,
		GdkEvent	*event,
		gpointer	user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_dialog_about_delete_event ]]\n" );
#endif
	W1_ModalDialogHideAction( about_dialog, main_window );

	return	TRUE;
}


  /*************************/
 /***** Reset setting *****/
/*************************/
void on_main_notebook_switch_page(
		GtkNotebook		*notebook,
		GtkNotebookPage	*page,
		guint			page_num,
		gpointer		user_data )
{
	static CNMSInt32 called_count = 0, ldata;

#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_main_notebook_switch_page(page_num:%d, called_count:%d) ]]\n", page_num, called_count );
#endif
	if( ( called_count < 2 ) || ( note_flag == CNMS_TRUE ) ){
		goto	EXIT;
	}

	CnmsColAdjSetCurrentPage( page_num );
	if( ( ldata = ChildDialogShow( CHILDDIALOG_ID_RESET_SETTING, main_window ) ) != CNMS_NO_ERR ){
		/* No dialog */
		W1_Reset( PREVIEW_RESET_RECT );
	}
EXIT:
	called_count ++;

	return;
}

  /************************/
 /***** Child Dialog *****/
/************************/

/* Clicked OK */
void on_dialog_child_with_checkBox_button_ok_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_dialog_child_with_checkBox_button_ok_clicked ]]\n" );
#endif
	CNMSInt32	ldata, id;

	if( ( id = ChildDialogGetId() ) == CNMS_ERR ){
		goto	EXIT;
	}
	else if( ( ldata = ChildDialogClickedOk() ) != CNMS_NO_ERR ){
		DBGMSG( "[on_dialog_child_with_checkBox_button_ok_clicked]Error is occured in ChildDialogClickedOk!\n" );
		goto	EXIT;
	}

	switch( id ){
		case	CHILDDIALOG_ID_RESET_SETTING:
			if( ( ldata = W1_Reset( PREVIEW_RESET_RECT ) ) != CNMS_NO_ERR ){
				goto	EXIT;
			}
			break;
		
		case	CHILDDIALOG_ID_OVER_100MB:
#ifdef	__GIMP_PLUGIN_ENABLE__
			if( ScanGimpGetStatus() == CNMS_TRUE ){
				ScanGimpExec();
			}
			else{
#endif
				SaveDialog_Show();
#ifdef	__GIMP_PLUGIN_ENABLE__
			}
#endif
			break;
		
		default:
			goto	EXIT;
			break;
	}
EXIT:
	return;
}

void on_dialog_child_with_ok_button_ok_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
	on_dialog_child_with_checkBox_button_ok_clicked( button, user_data );

	return;
}

void on_dialog_child_with_okcancel_button_ok_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
	on_dialog_child_with_checkBox_button_ok_clicked( button, user_data );

	return;
}

/* Clicked Cancel */
void on_dialog_child_with_checkBox_button_cancel_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_dialog_child_with_checkBox_button_cancel_clicked ]]\n" );
#endif
	GtkWidget	*widget;
	CNMSInt32	ldata, id;

	if( ( id = ChildDialogGetId() ) == CNMS_ERR ){
		goto	EXIT;
	}
	/* hide dialog */
	else if( ( ldata = ChildDialogClickedCancel() ) != CNMS_NO_ERR ){
		DBGMSG( "[on_dialog_child_with_checkBox_button_cancel_clicked]Error is occured in ChildDialogClickedCancel!\n" );
		goto	EXIT;
	}
	/* restore last tab */
	if( id == CHILDDIALOG_ID_RESET_SETTING ){
		note_flag = CNMS_TRUE;
		if( ( widget = lookup_widget( main_window, "main_notebook" ) ) == CNMSNULL ){
			DBGMSG( "[on_dialog_child_with_checkBox_button_cancel_clicked]Can't look up widget(main_notebook).\n" );
			goto	EXIT;
		}
		id = gtk_notebook_get_current_page( GTK_NOTEBOOK( widget ) );
		gtk_notebook_set_current_page( GTK_NOTEBOOK( widget ), ~id & 0x01 );
		note_flag = CNMS_FALSE;
		CnmsColAdjSetCurrentPage( ~id & 0x01 );
	}
	else if( id == CHILDDIALOG_ID_OVER_100MB ) {
		W1_MainWindowSetSensitiveTrue();
	}

EXIT:
	return;
}

void on_dialog_child_with_okcancel_button_cancel_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
	on_dialog_child_with_checkBox_button_cancel_clicked( button, user_data );

	return;
}

/* delete */
gboolean on_dialog_child_with_checkBox_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
	on_dialog_child_with_checkBox_button_cancel_clicked( CNMSNULL, CNMSNULL );

	return	TRUE;
}

gboolean on_dialog_child_with_ok_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
	gboolean	ret;

	ret = on_dialog_child_with_checkBox_delete_event( widget, event, user_data );

	return	ret;
}

gboolean on_dialog_child_with_okcancel_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data)
{
	gboolean	ret;

	ret = on_dialog_child_with_checkBox_delete_event( widget, event, user_data );

	return	ret;
}

  /****************/
 /***** scan *****/
/****************/
void on_simple_scan_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_simple_scan_button_clicked ]]\n" );
#endif

	on_advance_scan_button_clicked( button, user_data );

	return;
}

void on_advance_scan_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_scan_button_clicked ]]\n" );
#endif

	CNMSInt32	ldata;

	W1_MainWindowSetSensitiveFalse();

	/* Over pixel */
	if( ( ldata = ChildDialogShow( CHILDDIALOG_ID_PIXELS_OVER, CNMSNULL ) ) != CNMS_NO_ERR_2 ){
		W1_MainWindowSetSensitiveTrue();
		goto	EXIT;	/* Error or Show pixel over dialog -> exit */
	}
	else if( ( ldata = ChildDialogShow( CHILDDIALOG_ID_DATASIZE_OVER, CNMSNULL ) ) != CNMS_NO_ERR_2 ){
		W1_MainWindowSetSensitiveTrue();
		goto	EXIT;	/* Error or Show datasize over dialog -> exit */
	}
	else if( ( ldata = ChildDialogShow( CHILDDIALOG_ID_OVER_100MB, CNMSNULL ) ) != CNMS_NO_ERR_2 ){
		goto	EXIT;	/* Error or Show 100MB over dialog -> exit */
	}

#ifdef	__GIMP_PLUGIN_ENABLE__
	if( ScanGimpGetStatus() == CNMS_TRUE ){
		ScanGimpExec();
	}
	else{
#endif
		SaveDialog_Show();
#ifdef	__GIMP_PLUGIN_ENABLE__
	}
#endif
EXIT:
	return;
}

  /************************/
 /***** preview area *****/
/************************/
void on_simple_preview_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_simple_preview_button_clicked ]]\n" );
#endif
	
	on_advance_preview_button_clicked( button, user_data );
	
	return;
}

void on_advance_preview_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
	CNMSInt32	ldata;
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_preview_button_clicked ]]\n" );
#endif
	W1_MainWindowSetSensitiveFalse();

	if( ( ldata = W1_Preview() ) != CNMS_NO_ERR ){
		goto	EXIT;
	}

EXIT:
	W1_MainWindowSetSensitiveTrue();
	
	return;
}

void on_preview_clear_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_preview_clear_button_clicked ]]\n" );
#endif
	W1_PreviewClear( CLEARPREVIEW_TYPE_NORMAL );

	return;
}

void on_dialog_clear_preview_ok_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
	CNMSInt32	ldata;
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_dialog_clear_preview_ok_clicked ]]\n" );
#endif
	dialog_clear_preview_ok_clicked();
	W1_ModalDialogHideAction( clear_preview_dialog, main_window );

	CnmsColAdjAllDataReset();

	if( ( ldata = ComGtk_GtkMainQuit( COMGTK_GTKMAIN_OK ) ) == CNMS_ERR ){
		DBGMSG( "[[ on_dialog_clear_preview_ok_clicked ] Function error by ComGtk_GtkMainQuit() = %d ]\n",ldata );
	}

	return;
}

void on_dialog_clear_preview_cancel_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
	CNMSInt32	ldata;
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_dialog_clear_preview_cancel_clicked ]]\n" );
#endif
	W1_ModalDialogHideAction( clear_preview_dialog, main_window );

	if( ( ldata = ComGtk_GtkMainQuit( COMGTK_GTKMAIN_CANCEL ) ) == CNMS_ERR ){
		DBGMSG( "[[ on_dialog_clear_preview_cancel_clicked ] Function error by ComGtk_GtkMainQuit() = %d ]\n",ldata );
	}
	
	return;
}

gboolean on_dialog_clear_preview_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data)
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_dialog_clear_preview_delete_event ]]\n" );
#endif
	on_dialog_clear_preview_cancel_clicked( ( GtkButton* )widget, user_data );

	return TRUE;
}


gboolean on_main_preview_configure_event(
		GtkWidget			*widget,
		GdkEventConfigure	*event,
		gpointer			user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_main_preview_size_allocate ]]\n" );
#endif
	Preview_Configure_Event();

	return TRUE;
}

gboolean on_main_preview_expose_event(
		GtkWidget		*widget,
		GdkEventExpose	*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_main_preview_expose_event ]]\n" );
#endif

	Preview_Expose_Event();
EXIT:
	return	FALSE;
}

gboolean on_main_preview_button_press_event(
		GtkWidget		*widget,
		GdkEventButton	*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_main_preview_button_press_event ]]\n" );
#endif
	CNMSInt32			aspect, cursorPos[ CNMS_DIM_MAX ];
	CNMSBool			hgKey;
	int					x, y;
	GdkModifierType		state;

	if( W1_GetSelectSourceADF() == CNMS_TRUE )	goto EXIT;

	/* Avoid without Single Click */
	if( event->type != GDK_BUTTON_PRESS )
		goto	EXIT;

	gdk_window_get_pointer( event->window, &x, &y, &state );
	cursorPos[ CNMS_DIM_H ] = ( x < 0 ) ? 0 : (CNMSInt32)x;
	cursorPos[ CNMS_DIM_V ] = ( y < 0 ) ? 0 : (CNMSInt32)y;
	
	state &= ( GDK_BUTTON1_MASK | GDK_BUTTON3_MASK );

	if( ( ( state & GDK_BUTTON1_MASK ) != 0 ) && ( ( state & GDK_BUTTON3_MASK ) != 0 ) ){
		goto	EXIT;
	}
	else if( ( aspect = CnmsUiAspectGet( lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		goto	EXIT;
	}

	hgKey = CnmsHGGetToggleStatus();
	Preview_Mouse_Button_Press( hgKey, aspect, lpW1Comp->previewFlag, cursorPos, state );
EXIT:
	return	FALSE;
}

gboolean on_main_preview_button_release_event(
		GtkWidget		*widget,
		GdkEventButton	*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_main_preview_button_release_event ]]\n" );
#endif
	CNMSInt32			cursorPos[ CNMS_DIM_MAX ];
	CNMSBool			hgKey;
	int					x, y;
	GdkModifierType		state;

	if( W1_GetSelectSourceADF() == CNMS_TRUE )	goto EXIT;

	gdk_window_get_pointer( event->window, &x, &y, &state );
	cursorPos[ CNMS_DIM_H ] = ( x < 0 ) ? 0 : (CNMSInt32)x;
	cursorPos[ CNMS_DIM_V ] = ( y < 0 ) ? 0 : (CNMSInt32)y;

	state &= ( GDK_BUTTON1_MASK | GDK_BUTTON3_MASK );

	hgKey = CnmsHGGetToggleStatus();
	Preview_Mouse_Button_Release( hgKey, lpW1Comp->previewFlag, cursorPos, state );

EXIT:
	return	FALSE;
}

gboolean on_main_preview_motion_notify_event(
		GtkWidget		*widget,
		GdkEventMotion	*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT_DETAIL__
	DBGMSG( "[[ on_main_preview_motion_notify_event ]]\n" );
#endif
	CNMSInt32			rVal[2], gVal[2], bVal[2], cursorPos[ CNMS_DIM_MAX ];
	CNMSBool			hgKey;
	int					x, y;
	GdkModifierType		state;
	CNMSInt32	ret = CNMS_ERR;

	if( W1_GetSelectSourceADF() == CNMS_TRUE )	goto EXIT;

	if( event->is_hint != 0 ){
		gdk_window_get_pointer( event->window, &x, &y, &state );
		cursorPos[ CNMS_DIM_H ] = ( x < 0 ) ? 0 : (CNMSInt32)x;
		cursorPos[ CNMS_DIM_V ] = ( y < 0 ) ? 0 : (CNMSInt32)y;
	}
	else{
		cursorPos[ CNMS_DIM_H ] = ( event->x < 0 ) ? 0 : (CNMSInt32)( event->x );
		cursorPos[ CNMS_DIM_V ] = ( event->y < 0 ) ? 0 : (CNMSInt32)( event->y );
		state = event->state;
	}

	state &= ( GDK_BUTTON1_MASK | GDK_BUTTON3_MASK );

	hgKey = CnmsHGGetToggleStatus();

	Preview_Motion( hgKey, lpW1Comp->previewFlag, cursorPos, state );

	if ( lpW1Comp->previewFlag == CNMS_TRUE ) {
		ret = Preview_GetColor( cursorPos, rVal, gVal, bVal );
		
		if ( ret == CNMS_NO_ERR && ( ( state & GDK_BUTTON1_MASK ) == 0 ) ) {
			if( hgKey == CNMS_TRUE ) {
				CnmsHGPreviewRGBKValueDisp( rVal[1], gVal[1], bVal[1] );
			}
			/* Val[1]:Original, Val[0]:Current */
			CnmsFRPreviewRGBKValueDisp( rVal[0], rVal[1], gVal[0], gVal[1], bVal[0], bVal[1] );
		}
		else{
			CnmsHGPreviewRGBKValueUnDisp();
			CnmsFRPreviewRGBKValueUnDisp();
		}
	}
EXIT:
	return	FALSE;
}

  /***********************/
 /***** simple mode *****/
/***********************/
void on_simple_src_combo_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_simple_src_combo_changed ]]\n" );
#endif
	CNMSInt32	ldata;

	if( ( ldata = W1_ChangeCombo( combobox, CNMS_OBJ_S_SOURCE ) ) != CNMS_NO_ERR ){
		goto	EXIT;
	}
	CnmsColAdjColorModeChanged();
EXIT:
	return;
}

void on_simple_dst_combo_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_simple_dst_combo_changed ]]\n" );
#endif
	W1_ChangeCombo( combobox, CNMS_OBJ_S_DESTINATION );

	return;
}

void on_simple_output_size_combo_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_simple_output_size_combo_changed ]]\n" );
#endif
	W1_ChangeCombo( combobox, CNMS_OBJ_S_OUTPUT_SIZE );

	return;
}

  /************************/
 /***** advance mode *****/
/************************/
void on_advance_src_combo_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_src_combo_changed ]]\n" );
#endif
	W1_ChangeCombo( combobox, CNMS_OBJ_A_SOURCE );

	return;
}

void on_advance_paper_combo_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_paper_combo_changed ]]\n" );
#endif
	W1_ChangeCombo( combobox, CNMS_OBJ_A_PAPER_SIZE );

	return;
}

void
on_advance_binding_combo_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_binding_combo_changed ]]\n" );
#endif
	W1_ChangeCombo( combobox, CNMS_OBJ_A_BINDING_LOCATION );

	return;
}

void on_advance_color_combo_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_color_combo_changed ]]\n" );
#endif
	CNMSInt32	ldata;

	if( ( ldata = W1_ChangeCombo( combobox, CNMS_OBJ_A_COLOR_MODE ) ) != CNMS_NO_ERR ){
		goto	EXIT;
	}
	CnmsColAdjColorModeChanged();

EXIT:
	return;
}

void on_advance_input_width_spin_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_input_width_spin_value_changed ]]\n" );
#endif
	W1_ChangeSpin( spinbutton, CNMS_OBJ_A_INPUT_WIDTH );

	return;
}

void on_advance_input_height_spin_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_input_height_spin_value_changed ]]\n" );
#endif
	W1_ChangeSpin( spinbutton, CNMS_OBJ_A_INPUT_HEIGHT );

	return;
}

void
on_advance_input_width_spin_state_changed(
		GtkWidget		*widget,
		GtkStateType	state,
		gpointer		user_data)
{
	ComGtk_EditableClearSelection( widget, state );
}

void
on_advance_input_height_spin_state_changed(
		GtkWidget		*widget,
		GtkStateType	state,
		gpointer		user_data)
{
	ComGtk_EditableClearSelection( widget, state );
}

void on_advance_unit_combo_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_unit_combo_changed ]]\n" );
#endif
	W1_ChangeCombo( combobox, CNMS_OBJ_A_UNIT );

	return;
}

void on_advance_size_lock_toggle_toggled(
		GtkToggleButton	*togglebutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_size_lock_toggle_toggled ]]\n" );
#endif
	W1_ChangeToggle( togglebutton, CNMS_OBJ_A_RATIO_FIX );

	return;
}

void on_advance_output_resolution_combo_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_output_resolution_combo_changed ]]\n" );
#endif
	W1_ChangeCombo( combobox, CNMS_OBJ_A_OUTPUT_RESOLUTION );

	return;
}

void on_advance_output_size_combo_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_output_size_combo_changed ]]\n" );
#endif
	W1_ChangeCombo( combobox, CNMS_OBJ_A_OUTPUT_SIZE );

	return;
}

void on_advance_output_width_spin_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_output_width_spin_value_changed ]]\n" );
#endif
	W1_ChangeSpin( spinbutton, CNMS_OBJ_A_OUTPUT_WIDTH );

	return;
}

void on_advance_output_height_spin_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_output_height_spin_value_changed ]]\n" );
#endif
	W1_ChangeSpin( spinbutton, CNMS_OBJ_A_OUTPUT_HEIGHT );

	return;
}

void on_advance_scale_spin_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_scale_spin_value_changed ]]\n" );
#endif
	W1_ChangeSpin( spinbutton, CNMS_OBJ_A_SCALE );

	return;
}

void on_advance_output_width_spin_state_changed(
		GtkWidget		*widget,
		GtkStateType	state,
		gpointer		user_data)
{
	ComGtk_EditableClearSelection( widget, state );
}

void on_advance_output_height_spin_state_changed(
		GtkWidget		*widget,
		GtkStateType	state,
		gpointer		user_data)
{
	ComGtk_EditableClearSelection( widget, state );
}

void on_advance_scale_spin_state_changed(
		GtkWidget		*widget,
		GtkStateType	state,
		gpointer		user_data)
{
	ComGtk_EditableClearSelection( widget, state );
}


void on_advance_unsharp_combo_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_unsharp_combo_changed ]]\n" );
#endif
	W1_ChangeCombo( combobox, CNMS_OBJ_A_UNSHARP_MASK );

	return;
}

void on_advance_descreen_combo_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_descreen_combo_changed ]]\n" );
#endif
	W1_ChangeCombo( combobox, CNMS_OBJ_A_DESCREEN );

	return;
}



  /*****************************/
 /***** preference dialog *****/
/*****************************/
void on_preference_window_destroy(
		GtkObject		*object,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_preference_window_destroy ]]\n" );
#endif
	return;
}

gboolean on_preference_window_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data)
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_preference_window_delete_event ]]\n" );
#endif
	if( Preference_GetSensitive() ){
		Preference_Hide();
	}
	return TRUE;
}

void on_preference_ok_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_preference_ok_button_clicked ]]\n" );
#endif
	Preference_Save();
	Preference_Hide();

	return;
}

void on_preference_cancel_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_preference_cancel_button_clicked ]]\n" );
#endif
	Preference_Hide();

	return;
}

void on_preference_reset_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_preference_reset_button_clicked ]]\n" );
#endif
	Preference_GammaReset();

	return;
}

  /**************************/
 /***** Image Settings *****/
/**************************/
void on_advance_reset_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_reset_button_clicked ]]\n" );
#endif
	CnmsColAdjAllDataReset();

	return;
}

  /*******************************/
 /***** Brightness/Contrast *****/
/*******************************/
void on_advance_BC_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_BC_button_clicked ]]\n" );
#endif
	CnmsColAdjButtonClicked( CNMS_CA_DIALOG_BC );

	return;
}

gboolean on_dialog_bright_contrast_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_dialog_bright_contrast_delete_event ]]\n" );
#endif
	CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_BC, CNMS_FALSE );

	return	TRUE;
}

gboolean on_bc_drawingarea_configure_event(
		GtkWidget			*widget,
		GdkEventConfigure	*event,
		gpointer			user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_bc_drawingarea_configure_event ]]\n" );
#endif
	CnmsColAdjDrawingAreaConfigureEvent( widget, event, CNMS_CA_DIALOG_BC );

	return	TRUE;
}

gboolean on_bc_drawingarea_expose_event(
		GtkWidget		*widget,
		GdkEventExpose	*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_bc_drawingarea_expose_event ]]\n" );
#endif
	CnmsColAdjDrawingAreaExposeEvent( widget, event, CNMS_CA_DIALOG_BC );

	return	FALSE;
}

void on_bc_button_reset_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_bc_button_reset_clicked ]]\n" );
#endif
	CnmsBCReset();
	CnmsColAdjPreviewImageSet();

	return;
}

void on_bc_button_close_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_bc_button_close_clicked ]]\n" );
#endif
	CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_BC, CNMS_FALSE );

	return;
}

void on_bc_scale_bright_value_changed(
		GtkRange		*range,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_bc_scale_bright_value_changed ]]\n" );
#endif
	CnmsBCScaleBarValueChanged( CNMS_CA_ID_BRIGHT, range );

	return;
}

void on_bc_spinbutton_bright_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_bc_spinbutton_bright_value_changed ]]\n" );
#endif
	CnmsBCSpinButtonValueChanged( CNMS_CA_ID_BRIGHT, spinbutton );

	return;
}

gboolean on_bc_scale_bright_button_press_event(
		GtkWidget		*widget,
		GdkEventButton	*event,
		gpointer		user_data)
{
	CnmsBCScaleBarPress( CNMS_CA_ID_BRIGHT, event );
	return FALSE;
}

gboolean on_bc_scale_bright_button_release_event(
		GtkWidget		*widget,
		GdkEventButton	*event,
		gpointer		user_data)
{
	CnmsBCScaleBarRelease( event );
	return FALSE;
}

void on_bc_scale_contrast_value_changed(
		GtkRange		*range,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_bc_scale_contrast_value_changed ]]\n" );
#endif
	CnmsBCScaleBarValueChanged( CNMS_CA_ID_CONTRAST, range );

	return;
}

void on_bc_spinbutton_contrast_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_bc_spinbutton_contrast_value_changed ]]\n" );
#endif
	CnmsBCSpinButtonValueChanged( CNMS_CA_ID_CONTRAST, spinbutton );

	return;
}

void on_bc_combobox_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_bc_combobox_changed ]]\n" );
#endif
	CnmsBCChannelComboBoxChanged( combobox );

	return;
}

gboolean on_bc_scale_contrast_button_press_event(
		GtkWidget		*widget,
		GdkEventButton	*event,
		gpointer		user_data)
{
	CnmsBCScaleBarPress( CNMS_CA_ID_CONTRAST, event );
	return FALSE;
}

gboolean on_bc_scale_contrast_button_release_event(
		GtkWidget		*widget,
		GdkEventButton	*event,
		gpointer		user_data)
{
	CnmsBCScaleBarRelease( event );
	return FALSE;
}


  /*********************/
 /***** Histogram *****/
/*********************/
void on_advance_HG_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_HG_button_clicked ]]\n" );
#endif
	CnmsColAdjButtonClicked( CNMS_CA_DIALOG_HG );

	return;
}

gboolean on_dialog_histogram_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_dialog_histogram_delete_event ]]\n" );
#endif
	CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_HG, CNMS_FALSE );

	return	TRUE;
}

gboolean on_hg_drawingarea_configure_event(
		GtkWidget			*widget,
		GdkEventConfigure	*event,
		gpointer			user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_drawingarea_configure_event ]]\n" );
#endif
	CnmsColAdjDrawingAreaConfigureEvent( widget, event, CNMS_CA_DIALOG_HG );

	return	TRUE;
}

gboolean on_hg_drawingarea_expose_event(
		GtkWidget			*widget,
		GdkEventExpose		*event,
		gpointer			user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_drawingarea_expose_event ]]\n" );
#endif
	CnmsColAdjDrawingAreaExposeEvent( widget, event, CNMS_CA_DIALOG_HG );

	return	FALSE;
}

void on_hg_button_reset_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_button_reset_clicked ]]\n" );
#endif
	CnmsHGReset();
	CnmsColAdjPreviewImageSet();

	return;
}

void on_hg_button_close_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_button_close_clicked ]]\n" );
#endif
	CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_HG, CNMS_FALSE );

	return;
}

void on_hg_combobox_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_combobox_changed ]]\n" );
#endif
	CnmsHGChannelComboBoxChanged( combobox );

	return;
}

gboolean on_hg_drawingarea_scalebar_configure_event(
		GtkWidget			*widget,
		GdkEventConfigure	*event,
		gpointer			user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_drawingarea_scalebar_configure_event ]]\n" );
#endif
	CnmsHGScaleBarConfigureEvent( widget, event );

	return	TRUE;
}

gboolean on_hg_drawingarea_scalebar_expose_event(
		GtkWidget		*widget,
		GdkEventExpose	*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_drawingarea_scalebar_expose_event ]]\n" );
#endif
	CnmsHGScaleBarExposeEvent( widget, event );

	return	FALSE;
}

gboolean on_hg_drawingarea_scalebar_button_press_event(
		GtkWidget		*widget,
		GdkEventButton	*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_drawingarea_scalebar_button_press_event ]]\n" );
#endif
	CnmsHGScaleBarButtonPressEvent( event );

	return	TRUE;
}

gboolean on_hg_drawingarea_scalebar_button_release_event(
		GtkWidget		*widget,
		GdkEventButton	*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_drawingarea_scalebar_button_release_event ]]\n" );
#endif
	CnmsHGScaleBarButtonReleaseEvent( event );

	return	TRUE;
}

gboolean on_hg_drawingarea_scalebar_motion_notify_event(
		GtkWidget		*widget,
		GdkEventMotion	*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_drawingarea_scalebar_motion_notify_event ]]\n" );
#endif
	CnmsHGScaleBarMotionNotifyEvent( event );

	return	TRUE;
}

void on_hg_spinbutton_shadow_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_spinbutton_shadow_value_changed ]]\n" );
#endif
	CnmsHGSpinButtonValueChanged( spinbutton, CNMS_HG_GRAB_SHADOW );

	return;
}

void on_hg_spinbutton_mid_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_spinbutton_mid_value_changed ]]\n" );
#endif
	CnmsHGSpinButtonValueChanged( spinbutton, CNMS_HG_GRAB_MID );

	return;
}

void on_hg_spinbutton_highlight_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_spinbutton_highlight_value_changed ]]\n" );
#endif
	CnmsHGSpinButtonValueChanged( spinbutton, CNMS_HG_GRAB_HIGHLIGHT );

	return;
}

void on_hg_togglebutton_shadow_toggled(
		GtkToggleButton	*togglebutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_togglebutton_shadow_toggled ]]\n" );
#endif
	CNMSBool	toggleFlag;

	CnmsHGSpuitButtonToggled( togglebutton, CNMS_HG_SPUIT_SHADOW );
	toggleFlag = gtk_toggle_button_get_active( togglebutton );
	W1_SetModal( toggleFlag );

	return;
}

void on_hg_togglebutton_mid_toggled(
		GtkToggleButton	*togglebutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_togglebutton_mid_toggled ]]\n" );
#endif
	CNMSBool	toggleFlag;

	CnmsHGSpuitButtonToggled( togglebutton, CNMS_HG_SPUIT_MID );
	toggleFlag = gtk_toggle_button_get_active( togglebutton );
	W1_SetModal( toggleFlag );

	return;
}

void on_hg_togglebutton_highlight_toggled(
		GtkToggleButton	*togglebutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_togglebutton_highlight_toggled ]]\n" );
#endif
	CNMSBool	toggleFlag;

	CnmsHGSpuitButtonToggled( togglebutton, CNMS_HG_SPUIT_HIGHLIGHT );
	toggleFlag = gtk_toggle_button_get_active( togglebutton );
	W1_SetModal( toggleFlag );

	return;
}

void on_hg_togglebutton_gbalance_toggled(
		GtkToggleButton	*togglebutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_hg_togglebutton_gbalance_toggled ]]\n" );
#endif
	CNMSBool	toggleFlag;

	CnmsHGSpuitButtonToggled( togglebutton, CNMS_HG_SPUIT_GRAYLEVEL );
	toggleFlag = gtk_toggle_button_get_active( togglebutton );
	W1_SetModal( toggleFlag );

	return;
}

  /*******************************/
 /***** Tone Curve Settings *****/
/*******************************/
void on_advance_TC_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_TC_button_clicked ]]\n" );
#endif
	CnmsColAdjButtonClicked( CNMS_CA_DIALOG_TC );

	return;
}

gboolean on_dialog_tonecurve_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_dialog_tonecurve_delete_event ]]\n" );
#endif
	CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_TC, CNMS_FALSE );

	return	TRUE;
}

gboolean on_tc_drawingarea_configure_event(
		GtkWidget			*widget,
		GdkEventConfigure	*event,
		gpointer			user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_tc_drawingarea_configure_event ]]\n" );
#endif
	CnmsColAdjDrawingAreaConfigureEvent( widget, event, CNMS_CA_DIALOG_TC );

	return	FALSE;
}

gboolean on_tc_drawingarea_expose_event(
		GtkWidget			*widget,
		GdkEventExpose		*event,
		gpointer			user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_tc_drawingarea_expose_event ]]\n" );
#endif
	CnmsColAdjDrawingAreaExposeEvent( widget, event, CNMS_CA_DIALOG_TC );

	return	FALSE;
}

void on_tc_button_reset_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_tc_button_reset_clicked ]]\n" );
#endif
	CnmsTCReset();
	CnmsColAdjPreviewImageSet();

	return;
}

void on_tc_button_close_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_tc_button_close_clicked ]]\n" );
#endif
	CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_TC, CNMS_FALSE );

	return;
}

void on_tc_combobox_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_tc_combobox_changed ]]\n" );
#endif
	CnmsTCChannelComboBoxChanged( combobox );

	return;
}

void on_tc_combobox_tc_select_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_tc_combobox_tc_select_changed ]]\n" );
#endif
	CnmsTCToneCurveSelectComboBoxChanged( combobox );

	return;
}

  /************************/
 /***** Final Review *****/
/************************/
void on_advance_FR_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_FR_button_clicked ]]\n" );
#endif
	CnmsColAdjButtonClicked( CNMS_CA_DIALOG_FR );

	return;
}

gboolean on_dialog_final_review_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_dialog_final_review_delete_event ]]\n" );
#endif
	CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_FR, CNMS_FALSE );

	return	TRUE;
}

gboolean on_fr_drawingarea_configure_event(
		GtkWidget			*widget,
		GdkEventConfigure	*event,
		gpointer			user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_fr_drawingarea_configure_event ]]\n" );
#endif
	CnmsColAdjDrawingAreaConfigureEvent( widget, event, CNMS_CA_DIALOG_FR );

	return	TRUE;
}

gboolean on_fr_drawingarea_expose_event(
		GtkWidget			*widget,
		GdkEventExpose		*event,
		gpointer			user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_fr_drawingarea_expose_event ]]\n" );
#endif
	CnmsColAdjDrawingAreaExposeEvent( widget, event, CNMS_CA_DIALOG_FR );

	return	FALSE;
}

void on_fr_button_close_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_fr_button_close_clicked ]]\n" );
#endif
	CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_FR, CNMS_FALSE );

	return;
}

void on_fr_combobox_changed(
		GtkComboBox		*combobox,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_fr_combobox_changed ]]\n" );
#endif
	CnmsFRChannelComboBoxChanged( combobox );

	return;
}

  /*********************/
 /***** Threshold *****/
/*********************/
void on_advance_MC_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_advance_MC_button_clicked ]]\n" );
#endif
	CnmsColAdjButtonClicked( CNMS_CA_DIALOG_MC );

	return;
}

gboolean on_dialog_monochrome_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_dialog_monochrome_delete_event ]]\n" );
#endif
	CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_MC, CNMS_FALSE );

	return	TRUE;
}

gboolean on_mc_drawingarea_configure_event(
		GtkWidget			*widget,
		GdkEventConfigure	*event,
		gpointer			user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_mc_drawingarea_configure_event ]]\n" );
#endif
	CnmsColAdjDrawingAreaConfigureEvent( widget, event, CNMS_CA_DIALOG_MC );

	return	TRUE;
}

gboolean on_mc_drawingarea_expose_event(
		GtkWidget			*widget,
		GdkEventExpose		*event,
		gpointer			user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_mc_drawingarea_expose_event ]]\n" );
#endif
	CnmsColAdjDrawingAreaExposeEvent( widget, event, CNMS_CA_DIALOG_MC );

	return	FALSE;
}

void on_mc_button_reset_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_mc_button_reset_clicked ]]\n" );
#endif
	CnmsMCReset();
	CnmsColAdjPreviewImageSet();

	return;
}

void on_mc_button_close_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_mc_button_close_clicked ]]\n" );
#endif
	CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_MC, CNMS_FALSE );

	return;
}

void on_mc_spinbutton_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_mc_spinbutton_value_changed ]]\n" );
#endif
	CnmsMCSpinButtonThresholdValueChanged( spinbutton );

	return;
}

void on_mc_scale_value_changed(
		GtkRange		*range,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_mc_scale_value_changed ]]\n" );
#endif
	CnmsMCScaleThresholdValueChanged( range );

	return;
}

gboolean on_mc_scale_button_press_event(
		GtkWidget		*widget,
		GdkEventButton	*event,
		gpointer		user_data )
{
	CnmsMCScaleBarPress( event );
	return FALSE;
}


gboolean on_mc_scale_button_release_event(
		GtkWidget		*widget,
		GdkEventButton	*event,
		gpointer		user_data )
{
	CnmsMCScaleBarRelease( event );
	return FALSE;
}


  /********************/
 /***** progress *****/
/********************/
void on_progress_bar_cancel_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_progress_bar_cancel_button_clicked ]]\n" );
#endif
	ProgressBarCanceled();

EXIT:
	return;
}

gboolean on_progress_bar_dialog_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_progress_bar_dialog_delete_event ]]\n" );
#endif
	ProgressBarDeleted();

	return	TRUE;
}



  /***********************/
 /***** calibration *****/
/***********************/
gboolean on_dialog_calibration_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_dialog_calibration_delete_event ]]\n" );
#endif
	return	TRUE;
}


  /****************/
 /***** Save *****/
/****************/
gboolean on_save_main_dialog_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_save_main_dialog_delete_event ]]\n" );
#endif
	if( SaveDialog_GetSensitive() ) {
		on_save_main_cancel_button_clicked( CNMSNULL, CNMSNULL );
	}
	
	return	TRUE;
}

void on_save_main_cancel_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_save_main_cancel_button_clicked ]]\n" );
#endif
	SaveDialog_Hide();

	W1_MainWindowSetSensitiveTrue();

	return;
}

void on_save_main_save_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_save_main_save_button_clicked ]]\n" );
#endif
	SaveDialog_Exec();

	return;
}

  /******************************/
 /***** Warning over write *****/
/******************************/
gboolean on_save_overwrite_dialog_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_save_overwrite_dialog_delete_event ]]\n" );
#endif
	dialog_ok_del_clicked();

	return	TRUE;
}

void on_save_overwrite_cancel_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_save_overwrite_cancel_button_clicked ]]\n" );
#endif
	dialog_ok_del_clicked();

	return;
}

void on_save_overwrite_ok_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_save_overwrite_ok_button_clicked ]]\n" );
#endif
	dialog_ok_del_clicked();
	SaveDialog_Hide();
	CnmsScanFileExec();

	return;
}

  /*****************/
 /***** Error *****/
/*****************/
gboolean on_save_error_dialog_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_save_error_dialog_delete_event ]]\n" );
#endif
	dialog_ok_del_clicked();

	return	TRUE;
}

void on_save_error_ok_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_save_error_ok_button_clicked ]]\n" );
#endif
	dialog_ok_del_clicked();

	return;
}

gboolean on_error_dialog_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_error_dialog_delete_event ]]\n" );
#endif
	error_dialog_ok_del_clicked();

	return	TRUE;
}

void on_error_dialog_ok_button_clicked(
		GtkButton		*button,
		gpointer		user_data )
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_error_dialog_ok_button_clicked ]]\n" );
#endif
	error_dialog_ok_del_clicked();

	return;
}



  /**************************/
 /***** Quiet Settings *****/
/**************************/
void on_preference_scanner_quietsettings_button_clicked(
		GtkButton		*button,
		gpointer		user_data)
{
	Preference_QuietSettings_Show();
}


void on_dialog_quiet_settings_button_ok_clicked(
		GtkButton		*button,
		gpointer		user_data)
{
	Preference_QuietSettings_Hide( PREF_QUIETSETTINGS_OK );
}


void on_dialog_quiet_settings_button_cancel_clicked(
		GtkButton		*button,
		gpointer		user_data)
{
	Preference_QuietSettings_Hide( PREF_QUIETSETTINGS_CANCEL );
}


void on_dialog_quiet_settings_button_defaults_clicked(
		GtkButton		*button,
		gpointer		user_data)
{
	Preference_QuietSettings_SetDefaults();
}


void on_dialog_quiet_settings_radiobutton_off_clicked(
		GtkButton		*button,
		gpointer		user_data)
{
	Preference_QuietSettings_RadioClicked( PREF_QUIETSETTINGS_RADIO_OFF );
}


void on_dialog_quiet_settings_radiobutton_on_clicked(
		GtkButton		*button,
		gpointer		user_data)
{
	Preference_QuietSettings_RadioClicked( PREF_QUIETSETTINGS_RADIO_ON );
}


void on_dialog_quiet_settings_radiobutton_timer_clicked(
		GtkButton		*button,
		gpointer		user_data)
{
	Preference_QuietSettings_RadioClicked( PREF_QUIETSETTINGS_RADIO_TIMER );
}


gboolean on_dialog_quiet_settings_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data)
{
#ifdef	__CNMS_DEBUG_EVENT__
	DBGMSG( "[[ on_error_dialog_delete_event ]]\n" );
#endif
	on_dialog_quiet_settings_button_cancel_clicked( CNMSNULL, CNMSNULL );

	return	TRUE;
}


void on_dialog_quiet_settings_spin_start_h_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data)
{
	W1_ChangeSpinAdjustMinusZero( spinbutton );
}


void on_dialog_quiet_settings_spin_end_h_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data)
{
	W1_ChangeSpinAdjustMinusZero( spinbutton );
}


void on_dialog_quiet_settings_spin_start_m_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data)
{
	W1_ChangeSpinAdjustMinusZero( spinbutton );
}


void on_dialog_quiet_settings_spin_end_m_value_changed(
		GtkSpinButton	*spinbutton,
		gpointer		user_data)
{
	W1_ChangeSpinAdjustMinusZero( spinbutton );
}


gboolean on_dialog_get_settings_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data)
{
  return TRUE;
}


void on_dialog_get_settings_hide(
		GtkWidget		*widget,
		gpointer		user_data)
{
	ComGtk_GtkMainQuit( COMGTK_GTKMAIN_NORMAL );
}


void on_dialog_quiet_settings_spin_start_h_state_changed(
		GtkWidget		*widget,
		GtkStateType	state,
		gpointer		user_data)
{
	ComGtk_EditableClearSelection( widget, state );
}


void on_dialog_quiet_settings_spin_end_h_state_changed(
		GtkWidget		*widget,
		GtkStateType	state,
		gpointer		user_data)
{
	ComGtk_EditableClearSelection( widget, state );
}


void on_dialog_quiet_settings_spin_start_m_state_changed(
		GtkWidget		*widget,
		GtkStateType	state,
		gpointer		user_data)
{
	ComGtk_EditableClearSelection( widget, state );
}


void on_dialog_quiet_settings_spin_end_m_state_changed(
		GtkWidget		*widget,
		GtkStateType	state,
		gpointer		user_data)
{
	ComGtk_EditableClearSelection( widget, state );
}


  /******************************/
 /***** AutoPower Settings *****/
/******************************/
void on_preference_scanner_autopowersettings_button_clicked(
		GtkButton		*button,
		gpointer         user_data)
{
	Preference_AutoPowerSettings_Show();
}


void on_dialog_autopower_settings_on_combobox_changed(
		GtkComboBox		*combobox,
		gpointer		user_data)
{

}


void on_dialog_autopower_settings_off_combobox_changed(
		GtkComboBox		*combobox,
		gpointer		user_data)
{

}


void on_dialog_autopower_settings_button_ok_clicked(
		GtkButton		*button,
		gpointer		user_data)
{
	Preference_AutoPowerSettings_Hide( PREF_AUTOPOWERSETTINGS_OK );
}


void on_dialog_autopower_settings_button_cancel_clicked(
		GtkButton		*button,
		gpointer		user_data)
{
	Preference_AutoPowerSettings_Hide( PREF_AUTOPOWERSETTINGS_CANCEL );
}

gboolean on_dialog_autopower_settings_delete_event(
		GtkWidget		*widget,
		GdkEvent		*event,
		gpointer		user_data)
{
	on_dialog_autopower_settings_button_cancel_clicked( CNMSNULL, CNMSNULL );
	
	return TRUE;
}

#endif	/* _CALLBACKS_C_ */




