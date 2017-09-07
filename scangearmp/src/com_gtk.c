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

#ifndef	_COM_GTK_C_
#define	_COM_GTK_C_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "cnmstype.h"
#include "com_gtk.h"

static	CNMSInt32	GtkMainQuitValue = COMGTK_GTKMAIN_NORMAL;

CNMSInt32 ComGtk_CursorImageSet(
		GtkWidget		*widget,
		CNMSInt32		imageId )
{
	CNMSInt32		ret = CNMS_ERR;
	GdkCursor 		*cursor;

	if( widget == NULL ){
		goto	EXIT;
	}
	else if( ( cursor = gdk_cursor_new( (GdkCursorType)imageId ) ) == CNMSNULL ){
		goto	EXIT;
	}
	gdk_window_set_cursor( widget->window, cursor );

	gdk_cursor_unref( cursor );

	ret = CNMS_NO_ERR;
EXIT:
	return	ret;
}

CNMSVoid ComGtk_ClearCombo( GtkComboBox *combobox )
{
	if( combobox == CNMSNULL ){
		return;
	}
	
	while( 1 ){
		gtk_combo_box_set_active( combobox, 0 );
		if( gtk_combo_box_get_active_text( combobox ) == CNMSNULL ){
			break;
		}
		gtk_combo_box_remove_text( combobox, 0 );
	}
	return;
}

CNMSVoid ComGtk_WidgetHideAndShow(
		GtkWidget		*widget )
{
	if( widget == CNMSNULL ){
		return;
	}

	if( GTK_WIDGET_VISIBLE( widget ) == TRUE ){
		gtk_widget_hide( widget );
		gtk_widget_show( widget );
	}
	return;
}

static CNMSBool	GtkMainLocked	= CNMS_FALSE;

CNMSInt32 ComGtk_GtkMain(
		CNMSVoid				)
{
	CNMSInt32	ret = CNMS_ERR;
	
	/* Unlocked */
	if( GtkMainLocked == CNMS_FALSE ){
		GtkMainLocked = CNMS_TRUE;
		gtk_main();
		ret = GtkMainQuitValue;
	}
	
	return ret;
}

CNMSInt32 ComGtk_GtkMainQuit(
		CNMSInt32		value	)
{
	CNMSInt32	ret = CNMS_ERR;
	
	/* Locked */
	if( GtkMainLocked == CNMS_TRUE ){
		GtkMainQuitValue = value;
		GtkMainLocked = CNMS_FALSE;
		gtk_main_quit();
		ret = CNMS_NO_ERR;
	}
	
	return ret;
}

CNMSVoid ComGtk_EditableClearSelection(
		GtkWidget		*widget,
		GtkStateType	state		)
{
	if( widget == CNMSNULL ){
		return;
	}
	
	if( state != GTK_STATE_INSENSITIVE ) {
		gtk_editable_select_region( GTK_EDITABLE(widget), 0, 0 );
	}
}


#endif	/* _COM_GTK_C_ */
