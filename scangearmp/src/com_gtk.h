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

#ifndef	_COM_GTK_H_
#define	_COM_GTK_H_

#include <gtk/gtk.h>

#include "cnmstype.h"

enum{
	COMGTK_GTKMAIN_NORMAL = 0,
	COMGTK_GTKMAIN_OK,
	COMGTK_GTKMAIN_CANCEL,
};

CNMSInt32 ComGtk_CursorImageSet( GtkWidget *widget, CNMSInt32 imageId );
CNMSVoid ComGtk_ClearCombo( GtkComboBox *combobox );
CNMSVoid ComGtk_WidgetHideAndShow( GtkWidget *widget );
CNMSInt32 ComGtk_GtkMain( CNMSVoid );
CNMSInt32 ComGtk_GtkMainQuit( CNMSInt32 value );
CNMSVoid ComGtk_EditableClearSelection( GtkWidget *widget, GtkStateType state );

#endif	/* _COM_GTK_H_ */
