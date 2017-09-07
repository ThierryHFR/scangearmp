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

#ifndef	_SCANMAIN_H_
#define	_SCANMAIN_H_

#include	"cnmstype.h"
#include	"scanflow.h"
#include 	"cnmsfunc.h"

enum{
	CNMS_SCAN_GETDEVMODELSTR_FULL = 0,
	CNMS_SCAN_GETDEVMODELSTR_MODEL,
	CNMS_SCAN_GETDEVMODELSTR_MAX,
};

CNMSInt32 CnmsScanInit( CNMSBool init );
CNMSVoid  CnmsScanClose( CNMSVoid );
CNMSLPSTR CnmsScanGetDevModelStr( CNMSInt32 type, CNMSInt32 devIndex );
CNMSInt32 CnmsScanOpen( CNMSInt32 devIndex );

CNMSInt32 CnmsScanGetPreviewResolution( CNMSInt32 source );

/* return resolution (0:Error) */
CNMSInt32 CnmsScanGetRealMinSize( CNMSInt32 source, CNMSInt32 *lpMin );
CNMSInt32 CnmsScanGetRealMaxSize( CNMSInt32 source, CNMSInt32 *lpMax );
CNMSInt32 CnmsScanGetRealMinMaxSize( CNMSInt32 source, CNMSInt32 *lpMin, CNMSInt32 *lpMax );

CNMSInt32 CnmsScanGetMinMaxScale( CNMSInt32 res, CNMSInt32 *lpMinMax );

CNMSInt32 CnmsScanChageStatus( CNMSInt32 ActionMode, CNMSFd dstFd, LPCNMS_ROOT root, CNMSUIReturn *lpRtn );
CNMSVoid CnmsScanCancel( CNMSVoid );

CNMSInt32 CnmsGetScanSourceAbility( CNMSInt32 uiMediaType );

CNMSInt32 CnmsGetScanInfoParameter( CNMSScanInfo *info );

CNMSInt32 CnmsGetDeviceSettings( CANON_DEVICE_SETTINGS *devsets );
CNMSInt32 CnmsSetDeviceSettings( CANON_DEVICE_SETTINGS *devsets );

#endif	/* _SCANMAIN_H_ */
