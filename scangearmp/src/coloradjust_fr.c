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

#ifndef	_COLORADJUST_FR_C_
#define	_COLORADJUST_FR_C_

#include "coloradjust_fr.h"


/***   FinalReview Dialog Show   ***/
CNMSVoid CnmsFRDialogShow(
						CNMSInt32			color,
						CNMSInt32			keep					)
{
	/* Calcurate ToneCurve for FinalReview					*/
	CnmsColAdjCurveCalcFR();

	/* Channel ComboBox Item Change depending on ColorGroup */
	if( ( CnmsColAdjChannelItemSet( color, CNMS_CA_DIALOG_FR ) ) == CNMS_FALSE )	return;
	switch( color ){
		case CNMS_CA_COLORGROUP_COLOR:
				gtk_label_set_text( GTK_LABEL( fr_label_str_a1 ), gettext(ca_label_red) );
				gtk_label_set_text( GTK_LABEL( fr_label_str_b1 ), gettext(ca_label_red) );
				gtk_label_set_text( GTK_LABEL( fr_label_str_a2 ), gettext(ca_label_green) );
				gtk_label_set_text( GTK_LABEL( fr_label_str_b2 ), gettext(ca_label_green) );
				gtk_label_set_text( GTK_LABEL( fr_label_str_a3 ), gettext(ca_label_blue) );
				gtk_label_set_text( GTK_LABEL( fr_label_str_b3 ), gettext(ca_label_blue) );
				CnmsFRPreviewRGBKValueUnDisp();
				break;
		case CNMS_CA_COLORGROUP_GRAY:
				gtk_label_set_text( GTK_LABEL( fr_label_str_a1 ), gettext(ca_label_gray) );
				gtk_label_set_text( GTK_LABEL( fr_label_str_b1 ), gettext(ca_label_gray) );
				gtk_label_set_text( GTK_LABEL( fr_label_str_a2 ), ca_label_null );
				gtk_label_set_text( GTK_LABEL( fr_label_str_b2 ), ca_label_null );
				gtk_label_set_text( GTK_LABEL( fr_label_str_a3 ), ca_label_null );
				gtk_label_set_text( GTK_LABEL( fr_label_str_b3 ), ca_label_null );
				CnmsFRPreviewRGBKValueUnDisp();
				break;
		default:
				break;
	}

	/* Show/UnShow Change */
	if( keep == CNMS_CA_DIALOG_NON_KEEP ){
		CnmsColAdjDialogShowHide( CNMS_CA_DIALOG_FR, !( GTK_WIDGET_VISIBLE( fr_dlg.widget ) ) );
	}
	CnmsColAdjSetChannelItem( &fr_dlg );

	return;
}


/***   Reset   ***/
CNMSVoid CnmsFRReset(
						CNMSVoid									)
{
	CNMSInt32	i = 0;

	/* Channel Set ComboBox Top				*/
	if( fr_dlg.channel != CNMS_CA_GRAY ){
		fr_dlg.channel = CNMS_CA_MASTER;
	}
	if( GTK_WIDGET_VISIBLE( fr_dlg.widget ) == CNMS_TRUE ){
		CnmsColAdjSetChannelItem( &fr_dlg );
	}

	/* Initialize Histogram	Value			*/
	CnmsColAdjInitCurveAndHistogram( &fr_dlg );

	return;
}


/***   Channel ComboBox Changed   ***/
CNMSVoid CnmsFRChannelComboBoxChanged(
						GtkComboBox			*combobox				)
{
	CNMSInt8	*channel	= CNMSNULL;

	/* If flag is True, Called by Combobox Clear func. The case, No action the process  */
	if( ca_info.clearcbox == CNMS_TRUE ){
		return;
	}

	/* Get Current Channel ID from Combobox String */ 
	if( ( fr_dlg.channel = CnmsColAdjGetChannelIDFromCombobox( combobox ) ) == CNMS_ERR ){
		DBGMSG( "[CnmsFRChannelComboBoxChanged] value[fr_dlg.channel] is invalid.\n" );
		return;
	}

	/* Set Data			*/
	CnmsColAdjToneCurveDraw( CNMS_CA_DIALOG_FR );
		
	return;
}


/***   Disp RGBK Value   ***/
CNMSVoid CnmsFRPreviewRGBKValueDisp(
						CNMSInt32			rValA,
						CNMSInt32			rValB,
						CNMSInt32			gValA,
						CNMSInt32			gValB,
						CNMSInt32			bValA,
						CNMSInt32			bValB					)
{
	CNMSInt8		value_a1[ CNMS_CA_STRING_LEN ],
					value_b1[ CNMS_CA_STRING_LEN ],
					value_a2[ CNMS_CA_STRING_LEN ],
					value_b2[ CNMS_CA_STRING_LEN ],
					value_a3[ CNMS_CA_STRING_LEN ],
					value_b3[ CNMS_CA_STRING_LEN ];

	if( ( rValA < 0 ) || ( rValB < 0 ) || ( gValA < 0 ) || ( gValB < 0 ) || ( bValA < 0 ) || ( bValB < 0 ) ){
		CnmsFRPreviewRGBKValueUnDisp();
		DBGMSG( "[CnmsFRPreviewRGBKValueDisp] Value is invalid. rValA[%d], rValB[%d], gValA[%d], gValB[%d], bValA[%d], bValB[%d]\n",
																											rValA, rValB, gValA, gValB, bValA, bValB );
		return;
	}

	switch( ca_info.colorgroup )
	{
		case CNMS_CA_COLORGROUP_COLOR:
				sprintf(value_a1,"%d\0",rValA);
				gtk_label_set_text( GTK_LABEL( fr_label_value_a1 ), value_a1 );
				sprintf(value_b1,"%d\0",rValB);
				gtk_label_set_text( GTK_LABEL( fr_label_value_b1 ), value_b1 );
				sprintf(value_a2,"%d\0",gValA);
				gtk_label_set_text( GTK_LABEL( fr_label_value_a2 ), value_a2 );
				sprintf(value_b2,"%d\0",gValB);
				gtk_label_set_text( GTK_LABEL( fr_label_value_b2 ), value_b2 );
				sprintf(value_a3,"%d\0",bValA);
				gtk_label_set_text( GTK_LABEL( fr_label_value_a3 ), value_a3 );
				sprintf(value_b3,"%d\0",bValB);
				gtk_label_set_text( GTK_LABEL( fr_label_value_b3 ), value_b3 );
				break;
		case CNMS_CA_COLORGROUP_GRAY:
				sprintf(value_a1,"%d\0",gValA);
				gtk_label_set_text( GTK_LABEL( fr_label_value_a1 ), value_a1 );
				sprintf(value_b1,"%d\0",gValB);
				gtk_label_set_text( GTK_LABEL( fr_label_value_b1 ), value_b1 );
				break;
		default:
			DBGMSG( "[CnmsFRPreviewRGBKValueDisp] Value is invalid. ca_info.colorgroup[%d]\n", ca_info.colorgroup );
				break;
	}
	return;
}


/***   UnDisp RGBK Value   ***/
CNMSVoid CnmsFRPreviewRGBKValueUnDisp(
						CNMSVoid									)
{
	if( ( fr_label_value_a1 == CNMSNULL ) || ( fr_label_value_a2 == CNMSNULL ) || ( fr_label_value_a3 == CNMSNULL ) ){
		DBGMSG( "[CnmsFRPreviewRGBKValueUnDisp] Pointer is NULL. fr_label_value_a1[%p], fr_label_value_a2[%p], fr_label_value_a3[%p]\n",
																	 					fr_label_value_a1, fr_label_value_a2, fr_label_value_a3 );
		return;
	}
	if( ( fr_label_value_b1 == CNMSNULL ) || ( fr_label_value_b2 == CNMSNULL ) || ( fr_label_value_b3 == CNMSNULL ) ){
		DBGMSG( "[CnmsFRPreviewRGBKValueUnDisp] Pointer is NULL. fr_label_value_b1[%p], fr_label_value_b2[%p], fr_label_value_b3[%p]\n",
																	 					fr_label_value_b1, fr_label_value_b2, fr_label_value_b3 );
		return;
	}

	gtk_label_set_text( GTK_LABEL( fr_label_value_a1 ), ca_label_null );
	gtk_label_set_text( GTK_LABEL( fr_label_value_b1 ), ca_label_null );
	gtk_label_set_text( GTK_LABEL( fr_label_value_a2 ), ca_label_null );
	gtk_label_set_text( GTK_LABEL( fr_label_value_b2 ), ca_label_null );
	gtk_label_set_text( GTK_LABEL( fr_label_value_a3 ), ca_label_null );
	gtk_label_set_text( GTK_LABEL( fr_label_value_b3 ), ca_label_null );
	
	return;
}

#endif	/* _COLORADJUST_FR_C_ */
