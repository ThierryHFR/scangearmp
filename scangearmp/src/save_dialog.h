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

#ifndef	_SAVE_DIALOG_H_
#define	_SAVE_DIALOG_H_

#include "cnmstype.h"

enum{
	CNMS_SAVE_FORMAT_PNG = 0,
	CNMS_SAVE_FORMAT_PDF,
	CNMS_SAVE_FORMAT_PNM,

	CNMS_SAVE_FORMAT_MAX,
};

CNMSInt32 SaveDialog_Open( CNMSVoid  );
CNMSVoid  SaveDialog_Close( CNMSVoid );

CNMSInt32 SaveDialog_Show( CNMSVoid );
CNMSVoid SaveDialog_Hide( CNMSVoid );

CNMSInt32 SaveDialog_Exec( CNMSVoid );
CNMSInt32 SaveDialog_SaveFileMain( CNMSVoid );

CNMSLPSTR SaveDialog_GetPath( CNMSVoid );
CNMSInt32 SaveDialog_GetFileFormat( CNMSVoid );

CNMSVoid SaveDialog_HideAndShowButton( CNMSVoid );

void dialog_ok_del_clicked( void );
CNMSInt32 SaveDialog_GetSensitive( CNMSVoid );

#endif	/* _SAVE_DIALOG_H_ */
