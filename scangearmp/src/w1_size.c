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

#ifndef	_W1_SIZE_C_
#define	_W1_SIZE_C_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <gtk/gtk.h>
#include <gtk/gtkpreview.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "cnmstype.h"
#include "cnmsdef.h"
#include "cnmsstr.h"
#include "cnmsfunc.h"
#include "w1.h"
#include "prev_main.h"
#include "scanmain.h"

//#define	__CNMS_DEBUG_UI__

static CNMSVoid  ChangeInputSize( CNMSInt32 newVal, CNMSInt32 *lpIn0, CNMSInt32 *lpIn1, CNMSInt32 *lpOut0, CNMSInt32 *lpOut1 );

CNMSInt32 W1Size_GetOutputPixels(
		CNMSInt32		*lpSize )
{
	CNMSInt32		ret = CNMS_ERR, outRes;
	CNMSDec32		ddata;

	if( ( lpW1Comp == CNMSNULL ) || ( lpSize == CNMSNULL ) ){
		DBGMSG( "[W1Size_GetOutputPixels]Initialize parameter is wrong.\n" );
		goto	EXIT;
	}
	else if( ( outRes = CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) <= 0 ){
		DBGMSG( "[W1Size_GetOutputPixels]Error is occured in CnmsUiGetRealValue( CNMS_OBJ_A_OUTPUT_RESOLUTION ).\n" );
		goto	EXIT;
	}
	
	if( lpW1Comp->group == CNMSUI_SIZE_GROUP_DISPLAY ){
		ddata = (CNMSDec32)( lpW1Comp->base_res ) / (CNMSDec32)( lpW1Comp->size_res );
	}
	else{
		ddata = (CNMSDec32)( lpW1Comp->base_res ) / (CNMSDec32)outRes;
	}
	
	lpSize[ CNMS_DIM_H ] = (CNMSInt32)( (CNMSDec32)( lpW1Comp->outSize[ CNMS_DIM_H ] ) / ddata );
	lpSize[ CNMS_DIM_V ] = (CNMSInt32)( (CNMSDec32)( lpW1Comp->outSize[ CNMS_DIM_V ] ) / ddata );

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	if( lpSize != CNMSNULL ){
		DBGMSG( "[W1Size_GetOutputPixels(lpSize:%dx%d)]= %d.\n", lpSize[ CNMS_DIM_H ], lpSize[ CNMS_DIM_V ], ret );
	}
	else{
		DBGMSG( "[W1Size_GetOutputPixels(lpSize:NULL)]= %d.\n", ret );
	}
#endif
	return	ret;
}

CNMSDec32 W1Size_GetOutputSize( CNMSVoid )
{
	CNMSInt32		ldata, outSize[ CNMS_DIM_MAX ];
	CNMSDec32		ret = CNMS_ERR;

	if( lpW1Comp == CNMSNULL ){
		DBGMSG( "[W1Size_GetOutputSize]Initialize parameter is wrong.\n" );
		goto	EXIT;
	}
	else if( ( ldata = W1Size_GetOutputPixels( outSize ) ) != CNMS_NO_ERR ){
		DBGMSG( "[W1Size_GetOutputSize]Error is occured in W1Size_GetOutputPixels.\n" );
		goto	EXIT;
	}
	else if( ( ldata = CnmsUiGetRealValue( CNMS_OBJ_A_COLOR_MODE, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) <= 0 ){
		DBGMSG( "[W1Size_GetOutputSize]Error is occured in CnmsUiGetRealValue( CNMS_OBJ_A_COLOR_MODE ).\n" );
		goto	EXIT;
	}
	ret = (CNMSDec32)( outSize[ CNMS_DIM_H ] ) * (CNMSDec32)( outSize[ CNMS_DIM_V ] ) * (CNMSDec32)ldata / 8;	/* byte */
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1Size_GetOutputSize()]= %f.\n", ret );
#endif
	return	ret;
}

CNMSInt32 W1Size_GetEstimatedSizeStr(
		CNMSLPSTR		lpStr,
		CNMSInt32		bufSize )
{
	CNMSInt32		ret = CNMS_ERR, i, ldata;
	CNMSDec32		tmpSize;
	const CNMSLPSTR	unitStr[] = { " KB\0", " MB\0", " GB\0", " TB\0", };

	if( lpW1Comp == CNMSNULL ){
		DBGMSG( "[W1Size_GetEstimatedSizeStr]Initialize parameter is wrong.\n" );
		goto	EXIT;
	}
	else if( ( tmpSize = W1Size_GetOutputSize() ) <= 0 ){
		DBGMSG( "[W1Size_GetEstimatedSizeStr]Error is occured in W1Size_GetOutputSize.\n" );
		goto	EXIT;
	}

	lpStr[ 0 ] = '\0';
	ldata = sizeof( unitStr ) / sizeof( unitStr[ 0 ] );
	for( i = 0 ; i < ldata ; i ++ ){
		tmpSize = tmpSize / 1024;
		if( tmpSize < 1024 ){
			tmpSize = ( tmpSize < 0.01 ) ? 0.01 : tmpSize;
			if( ( i == 0 ) && ( 1 <= tmpSize ) ){
				tmpSize = CnmsRoundValue( tmpSize, 0, CNMS_ROUND_CUT );
				sprintf( lpStr, "%.0f\0", tmpSize );	/* ex. 10 KB */
			}
			else{
				tmpSize = CnmsRoundValue( tmpSize, 2, CNMS_ROUND_CUT );
				sprintf( lpStr, "%.2f\0", tmpSize );	/* ex. 10.00 MB */
			}
			break;
		}
	}

	if( ( ldata = CnmsStrLen( lpStr ) ) <= 0 ){
		DBGMSG( "[W1Size_GetEstimatedSizeStr]Can't change val to string.\n" );
		goto	EXIT;
	}

	CnmsStrCat( unitStr[ i ], lpStr + ldata, 256 );

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1Size_GetEstimatedSizeStr()]= %d, str:%s.\n", ret, ( ret == CNMS_NO_ERR ) ? lpStr : CNMSNULL );
#endif
	return	ret;
}

CNMSInt32 W1Size_ChangeSpin(
		CNMSInt32		inVal,
		CNMSInt32		objectID )
{
	CNMSInt32		ret = CNMS_ERR;

	/* check */
	if( lpW1Comp == CNMSNULL ){
		DBGMSG( "[W1Size_ChangeSpin]Initialize parameter is error.\n" );
		goto	EXIT;
	}
	switch( objectID ){
		case	CNMS_OBJ_A_INPUT_WIDTH:
			if( inVal == lpW1Comp->inSize[ CNMS_DIM_H ] ){
				goto	EXIT;
			}
			ChangeInputSize( inVal, lpW1Comp->inSize + CNMS_DIM_H, lpW1Comp->inSize + CNMS_DIM_V, lpW1Comp->outSize + CNMS_DIM_H, lpW1Comp->outSize + CNMS_DIM_V );
			break;
		case	CNMS_OBJ_A_INPUT_HEIGHT:
			if( inVal == lpW1Comp->inSize[ CNMS_DIM_V ] ){
				goto	EXIT;
			}
			ChangeInputSize( inVal, lpW1Comp->inSize + CNMS_DIM_V, lpW1Comp->inSize + CNMS_DIM_H, lpW1Comp->outSize + CNMS_DIM_V, lpW1Comp->outSize + CNMS_DIM_H );
			break;
		case	CNMS_OBJ_A_OUTPUT_WIDTH:
			if( inVal == lpW1Comp->outSize[ CNMS_DIM_H ] ){
				goto	EXIT;
			}
			lpW1Comp->outSize[ CNMS_DIM_H ] = inVal;
			lpW1Comp->scale = (CNMSInt32)( ( (CNMSDec32)inVal * CNMS_SCALE_100_PERCENT ) / (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_H ] ) );
			lpW1Comp->outSize[ CNMS_DIM_V ] = (CNMSInt32)( ( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_V ] ) * (CNMSDec32)( lpW1Comp->scale ) ) / CNMS_SCALE_100_PERCENT );
			break;
		case	CNMS_OBJ_A_OUTPUT_HEIGHT:
			if( inVal == lpW1Comp->outSize[ CNMS_DIM_V ] ){
				goto	EXIT;
			}
			lpW1Comp->outSize[ CNMS_DIM_V ] = inVal;
			lpW1Comp->scale = (CNMSInt32)( ( (CNMSDec32)inVal * CNMS_SCALE_100_PERCENT ) / (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_V ] ) );
			lpW1Comp->outSize[ CNMS_DIM_H ] = (CNMSInt32)( ( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_H ] ) * (CNMSDec32)( lpW1Comp->scale ) ) / CNMS_SCALE_100_PERCENT );
			break;
		default:	/* CNMS_OBJ_A_SCALE */
			if( inVal == lpW1Comp->scale ){
				goto	EXIT;
			}
			lpW1Comp->scale = inVal;
			/* output width and height */
			lpW1Comp->outSize[ CNMS_DIM_H ] = (CNMSInt32)( ( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_H ] ) * (CNMSDec32)( lpW1Comp->scale ) ) / CNMS_SCALE_100_PERCENT );
			lpW1Comp->outSize[ CNMS_DIM_V ] = (CNMSInt32)( ( (CNMSDec32)( lpW1Comp->inSize[ CNMS_DIM_V ] ) * (CNMSDec32)( lpW1Comp->scale ) ) / CNMS_SCALE_100_PERCENT );
			break;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1Size_ChangeSpin(inVal:%f, objectID:%d)]=%d.\n", inVal, objectID, ret );
#endif
	return	ret;
}

static CNMSVoid ChangeInputSize(
		CNMSInt32		newVal,
		CNMSInt32		*lpIn0,
		CNMSInt32		*lpIn1,
		CNMSInt32		*lpOut0,
		CNMSInt32		*lpOut1 )
{
	CNMSInt32		aspect;
	CNMSInt32		lastVal, tmpVal, i;

	/* get aspect */
	if( ( aspect = CnmsUiAspectGet( lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[ChangeInputSize]Error is occured in CnmsUiAspectGet.\n" );
		goto	EXIT;
	}

	lastVal = *lpIn0;
	*lpIn0 = newVal;

	switch( aspect ){
		case	CNMS_ASPECT_VARIED:	/* varied */
			/* change output height( or width ) */
			*lpOut0 = (CNMSInt32)( ( (CNMSDec32)newVal * (CNMSDec32)( lpW1Comp->scale ) ) / CNMS_SCALE_100_PERCENT );
			break;
		case	CNMS_ASPECT_FIXED:	/* fixed aspect only */
			/* change all */
			tmpVal = *lpIn1;	/* last input height */
			*lpIn1 = (CNMSInt32)( ( (CNMSDec32)tmpVal * (CNMSDec32)newVal ) / (CNMSDec32)lastVal );	/* new input height */
			*lpOut0 = (CNMSInt32)( ( (CNMSDec32)newVal * (CNMSDec32)( lpW1Comp->scale ) ) / CNMS_SCALE_100_PERCENT );
			*lpOut1 = (CNMSInt32)( ( ( ( (CNMSDec32)tmpVal * (CNMSDec32)newVal ) / (CNMSDec32)lastVal ) * (CNMSDec32)( lpW1Comp->scale ) ) / CNMS_SCALE_100_PERCENT );
			break;
		case	CNMS_ASPECT_STANDERD:	/* fixed both aspect and size */
			/* change input height( or width ) not change output size */
			tmpVal = *lpIn1;	/* last input height */
			*lpIn1 = (CNMSInt32)( (CNMSDec32)tmpVal * (CNMSDec32)newVal / (CNMSDec32)lastVal );	/* new input height */
			lpW1Comp->scale = (CNMSInt32)( ( (CNMSDec32)( *lpOut0 ) * CNMS_SCALE_100_PERCENT ) / (CNMSDec32)newVal );
			break;
	}
	/* Clop */
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		lpW1Comp->inSize[ i ] = ( lpW1Comp->inSize[ i ] < lpW1Comp->minSize[ i ] ) ? lpW1Comp->minSize[ i ] : ( ( lpW1Comp->maxSize[ i ] < lpW1Comp->inSize[ i ] ) ? lpW1Comp->maxSize[ i ] : lpW1Comp->inSize[ i ] );
	}
EXIT:
	return;
}

/* called at changing rect */
CNMSInt32 W1Size_ChangeRectAreaAfter(
		CNMSInt32		*lpSize,
		CNMSInt32		*lpOffset )
{
	CNMSInt32		ret = CNMS_ERR, i, aspect, scale[ CNMS_DIM_MAX ];

	/* check */
	if( ( lpW1Comp == CNMSNULL ) || ( lpSize == CNMSNULL ) || ( lpOffset == CNMSNULL ) ){
		DBGMSG( "[W1Size_ChangeRectAreaAfter]Initialize parameter is error.\n" );
		goto	EXIT;
	}
	else if( ( aspect = CnmsUiAspectGet( lpW1Comp->lpLink, lpW1Comp->linkNum ) ) < 0 ){
		DBGMSG( "[W1Size_ChangeRectAreaAfter]Error is occured in CnmsUiAspectGet.\n" );
		goto	EXIT;
	}

	/* Clop */
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		lpSize[ i ] = ( lpSize[ i ] < lpW1Comp->minSize[ i ] ) ? lpW1Comp->minSize[ i ] : ( ( lpW1Comp->maxSize[ i ] < lpSize[ i ] ) ? lpW1Comp->maxSize[ i ] : lpSize[ i ] );
	}
	switch( aspect ){
		case	CNMS_ASPECT_VARIED:	/* varied */
			for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
				if( lpW1Comp->inSize[ i ] != lpSize[ i ]  ){
					/* change output height( or width ) */
					lpW1Comp->outSize[ i ] = (CNMSInt32)( ( (CNMSDec32)lpSize[ i ] * (CNMSDec32)( lpW1Comp->scale ) ) / CNMS_SCALE_100_PERCENT );
					lpW1Comp->inSize[ i ] = lpSize[ i ];
				}
			}
			break;

		case	CNMS_ASPECT_FIXED:	/* fixed only aspect */
			/* change input and output size, keep scale */
			for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
				/* output size */
				lpW1Comp->outSize[ i ] = (CNMSInt32)( ( (CNMSDec32)lpSize[ i ] * (CNMSDec32)( lpW1Comp->scale ) ) / CNMS_SCALE_100_PERCENT );
				lpW1Comp->inSize[ i ] = lpSize[ i ];
			}
			break;

		case	CNMS_ASPECT_STANDERD:	/* fixed both aspect and size */
			/* change input size and scale, keep output size */
			for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
				scale[ i ] = (CNMSInt32)( ( (CNMSDec32)( lpW1Comp->outSize[ i ] ) * CNMS_SCALE_100_PERCENT ) / (CNMSDec32)( lpSize[ i ] ) );
			}
			lpW1Comp->scale = ( scale[ CNMS_DIM_H ] < scale[ CNMS_DIM_V ] ) ? scale[ CNMS_DIM_H ] : scale[ CNMS_DIM_V ];
			for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
				lpW1Comp->inSize[ i ] = (CNMSInt32)( ( (CNMSDec32)( lpW1Comp->outSize[ i ] ) * CNMS_SCALE_100_PERCENT ) / (CNMSDec32)( lpW1Comp->scale ) );
			}
			break;
	}

	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		/* Offset */
		lpW1Comp->offset[ i ] = lpOffset[ i ];
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_UI__
	DBGMSG( "[W1Size_ChangeRectAreaAfter(size:%dx%d, offset:%dx%d)]=%d\n", lpSize[ CNMS_DIM_H ], lpSize[ CNMS_DIM_V ], lpOffset[ CNMS_DIM_H ], lpOffset[ CNMS_DIM_V ], ret );
#endif
	return	ret;
}

#endif	/* _W1_SIZE_C_ */

