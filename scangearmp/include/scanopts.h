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


#ifndef _SCANOPTS_H_
#define _SCANOPTS_H_

#include <sane/sane.h>

#include "cnmstype.h"
#include "cnmsability.h"

typedef struct {
	SANE_Int	num;

	SANE_Int	preview;
	SANE_Int	scanMode;
	SANE_Int	resBind;
	SANE_Int	resX;
	SANE_Int	resY;
	SANE_Int	mm_tl_X;
	SANE_Int	mm_tl_Y;
	SANE_Int	mm_br_X;
	SANE_Int	mm_br_Y;
	SANE_Int	getStatus;
	SANE_Int	getCalStatus;
	SANE_Int	getAbilityVer;
	SANE_Int	getAbility;
	SANE_Int	useAreaPixel;
	SANE_Int	pixel_tl_X;
	SANE_Int	pixel_tl_Y;
	SANE_Int	pixel_br_X;
	SANE_Int	pixel_br_Y;
	SANE_Int	setGamma;
	/* Ver.1.20 */
	SANE_Int	setEveryCalibration;
	SANE_Int	setSilent;
	/* Ver.1.30 */
	SANE_Int	scanMethod;
	/* Ver.1.90 */
	SANE_Int	deviceSettings;
} CNMSOPTSNUM, *LPCNMSOPTSNUM;

CNMSInt32 CnmsSetOptsNum( SANE_Handle h, LPCNMSOPTSNUM lpOpts );

#endif	/* _SCANOPTS_H_ */
