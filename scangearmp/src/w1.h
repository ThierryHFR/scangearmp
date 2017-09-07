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

#ifndef	_W1_H_
#define	_W1_H_

#include <gtk/gtk.h>

#include "cnmstype.h"
#include "cnmsdef.h"
#include "cnmsui.h"
#include "scanmain.h"

#ifdef	_GLOBALS_
	#define	GLOBAL
#else
	#define	GLOBAL	extern
#endif


#define	STR_GIMP_BLURB		"scanning front-end GUI for Canon Multifunction Inkjet Printer."
#define	STR_GIMP_HELP		"You can scan image from Canon Multifunction Inkjet Printer, and select the scanning parameters, for example, input/output size, resolution, etc."
#define	STR_GIMP_MENU_LABEL	"<Toolbox>/File/Acquire/ScanGear MP..."

enum{
	NOTEBOOK_PAGE_SIMPLE = 0,
	NOTEBOOK_PAGE_ADVANCED,
};

enum{
	W1KEY_NONE = 0,
	W1KEY_RESET,
	W1KEY_CHANGE_OUTSIZE,
	W1KEY_CHANGE_UNIT,
};

enum{
	CLEARPREVIEW_TYPE_NORMAL = 0,
	CLEARPREVIEW_TYPE_CHANGE_SOURCE,
};

typedef struct{
	/* for show menu */
	CNMSInt32			linkNum;
	LPCNMSUILINKCOMP	lpLink;
	/* for scan */
	CNMSBool			previewFlag;	/* 0:OFF, 1:ON */
	CNMSInt32			group;
	CNMSInt32			env_unit;
	CNMSInt32			base_res;
	CNMSInt32			size_res;
	CNMSInt32			crop_res;
	CNMSInt32			scale;
	CNMSInt32			minSize[ CNMS_DIM_MAX ];	/* unit:pixel res:base_res */
	CNMSInt32			maxSize[ CNMS_DIM_MAX ];	/* unit:pixel res:base_res */
	CNMSInt32			offset[ CNMS_DIM_MAX ];		/* unit:pixel */
	CNMSInt32			inSize[ CNMS_DIM_MAX ];		/* unit:pixel */
	CNMSInt32			outSize[ CNMS_DIM_MAX ];	/* unit:pixel */
	/* format type */
	CNMSInt32			formatType;
	CNMSInt32			thresholdDef;
}W1COMP, *LPW1COMP;

GLOBAL GtkWidget	*main_window
#ifdef	_GLOBALS_
 = CNMSNULL
#endif
;

GLOBAL GtkWidget	*select_model_dialog
#ifdef	_GLOBALS_
 = CNMSNULL
#endif
;

/* for calibration */
GLOBAL GtkWidget	*calibration_dialog
#ifdef	_GLOBALS_
 = CNMSNULL
#endif
;

/* for error dialog */
GLOBAL GtkWidget *error_dialog
#ifdef	_GLOBALS_
 = CNMSNULL
#endif
;

/* for error dialog */
GLOBAL GtkWidget *about_dialog
#ifdef	_GLOBALS_
 = CNMSNULL
#endif
;

/* for clear preview dialog */
GLOBAL GtkWidget *clear_preview_dialog
#ifdef	_GLOBALS_
 = CNMSNULL
#endif
;

/* for main window set_sensitive */
GLOBAL CNMSBool main_window_sensitive
#ifdef	_GLOBALS_
 = CNMS_TRUE
#endif
;

GLOBAL LPW1COMP	lpW1Comp
#ifdef	_GLOBALS_
 = CNMSNULL
#endif
;

CNMSInt32 W1_Open( CNMSInt32 devIndex );
CNMSVoid W1_Close( CNMSVoid );

CNMSInt32 W1_Init( CNMSInt32 source );
CNMSInt32 W1_Reset( CNMSInt32 prevKey );
CNMSInt32 W1_SwitchPlatenAndADFReset( CNMSInt32 prevKey );

CNMSInt32 W1_ChangeToggle( GtkToggleButton *togglebutton, CNMSInt32 objectID );
CNMSInt32 W1_ChangeSpin( GtkSpinButton *spin_button, CNMSInt32 objectID );
CNMSInt32 W1_ChangeCombo( GtkComboBox *combobox, CNMSInt32 objectID );
CNMSInt32 W1_ChangeComboSub( CNMSInt32 objectID, CNMSInt32 value, CNMSInt32 clipKey );

CNMSInt32 W1_ResetSize( CNMSVoid );

CNMSInt32 W1_Preview( CNMSVoid );
CNMSInt32 W1_PreviewCancel( CNMSVoid );
CNMSInt32 W1_PreviewClear( CNMSInt32 id );

CNMSVoid W1_HideAndShowAllButton( CNMSVoid );
CNMSVoid W1_ModalDialogShowAction( GtkWidget *widget, GtkWidget *parent );
CNMSVoid W1_ModalDialogHideAction( GtkWidget *widget, GtkWidget *parent );

CNMSVoid W1_MainWindowSetSensitiveFalse( CNMSVoid );
CNMSVoid W1_MainWindowSetSensitiveTrue( CNMSVoid );

CNMSVoid W1_WidgetGrabFocus( GtkWidget *parent, const gchar *widget_name );

void dialog_clear_preview_ok_clicked(void);

CNMSInt32 W1_JudgeFormatType( CNMSInt32 object );
CNMSInt32 W1_GetSelectSourceADF( CNMSVoid );

CNMSInt32 W1_ShowSelectModelForNetwork( CNMSInt32 devnum );
CNMSInt32 W1_ReloadSelectModel( CNMSVoid );

CNMSVoid  W1_ChangeSpinAdjustMinusZero( GtkSpinButton *spinbutton );


#ifdef	__CNMS_DEBUG__
CNMSVoid DebugW1Comp( CNMSVoid );
#endif

/* defined in w1_size.c */
CNMSInt32 W1Size_GetOutputPixels( CNMSInt32 *lpSize );
CNMSDec32 W1Size_GetOutputSize( CNMSVoid );
CNMSInt32 W1Size_GetEstimatedSizeStr( CNMSLPSTR lpStr, CNMSInt32 bufSize );
CNMSInt32 W1Size_ChangeSpin( CNMSInt32 inVal, CNMSInt32 objectID );
CNMSInt32 W1Size_ChangeRectAreaAfter( CNMSInt32 *lpSize, CNMSInt32 *lpOffset );

/* defined in w1_ui.c */
CNMSInt32 W1Ui_HideNoUseBox( CNMSVoid );
CNMSInt32 W1Ui_Init( CNMSVoid );
CNMSInt32 W1Ui_ShowSize( CNMSVoid );

CNMSInt32 W1Ui_AspectInfoSet( CNMSInt32 aspectFlag );
CNMSInt32 W1Ui_AspectReflect( CNMSVoid );

CNMSInt32 W1Ui_ChangeClearPrevButtonVisible( CNMSVoid );

CNMSInt32 W1Ui_SetComboDefVal( GtkWidget *combobox, CNMSInt32 obj );
CNMSInt32 W1Ui_SetComboSelectedVal( GtkWidget *combobox, CNMSInt32 obj, CNMSLPSTR str );

CNMSInt32 W1Ui_InitUIFormat( CNMSVoid );
CNMSInt32 W1Ui_SwitchPlatenAndADF( CNMSInt32 object, CNMSInt32 currentGp, CNMSInt32 changedGp );


#endif	/* _W1_H_ */
