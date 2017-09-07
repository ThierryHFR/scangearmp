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


#ifndef	_COLORADJUSTSTR_H_
#define	_COLORADJUSTSTR_H_

#include "cnmsstrings.h"

/* RGBK label strings */
static	gchar	*ca_label_red	= "R :",
				*ca_label_green	= "G :",
				*ca_label_blue	= "B :",
				*ca_label_gray	= "L :",
				*ca_label_null	= "\0";

/* Channel strings */
static 	gchar 	*ca_ch_litem_master	= STR_CH_LIST_ITEM_MASTER,
				*ca_ch_litem_red	= STR_CH_LIST_ITEM_RED,
				*ca_ch_litem_green	= STR_CH_LIST_ITEM_GREEN,
				*ca_ch_litem_blue	= STR_CH_LIST_ITEM_BLUE,
				*ca_ch_litem_gray	= STR_CH_LIST_ITEM_GRAYSCALE;

/* Tone Curve strings */
static	gchar	*tc_litem_no_adjust	= STR_TC_LIST_ITEM_NO_AJUST,
				*tc_litem_exp_over	= STR_TC_LIST_ITEM_EXP_OVER,
				*tc_litem_exp_under	= STR_TC_LIST_ITEM_EXP_UNDER,
				*tc_litem_high_cont	= STR_TC_LIST_ITEM_HIGH_CONT;

#endif /* _COLORADJUSTSTR_H_ */

