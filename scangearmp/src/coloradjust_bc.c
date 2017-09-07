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

#ifndef	_COLORADJUST_BC_C_
#define	_COLORADJUST_BC_C_

#include "coloradjust_bc.h"

#define		BC_DEF_BRIGHT		(0)		/* Default Value for Brightness	*/
#define		BC_DEF_CONTRAST		(0)		/* Default Value for Contrast	*/

	static	CNMSCAScaleBarInfo	bc_scalebar;

/***   Brightness/Contrast Dialog Show   ***/
CNMSVoid CnmsBCDialogShow(
						CNMSInt32			color,
						CNMSInt32			keep					)
{
	/* Channel ComboBox Item Change depending on ColorGroup */
	if( ( CnmsColAdjChannelItemSet( color, CNMS_CA_DIALOG_BC ) ) == CNMS_FALSE )	return;

	/* Show/UnShow Change */
	if( keep == CNMS_CA_DIALOG_NON_KEEP ){
		CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_BC, !( GTK_WIDGET_VISIBLE( bc_dlg.widget ) ) );
	}
	CnmsColAdjSetChannelItem( &bc_dlg );

	return;
}


/***   Data Initialize   ***/
CNMSVoid CnmsBCInit(
						CNMSVoid									)
{
	CNMSInt32	i = 0;

	/* Initialize Brightness Value	*/
	bc[CNMS_CA_MASTER].bright = bc[CNMS_CA_RED].bright = bc[CNMS_CA_GREEN].bright = bc[CNMS_CA_BLUE].bright = bc[CNMS_CA_GRAY].bright = BC_DEF_BRIGHT;
	ca_param_all.brightness.m = ca_param_all.brightness.r = ca_param_all.brightness.g = ca_param_all.brightness.b = 128;
	
	/* Initialize Contrast Value	*/
	bc[CNMS_CA_MASTER].contrast = bc[CNMS_CA_RED].contrast = bc[CNMS_CA_GREEN].contrast = bc[CNMS_CA_BLUE].contrast = bc[CNMS_CA_GRAY].contrast = BC_DEF_BRIGHT;
	ca_param_all.contrast.m = ca_param_all.contrast.r = ca_param_all.contrast.g = ca_param_all.contrast.b = 128;

	/* Initialize ToneCueve Value	*/
	CnmsColAdjInitCurveAndHistogram( &bc_dlg );

	/* Initialize ScaleBar Info		*/
	bc_scalebar.x = bc_scalebar.value = bc_scalebar.range = bc_scalebar.flag = 0;

	return;
}


/***   Reset   ***/
CNMSVoid CnmsBCReset(
						CNMSVoid									)
{
	/* Reset Displayed Brightness/Contrast	*/
	gtk_spin_button_set_value( GTK_SPIN_BUTTON( bc_spinbutton_bright), BC_DEF_BRIGHT );
	gtk_range_set_value( GTK_RANGE( bc_scale_bright ), BC_DEF_BRIGHT );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON( bc_spinbutton_contrast), BC_DEF_CONTRAST );
	gtk_range_set_value( GTK_RANGE( bc_scale_contrast ), BC_DEF_CONTRAST );

	/* Data Initialize						*/
	CnmsBCInit();

	/* Channel Set ComboBox Top				*/
	if( fr_dlg.channel != CNMS_CA_GRAY ){
		bc_dlg.channel = CNMS_CA_MASTER;
	}
	if( GTK_WIDGET_VISIBLE( bc_dlg.widget ) == CNMS_TRUE ){
		CnmsColAdjSetChannelItem( &bc_dlg );
		CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_BC );
	}

	return;
}


/***   Channel ComboBox Changed   ***/
CNMSVoid CnmsBCChannelComboBoxChanged(
						GtkComboBox			*combobox				)
{
	CNMSInt8	*channel	= CNMSNULL;

	/* If flag is True, Called by Combobox Clear func. The case, No action the process  */
	if( ca_info.clearcbox == CNMS_TRUE ){
		return;
	}

	/* Get Current Channel ID from Combobox String */ 
	if( ( bc_dlg.channel = CnmsColAdjGetChannelIDFromCombobox( combobox ) ) == CNMS_ERR ){
		DBGMSG( "[CnmsBCChannelComboBoxChanged] value[bc_dlg.channel] is invalid.\n" );
		return;
	}

	/* Set Data			*/
	CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_BC );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON( bc_spinbutton_bright), bc[bc_dlg.channel].bright );
	gtk_range_set_value( GTK_RANGE( bc_scale_bright ), bc[bc_dlg.channel].bright );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON( bc_spinbutton_contrast), bc[bc_dlg.channel].contrast );
	gtk_range_set_value( GTK_RANGE( bc_scale_contrast ), bc[bc_dlg.channel].contrast );

	return;
}

static	CNMSInt32	pressx = -1;
static	CNMSInt32	pressVal = 0;
static	CNMSDec32	pressrange = 0;
static	CNMSInt32	pressflag = FALSE;

/***   Brightness Value Changed with ScaleBar   ***/
CNMSInt32 CnmsBCScaleBarValueChanged(
						CNMSInt32			type,
						GtkRange			*range					)
{
	CNMSInt32	AdjustValue		= 128,
				ret				= CNMS_ERR;
	CNMSDec32	*value			= CNMSNULL;
	GtkWidget	*spinbutton		= CNMSNULL,
				*scale			= CNMSNULL;
	CNMSRGB08	*param			= CNMSNULL;

	if( range == CNMSNULL ){
		DBGMSG( "[CnmsBCScaleBrightValueChanged] Pointer is NULL. range[%p]\n", range );
		goto EXIT;
	}

	switch( type ){
		case CNMS_CA_ID_BRIGHT:		value = &( bc[bc_dlg.channel].bright );
									spinbutton = bc_spinbutton_bright;
									scale = bc_scale_bright;
									param = &( ca_param_all.brightness );
									break;

		case CNMS_CA_ID_CONTRAST:	value = &( bc[bc_dlg.channel].contrast );
									spinbutton = bc_spinbutton_contrast;
									scale = bc_scale_contrast;
									param = &( ca_param_all.contrast );
									break;

		default:					DBGMSG( "[CnmsBCScaleBarValueChanged] type[%d] is invalid.\n",type );
									goto EXIT;
	}

	/* Get ScaleBar Value & Set Value to SpinButton	*/
	if( bc_scalebar.flag == TRUE ){
		gtk_spin_button_set_value( GTK_SPIN_BUTTON( spinbutton ), bc_scalebar.value );
		gtk_range_set_value( GTK_RANGE( scale ), bc_scalebar.range );
		bc_scalebar.flag = FALSE;
	}
	else{
		*value = gtk_range_get_value( range );
		gtk_spin_button_set_value( GTK_SPIN_BUTTON( spinbutton ), *value );
	}
	CnmsColAdjSetZeroIfMinusZeroForSpinbutton( spinbutton );

	/* Keep Parameter depending on Current Channel	*/
	AdjustValue = ( CNMSInt32 )( ( ( CNMSDec32 )( (*value) + 127 ) * 255 / 254 ) + 0.5 );	/* -127 ~ +127 to 0 ~ +255			*/
	switch( bc_dlg.channel ){
		case	CNMS_CA_MASTER:		param->m = AdjustValue;		break;
		case	CNMS_CA_RED:		param->r = AdjustValue;		break;
		case	CNMS_CA_GREEN:		param->g = AdjustValue;		break;
		case	CNMS_CA_BLUE:		param->b = AdjustValue;		break;
		case	CNMS_CA_GRAY:		param->m = AdjustValue;		break;					/* Case Gray, Keep to Master Area	*/
		default:					DBGMSG( "[CnmsBCScaleBarValueChanged] bc_dlg.channel[%d] is invalid.\n",bc_dlg.channel );
									goto EXIT;
	}

	/* Get ToneCurve Value & Draw ToneCurve			*/
	CnmsColAdjCurveCalcBC( &ca_param_all );
	CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_BC );

	/* Redraw Preview Data by Parameter Changed */
	CnmsColAdjPreviewImageSet();

	ret = CNMS_NO_ERR;
EXIT:
	return ret;
}


/***   Brightness Value Changed with SpinButton   ***/
CNMSVoid CnmsBCSpinButtonValueChanged(
						CNMSInt32			type,
						GtkSpinButton		*spinbutton				)
{
	CNMSDec32	*value			= CNMSNULL;
	GtkWidget	*scale			= CNMSNULL;

	if( spinbutton == CNMSNULL ){
		DBGMSG( "[CnmsBCSpinButtonBrightValueChanged] Arg pointer [spinbutton] is NULL.\n" );
		return;
	}

	switch( type ){
		case CNMS_CA_ID_BRIGHT:		value = &( bc[bc_dlg.channel].bright );		scale = bc_scale_bright;			break;
		case CNMS_CA_ID_CONTRAST:	value = &( bc[bc_dlg.channel].contrast );	scale = bc_scale_contrast;			break;
		default:					DBGMSG( "[CnmsBCSpinButtonValueChanged] type[%d] is invalid.\n",type );			return;
	}

	/* Get SpinButton Value & Set Value to ScaleBar	*/
	*value = gtk_spin_button_get_value( spinbutton );
	gtk_range_set_value( GTK_RANGE( scale ), (*value) );
	
	return;
}

CNMSVoid CnmsBCScaleBarPress(
						CNMSInt32			type,
						GdkEventButton		*event					)
{
	GtkWidget	*spinbutton		= CNMSNULL,
				*scale			= CNMSNULL;

	switch( type ){
		case CNMS_CA_ID_BRIGHT:		spinbutton = bc_spinbutton_bright;		scale = bc_scale_bright;			break;
		case CNMS_CA_ID_CONTRAST:	spinbutton = bc_spinbutton_contrast;	scale = bc_scale_contrast;			break;
		default:					DBGMSG( "[CnmsBCSpinButtonValueChanged] type[%d] is invalid.\n",type );		return;
	}

	bc_scalebar.x = event->x;
	bc_scalebar.value = (CNMSInt32)atoi( gtk_entry_get_text( GTK_ENTRY( spinbutton ) ) );
	bc_scalebar.range = gtk_range_get_value( GTK_RANGE( scale ) );
	return;
}


CNMSVoid CnmsBCScaleBarRelease(
						GdkEventButton		*event					)
{
	if( event->x == bc_scalebar.x ){
		bc_scalebar.flag = TRUE;
	}
	return;
}

#endif	/* _COLORADJUST_BC_C_ */
