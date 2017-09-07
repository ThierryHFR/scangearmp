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

#ifndef	_CNMSSTR_H_
#define	_CNMSSTR_H_

#include "cnmstype.h"

CNMSLPSTR CnmsChgValToStr( CNMSInt32 value, CNMSInt32 object );
CNMSInt32 CnmsChgStrToVal( CNMSLPSTR str, CNMSInt32 object );

#ifdef	__CNMS_DEBUG__
#include "cnmsui.h"
CNMSVoid CnmsDbgUiInfo( CNMSBool activeKey, LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum );
#endif	/* __CNMS_DEBUG__ */

#endif	/* _CNMSSTR_H_ */
