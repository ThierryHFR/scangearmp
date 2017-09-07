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

#ifndef	_COLORADJUST_TC_C_
#define	_COLORADJUST_TC_C_

#include "cnmsfunc.h"
#include "coloradjust_tc.h"


/***   ToneCurve Adjustment Dialog Show   ***/
CNMSVoid CnmsTCDialogShow(
						CNMSInt32			color,
						CNMSInt32			keep					)
{
	/* Channel ComboBox Item Change depending on ColorGroup */
	if( ( CnmsColAdjChannelItemSet( color, CNMS_CA_DIALOG_TC ) ) == CNMS_FALSE )	return;

	/* Show/UnShow Change */
	if( keep == CNMS_CA_DIALOG_NON_KEEP ){
		CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_TC, !( GTK_WIDGET_VISIBLE( tc_dlg.widget ) ) );
	}
	CnmsColAdjSetChannelItem( &tc_dlg );
	gtk_combo_box_set_active( GTK_COMBO_BOX( tc_combobox_tone_select ), tc[tc_dlg.channel].tc_num );
	
	return;
}


/***   Data Initialize   ***/
CNMSVoid CnmsTCInit(
						CNMSVoid									)
{
	CNMSInt32	i = 0;

	/* Initialize ToneCurve Mode	*/
	tc[CNMS_CA_MASTER].tc_num = tc[CNMS_CA_RED].tc_num = tc[CNMS_CA_GREEN].tc_num = tc[CNMS_CA_BLUE].tc_num = tc[CNMS_CA_GRAY].tc_num = CNMS_TC_NO_ADJUST;

	/* Initialize ToneCueve Value	*/
	CnmsColAdjInitCurveAndHistogram( &tc_dlg );

	return;
}


/***   Reset   ***/
CNMSVoid CnmsTCReset(
						CNMSVoid									)
{
	CNMSInt32		i						= 0;
	CNMSUInt8		tc_curve_out[CNMS_CA_VECTOR_LEN];
	CNMSUInt16		tc_curve_out16[CNMS_CA_VECTOR_LONGLEN];

	/* Data Initialize						*/
	CnmsTCInit();

	lpCnmsImgApi->p_CnmsImgSplineCalc( tc_curve_out, CNMS_TC_NO_ADJUST );
	lpCnmsImgApi->p_CnmsImgSplineCalc16( tc_curve_out16, CNMS_TC_NO_ADJUST );

	for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ){
		ca_usertone[i].m = ca_usertone[i].r = ca_usertone[i].g = ca_usertone[i].b = tc_curve_out[i];
	}
	for( i = 0 ; i < CNMS_CA_VECTOR_LONGLEN ; i++ ){
		ca_usertone16[i].m = ca_usertone16[i].r = ca_usertone16[i].g = ca_usertone16[i].b = tc_curve_out16[i];
	}

	/* Channel Set ComboBox Top				*/
	if( tc_dlg.channel != CNMS_CA_GRAY ){
		tc_dlg.channel = CNMS_CA_MASTER;
	}
	if( GTK_WIDGET_VISIBLE( tc_dlg.widget ) != 0 ){
		CnmsColAdjSetChannelItem( &tc_dlg );
		gtk_combo_box_set_active( GTK_COMBO_BOX( tc_combobox_tone_select ), CNMS_TC_NO_ADJUST );
		CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_TC );
	}

	return;
}


/***   Channel ComboBox Changed   ***/
CNMSVoid CnmsTCChannelComboBoxChanged(
						GtkComboBox			*combobox				)
{
	CNMSInt8	*channel	= CNMSNULL;

	/* If flag is True, Called by Combobox Clear func. The case, No action the process  */
	if( ca_info.clearcbox == CNMS_TRUE ){
		return;
	}

	/* Get Current Channel ID from Combobox String */ 
	if( ( tc_dlg.channel = CnmsColAdjGetChannelIDFromCombobox( combobox ) ) == CNMS_ERR ){
		DBGMSG( "[CnmsTCChannelComboBoxChanged] value[tc_dlg.channel] is invalid.\n" );
		return;
	}

	/* Set Data			*/
	CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_TC );
	gtk_combo_box_set_active( GTK_COMBO_BOX( tc_combobox_tone_select ), tc[tc_dlg.channel].tc_num );

	return;
}


/***   ToneCurve Adjustment ConboBox Changed   ***/
CNMSVoid CnmsTCToneCurveSelectComboBoxChanged(
						GtkComboBox			*combobox				)
{
	CNMSInt8		*tone					= CNMSNULL;
	CNMSUInt8		tc_curve_out[CNMS_CA_VECTOR_LEN];
	CNMSUInt16		tc_curve_out16[CNMS_CA_VECTOR_LONGLEN];
	CNMSInt32		i						= 0;

	if( combobox == CNMSNULL ){
		DBGMSG( "[CnmsTCToneCurveSelectComboBoxChanged] Arg Pointer is NULL. combobox[%p]\n",combobox );
		return;
	}		

	/* Get Selected ToneCurve String	*/
	if( ( tone = gtk_combo_box_get_active_text( combobox ) ) == CNMSNULL ){
		DBGMSG( "[CnmsTCToneCurveSelectComboBoxChanged] Pointer is NULL. tone[%p]\n",tone );
		return;
	}		
		
	/* Set Selected ToneCurve Parameter	*/
	CnmsSetMem( (CNMSLPSTR)tc_curve_out, 0, sizeof( tc_curve_out ) );
	CnmsSetMem( (CNMSLPSTR)tc_curve_out16, 0, sizeof( tc_curve_out16 ) );

	if		( CnmsStrCompare( tone, gettext( tc_litem_no_adjust ) )	== CNMS_NO_ERR ){		/* No Adjustment	*/
		tc[tc_dlg.channel].tc_num = CNMS_TC_NO_ADJUST;
		lpCnmsImgApi->p_CnmsImgSplineCalc( tc_curve_out, tc[tc_dlg.channel].tc_num );
		lpCnmsImgApi->p_CnmsImgSplineCalc16( tc_curve_out16, tc[tc_dlg.channel].tc_num );
	}
	else if	( CnmsStrCompare( tone, gettext( tc_litem_exp_over ) )	== CNMS_NO_ERR ){		/* Exposure over	*/
		tc[tc_dlg.channel].tc_num = CNMS_TC_EXP_OVER;
		lpCnmsImgApi->p_CnmsImgSplineCalc( tc_curve_out, tc[tc_dlg.channel].tc_num );
		lpCnmsImgApi->p_CnmsImgSplineCalc16( tc_curve_out16, tc[tc_dlg.channel].tc_num );
	}
	else if	( CnmsStrCompare( tone, gettext( tc_litem_exp_under ) )	== CNMS_NO_ERR ){		/* Exposure under	*/
		tc[tc_dlg.channel].tc_num = CNMS_TC_EXP_UNDER;
		lpCnmsImgApi->p_CnmsImgSplineCalc( tc_curve_out, tc[tc_dlg.channel].tc_num );
		lpCnmsImgApi->p_CnmsImgSplineCalc16( tc_curve_out16, tc[tc_dlg.channel].tc_num );
	}
	else if	( CnmsStrCompare( tone, gettext( tc_litem_high_cont ) )	== CNMS_NO_ERR ){		/* High contrast	*/
		tc[tc_dlg.channel].tc_num = CNMS_TC_HIGH_CONT;
		lpCnmsImgApi->p_CnmsImgSplineCalc( tc_curve_out, tc[tc_dlg.channel].tc_num );
		lpCnmsImgApi->p_CnmsImgSplineCalc16( tc_curve_out16, tc[tc_dlg.channel].tc_num );
	}
	else{
		DBGMSG( "[CnmsTCToneCurveSelectComboBoxChanged] String is invalid. tone[%s]\n",tone );
		return;
	}		

	/* Keep Parameter depending on Current Channel	*/
	if( tc_dlg.channel == CNMS_CA_MASTER ){
		for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ) {
			tc_dlg.curve[CNMS_CA_MASTER][i] = ca_usertone[i].m = ( CNMSUInt32 )tc_curve_out[i];
		}
		for( i = 0 ; i < CNMS_CA_VECTOR_LONGLEN ; i++ ) {
			ca_usertone16[i].m = ( CNMSUInt32 )tc_curve_out16[i];
		}
	}
	else if( tc_dlg.channel == CNMS_CA_RED ){
		for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ) {
			tc_dlg.curve[CNMS_CA_RED][i] = ca_usertone[i].r = ( CNMSUInt32 )tc_curve_out[i];
		}
		for( i = 0 ; i < CNMS_CA_VECTOR_LONGLEN ; i++ ) {
			ca_usertone16[i].r = ( CNMSUInt32 )tc_curve_out16[i];
		}
	}
	else if( tc_dlg.channel == CNMS_CA_GREEN ){
		for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ) {
			tc_dlg.curve[CNMS_CA_GREEN][i] = ca_usertone[i].g = ( CNMSUInt32 )tc_curve_out[i];
		}
		for( i = 0 ; i < CNMS_CA_VECTOR_LONGLEN ; i++ ) {
			ca_usertone16[i].g = ( CNMSUInt32 )tc_curve_out16[i];
		}
	}
	else if( tc_dlg.channel == CNMS_CA_BLUE ){
		for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ) {
			tc_dlg.curve[CNMS_CA_BLUE][i] = ca_usertone[i].b = ( CNMSUInt32 )tc_curve_out[i];
		}
		for( i = 0 ; i < CNMS_CA_VECTOR_LONGLEN ; i++ ) {
			ca_usertone16[i].b = ( CNMSUInt32 )tc_curve_out16[i];
		}
	}
	else if( tc_dlg.channel == CNMS_CA_GRAY ){							/* Case Gray, Keep as Red instead of Gray	*/
		for( i = 0 ; i < CNMS_CA_VECTOR_LEN ; i++ ) {
			tc_dlg.curve[CNMS_CA_GRAY][i] = ca_usertone[i].r = ( CNMSUInt32 )tc_curve_out[i];
		}
		for( i = 0 ; i < CNMS_CA_VECTOR_LONGLEN ; i++ ) {
			ca_usertone16[i].m = ( CNMSUInt32 )tc_curve_out16[i];
		}
	}
	
	/* Get ToneCurve Value & Draw ToneCurve			*/
	CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_TC );

	/* Redraw Preview Data by Parameter Changed */
	CnmsColAdjPreviewImageSet();

	return;
}

#endif	/* _COLORADJUST_TC_C_ */
