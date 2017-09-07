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

#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include "cnmstype.h"

#define	ProgressValue	80		/* Separated Value Scanning from ImageProcessing */

enum{
	PROGRESSBAR_ID_SCAN = 0,
	PROGRESSBAR_ID_SAVE,
	PROGRESSBAR_ID_TRANSFER_GIMP,
	PROGRESSBAR_ID_SEARCH,
	PROGRESSBAR_ID_MAX,
};

CNMSInt32 ProgressBarOpen( CNMSVoid );
CNMSVoid  ProgressBarClose( CNMSVoid );

CNMSInt32 ProgressBarStart( CNMSInt32 id, CNMSInt32 page );
CNMSVoid  ProgressBarEnd( CNMSVoid );
CNMSInt32 ProgressBarUpdate( CNMSInt32 percentVal, CNMSInt32 page );

CNMSVoid  ProgressBarShow( CNMSVoid );
CNMSVoid  ProgressBarHide( CNMSVoid );

CNMSInt32 ProgressBarGetId( CNMSVoid );

CNMSVoid  ProgressBarCanceled( CNMSVoid );
CNMSVoid  ProgressBarDeleted( CNMSVoid );

int ProgressBarUpdatePulse( void );
CNMSVoid ProgressBarWaitFinish( CNMSVoid );
CNMSBool ProgressBarGetCanceledStatus( CNMSVoid );

#endif	/* _PROGRESS_BAR_H_ */
