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


#include <gtk/gtk.h>


void
main_window_destroy                    (GtkObject       *object,
                                        gpointer         user_data);

gboolean
on_main_window_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_preview_clear_button_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_toolbar_about_button_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_preference_button_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_close_button_clicked                (GtkButton       *button,
                                        gpointer         user_data);
gboolean
on_main_preview_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_main_preview_button_release_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_main_preview_motion_notify_event    (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_main_preview_configure_event        (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_main_preview_expose_event           (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_main_notebook_switch_page           (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        guint            page_num,
                                        gpointer         user_data);

void
on_simple_src_combo_changed            (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_simple_preview_button_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_simple_dst_combo_changed            (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_simple_output_size_combo_changed    (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_simple_scan_button_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_advance_src_combo_changed           (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_advance_paper_combo_changed         (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_advance_binding_combo_changed       (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_advance_color_combo_changed         (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_advance_input_height_spin_value_changed
                                        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_advance_unit_combo_changed          (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_advance_input_width_spin_value_changed
                                        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_advance_size_lock_toggle_toggled    (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_advance_output_resolution_combo_changed
                                        (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_advance_output_width_spin_value_changed
                                        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_advance_output_height_spin_value_changed
                                        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_advance_scale_spin_value_changed    (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_advance_output_size_combo_changed   (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_advance_unsharp_combo_changed       (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_advance_descreen_combo_changed      (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_advance_scan_button_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_advance_preview_button_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_advance_BC_button_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_advance_HG_button_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_advance_TC_button_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_advance_FR_button_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_advance_MC_button_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_advance_reset_button_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_preference_window_destroy           (GtkObject       *object,
                                        gpointer         user_data);

gboolean
on_preference_window_delete_event      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_preference_reset_button_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_preference_ok_button_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_preference_cancel_button_clicked    (GtkButton       *button,
                                        gpointer         user_data);

void
on_preference_scanner_quietmode_check_toggled(
										GtkToggleButton *togglebutton,
                                        gpointer         user_data);

gboolean
on_select_model_dialog_delete_event    (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_select_model_ok_button_clicked      (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_save_main_dialog_delete_event       (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_save_main_save_button_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_save_main_cancel_button_clicked     (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_save_overwrite_dialog_delete_event  (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_save_overwrite_ok_button_clicked    (GtkButton       *button,
                                        gpointer         user_data);

void
on_save_overwrite_cancel_button_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_save_error_dialog_delete_event      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_save_error_ok_button_clicked        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dialog_bright_contrast_delete_event (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_bc_combobox_changed                 (GtkComboBox     *combobox,
                                        gpointer         user_data);

gboolean
on_bc_drawingarea_configure_event      (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_bc_drawingarea_expose_event         (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_bc_spinbutton_bright_value_changed  (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_bc_scale_bright_value_changed       (GtkRange        *range,
                                        gpointer         user_data);

void
on_bc_spinbutton_contrast_value_changed
                                        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_bc_scale_contrast_value_changed     (GtkRange        *range,
                                        gpointer         user_data);

void
on_bc_button_reset_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_bc_button_close_clicked             (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_bc_scale_bright_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_bc_scale_bright_button_release_event(GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_bc_scale_contrast_button_press_event(GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_bc_scale_contrast_button_release_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_dialog_histogram_delete_event       (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_hg_combobox_changed                 (GtkComboBox     *combobox,
                                        gpointer         user_data);

gboolean
on_hg_drawingarea_configure_event      (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_hg_drawingarea_expose_event         (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
on_hg_drawingarea_scalebar_configure_event
                                        (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_hg_drawingarea_scalebar_expose_event
                                        (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
on_hg_drawingarea_scalebar_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_hg_drawingarea_scalebar_button_release_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_hg_drawingarea_scalebar_motion_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

void
on_hg_togglebutton_shadow_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_hg_spinbutton_shadow_value_changed  (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_hg_togglebutton_mid_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_hg_spinbutton_mid_value_changed     (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_hg_togglebutton_highlight_toggled   (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_hg_spinbutton_highlight_value_changed
                                        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_hg_togglebutton_gbalance_toggled    (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_hg_button_reset_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_hg_button_close_clicked             (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dialog_tonecurve_delete_event       (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_tc_combobox_changed                 (GtkComboBox     *combobox,
                                        gpointer         user_data);

gboolean
on_tc_drawingarea_configure_event      (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_tc_drawingarea_expose_event         (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_tc_combobox_tc_select_changed       (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_tc_button_reset_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_tc_button_close_clicked             (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dialog_final_review_delete_event    (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_fr_combobox_changed                 (GtkComboBox     *combobox,
                                        gpointer         user_data);

gboolean
on_fr_drawingarea_configure_event      (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_fr_drawingarea_expose_event         (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_fr_button_close_clicked             (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dialog_monochrome_delete_event      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_mc_drawingarea_configure_event      (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_mc_drawingarea_expose_event         (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_mc_spinbutton_value_changed         (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_mc_scale_value_changed              (GtkRange        *range,
                                        gpointer         user_data);

void
on_mc_button_reset_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_mc_button_close_clicked             (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dialog_calibration_delete_event     (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_error_dialog_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_error_dialog_ok_button_clicked      (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dialog_about_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_dialog_about_ok_button_clicked      (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dialog_clear_preview_delete_event   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_dialog_clear_preview_ok_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_dialog_clear_preview_cancel_clicked (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_progress_bar_dialog_delete_event    (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_progress_bar_cancel_button_clicked  (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dialog_child_with_checkBox_delete_event
                                        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_dialog_child_with_checkBox_button_ok_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_dialog_child_with_checkBox_button_cancel_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dialog_child_with_ok_delete_event   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_dialog_child_with_ok_button_ok_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dialog_child_with_okcancel_delete_event
                                        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_dialog_child_with_okcancel_button_ok_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_dialog_child_with_okcancel_button_cancel_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_mc_scale_button_press_event         (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_mc_scale_button_release_event       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_select_model_search_button_clicked  (GtkButton       *button,
                                        gpointer         user_data);

void
on_select_model_cancel_button_clicked  (GtkButton       *button,
                                        gpointer         user_data);


void
on_preference_scanner_quietsettings_button_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_button_ok_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_button_cancel_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_button_defaults_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_radiobutton_off_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_radiobutton_on_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_radiobutton_timer_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dialog_quiet_settings_delete_event  (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_spin_start_h_value_changed
                                        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_spin_end_h_value_changed
                                        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_spin_start_m_value_changed
                                        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_spin_end_m_value_changed
                                        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

gboolean
on_dialog_get_settings_delete_event    (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_dialog_get_settings_hide            (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_advance_input_width_spin_state_changed
                                        (GtkWidget       *widget,
                                        GtkStateType     state,
                                        gpointer         user_data);

void
on_advance_input_height_spin_state_changed
                                        (GtkWidget       *widget,
                                        GtkStateType     state,
                                        gpointer         user_data);

void
on_advance_output_width_spin_state_changed
                                        (GtkWidget       *widget,
                                        GtkStateType     state,
                                        gpointer         user_data);

void
on_advance_output_height_spin_state_changed
                                        (GtkWidget       *widget,
                                        GtkStateType     state,
                                        gpointer         user_data);

void
on_advance_scale_spin_state_changed    (GtkWidget       *widget,
                                        GtkStateType     state,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_spin_start_h_state_changed
                                        (GtkWidget       *widget,
                                        GtkStateType     state,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_spin_start_m_state_changed
                                        (GtkWidget       *widget,
                                        GtkStateType     state,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_spin_end_h_state_changed
                                        (GtkWidget       *widget,
                                        GtkStateType     state,
                                        gpointer         user_data);

void
on_dialog_quiet_settings_spin_end_m_state_changed
                                        (GtkWidget       *widget,
                                        GtkStateType     state,
                                        gpointer         user_data);


void
on_preference_scanner_autopowersettings_button_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);


void
on_dialog_autopower_settings_on_combobox_changed
                                        (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_dialog_autopower_settings_off_combobox_changed
                                        (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_dialog_autopower_settings_button_ok_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_dialog_autopower_settings_button_cancel_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dialog_autopower_settings_delete_event
                                        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);
