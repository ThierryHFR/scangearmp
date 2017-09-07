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

#ifndef	_PREV_SUB_C_
#define	_PREV_SUB_C_

//#include <gtk/gtk.h>
//#include <gdk/gdkkeysyms.h>

#include "cnmstype.h"
#include "cnmsdef.h"
#include "cnmsfunc.h"

#include "prev_sub.h"
#include "prev_main.h"
#include "w1.h"
#include "cnmsability.h"


#define	__CNMS_DEBUG_DETAIL__


#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
#ifdef	__CNMS_DEBUG_DETAIL__
	#define	__CNMS_DEBUG_PREVIEW_SUB_DETAIL__
#endif
#endif

static CNMSInt32 RectReset( LPPREVSUBCOMP lpSub );
static CNMSVoid SetCurrentPos( CNMSInt32 dim, CNMSInt32 pos, LPPREVSUBCOMP lpSub );

static CNMSVoid MoveRectArea( CNMSInt32 dim, CNMSInt32 pos, LPPREVSUBCOMP lpSub );
static CNMSInt32 ResizeRect( CNMSInt32 *lpCurPos, LPPREVSUBCOMP lpSub );
static CNMSVoid ResizeRectAreaFixed( CNMSInt32 *lpPos, LPPREVSUBCOMP lpSub );

static CNMSDec32 IntFitSizeKeepAspectRatio( CNMSInt32 type, const CNMSInt32 *lpBase, CNMSInt32 *lpCurr );
static CNMSInt32 IntConvAbsolutePos( const CNMSInt32 *lpSrcPos, CNMSInt32 *lpDstPos, CNMSDec32 scale );
static CNMSInt32 IntConvRelativePos( const CNMSInt32 *lpSrcPos, CNMSInt32 *lpDstPos, CNMSDec32 scale );

#ifdef	__CNMS_DEBUG__
CNMSVoid DebugPreviewSubComp( LPPREVSUBCOMP lpSub );
#endif

/***** open / close / reset *****/
CNMSInt32 PrevSub_Open(
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32			ret = CNMS_ERR, ldata, i;

	/* check */
	if( lpSub == CNMSNULL ){
		DBGMSG( "[PrevSub_Open]Parameter is error!\n" );
		goto	EXIT;
	}
	/* clear */
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		lpSub->minSize[ i ] = lpSub->maxSize[ i ] = 0;
	}
	if( ( i = PrevSub_Reset( lpSub ) ) != CNMS_NO_ERR ){
		DBGMSG( "[PrevSub_Open]Error is occured in PrevSub_Reset!\n" );
		goto	EXIT;
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
	DBGMSG( "[PrevSub_Open()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSVoid PrevSub_Close(
		LPPREVSUBCOMP	lpSub )
{
#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
	DBGMSG( "[PrevSub_Close()].\n" );
#endif
	return;
}

CNMSInt32 PrevSub_Init(
		CNMSInt32		*lpMinSize,
		CNMSInt32		*lpMaxSize,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ret = CNMS_ERR, i;

	if( ( lpMinSize == CNMSNULL ) || ( lpMaxSize == CNMSNULL ) || ( lpSub == CNMSNULL ) ){
		DBGMSG( "[PrevSub_Init]Parameter is error.\n" );
		goto	EXIT;
	}
	/* save param */
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		lpSub->minSize[ i ] = lpMinSize[ i ];
		lpSub->maxSize[ i ] = lpMaxSize[ i ];
	}
	if( ( i = PrevSub_Reset( lpSub ) ) != CNMS_NO_ERR ){
		DBGMSG( "[PrevSub_Init]Error is occured in PrevSub_Reset!\n" );
		goto	EXIT;
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
	DBGMSG( "[PrevSub_Init(minSize:%dx%d, maxSize:%dx%d)]=%d.\n", lpMinSize[ CNMS_DIM_H ], lpMinSize[ CNMS_DIM_V ], lpMaxSize[ CNMS_DIM_H ], lpMaxSize[ CNMS_DIM_V ], ret );
#endif
	return	ret;
}

CNMSInt32 PrevSub_Reset(
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ret = CNMS_ERR, i;

	if( lpSub == CNMSNULL ){
		DBGMSG( "[PrevSub_Reset()]Parameter is error.\n" );
		goto	EXIT;
	}
	/* reset param */
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		lpSub->topPos[ i ]  = lpSub->bottomPos[ i ] = lpSub->clickPos[ i ] = 0;
		lpSub->fixedSize[ i ] = PREVSUB_RECT_AREA_FREE;
		lpSub->clickId[ i ] = CNMS_ERR;
	}
	lpSub->status = PREVSUB_STATUS_CROP_NONE;

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
	DBGMSG( "[PrevSub_Reset()]=%d.\n", ret );
#endif
	return	ret;

}

CNMSInt32 PrevSub_GetStatus(
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ret = CNMS_ERR;

	if( lpSub == CNMSNULL ){
		DBGMSG( "[PrevSub_GetStatus]Parameter is error.\n" );
		goto	EXIT;
	}
	else if( ( lpSub->status < 0 ) || ( PREVSUB_STATUS_MAX <= lpSub->status ) ){
		DBGMSG( "[PrevSub_GetStatus]Status is error.\n" );
		goto	EXIT;
	}
	ret = lpSub->status;
EXIT:
	return	ret;
}

CNMSInt32 PrevSub_RectSet(
		CNMSInt32		*lpOffset, 
		CNMSInt32		*lpSize,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ret = CNMS_ERR, i, ldata;

	/* check */
	if( ( lpOffset == CNMSNULL ) || ( lpSize == CNMSNULL ) || ( lpSub == CNMSNULL ) ){
		DBGMSG( "[PrevSub_RectSet]Parameter is error!\n" );
		goto	EXIT;
	}

	if( lpSub->status == PREVSUB_STATUS_CROP_NONE ){
		/* No old rect -> Set start position */
		lpSub->topPos[ CNMS_DIM_H ] = lpSub->topPos[ CNMS_DIM_V ] = 0;
	}
	/* set new address */
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( lpSub->maxSize[ i ] < lpSub->topPos[ i ] + lpSize[ i ] ){
			lpSub->bottomPos[ i ] = lpSub->maxSize[ i ] - 1;	/* clip */
			lpOffset[ i ] = lpSub->topPos[ i ] = lpSub->bottomPos[ i ] - lpSize[ i ];
		}
		else{
			lpSub->bottomPos[ i ] = lpSub->topPos[ i ] + lpSize[ i ];
		}
	}
	lpSub->status = PREVSUB_STATUS_CROP_END;
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
	DBGMSG( "[PrevSub_RectSet(offset:%dx%d, size:%dx%d)]=%d.\n", ret, lpOffset[ CNMS_DIM_H ], lpOffset[ CNMS_DIM_V ], lpSize[ CNMS_DIM_H ], lpSize[ CNMS_DIM_V ] );
#endif
	return	ret;
}

CNMSInt32 PrevSub_RectGet(
		CNMSInt32		*lpSize,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32	ret = CNMS_ERR, i;

	if( ( lpSize == CNMSNULL ) || ( lpSub == CNMSNULL ) ){
		DBGMSG( "[PrevSub_RectGet]Parameter is error!\n" );
		goto	EXIT;
	}
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		lpSize[ i ] = ( lpSub->status >= PREVSUB_STATUS_CROP_WRITE ) ? ( lpSub->bottomPos[ i ] - lpSub->topPos[ i ] + 1 ) : 0;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
	DBGMSG( "[PrevSub_RectGet()]=%d. size:%dx%d\n", ret, lpSize[ CNMS_DIM_H ], lpSize[ CNMS_DIM_V ] );
#endif
	return	ret;
}

CNMSInt32 PrevSub_SetFixedSize(
		CNMSInt32		*lpSize,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ret = CNMS_ERR, i;

	if( lpSub == CNMSNULL ){
		DBGMSG( "[PrevSub_SetFixedSize]Parameter is error!\n" );
		goto	EXIT;
	}

	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( lpSize != CNMSNULL ){
			lpSub->fixedSize[ i ] = lpSize[ i ];
		}
		else{
			lpSub->fixedSize[ i ] = PREVSUB_RECT_AREA_FREE;
		}
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
	DBGMSG( "[PrevSub_SetFixedSize()]=%d.\n", ret );
#endif
	return	ret;
}

/***** Click *****/
CNMSInt32 PrevSub_ClickLeft(
		CNMSBool		hgFlag,
		CNMSBool		prevFlag,
		CNMSInt32		scale,
		CNMSInt32		*lpCurPos,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ret = CNMS_ERR, clickType, ldata;

	if( ( lpCurPos == CNMSNULL )|| ( lpSub == CNMSNULL ) ){
		DBGMSG( "[PrevSub_ClickLeft]Parameter is error!\n" );
		goto	EXIT;
	}
	
	if( ( ldata = PrevSub_GetCursorRectId( hgFlag, prevFlag, scale, lpCurPos, lpSub ) ) != CNMS_NO_ERR ){
		DBGMSG( "[PrevSub_ClickLeft]Error is occured in PrevSub_GetCursorRectId!\n" );
		goto	EXIT;
	}

	if( ( lpSub->clickId[ CNMS_DIM_H ] == PREVSUB_CURSOR_OUTSIDE_PLATEN ) || ( lpSub->clickId[ CNMS_DIM_V ] == PREVSUB_CURSOR_OUTSIDE_PLATEN ) ){
		/* outside platen */
		if( ( ldata = PrevSub_ClickRight( lpSub ) ) != CNMS_NO_ERR ){
			DBGMSG( "[PrevSub_ClickLeft]Error is occured in PrevSub_ClickRight!\n" );
			goto	EXIT;
		}
	}
	else if( ( lpSub->clickId[ CNMS_DIM_H ] == PREVSUB_CURSOR_OUTSIDE ) || ( lpSub->clickId[ CNMS_DIM_V ] == PREVSUB_CURSOR_OUTSIDE ) ){
		/* select */
		if( ( ldata = PrevSub_ClickRight( lpSub ) ) != CNMS_NO_ERR ){
			DBGMSG( "[PrevSub_ClickLeft]Error is occured in PrevSub_ClickRight!\n" );
			goto	EXIT;
		}
		lpSub->topPos[ CNMS_DIM_H ] = lpSub->bottomPos[ CNMS_DIM_H ] = lpSub->clickPos[ CNMS_DIM_H ] = lpCurPos[ CNMS_DIM_H ];
		lpSub->topPos[ CNMS_DIM_V ] = lpSub->bottomPos[ CNMS_DIM_V ] = lpSub->clickPos[ CNMS_DIM_V ] = lpCurPos[ CNMS_DIM_V ];

		lpSub->status = PREVSUB_STATUS_CROP_START;
	}
	else if( ( lpSub->clickId[ CNMS_DIM_H ] == PREVSUB_CURSOR_INSIDE ) && ( lpSub->clickId[ CNMS_DIM_V ] == PREVSUB_CURSOR_INSIDE ) ){
		/* move */
		lpSub->clickPos[ CNMS_DIM_H ] = lpCurPos[ CNMS_DIM_H ];
		lpSub->clickPos[ CNMS_DIM_V ] = lpCurPos[ CNMS_DIM_V ];

		lpSub->status = PREVSUB_STATUS_CROP_MOVE;
	}
	else{
		/* resize */
		lpSub->clickPos[ CNMS_DIM_H ] = ( lpSub->clickId[ CNMS_DIM_H ] == PREVSUB_CURSOR_ON_LINE_LEFT_TOP ) ? lpSub->bottomPos[ CNMS_DIM_H ] : lpSub->topPos[ CNMS_DIM_H ];
		lpSub->clickPos[ CNMS_DIM_V ] = ( lpSub->clickId[ CNMS_DIM_V ] == PREVSUB_CURSOR_ON_LINE_LEFT_TOP ) ? lpSub->bottomPos[ CNMS_DIM_V ] : lpSub->topPos[ CNMS_DIM_V ];

		lpSub->status = PREVSUB_STATUS_CROP_RESIZE;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
	DBGMSG( "[PrevSub_ClickLeft(hg:%d, prev:%d, pos:%dx%d)]=%d.\n", hgFlag, prevFlag, lpCurPos[ CNMS_DIM_H ], lpCurPos[ CNMS_DIM_V ], ret );
#endif
	return	ret;
}

CNMSInt32 PrevSub_ClickRight(
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ret = CNMS_ERR, ldata, i;

	if ( ( ldata = PrevSub_Reset( lpSub ) ) != CNMS_NO_ERR ){
		DBGMSG( "[PrevSub_ClickRight]Error is occured in PrevSub_Reset!\n" );
		goto	EXIT;
	}
	else if( ( ldata = W1_ResetSize() ) != CNMS_NO_ERR ){
		DBGMSG( "[PrevSub_ClickRight]Error is occured in W1_ResetSize!\n" );
		goto	EXIT;
	}
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		lpSub->topPos[ i ] = 0;
		lpSub->bottomPos[ i ] = lpSub->maxSize[ i ] - 1;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
	DBGMSG( "[PrevSub_ClickRight()]=%d.\n", ret );
#endif
	return	ret;
}

/***** Release *****/
CNMSInt32 PrevSub_ReleaseLeft(
		CNMSBool		hgFlag,
		CNMSBool		prevFlag,
		CNMSInt32		*lpCurPos,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ret = CNMS_ERR, lastStatus, ldata, i;

	if( ( lpCurPos == CNMSNULL )|| ( lpSub == CNMSNULL ) ){
		DBGMSG( "[PrevSub_ReleaseLeft]Parameter is error!\n" );
		goto	EXIT;
	}
	lastStatus = lpSub->status;
	
	switch( lastStatus ){
		case	PREVSUB_STATUS_CROP_START:
		case	PREVSUB_STATUS_CROP_WRITE:
			if( ( lpCurPos[ CNMS_DIM_H ] == lpSub->clickPos[ CNMS_DIM_H ] ) && ( lpCurPos[ CNMS_DIM_V ] == lpSub->clickPos[ CNMS_DIM_V ] ) ){
				/* left button press -> don't move cursor -> left button release */
				if ( ( ldata = PrevSub_ClickRight( lpSub ) ) != CNMS_NO_ERR ){
					DBGMSG( "[PrevSub_ReleaseLeft]Error is occured in PrevSub_ClickRight!\n" );
					goto	EXIT;
				}
			}
			else{
				/* set rect */
				for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
					SetCurrentPos( i, lpCurPos[ i ], lpSub );
				}
				lpSub->status = PREVSUB_STATUS_CROP_END;
			}
			break;
			
		case	PREVSUB_STATUS_CROP_MOVE:
		case	PREVSUB_STATUS_CROP_RESIZE:
			lpSub->status = PREVSUB_STATUS_CROP_END;
			break;
	}

	/* clear clicked address */
	lpSub->clickPos[ CNMS_DIM_H ] = lpSub->clickPos[ CNMS_DIM_V ] = 0;

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
	DBGMSG( "[PrevSub_ReleaseLeft(hg:%d, prev:%d, pos:%dx%d)]=%d.\n", hgFlag, prevFlag, lpCurPos[ CNMS_DIM_H ], lpCurPos[ CNMS_DIM_V ], ret );
#endif
	return	ret;
}

/* select */
static CNMSVoid SetCurrentPos(
		CNMSInt32		dim,
		CNMSInt32		pos,
		LPPREVSUBCOMP	lpSub )
{
	if( lpSub->clickPos[ dim ] < pos ){
		/* move forward plus */
		if( pos < lpSub->clickPos[ dim ] + lpSub->minSize[ dim ] ){
			/* under min size */
			lpSub->bottomPos[ dim ] = ( lpSub->maxSize[ dim ] < lpSub->clickPos[ dim ] + lpSub->minSize[ dim ] ) ? lpSub->maxSize[ dim ] : ( lpSub->clickPos[ dim ] + lpSub->minSize[ dim ] );
			lpSub->topPos[ dim ] = lpSub->bottomPos[ dim ] - lpSub->minSize[ dim ];
		}
		else{
			lpSub->topPos[ dim ] = lpSub->clickPos[ dim ];
			lpSub->bottomPos[ dim ] = ( lpSub->maxSize[ dim ] <= pos ) ? ( lpSub->maxSize[ dim ] - 1 ) : pos;
		}
	}
	else{
		/* move forward minus */
		if( lpSub->clickPos[ dim ] < pos + lpSub->minSize[ dim ] ){
			/* under min size */
			lpSub->topPos[ dim ] = ( lpSub->clickPos[ dim ] < lpSub->minSize[ dim ] ) ? 0 : ( lpSub->clickPos[ dim ] - lpSub->minSize[ dim ] );
			lpSub->bottomPos[ dim ] = lpSub->topPos[ dim ] + lpSub->minSize[ dim ];
		}
		else{
			lpSub->topPos[ dim ] = pos;
			lpSub->bottomPos[ dim ] = lpSub->clickPos[ dim ];
		}
	}
#ifdef	__CNMS_DEBUG_PREVIEW_SUB_DETAIL__
	DBGMSG( "[SetCurrentPos(dim:%d, pos:%d)]top:%d, bottom:%d, max:%d.\n", dim, pos, lpSub->topPos[ dim ], lpSub->bottomPos[ dim ], lpSub->maxSize[ dim ] );
#endif

	return;
}

/***** Motion *****/
CNMSInt32 PrevSub_Motion(
		CNMSInt32		*lpCurPos,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ret = CNMS_ERR, i;

	if( ( lpCurPos == CNMSNULL )|| ( lpSub == CNMSNULL ) ){
		DBGMSG( "[PrevSub_Motion]Parameter is error!\n" );
		goto	EXIT;
	}

	/* select motion */
	switch( lpSub->status ){
		case	PREVSUB_STATUS_CROP_START:
		case	PREVSUB_STATUS_CROP_WRITE:
			/* select */
			for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
				SetCurrentPos( i, lpCurPos[ i ], lpSub );
			}
			lpSub->status = PREVSUB_STATUS_CROP_WRITE;
			break;

		case	PREVSUB_STATUS_CROP_MOVE:
			for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
				MoveRectArea( i, lpCurPos[ i ], lpSub );
			}
			break;

		case	PREVSUB_STATUS_CROP_RESIZE:
			if( ( i = ResizeRect( lpCurPos, lpSub ) ) != CNMS_NO_ERR ){
				DBGMSG( "[PrevSub_Motion]Error is occured in ResizeRect!\n" );
				goto	EXIT;
			}
			break;
		
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
	DBGMSG( "[PrevSub_Motion(pos:%dx%d)]=%d, status:%d.\n", lpCurPos[ CNMS_DIM_H ], lpCurPos[ CNMS_DIM_V ], ret, lpSub->status );
#endif
	return	ret;
}

static CNMSVoid MoveRectArea(
		CNMSInt32		dim,
		CNMSInt32		pos,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32	moveDist;

	if( lpSub->clickPos[ dim ] < pos ){
		/* Plus */
		moveDist = pos - lpSub->clickPos[ dim ];
		if( lpSub->maxSize[ dim ] <= lpSub->bottomPos[ dim ] + moveDist ){
			/* over */
			lpSub->topPos[ dim ]   += ( lpSub->maxSize[ dim ] - lpSub->bottomPos[ dim ] - 1 );
			lpSub->bottomPos[ dim ] = lpSub->maxSize[ dim ] - 1;
		}
		else{
			lpSub->topPos[ dim ]    += moveDist;
			lpSub->bottomPos[ dim ] += moveDist;
			lpSub->clickPos[ dim ]  = pos;
		}
	}
	else{
		/* Minus */
		moveDist = lpSub->clickPos[ dim ] - pos;
		if( lpSub->topPos[ dim ] < moveDist ){
			/* under */
			lpSub->bottomPos[ dim ] -= lpSub->topPos[ dim ];
			lpSub->topPos[ dim ]     = 0;
		}
		else{
			lpSub->topPos[ dim ]    -= moveDist;
			lpSub->bottomPos[ dim ] -= moveDist;
			lpSub->clickPos[ dim ]   = pos;
		}
	}
#ifdef	__CNMS_DEBUG_PREVIEW_SUB_DETAIL__
	DBGMSG( "[MoveRectArea(dim:%d, pos:%d)].\n", dim, pos );
#endif
	return;
}

static CNMSInt32 ResizeRect(
		CNMSInt32		*lpCurPos,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ret = CNMS_ERR, i, ldata;
	CNMSInt32		curPos[ CNMS_DIM_MAX ], tmpPos[ CNMS_DIM_MAX ];

	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		curPos[ i ] = ( lpSub->maxSize[ i ] <= lpCurPos[ i ] ) ? ( lpSub->maxSize[ i ] - 1 ) : lpCurPos[ i ];
	}
	/* Resize Rect */
	if( lpSub->fixedSize[ CNMS_DIM_H ] == PREVSUB_RECT_AREA_FREE ){	/* free size */
		for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
			if( ( lpSub->clickId[ i ] == PREVSUB_CURSOR_ON_LINE_LEFT_TOP ) || ( lpSub->clickId[ i ] == PREVSUB_CURSOR_ON_LINE_RIGHT_BOTTOM ) ){
				SetCurrentPos( i, curPos[ i ], lpSub );
			}
		}
	}
	else{
		ResizeRectAreaFixed( curPos, lpSub );
	}
	lpSub->status = PREVSUB_STATUS_CROP_RESIZE;

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB_DETAIL__
	DBGMSG( "[ResizeRect(pos:%dx%d)]=%d.\n", lpCurPos[ CNMS_DIM_H ], lpCurPos[ CNMS_DIM_V ], ret );
#endif
	return	ret;
}

static CNMSVoid ResizeRectAreaFixed(
		CNMSInt32		*lpPos,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ldata, i;
	CNMSInt32		tmpSize[ CNMS_DIM_MAX ], newSize[ CNMS_DIM_MAX ], minSize[ CNMS_DIM_MAX ], maxSize[ CNMS_DIM_MAX ];

	/* set size */
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( lpSub->clickId[ i ] == PREVSUB_CURSOR_INSIDE ){
			tmpSize[ i ] = 1;
			maxSize[ i ] = lpSub->maxSize[ i ] - lpSub->clickPos[ i ];
		}
		else{
			if( lpSub->clickPos[ i ] < lpPos[ i ] ){
				tmpSize[ i ] = lpPos[ i ] - lpSub->clickPos[ i ] + 1;
				maxSize[ i ] = lpSub->maxSize[ i ] - lpSub->clickPos[ i ];
			}
			else{
				tmpSize[ i ] = lpSub->clickPos[ i ] - lpPos[ i ] + 1;
				maxSize[ i ] = lpSub->clickPos[ i ] + 1;
			}
		}
	}

	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		newSize[ i ] = minSize[ i ] = lpSub->fixedSize[ i ];
	}
	CnmsFitSizeKeepAspectRatio( CNMS_FIT_OUTSIDE, lpSub->minSize, minSize );
	CnmsFitSizeKeepAspectRatio( CNMS_FIT_OUTSIDE, tmpSize, newSize );

	/* clip Under minSize and Over maxSize */
	ldata = 0;
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( ( maxSize[ i ] < minSize[ i ] ) || ( newSize[ i ] < minSize[ i ] ) ){
			ldata = -1;
			break;
		}
		else if( maxSize[ i ] <= newSize[ i ] ){
			ldata = 1;
			break;
		}
	}
	/* clip minSize */
	if( ldata == -1 ){
		for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
			newSize[ i ] = minSize[ i ];
		}
	}
	/* clip maxSize */
	else if( ldata == 1 ){
		for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
			newSize[ i ] = lpSub->fixedSize[ i ];
		}
		CnmsFitSizeKeepAspectRatio( CNMS_FIT_INSIDE, maxSize, newSize );
	}

	/* set new position */
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( lpSub->clickId[ i ] == PREVSUB_CURSOR_INSIDE ){
			if( lpSub->maxSize[ i ] <= lpSub->clickPos[ i ] + newSize[ i ] ){
				lpSub->bottomPos[ i ] = lpSub->maxSize[ i ] - 1;
				lpSub->topPos[ i ] = lpSub->bottomPos[ i ] - newSize[ i ];
			}
			else{
				lpSub->topPos[ i ] = lpSub->clickPos[ i ];
				lpSub->bottomPos[ i ] = lpSub->topPos[ i ] + newSize[ i ];
			}
		}
		else{
			if( lpSub->clickPos[ i ] <= lpPos[ i ] ){
				if( lpSub->maxSize[ i ] <= lpSub->clickPos[ i ] + newSize[ i ] ){
					lpSub->bottomPos[ i ] = lpSub->maxSize[ i ] - 1;
					lpSub->topPos[ i ] = lpSub->bottomPos[ i ] - newSize[ i ];
				}
				else{
					lpSub->topPos[ i ] = lpSub->clickPos[ i ];
					lpSub->bottomPos[ i ] = lpSub->topPos[ i ] + newSize[ i ];
				}
			}
			else{
				if( lpSub->clickPos[ i ] < newSize[ i ] - 1 ){
					lpSub->topPos[ i ] = 0;
					lpSub->bottomPos[ i ] = newSize[ i ] - 1;
				}
				else{
					lpSub->bottomPos[ i ] = lpSub->clickPos[ i ];
					lpSub->topPos[ i ] = lpSub->bottomPos[ i ] + 1 - newSize[ i ];
				}
			}
		}
	}

	return;
}

CNMSInt32 PrevSub_GetMinMaxSize(
		CNMSInt32		*lpMinSize,
		CNMSInt32		*lpMaxSize,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ret = CNMS_ERR, i, ldata, decFig, tmpSize[ CNMS_DIM_MAX ];
	CNMSBool		fixedKey;

	/* check */
	if( lpSub == CNMSNULL ){
		DBGMSG( "[PrevSub_GetMinMaxSize]Status is error!\n" );
		goto	EXIT;
	}
	else if( ( lpMinSize == CNMSNULL ) || ( lpMaxSize == CNMSNULL ) ){
		DBGMSG( "[PrevSub_GetMinMaxSize]Parameter is error!\n" );
		goto	EXIT;
	}
	fixedKey = ( lpSub->fixedSize[ CNMS_DIM_H ] == PREVSUB_RECT_AREA_FREE ) ? CNMS_FALSE : CNMS_TRUE;
	/* set min size */
	if( fixedKey == CNMS_TRUE ){	/* fixed size */
		for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
			lpMinSize[ i ] = lpSub->fixedSize[ i ];
		}
		CnmsFitSizeKeepAspectRatio( CNMS_FIT_OUTSIDE, lpSub->minSize, lpMinSize );
	}
	else{	/* free size */
		for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
			lpMinSize[ i ] = lpSub->minSize[ i ];
		}
	}
	/* set max size */
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){	/* max area */
		tmpSize[ i ] = lpSub->maxSize[ i ] - lpSub->topPos[ i ];
	}
	if( fixedKey == CNMS_TRUE ){	/* fixed size */
		for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
			lpMaxSize[ i ] = lpSub->fixedSize[ i ];
		}
		CnmsFitSizeKeepAspectRatio( CNMS_FIT_INSIDE, tmpSize, lpMaxSize );
	}
	else{	/* free size */
		for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
			lpMaxSize[ i ] = tmpSize[ i ];
		}
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
	DBGMSG( "[PrevSub_GetMinMaxSize()]=%d, min:%dx%d, max:%dx%d.\n", ret, lpMinSize[ CNMS_DIM_H ], lpMinSize[ CNMS_DIM_V ], lpMaxSize[ CNMS_DIM_H ], lpMaxSize[ CNMS_DIM_V ] );
#endif
	return	ret;
}

CNMSInt32 PrevSub_GetCursorIndex(
		CNMSBool		hgFlag,
		CNMSBool		prevFlag,
		CNMSInt32		scale,
		CNMSInt32		*lpCurPos,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32	ret = CNMS_ERR, ldata;
	CNMSInt32	cursorIndexHg[ PREVSUB_CURSOR_MAX ][ PREVSUB_CURSOR_MAX ] =
		/* Horizontal Pos */
		/* :on Left line,			between L,R lines,		on Right line,			outside rect,			outside preview 			Vertical Position*/
		{ { CNMS_CURSOR_ID_PEN,		CNMS_CURSOR_ID_PEN,		CNMS_CURSOR_ID_PEN,		CNMS_CURSOR_ID_CROSS,	CNMS_CURSOR_ID_CROSS },		/* on top line */
		  { CNMS_CURSOR_ID_PEN,		CNMS_CURSOR_ID_PEN,		CNMS_CURSOR_ID_PEN,		CNMS_CURSOR_ID_CROSS,	CNMS_CURSOR_ID_CROSS },		/* between T,B lines */
		  { CNMS_CURSOR_ID_PEN,		CNMS_CURSOR_ID_PEN,		CNMS_CURSOR_ID_PEN,		CNMS_CURSOR_ID_CROSS,	CNMS_CURSOR_ID_CROSS },		/* on bottom line */
		  { CNMS_CURSOR_ID_CROSS,	CNMS_CURSOR_ID_CROSS,	CNMS_CURSOR_ID_CROSS,	CNMS_CURSOR_ID_CROSS,	CNMS_CURSOR_ID_CROSS },		/* outside rect */
		  { CNMS_CURSOR_ID_CROSS,	CNMS_CURSOR_ID_CROSS,	CNMS_CURSOR_ID_CROSS,	CNMS_CURSOR_ID_CROSS,	CNMS_CURSOR_ID_CROSS }		/* outside preview */
		};

	CNMSInt32	cursorIndexNormal[ PREVSUB_CURSOR_MAX ][ PREVSUB_CURSOR_MAX ] =
		/* Horizontal Pos */
		/* :on Left line,			between L,R lines,		on Right line,			outside rect,			outside preview 			Vertical Position*/
		{ { CNMS_CURSOR_ID_LT_RB,	CNMS_CURSOR_ID_T_B,		CNMS_CURSOR_ID_RT_LB,	CNMS_CURSOR_ID_OUTSIDE,	CNMS_CURSOR_ID_NORMAL },	/* on top line */
		  { CNMS_CURSOR_ID_L_R,		CNMS_CURSOR_ID_INSIDE,	CNMS_CURSOR_ID_L_R,		CNMS_CURSOR_ID_OUTSIDE,	CNMS_CURSOR_ID_NORMAL },	/* between T,B lines */
		  { CNMS_CURSOR_ID_RT_LB,	CNMS_CURSOR_ID_T_B,		CNMS_CURSOR_ID_LT_RB,	CNMS_CURSOR_ID_OUTSIDE,	CNMS_CURSOR_ID_NORMAL },	/* on bottom line */
		  { CNMS_CURSOR_ID_OUTSIDE,	CNMS_CURSOR_ID_OUTSIDE,	CNMS_CURSOR_ID_OUTSIDE,	CNMS_CURSOR_ID_OUTSIDE,	CNMS_CURSOR_ID_NORMAL },	/* outside rect */
		  { CNMS_CURSOR_ID_NORMAL,	CNMS_CURSOR_ID_NORMAL,	CNMS_CURSOR_ID_NORMAL,	CNMS_CURSOR_ID_NORMAL,	CNMS_CURSOR_ID_NORMAL }		/* outside preview */
		};

	if( ( ldata = PrevSub_GetCursorRectId( hgFlag, prevFlag, scale, lpCurPos, lpSub ) ) != CNMS_NO_ERR ){
		goto	EXIT;
	}
	if( hgFlag == CNMS_TRUE ){
		ret = cursorIndexHg[ lpSub->clickId[ CNMS_DIM_V ] ][ lpSub->clickId[ CNMS_DIM_H ] ];
	}
	else{
		ret = cursorIndexNormal[ lpSub->clickId[ CNMS_DIM_V ] ][ lpSub->clickId[ CNMS_DIM_H ] ];
	}
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB_DETAIL__
	DBGMSG( "[PrevSub_GetCursorIndex(hg:%d, prev:%d, pos:%dx%d)]=%d.\n", hgFlag, prevFlag, lpCurPos[ CNMS_DIM_H ], lpCurPos[ CNMS_DIM_V ], ret );
#endif
	return	ret;
}

CNMSInt32 PrevSub_GetCursorRectId(
		CNMSBool		hgFlag,
		CNMSBool		prevFlag,
		CNMSInt32		scale,
		CNMSInt32		*lpCurPos,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ret = CNMS_ERR, i, ldata;
	CNMSInt32		cursor, topPos[ CNMS_DIM_MAX ], btmPos[ CNMS_DIM_MAX ], curPos[ CNMS_DIM_MAX ], maxSize[ CNMS_DIM_MAX ];

	if( ( lpCurPos == CNMSNULL ) || ( lpSub == CNMSNULL ) || ( scale == 0 ) ){
		DBGMSG( "[PrevSub_GetCursorRectId]Parameter is error!\n" );
		goto	EXIT;
	}
	CnmsConvRelativePos( lpSub->topPos, topPos, scale );
	CnmsConvRelativePos( lpSub->bottomPos, btmPos, scale );
	CnmsConvRelativePos( lpCurPos, curPos, scale );
	CnmsConvRelativePos( lpSub->maxSize, maxSize, scale );

	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( ( hgFlag == CNMS_TRUE ) || ( lpSub->status == PREVSUB_STATUS_CROP_NONE ) ){
			cursor = 0;
		}
		else{
			cursor = ( btmPos[ i ] - topPos[ i ] ) / 4;
			cursor = ( 5 < cursor ) ? 5 : ( ( cursor < 1 ) ? 1 : cursor );
		}
		
		if( lpSub->status == PREVSUB_STATUS_CROP_NONE ){	/* Rect is none */
			lpSub->clickId[ i ] = ( curPos[ i ] < maxSize[ i ] ) ? PREVSUB_CURSOR_OUTSIDE : PREVSUB_CURSOR_OUTSIDE_PLATEN;
		}
		else if( curPos[ i ] + cursor < topPos[ i ] ){
			lpSub->clickId[ i ] = PREVSUB_CURSOR_OUTSIDE;
		}
		else if( curPos[ i ] <= topPos[ i ] + cursor ){
			lpSub->clickId[ i ] = PREVSUB_CURSOR_ON_LINE_LEFT_TOP;
		}
		else if( curPos[ i ] + cursor < btmPos[ i ] ){
			lpSub->clickId[ i ] = PREVSUB_CURSOR_INSIDE;
		}
		else if( curPos[ i ] <= btmPos[ i ] ){
			lpSub->clickId[ i ] = PREVSUB_CURSOR_ON_LINE_RIGHT_BOTTOM;
		}
		else if( curPos[ i ] < maxSize[ i ] ){
			lpSub->clickId[ i ] = PREVSUB_CURSOR_OUTSIDE;
		}
		else{
			lpSub->clickId[ i ] = PREVSUB_CURSOR_OUTSIDE_PLATEN;
		}
	}
	
	if( hgFlag == CNMS_TRUE ){
		if( lpSub->status == PREVSUB_STATUS_CROP_NONE ){
			/* Rect is none */
			for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
				if( lpSub->clickId[ i ] != PREVSUB_CURSOR_OUTSIDE_PLATEN ){
					lpSub->clickId[ i ] = ( prevFlag == CNMS_TRUE ) ? PREVSUB_CURSOR_INSIDE : PREVSUB_CURSOR_OUTSIDE;
				}
			}
		}
		else{
			/* Rect */
			for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
				if( ( lpSub->clickId[ i ] != PREVSUB_CURSOR_OUTSIDE_PLATEN ) && ( prevFlag == CNMS_FALSE ) ){
					lpSub->clickId[ i ] = PREVSUB_CURSOR_OUTSIDE;
				}
			}
		}
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB_DETAIL__
	DBGMSG( "[PrevSub_GetCursorRectId(hg:%d, prev:%d, scale:%d, pos:%dx%d)]=%d, id:%dx%d\n", hgFlag, prevFlag, scale, lpCurPos[ CNMS_DIM_H ], lpCurPos[ CNMS_DIM_V ], ret, lpSub->clickId[ CNMS_DIM_H ], lpSub->clickId[ CNMS_DIM_V ] );
#endif
	return	ret;
}

CNMSVoid PrevSub_ChageAspect(
		CNMSInt32		aspect,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32	i;
	
	if( lpSub == CNMSNULL ){
		DBGMSG( "[PrevSub_ChageAspect]Parameter is error!\n" );
		goto	EXIT;
	}
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( aspect == CNMS_ASPECT_VARIED ){
			lpSub->fixedSize[ i ] = PREVSUB_RECT_AREA_FREE;
		}
		else{
			lpSub->fixedSize[ i ] = lpSub->bottomPos[ i ] - lpSub->topPos[ i ];
		}
	}
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_SUB__
	DBGMSG( "[PrevSub_ChageAspect(aspect:%d)].\n", aspect );
#endif
	return;
}


#ifdef	__CNMS_DEBUG__
CNMSVoid DebugPreviewSubComp( LPPREVSUBCOMP lpSub )
{
	if( lpSub == CNMSNULL ){
		DBGMSG( "[DebugPreviewSubComp]lpSub == NULL\n" );
		goto	EXIT;
	}

	DBGMSG( "[DebugPreviewSubComp]status    :%d\n", lpSub->status );
	DBGMSG( "[DebugPreviewSubComp]topPos    :%d x %d\n", lpSub->topPos[ CNMS_DIM_H ], lpSub->topPos[ CNMS_DIM_V ] );
	DBGMSG( "[DebugPreviewSubComp]bottomPos :%d x %d\n", lpSub->bottomPos[ CNMS_DIM_H ], lpSub->bottomPos[ CNMS_DIM_V ] );
	DBGMSG( "[DebugPreviewSubComp]clickId   :%d x %d\n", lpSub->clickId[ CNMS_DIM_H ], lpSub->clickId[ CNMS_DIM_V ] );
	DBGMSG( "[DebugPreviewSubComp]clickPos  :%d x %d\n", lpSub->clickPos[ CNMS_DIM_H ], lpSub->clickPos[ CNMS_DIM_V ] );
	DBGMSG( "[DebugPreviewSubComp]fixedSize :%d x %d\n\n", lpSub->fixedSize[ CNMS_DIM_H ], lpSub->fixedSize[ CNMS_DIM_V ] );
EXIT:
	return;
}
#endif	/* __CNMS_DEBUG__ */

#endif	/* _PREV_SUB_C_ */
