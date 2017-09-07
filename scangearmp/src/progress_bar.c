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

#ifndef _PROGRESS_BAR_C_
#define _PROGRESS_BAR_C_

//#define	__CNMS_DEBUG_PROGRESS__

#include <gtk/gtk.h>
#include "interface.h"
#include "support.h"

#include "cnmstype.h"
#include "cnmsfunc.h"
#include "w1.h"
#include "scanfile.h"
#include "scangimp.h"
#include "progress_bar.h"
#include "cnmsstrings.h"

static const CNMSLPSTR progressTitleStr[ PROGRESSBAR_ID_MAX ] = {
	STR_PROGRESSBAR_ID_SCAN,			/* PROGRESSBAR_ID_SCAN */
	STR_PROGRESSBAR_ID_SAVE,			/* PROGRESSBAR_ID_SAVE */
	STR_PROGRESSBAR_ID_TRANSFER_GIMP,	/* PROGRESSBAR_ID_TRANSFER_GIMP */
	STR_PROGRESSBAR_ID_SEARCH,			/* PROGRESSBAR_ID_SEARCH */
};

typedef struct{
	CNMSInt32			prev;
	CNMSInt32			id;
	CNMSBool			progress_canceled;
	GtkWidget			*dialog;
	GtkWidget			*bar;
	GtkWidget			*perStr;
	GtkWidget			*pagesStr;
	GtkWidget			*button;
	GtkWidget			*buttonStr;
	GtkWidget			*captionStr;
}PROGRESSBARCOMP, *LPPROGRESSBARCOMP;

static LPPROGRESSBARCOMP	lpProgress = CNMSNULL;

static CNMSInt32 SetProgressValue( CNMSInt32 percentVal );

CNMSInt32 ProgressBarOpen( CNMSVoid )
{
	CNMSInt32		ret = CNMS_ERR;

	if( lpProgress != CNMSNULL ){
		ProgressBarClose();
	}
	if( ( lpProgress = (LPPROGRESSBARCOMP)CnmsGetMem( sizeof( PROGRESSBARCOMP ) ) ) == CNMSNULL ){
		DBGMSG( "[ProgressBarOpen]Error is occured in CnmsGetMem.\n" );
		goto	EXIT;
	}

	lpProgress->prev = 0;
	lpProgress->id = CNMS_ERR;
	lpProgress->progress_canceled = CNMS_FALSE;
	if( ( lpProgress->dialog = create_dialog_progress_bar() ) == CNMSNULL ){
		DBGMSG( "[ProgressBarOpen]Error is occured in create_dialog_progress_bar.\n" );
		goto	EXIT;
	}
	else if( ( lpProgress->bar = (GtkWidget *)lookup_widget( lpProgress->dialog, "progressBar" ) ) == CNMSNULL ){
		DBGMSG( "[ProgressBarOpen]Can't look up widget(progressBar).\n" );
		goto	EXIT;
	}
	else if( ( lpProgress->perStr = (GtkWidget *)lookup_widget( lpProgress->dialog, "progressBar_label" ) ) == CNMSNULL ){
		DBGMSG( "[ProgressBarOpen]Can't look up widget(progressBar_label).\n" );
		goto	EXIT;
	}
	else if( ( lpProgress->pagesStr = (GtkWidget *)lookup_widget( lpProgress->dialog, "dialog_progress_bar_label_pages" ) ) == CNMSNULL ){
		DBGMSG( "[ProgressBarOpen]Can't look up widget(dialog_progress_bar_label_pages).\n" );
		goto	EXIT;
	}
	else if( ( lpProgress->button = (GtkWidget *)lookup_widget( lpProgress->dialog, "progressBar_cancel_button" ) ) == CNMSNULL ){
		DBGMSG( "[ProgressBarOpen]Can't look up widget(progressBar_cancel_button).\n" );
		goto	EXIT;
	}
	else if( ( lpProgress->buttonStr = (GtkWidget *)lookup_widget( lpProgress->dialog, "dialog_progress_bar_label_button" ) ) == CNMSNULL ){
		DBGMSG( "[ProgressBarOpen]Can't look up widget(dialog_progress_bar_label_button).\n" );
		goto	EXIT;
	}
	else if( ( lpProgress->captionStr = (GtkWidget *)lookup_widget( lpProgress->dialog, "dialog_progress_bar_label_caption" ) ) == CNMSNULL ){
		DBGMSG( "[ProgressBarOpen]Can't look up widget(dialog_progress_bar_label_caption).\n" );
		goto	EXIT;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PROGRESS__
	DBGMSG( "[ProgressBarOpen()]=%d.\n", ret );
#endif
	if( ret != CNMS_NO_ERR ){
		ProgressBarClose();
	}
	return	ret;
}

CNMSVoid ProgressBarClose( CNMSVoid )
{
	if( lpProgress != CNMSNULL ){
		CnmsFreeMem( (CNMSLPSTR)lpProgress );
	}
	lpProgress = CNMSNULL;

#ifdef	__CNMS_DEBUG_PROGRESS__
	DBGMSG( "[ProgressBarClose()].\n" );
#endif
	return;
}

CNMSInt32 ProgressBarStart(
		CNMSInt32		id,
		CNMSInt32		page )
{
	CNMSInt32		ret = CNMS_ERR, ldata;
	CNMSByte		pagesStr[256];

	if( ( lpProgress == CNMSNULL ) || ( lpProgress->id != CNMS_ERR ) || ( id < 0 ) || ( PROGRESSBAR_ID_MAX <= id ) ){
		DBGMSG( "[ProgressBarStart]Status is error.\n" );
		goto	EXIT;
	}
	
	gtk_window_set_title( GTK_WINDOW( lpProgress->dialog ), gettext( progressTitleStr[ id ] ) );

	lpProgress->id = id;
	lpProgress->progress_canceled = CNMS_FALSE;
	
	gtk_label_set_text( GTK_LABEL( lpProgress->buttonStr ), gettext( STR_PROGRESSBAR_BUTTON_CANCEL ) );
	gtk_widget_set_sensitive( lpProgress->button, TRUE );
	
	gtk_label_set_text( GTK_LABEL( lpProgress->captionStr ), gettext( progressTitleStr[ id ] ) );
	
	if ( id != PROGRESSBAR_ID_SEARCH ) {
		if( ( ldata = SetProgressValue( 0 ) ) == CNMS_ERR ){
			DBGMSG( "[ProgressBarStart]Error is occured in SetProgressValue.\n" );
			goto	EXIT;
		}
		if( page > 0 ) {	/* show pages */
			snprintf( pagesStr, 256, gettext( STR_PROGRESSBAR_PAGES ), page );
			gtk_label_set_text( GTK_LABEL( lpProgress->pagesStr ), pagesStr );
		}
		else {
			gtk_label_set_text( GTK_LABEL( lpProgress->pagesStr ), "" );
		}
		gtk_window_set_title( GTK_WINDOW( lpProgress->dialog ), "ScanGear" );
		gtk_widget_show( lpProgress->captionStr );
		
		/* ProgressBarShow(); */
		W1_ModalDialogShowAction( lpProgress->dialog, main_window );
	}
	else {	/* id == PROGRESSBAR_ID_SEARCH */
		gtk_label_set_text( GTK_LABEL( lpProgress->perStr ), gettext( STR_PROGRESSBAR_MESS_SEARCH ) );
		gtk_label_set_text( GTK_LABEL( lpProgress->pagesStr ), "" );
		
		gtk_widget_hide( lpProgress->captionStr );
		
		gtk_window_set_transient_for( GTK_WINDOW( lpProgress->dialog ), GTK_WINDOW( select_model_dialog ) );
		gtk_widget_show( lpProgress->dialog );
		gtk_widget_set_sensitive( select_model_dialog, FALSE );
	}

	if ( id == PROGRESSBAR_ID_SAVE || id == PROGRESSBAR_ID_TRANSFER_GIMP ) {
		usleep( 50000);		/* wait 50msec */
	}
	while( gtk_events_pending() ){
		gtk_main_iteration();
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PROGRESS__
	DBGMSG( "[ProgressBarStart(%d)]=%d.\n", id, ret );
#endif
	return	ret;
}

static CNMSInt32 SetProgressValue(
		CNMSInt32	percentVal )
{
	CNMSInt32		ret = CNMS_ERR;
	CNMSByte		str[ 16 ];

	if( ( percentVal < 0 ) || ( 100 < percentVal ) || ( lpProgress == CNMSNULL ) ){
		DBGMSG( "[SetProgressValue]Status is error.\n" );
		goto	EXIT;
	}
	if ( percentVal == 0 ) {
		lpProgress->prev = 0;
	}
	else if ( percentVal != lpProgress->prev ) {
		lpProgress->prev = percentVal;
	}
	else {
		/* skip update */
		ret = CNMS_NO_ERR_2;
		goto EXIT;
	}
	
	gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( lpProgress->bar ), (CNMSDec32)percentVal / 100 );
	/* percent value */
	snprintf( str, sizeof( str ), "%d%%", percentVal );
	gtk_label_set_text( GTK_LABEL( lpProgress->perStr ), str );

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PROGRESS__
//	DBGMSG( "\t[SetProgressValue(%d)]=%d.\n", percentVal, ret );
#endif
	return	ret;
}

CNMSVoid ProgressBarEnd( CNMSVoid )
{
	CNMSInt32	ldata;

	if( ( lpProgress == CNMSNULL ) || ( lpProgress->id == CNMS_ERR ) ){
		DBGMSG( "[ProgressBarEnd]Status is error.\n" );
		goto	EXIT;
	}
	
	if ( lpProgress->id != PROGRESSBAR_ID_SEARCH ) {
		if( !lpProgress->progress_canceled ) {
			if( ( ldata = SetProgressValue( 100 ) ) == CNMS_ERR ){
				DBGMSG( "[ProgressBarEnd]Error is occured in SetProgressValue.\n" );
				goto	EXIT;
			}
		}
		/* ProgressBarHide(); */
		gtk_window_set_transient_for( GTK_WINDOW( lpProgress->dialog ), CNMSNULL );
		W1_ModalDialogHideAction( lpProgress->dialog, CNMSNULL );
	}
	else {	/* lpProgress->id == PROGRESSBAR_ID_SEARCH */
		gtk_widget_hide( lpProgress->dialog );
	}
	
	while( gtk_events_pending() ){
		gtk_main_iteration();
	}

	lpProgress->id = CNMS_ERR;
	lpProgress->prev = 0;
EXIT:
#ifdef	__CNMS_DEBUG_PROGRESS__
	DBGMSG( "[ProgressBarEnd()].\n" );
#endif
	return;
}

CNMSInt32 ProgressBarUpdate(
		CNMSInt32		percentVal,
		CNMSInt32		page )
{
	CNMSInt32		ret = CNMS_ERR, ldata;
	CNMSByte		pagesStr[256];

	if( ( lpProgress == CNMSNULL ) || ( lpProgress->id == CNMS_ERR ) ){
		DBGMSG( "[ProgressBarUpdate]Status is error.\n" );
		goto	EXIT;
	}
	else if( lpProgress->progress_canceled ) {
		DBGMSG( "[ProgressBarUpdate] canceled.\n" );
		goto	EXIT;
	}
	else if( ( ldata = SetProgressValue( percentVal ) ) == CNMS_ERR ){
		DBGMSG( "[ProgressBarUpdate]Error is occured in SetProgressValue.\n" );
		goto	EXIT;
	}
	if ( ldata == CNMS_NO_ERR ) {
		if( page > 0 ) {	/* show pages */
			snprintf( pagesStr, 256, gettext( STR_PROGRESSBAR_PAGES ), page );
			gtk_label_set_text( GTK_LABEL( lpProgress->pagesStr ), pagesStr );
		}
		else {
			gtk_label_set_text( GTK_LABEL( lpProgress->pagesStr ), "" );
		}
		while( gtk_events_pending() ){
			gtk_main_iteration();
		}
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PROGRESS__
//	DBGMSG( "[ProgressBarUpdate(%d)]=%d.\n", percentVal, ret );
#endif
	return	ret;
}

CNMSVoid ProgressBarShow( CNMSVoid )
{
	if( ( lpProgress == CNMSNULL ) || ( lpProgress->id == CNMS_ERR ) ){
		DBGMSG( "[ProgressBarShow]Status is error.\n" );
		goto	EXIT;
	}
	W1_ModalDialogShowAction( lpProgress->dialog, main_window );

	while( gtk_events_pending() ){
		gtk_main_iteration();
	}
EXIT:
#ifdef	__CNMS_DEBUG_PROGRESS__
	DBGMSG( "[ProgressBarShow()].\n" );
#endif
	return;
}

CNMSVoid ProgressBarHide( CNMSVoid )
{
	if( ( lpProgress == CNMSNULL ) || ( lpProgress->id == CNMS_ERR ) ){
		DBGMSG( "[ProgressBarHide]Status is error.\n" );
		goto	EXIT;
	}
	W1_ModalDialogHideAction( lpProgress->dialog, CNMSNULL );

	while( gtk_events_pending() ){
		gtk_main_iteration();
	}

EXIT:
#ifdef	__CNMS_DEBUG_PROGRESS__
	DBGMSG( "[ProgressBarHide()].\n" );
#endif
	return;
}

CNMSInt32 ProgressBarGetId( CNMSVoid )
{
	CNMSInt32	ret = CNMS_ERR;

	if( ( lpProgress == CNMSNULL ) || ( lpProgress->id == CNMS_ERR ) ){
		DBGMSG( "[ProgressBarGetId]Status is error.\n" );
		goto	EXIT;
	}
	ret = lpProgress->id;
EXIT:
#ifdef	__CNMS_DEBUG_PROGRESS__
	DBGMSG( "[ProgressBarGetId()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSVoid ProgressBarCanceled( CNMSVoid )
{
	if( ( lpProgress == CNMSNULL ) || ( lpProgress->id == CNMS_ERR ) ){
		DBGMSG( "[ProgressBarCanceled]Status is error.\n" );
		goto	EXIT;
	}
	lpProgress->progress_canceled = CNMS_TRUE;
	gtk_widget_set_sensitive( lpProgress->button, FALSE );
	
	switch( lpProgress->id ){
		case	PROGRESSBAR_ID_SCAN:
			W1_PreviewCancel();
			break;
		case	PROGRESSBAR_ID_SAVE:
			CnmsScanFileCancel();
			break;
#ifdef	__GIMP_PLUGIN_ENABLE__
		case	PROGRESSBAR_ID_TRANSFER_GIMP:
			ScanGimpCancel();
			break;
#endif
		case	PROGRESSBAR_ID_SEARCH:
			break;
		default:
			break;
	}
	
EXIT:
#ifdef	__CNMS_DEBUG_PROGRESS__
	DBGMSG( "[ProgressBarCanceled()].\n" );
#endif
	return;
}

CNMSVoid ProgressBarDeleted( CNMSVoid )
{
	if( ( lpProgress == CNMSNULL ) || ( lpProgress->id == CNMS_ERR ) ){
		DBGMSG( "[ProgressBarDeleted]Status is error.\n" );
		goto	EXIT;
	}
EXIT:
#ifdef	__CNMS_DEBUG_PROGRESS__
	DBGMSG( "[ProgressBarDeleted()].\n" );
#endif
	return;
}

int ProgressBarUpdatePulse( void )
{
	int	ret = -1;
	if( ( lpProgress == CNMSNULL ) || ( lpProgress->id == CNMS_ERR ) ){
		DBGMSG( "[ProgressBarUpdatePulse]Status is error.\n" );
		goto	EXIT;
	}
	
	if ( lpProgress->id == PROGRESSBAR_ID_SEARCH ) {
		if( lpProgress->progress_canceled ) {
			ret = -1;
			goto EXIT;
		}
		gtk_progress_bar_pulse( GTK_PROGRESS_BAR( lpProgress->bar ) );
		while( gtk_events_pending() ){
			gtk_main_iteration();
		}
		ret = 0;
	}
EXIT:
	return ret;
}

CNMSVoid ProgressBarWaitFinish( CNMSVoid )
{
	if( ( lpProgress == CNMSNULL ) || ( lpProgress->id == CNMS_ERR ) ){
		DBGMSG( "[ProgressBarWaitEnd]Status is error.\n" );
		goto	EXIT;
	}
	if ( lpProgress->id == PROGRESSBAR_ID_SEARCH ) {
		gtk_label_set_text( GTK_LABEL( lpProgress->perStr ), gettext( STR_PROGRESSBAR_MESS_FINISH_SEARCH ) );
	}
	gtk_widget_set_sensitive( lpProgress->button, FALSE );
	while( gtk_events_pending() ){
		usleep( 50000);		/* wait 50msec */
		gtk_main_iteration();
	}
EXIT:
	return;
}

CNMSBool ProgressBarGetCanceledStatus( CNMSVoid )
{
	return lpProgress->progress_canceled;
}

#endif	/* _PROGRESS_BAR_C_ */
