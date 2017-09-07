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

#ifndef	_PREFERENCE_C_
#define	_PREFERENCE_C_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"

#include "cnmstype.h"
#include "cnmsdef.h"
#include "cnmsfunc.h"
#include "com_gtk.h"
#include "prev_main.h"
#include "w1.h"
#include "preference.h"
#include "child_dialog.h"
#include "cnmsstr.h"
#include "scanflow.h"

#include "scanmsg.h"
#include "errors.h"

enum{
	PREF_COLOR_24 = 0,
	PREF_COLOR_8,
	PREF_COLOR_1,
	PREF_COLOR_NUM,
};

enum{
	PREF_OBJ_COLORSET = 0,
	PREF_OBJ_MONGAMMA,
	PREF_OBJ_QUIETMODE,
	PREF_OBJ_EVERYCALIB,
	PREF_OBJ_MAX,
};

enum{
	PREF_GET_AP_OFF_UI = 0,
	PREF_GET_AP_ON_UI,
	PREF_GET_AP_OFF_DEV,
	PREF_GET_AP_ON_DEV,
	PREF_GET_AP_MAX,
};

/* #define	__CNMS_DEBUG_PREFERENCE__ */

#define	PREF_WIDGET_NUM				(2)
#define	PREF_GAMMA_RESET_VALUE		(2.20)

typedef struct{
	CNMSInt32		obj;
	CNMSInt32		value;
	CNMSLPSTR		str;
}PREFWIDGETCOMP, *LPPREFWIDGETCOMP;

typedef struct{
	GtkWidget		*widget;		/* widget					*/
	CNMSInt32		valInt;			/* Value of Integer type	*/
	CNMSDec32		valDouble;		/* Value of Double type		*/
}PREFOBJCOMP, *LPPREFOBJCOMP;

typedef struct{
	GtkWidget		*widget;		/* widget	*/
	CNMSInt32		type;			/* type		*/
}PREFRADIO, *LPPREFRADIO;

typedef struct{
	/* preference */
	GtkWidget		*preference_window;
	/* quiet settings */
	GtkWidget		*preference_dialog_quiet_settings;
	GtkWidget		*preference_quiet_hbox;
	GtkWidget		*preference_quiet_settings_label;
	PREFRADIO		radio[PREF_QUIETSETTINGS_RADIO_MAX];
	GtkWidget		*timer_hm[PREF_QUIETSETTINGS_TIMER_MAX];
	GtkWidget		*timerTable;
	/* auto power settings */
	CNMSInt32		autopower_type;
	GtkWidget		*preference_scanner_autopower_vbox;
	GtkWidget		*preference_dialog_autopower_settings;
	GtkWidget		*dialog_autopower_settings_on_label;
	GtkWidget		*dialog_autopower_settings_on_combobox;
	GtkWidget		*dialog_autopower_settings_off_label;
	GtkWidget		*dialog_autopower_settings_off_combobox;
	/* others */
	GtkWidget		*preference_dialog_get_settings;
	GtkWidget		*preference_dialog_get_settings_label;
	CNMSInt32		current_color;
	PREFOBJCOMP		obj[PREF_OBJ_MAX];
	CNMSDec32		monitor_gamma;
	CNMSInt32		widgetMode[ PREF_COLOR_NUM ][ PREF_WIDGET_NUM ];
	CNMSInt32		network_support;
}PREFERENCECOMP, *LPPREFERENCECOMP;

typedef struct {
	CNMSInt32		valUi;
	unsigned char	valDevsets;
}AUTOPOWER_TABLE, *LPAUTOPOWER_TABLE;

static PREFWIDGETCOMP	PrefWidgetComp[ PREF_WIDGET_NUM ] = {
	{ CNMS_OBJ_A_COLOR_CONFIG,	CNMS_A_COLOR_CONFIG_RECOM,	"preference_color_recommended_radio" },
/*	{ CNMS_OBJ_A_COLOR_CONFIG,	CNMS_A_COLOR_CONFIG_MATCH,	"preference_color_matching_radio"    }, */
	{ CNMS_OBJ_A_COLOR_CONFIG,	CNMS_A_COLOR_CONFIG_NON,	"preference_color_none_radio"        },
};

static LPPREFERENCECOMP lpPrefComp = CNMSNULL;

static CNMSInt32 ColorConv(
		CNMSInt32		colorMode )
{
	const CNMSInt32 prefColor[][ 2 ] = {
						{ CNMS_A_COLOR_MODE_COLOR,				PREF_COLOR_24 },
						{ CNMS_A_COLOR_MODE_COLOR_DOCUMENTS,	PREF_COLOR_24 },
						{ CNMS_A_COLOR_MODE_GRAY,				PREF_COLOR_8  },
						{ CNMS_A_COLOR_MODE_MONO,				PREF_COLOR_1  },
						{ CNMS_A_COLOR_MODE_TEXT_ENHANCED,		PREF_COLOR_1  } };
	CNMSInt32		ret = CNMS_ERR, i;

	for( i = 0 ; i < sizeof( prefColor ) / sizeof( prefColor[ 0 ] ) ; i ++ ){
		if( colorMode == prefColor[ i ][ 0 ] ){
			ret = prefColor[ i ][ 1 ];
			break;
		}
	}

	return	ret;
};

static CNMSInt32 ConvAutoPowerSettingsValue(
		CNMSInt32		*valUi,
		unsigned char	*valDevsets,
		CNMSInt32		mode )
{
	AUTOPOWER_TABLE ap_off_table[] = {
		{ CNMS_P_AUTO_POWER_OFF_DISABLE,	CNMS_DEVSET_AP_OFF_SETTING_DISABLE },
		{ CNMS_P_AUTO_POWER_OFF_15MIN,		CNMS_DEVSET_AP_OFF_SETTING_15MIN },
		{ CNMS_P_AUTO_POWER_OFF_30MIN,		CNMS_DEVSET_AP_OFF_SETTING_30MIN },
		{ CNMS_P_AUTO_POWER_OFF_60MIN,		CNMS_DEVSET_AP_OFF_SETTING_60MIN },
		{ CNMS_P_AUTO_POWER_OFF_120MIN,		CNMS_DEVSET_AP_OFF_SETTING_120MIN },
		{ CNMS_P_AUTO_POWER_OFF_240MIN,		CNMS_DEVSET_AP_OFF_SETTING_240MIN },
		{ -1,	-1 }};
	AUTOPOWER_TABLE ap_on_table[] = {
		{ CNMS_P_AUTO_POWER_ON_DISABLE,		CNMS_DEVSET_AP_ON_SETTING_DISABLE },
		{ CNMS_P_AUTO_POWER_ON_ENABLE,		CNMS_DEVSET_AP_ON_SETTING_ENABLE },
		{ -1,	-1 }};
	
	LPAUTOPOWER_TABLE	ptable = NULL;
	CNMSBool			getui = FALSE;
	CNMSInt32			ret = CNMS_ERR, i;

	if( valUi == CNMSNULL || valDevsets == CNMSNULL ) {
		goto EXIT;
	}
	switch( mode ) {
		case PREF_GET_AP_OFF_UI:
				getui = TRUE;
				ptable = ap_off_table;
				break;
		case PREF_GET_AP_ON_UI:
				getui = TRUE;
				ptable = ap_on_table;
				break;
		case PREF_GET_AP_OFF_DEV:
				getui = FALSE;
				ptable = ap_off_table;
				break;
		case PREF_GET_AP_ON_DEV:
				getui = FALSE;
				ptable = ap_on_table;
				break;
		default :
				goto EXIT;
	};
	
	if( getui ) {
		for( i = 0 ; ptable[i].valUi >= 0 ; i ++ ){
			if( ptable[i].valDevsets == *valDevsets ) {
				*valUi = ptable[i].valUi;
				ret = CNMS_NO_ERR;
				break;
			}
		}
	}
	else {
		for( i = 0 ; ptable[i].valUi >= 0 ; i ++ ){
			if( ptable[i].valUi == *valUi ) {
				*valDevsets = ptable[i].valDevsets;
				ret = CNMS_NO_ERR;
				break;
			}
		}
	}
	
EXIT:
	return	ret;

}


static CNMSVoid Preference_SetDefaultTab( CNMSVoid )
{
	GtkWidget	*notebook = CNMSNULL;

	if( lpPrefComp->preference_window == CNMSNULL ){
		DBGMSG( "[Preference_SetDefaultTab]Parameter lpPrefComp->preference_window[%p] is Invalid.\n",lpPrefComp->preference_window );
		goto	EXIT;
	}
	if( ( notebook = lookup_widget( lpPrefComp->preference_window, "notebook2" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_SetDefaultTab]Parameter notebook[%p] is Invalid.\n",notebook );
		goto	EXIT;
	}
	
	gtk_notebook_set_current_page( GTK_NOTEBOOK( notebook ), 0 );

EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_HideTab()]=%d.\n", tabID );
#endif
	return;
}

static CNMSVoid Preference_QuietSettingsDlg_Init( CNMSVoid )
{
	GtkWidget		*widget;
	
	/* quiet settings off */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_quiet_settings, "dialog_quiet_settings_radiobutton_off" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettingsDlg_Init]Can't look up widget(dialog_quiet_settings_radiobutton_off).\n" );
		goto	EXIT;
	}
	gtk_button_set_label( GTK_BUTTON( widget ), gettext( MSG_PREF_QUIET_RADIO_OFF ) );
	lpPrefComp->radio[PREF_QUIETSETTINGS_RADIO_OFF].widget = widget;
	lpPrefComp->radio[PREF_QUIETSETTINGS_RADIO_OFF].type = FALSE;
	
	/* quiet settings on */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_quiet_settings, "dialog_quiet_settings_radiobutton_on" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettingsDlg_Init]Can't look up widget(dialog_quiet_settings_radiobutton_on).\n" );
		goto	EXIT;
	}
	gtk_button_set_label( GTK_BUTTON( widget ), gettext( MSG_PREF_QUIET_RADIO_ON ) );
	lpPrefComp->radio[PREF_QUIETSETTINGS_RADIO_ON].widget = widget;
	lpPrefComp->radio[PREF_QUIETSETTINGS_RADIO_ON].type = FALSE;
	
	/* quiet settings timer */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_quiet_settings, "dialog_quiet_settings_radiobutton_timer" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettingsDlg_Init]Can't look up widget(dialog_quiet_settings_radiobutton_timer).\n" );
		goto	EXIT;
	}
	gtk_button_set_label( GTK_BUTTON( widget ), gettext( MSG_PREF_QUIET_RADIO_TIMER ) );
	lpPrefComp->radio[PREF_QUIETSETTINGS_RADIO_TIMER].widget = widget;
	lpPrefComp->radio[PREF_QUIETSETTINGS_RADIO_TIMER].type = TRUE;

	/* quiet settings timer table */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_quiet_settings, "dialog_quiet_settings_timer_table" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettingsDlg_Init]Can't look up widget(dialog_quiet_settings_timer_table).\n" );
		goto	EXIT;
	}
	lpPrefComp->timerTable = widget;
	
	/* quiet settings start */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_quiet_settings, "dialog_quiet_settings_label_start" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettingsDlg_Init]Can't look up widget(dialog_quiet_settings_label_start).\n" );
		goto	EXIT;
	}
	gtk_label_set_text( GTK_LABEL( widget ), gettext( MSG_PREF_QUIET_TIMER_START ) );
	
	/* quiet settings end */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_quiet_settings, "dialog_quiet_settings_label_end" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettingsDlg_Init]Can't look up widget(dialog_quiet_settings_label_end).\n" );
		goto	EXIT;
	}
	gtk_label_set_text( GTK_LABEL( widget ), gettext( MSG_PREF_QUIET_TIMER_END ) );
	
	/* quiet settings label */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_quiet_settings, "dialog_quiet_settings_label" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettingsDlg_Init]Can't look up widget(dialog_quiet_settings_label).\n" );
		goto	EXIT;
	}
	lpPrefComp->preference_quiet_settings_label = widget;
	gtk_label_set_text( GTK_LABEL( widget ), gettext( MSG_PREF_QUIET_MESS ) );
	
	/* quiet settings start h spin */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_quiet_settings, "dialog_quiet_settings_spin_start_h" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettingsDlg_Init]Can't look up widget(dialog_quiet_settings_spin_start_h).\n" );
		goto	EXIT;
	}
	lpPrefComp->timer_hm[PREF_QUIETSETTINGS_TIMER_START_H] = widget;
	
	/* quiet settings start m spin */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_quiet_settings, "dialog_quiet_settings_spin_start_m" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettingsDlg_Init]Can't look up widget(dialog_quiet_settings_spin_start_m).\n" );
		goto	EXIT;
	}
	lpPrefComp->timer_hm[PREF_QUIETSETTINGS_TIMER_START_M] = widget;
	
	/* quiet settings end h spin */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_quiet_settings, "dialog_quiet_settings_spin_end_h" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettingsDlg_Init]Can't look up widget(dialog_quiet_settings_spin_end_h).\n" );
		goto	EXIT;
	}
	lpPrefComp->timer_hm[PREF_QUIETSETTINGS_TIMER_END_H] = widget;
	
	/* quiet settings end m spin */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_quiet_settings, "dialog_quiet_settings_spin_end_m" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettingsDlg_Init]Can't look up widget(dialog_quiet_settings_spin_end_m).\n" );
		goto	EXIT;
	}
	lpPrefComp->timer_hm[PREF_QUIETSETTINGS_TIMER_END_M] = widget;
	
	Preference_QuietSettings_SetDefaults();
	
	
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_QuietSettingsDlg_Init()].\n" );
#endif
	return;
}

static CNMSVoid Preference_AutoPowerSettingsDlg_Init( CNMSVoid )
{
	GtkWidget		*widget;
	
	/* auto power settings vbox (includes auto power settings button ...) */
	if( ( widget = lookup_widget( lpPrefComp->preference_window, "preference_scanner_autopower_vbox" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_AutoPowerSettingsDlg_Init]Can't look up widget(preference_scanner_autopower_vbox).\n" );
		goto	EXIT;
	}
	lpPrefComp->preference_scanner_autopower_vbox = widget;
	/* auto power on label */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_autopower_settings, "dialog_autopower_settings_on_label" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_AutoPowerSettingsDlg_Init]Can't look up widget(dialog_autopower_settings_on_label).\n" );
		goto	EXIT;
	}
	lpPrefComp->dialog_autopower_settings_on_label = widget;
	/* auto power on combbox */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_autopower_settings, "dialog_autopower_settings_on_combobox" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_AutoPowerSettingsDlg_Init]Can't look up widget(dialog_autopower_settings_on_combobox).\n" );
		goto	EXIT;
	}
	lpPrefComp->dialog_autopower_settings_on_combobox = widget;
	if( W1Ui_SetComboDefVal( lpPrefComp->dialog_autopower_settings_on_combobox, CNMS_OBJ_P_AUTO_POWER_ON ) == CNMS_ERR ){
		DBGMSG( "[Preference_AutoPowerSettingsDlg_Init]Func [W1Ui_SetComboDefVal] is Error.\n" );
		goto	EXIT;
	}
	/* auto power off label */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_autopower_settings, "dialog_autopower_settings_off_label" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_AutoPowerSettingsDlg_Init]Can't look up widget(dialog_autopower_settings_off_label).\n" );
		goto	EXIT;
	}
	lpPrefComp->dialog_autopower_settings_off_label = widget;
	/* auto power off combbox */
	if( ( widget = lookup_widget( lpPrefComp->preference_dialog_autopower_settings, "dialog_autopower_settings_off_combobox" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_AutoPowerSettingsDlg_Init]Can't look up widget(dialog_autopower_settings_off_combobox).\n" );
		goto	EXIT;
	}
	lpPrefComp->dialog_autopower_settings_off_combobox = widget;
	if( W1Ui_SetComboDefVal( lpPrefComp->dialog_autopower_settings_off_combobox, CNMS_OBJ_P_AUTO_POWER_OFF ) == CNMS_ERR ){
		DBGMSG( "[Preference_AutoPowerSettingsDlg_Init]Func [W1Ui_SetComboDefVal] is Error.\n" );
		goto	EXIT;
	}
	
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_AutoPowerSettingsDlg_Init()].\n" );
#endif
	return;
}

CNMSInt32 Preference_Open(
		CNMSLPSTR	lpModel )
{
	CNMSInt32			ret		= CNMS_ERR,
						i,
						j;
	LPPREFOBJCOMP		obj		= CNMSNULL;
	GtkAdjustment		*adjust_ment;
	CNMSByte			window_title[512];

	if( lpPrefComp != CNMSNULL ){
		Preference_Close();
	}
	if( lpModel == CNMSNULL ){
		DBGMSG( "[Preference_Open]Status is error.\n" );
		goto	EXIT;
	}
	if( ( lpPrefComp = (LPPREFERENCECOMP)CnmsGetMem( sizeof( PREFERENCECOMP ) ) ) == CNMSNULL ){
		DBGMSG( "[Preference_Open]Error is occured in CnmsGetMem.\n" );
		goto	EXIT;
	}

	for( i = 0 ; i < PREF_COLOR_NUM ; i ++ ){
		for( j = 0 ; j < PREF_WIDGET_NUM ; j ++ ){
			lpPrefComp->widgetMode[ i ][ j ] = CNMS_ERR;
		}
	}
	lpPrefComp->current_color = CNMS_ERR;

	/* Preference Window */
	if( ( lpPrefComp->preference_window = create_preference_window() ) == CNMSNULL ){
		DBGMSG( "[Preference_Open]Error is occured in create_preference_window.\n" );
		goto	EXIT;
	}
	/* Quiet Settings dialog */
	if( ( lpPrefComp->preference_dialog_quiet_settings = create_dialog_quiet_settings() ) == CNMSNULL ){
		DBGMSG( "[Preference_Open]Error is occured in create_dialog_quiet_settings.\n" );
		goto	EXIT;
	}
	Preference_QuietSettingsDlg_Init();
	gtk_window_set_transient_for( GTK_WINDOW( lpPrefComp->preference_dialog_quiet_settings ), GTK_WINDOW( lpPrefComp->preference_window ) );
	
	/* AutoPower Settings dialog */
	if( ( lpPrefComp->preference_dialog_autopower_settings = create_dialog_autopower_settings() ) == CNMSNULL ){
		DBGMSG( "[Preference_Open]Error is occured in create_dialog_autopower_settings.\n" );
		goto	EXIT;
	}
	Preference_AutoPowerSettingsDlg_Init();
	gtk_window_set_transient_for( GTK_WINDOW( lpPrefComp->preference_dialog_autopower_settings ), GTK_WINDOW( lpPrefComp->preference_window ) );
	
	/* Quiet Settings get setting dialog */
	if( ( lpPrefComp->preference_dialog_get_settings = create_dialog_get_settings() ) == CNMSNULL ){
		DBGMSG( "[Preference_Open]Error is occured in create_dialog_get_settings.\n" );
		goto	EXIT;
	}
	snprintf( window_title, sizeof( window_title ) - 1 , "Canon %s", lpModel );
	gtk_window_set_title( GTK_WINDOW( lpPrefComp->preference_dialog_get_settings ), window_title );
	gtk_window_set_transient_for( GTK_WINDOW( lpPrefComp->preference_dialog_get_settings ), GTK_WINDOW( lpPrefComp->preference_window ) );
	
	/* get/set setting dialog label */
	if( ( lpPrefComp->preference_dialog_get_settings_label = lookup_widget( lpPrefComp->preference_dialog_get_settings, "dialog_get_settings_label" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_Open]Can't look up widget(dialog_get_settings_label).\n" );
		goto	EXIT;
	}
	
	/* Quiet Settings hbox */
	if( ( lpPrefComp->preference_quiet_hbox = lookup_widget( lpPrefComp->preference_window, "preference_scanner_quietsettings_hbox" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_Open]Can't look up widget(preference_scanner_quietsettings_hbox).\n" );
		goto	EXIT;
	}

	/* Color Setting */
	obj = &lpPrefComp->obj[PREF_OBJ_COLORSET];
	obj->widget		= CNMSNULL;
	obj->valInt		= CNMS_ERR;
	obj->valDouble	= CNMS_ERR;

	/* Monitor Gamma */
	obj = &lpPrefComp->obj[PREF_OBJ_MONGAMMA];	
	if( ( obj->widget = lookup_widget( lpPrefComp->preference_window, "preference_gamma_spin" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_Open]Can't look up widget(preference_gamma_spin).\n" );
		goto	EXIT;
	}
	obj->valInt		= CNMS_ERR;
	obj->valDouble	= PREF_GAMMA_RESET_VALUE;
	if( ( adjust_ment = gtk_spin_button_get_adjustment( GTK_SPIN_BUTTON( obj->widget ) ) ) == CNMSNULL ){
		DBGMSG( "[Preference_Open]Can't get adjustment of spin button at scale.\n" );
		goto	EXIT;
	}
	adjust_ment->lower = 0.1;
	adjust_ment->upper = 10.0;
	gtk_spin_button_set_adjustment( GTK_SPIN_BUTTON( obj->widget ), adjust_ment );

	/* Quiet Mode */
	obj = &lpPrefComp->obj[PREF_OBJ_QUIETMODE];	
	if( ( obj->widget = lookup_widget( lpPrefComp->preference_window, "preference_scanner_quietmode_check" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_Open]Can't look up widget(preference_scanner_quietmode_check).\n" );
		goto	EXIT;
	}
	obj->valInt		= CNMS_FALSE;
	obj->valDouble	= CNMS_ERR;

	/* Every Calibration */
	obj = &lpPrefComp->obj[PREF_OBJ_EVERYCALIB];	
	if( ( obj->widget = lookup_widget( lpPrefComp->preference_window, "preference_scanner_calibset_every_combo" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_Open]Can't look up widget(preference_scanner_calibset_every_combo).\n" );
		goto	EXIT;
	}
	obj->valInt		= 0;
	obj->valDouble	= CNMS_ERR;
	if( W1Ui_SetComboDefVal( obj->widget, CNMS_OBJ_P_EVERY_CALIBRATION ) == CNMS_ERR ){
		DBGMSG( "[Preference_Open]Func [CnmsSetComboDefVal] is Error.\n" );
		goto	EXIT;
	}
	
	/* network support */
	lpPrefComp->network_support = CnmsGetNetworkSupport( lpModel );

	ret = CNMS_NO_ERR;
EXIT:
	if( ret != CNMS_NO_ERR ){
		Preference_Close();
	}
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_Open()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSVoid Preference_Close( CNMSVoid )
{
	if( lpPrefComp != CNMSNULL ){
		CnmsFreeMem( (CNMSLPSTR)lpPrefComp );
	}
	lpPrefComp = CNMSNULL;

#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_Close()].\n" );
#endif
	return;
}

CNMSInt32 Preference_Link(
		LPCNMSUILINKCOMP	lpLink,
		CNMSInt32			linkNum )
{
	CNMSInt32		ret		= CNMS_ERR,
					i,
					j;
	GtkWidget		*widget;

	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_Link]Status is error.\n" );
		goto	EXIT;
	}
	else if( ( lpLink == CNMSNULL ) || ( linkNum <= 0 ) ){
		DBGMSG( "[Preference_Link]Parameter is error.\n" );
		goto	EXIT;
	}
	for( i = 0 ; i < linkNum ; i ++ ){
		if( ( lpLink[ i ].object == CNMS_OBJ_A_COLOR_MODE ) && ( lpLink[ i ].mode == CNMS_MODE_SELECT ) ){
			break;
		}
	}
	
	if( ( i == linkNum ) || ( ( lpPrefComp->current_color = ColorConv( lpLink[ i ].value ) ) == CNMS_ERR ) ){
		DBGMSG( "[Preference_Link]Color mode is error(%d).\n", lpLink[ i ].value );
		goto	EXIT;
	}
	
	if( lpPrefComp->widgetMode[ lpPrefComp->current_color ][ 0 ] != CNMS_ERR ){
		goto	EXIT_NO_ERR;
	}
	for( i = 0 ; i < PREF_WIDGET_NUM ; i ++ ){
		for( j = 0 ; j < linkNum ; j ++ ){
			if( ( lpLink[ j ].object == PrefWidgetComp[ i ].obj ) && ( lpLink[ j ].value == PrefWidgetComp[ i ].value ) ){
				lpPrefComp->widgetMode[ lpPrefComp->current_color ][ i ] = lpLink[ j ].mode;
				break;
			}
		}
	}
EXIT_NO_ERR:
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_Link()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSInt32 Preference_Show(
		LPCNMSUILINKCOMP	lpLink,
		CNMSInt32			linkNum )
{
	CNMSInt32		ret		= CNMS_ERR,
					i,
					ldata;
	GtkWidget		*widget	= CNMSNULL;
	LPPREFOBJCOMP	obj		= CNMSNULL;

	if( ( ldata = Preference_Link( lpLink, linkNum ) ) != CNMS_NO_ERR ){
		DBGMSG( "[Preference_Show]Error is occured in Preference_Link.\n" );
		goto	EXIT;
	}
	
	/* Color Setting */
	for( i = 0 ; i < PREF_WIDGET_NUM ; i ++ ){
		if( ( widget = lookup_widget( lpPrefComp->preference_window, PrefWidgetComp[ i ].str ) ) == CNMSNULL ){
			DBGMSG( "[Preference_Show]Can't look up widget(%s).\n", PrefWidgetComp[ i ].str );
			goto	EXIT;
		}
		switch( lpPrefComp->widgetMode[ lpPrefComp->current_color ][ i ] ){
			case	CNMS_MODE_SELECT:
				if( GTK_WIDGET_SENSITIVE( widget ) == 0 ){
					gtk_widget_set_sensitive( widget, TRUE );
				}
				gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), TRUE );
				break;
			case	CNMS_MODE_ADD:
				if( GTK_WIDGET_SENSITIVE( widget ) == 0 ){
					gtk_widget_set_sensitive( widget, TRUE );
				}
				break;
			case	CNMS_MODE_HIDE:
				if( GTK_WIDGET_SENSITIVE( widget ) != 0 ){
					gtk_widget_set_sensitive( widget, FALSE );
				}
				if( GTK_WIDGET_VISIBLE( widget ) == 0 ){
					gtk_widget_show( widget );
				}
				break;
		}
	}

	/* Monitor Gamma */
	obj = &lpPrefComp->obj[PREF_OBJ_MONGAMMA];	
	gtk_spin_button_set_value( GTK_SPIN_BUTTON( obj->widget ), obj->valDouble );

	/* Quiet Mode */
	if( W1_JudgeFormatType( CNMS_OBJ_P_SILENT_MODE ) == CNMS_TRUE ){
		obj = &lpPrefComp->obj[PREF_OBJ_QUIETMODE];	
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( obj->widget ), obj->valInt );
	}

	/* Every Calibration */
	if( W1_JudgeFormatType( CNMS_OBJ_P_EVERY_CALIBRATION ) == CNMS_TRUE ){
		obj = &lpPrefComp->obj[PREF_OBJ_EVERYCALIB];	
		gtk_combo_box_set_active( GTK_COMBO_BOX( obj->widget ), obj->valInt );
	}

	Preference_SetDefaultTab();
	/* set focus -> OK */
	W1_WidgetGrabFocus( lpPrefComp->preference_window, "preference_ok_button" );
	
	W1_ModalDialogShowAction( lpPrefComp->preference_window, main_window );

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_Show()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSVoid  Preference_Hide( CNMSVoid )
{
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_Hide]Status is error.\n" );
		goto	EXIT;
	}

	W1_ModalDialogHideAction( lpPrefComp->preference_window, main_window );

EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_Hide()].\n" );
#endif
	return;
}

CNMSInt32 Preference_Save( CNMSVoid )
{
	CNMSInt32		ret		= CNMS_ERR,
					i;
	CNMSBool		flag;
	GtkWidget		*widget	= CNMSNULL;
	LPPREFOBJCOMP	obj		= CNMSNULL;

	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_Save]Status is error.\n" );
		goto	EXIT;
	}
	/* color setting */
	for( i = 0 ; i < PREF_WIDGET_NUM ; i ++ ){
		if( ( widget = lookup_widget( lpPrefComp->preference_window, PrefWidgetComp[ i ].str ) ) == CNMSNULL ){
			DBGMSG( "[Preference_Save]Can't look up widget(%s).\n", PrefWidgetComp[ i ].str );
			goto	EXIT;
		}
		if( ( flag = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) ) != CNMS_TRUE ){
			/* toggle is OFF or HIDE */
			if( lpPrefComp->widgetMode[ lpPrefComp->current_color ][ i ] != CNMS_MODE_HIDE ){
				lpPrefComp->widgetMode[ lpPrefComp->current_color ][ i ] = CNMS_MODE_ADD;
			}
		}
		else{
			/* toggle is ON */
			lpPrefComp->widgetMode[ lpPrefComp->current_color ][ i ] = CNMS_MODE_SELECT;
		}
	}

	/* monitor gamma */
	obj = &lpPrefComp->obj[PREF_OBJ_MONGAMMA];	
	obj->valDouble = gtk_spin_button_get_value( GTK_SPIN_BUTTON( obj->widget ) );

	/* Quiet Mode */
	if( W1_JudgeFormatType( CNMS_OBJ_P_SILENT_MODE ) == CNMS_TRUE ){
		obj = &lpPrefComp->obj[PREF_OBJ_QUIETMODE];	
		obj->valInt = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( obj->widget ) );
	}
	
	/* Every Calibration */
	if( W1_JudgeFormatType( CNMS_OBJ_P_EVERY_CALIBRATION ) == CNMS_TRUE ){
		obj = &lpPrefComp->obj[PREF_OBJ_EVERYCALIB];	
		obj->valInt = gtk_combo_box_get_active( GTK_COMBO_BOX( obj->widget ) );
	}

	if( ( ret = Preview_ChangeStatus( CNMSSCPROC_ACTION_CROP_CORRECT ) ) != CNMS_NO_ERR ){
		DBGMSG( "[Preference_Save]Error is occured in Preview_ChangeStatus( CNMSSCPROC_ACTION_CROP_CORRECT )!\n" );
		goto	EXIT;
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_Save()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSInt32 Preference_GammaReset( CNMSVoid )
{
	CNMSInt32		ret = CNMS_ERR;
	LPPREFOBJCOMP	obj = CNMSNULL;
	
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_GammaReset]Status is error.\n" );
		goto	EXIT;
	}
	obj = &lpPrefComp->obj[PREF_OBJ_MONGAMMA];	
	gtk_spin_button_set_value( GTK_SPIN_BUTTON( obj->widget ), PREF_GAMMA_RESET_VALUE );

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_GammaReset()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSInt32 Preference_Get(
		LPCNMSUILINKCOMP	lpLink,
		CNMSInt32			linkNum,
		LPCNMSPREFCOMP		lpPref )
{
	CNMSInt32		ret = CNMS_ERR, i;

	if( ( lpPref == CNMSNULL ) || ( lpPrefComp == CNMSNULL ) ){
		DBGMSG( "[Preference_Get]Initialize parameter is error.\n" );
		goto	EXIT;
	}

	else if( ( i = Preference_Link( lpLink, linkNum ) ) != CNMS_NO_ERR ){
		DBGMSG( "[Preference_Get]Error is occured in Preference_Link.\n" );
		goto	EXIT;
	}

	/* color setting */
	for( i = 0 ; i < PREF_WIDGET_NUM ; i ++ ){
		if( lpPrefComp->widgetMode[ lpPrefComp->current_color ][ i ] == CNMS_MODE_SELECT ){
			lpPref->color_setting = PrefWidgetComp[ i ].value;
			break;
		}
	}

	/* monitor gamma */
	lpPref->monitor_gamma = lpPrefComp->obj[PREF_OBJ_MONGAMMA].valDouble;

	/* silent mode */
	lpPref->silent_mode = lpPrefComp->obj[PREF_OBJ_QUIETMODE].valInt;

	/* every calibration */
	lpPref->every_calibration = lpPrefComp->obj[PREF_OBJ_EVERYCALIB].valInt;

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_Get()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSVoid Preference_HideTab(
		CNMSInt32			tabID )
{
	GtkWidget	*notebook = CNMSNULL;

	if( ( tabID < 0 ) || ( PREF_TAB_MAX <= tabID ) ){
		DBGMSG( "[Preference_HideTab]Arg parameter tabID[%d] is Invalid.\n",tabID );
		goto	EXIT;
	}
	if( lpPrefComp->preference_window == CNMSNULL ){
		DBGMSG( "[Preference_HideTab]Parameter lpPrefComp->preference_window[%p] is Invalid.\n",lpPrefComp->preference_window );
		goto	EXIT;
	}
	if( ( notebook = lookup_widget( lpPrefComp->preference_window, "notebook2" ) ) == CNMSNULL ){
		DBGMSG( "[Preference_HideTab]Parameter notebook[%p] is Invalid.\n",notebook );
		goto	EXIT;
	}
	
	gtk_notebook_remove_page( GTK_NOTEBOOK( notebook ), tabID );

EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_HideTab()]=%d.\n", tabID );
#endif
	return;
}

CNMSInt32 Preference_GetSensitive( CNMSVoid )
{
	if( lpPrefComp == CNMSNULL ){
		return CNMS_TRUE;
	}
	else if( lpPrefComp->preference_window == CNMSNULL ) {
		return CNMS_TRUE;
	}
	else {
		return ( GTK_WIDGET_SENSITIVE( lpPrefComp->preference_window ) );
	}
}


CNMSVoid Preference_InitSilent(
		CNMSInt32 type )
{
	LPPREFOBJCOMP	obj		= CNMSNULL;
	GtkWidget		*widget	= CNMSNULL;
	CNMSBool		hide_silent		= FALSE;	/* checkbox */
	CNMSBool		hide_quiet		= FALSE;	/* button */
	CNMSBool		hide_sep		= FALSE;	/* separator */
	CNMSBool		hide_quiet_mess	= FALSE;	/* message (quiet settings dialog) */
	
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_InitSilent]Initialize parameter is error.\n" );
		goto	EXIT;
	}
	
	switch ( type ) {
		case CNMS_P_SILENT_MODE_NULL :
			hide_silent		= TRUE;
			hide_quiet		= TRUE;
			hide_sep		= TRUE;
			break;
		case CNMS_P_SILENT_MODE_TYPE2 :
			hide_silent		= TRUE;
			hide_quiet_mess	= TRUE;
			break;
		case CNMS_P_SILENT_MODE_TYPE3 :
			hide_silent		= TRUE;
			break;
		case CNMS_P_SILENT_MODE_OFF :
		case CNMS_P_SILENT_MODE_ON :
			hide_quiet		= TRUE;
			break;
		default :
			/* do nothing. (scanner tab hidden) */
			break;
	}
	
	/* widgets control */
	if( hide_silent ) {
		/* Silent Mode */
		obj = &lpPrefComp->obj[PREF_OBJ_QUIETMODE];	
		if( obj->widget == CNMSNULL ){
			DBGMSG( "[Preference_InitSilent]Can't look up widget(preference_scanner_quietmode_check).\n" );
			goto	EXIT;
		}
		gtk_widget_hide( obj->widget );
	}
	if( hide_quiet ) {
		/* Quiet Settings hbox */
		gtk_widget_hide( lpPrefComp->preference_quiet_hbox );
	}
	if( hide_quiet_mess ) {
		/* Quiet Settings hbox */
		gtk_widget_hide( lpPrefComp->preference_quiet_settings_label );
	}
	if( hide_sep ) {
		/* Scanner Tab Separator */
		if( ( widget = lookup_widget( lpPrefComp->preference_window, "preference_scanner_separator" ) ) == CNMSNULL ){
			DBGMSG( "[Preference_InitSilent]Can't look up widget(preference_scanner_separator).\n" );
			goto	EXIT;
		}
		gtk_widget_hide( widget );
	}
	
	/* get struct version */
	

EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_InitSilent()]\n" );
#endif
	return;
}

/* get value from Quiet Settings dialog */
static CNMSInt32 Preference_QuietSettings_GetValue(
	CANON_DEVICE_SETTINGS	*pDevsets )
{
	CNMSInt32				ret = CNMS_ERR;
	CNMSDec32				dval = 0;
	unsigned char			cval = 0;
	
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettings_GetValue]Status is error.\n" );
		goto	EXIT;
	}
	else if ( pDevsets == CNMSNULL ) {
		DBGMSG( "[Preference_QuietSettings_GetValue]Status is error.\n" );
		goto	EXIT;
	}
	
	/*
		PREF_QUIETSETTINGS_RADIO_OFF		= CNMS_DEVSET_QUIET_SETTING_OFF,
		PREF_QUIETSETTINGS_RADIO_ON			= CNMS_DEVSET_QUIET_SETTING_ON,
		PREF_QUIETSETTINGS_RADIO_TIMER		= CNMS_DEVSET_QUIET_SETTING_TIMER,
		
		PREF_QUIETSETTINGS_TIMER_START_H	= CNMS_DEVSET_QUIET_SETTING_TIMER_START_H,
		PREF_QUIETSETTINGS_TIMER_START_M	= CNMS_DEVSET_QUIET_SETTING_TIMER_START_M,
		PREF_QUIETSETTINGS_TIMER_END_H		= CNMS_DEVSET_QUIET_SETTING_TIMER_END_H,
		PREF_QUIETSETTINGS_TIMER_END_M		= CNMS_DEVSET_QUIET_SETTING_TIMER_END_M,
	*/
	for( cval = 0; cval < PREF_QUIETSETTINGS_RADIO_MAX; cval++ ) {
		if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(lpPrefComp->radio[ cval ].widget) ) ) {
			break;
		}
	}
	if( cval == PREF_QUIETSETTINGS_RADIO_MAX ) {
		DBGMSG( "[Preference_QuietSettings_GetValue]Status is error.\n" );
		goto	EXIT;
	}
	pDevsets->quiet_setting = cval;
	
	dval = gtk_spin_button_get_value( GTK_SPIN_BUTTON(lpPrefComp->timer_hm[ PREF_QUIETSETTINGS_TIMER_START_H ]) );
	pDevsets->quiet_setting_time[ CNMS_DEVSET_QUIET_SETTING_TIMER_START_H ]	= (unsigned char)dval;
	dval = gtk_spin_button_get_value( GTK_SPIN_BUTTON(lpPrefComp->timer_hm[ PREF_QUIETSETTINGS_TIMER_START_M ]) );
	pDevsets->quiet_setting_time[ CNMS_DEVSET_QUIET_SETTING_TIMER_START_M ]	= (unsigned char)dval;
	dval = gtk_spin_button_get_value( GTK_SPIN_BUTTON(lpPrefComp->timer_hm[ PREF_QUIETSETTINGS_TIMER_END_H ]) );
	pDevsets->quiet_setting_time[ CNMS_DEVSET_QUIET_SETTING_TIMER_END_H ]	= (unsigned char)dval;
	dval = gtk_spin_button_get_value( GTK_SPIN_BUTTON(lpPrefComp->timer_hm[ PREF_QUIETSETTINGS_TIMER_END_M ]) );
	pDevsets->quiet_setting_time[ CNMS_DEVSET_QUIET_SETTING_TIMER_END_M ]	= (unsigned char)dval;
	
	ret = CNMS_NO_ERR;
	
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_QuietSettings_Show()]\n" );
#endif
	return ret;
}

/* set value to Quiet Settings dialog */
static CNMSInt32 Preference_QuietSettings_SetValue(
	CANON_DEVICE_SETTINGS	*pDevsets )
{
	CNMSInt32				ret = CNMS_ERR;
	
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettings_SetValue]Status is error.\n" );
		goto	EXIT;
	}
	else if ( pDevsets == CNMSNULL ) {
		DBGMSG( "[Preference_QuietSettings_SetValue]Status is error.\n" );
		goto	EXIT;
	}
	
	/*
		PREF_QUIETSETTINGS_RADIO_OFF		= CNMS_DEVSET_QUIET_SETTING_OFF,
		PREF_QUIETSETTINGS_RADIO_ON			= CNMS_DEVSET_QUIET_SETTING_ON,
		PREF_QUIETSETTINGS_RADIO_TIMER		= CNMS_DEVSET_QUIET_SETTING_TIMER,
		
		PREF_QUIETSETTINGS_TIMER_START_H	= CNMS_DEVSET_QUIET_SETTING_TIMER_START_H,
		PREF_QUIETSETTINGS_TIMER_START_M	= CNMS_DEVSET_QUIET_SETTING_TIMER_START_M,
		PREF_QUIETSETTINGS_TIMER_END_H		= CNMS_DEVSET_QUIET_SETTING_TIMER_END_H,
		PREF_QUIETSETTINGS_TIMER_END_M		= CNMS_DEVSET_QUIET_SETTING_TIMER_END_M,
	*/
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(lpPrefComp->radio[ pDevsets->quiet_setting ].widget), TRUE );
	Preference_QuietSettings_RadioClicked( pDevsets->quiet_setting );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(lpPrefComp->timer_hm[ PREF_QUIETSETTINGS_TIMER_START_H ]),
										pDevsets->quiet_setting_time[ CNMS_DEVSET_QUIET_SETTING_TIMER_START_H ] );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(lpPrefComp->timer_hm[ PREF_QUIETSETTINGS_TIMER_START_M ]),
										pDevsets->quiet_setting_time[ CNMS_DEVSET_QUIET_SETTING_TIMER_START_M ] );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(lpPrefComp->timer_hm[ PREF_QUIETSETTINGS_TIMER_END_H ]),
										pDevsets->quiet_setting_time[ CNMS_DEVSET_QUIET_SETTING_TIMER_END_H ] );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(lpPrefComp->timer_hm[ PREF_QUIETSETTINGS_TIMER_END_M ]),
										pDevsets->quiet_setting_time[ CNMS_DEVSET_QUIET_SETTING_TIMER_END_M ] );
	
	ret = CNMS_NO_ERR;
	
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_QuietSettings_Show()]\n" );
#endif
	return ret;
}


CNMSVoid Preference_InitAutoPower(
		CNMSInt32 type )
{
	GtkWidget		*widget	= CNMSNULL;
	CNMSBool		hide_autopower	= FALSE;	/* auto power vbox */
	
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_InitAutoPower]Initialize parameter is error.\n" );
		goto	EXIT;
	}
	
	switch ( type ) {
		case CNMS_P_AUTO_POWER_SETTINGS_NULL :
			hide_autopower	= TRUE;
			lpPrefComp->autopower_type = 0;
			break;
		case CNMS_P_AUTO_POWER_SETTINGS_TYPE1 :
			hide_autopower	= FALSE;
			lpPrefComp->autopower_type = CNMS_DEVSET_MODE_AP_OFF | CNMS_DEVSET_MODE_AP_ON;
			break;
		default :
			/* do nothing. (scanner tab hidden) */
			break;
	}
	
	/* widgets control */
	if( hide_autopower ) {
		gtk_widget_hide( lpPrefComp->preference_scanner_autopower_vbox );
	}
	if( !( lpPrefComp->autopower_type & CNMS_DEVSET_MODE_AP_ON ) ) {
		gtk_widget_hide( lpPrefComp->dialog_autopower_settings_on_label );
		gtk_widget_hide( lpPrefComp->dialog_autopower_settings_on_combobox );
	}
	if( !( lpPrefComp->autopower_type & CNMS_DEVSET_MODE_AP_OFF ) ) {
		gtk_widget_hide( lpPrefComp->dialog_autopower_settings_off_label );
		gtk_widget_hide( lpPrefComp->dialog_autopower_settings_off_combobox );
	}

EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_InitAutoPower()]\n" );
#endif
	return;
}

/* get value from Auto Power Settings dialog */
static CNMSInt32 Preference_AutoPowerSettings_GetValue(
	CANON_DEVICE_SETTINGS	*pDevsets )
{
	CNMSLPSTR		str = CNMSNULL;
	CNMSInt32		ret = CNMS_ERR, retVal = CNMS_ERR;
	CNMSInt32		valUi;
	unsigned char	valDevsets;
	
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_AutoPowerSettings_GetValue]Status is error.\n" );
		goto	EXIT;
	}
	else if ( pDevsets == CNMSNULL ) {
		DBGMSG( "[Preference_AutoPowerSettings_GetValue]Status is error.\n" );
		goto	EXIT;
	}
	
	/* auto power on */
	if( lpPrefComp->autopower_type & CNMS_DEVSET_MODE_AP_ON ) {
		/* get active string(auto power on) */
		if( ( str = gtk_combo_box_get_active_text( GTK_COMBO_BOX( lpPrefComp->dialog_autopower_settings_on_combobox ) ) ) == CNMSNULL ){
			DBGMSG( "[Preference_AutoPowerSettings_GetValue] Pointer[str] is NULL.\n" );
			goto	EXIT;
		}
		/* combobox text -> ui value */
		if( ( valUi = CnmsChgStrToVal( str, CNMS_OBJ_P_AUTO_POWER_ON ) ) == CNMS_ERR ) {
			DBGMSG( "[Preference_AutoPowerSettings_GetValue]CnmsChgStrToVal error.\n" );
			goto EXIT;
		}
		/* ui value -> devsets value */
		if( ConvAutoPowerSettingsValue( &valUi, &valDevsets, PREF_GET_AP_ON_DEV ) == CNMS_ERR ) {
			DBGMSG( "[Preference_AutoPowerSettings_GetValue]ConvAutoPowerSettingsValue error.\n" );
			goto EXIT;
		}
		pDevsets->auto_power_on_setting = valDevsets;
	}
	
	/* auto power off */
	if( lpPrefComp->autopower_type & CNMS_DEVSET_MODE_AP_OFF ) {
		/* get active string(auto power off) */
		if( ( str = gtk_combo_box_get_active_text( GTK_COMBO_BOX( lpPrefComp->dialog_autopower_settings_off_combobox ) ) ) == CNMSNULL ){
			DBGMSG( "[Preference_AutoPowerSettings_GetValue] Pointer[str] is NULL.\n" );
			goto	EXIT;
		}
		/* combobox text -> ui value */
		if( ( valUi = CnmsChgStrToVal( str, CNMS_OBJ_P_AUTO_POWER_OFF ) ) == CNMS_ERR ) {
			DBGMSG( "[Preference_AutoPowerSettings_GetValue]CnmsChgStrToVal error.\n" );
			goto EXIT;
		}
		/* ui value -> devsets value */
		if( ConvAutoPowerSettingsValue( &valUi, &valDevsets, PREF_GET_AP_OFF_DEV ) == CNMS_ERR ) {
			DBGMSG( "[Preference_AutoPowerSettings_GetValue]ConvAutoPowerSettingsValue error.\n" );
			goto EXIT;
		}
		pDevsets->auto_power_off_setting = valDevsets;
	}
	
	ret = CNMS_NO_ERR;
	
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_AutoPowerSettings_GetValue()]\n" );
#endif
	return ret;
}

/* set value to Auto Power Settings dialog */
static CNMSInt32 Preference_AutoPowerSettings_SetValue(
	CANON_DEVICE_SETTINGS	*pDevsets )
{
	CNMSLPSTR		str = CNMSNULL;
	CNMSInt32		ret = CNMS_ERR, retVal = CNMS_ERR;
	CNMSInt32		valUi;
	unsigned char	valDevsets;
	
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_AutoPowerSettings_SetValue]Status is error.\n" );
		goto	EXIT;
	}
	else if ( pDevsets == CNMSNULL ) {
		DBGMSG( "[Preference_AutoPowerSettings_SetValue]Status is error.\n" );
		goto	EXIT;
	}
	
	/* auto power on */
	if( lpPrefComp->autopower_type & CNMS_DEVSET_MODE_AP_ON ) {
		/* set active string(auto power on) */
		valDevsets = pDevsets->auto_power_on_setting;
		/* devsets value -> ui value */
		if( ConvAutoPowerSettingsValue( &valUi, &valDevsets, PREF_GET_AP_ON_UI ) == CNMS_ERR ) {
			DBGMSG( "[Preference_AutoPowerSettings_SetValue]ConvAutoPowerSettingsValue error.\n" );
			goto EXIT;
		}
		/* ui value -> combobox text */
		if( ( str = CnmsChgValToStr( valUi, CNMS_OBJ_P_AUTO_POWER_ON ) ) == CNMSNULL ) {
			DBGMSG( "[Preference_AutoPowerSettings_SetValue]CnmsChgValToStr error.\n" );
			goto EXIT;
		}
		/* setup combobox */
		if( W1Ui_SetComboSelectedVal( lpPrefComp->dialog_autopower_settings_on_combobox, CNMS_OBJ_P_AUTO_POWER_ON, str ) == CNMS_ERR ){
			DBGMSG( "[Preference_AutoPowerSettings_SetValue]Func [W1Ui_SetComboSelectedVal] is Error.\n" );
			goto	EXIT;
		}
	}
	
	/* auto power off */
	if( lpPrefComp->autopower_type & CNMS_DEVSET_MODE_AP_OFF ) {
		/* set active string(auto power off) */
		valDevsets = pDevsets->auto_power_off_setting;
		/* devsets value -> ui value */
		if( ConvAutoPowerSettingsValue( &valUi, &valDevsets, PREF_GET_AP_OFF_UI ) == CNMS_ERR ) {
			DBGMSG( "[Preference_AutoPowerSettings_SetValue]ConvAutoPowerSettingsValue error.\n" );
			goto EXIT;
		}
		/* ui value -> combobox text */
		if( ( str = CnmsChgValToStr( valUi, CNMS_OBJ_P_AUTO_POWER_OFF ) ) == CNMSNULL ) {
			DBGMSG( "[Preference_AutoPowerSettings_SetValue]CnmsChgValToStr error.\n" );
			goto EXIT;
		}
		/* setup combobox */
		if( W1Ui_SetComboSelectedVal( lpPrefComp->dialog_autopower_settings_off_combobox, CNMS_OBJ_P_AUTO_POWER_OFF, str ) == CNMS_ERR ){
			DBGMSG( "[Preference_AutoPowerSettings_SetValue]Func [W1Ui_SetComboSelectedVal] is Error.\n" );
			goto	EXIT;
		}
	}
	
	ret = CNMS_NO_ERR;
	
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_AutoPowerSettings_SetValue()]\n" );
#endif
	return ret;
}


static CNMSInt32 result_Preference_GetDeviceSettings_Exec = CNMS_ERR;

gboolean Preference_GetDeviceSettings_Exec(
		gpointer	data )
{
	CANON_DEVICE_SETTINGS	*pdevsets = (CANON_DEVICE_SETTINGS *)data;

	result_Preference_GetDeviceSettings_Exec = CNMS_ERR;
	if ( data == CNMSNULL ) {
		DBGMSG( "[Preference_GetDeviceSettings_Exec]Status is error.\n" );
		goto	EXIT;
	}
	
	result_Preference_GetDeviceSettings_Exec = CnmsGetDeviceSettings( pdevsets );
	
EXIT:
	gtk_widget_hide( lpPrefComp->preference_dialog_get_settings );

	/* do only once */
	return FALSE;
}


static CNMSInt32 result_Preference_SetDeviceSettings_Exec = CNMS_ERR;

gboolean Preference_SetDeviceSettings_Exec(
		gpointer	data )
{
	CANON_DEVICE_SETTINGS	*pdevsets = (CANON_DEVICE_SETTINGS *)data;

	result_Preference_SetDeviceSettings_Exec = CNMS_ERR;
	if ( data == CNMSNULL ) {
		DBGMSG( "[Preference_SetDeviceSettings_Exec]Status is error.\n" );
		goto	EXIT;
	}
	
	result_Preference_SetDeviceSettings_Exec = CnmsSetDeviceSettings( pdevsets );
	
EXIT:
	gtk_widget_hide( lpPrefComp->preference_dialog_get_settings );

	/* do only once */
	return FALSE;
}

#define	CNMS_PREF_DELAY_TIME	(1000)	/* msec */

CNMSInt32 Preference_QuietSettings_Show(
		CNMSVoid )
{
	CANON_DEVICE_SETTINGS	devsets;
	CNMSInt32				ret = CNMS_ERR, retval = CNMS_ERR;
	
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettings_Show]Status is error.\n" );
		goto	EXIT;
	}
	else if ( lpPrefComp->preference_dialog_quiet_settings == CNMSNULL ) {
		DBGMSG( "[Preference_QuietSettings_Show]Status is error.\n" );
		goto	EXIT;
	}
	CnmsSetMem( (CNMSLPSTR)&devsets, 0, sizeof( CANON_DEVICE_SETTINGS ) );
	Preference_QuietSettings_SetDefaults();
	/* set focus -> OK */
	W1_WidgetGrabFocus( lpPrefComp->preference_dialog_quiet_settings, "dialog_quiet_settings_button_ok" );
	
	/* set "get settings" message */
	gtk_label_set_text( GTK_LABEL( lpPrefComp->preference_dialog_get_settings_label ), gettext( MSG_PREF_GET_SETTINGS ) );
	
	/* disable preference window */
	if( GTK_WIDGET_VISIBLE( lpPrefComp->preference_window ) ){
		if( GTK_WIDGET_SENSITIVE( lpPrefComp->preference_window ) ){
			gtk_widget_set_sensitive( lpPrefComp->preference_window, FALSE );
		}
	}
	else {
		goto EXIT;
	}
	
	/* get settings... */
	/* show dialog */
	gtk_window_set_transient_for( GTK_WINDOW( lpPrefComp->preference_dialog_get_settings ), GTK_WINDOW( lpPrefComp->preference_window ) );
	gtk_widget_show( lpPrefComp->preference_dialog_get_settings );
	
	result_Preference_GetDeviceSettings_Exec = CNMS_ERR;
	/* get device settings(quiet), dialog hide -> call [CNMS_PREF_DELAY_TIME]msec later */
	devsets.mode = CNMS_DEVSET_MODE_QUIET;
	gtk_timeout_add( CNMS_PREF_DELAY_TIME, ( GtkFunction )Preference_GetDeviceSettings_Exec, (gpointer)&devsets );
	ComGtk_GtkMain();
	
	retval = result_Preference_GetDeviceSettings_Exec;
	
	if( retval != CNMS_NO_ERR ) {
		if( ShowErrorDialog() == ERROR_QUIT_TRUE ){
			Preference_Hide();
			W1_Close();
			goto EXIT;
		}
	}
	
	/* set devsets value */
	if( Preference_QuietSettings_SetValue( &devsets ) != CNMS_NO_ERR ) {
		DBGMSG( "[Preference_QuietSettings_Show]Preference_QuietSettings_SetValue is error.\n" );
		goto EXIT;
	}
	
	gtk_window_set_transient_for( GTK_WINDOW( lpPrefComp->preference_dialog_quiet_settings ), GTK_WINDOW( lpPrefComp->preference_window ) );
	gtk_widget_show( lpPrefComp->preference_dialog_quiet_settings );
	
	ret = CNMS_NO_ERR;
	
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_QuietSettings_Show()]\n" );
#endif
	return ret;
}

CNMSInt32 Preference_QuietSettings_Hide(
		CNMSInt32 mode )
{
	CANON_DEVICE_SETTINGS	devsets;
	CNMSInt32				ret = CNMS_ERR, retval = CNMS_ERR, retChild = CNMS_ERR;
	CNMSBool				preference_activate = CNMS_TRUE;
	
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettings_Hide]Status is error.\n" );
		goto	EXIT;
	}
	else if ( lpPrefComp->preference_dialog_quiet_settings == CNMSNULL ) {
		DBGMSG( "[Preference_QuietSettings_Hide]Status is error.\n" );
		goto	EXIT;
	}
	
	CnmsSetMem( (CNMSLPSTR)&devsets, 0, sizeof( CANON_DEVICE_SETTINGS ) );
	if ( mode == PREF_QUIETSETTINGS_CANCEL ) {
		/* hide dialog */
		gtk_widget_hide( lpPrefComp->preference_dialog_quiet_settings );
	}
	else {
		/* show apply dialog */
		if ( ( retChild = ChildDialogShow( CHILDDIALOG_ID_SETTING_APPLY, lpPrefComp->preference_dialog_quiet_settings ) ) != COMGTK_GTKMAIN_OK ) {
			/* cancel -> do nothing */
			preference_activate = CNMS_FALSE;
		}
		else {
			/* get devsets value from dialog */
			if( Preference_QuietSettings_GetValue( &devsets ) != CNMS_NO_ERR ) {
				DBGMSG( "[Preference_SetDeviceSettings_Exec]Preference_QuietSettings_GetValue is error.\n" );
				goto EXIT;
			}
			/* hide dialog */
			gtk_widget_hide( lpPrefComp->preference_dialog_quiet_settings );
			
			/* send settings... */
			/* set "set settings" message */
			gtk_label_set_text( GTK_LABEL( lpPrefComp->preference_dialog_get_settings_label ), gettext( MSG_PREF_SET_SETTINGS ) );
			/* show dialog */
			gtk_window_set_transient_for( GTK_WINDOW( lpPrefComp->preference_dialog_get_settings ), GTK_WINDOW( lpPrefComp->preference_window ) );
			gtk_widget_show( lpPrefComp->preference_dialog_get_settings );
	
			result_Preference_SetDeviceSettings_Exec = CNMS_ERR;
			/* set device settings(quiet), dialog hide -> call [CNMS_PREF_DELAY_TIME]msec later */
			devsets.mode = CNMS_DEVSET_MODE_QUIET;
			gtk_timeout_add( CNMS_PREF_DELAY_TIME , ( GtkFunction )Preference_SetDeviceSettings_Exec, (gpointer)&devsets );
			ComGtk_GtkMain();
			
			retval = result_Preference_SetDeviceSettings_Exec;
			
			if( retval != CNMS_NO_ERR ) {
				if( ShowErrorDialog() == ERROR_QUIT_TRUE ){
					Preference_Hide();
					W1_Close();
					goto EXIT;
				}
			}
		}
	}
	
	/* activate preference window */
	if ( preference_activate ) {
		if( GTK_WIDGET_VISIBLE( lpPrefComp->preference_window ) ){
			if( !GTK_WIDGET_SENSITIVE( lpPrefComp->preference_window ) ){
				gtk_widget_set_sensitive( lpPrefComp->preference_window, TRUE );
			}
		}
	}
	
	ret = CNMS_NO_ERR;
	
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_QuietSettings_Hide()]\n" );
#endif
	return ret;
}

CNMSInt32 Preference_QuietSettings_SetDefaults(
		CNMSVoid )
{
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettings_SetDefaults]Status is error.\n" );
		goto	EXIT;
	}
	else if ( lpPrefComp->preference_dialog_quiet_settings == CNMSNULL ) {
		DBGMSG( "[Preference_QuietSettings_SetDefaults]Status is error.\n" );
		goto	EXIT;
	}
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(lpPrefComp->radio[PREF_QUIETSETTINGS_RADIO_OFF].widget), TRUE );
	Preference_QuietSettings_RadioClicked( PREF_QUIETSETTINGS_RADIO_OFF );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(lpPrefComp->timer_hm[ PREF_QUIETSETTINGS_TIMER_START_H ]),	21 );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(lpPrefComp->timer_hm[ PREF_QUIETSETTINGS_TIMER_START_M ]),	0 );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(lpPrefComp->timer_hm[ PREF_QUIETSETTINGS_TIMER_END_H ]),		7 );
	gtk_spin_button_set_value( GTK_SPIN_BUTTON(lpPrefComp->timer_hm[ PREF_QUIETSETTINGS_TIMER_END_M ]),		0 );
	
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_QuietSettings_Hide()]\n" );
#endif
	return;
}

CNMSInt32 Preference_QuietSettings_RadioClicked(
		CNMSInt32 mode )
{
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_QuietSettings_RadioClicked]Status is error.\n" );
		goto	EXIT;
	}
	gtk_widget_set_sensitive( lpPrefComp->timerTable, lpPrefComp->radio[ mode ].type );
	
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_QuietSettings_RadioClicked()]\n" );
#endif
	return;
}


CNMSInt32 Preference_AutoPowerSettings_Show( CNMSVoid )
{
	CANON_DEVICE_SETTINGS	devsets;
	CNMSInt32				ret = CNMS_ERR, retval = CNMS_ERR;
	
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_AutoPowerSettings_Show]Status is error.\n" );
		goto	EXIT;
	}
	else if ( lpPrefComp->preference_dialog_autopower_settings == CNMSNULL ) {
		DBGMSG( "[Preference_AutoPowerSettings_Show]Status is error.\n" );
		goto	EXIT;
	}
	CnmsSetMem( (CNMSLPSTR)&devsets, 0, sizeof( CANON_DEVICE_SETTINGS ) );
	/* set focus -> OK */
	W1_WidgetGrabFocus( lpPrefComp->preference_dialog_autopower_settings, "dialog_autopower_settings_button_ok" );
	
	/* set "get settings" message */
	gtk_label_set_text( GTK_LABEL( lpPrefComp->preference_dialog_get_settings_label ), gettext( MSG_PREF_GET_SETTINGS ) );
	
	/* disable preference window */
	if( GTK_WIDGET_VISIBLE( lpPrefComp->preference_window ) ){
		if( GTK_WIDGET_SENSITIVE( lpPrefComp->preference_window ) ){
			gtk_widget_set_sensitive( lpPrefComp->preference_window, FALSE );
		}
	}
	else {
		goto EXIT;
	}
	
	/* get settings... */
	/* show dialog */
	gtk_window_set_transient_for( GTK_WINDOW( lpPrefComp->preference_dialog_get_settings ), GTK_WINDOW( lpPrefComp->preference_window ) );
	gtk_widget_show( lpPrefComp->preference_dialog_get_settings );
	
	result_Preference_GetDeviceSettings_Exec = CNMS_ERR;
	/* get device settings(autopower off/on), dialog hide -> call 500msec later */
	devsets.mode = lpPrefComp->autopower_type;
	gtk_timeout_add( CNMS_PREF_DELAY_TIME, ( GtkFunction )Preference_GetDeviceSettings_Exec, (gpointer)&devsets );
	ComGtk_GtkMain();
	
	retval = result_Preference_GetDeviceSettings_Exec;
	
	if( retval != CNMS_NO_ERR ) {
		if( ShowErrorDialog() == ERROR_QUIT_TRUE ){
			Preference_Hide();
			W1_Close();
			goto EXIT;
		}
	}

	/* set devsets value */
	if( Preference_AutoPowerSettings_SetValue( &devsets ) != CNMS_NO_ERR ) {
		DBGMSG( "[Preference_AutoPowerSettings_Show]Preference_AutoPowerSettings_SetValue is error.\n" );
		goto EXIT;
	}
	
	gtk_window_set_transient_for( GTK_WINDOW( lpPrefComp->preference_dialog_autopower_settings ), GTK_WINDOW( lpPrefComp->preference_window ) );
	gtk_widget_show( lpPrefComp->preference_dialog_autopower_settings );
	
	ret = CNMS_NO_ERR;
	
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_AutoPowerSettings_Show()]\n" );
#endif
	return ret;
}

CNMSInt32 Preference_AutoPowerSettings_Hide( CNMSInt32 mode )
{
	CANON_DEVICE_SETTINGS	devsets;
	CNMSInt32				ret = CNMS_ERR, retval = CNMS_ERR, retChild = CNMS_ERR;
	CNMSInt32				idChild = CNMS_ERR;
	CNMSBool				preference_activate = CNMS_TRUE;
	
	if( lpPrefComp == CNMSNULL ){
		DBGMSG( "[Preference_AutoPowerSettings_Hide]Status is error.\n" );
		goto	EXIT;
	}
	else if ( lpPrefComp->preference_dialog_autopower_settings == CNMSNULL ) {
		DBGMSG( "[Preference_AutoPowerSettings_Hide]Status is error.\n" );
		goto	EXIT;
	}
	
	CnmsSetMem( (CNMSLPSTR)&devsets, 0, sizeof( CANON_DEVICE_SETTINGS ) );
	if ( mode == PREF_QUIETSETTINGS_CANCEL ) {
		/* hide dialog */
		gtk_widget_hide( lpPrefComp->preference_dialog_autopower_settings );
	}
	else {
		/* get devsets value from dialog */
		if( Preference_AutoPowerSettings_GetValue( &devsets ) != CNMS_NO_ERR ) {
			DBGMSG( "[Preference_SetDeviceSettings_Exec]Preference_QuietSettings_GetValue is error.\n" );
			goto EXIT;
		}
		
		idChild = CHILDDIALOG_ID_SETTING_APPLY;
		/* check auto power off support */
		if( lpPrefComp->autopower_type & CNMS_DEVSET_MODE_AP_OFF ) {
			/* select message of apply dialog */
			if ( ( devsets.auto_power_off_setting != CNMS_DEVSET_AP_OFF_SETTING_DISABLE ) && lpPrefComp->network_support ) {
				idChild = CHILDDIALOG_ID_AUTOPOWEROFF_APPLY;
			}
			else {
				idChild = CHILDDIALOG_ID_SETTING_APPLY;
			}
		}
		/* show apply dialog */
		if ( ( retChild = ChildDialogShow( idChild, lpPrefComp->preference_dialog_autopower_settings ) ) != COMGTK_GTKMAIN_OK ) {
			/* cancel -> do nothing */
			preference_activate = CNMS_FALSE;
		}
		else {
			/* hide dialog */
			gtk_widget_hide( lpPrefComp->preference_dialog_autopower_settings );
			
			/* send settings... */
			/* set "set settings" message */
			gtk_label_set_text( GTK_LABEL( lpPrefComp->preference_dialog_get_settings_label ), gettext( MSG_PREF_SET_SETTINGS ) );
			/* show dialog */
			gtk_window_set_transient_for( GTK_WINDOW( lpPrefComp->preference_dialog_get_settings ), GTK_WINDOW( lpPrefComp->preference_window ) );
			gtk_widget_show( lpPrefComp->preference_dialog_get_settings );
	
			result_Preference_SetDeviceSettings_Exec = CNMS_ERR;
			/* set device settings(autopower off/on), dialog hide -> call [CNMS_PREF_DELAY_TIME]msec later */
			devsets.mode = lpPrefComp->autopower_type;
			gtk_timeout_add( CNMS_PREF_DELAY_TIME , ( GtkFunction )Preference_SetDeviceSettings_Exec, (gpointer)&devsets );
			ComGtk_GtkMain();
			
			retval = result_Preference_SetDeviceSettings_Exec;
			
			if( retval != CNMS_NO_ERR ) {
				if( ShowErrorDialog() == ERROR_QUIT_TRUE ){
					Preference_Hide();
					W1_Close();
					goto EXIT;
				}
			}
		}
	}
	
	/* activate preference window */
	if ( preference_activate ) {
		if( GTK_WIDGET_VISIBLE( lpPrefComp->preference_window ) ){
			if( !GTK_WIDGET_SENSITIVE( lpPrefComp->preference_window ) ){
				gtk_widget_set_sensitive( lpPrefComp->preference_window, TRUE );
			}
		}
	}
	
EXIT:
#ifdef	__CNMS_DEBUG_PREFERENCE__
	DBGMSG( "[Preference_AutoPowerSettings_Hide()]\n" );
#endif
	return ret;
}



#endif	/* _PREFERENCE_C_ */

