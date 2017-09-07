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

#ifndef	_CHILD_DIALOG_H_
#define	_CHILD_DIALOG_H_

#include "cnmstype.h"

enum{
	CHILDDIALOG_ID_RESET_SETTING = 0,
	CHILDDIALOG_ID_OVER_100MB,
	CHILDDIALOG_ID_PIXELS_OVER,
	CHILDDIALOG_ID_WARNING_MOIRE,
	CHILDDIALOG_ID_NO_PAPER,
	CHILDDIALOG_ID_NOT_FOUND,
	CHILDDIALOG_ID_DATASIZE_OVER,
	CHILDDIALOG_ID_SETTING_APPLY,
	CHILDDIALOG_ID_AUTOPOWEROFF_APPLY,
	CHILDDIALOG_ID_MAX,
};

CNMSInt32 ChildDialogOpen( CNMSVoid );
CNMSVoid  ChildDialogClose( CNMSVoid );

CNMSInt32 ChildDialogShow( CNMSInt32 id, GtkWidget *parent );
CNMSVoid  ChildDialogHide( CNMSVoid );

CNMSInt32 ChildDialogGetId( CNMSVoid );

CNMSInt32 ChildDialogClickedOk( CNMSVoid );
CNMSInt32 ChildDialogClickedCancel( CNMSVoid );

#endif	/* _CHILD_DIALOG_H_ */
