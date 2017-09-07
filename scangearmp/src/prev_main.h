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

#ifndef	_PREV_MAIN_H_
#define	_PREV_MAIN_H_

#include <gtk/gtk.h>

#include "cnmstype.h"
#include "coloradjust.h"

#define	PREVIEW_RESET_IMAGE			(1)		/* clear preview image only */
#define	PREVIEW_RESET_RECT			(2)		/* clear preview rect only */
#define	PREVIEW_RESET_ALL			( (PREVIEW_RESET_IMAGE) | (PREVIEW_RESET_RECT) )	/* clear preview image and rect */

CNMSInt32 Preview_Open( GtkWidget *mainWindow );
CNMSVoid  Preview_Close( CNMSVoid );
CNMSInt32 Preview_Init( CNMSInt32 source );
CNMSInt32 Preview_Reset( CNMSInt32 type );

CNMSInt32 Preview_ChangeStatus( CNMSInt32 Actionmode );

CNMSInt32 Preview_Configure_Event( CNMSVoid );
CNMSVoid  Preview_Expose_Event( CNMSVoid );

CNMSInt32 Preview_GetMinMaxSize( CNMSInt32 *lpMinSize, CNMSInt32 *lpMaxSize );
CNMSInt32 Preview_GetNewInOutSize( CNMSInt32 *lpNewSize, CNMSInt32 *lpInSize, CNMSInt32 *lpOutSize, CNMSInt32 *lpScale );

CNMSInt32 Preview_RectSet( CNMSInt32 *lpOffset, CNMSInt32 *lpInSize );
CNMSInt32 Preview_RectGet( CNMSInt32 *lpSize );

CNMSInt32 Preview_GetColor( CNMSInt32 *lpCurPosR, CNMSInt32 *lpR, CNMSInt32 *lpG, CNMSInt32 *lpB );

CNMSVoid Preview_Mouse_Button_Press( CNMSBool hgFlag, CNMSBool aspectFlag, CNMSBool prevFlag, CNMSInt32 *lpCurPosR, GdkModifierType state );
CNMSVoid Preview_Motion( CNMSBool hgFlag, CNMSBool prevFlag, CNMSInt32 *lpCurPosR, GdkModifierType state );
CNMSVoid Preview_Mouse_Button_Release( CNMSBool hgFlag, CNMSBool prevFlag, CNMSInt32 *lpCurPosR, GdkModifierType state );

CNMSVoid Preview_AspectSet( CNMSInt32 aspect );

CNMSVoid Preview_ResetMaxSize( CNMSInt32 *lpMax );

#endif	/* _PREV_MAIN_H_ */
