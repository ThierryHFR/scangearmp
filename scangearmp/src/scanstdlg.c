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

#ifndef _SCANSTDLG_C_
#define _SCANSTDLG_C_

#include "w1.h"
#include "scanstdlg.h"
#include "scanmsg.h"
#include "progress_bar.h"
#include "com_gtk.h"

#include "errors.h"

static ERROR_MSG_TABLE *GetErrorMessage( CNMSInt32 code );


static CNMSInt32 lastErrorQuit;

CNMSInt32 ShowErrorDialog( CNMSVoid )
{
	SANE_Status		status;
	CNMSInt32		errorCode = 0;
	CNMSInt32		ret = ERROR_QUIT_FALSE, ldata;

	if ( lastIOErrCode ) {
		errorCode = -lastIOErrCode;
		if( errorCode != ERR_CODE_ENOSPC ) {
			errorCode = ERR_CODE_INT;
		}
	}
	else if ( lastModuleErrCode ) {
		errorCode = ERR_CODE_INT;
	}
	else {
		errorCode = lastBackendErrCode;
	}
	lastErrorQuit = ERROR_QUIT_FALSE;

	/* error occurred. */
	if ( errorCode ) {
	
		GtkWidget			*label = lookup_widget( error_dialog, "error_dialog_msg" );
		CNMSByte			str[1024];
		ERROR_MSG_TABLE		*em_tbl;

		if ( ( em_tbl = GetErrorMessage( errorCode ) ) != NULL ) {
			gtk_label_set_text( GTK_LABEL( label ), gettext( em_tbl->msg ) );
			lastErrorQuit = em_tbl->quit;
		}
		else {
			snprintf(str, sizeof(str), "returned error code : %d", errorCode);
			gtk_label_set_text( GTK_LABEL( label ), gettext( str ) );
			lastErrorQuit = ERROR_QUIT_TRUE;
		}
		
		/* show error dialog */
		W1_ModalDialogShowAction( error_dialog, CNMSNULL );
	}
	ret = lastErrorQuit;
	if( GTK_WIDGET_VISIBLE( error_dialog ) == CNMS_TRUE ){
		if( ( ldata = ComGtk_GtkMain() ) == CNMS_ERR ){
			DBGMSG( "[ShowErrorDialog]Function error by ComGtk_GtkMain() = %d.\n",ldata );
			ret = ldata;
		}
	}
	
_EXIT:
	/* clear backend error code. */
	lastBackendErrCode = 0;
	
	return ret;
}


void error_dialog_ok_del_clicked()
{
	CNMSInt32	ldata;
	
	/* hide calibration dialog */
	if ( GTK_WIDGET_VISIBLE( calibration_dialog ) ) {
		W1_ModalDialogHideAction( calibration_dialog, main_window );
	}
	/* hide progress dialog */
	ProgressBarHide();

	if( GTK_WIDGET_VISIBLE( error_dialog ) == CNMS_TRUE ){
		if( ( ldata = ComGtk_GtkMainQuit( COMGTK_GTKMAIN_OK ) ) == CNMS_ERR ){
			DBGMSG( "[error_dialog_ok_del_clicked]Function error by ComGtk_GtkMain() = %d.\n",ldata );
		}			
	}
	W1_ModalDialogHideAction( error_dialog, CNMSNULL );

	return;
}


static ERROR_MSG_TABLE *GetErrorMessage( CNMSInt32 code )
{
	CNMSInt32			i, type;
	ERROR_MSG_TABLE		*em_table = NULL;
	
	for( i = 0, type = -1; error_type_index_table[i].code != -1;i++ ) {
		if ( code == error_type_index_table[i].code ) {
			type = error_type_index_table[i].type;
			break;
		}
	}
	if ( error_type_index_table[i].code < 0 ) {
		type = error_type_index_table[i].type; /* FATAL */
	}
	
	for( i = 0; error_msg_table[i].type != -1;i++ ) {
		if ( type == error_msg_table[i].type ) {
			em_table = &error_msg_table[i];
			break;
		}
	}

_EXIT:
	return em_table;
}

#endif	/* _SCANSTDLG_C_ */
