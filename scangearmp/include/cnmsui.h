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

#ifndef	_CNMSUI_H_
#define	_CNMSUI_H_

#include "cnmstype.h"
#include "cnmsdef.h"

/* for menu link */
typedef struct{
	CNMSInt32		object;
	CNMSInt32		value;
	CNMSInt32		mode;
}CNMSUILINKCOMP, *LPCNMSUILINKCOMP;

CNMSInt32 CnmsUiLinkOpen( CNMSLPSTR lpDevName, CNMSLPSTR lpLibPath );
CNMSVoid  CnmsUiLinkClose( CNMSVoid );

CNMSInt32 CnmsUiLinkGetSize( CNMSVoid );
CNMSInt32 CnmsUiLinkDef( LPCNMSUILINKCOMP lpLink );
CNMSInt32 CnmsUiLinkMode( LPCNMSUILINKCOMP lpLink );

CNMSInt32 CnmsGetFormatType( CNMSLPSTR lpDevName, CNMSLPSTR lpLibPath );

/*** Get Info ***/
#define	CNMSUI_SIZE_GROUP_FREE		(0)
#define	CNMSUI_SIZE_GROUP_PRINT		(1)
#define	CNMSUI_SIZE_GROUP_DISPLAY	(2)

typedef struct{
	CNMSInt32	group;
	CNMSInt32	unit;
	CNMSDec32	size[ CNMS_DIM_MAX ];
}CNMSUISIZECOMP, *LPCNMSUISIZECOMP;

CNMSInt32 CnmsUiSetChange( CNMSInt32 object, CNMSInt32 value, LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum );
CNMSInt32 CnmsUiSetValue( CNMSInt32 object, CNMSInt32 value, LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum );
CNMSInt32 CnmsUiGetValue( CNMSInt32 object, LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum );
CNMSInt32 CnmsUiSetRealValue( CNMSInt32 object, CNMSInt32 conv_value, LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum );
CNMSInt32 CnmsUiGetRealValue( CNMSInt32 object, LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum );

CNMSInt32 CnmsUiGetRealSize( CNMSInt32 object, LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum, LPCNMSUISIZECOMP lpSize );

CNMSInt32 CnmsUiAspectSet( CNMSInt32 aspect, LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum );
CNMSInt32 CnmsUiAspectGet( LPCNMSUILINKCOMP lpLink, CNMSInt32 linkNum );

#endif	/* _CNMSUI_H_ */
