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

#ifndef	_CNMSFUNC_C_
#define	_CNMSFUNC_C_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cnmstype.h"
#include "cnmsdef.h"

#include "cnmsfunc.h"

/*	#define	__CNMS_DEBUG_FUNC__	*/

CNMSInt32 CnmsStrLen(
		CNMSLPSTR		lpStr )
{
	CNMSInt32	ret = CNMS_ERR;

	if( lpStr == CNMSNULL ){
		DBGMSG( "[CnmsStrLen]Parameter is error.\n" );
		goto	EXIT;
	}
	ret = strlen( lpStr );
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsStrLen(lpStr:%s)]=%d.\n", lpStr, ret );
#endif
	return	ret;
}

CNMSInt32 CnmsStrCopy(
		CNMSLPSTR		lpSrc,
		CNMSLPSTR		lpDst,
		CNMSInt32		dstLen )
{
	CNMSInt32	ret = CNMS_ERR, srcLen;
	
	if( ( lpSrc == CNMSNULL ) || ( lpDst == CNMSNULL ) || ( dstLen <= 0 ) ){
		DBGMSG( "[CnmsStrCopy]Parameter is error.\n" );
		goto	EXIT;
	}

	if( ( srcLen  = CnmsStrLen( (CNMSInt8 *)lpSrc ) ) >= dstLen ){
		DBGMSG( "[CnmsStrCopy]src string(%d) is too long(>%d).\n", srcLen, dstLen );
		goto	EXIT;
	}
	
	strncpy( (CNMSInt8 *)lpDst, (CNMSInt8 *)lpSrc, srcLen );
	lpDst[ srcLen ] = '\0';

	ret = srcLen;
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsStrCopy(lpSrc:%s,lpDst:%s,dstLen:%d)]=%d.\n", lpSrc, lpDst, dstLen, ret );
#endif
	return	ret;
}

CNMSInt32 CnmsStrCat(
		CNMSLPSTR		lpSrc,
		CNMSLPSTR		lpDst,
		CNMSInt32		dstLen )
{
	CNMSInt32	ret = CNMS_ERR, totalLen, srcLen;
	
	if( ( lpSrc == CNMSNULL ) || ( lpDst == CNMSNULL ) || ( dstLen <= 0 ) ){
		DBGMSG( "[CnmsStrCat]Parameter is error.\n" );
		goto	EXIT;
	}

	srcLen = CnmsStrLen( (CNMSInt8 *)lpSrc );
	if( ( totalLen  = srcLen + CnmsStrLen( (CNMSInt8 *)lpDst ) ) >= dstLen ){
		DBGMSG( "[CnmsStrCat]total string(%d) is too long(>%d).\n", totalLen, dstLen );
		goto	EXIT;
	}
	
	strncat( (CNMSInt8 *)lpDst, (CNMSInt8 *)lpSrc, srcLen );
	lpDst[ totalLen ] = '\0';

	ret = totalLen;
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsStrCat(lpSrc:%s,lpDst:%s,dstLen:%d)]=%d.\n", lpSrc, lpDst, dstLen, ret );
#endif
	return	ret;
}

CNMSInt32 CnmsStrCompare(
		const CNMSLPSTR		lpStr1,
		const CNMSLPSTR		lpStr2 )
{
	CNMSInt32	ret = CNMS_ERR;
	
	if( ( lpStr1 == CNMSNULL ) || ( lpStr2 == CNMSNULL ) ){
		DBGMSG( "[CnmsStrCompare]Parameter is error.\n" );
		goto	EXIT;
	}
	else if( strcmp( lpStr1, lpStr2 ) != 0 ){
/*		DBGMSG( "[CnmsStrCompare]Error is occured in strcmp.\n" );	*/
		goto	EXIT;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsStrCompare(lpStr1:%s,lpStr2:%s)]=%d.\n", lpStr1, lpStr2, ret );
#endif
	return	ret;
}


CNMSLPSTR CnmsGetMem(
		CNMSInt32		size )
{
	CNMSLPSTR	ptr = CNMSNULL;

	if( ( ptr = (CNMSLPSTR)malloc( size ) ) == CNMSNULL ){
		DBGMSG( "[CnmsGetMem]Error is occured in malloc.\n" );
		goto	EXIT;
	}
	CnmsSetMem( ptr, 0, size );
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsGetMem(size:%d)]=%d.\n", size, ptr );
#endif
	return	ptr;
}

CNMSVoid  CnmsFreeMem(
		CNMSLPSTR		ptr )
{
	if( ptr != CNMSNULL ){
		free( ptr );
	}
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsFreeMem(ptr:%d)].\n", ptr );
#endif
	return;
}

CNMSVoid CnmsCopyMem(
		CNMSLPSTR		lpSrc,
		CNMSLPSTR		lpDst,
		CNMSInt32		size )
{
	if( ( lpSrc == CNMSNULL ) || ( lpDst == CNMSNULL ) || ( size <= 0 ) ){
		DBGMSG( "[CnmsCopyMem]Parameter is error.\n" );
		goto	EXIT;
	}
	memcpy( lpDst, lpSrc, size );
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsCopyMem(lpSrc:%d,lpDst:%d,size:%d)].\n", lpSrc, lpDst, size );
#endif
	return;
}

CNMSVoid CnmsMoveMem(
		CNMSLPSTR		lpSrc,
		CNMSLPSTR		lpDst,
		CNMSInt32		size )
{
	/* white src by NULL after copy */
	if( ( lpSrc == CNMSNULL ) || ( lpDst == CNMSNULL ) || ( size <= 0 ) ){
		DBGMSG( "[CnmsMoveMem]Parameter is error.\n" );
		goto	EXIT;
	}
	CnmsCopyMem( lpSrc, lpDst, size );
	CnmsSetMem( lpSrc, 0, size );
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsMoveMem(lpSrc:%d,lpDst:%d,size:%d)].\n", lpSrc, lpDst, size );
#endif
	return;
}

CNMSVoid CnmsSetMem(
		CNMSLPSTR		lpDst,
		CNMSByte		val,
		CNMSInt32		size )
{
	if( ( lpDst == CNMSNULL ) || ( size <= 0 ) ){
		DBGMSG( "[CnmsSetMem]Parameter is error.\n" );
		goto	EXIT;
	}
	memset( lpDst, val, size );
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsSetMem(lpDst:%d,val:%d,size:%d)].\n", lpDst, val, size );
#endif
	return;
}

CNMSDec32 CnmsRoundValue(
		CNMSDec32	value,
		CNMSInt32	decFig,
		CNMSInt32	type )
{
	CNMSInt32	i;
	CNMSDec32	ret;
	CNMSByte	str[ 128 ];

	for( i = 0 ; i < decFig ; i ++ ){
		value = value * 10;
	}
	if( type == CNMS_ROUND_UP_DOWN ){
		value += 0.5;
	}
	ret = floor( value );

	snprintf( str, 128, "%f", ret );

	if( ( value - ret ) >= 1 ){
		ret += 1;
	}

	for( i = 0 ; i < decFig ; i ++ ){
		ret = ret / 10;
	}
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsRoundValue(value:%f,decFig:%d,type:%d)]=%f.\n", value, decFig, type, ret );
#endif
	return	ret;
}

CNMSDec32 CnmsConvUnit(
		CNMSDec32	in,
		CNMSInt32	unit_in,
		CNMSInt32	unit_out,
		CNMSInt32	res_in,
		CNMSInt32	res_out )
{
	CNMSDec32		ret = CNMS_ERR, ddata;
	CNMSInt32		decFig;

	if( ( decFig = CnmsGetDecFig( unit_out ) ) < 0 ){
		DBGMSG( "[CnmsConvUnit]Error is occured in CnmsGetDecFig.\n" );
		goto	EXIT;
	}

	if( unit_in == unit_out ){
		if( ( res_in == res_out ) || ( unit_in != CNMS_A_UNIT_PIXEL ) ){
			ret = in;
			goto	EXIT;
		}
		else{
			ddata = in * (CNMSDec32)res_out / (CNMSDec32)res_in;
		}
	}
	else if( unit_in == CNMS_A_UNIT_PIXEL ){
		switch( unit_out ){
			case	CNMS_A_UNIT_INCH:
				ddata = in / (CNMSDec32)res_in;
				break;
			case	CNMS_A_UNIT_CENTIMETER:
				ddata = in / (CNMSDec32)res_in * 2.54;
				break;
			default:	/* CNMS_A_UNIT_MILLIMETER */
				ddata = in / (CNMSDec32)res_in * 25.4;
				break;
		}
	}
	else if( unit_in == CNMS_A_UNIT_INCH ){
		switch( unit_out ){
			case	CNMS_A_UNIT_PIXEL:
				ddata = in * (CNMSDec32)res_out;
				break;
			case	CNMS_A_UNIT_CENTIMETER:
				ddata = in * 2.54;
				break;
			default:	/* CNMS_A_UNIT_MILLIMETER */
				ddata = in * 25.4;
				break;
		}
	}
	else if( unit_in == CNMS_A_UNIT_CENTIMETER ){
		switch( unit_out ){
			case	CNMS_A_UNIT_PIXEL:
				ddata = in * (CNMSDec32)res_out / 2.54;
				break;
			case	CNMS_A_UNIT_INCH:
				ddata = in / 2.54;
				break;
			default:	/* CNMS_A_UNIT_MILLIMETER */
				ddata = in * 10;
				break;
		}
	}
	else{	/* CNMS_A_UNIT_MILLIMETER */
		switch( unit_out ){
			case	CNMS_A_UNIT_PIXEL:
				ddata = in * (CNMSDec32)res_out / 25.4;
				break;
			case	CNMS_A_UNIT_INCH:
				ddata = in / 25.4;
				break;
			default:	/* CNMS_A_UNIT_CENTIMETER */
				ddata = in / 10;
				break;
		}
	}

	if( unit_out == CNMS_A_UNIT_PIXEL ){
		ret = CnmsRoundValue( ddata, decFig, CNMS_ROUND_CUT );
	}
	else{
		ret = CnmsRoundValue( ddata, decFig, CNMS_ROUND_UP_DOWN );		
	}
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsConvUnit(in:%f,unit:%d->%d,ret:%d->%d)]=%f.\n", in, unit_in, unit_out, res_in, res_out, ret );
#endif
	return	ret;
}

CNMSInt32 CnmsGetDecFig( CNMSInt32 unit )
{
	CNMSInt32	ret = CNMS_ERR, retFig[ CNMS_A_UNIT_MAX ] = { 0, 2, 2, 1 };

	if( ( unit < 0 ) || ( CNMS_A_UNIT_MAX <= unit ) ){
		DBGMSG( "[CnmsGetDecFig]Parameter is error.\n" );
		goto	EXIT;
	}
	ret = retFig[ unit ];
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsGetDecFig(unit:%d)]=%d.\n", unit, ret );
#endif
	return	ret;
}

CNMSVoid  CnmsClopSize(
		const CNMSInt32	*lpMaxSize,
		CNMSInt32			*lpPos )
{
	CNMSInt32		i;
	
	if( ( lpMaxSize == CNMSNULL ) || ( lpPos == CNMSNULL ) ){
		DBGMSG( "[CnmsClopSize]Parameter is error.\n" );
		goto	EXIT;
	}
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( lpMaxSize[ i ] <= lpPos[ i ] ){
			lpPos[ i ] = lpMaxSize[ i ] - 1;
		}
	}
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsClopSize(max:%dx%d,pos:%dx%d)].\n", lpMaxSize[ 0 ], lpMaxSize[ 1 ], lpPos[ 0 ], lpPos[ 1 ] );
#endif
	return;
}

CNMSInt32 CnmsGetFitScale(
		CNMSInt32			type,
		const CNMSInt32	*lpBase,
		const CNMSInt32	*lpCurr )
{
	CNMSInt32	i, scale = CNMS_ERR;
	CNMSDec32	base[ CNMS_DIM_MAX ], curr[ CNMS_DIM_MAX ];
	
	/* check */
	if( ( type < 0 ) || ( CNMS_FIT_MAX <= type ) || ( lpBase == CNMSNULL ) || ( lpCurr == CNMSNULL ) ){
		DBGMSG( "[CnmsGetFitScale]Parameter is error.\n" );
		goto	EXIT;
	}
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		base[ i ] = (CNMSDec32)lpBase[ i ];
		curr[ i ] = (CNMSDec32)lpCurr[ i ];
	}
	if( base[ CNMS_DIM_H ] * curr[ CNMS_DIM_V ] >= base[ CNMS_DIM_V ] * curr[ CNMS_DIM_H ] ){
		if( type == CNMS_FIT_INSIDE ){
			scale = ( base[ CNMS_DIM_V ] * CNMS_SCALE_100_PERCENT ) / curr[ CNMS_DIM_V ];
		}
		else{
			scale = ( base[ CNMS_DIM_H ] * CNMS_SCALE_100_PERCENT ) / curr[ CNMS_DIM_H ];
		}
	}
	else{
		if( type == CNMS_FIT_INSIDE ){
			scale = ( base[ CNMS_DIM_H ] * CNMS_SCALE_100_PERCENT ) / curr[ CNMS_DIM_H ];
		}
		else{
			scale = ( base[ CNMS_DIM_V ] * CNMS_SCALE_100_PERCENT ) / curr[ CNMS_DIM_V ];
		}
	}
	
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		if( ( type == CNMS_FIT_INSIDE ) && ( ( base[ i ] * scale ) < ( curr[ i ] * CNMS_SCALE_100_PERCENT ) ) ){
			scale --;
		}
		if( ( type == CNMS_FIT_OUTSIDE ) && ( ( base[ i ] * scale ) > ( curr[ i ] * CNMS_SCALE_100_PERCENT ) ) ){
			scale ++;
		}
	}
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsGetFitScale(type:%d,base:%dx%d,cur:%dx%d)]=%d.\n", type, lpBase[ 0 ], lpBase[ 1 ], lpCurr[ 0 ], lpCurr[ 1 ], scale );
#endif
	return	scale;
}

CNMSInt32 CnmsFitSizeKeepAspectRatio(
		CNMSInt32			type,
		const CNMSInt32	*lpBase,
		CNMSInt32			*lpCurr )
{
	CNMSInt32	scale;

	if( ( scale = CnmsGetFitScale( type, lpBase, lpCurr ) ) <= 0 ){
		DBGMSG( "[CnmsFitSizeKeepAspectRatio]Error is occured in CnmsGetFitScale.\n" );
		goto	EXIT;
	}
	lpCurr[ CNMS_DIM_H ] = (CNMSInt32)( ( (CNMSDec32)( lpCurr[ CNMS_DIM_H ] ) * (CNMSDec32)scale ) / CNMS_SCALE_100_PERCENT );
	lpCurr[ CNMS_DIM_V ] = (CNMSInt32)( ( (CNMSDec32)( lpCurr[ CNMS_DIM_V ] ) * (CNMSDec32)scale ) / CNMS_SCALE_100_PERCENT );
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsFitSizeKeepAspectRatio(type:%d,base:%dx%d,cur:%dx%d)]=%d.\n", type, lpBase[ 0 ], lpBase[ 1 ], lpCurr[ 0 ], lpCurr[ 1 ], scale );
#endif
	return	scale;
}

CNMSInt32 CnmsConvAbsolutePos(
		const CNMSInt32	*lpSrcPos,
		CNMSInt32			*lpDstPos,
		CNMSInt32			scale )
{
	CNMSInt32	ret = CNMS_ERR;

	if( ( lpSrcPos == CNMSNULL ) || ( lpDstPos == CNMSNULL ) || ( scale == 0 ) ){
		DBGMSG( "[CnmsConvAbsolutePos]Parameter is error.\n" );
		goto	EXIT;
	}
	lpDstPos[ CNMS_DIM_H ] = (CNMSInt32)( ( (CNMSDec32)( lpSrcPos[ CNMS_DIM_H ] ) * CNMS_SCALE_100_PERCENT ) / (CNMSDec32)scale );
	lpDstPos[ CNMS_DIM_V ] = (CNMSInt32)( ( (CNMSDec32)( lpSrcPos[ CNMS_DIM_V ] ) * CNMS_SCALE_100_PERCENT ) / (CNMSDec32)scale );

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsConvAbsolutePos(src:%dx%d,dst:%dx%d,scale%d)]=%d.\n", lpSrcPos[ 0 ], lpSrcPos[ 1 ], lpDstPos[ 0 ], lpDstPos[ 1 ], scale, ret );
#endif
	return	ret;
}

CNMSInt32 CnmsConvRelativePos(
		const CNMSInt32	*lpSrcPos,
		CNMSInt32			*lpDstPos,
		CNMSInt32			scale )
{
	CNMSInt32	ret = CNMS_ERR;

	if( ( lpSrcPos == CNMSNULL ) || ( lpDstPos == CNMSNULL ) || ( scale == 0 ) ){
		DBGMSG( "[CnmsConvRelativePos]Parameter is error.\n" );
		goto	EXIT;
	}
	lpDstPos[ CNMS_DIM_H ] = (CNMSInt32)( ( (CNMSDec32)( lpSrcPos[ CNMS_DIM_H ] ) * (CNMSDec32)scale ) / CNMS_SCALE_100_PERCENT );
	lpDstPos[ CNMS_DIM_V ] = (CNMSInt32)( ( (CNMSDec32)( lpSrcPos[ CNMS_DIM_V ] ) * (CNMSDec32)scale ) / CNMS_SCALE_100_PERCENT );

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_FUNC__
	DBGMSG( "[CnmsConvRelativePos(src:%dx%d,dst:%dx%d,scale%d)]=%d.\n", lpSrcPos[ 0 ], lpSrcPos[ 1 ], lpDstPos[ 0 ], lpDstPos[ 1 ], scale, ret );
#endif
	return	ret;
}


LPCNMS_ROOT CnmsCreateRoot( CNMSVoid )
{
	LPCNMS_ROOT root = CNMSNULL;
	
	if( ( root = (LPCNMS_ROOT)CnmsGetMem( sizeof( CNMS_ROOT ) ) ) == CNMSNULL ) {
		goto EXIT;
	}
	root->page_num = 0;
	root->head = CNMSNULL;
	root->tail = CNMSNULL;
EXIT:
	return root;
}

CNMSInt32 CnmsDisposeRoot( LPCNMS_ROOT *root )
{
	if( root == CNMSNULL ) {
		return -1;
	}
	else if( *root == CNMSNULL ) {
		return -1;
	}
	else if( (*root)->tail != CNMSNULL  ) {
		return 1;
	}

	CnmsFreeMem( (CNMSLPSTR)(*root) );
	*root = CNMSNULL;
	return 0;
}

LPCNMS_NODE CnmsNewNode( CNMSByte *file_path )
{
	LPCNMS_NODE	node = CNMSNULL;

	if( file_path == CNMSNULL) {
		goto EXIT;
	}

	if( ( node = (LPCNMS_NODE)CnmsGetMem( sizeof( CNMS_NODE ) ) ) == CNMSNULL ) {
		goto EXIT;
	}
	if( CnmsStrCopy( file_path, node->file_path, sizeof( node->file_path ) ) == CNMS_ERR ) {
		CnmsFreeMem( (CNMSLPSTR)node );
		goto EXIT;
	}
	node->page = -1;
	node->show_page = CNMS_FALSE;
	node->rotate = CNMS_FALSE;
	node->prev = CNMSNULL;
	node->next = CNMSNULL;
	node->fd   = CNMS_FILE_ERR;

EXIT:
	return node;
}

CNMSVoid CnmsDisposeNode( LPCNMS_NODE *pnode )
{
	LPCNMS_NODE	node = CNMSNULL;
	
	if( pnode == CNMSNULL ) {
		return;
	}
	node = *pnode;
	
	if( node != CNMSNULL ) {
		CnmsFreeMem( (CNMSLPSTR)node );
		*pnode = CNMSNULL;
	}
}

LPCNMS_NODE CnmsPutQueue( LPCNMS_ROOT root, LPCNMS_NODE node )
{
	LPCNMS_NODE	ret = CNMSNULL;
	
	if( root == CNMSNULL || node == CNMSNULL ) {
		goto EXIT;
	}
	
	if( root->head == CNMSNULL ) {
		root->head = node; /* first node */
	}
	if( root->tail != CNMSNULL ) {
		node->prev = root->tail;
		root->tail->next = node;
	}
	root->tail = node;
	root->page_num++;
	node->page = root->page_num;

	ret = node;

EXIT:
	return ret;
}

CNMSInt32 CnmsDisposeQueue( LPCNMS_ROOT root, CNMSInt32 type )
{
	CNMSInt32	ret = CNMS_ERR;
	LPCNMS_NODE	node = CNMSNULL;
	
	if( root == CNMSNULL ){
		goto EXIT;
	}
	else if ( root->tail == CNMSNULL || root->head == CNMSNULL ) {
		goto EXIT; /* no node */
	}
	
	switch( type )
	{
		case CNMS_NODE_TAIL :
			/* remove node. */
			node = root->tail;
			root->tail = node->prev;
			
			if( node->prev == CNMSNULL)
				root->head = CNMSNULL; /* removed last node. */
			else
				node->prev->next = CNMSNULL;

			break;
			
		case CNMS_NODE_HEAD :
			/* remove node. */
			node = root->head;
			root->head = node->next;
			
			if( node->next == CNMSNULL)
				root->tail = CNMSNULL; /* removed last node. */
			else
				node->next->prev = CNMSNULL;

			break;
			
		default :
			goto EXIT;
	}

	CnmsFreeMem( (CNMSLPSTR)node );
	ret = CNMS_NO_ERR;
	
EXIT:
	return ret;
}


#endif	/* _CNMSFUNC_C_ */
