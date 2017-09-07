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

#ifndef _CNMSNETWORK_H_
#define _CNMSNETWORK_H_

#include "cnmstype.h"


CNMSInt32 ProgressBar2Open( CNMSVoid );
CNMSVoid  ProgressBar2Close( CNMSVoid );

CNMSInt32 ProgressBar2Start( CNMSInt32 id, CNMSInt32 page );
CNMSVoid  ProgressBar2End( CNMSVoid );
int ProgressBar2Update( void );		/* callback function -> CNNL_Config */

CNMSVoid  ProgressBar2Show( CNMSVoid );
CNMSVoid  ProgressBar2Hide( CNMSVoid );

CNMSVoid  ProgressBar2Canceled( CNMSVoid );
CNMSVoid  ProgressBar2Deleted( CNMSVoid );

CNMSInt32 CnmsReloadNetworkCache( CNMSVoid );

#endif	/* _CNMSNETWORK_H_ */
