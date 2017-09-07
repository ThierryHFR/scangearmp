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

#ifndef	_PREV_SUB_H_
#define	_PREV_SUB_H_

#include "cnmstype.h"

#define	PREVSUB_RECT_AREA_FREE		(0)

enum{
	PREVSUB_STATUS_CROP_NONE = 0,
	PREVSUB_STATUS_CROP_START,
	PREVSUB_STATUS_CROP_WRITE,
	PREVSUB_STATUS_CROP_END,
	PREVSUB_STATUS_CROP_MOVE,
	PREVSUB_STATUS_CROP_RESIZE,
	PREVSUB_STATUS_MAX,
};

enum{
	PREVSUB_CURSOR_ON_LINE_LEFT_TOP = 0,
	PREVSUB_CURSOR_INSIDE,
	PREVSUB_CURSOR_ON_LINE_RIGHT_BOTTOM,
	PREVSUB_CURSOR_OUTSIDE,				/* outside rect area */
	PREVSUB_CURSOR_OUTSIDE_PLATEN,		/* outside platen */
	PREVSUB_CURSOR_MAX,
};

enum{
	PREVSUB_TOTAL_CURSOR_INSIDE = 0,
	PREVSUB_TOTAL_CURSOR_ONLINE,
	PREVSUB_TOTAL_CURSOR_OUTSIDE,
	PREVSUB_TOTAL_CURSOR_OUTSIDE_PLATEN,
	PREVSUB_TOTAL_CURSOR_MAX,
};

typedef struct{
	CNMSInt32			status;
	/* min/max size ( PrevSub_Init set this param ) */
	CNMSInt32			minSize[ CNMS_DIM_MAX ];
	CNMSInt32			maxSize[ CNMS_DIM_MAX ];
	/* select rectangle area */
	CNMSInt32			topPos[ CNMS_DIM_MAX ];
	CNMSInt32			bottomPos[ CNMS_DIM_MAX ];
	/* cursor icon */
	CNMSInt32			clickId[ CNMS_DIM_MAX ];
	CNMSInt32			clickPos[ CNMS_DIM_MAX ];
	/* fixed size */
	CNMSInt32			fixedSize[ CNMS_DIM_MAX ];	/* fixed : real input size, free : PREVIEW_RECT_AREA_FREE */
}PREVSUBCOMP, *LPPREVSUBCOMP;

CNMSInt32 PrevSub_Open( LPPREVSUBCOMP lpSub );
CNMSVoid  PrevSub_Close( LPPREVSUBCOMP lpSub );
CNMSInt32 PrevSub_Init( CNMSInt32 *lpMinSize, CNMSInt32 *lpMaxSize, LPPREVSUBCOMP lpSub );
CNMSInt32 PrevSub_Reset( LPPREVSUBCOMP lpSub );

CNMSInt32 PrevSub_GetStatus( LPPREVSUBCOMP lpSub );

CNMSInt32 PrevSub_RectSet( CNMSInt32 *lpOffset, CNMSInt32 *lpSize, LPPREVSUBCOMP lpSub );
CNMSInt32 PrevSub_RectGet( CNMSInt32 *lpSize, LPPREVSUBCOMP lpSub );
CNMSInt32 PrevSub_SetFixedSize( CNMSInt32 *lpSize, LPPREVSUBCOMP lpSub );

CNMSInt32 PrevSub_ClickLeft( CNMSBool hgFlag, CNMSBool prevFlag, CNMSInt32 scale, CNMSInt32 *lpCurPos, LPPREVSUBCOMP lpSub );
CNMSInt32 PrevSub_ClickRight( LPPREVSUBCOMP lpSub );
CNMSInt32 PrevSub_ReleaseLeft( CNMSBool hgFlag, CNMSBool prevFlag, CNMSInt32 *lpCurPos, LPPREVSUBCOMP lpSub );
CNMSInt32 PrevSub_Motion( CNMSInt32 *lpCurPos, LPPREVSUBCOMP lpSub );

CNMSInt32 PrevSub_GetMinMaxSize( CNMSInt32 *lpMinSize, CNMSInt32 *lpMaxSize, LPPREVSUBCOMP lpSub );
CNMSInt32 PrevSub_GetCursorIndex( CNMSBool hgFlag, CNMSBool prevFlag, CNMSInt32 scale, CNMSInt32 *lpCurPos, LPPREVSUBCOMP lpSub );
CNMSInt32 PrevSub_GetCursorRectId( CNMSBool hgFlag, CNMSBool prevFlag, CNMSInt32 scale, CNMSInt32 *lpCurPos, LPPREVSUBCOMP lpSub );

CNMSVoid PrevSub_ChageAspect( CNMSInt32 aspect, LPPREVSUBCOMP lpSub );

#endif	/* _PREV_SUB_H_ */
