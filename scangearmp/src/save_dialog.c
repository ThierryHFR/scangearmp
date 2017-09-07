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

#ifndef	_SAVE_DIALOG_C_
#define	_SAVE_DIALOG_C_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <errno.h>
#include <limits.h>

#include "interface.h"
#include "support.h"

#include "cnmstype.h"
#include "cnmsfunc.h"

#include "com_gtk.h"
#include "w1.h"
#include "save_dialog.h"
#include "scanstdlg.h"
#include "file_control.h"
#include "cnmsstrings.h"

#include "errors.h"

//#define	__CNMS_DEBUG_SAVE_DIALOG__


enum{
	SAVE_EXT_ARRAY_0 = 0,
	SAVE_EXT_ARRAY_1,
	SAVE_EXT_ARRAY_2,
	SAVE_EXT_ARRAY_MAX,
};

enum{
	SAVE_ERROR_FALSE = -1,
	SAVE_ERROR_OVERWRITE,
	SAVE_ERROR_NO_INPUT_FNAME,
	SAVE_ERROR_NO_ACCESS_PERM,
	SAVE_ERROR_INVALID_DIR,
	SAVE_ERROR_SERIOUS_ERROR1,
	SAVE_ERROR_MAX,
};

typedef struct{
	CNMSInt32		num;
	CNMSLPSTR		str;
}SAVEERROR; 


typedef struct{
	CNMSInt32		val;
	CNMSLPSTR		strArray[ SAVE_EXT_ARRAY_MAX ];
	CNMSLPSTR		strInfo;
}CNMSSAVEEXTCOMP, *LPCNMSSAVEEXTCOMP;

static CNMSSAVEEXTCOMP extComp[ CNMS_SAVE_FORMAT_MAX ] = {
	{ CNMS_SAVE_FORMAT_PNG,	{ ".png\0", CNMSNULL, CNMSNULL }, "PNG\0" },
	{ CNMS_SAVE_FORMAT_PDF,	{ ".pdf\0", CNMSNULL, CNMSNULL }, "PDF\0" },
	{ CNMS_SAVE_FORMAT_PNM,	{ ".pnm\0", CNMSNULL, CNMSNULL }, "PNM\0" },
};

typedef struct{
	GtkWidget		*save_main_dialog;
	GtkWidget		*save_overwrite_dialog;
	GtkWidget		*save_error_dialog;
	CNMSInt32		file_format;
	CNMSByte		select_path[ PATH_MAX ];
}SAVEDIALOGCOMP, *LPSAVEDIALOGCOMP;

LPSAVEDIALOGCOMP	lpSave = CNMSNULL;

static CNMSInt32 get_file_status( CNMSLPSTR path );
static CNMSInt32 AddExt( CNMSLPSTR lpName, LPCNMSSAVEEXTCOMP lpExtComp );
static CNMSInt32 JudgeExt( CNMSLPSTR lpName, LPCNMSSAVEEXTCOMP lpExtComp );

static CNMSVoid dialog_save_error_disp( CNMSInt32 error_num, GtkWidget* child, GtkWidget* parent );

static SAVEERROR SaveError[] = {
	{ SAVE_ERROR_NO_INPUT_FNAME,	MSG_SAVE_ERROR_NO_INPUT_FNAME },
	{ SAVE_ERROR_NO_ACCESS_PERM,	MSG_SAVE_ERROR_NO_ACCESS_PERM },
	{ SAVE_ERROR_INVALID_DIR,		MSG_SAVE_ERROR_INVALID_DIR },
	{ SAVE_ERROR_SERIOUS_ERROR1,	MSG_SAVE_ERROR_SERIOUS_ERROR1 },
	{ SAVE_ERROR_FALSE, "\0" },
};

static CNMSInt32	current_save_error_num = SAVE_ERROR_FALSE;

static CNMSInt32	prev_format_index = 0;	/* default */
static CNMSBool		png_enable = CNMS_FALSE;

CNMSInt32 SaveDialog_Open( CNMSVoid )
{
	GtkWidget	*combobox_save_main;
	CNMSInt32	ret = CNMS_ERR, i, j, format_start;
	CNMSInt8	extbuf[32];
	GtkWidget	*filechooserwidget_save_main;
	char 		*home_dir = CNMSNULL;
	
	if( lpSave != CNMSNULL ){
		SaveDialog_Close();
	}
	if( ( lpSave = (LPSAVEDIALOGCOMP)CnmsGetMem( sizeof( SAVEDIALOGCOMP ) ) ) == CNMSNULL ){
		DBGMSG( "[SaveDialog_Open]Error is occured in CnmsGetMem.\n" );
		goto	EXIT;
	}
	lpSave->file_format = CNMS_ERR;
	lpSave->select_path[ 0 ] = '\0';
	if( ( lpSave->save_main_dialog = create_save_main_dialog() ) == CNMSNULL ){
		DBGMSG( "[SaveDialog_Open]Error is occured in create_save_main_dialog.\n" );
		goto	EXIT;
	}
	else if( ( lpSave->save_overwrite_dialog = create_save_overwrite_dialog() ) == CNMSNULL ){
		DBGMSG( "[SaveDialog_Open]Error is occured in create_save_overwrite_dialog.\n" );
		goto	EXIT;
	}
	else if( ( lpSave->save_error_dialog = create_save_error_dialog() ) == CNMSNULL ){
		DBGMSG( "[SaveDialog_Open]Error is occured in create_save_error_dialog.\n" );
		goto	EXIT;
	}
	else if( ( combobox_save_main = lookup_widget( lpSave->save_main_dialog, "save_file_type_combo" ) ) == CNMSNULL ){
		DBGMSG( "[SaveDialog_Open]Can't look up comboBox(save_file_type_combo).\n" );
		goto	EXIT;
	}
	else if( ( filechooserwidget_save_main = lookup_widget( lpSave->save_main_dialog, "save_main_filechooser" ) ) == CNMSNULL ){
		DBGMSG( "[SaveDialog_Exec]Can't look up widget.\n" );
		goto	EXIT;
	}
	png_enable = CnmsLibpngOpen();
	
	ComGtk_ClearCombo( ( GtkComboBox* )combobox_save_main );

	format_start = ( png_enable ) ? CNMS_SAVE_FORMAT_PNG : CNMS_SAVE_FORMAT_PDF;
	for( i = format_start ; i < CNMS_SAVE_FORMAT_MAX ; i ++ ){
		snprintf( extbuf, sizeof(extbuf) - 1, "%s (*%s)", extComp[ i ].strInfo, extComp[ i ].strArray[ SAVE_EXT_ARRAY_0 ] );
		extbuf[ sizeof(extbuf) - 1 ] = '\0';

		gtk_combo_box_append_text( GTK_COMBO_BOX( combobox_save_main ), extbuf );
	}
	
	if( ( home_dir = getenv("HOME") ) != CNMSNULL ) {
		gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER( filechooserwidget_save_main ), home_dir );
	}
	
	gtk_window_set_default_size( (GtkWindow *)lpSave->save_main_dialog, gdk_screen_width() * 0.5, gdk_screen_height() * 0.5 );
	
	ret = CNMS_NO_ERR;
EXIT:
	if( ret != CNMS_NO_ERR ){
		SaveDialog_Close();
	}
#ifdef	__CNMS_DEBUG_SAVE_DIALOG__
	DBGMSG( "[SaveDialog_Open()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSVoid SaveDialog_Close( CNMSVoid )
{
	CnmsLibpngClose();
	
	if( lpSave != CNMSNULL ){
		CnmsFreeMem( (CNMSLPSTR)lpSave );
	}
	lpSave = CNMSNULL;
	
#ifdef	__CNMS_DEBUG_SAVE_DIALOG__
	DBGMSG( "[SaveDialog_Close()].\n" );
#endif
	return;
}

CNMSInt32 SaveDialog_Show( CNMSVoid )
{
	CNMSInt32	ret = CNMS_ERR, i, format_start;
	GtkWidget	*widget;
	CNMSInt8	extbuf[32];

	if( lpSave == CNMSNULL ){
		DBGMSG( "[SaveDialog_Show]Status is error.\n" );
		goto	EXIT;
	}
	else if( ( widget = lookup_widget( lpSave->save_main_dialog, "save_file_type_combo" ) ) == CNMSNULL ){
		DBGMSG( "[SaveDialog_Show]Can't look up comboBox(save_file_type_combo).\n" );
		goto	EXIT;
	}
	
	ComGtk_ClearCombo( ( GtkComboBox* )widget );
	format_start = ( png_enable ) ? CNMS_SAVE_FORMAT_PNG : CNMS_SAVE_FORMAT_PDF;
	for( i = format_start ; i < CNMS_SAVE_FORMAT_MAX ; i ++ ){
		if( ( W1_GetSelectSourceADF() == CNMS_FALSE ) || ( extComp[ i ].val == CNMS_SAVE_FORMAT_PDF ) ){
			snprintf( extbuf, sizeof(extbuf) - 1, "%s (*%s)", extComp[ i ].strInfo, extComp[ i ].strArray[ SAVE_EXT_ARRAY_0 ] );
			extbuf[ sizeof(extbuf) - 1 ] = '\0';
			gtk_combo_box_append_text( GTK_COMBO_BOX( widget ), extbuf );
		}
	}
	
	/* Set previous ext name */
	if ( 0 <= prev_format_index && prev_format_index < CNMS_SAVE_FORMAT_MAX ) {
		if( W1_GetSelectSourceADF() == CNMS_TRUE ){
			prev_format_index = 0;
		}
		gtk_combo_box_set_active( GTK_COMBO_BOX( widget ), prev_format_index );
	}
	else {
		DBGMSG( "[SaveDialog_Show]Invalid ext name.\n" );
		goto	EXIT;
	}

	W1_ModalDialogShowAction( lpSave->save_main_dialog, main_window );
	
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SAVE_DIALOG__
	DBGMSG( "[SaveDialog_Show()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSVoid SaveDialog_Hide( CNMSVoid )
{
	GtkWidget	*widget;
	
	if( lpSave == CNMSNULL ){
		DBGMSG( "[SaveDialog_Hide]Status is error.\n" );
		goto	EXIT;
	}
	else if( ( widget = lookup_widget( lpSave->save_main_dialog, "save_file_type_combo" ) ) == CNMSNULL ){
		DBGMSG( "[SaveDialog_Hide]Can't look up comboBox(save_file_type_combo).\n" );
		goto	EXIT;
	}
	/* Get current ext name */
	else if( ( prev_format_index = gtk_combo_box_get_active( GTK_COMBO_BOX( widget ) ) ) < 0 ){
		DBGMSG( "[SaveDialog_Hide]Can't get ext str.\n" );
		goto	EXIT;
	}
	W1_ModalDialogHideAction( lpSave->save_main_dialog, main_window );
	
EXIT:
#ifdef	__CNMS_DEBUG_SAVE_DIALOG__
	DBGMSG( "[SaveDialog_Hide()].\n" );
#endif
	return;
}

CNMSInt32 SaveDialog_Exec( CNMSVoid )
{
	GtkWidget	*filechooserwidget_save_main;
	GtkWidget	*combobox_save_main;
	CNMSInt32	ret = CNMS_ERR, i, ldata, ExtAddFlag, format_index;

	CNMSLPSTR	filename = CNMSNULL;

	if( lpSave == CNMSNULL ){
		DBGMSG( "[SaveDialog_Exec]Status is error.\n" );
		goto	EXIT;
	}
	else if( ( filechooserwidget_save_main = lookup_widget( lpSave->save_main_dialog, "save_main_filechooser" ) ) == CNMSNULL ){
		DBGMSG( "[SaveDialog_Exec]Can't look up widget.\n" );
		goto	EXIT_ERR;
	}
	else if( ( combobox_save_main = lookup_widget( lpSave->save_main_dialog, "save_file_type_combo" ) ) == CNMSNULL ){
		DBGMSG( "[SaveDialog_Exec]Can't look up combo.\n" );
		goto	EXIT_ERR;
	}
	/* Get file name */
	if( ( filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER( filechooserwidget_save_main ) ) ) == CNMSNULL ){
		dialog_save_error_disp( SAVE_ERROR_NO_INPUT_FNAME, lpSave->save_error_dialog, lpSave->save_main_dialog );
		DBGMSG( "[SaveDialog_Exec] No input file name!\n" );
		goto	EXIT;
	}
	CnmsStrCopy( filename, lpSave->select_path, PATH_MAX );
	g_free( filename );
	
	/* Get ext name */
	if( W1_GetSelectSourceADF() == CNMS_FALSE ){
		if( ( format_index = gtk_combo_box_get_active( GTK_COMBO_BOX( combobox_save_main ) ) ) < 0 ){
			DBGMSG( "[SaveDialog_Exec]Can't get ext str.\n" );
			goto	EXIT_ERR;
		}
		/* disable PNG -> format_index = 0:PDF (!=CNMS_SAVE_FORMAT_PDF) -> +1 -> format_index = 1 (==CNMS_SAVE_FORMAT_PDF) */
		lpSave->file_format = ( png_enable ) ? format_index : (format_index + 1);
	}
	else{
		lpSave->file_format = CNMS_SAVE_FORMAT_PDF;
	}

	/* Judge ext str */
	if( ( ExtAddFlag = JudgeExt( lpSave->select_path, extComp + lpSave->file_format ) ) == CNMS_FALSE ){
		if( ( ldata = AddExt( lpSave->select_path, extComp + lpSave->file_format ) ) != CNMS_NO_ERR ){
			DBGMSG( "[SaveDialog_Exec]Can't add ext str.\n" );
			goto	EXIT;
		}
	}

	switch( FileControlGetStatus( lpSave->select_path, PATH_MAX ) ){
		case	FILECONTROL_STATUS_NOT_EXIST:		/* No Error Save */
			SaveDialog_Hide();
			CnmsScanFileExec();
			break;
		
		case	FILECONTROL_STATUS_WRITE_OK:		/* Overwrite Save */
			dialog_save_error_disp( SAVE_ERROR_OVERWRITE, lpSave->save_overwrite_dialog, lpSave->save_main_dialog );
			break;
		
		case	FILECONTROL_STATUS_WRITE_NG:		/* No Permission */
			dialog_save_error_disp( SAVE_ERROR_NO_ACCESS_PERM, lpSave->save_error_dialog, lpSave->save_main_dialog );
			break;
		
		case	FILECONTROL_STATUS_ISNOT_FILE:		/* Not file */
			dialog_save_error_disp( SAVE_ERROR_SERIOUS_ERROR1, lpSave->save_error_dialog, lpSave->save_main_dialog );
			break;
		
		case	FILECONTROL_STATUS_INVALID_DIR:	/* Invalid directory */
			dialog_save_error_disp( SAVE_ERROR_INVALID_DIR, lpSave->save_error_dialog, lpSave->save_main_dialog );
			break;
		
		case	FILECONTROL_STATUS_OTHER_ERROR:	/* Other error */
			dialog_save_error_disp( SAVE_ERROR_SERIOUS_ERROR1, lpSave->save_error_dialog, lpSave->save_main_dialog );
			break;
		
		default:	/* May not use */
			dialog_save_error_disp( SAVE_ERROR_SERIOUS_ERROR1, lpSave->save_error_dialog, lpSave->save_main_dialog );
			break;
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_SAVE_DIALOG__
	DBGMSG( "[SaveDialog_Exec()]=%d.\n", ret );
#endif
	return	ret;

EXIT_ERR:
	dialog_save_error_disp( SAVE_ERROR_SERIOUS_ERROR1, lpSave->save_error_dialog, lpSave->save_main_dialog );
	goto	EXIT;
}

static CNMSInt32 AddExt(
		CNMSLPSTR			lpName,
		LPCNMSSAVEEXTCOMP	lpExtComp )
{
	CNMSInt32	ret = CNMS_ERR,
				ldata;

	while( *lpName != '\0' ){
		lpName ++;
	}
	
	if( ( ldata = CnmsStrCat( lpExtComp->strArray[ 0 ], lpName, PATH_MAX ) ) < 0 ){
		DBGMSG( "[AddExt]Can't add file ext string.\n" );
		goto	EXIT;
	}

	ret = CNMS_NO_ERR;

EXIT:

	return	ret;
}


static CNMSInt32 JudgeExt(
		CNMSLPSTR			lpName,
		LPCNMSSAVEEXTCOMP	lpExtComp )
{
	CNMSInt32	ret = CNMS_TRUE,
				NumExt,
				NumFile;
	CNMSUInt8	BufExt[ PATH_MAX ],
				BufFile[ PATH_MAX ];
	
	CnmsCopyMem( (CNMSLPSTR)( lpExtComp->strArray[ 0 ] ), (CNMSLPSTR)BufExt, strlen( lpExtComp->strArray[ 0 ] ) + 1 );
	for( NumExt = 0 ; BufExt[NumExt] != '\0' ; NumExt++ );

	CnmsCopyMem( (CNMSLPSTR)lpName, (CNMSLPSTR)BufFile, strlen( lpName ) + 1 );
	for( NumFile = 0 ; BufFile[NumFile] != '\0' ; NumFile++ );

	while( ( NumFile >= 0 ) && ( NumExt >= 0 ) ){
		if( BufExt[NumExt] != BufFile[NumFile] ){
			ret = CNMS_FALSE;
			break;
		}
		NumExt--;
		NumFile--;
	}
	
	return	ret;
}

CNMSLPSTR SaveDialog_GetPath( CNMSVoid )
{
	CNMSLPSTR	lpRet = CNMSNULL;

	if( ( lpSave == CNMSNULL ) || ( lpSave->select_path[ 0 ] == '\0' ) ){
		DBGMSG( "[SaveDialog_GetPath]Status is error.\n" );
		goto	EXIT;
	}
	lpRet = lpSave->select_path;
EXIT:
#ifdef	__CNMS_DEBUG_SAVE_DIALOG__
	DBGMSG( "[SaveDialog_GetPath()]=%s.\n", lpRet );
#endif
	return	lpRet;
}

CNMSInt32 SaveDialog_GetFileFormat( CNMSVoid )
{
	CNMSInt32	ret = CNMS_ERR;

	if( ( lpSave == CNMSNULL ) || ( lpSave->file_format == CNMS_ERR ) ){
		DBGMSG( "[SaveDialog_GetFileFormat]Status is error.\n" );
		goto	EXIT;
	}
	ret = lpSave->file_format;
EXIT:
#ifdef	__CNMS_DEBUG_SAVE_DIALOG__
	DBGMSG( "[SaveDialog_GetFileFormat()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSVoid SaveDialog_HideAndShowButton( CNMSVoid )
{
	if( lpSave == CNMSNULL ){
		DBGMSG( "[SaveDialog_HideAndShowButton]Status is error.\n" );
		goto	EXIT;
	}
	ComGtk_WidgetHideAndShow( lookup_widget( lpSave->save_main_dialog, "save_main_save_button" ) );
	ComGtk_WidgetHideAndShow( lookup_widget( lpSave->save_main_dialog, "save_main_cancel_button" ) );
EXIT:
#ifdef	__CNMS_DEBUG_SAVE_DIALOG__
	DBGMSG( "[SaveDialog_HideAndShowButton()].\n" );
#endif
	return;
}

static CNMSVoid dialog_save_error_disp(
		CNMSInt32		error_num,
		GtkWidget		*child,
		GtkWidget		*parent )
{
	CNMSInt32	i;
	GtkWidget	*label;
	CNMSLPSTR	str = CNMSNULL;
	GtkWidget	*widget;

	current_save_error_num = error_num;

	if( error_num != SAVE_ERROR_OVERWRITE )
	{
		label = lookup_widget( child, "save_error_dialog_label" );

		for( i=0 ; SaveError[i].num != SAVE_ERROR_FALSE ; i++ ){
			if( SaveError[i].num == error_num ){
				str = (SaveError[i]).str;
				break;
			}		
		}
		
		gtk_label_set_text( GTK_LABEL( label ), gettext( str ) );
	}
		
	if( GTK_WIDGET_VISIBLE( child ) == 0 ){
		gtk_widget_set_sensitive( lpSave->save_main_dialog, FALSE );
		gtk_window_set_transient_for( GTK_WINDOW( child ), GTK_WINDOW( parent ) );
		gtk_widget_show( child );
		
		if( error_num == SAVE_ERROR_OVERWRITE ) {
			/* set focus -> Cancel */
			W1_WidgetGrabFocus( child, "save_overwrite_cancel_button" );
		}
	}
EXIT:
#ifdef	__CNMS_DEBUG_SAVE_DIALOG__
	DBGMSG( "[dialog_save_error_disp()].\n" );
#endif

	return;
}

CNMSVoid dialog_ok_del_clicked( CNMSVoid )
{
	if( current_save_error_num == SAVE_ERROR_OVERWRITE ){
		gtk_window_set_transient_for( GTK_WINDOW( lpSave->save_overwrite_dialog ), NULL );
		gtk_widget_hide( lpSave->save_overwrite_dialog );
		gtk_window_set_transient_for( GTK_WINDOW( lpSave->save_error_dialog ), NULL );
		gtk_widget_hide( lpSave->save_error_dialog );
	}
	else{
		gtk_window_set_transient_for( GTK_WINDOW( lpSave->save_error_dialog ), NULL );
		gtk_widget_hide( lpSave->save_error_dialog );
	}
	gtk_widget_set_sensitive( lpSave->save_main_dialog, TRUE );
	W1_HideAndShowAllButton();

#ifdef	__CNMS_DEBUG_SAVE_DIALOG__
	DBGMSG( "[dialog_ok_del_clicked()].\n" );
#endif
	return;
}

CNMSInt32 SaveDialog_GetSensitive( CNMSVoid )
{
	if( lpSave == CNMSNULL ){
		return CNMS_TRUE;
	}
	else if( lpSave->save_main_dialog == CNMSNULL ){
		return CNMS_TRUE;
	}
	else{
		return ( GTK_WIDGET_SENSITIVE( lpSave->save_main_dialog ) );
	}
}



#endif	/* _SAVE_DIALOG_C_ */
