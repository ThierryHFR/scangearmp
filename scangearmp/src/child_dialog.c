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

#ifndef	_CHILD_DIALOG_C_
#define	_CHILD_DIALOG_C_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#include "support.h"
#include "interface.h"

#include "cnmstype.h"
#include "cnmsfunc.h"
#include "com_gtk.h"

#include "keep_setting.h"
#include "child_dialog.h"
#include "w1.h"
#include "cnmsstrings.h"

/*	#define	__CNMS_DEBUG_CHILD_DIALOG__	*/

enum{
	CHILDDIALOG_TYPE_1 = 0,	/* with check box and OK/Cancel Button */
	CHILDDIALOG_TYPE_2,		/* with OK Button */
	CHILDDIALOG_TYPE_3,		/* with OK/Cancel Button */
	CHILDDIALOG_TYPE_MAX,
};

static const CNMSInt32	ChildDialogMaxPixels[ CNMS_DIM_MAX ]   = {  21000,   30000 };
static const CNMSLPSTR	ChildDialogMaxPixelStr[ CNMS_DIM_MAX ] = { "21000", "30000" };

/* message strings -> cnmsstrings.h */

/* data struct */
typedef struct{
	CNMSInt32		type;
	CNMSInt32		keepSettingId;
	CNMSLPSTR		lpStr1;
	CNMSLPSTR		lpStr2;
	CNMSInt32		retOKCancel;
}CHILDDATACOMP, *LPCHILDDATACOMP;

static const CHILDDATACOMP ChildDataComp[ CHILDDIALOG_ID_MAX ] = {
	{ CHILDDIALOG_TYPE_1,	KEEPSETTING_ID_RESET_SETTING,	MSG_CHILD_DLG_RESET,				MSG_CHILD_DLG_NO_MESS,	0 },	/* CHILDDIALOG_ID_RESET_SETTING */
	{ CHILDDIALOG_TYPE_1,	KEEPSETTING_ID_OVER_100MB,		MSG_CHILD_DLG_100MBOVER,			MSG_CHILD_DLG_NO_MESS,	0 },	/* CHILDDIALOG_ID_OVER_100MB */
	{ CHILDDIALOG_TYPE_2,	CNMS_ERR,						MSG_CHILD_DLG_OVER_PIXEL,			CNMSNULL,				0 },	/* CHILDDIALOG_ID_PIXELS_OVER */
	{ CHILDDIALOG_TYPE_2,	CNMS_ERR,						MSG_CHILD_DLG_WARN_MOIRE,			CNMSNULL,				0 },	/* CHILDDIALOG_ID_WARNING_MOIRE */
	{ CHILDDIALOG_TYPE_3,	CNMS_ERR,						MSG_CHILD_DLG_NO_PAPER,				CNMSNULL,				1 },	/* CHILDDIALOG_ID_NO_PAPER */
	{ CHILDDIALOG_TYPE_2,	CNMS_ERR,						MSG_CHILD_DLG_NOT_FOUND,			CNMSNULL,				0 },	/* CHILDDIALOG_ID_NOT_FOUND */
	{ CHILDDIALOG_TYPE_2,	CNMS_ERR,						MSG_CHILD_DLG_OVER_DATASIZE,		CNMSNULL,				0 },	/* CHILDDIALOG_ID_DATASIZE_OVER */
	{ CHILDDIALOG_TYPE_3,	CNMS_ERR,						MSG_CHILD_DLG_SETTING_APPLY,		CNMSNULL,				1 },	/* CHILDDIALOG_ID_SETTING_APPLY */
	{ CHILDDIALOG_TYPE_3,	CNMS_ERR,						MSG_CHILD_DLG_AUTOPOWEROFF_APPLY,	CNMSNULL,				1 },	/* CHILDDIALOG_ID_AUTOPOWEROFF_APPLY */
};

/* work struct */
typedef struct{
	CNMSInt32		id;
	GtkWidget		*show_dialog;
	GtkWidget		*dialog1;	/* with check box and OK/Cancel Button */
	GtkWidget		*dialog2;	/* with OK Button */
	GtkWidget		*dialog3;	/* with OK/Cancel Button */
	GtkWidget		*parent;	/* parent widget(window/dialog) */
	CNMSInt32		ret;
}CHILDDIALOGCOMP, *LPCHILDDIALOGCOMP;

static LPCHILDDIALOGCOMP	lpChild = CNMSNULL;

static CNMSInt32 SubShowDialog1( CNMSInt32 id );
static CNMSInt32 SubShowDialog2( CNMSInt32 id );
static CNMSInt32 SubShowDialog3( CNMSInt32 id );

static CNMSInt32 GetPixelsOverString( CNMSLPSTR lpSrc, CNMSLPSTR lpDst );
static CNMSInt32 CheckDataSize( CNMSVoid );

CNMSInt32 ChildDialogOpen( CNMSVoid )
{
	CNMSInt32		ret = CNMS_ERR;

	/* check */
	if( lpChild != CNMSNULL ){
		ChildDialogClose();
	}
	if( ( lpChild = (LPCHILDDIALOGCOMP)CnmsGetMem( sizeof( CHILDDIALOGCOMP ) ) ) == CNMSNULL ){
		DBGMSG( "[ChildDialogOpen]Error is occured in CnmsGetMem.\n" );
		goto	EXIT;
	}
	/* set parameter */
	lpChild->id = CNMS_ERR;
	lpChild->show_dialog = CNMSNULL;
	lpChild->parent = CNMSNULL;
	/* OK/Cancel/CheckBox */
	if( ( lpChild->dialog1 = create_dialog_child_with_checkBox() ) == CNMSNULL ){
		DBGMSG( "[ChildDialogOpen]Error is occured in create_dialog_child_with_checkBox.\n" );
		goto	EXIT;
	}
	/* OK only */
	else if( ( lpChild->dialog2 = create_dialog_child_with_ok() ) == CNMSNULL ){
		DBGMSG( "[ChildDialogOpen]Error is occured in create_dialog_child_normal.\n" );
		goto	EXIT;
	}
	/* OK/Cancel */
	else if( ( lpChild->dialog3 = create_dialog_child_with_okcancel() ) == CNMSNULL ){
		DBGMSG( "[ChildDialogOpen]Error is occured in create_dialog_child_normal.\n" );
		goto	EXIT;
	}

	ret = CNMS_NO_ERR;
EXIT:
	if( ret != CNMS_NO_ERR ){
		ChildDialogClose();
	}
#ifdef	__CNMS_DEBUG_CHILD_DIALOG__
	DBGMSG( "[ChildDialogOpen()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSVoid ChildDialogClose( CNMSVoid )
{
	if( lpChild != CNMSNULL ){
		CnmsFreeMem( (CNMSLPSTR)lpChild );
	}
	lpChild = CNMSNULL;

#ifdef	__CNMS_DEBUG_CHILD_DIALOG__
	DBGMSG( "[ChildDialogClose()].\n" );
#endif
	return;
}

CNMSInt32 ChildDialogShow(
		CNMSInt32		id,
		GtkWidget		*parent )
{
	CNMSInt32		ret = CNMS_ERR, ldata;
	GtkWidget		*widget;

	if( ( id < 0 ) || ( CHILDDIALOG_ID_MAX <= id ) || ( lpChild == CNMSNULL ) || ( lpChild->id != CNMS_ERR ) ){
		DBGMSG( "[ChildDialogShow]Parameter is error.\n" );
		goto	EXIT;
	}
	
	switch( ChildDataComp[ id ].type ){
		case	CHILDDIALOG_TYPE_1:
			ldata = SubShowDialog1( id );
			break;
		case	CHILDDIALOG_TYPE_2:
			ldata = SubShowDialog2( id );
			break;
		case	CHILDDIALOG_TYPE_3:
			ldata = SubShowDialog3( id );
			break;
		default:
			ldata = CNMS_ERR;
			break;
	}
	if( ldata != CNMS_NO_ERR ){
		ret = ldata;	/* CNMS_NO_ERR_2(no dialog) or CNMS_ERR */
		goto	EXIT;
	}
	lpChild->id = id;
	lpChild->parent = parent;
	
	/* show dialog */
	W1_ModalDialogShowAction( lpChild->show_dialog, parent );
	
	if ( id != KEEPSETTING_ID_RESET_SETTING ) {
		if( ( ret = ComGtk_GtkMain() ) == CNMS_ERR ){
			DBGMSG( "[ChildDialogShow]Function error by ComGtk_GtkMain() = %d.\n",ret );
			goto	EXIT;
		}
	}
	if ( ChildDataComp[ id ].retOKCancel ) {
		/* use ComGtk_GtkMain return value. */
	}
	else {
		ret = CNMS_NO_ERR;
	}
EXIT:
#ifdef	__CNMS_DEBUG_CHILD_DIALOG__
	DBGMSG( "[ChildDialogShow(id:%d)]=%d.\n", id, ret );
#endif
	return	ret;
}

static CNMSInt32 SubShowDialog1(
		CNMSInt32		id )
{
	CNMSInt32		ret = CNMS_ERR, ldata;
	CNMSDec32		ddata;
	GtkWidget		*widget;

	/* dialog1( with checkBox ) */
	if( ( ldata = KeepSettingGetValue( ChildDataComp[ id ].keepSettingId ) ) == CNMS_ERR ){
		DBGMSG( "[SubShowDialog1]Error is occured in KeepSettingGetValue.\n" );
		goto	EXIT;
	}
	else if( ldata == CNMS_TRUE ){
		ret = CNMS_NO_ERR_2;	/* no dialog */
		goto	EXIT;
	}
	
	if( id == CHILDDIALOG_ID_OVER_100MB ){
		/* get param */
		if( ( ddata = W1Size_GetOutputSize() ) <= 0 ){
			DBGMSG( "[SubShowDialog1]Error is occured in W1Size_GetOutputPixels.\n" );
			goto	EXIT;
		}
		ddata = ddata / 1024 / 1024;	/* byte -> MB */
		if( ddata < 100 ){	/* under 100MB */
			ret = CNMS_NO_ERR_2;	/* no dialog */
			goto	EXIT;
		}
	}
	/* set title string */
	if( ( widget = (GtkWidget *)lookup_widget( lpChild->dialog1, "dialog_child_with_checkBox_label" ) ) == CNMSNULL ){
		DBGMSG( "[SubShowDialog1]Can't look up widget(dialog_child_with_checkBox_label).\n" );
		goto	EXIT;
	}
	gtk_label_set_text( GTK_LABEL( widget ), gettext( ChildDataComp[ id ].lpStr1 ) );
	/* set checkBox string */
	if( ( widget = (GtkWidget *)lookup_widget( lpChild->dialog1, "dialog_child_checkBox" ) ) == CNMSNULL ){
		DBGMSG( "[SubShowDialog1]Can't look up widget(dialog_child_checkBox).\n" );
		goto	EXIT;
	}
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), FALSE );
	gtk_button_set_label( GTK_BUTTON( widget ), gettext( ChildDataComp[ id ].lpStr2 ) );

	/* set focus */
	switch( id ){
		case	CHILDDIALOG_ID_RESET_SETTING:
			/* set focus -> OK */
			W1_WidgetGrabFocus( lpChild->dialog1, "dialog_child_with_checkBox_button_ok" );
			break;
		case	CHILDDIALOG_ID_OVER_100MB:
			/* set focus -> Cancel */
			W1_WidgetGrabFocus( lpChild->dialog1, "dialog_child_with_checkBox_button_cancel" );
			break;
		default:
			break;
	}

	lpChild->show_dialog = lpChild->dialog1;

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_CHILD_DIALOG__
	DBGMSG( "\t[SubShowDialog1(id:%d)]=%d.\n", id, ret );
#endif
	return	ret;
}

static CNMSInt32 SubShowDialog2(
		CNMSInt32		id )
{
	CNMSInt32		ret = CNMS_ERR, ldata;
	CNMSLPSTR		lpStr = CNMSNULL;
	GtkWidget		*widget;

	/* show dialog2( OK only ) */
	/* set title string */
	if( ( widget = (GtkWidget *)lookup_widget( lpChild->dialog2, "dialog_child_with_ok_label" ) ) == CNMSNULL ){
		DBGMSG( "[SubShowDialog2]Can't look up widget(dialog_child_with_ok_label).\n" );
		goto	EXIT;
	}
	switch( id ){
		case	CHILDDIALOG_ID_PIXELS_OVER:
			if( ( lpStr = CnmsGetMem( 512 ) ) == CNMSNULL ){
				DBGMSG( "[SubShowDialog2]Error is occured in CnmsGetMem.\n" );
				goto	EXIT;
			}
			else if( ( ldata = GetPixelsOverString( ChildDataComp[ id ].lpStr1, lpStr ) ) == CNMS_ERR ){
				DBGMSG( "[SubShowDialog2]Error is occured in GetPixelsOverString.\n" );
				goto	EXIT;
			}
			else if( ldata == CNMS_NO_ERR_2 ){
				ret = ldata;
				goto	EXIT;	/* No dialog */
			}
			gtk_label_set_text( GTK_LABEL( widget ), lpStr );
			break;
		
		case	CHILDDIALOG_ID_DATASIZE_OVER:
			if( ( ldata = CheckDataSize() ) == CNMS_ERR ){
				DBGMSG( "[SubShowDialog2]Error is occured in CheckDataSize.\n" );
				goto	EXIT;
			}
			else if( ldata == CNMS_NO_ERR_2 ){
				ret = ldata;
				goto	EXIT;	/* No dialog */
			}
			gtk_label_set_text( GTK_LABEL( widget ), gettext( ChildDataComp[ id ].lpStr1 ) );
			break;

		default:
			gtk_label_set_text( GTK_LABEL( widget ), gettext( ChildDataComp[ id ].lpStr1 ) );
			break;
	}
	lpChild->show_dialog = lpChild->dialog2;
	
	ret = CNMS_NO_ERR;
EXIT:
	if( lpStr != CNMSNULL ){
		CnmsFreeMem( lpStr );
	}
#ifdef	__CNMS_DEBUG_CHILD_DIALOG__
	DBGMSG( "\t[SubShowDialog2(id:%d)]=%d.\n", id, ret );
#endif
	return	ret;
}

static CNMSInt32 SubShowDialog3(
		CNMSInt32		id )
{
	CNMSInt32		ret = CNMS_ERR, ldata;
	CNMSLPSTR		lpStr = CNMSNULL;
	GtkWidget		*widget;

	/* show dialog3( OK/Cancel ) */
	/* set title string */
	if( ( widget = (GtkWidget *)lookup_widget( lpChild->dialog3, "dialog_child_with_okcancel_label" ) ) == CNMSNULL ){
		DBGMSG( "[SubShowDialog3]Can't look up widget(dialog_child_with_okcancel_label).\n" );
		goto	EXIT;
	}
	switch( id ){
		default:
			gtk_label_set_text( GTK_LABEL( widget ), gettext( ChildDataComp[ id ].lpStr1 ) );
			break;
	}
	/* set focus -> OK */
	W1_WidgetGrabFocus( lpChild->dialog3, "dialog_child_with_okcancel_button_ok" );
	
	lpChild->show_dialog = lpChild->dialog3;
	
	ret = CNMS_NO_ERR;
EXIT:
	if( lpStr != CNMSNULL ){
		CnmsFreeMem( lpStr );
	}
#ifdef	__CNMS_DEBUG_CHILD_DIALOG__
	DBGMSG( "\t[SubShowDialog3(id:%d)]=%d.\n", id, ret );
#endif
	return	ret;
}

static CNMSInt32 GetPixelsOverString(
		CNMSLPSTR		lpSrc,
		CNMSLPSTR		lpDst )
{
	GtkWidget		*widget;
	CNMSInt32		ret = CNMS_ERR, i, ldata, outSize[ CNMS_DIM_MAX ];
	CNMSLPSTR		lpStr;

	/* check */
	if( ( lpSrc == CNMSNULL ) || ( lpDst == CNMSNULL ) ){
		DBGMSG( "[GetPixelsOverString]Parameter is error.\n" );
		goto	EXIT;
	}
	/* get param */
	if( ( ldata = W1Size_GetOutputPixels( outSize ) ) != CNMS_NO_ERR ){
		DBGMSG( "[GetPixelsOverString]Error is occured in W1Size_GetOutputPixels.\n" );
		goto	EXIT;
	}

	/* check over pixel */
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( outSize[ i ] > ChildDialogMaxPixels[ i ] ){
			break;
		}
	}
	if( i == CNMS_DIM_MAX ){
		ret = CNMS_NO_ERR_2;	/* No dialog */
		goto	EXIT;
	}

	lpStr = gettext( lpSrc );

	snprintf( lpDst, 512, lpStr, ChildDialogMaxPixelStr[ CNMS_DIM_H ], ChildDialogMaxPixelStr[ CNMS_DIM_V ] );

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_CHILD_DIALOG__
	DBGMSG( "\t[GetPixelsOverString(lpSrc:%s,lpDst:%s)]=%d.\n", lpSrc, lpDst, ret );
#endif
	return	ret;
}

static CNMSInt32 CheckDataSize( CNMSVoid )
{
	CNMSInt32		ret = CNMS_ERR, ldata, bpp;
	CNMSScanInfo	info;

	/* get param */
	if( ( ldata = CnmsGetScanInfoParameter( &info ) ) != CNMS_NO_ERR ){
		DBGMSG( "[CheckDataSize]Error is occured in CnmsGetScanInfoParameter.\n" );
		goto	EXIT;
	}
	if( ( bpp = CnmsGetBytesPerPixel() ) == CNMS_ERR ){
		DBGMSG( "[CheckDataSize]Error is occured in CnmsGetBytesPerPixel.\n" );
		goto	EXIT;
	}

	/* check over pixel */
	if( ( ( bpp * info.InRes.X * info.InSize.Width / info.OutRes ) >= ( 0x80000000UL ) / ( info.InRes.X * info.InSize.Height / info.OutRes ) ) ||
		( ( bpp * info.InRes.Y * info.InSize.Width / info.OutRes ) >= ( 0x80000000UL ) / ( info.InRes.Y * info.InSize.Height / info.OutRes ) ) ){
		ret = CNMS_NO_ERR;
	}
	else{
		ret = CNMS_NO_ERR_2;	/* No dialog */
	}

EXIT:
#ifdef	__CNMS_DEBUG_CHILD_DIALOG__
	DBGMSG( "\t[CheckDataSize()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSVoid ChildDialogHide( CNMSVoid )
{
	CNMSInt32		id, ret = CNMS_ERR;
	if( ( lpChild == CNMSNULL ) || ( lpChild->id == CNMS_ERR ) ){
		DBGMSG( "[ChildDialogHide]Parameter is error.\n" );
		goto	EXIT;
	}
	id = lpChild->id;
	W1_ModalDialogHideAction( lpChild->show_dialog, lpChild->parent );
	lpChild->id = CNMS_ERR;
	lpChild->show_dialog = CNMSNULL;
	lpChild->parent = CNMSNULL;
EXIT:
#ifdef	__CNMS_DEBUG_CHILD_DIALOG__
	DBGMSG( "[ChildDialogHide()].\n" );
#endif

	if ( id != KEEPSETTING_ID_RESET_SETTING ) {
		if( ( ret = ComGtk_GtkMainQuit( lpChild->ret ) ) == CNMS_ERR ){
			DBGMSG( "[ChildDialogHide]Function error by ComGtk_GtkMain() = %d.\n",ret );
		}
	}

	return;
}

CNMSInt32 ChildDialogGetId( CNMSVoid )
{
	CNMSInt32		ret = CNMS_ERR;

	if( ( lpChild == CNMSNULL ) || ( lpChild->id == CNMS_ERR ) ){
		DBGMSG( "[ChildDialogGetId]Parameter is error.\n" );
		goto	EXIT;
	}
	ret = lpChild->id;
EXIT:
#ifdef	__CNMS_DEBUG_CHILD_DIALOG__
	DBGMSG( "[ChildDialogGetId()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSInt32 ChildDialogClickedOk( CNMSVoid )
{
	CNMSInt32		ret = CNMS_ERR, ldata, val;
	GtkWidget		*widget;

	if( ( lpChild == CNMSNULL ) || ( lpChild->id == CNMS_ERR ) ){
		DBGMSG( "[ChildDialogClickedOk]Parameter is error.\n" );
		goto	EXIT;
	}

	if( ChildDataComp[ lpChild->id ].keepSettingId != CNMS_ERR ){
		if( ( widget = (GtkWidget *)lookup_widget( lpChild->dialog1, "dialog_child_checkBox" ) ) == CNMSNULL ){
			DBGMSG( "[ChildDialogClickedOk]Can't look up widget(dialog_child_checkBox).\n" );
			goto	EXIT;
		}
	
		if( ( ldata = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) ) != FALSE ){
			val = CNMS_TRUE;	/* check box is set on */
		}
		else{
			val = CNMS_FALSE;	/* check box is set off */
		}
		if( ( ldata = KeepSettingSetValue( ChildDataComp[ lpChild->id ].keepSettingId, val ) ) != CNMS_NO_ERR ){
			goto	EXIT;
		}
	}
	
	lpChild->ret = COMGTK_GTKMAIN_OK;
	
	ChildDialogHide();

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_CHILD_DIALOG__
	DBGMSG( "[ChildDialogClickedOk()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSInt32 ChildDialogClickedCancel( CNMSVoid )
{
	lpChild->ret = COMGTK_GTKMAIN_CANCEL;

	ChildDialogHide();

#ifdef	__CNMS_DEBUG_CHILD_DIALOG__
	DBGMSG( "[ChildDialogClickedCancel()]=0.\n" );
#endif
	return	CNMS_NO_ERR;
}

#endif	/* _CHILD_DIALOG_C_ */

