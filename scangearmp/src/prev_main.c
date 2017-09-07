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

#ifndef	_PREV_MAIN_C_
#define	_PREV_MAIN_C_

/*	#define	__CNMS_DEBUG_PREVIEW__	*/

#include <limits.h>
//#include <math.h>
#include <gtk/gtk.h>
//#include <gdk/gdkkeysyms.h>

#include "support.h"

#include "cnmstype.h"
#include "cnmsdef.h"
#include "cnmsfunc.h"

#include "file_control.h"
#include "prev_main.h"
#include "prev_sub.h"
#include "cnmsability.h"
#include "w1.h"

#include "errors.h"

#define	PREVIEW_OUT_COLOR_R			(64)
#define	PREVIEW_OUT_COLOR_G			(64)
#define	PREVIEW_OUT_COLOR_B			(64)

#ifdef	__CNMS_DEBUG_PREVIEW__
#ifdef	__CNMS_DEBUG_DETAIL__
	#define	__CNMS_DEBUG_PREVIEW_DETAIL__
#endif
#endif

enum{
	PREVIEW_MOUSE_CLICKED_NONE = 0,
	PREVIEW_MOUSE_CLICKED_LEFT,
	PREVIEW_MOUSE_CLICKED_RIGHT,
	PREVIEW_MOUSE_CLICKED_MAX,
};

typedef struct{
	GtkWidget			*main_widget;
	GtkWidget			*prev_widget;
	CNMSInt32			clickedButton;
	/* min size */
	CNMSInt32			realRes;
	CNMSInt32			minSize[ CNMS_DIM_MAX ];	/* unit:pixel, res:realRes */
	CNMSInt32			maxSize[ CNMS_DIM_MAX ];	/* unit:pixel, res:realRes */
	CNMSInt32			platenSize[ CNMS_DIM_MAX ];	/* unit:pixel, res:realRes */

	CNMSInt32			prevRes;
	CNMSInt32			scale;	/* realSize to preview area */
	
	CNMSInt32			imageSize[ CNMS_DIM_MAX ];
	CNMSInt32			memSize;	/* size of lpImage */
	CNMSLPSTR			lpImage;

/* GtkPreview -> GtkDrawingArea */
	GdkGC				*gc;
	CNMSUInt8			*lpBuf;
	CNMSInt32			areaSize[ CNMS_DIM_MAX ];
	CNMSInt32			pre_areaSize[ CNMS_DIM_MAX ];

	PREVSUBCOMP			subComp;
}PREVIEWCOMP, *LPPREVIEWCOMP;

static LPPREVIEWCOMP	lpPrevComp = CNMSNULL;

static CNMSInt32 DrawRectInvert( CNMSVoid );
static CNMSInt32 SubPrevResize( CNMSBool prevFlag );
static CNMSVoid SubClickHg( CNMSBool prevFlag, CNMSInt32 scale, CNMSInt32 *lpCurPos, LPPREVSUBCOMP lpSub );
static CNMSInt32 SubSetShowSizeToSpin( LPPREVSUBCOMP lpSub );

#ifdef	__CNMS_DEBUG__
CNMSVoid DebugPreviewComp( CNMSVoid );
#endif

CNMSInt32 Preview_Open( GtkWidget *mainWindow )
{
	CNMSInt32			ret = CNMS_ERR, ldata, i;

	if( mainWindow == CNMSNULL ){
		DBGMSG( "[Preview_Open]Parameter is error!\n" );
		goto	EXIT;
	}
	/* close */
	if( lpPrevComp != CNMSNULL ){
		Preview_Close();
	}
	/* get memory */
	if( ( lpPrevComp = (LPPREVIEWCOMP)CnmsGetMem( sizeof( PREVIEWCOMP ) ) ) == CNMSNULL ){
		DBGMSG( "[Preview_Open]Can't get work memory!\n" );
		goto	EXIT;
	}
	/* init */
	lpPrevComp->main_widget = mainWindow;
	lpPrevComp->clickedButton = PREVIEW_MOUSE_CLICKED_NONE;
	lpPrevComp->lpImage = CNMSNULL;
	lpPrevComp->memSize = 0;
	lpPrevComp->prevRes = 0;
	lpPrevComp->scale = 0;
	/* GtkPreview -> GtkDrawingArea */
	lpPrevComp->gc = CNMSNULL;
	lpPrevComp->lpBuf = CNMSNULL;
	
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		lpPrevComp->minSize[ i ] = lpPrevComp->maxSize[ i ] = lpPrevComp->imageSize[ i ] = 0;
	}
	if( ( lpPrevComp->prev_widget = (GtkWidget*)lookup_widget( mainWindow, "main_preview" ) ) == CNMSNULL ){
		DBGMSG( "[Preview_Open]Can't look up widget(main_preview)!\n" );
		goto	EXIT;
	}
	else if( ( ldata = PrevSub_Open( &( lpPrevComp->subComp ) ) ) != CNMS_NO_ERR ){
		DBGMSG( "[Preview_Open]Error is occured in PrevSub_Open!\n" );
		goto	EXIT;
	}

	ret = CNMS_NO_ERR;
EXIT:
	if( ret != CNMS_NO_ERR ){
		Preview_Close();
	}
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_Open()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSVoid Preview_Close( CNMSVoid )
{
	if( lpPrevComp == CNMSNULL ){
		goto	EXIT;
	}
	
	if( lpPrevComp->lpImage != CNMSNULL ){
		CnmsFreeMem( (CNMSInt8 *)lpPrevComp->lpImage );
		lpPrevComp->lpImage = CNMSNULL;
	}
	/* GtkPreview -> GtkDrawingArea */
	if( lpPrevComp->lpBuf != CNMSNULL ){
		CnmsFreeMem( (CNMSInt8 *)lpPrevComp->lpBuf );
		lpPrevComp->lpBuf = CNMSNULL;
	}
	PrevSub_Close( &( lpPrevComp->subComp ) );

	CnmsFreeMem( (CNMSLPSTR)lpPrevComp );
	lpPrevComp = CNMSNULL;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_Close()].\n" );
#endif
	return;
}

CNMSInt32 Preview_Init(
		CNMSInt32		source )
{
	CNMSInt32		ret = CNMS_ERR, i, status;

	if( lpPrevComp == CNMSNULL ){
		DBGMSG( "[Preview_Init]Status is error!\n" );
		goto	EXIT;
	}
	/* get min/max size */
	else if( ( lpPrevComp->realRes = CnmsScanGetRealMinMaxSize( source, lpPrevComp->minSize, lpPrevComp->maxSize ) ) <= 0 ){
		DBGMSG( "[Preview_Init]Error is occured in CnmsScanGetRealMinMaxSize!\n" );
		goto	EXIT;
	}
	/* get preview res */
	else if( ( lpPrevComp->prevRes = CnmsScanGetPreviewResolution( source ) ) <= 0 ){
		DBGMSG( "[Preview_Init]Error is occured in CnmsScanGetPreviewResolution!\n" );
		goto	EXIT;
	}
	else if( lpPrevComp->realRes < lpPrevComp->prevRes ){
		DBGMSG( "[Preview_Init]Resolution is error( minmax:%d < prev:%d )!\n", lpPrevComp->realRes, lpPrevComp->prevRes );
		goto	EXIT;
	}
	
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		/* set preview image size */
		lpPrevComp->imageSize[ i ] = (CNMSInt32)( ( (CNMSDec32)( lpPrevComp->maxSize[ i ] ) * (CNMSDec32)( lpPrevComp->prevRes ) ) / (CNMSDec32)( lpPrevComp->realRes ) );
		/* set platen size */
		lpPrevComp->platenSize[ i ] = lpPrevComp->maxSize[ i ];
	}
	if( ( status = PrevSub_GetStatus( &( lpPrevComp->subComp ) ) ) == CNMS_ERR ){
		DBGMSG( "[Preview_Init]Error is occured in PrevSub_GetStatus!\n" );
		goto	EXIT;
	}
	if( status  >= PREVSUB_STATUS_CROP_WRITE ){
		/* Clear old rect( if exist ) */
		DrawRectInvert();
	}
	if( ( i = PrevSub_Init( lpPrevComp->minSize, lpPrevComp->maxSize, &( lpPrevComp->subComp ) ) ) != CNMS_NO_ERR ){
		DBGMSG( "[Preview_Init]Error is occured in PrevSub_Init!\n" );
		goto	EXIT;
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_Init(source:%d)]=%d.\n", source, ret );
#endif
	return	ret;
}

CNMSInt32 Preview_Reset(
		CNMSInt32		type )
{
	CNMSInt32		ret = CNMS_ERR, i, ldata, status;

	/* check and init */
	if( lpPrevComp == CNMSNULL ){
		DBGMSG( "[Preview_Reset]Status is error!\n" );
		goto	EXIT;
	}
	/* reset image */
	if ( ( type & PREVIEW_RESET_IMAGE ) != 0 ) {
		if( ( ldata = Preview_ChangeStatus( CNMSSCPROC_ACTION_PREV_CLEAR ) ) != CNMS_NO_ERR ){
			DBGMSG( "[Preview_Reset]Error is occured in Preview_ChangeStatus!\n" );
			goto	EXIT;
		}
		else if( ( ldata = W1Ui_ChangeClearPrevButtonVisible() ) != CNMS_NO_ERR ){
			DBGMSG( "[Preview_Reset]Error is occured in W1Ui_ChangeClearPrevButtonVisible.\n" );
			goto	EXIT;
		}
	}
	/* reset rect */
	if( ( type & PREVIEW_RESET_RECT ) != 0 ){
		if( lpPrevComp->scale == 0 ){
			DBGMSG( "[Preview_Reset]Scale is error!\n" );
			goto	EXIT;
		}
		else if( ( status = PrevSub_GetStatus( &( lpPrevComp->subComp ) ) ) == CNMS_ERR ){
			DBGMSG( "[Preview_Reset]Error is occured in PrevSub_GetStatus!\n" );
			goto	EXIT;
		}
		/* Clear old rect( if exist ) */
		if( status >= PREVSUB_STATUS_CROP_WRITE ){
			DrawRectInvert();
		}
		if( ( ldata = PrevSub_Reset( &( lpPrevComp->subComp ) ) ) != CNMS_NO_ERR ){
			DBGMSG( "[Preview_Reset]Error is occured in PrevSub_Reset!\n" );
			goto	EXIT;
		}
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_Reset(type:%d)]=%d.\n", type, ret );
#endif
	return	ret;
}

CNMSVoid Preview_ResetMaxSize( CNMSInt32 *lpMax )
{
	lpPrevComp->maxSize[ CNMS_DIM_H ] = lpMax[ CNMS_DIM_H ];
	lpPrevComp->maxSize[ CNMS_DIM_V ] = lpMax[ CNMS_DIM_V ];
	lpPrevComp->subComp.maxSize[ CNMS_DIM_H ] = lpMax[ CNMS_DIM_H ];
	lpPrevComp->subComp.maxSize[ CNMS_DIM_V ] = lpMax[ CNMS_DIM_V ];
}


static CNMSInt32 DrawRectInvert( CNMSVoid )
{
	CNMSInt32		ret = CNMS_ERR, i, topPos[ CNMS_DIM_MAX ], bottomPos[ CNMS_DIM_MAX ], min, max;
	LPPREVSUBCOMP	lpSub;
	GdkGC			*gc = NULL;
	GdkRectangle	update_rect;

	/* check */
	if( lpPrevComp == CNMSNULL ){
		DBGMSG( "[Preview_Reset]Status is error!\n" );
		goto	EXIT;
	}
	lpSub = &( lpPrevComp->subComp );

	if( ( lpSub->topPos[ CNMS_DIM_H ] == lpSub->bottomPos[ CNMS_DIM_H ] ) || ( lpSub->topPos[ CNMS_DIM_V ] == lpSub->bottomPos[ CNMS_DIM_V ] ) ){
		goto	EXIT;
	}
	CnmsConvRelativePos( lpSub->topPos, topPos, lpPrevComp->scale );
	CnmsConvRelativePos( lpSub->bottomPos, bottomPos, lpPrevComp->scale );
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		min = CNMS_GET_MIN_2( topPos[ i ], bottomPos[ i ] );
		max = CNMS_GET_MAX_2( topPos[ i ], bottomPos[ i ] );
		topPos[ i ] = min;
		bottomPos[ i ] = max;
	}

	gc = gdk_gc_new( lpPrevComp->main_widget->window );
	gdk_gc_set_function( gc, GDK_INVERT );

	update_rect.x = (gdouble)( topPos[ CNMS_DIM_H ] );
	update_rect.y = (gdouble)( topPos[ CNMS_DIM_V ] );
	update_rect.width  = (gdouble)( bottomPos[ CNMS_DIM_H ] - topPos[ CNMS_DIM_H ] - 1 );
	update_rect.height = (gdouble)( bottomPos[ CNMS_DIM_V ] - topPos[ CNMS_DIM_V ] - 1 );
	gdk_gc_set_line_attributes( gc, 1, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_BEVEL );
	gdk_draw_rectangle( lpPrevComp->prev_widget->window, gc, FALSE, update_rect.x, update_rect.y, update_rect.width, update_rect.height );
	gdk_gc_unref( gc );

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_DETAIL__
	DBGMSG( "[DrawRectInvert()]=%d.\n", ret );
#endif
	return	ret;
}

static CNMSInt32 ret_Preview_ChangeStatus_Exec = CNMS_ERR;
static CNMSInt32 mode_Preview_ChangeStatus_Exec;

gint Preview_ChangeStatus_Exec(
		CNMSVoid )
{
	CNMSInt32		Actionmode = mode_Preview_ChangeStatus_Exec;
	CNMSInt32		ret = CNMS_ERR, ldata;
	CNMSByte		tmpPath[ PATH_MAX ];
	CNMSFd			tmpFd = CNMS_FILE_ERR;
	CNMSUIReturn	uiRtn;

	if( lpPrevComp == CNMSNULL ){
		DBGMSG( "[Preview_ChangeStatus_Exec]Status is error!\n" );
		goto	EXIT;
	}
	/* make temp file */
	if( ( tmpFd = FileControlMakeTempFile( tmpPath, PATH_MAX ) ) == CNMS_FILE_ERR ){
		DBGMSG( "[Preview_ChangeStatus_Exec]Error is occured in FileControlMakeTempFile!\n" );
		goto	EXIT;
	}
	else if( ( ldata = CnmsScanChangeStatus( Actionmode, tmpFd, CNMSNULL, &uiRtn ) ) != CNMS_NO_ERR ){
		DBGMSG( "[Preview_ChangeStatus_Exec]Error is occured in CnmsScanChangeStatus!\n" );
		goto	EXIT;
	}
	if( ( uiRtn.ResultImgSize.Width != 0 ) && ( uiRtn.ResultImgSize.Height != 0 ) ){
		/* rewrite */
		lpPrevComp->imageSize[ CNMS_DIM_H ] = (CNMSInt32)( uiRtn.ResultImgSize.Width );
		lpPrevComp->imageSize[ CNMS_DIM_V ] = (CNMSInt32)( uiRtn.ResultImgSize.Height );
		ldata = uiRtn.ResultImgSize.Width * uiRtn.ResultImgSize.Height * 3;
		/* get memory again */
		if( lpPrevComp->memSize < ldata ){
			lpPrevComp->memSize = ldata;
			if( lpPrevComp->lpImage != CNMSNULL ){
				CnmsFreeMem( lpPrevComp->lpImage );
			}
			if( ( lpPrevComp->lpImage = CnmsGetMem( lpPrevComp->memSize ) ) == CNMSNULL ){
				DBGMSG( "[Preview_ChangeStatus_Exec]Can't get image memory!\n" );
				goto	EXIT;
			}
		}
		if( ( ldata = FileControlSeekFile( tmpFd, 0, FILECONTROL_SEEK_FROM_TOP ) ) < 0 ) {
			DBGMSG( "[Preview_ChangeStatus_Exec]Error is occured in FileControlSeekFile!\n" );
			goto	EXIT;
		}
		if( ( ldata = FileControlReadFile( tmpFd, lpPrevComp->lpImage, lpPrevComp->memSize ) ) == CNMS_ERR ) {
			DBGMSG( "[Preview_ChangeStatus_Exec]Error is occured in FileControlReadFile!\n" );
			goto	EXIT;
		}
		/* write preview data */
		if( ( ldata = SubPrevResize( CNMS_TRUE ) ) != CNMS_NO_ERR ){
			DBGMSG( "[Preview_ChangeStatus_Exec]Error is occured in SubPrevResize!\n" );
			goto	EXIT;
		}
	}

	ret = CNMS_NO_ERR;
EXIT:
	if( tmpFd != CNMS_FILE_ERR ){
		/* delete tmp file */
		FileControlDeleteFile( tmpPath, tmpFd );
	}
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_ChangeStatus_Exec()].\n" );
#endif

	ret_Preview_ChangeStatus_Exec = ret;
	
	return 0;
}

#define CNMS_RELOAD_TIME	(200)

CNMSInt32 Preview_ChangeStatus(
		CNMSInt32		Actionmode )
{
	static	CNMSInt32	time_current	= CNMS_ERR;
	static	CNMSInt32 	time_before		= CNMS_ERR;

	mode_Preview_ChangeStatus_Exec = Actionmode;
	ret_Preview_ChangeStatus_Exec = CNMS_NO_ERR;
	
	if ( Actionmode == CNMSSCPROC_ACTION_PREV_CLEAR || Actionmode < CNMSSCPROC_ACTION_PARAM ) {
		Preview_ChangeStatus_Exec();
	}
	else {
		time_current = gtk_timeout_add( CNMS_RELOAD_TIME, ( GtkFunction )Preview_ChangeStatus_Exec, CNMSNULL );
		gtk_timeout_remove( time_before );
		time_before = time_current;
	}

#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_ChangeStatus(Actionmode:%d)]=%d.\n", Actionmode, ret_Preview_ChangeStatus_Exec );
#endif
	return ret_Preview_ChangeStatus_Exec;
}

static CNMSInt32 SubPrevResize( CNMSBool prevFlag )
{
	CNMSInt32		ret = CNMS_ERR, i, ldata, scale, dstSize[ CNMS_DIM_MAX ], areaSize[ CNMS_DIM_MAX ];
	CNMSUInt8		*lpDst = CNMSNULL, *lpDstTemp, *lpArea;
	CNMSInt32		dstOffset, areaOffset, outWidth;

	if( lpPrevComp == CNMSNULL ){
		DBGMSG( "[SubPrevResize]Status is error!\n" );
		goto	EXIT;
	}
	areaSize[ CNMS_DIM_H ] = (CNMSInt32)( lpPrevComp->prev_widget->allocation.width );
	areaSize[ CNMS_DIM_V ] = (CNMSInt32)( lpPrevComp->prev_widget->allocation.height );
	
	if( !prevFlag	&& areaSize[ CNMS_DIM_H ] == lpPrevComp->pre_areaSize[ CNMS_DIM_H ] 
					&& areaSize[ CNMS_DIM_V ] == lpPrevComp->pre_areaSize[ CNMS_DIM_V ] ){
		ret = CNMS_NO_ERR;
		goto	EXIT;
	}
	
	/* calc scale */
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		dstSize[ i ] = lpPrevComp->platenSize[ i ];				/* platen size */
	}
	lpPrevComp->scale = CnmsFitSizeKeepAspectRatio( CNMS_FIT_INSIDE, areaSize, dstSize );	/* fit image inside area */

	/* init */
	if ( lpPrevComp->lpBuf ) CnmsFreeMem( (CNMSLPSTR)lpPrevComp->lpBuf );
	if( ( lpPrevComp->lpBuf = (CNMSUInt8 *)CnmsGetMem( areaSize[ CNMS_DIM_H ] * areaSize[ CNMS_DIM_V ] * 3 ) ) == CNMSNULL ){
		DBGMSG( "[SubPrevResize]Can't get memory.\n" );
		goto	EXIT;
	}
	if( ( lpDst = (CNMSUInt8 *)CnmsGetMem( dstSize[ CNMS_DIM_H ] * dstSize[ CNMS_DIM_V ] * 3 ) ) == CNMSNULL ){
		DBGMSG( "[SubPrevResize]Can't get memory.\n" );
		goto	EXIT;
	}
	lpPrevComp->areaSize[ CNMS_DIM_H ] = lpPrevComp->pre_areaSize[ CNMS_DIM_H ] = areaSize[ CNMS_DIM_H ];
	lpPrevComp->areaSize[ CNMS_DIM_V ] = lpPrevComp->pre_areaSize[ CNMS_DIM_V ] = areaSize[ CNMS_DIM_V ];

	if( ( ldata = lpCnmsImgApi->p_CnmsImgScaleFromMemory( ( CNMSUInt8* )lpPrevComp->lpImage, lpDst, 24, (CNMSInt32 *)( lpPrevComp->imageSize ), (CNMSInt32 *)dstSize ) ) != CNMS_NO_ERR ){
		DBGMSG( "[SubPrevResize]Can't change scale.\n" );
		goto	EXIT;
	}
	if( lpPrevComp->gc == CNMSNULL ) {
		lpPrevComp->gc = gdk_gc_new( lpPrevComp->prev_widget->window );
	}
	dstOffset = dstSize[ CNMS_DIM_H ] * 3;
	areaOffset = areaSize[ CNMS_DIM_H ] * 3;
	outWidth = areaOffset - dstOffset;

	/* copy preview data */
	for( i = 0, lpDstTemp = lpDst, lpArea = lpPrevComp->lpBuf ; i < dstSize[ CNMS_DIM_V ] ; i++, lpDstTemp += dstOffset, lpArea += areaOffset ) {
		CnmsCopyMem( (CNMSLPSTR)lpDstTemp, (CNMSLPSTR)lpArea, dstOffset );
		CnmsSetMem( (CNMSLPSTR)( lpArea + dstOffset ), PREVIEW_OUT_COLOR_R, outWidth );
	}
	CnmsSetMem( (CNMSLPSTR)lpArea, PREVIEW_OUT_COLOR_R, areaOffset * ( areaSize[ CNMS_DIM_V ] - dstSize[ CNMS_DIM_V ]) );

	/* redraw preview area */
	gdk_draw_rgb_image( lpPrevComp->prev_widget->window, lpPrevComp->gc, 0, 0,
			areaSize[ CNMS_DIM_H ], areaSize[ CNMS_DIM_V ], GDK_RGB_DITHER_NONE, 
			(guchar*)lpPrevComp->lpBuf, areaSize[ CNMS_DIM_H ] * 3 );

	/* redraw rect line */
	if( ( ldata = PrevSub_GetStatus( &( lpPrevComp->subComp ) ) ) == CNMS_ERR ){
		DBGMSG( "[Preview_Size_Allocate]Error is occured in PrevSub_GetStatus!\n" );
		goto	EXIT;
	}
	if( ldata >= PREVSUB_STATUS_CROP_WRITE ){
		DrawRectInvert();
	}

	gtk_widget_queue_draw( lpPrevComp->prev_widget );

	ret = CNMS_NO_ERR;
EXIT:
	if( lpDst != CNMSNULL ){
		CnmsFreeMem( (CNMSLPSTR)lpDst );
	}
#ifdef	__CNMS_DEBUG_PREVIEW_DETAIL__
	DBGMSG( "[SubPrevResize(area:%dx%d)]=%d.\n", areaSize[ CNMS_DIM_H ], areaSize[ CNMS_DIM_V ], ret );
#endif
	return	ret;
}

CNMSInt32 Preview_Configure_Event( CNMSVoid  )
{
	CNMSInt32	ret = CNMS_ERR, ldata;

	/* check */
	if( ( lpPrevComp == CNMSNULL ) || ( lpPrevComp->lpImage == CNMSNULL ) ){	/* no error message */
		goto	EXIT;
	}

	/* Resize */
	if( ( ldata = SubPrevResize( CNMS_FALSE ) ) != CNMS_NO_ERR ){
		DBGMSG( "[Preview_Size_Allocate]Error is occured in SubPrevResize!\n" );
		goto	EXIT;
	}

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_Size_Allocate()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSVoid Preview_Expose_Event( CNMSVoid )
{
	CNMSInt32		ldata;
	
	if( lpPrevComp != CNMSNULL ){
		/* redraw preview area */
		if( lpPrevComp->lpBuf && lpPrevComp->gc ) {
			gdk_draw_rgb_image( lpPrevComp->prev_widget->window, lpPrevComp->gc, 0, 0,
					lpPrevComp->areaSize[ CNMS_DIM_H ], lpPrevComp->areaSize[ CNMS_DIM_V ], GDK_RGB_DITHER_NONE, 
					(guchar*)lpPrevComp->lpBuf, lpPrevComp->areaSize[ CNMS_DIM_H ] * 3 );
		}
		if( ( ldata = PrevSub_GetStatus( &( lpPrevComp->subComp ) ) ) == CNMS_ERR ){
			DBGMSG( "[Preview_Expose_Event_After]Error is occured in PrevSub_GetStatus!\n" );
			return;
		}
		if( ldata >= PREVSUB_STATUS_CROP_WRITE ){
			DrawRectInvert();
		}
	}
}

CNMSInt32 Preview_GetMinMaxSize(
		CNMSInt32		*lpMinSize,
		CNMSInt32		*lpMaxSize )
{
	CNMSInt32		ret = CNMS_ERR, ldata;

	/* check */
	if( lpPrevComp == CNMSNULL ){
		DBGMSG( "[Preview_GetMinMaxSize]Status is error!\n" );
		goto	EXIT;
	}
	else if( ( ldata = PrevSub_GetMinMaxSize( lpMinSize, lpMaxSize, &( lpPrevComp->subComp ) ) ) != CNMS_NO_ERR ){
		DBGMSG( "[Preview_GetMinMaxSize]Error is occured in PrevSub_GetMinMaxSize!\n" );
		goto	EXIT;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_GetMinMaxSize()]=%d, min:%dx%d, max:%dx%d.\n", ret, lpMinSize[ CNMS_DIM_H ], lpMinSize[ CNMS_DIM_V ], lpMaxSize[ CNMS_DIM_H ], lpMaxSize[ CNMS_DIM_V ] );
#endif
	return	ret;
}

CNMSInt32 Preview_GetNewInOutSize(
		CNMSInt32		*lpNewSize,
		CNMSInt32		*lpInSize,
		CNMSInt32		*lpOutSize,
		CNMSInt32		*lpScale )
{
	CNMSInt32		ret = CNMS_ERR, i, ldata, scale, scale2, rectSize[ CNMS_DIM_MAX ], inSize[ CNMS_DIM_MAX ];
	CNMSBool		rectKey;

	if( lpPrevComp == CNMSNULL ){
		DBGMSG( "[Preview_GetNewInOutSize]Status is error!\n" );
		goto	EXIT;
	}
	else if( ( lpNewSize == CNMSNULL ) || ( lpInSize == CNMSNULL ) || ( lpOutSize == CNMSNULL ) || ( lpScale == CNMSNULL ) ){
		DBGMSG( "[Preview_GetNewInOutSize]Parameter is error!\n" );
		goto	EXIT;
	}
	else if( ( ldata = PrevSub_RectGet( rectSize, &( lpPrevComp->subComp ) ) ) != CNMS_NO_ERR ){
		DBGMSG( "[Preview_GetNewInOutSize]Error is occured in PrevSub_RectGet!\n" );
		goto	EXIT;
	}
	rectKey = ( rectSize[ CNMS_DIM_H ] == 0 ) ? CNMS_FALSE : CNMS_TRUE;

	scale = *lpScale;
	/*** change to Free Size ***/
	if( lpNewSize[ CNMS_DIM_H ] == 0 ){
		for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
			lpOutSize[ i ] = lpInSize[ i ] = ( rectKey == CNMS_FALSE ) ? lpPrevComp->maxSize[ i ] : rectSize[ i ];
		}
		scale = CNMS_SCALE_100_PERCENT;
		PrevSub_SetFixedSize( CNMSNULL, &( lpPrevComp->subComp ) );
	}
	/*** change to Fixed Size ***/
	else{
		for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
			/* set output size */
			lpOutSize[ i ] = lpNewSize[ i ];
			inSize[ i ] = lpNewSize[ i ] * CNMS_SCALE_100_PERCENT / scale;
		}
		if( rectKey == CNMS_TRUE ){	/* Area is already selected */
			CnmsFitSizeKeepAspectRatio( CNMS_FIT_INSIDE, rectSize, inSize );
			scale = CnmsGetFitScale( CNMS_FIT_INSIDE, lpOutSize, inSize );
		}
		else{	/* No rect area -> check over maxSize */
			if( ( scale2 = CnmsGetFitScale( CNMS_FIT_INSIDE, lpPrevComp->maxSize, inSize ) ) < CNMS_SCALE_100_PERCENT ){
				CnmsFitSizeKeepAspectRatio( CNMS_FIT_INSIDE, lpPrevComp->maxSize, inSize );
				scale = CnmsGetFitScale( CNMS_FIT_INSIDE, lpOutSize, inSize );
			}
		}
		/* check under minSize */
		if( ( inSize[ CNMS_DIM_H ] < lpPrevComp->minSize[ CNMS_DIM_H ] ) || ( inSize[ CNMS_DIM_V ] < lpPrevComp->minSize[ CNMS_DIM_V ] ) ){
			CnmsFitSizeKeepAspectRatio( CNMS_FIT_OUTSIDE, lpPrevComp->minSize, inSize );
			scale = CnmsGetFitScale( CNMS_FIT_INSIDE, lpOutSize, inSize );
		}
		/* set input size */
		for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
			lpInSize[ i ] = ( lpOutSize[ i ] * CNMS_SCALE_100_PERCENT ) / scale;
			/* check over maxSize */
			if( lpInSize[ i ] > lpPrevComp->maxSize[ i ] ) {
				lpInSize[ i ] = lpPrevComp->maxSize[ i ];
			}
		}
		PrevSub_SetFixedSize( lpInSize, &( lpPrevComp->subComp ) );
	}
	*lpScale = scale;
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_GetNewInOutSize(newSize:%dx%d)]=%d.inSize:%dx%d, outSize:%dx%d, scale:%d.\n", lpNewSize[ CNMS_DIM_H ], lpNewSize[ CNMS_DIM_V ], ret, lpInSize[ CNMS_DIM_H ], lpInSize[ CNMS_DIM_V ], lpOutSize[ CNMS_DIM_H ], lpOutSize[ CNMS_DIM_V ], scale );
#endif
	return	ret;
}

CNMSInt32 Preview_RectSet(
		CNMSInt32		*lpOffset,
		CNMSInt32		*lpInSize )
{
	CNMSInt32		ret = CNMS_ERR, ldata;
	LPPREVSUBCOMP	lpSub;

	if( ( lpPrevComp == CNMSNULL ) || ( lpPrevComp->lpImage == CNMSNULL ) ){
		DBGMSG( "[Preview_RectSet]Status is error!\n" );
		goto	EXIT;
	}
	else if( lpInSize == CNMSNULL ){
		DBGMSG( "[Preview_RectSet]Parameter is error!\n" );
		goto	EXIT;
	}
	lpSub = &( lpPrevComp->subComp );
	/* Erase old rect ( if exist ) */
	if( ( ldata = PrevSub_GetStatus( &( lpPrevComp->subComp ) ) ) >= PREVSUB_STATUS_CROP_WRITE ){
		if( ( ldata = DrawRectInvert() ) != CNMS_NO_ERR ){
			DBGMSG( "[Preview_RectSet]Error is occured in DrawRectInvert(erase)!\n" );
			goto	EXIT;
		}
	}
	if( ( ldata = PrevSub_RectSet( lpOffset, lpInSize, &( lpPrevComp->subComp ) ) ) != CNMS_NO_ERR ){
		DBGMSG( "[Preview_RectSet]Error is occured in PrevSub_RectSet!\n" );
		goto	EXIT;
	}
	if( ( ldata = DrawRectInvert() ) != CNMS_NO_ERR ){
		DBGMSG( "[Preview_RectSet]Error is occured in DrawRectInvert(write)!\n" );
		goto	EXIT;
	}

	gtk_widget_queue_draw( lpPrevComp->prev_widget );
	
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_RectSet(offset:%dx%d, size:%dx%d)]=%d.\n", lpOffset[ CNMS_DIM_H ], lpOffset[ CNMS_DIM_V ], lpInSize[ CNMS_DIM_H ], lpInSize[ CNMS_DIM_V ], ret );
#endif
	return	ret;
}

CNMSInt32 Preview_RectGet(
		CNMSInt32		*lpSize )
{
	CNMSInt32	ret = CNMS_ERR, ldata;

	if( lpPrevComp == CNMSNULL ){
		DBGMSG( "[Preview_RectGet]Status is error!\n" );
		goto	EXIT;
	}
	else if( ( ldata = PrevSub_RectGet( lpSize, &( lpPrevComp->subComp ) ) ) != CNMS_NO_ERR ){
		goto	EXIT;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_RectGet()]=%d, rect:%dx%d.\n", lpSize[ CNMS_DIM_H ], lpSize[ CNMS_DIM_V ], ret );
#endif
	return	ret;
}

CNMSInt32 Preview_GetColor(
		CNMSInt32			*lpCurPosR,	/* Relative Preview Area */
		CNMSInt32			*lpR,
		CNMSInt32			*lpG,
		CNMSInt32			*lpB )
{
	CNMSInt32	ret = CNMS_ERR, ldata, i, curPos[ CNMS_DIM_MAX ];

	if( ( lpPrevComp == CNMSNULL ) || ( lpCurPosR == CNMSNULL ) || ( lpR == CNMSNULL ) || ( lpG == CNMSNULL ) || ( lpB == CNMSNULL ) ){
		DBGMSG( "[Preview_GetColor]Parameter is error!\n" );
		goto	EXIT;
	}
	if( ( ldata = PrevSub_GetStatus( &( lpPrevComp->subComp ) ) ) == CNMS_ERR ){
		DBGMSG( "[Preview_GetColor]Error is occured in PrevSub_GetStatus!\n" );
		goto	EXIT;
	}
	if( ( ldata != PREVSUB_STATUS_CROP_NONE ) && ( ldata != PREVSUB_STATUS_CROP_END ) ){
		goto	EXIT;
	}
	CnmsConvAbsolutePos( lpCurPosR, curPos, lpPrevComp->scale );

	if( ( ldata = PrevSub_GetCursorRectId( CNMS_TRUE, CNMS_TRUE, lpPrevComp->scale, curPos, &( lpPrevComp->subComp ) ) ) != CNMS_NO_ERR ){
		DBGMSG( "[Preview_GetColor]Error is occured in PrevSub_GetCurId!\n" );
		goto	EXIT;
	}
	else if( ( PREVSUB_CURSOR_OUTSIDE <= lpPrevComp->subComp.clickId[ CNMS_DIM_H ] ) || ( PREVSUB_CURSOR_OUTSIDE <= lpPrevComp->subComp.clickId[ CNMS_DIM_V ] ) ){
		/* no error message */
		goto	EXIT;
	}

	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		ldata = (CNMSInt32)( (CNMSDec32)( curPos[ i ] ) * (CNMSDec32)( lpPrevComp->prevRes ) / (CNMSDec32)( lpPrevComp->realRes ) );
		curPos[ i ] = ( lpPrevComp->imageSize[ i ] <= ldata ) ? ( lpPrevComp->imageSize[ i ] - 1 ) : ldata;
	}

	ret = CnmsScanFlowGetOriginalRGB( curPos, lpR+1, lpG+1, lpB+1 );
	if( ret == CNMS_NO_ERR ){
	
		ldata = ( lpPrevComp->imageSize[ CNMS_DIM_H ] * curPos[ CNMS_DIM_V ] + curPos[ CNMS_DIM_H ] ) * 3;
	
		*lpR = (CNMSInt32)( *( lpPrevComp->lpImage + ldata     ) ) & 0xFF;
		*lpG = (CNMSInt32)( *( lpPrevComp->lpImage + ldata + 1 ) ) & 0xFF;
		*lpB = (CNMSInt32)( *( lpPrevComp->lpImage + ldata + 2 ) ) & 0xFF;
		
#ifdef	__CNMS_DEBUG_PREVIEW__
		DBGMSG("Original (%3d,%3d,%3d) ", *(lpR+1), *(lpG+1), *(lpB+1));
		DBGMSG("Current (%3d,%3d,%3d)\n", *(lpR+0), *(lpG+0), *(lpB+0));
#endif
	}
	else {
		*(lpR+1) = *(lpG+1) = *(lpB+1) = *lpR = *lpG = *lpB = -1;
//		DBGMSG("[Preview_GetColor]Error is occured in CnmsScanFlowGetOriginalRGB\n");
	}

EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_GetColor()]=%d.\n", ret );
#endif
	return	ret;
}

CNMSVoid Preview_Mouse_Button_Press(
		CNMSBool			hgFlag,
		CNMSBool			aspectFlag,
		CNMSBool			prevFlag,
		CNMSInt32			*lpCurPosR,	/* Relative Preview Area */
		GdkModifierType		state )
{
	CNMSInt32		i, ldata, curPos[ CNMS_DIM_MAX ];
	LPPREVSUBCOMP	lpSub;

	/* check */
	if( lpPrevComp == CNMSNULL ){
		DBGMSG( "[Preview_Mouse_Button_Press]Status is error!\n" );
		goto	EXIT;
	}
	CnmsConvAbsolutePos( lpCurPosR, curPos, lpPrevComp->scale );

	lpSub = &( lpPrevComp->subComp );

	/* left button is clicked */
	if( ( state & GDK_BUTTON1_MASK ) != 0 ){
		/* Spuit Tool ON & Cursor Inside Crop Area */
		if( hgFlag == CNMS_TRUE ){
			SubClickHg( prevFlag, lpPrevComp->scale, curPos, lpSub );
			goto	EXIT;
		}
		/* Clear Rect */
		if( PREVSUB_STATUS_CROP_WRITE <= lpSub->status ){
			if( ( ldata = DrawRectInvert() ) != CNMS_NO_ERR ){
				DBGMSG( "[Preview_Mouse_Button_Press]Error is occured in DrawRectInvert(erase)!\n" );
				goto	EXIT;
			}
		}
		if( ( ldata = PrevSub_ClickLeft( hgFlag, prevFlag, lpPrevComp->scale, curPos, lpSub ) ) != CNMS_NO_ERR ){
			DBGMSG( "[Preview_Mouse_Button_Press]Error is occured in PrevSub_ClickLeft!\n" );
			goto	EXIT;
		}
		if( lpSub->status == PREVSUB_STATUS_CROP_NONE ){
			/* Outside platen -> Reset rect area (call scanflow) */
			if( ( ldata = Preview_ChangeStatus( CNMSSCPROC_ACTION_CROP_CORRECT ) ) != CNMS_NO_ERR ){
				DBGMSG( "[Preview_Mouse_Button_Press]Error is occured in Preview_ChangeStatus( CNMSSCPROC_ACTION_PREV_CLEAR )!\n" );
				goto	EXIT;
			}
		}
		else{
			if( PREVSUB_STATUS_CROP_WRITE <= lpSub->status ){
				if( ( ldata = DrawRectInvert() ) != CNMS_NO_ERR ){
					DBGMSG( "[Preview_Mouse_Button_Press]Error is occured in DrawRectInvert(write)!\n" );
					goto	EXIT;
				}
			}
			lpPrevComp->clickedButton = PREVIEW_MOUSE_CLICKED_LEFT;
		}
	}
	/* right button is clicked */
	if( ( state & GDK_BUTTON3_MASK ) != 0 ){
		lpPrevComp->clickedButton = PREVIEW_MOUSE_CLICKED_RIGHT;
		if( PREVSUB_STATUS_CROP_WRITE <= lpSub->status ){
			if( ( ldata = DrawRectInvert() ) != CNMS_NO_ERR ){
				DBGMSG( "[Preview_Mouse_Button_Press]Error is occured in DrawRectInvert(erase)!\n" );
				goto	EXIT;
			}
		}
		if( ( ldata = PrevSub_ClickRight( lpSub ) ) != CNMS_NO_ERR ){
			DBGMSG( "[Preview_Mouse_Button_Press]Error is occured in PrevSub_ClickRight!\n" );
			goto	EXIT;
		}
		if( ( ldata = Preview_ChangeStatus( CNMSSCPROC_ACTION_CROP_CORRECT ) ) != CNMS_NO_ERR ){
			DBGMSG( "[Preview_Mouse_Button_Press]Error is occured in Preview_ChangeStatus( CNMSSCPROC_ACTION_CROP_CORRECT )!\n" );
			goto	EXIT;
		}
	}
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_Mouse_Button_Press(hgFlag:%d, aspectFlag:%d, curPosR:%dx%d)].\n", hgFlag, aspectFlag, lpCurPosR[ CNMS_DIM_H ], lpCurPosR[ CNMS_DIM_V ] );
#endif
	return;
}

static CNMSVoid SubClickHg(
		CNMSBool		prevFlag,
		CNMSInt32		scale,
		CNMSInt32		*lpCurPos,
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32	ldata;

	if( ( ldata = PrevSub_GetCursorRectId( CNMS_TRUE, prevFlag, scale, lpCurPos, lpSub ) ) != CNMS_NO_ERR ){
		DBGMSG( "[SubClickHg]Error is occured in PrevSub_GetCursorRectId!\n" );
		goto	EXIT;
	}
	if( ( lpSub->clickId[ CNMS_DIM_H ] == PREVSUB_CURSOR_INSIDE ) && ( lpSub->clickId[ CNMS_DIM_V ] == PREVSUB_CURSOR_INSIDE ) ){
		CnmsHGSpuitToolAction();
	}
EXIT:
	return;
}

CNMSVoid Preview_Motion(
		CNMSBool			hgFlag,
		CNMSBool			prevFlag,
		CNMSInt32			*lpCurPosR,	/* Relative Preview Area */
		GdkModifierType		state )
{
	CNMSInt32		ldata, curPos[ CNMS_DIM_MAX ];
	LPPREVSUBCOMP	lpSub;

	/* check */
	if( lpPrevComp == CNMSNULL ){
		DBGMSG( "[Preview_Motion]Status is error!\n" );
		goto	EXIT;
	}
	else if( lpCurPosR == CNMSNULL ){
		DBGMSG( "[Preview_Motion]Parameter is error!\n" );
		goto	EXIT;
	}
	CnmsConvAbsolutePos( lpCurPosR, curPos, lpPrevComp->scale );
	lpSub = &( lpPrevComp->subComp );

	if( ( state & GDK_BUTTON1_MASK ) != 0 ){
		/* check */
		if( lpPrevComp->clickedButton != PREVIEW_MOUSE_CLICKED_LEFT ){
			goto	EXIT;
		}
		/* Erase Rect Line */
		if( PREVSUB_STATUS_CROP_WRITE <= lpSub->status ){
			if( ( ldata = DrawRectInvert() ) != CNMS_NO_ERR ){
				DBGMSG( "[Preview_Motion]Error is occured in DrawRectInvert(erase)!\n" );
				goto	EXIT;
			}
		}
		if( ( ldata = PrevSub_Motion( curPos, lpSub ) ) != CNMS_NO_ERR ){
			DBGMSG( "[Preview_Motion]Error is occured in PrevSub_Motion!\n" );
			goto	EXIT;
		}
		/* Write Rect */
		if( ( ldata = DrawRectInvert() ) != CNMS_NO_ERR ){
			DBGMSG( "[Preview_Motion]Error is occured in DrawRectInvert(write)!\n" );
			goto	EXIT;
		}
		/* show size to spinbox */
		if( ( ldata = SubSetShowSizeToSpin( lpSub ) ) != CNMS_NO_ERR ){
			DBGMSG( "[Preview_Motion]Error is occured in SubSetShowSizeToSpin!\n" );
			goto	EXIT;
		}
	}
	else if( state == 0 ){	/* clicked button is none */
		ldata = PrevSub_GetCursorIndex( hgFlag, prevFlag, lpPrevComp->scale, curPos, lpSub );
		ComGtk_CursorImageSet( lpPrevComp->prev_widget, ldata );
	}
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_CURSOR__
	DBGMSG( "[Preview_Motion(hgFlag:%d, state:%d, curPosR:%dx%d)].\n", hgFlag, state, lpCurPosR[ CNMS_DIM_H ], lpCurPosR[ CNMS_DIM_V ] );
#endif
	return;
}

static CNMSInt32 SubSetShowSizeToSpin(
		LPPREVSUBCOMP	lpSub )
{
	CNMSInt32		ret = CNMS_ERR, i, Offset[ CNMS_DIM_MAX ], Size[ CNMS_DIM_MAX ];

	if( lpSub->status < PREVSUB_STATUS_CROP_WRITE ){
		DBGMSG( "[SubSetShowSizeToSpin]Status is error!\n" );
		goto	EXIT;
	}
	/* show ui size */
	for( i = 0 ; i < CNMS_DIM_MAX ; i ++ ){
		Offset[ i ] = lpSub->topPos[ i ];
		Size[ i ]   = lpSub->bottomPos[ i ] - lpSub->topPos[ i ] + 1;
	}
	W1Size_ChangeRectAreaAfter( Size, Offset );
	W1Ui_ShowSize();

	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW_CURSOR__
	DBGMSG( "[SubSetShowSizeToSpin(motionType:%d)]=%d.\n", motionType, ret );
#endif
	return	ret;
}

CNMSVoid Preview_Mouse_Button_Release(
		CNMSBool			hgFlag,
		CNMSBool			prevFlag,
		CNMSInt32			*lpCurPosR,	/* Relative Preview Area */
		GdkModifierType		state )
{
	CNMSInt32		ldata, lastStatus, curPos[ CNMS_DIM_MAX ];
	LPPREVSUBCOMP	lpSub;

	/* check */
	if( lpPrevComp == CNMSNULL ){
		DBGMSG( "[Preview_Mouse_Button_Release]Status is error!\n" );
		goto	EXIT;
	}
	CnmsConvAbsolutePos( lpCurPosR, curPos, lpPrevComp->scale );

	lpSub = &( lpPrevComp->subComp );
	lastStatus = lpSub->status;

	/* left button is released */
	if( ( lpPrevComp->clickedButton == PREVIEW_MOUSE_CLICKED_LEFT ) && ( ( state & GDK_BUTTON1_MASK ) == 0 ) ){
		if( PREVSUB_STATUS_CROP_WRITE <= lpSub->status ){
			if( ( ldata = DrawRectInvert() ) != CNMS_NO_ERR ){
				DBGMSG( "[Preview_Mouse_Button_Release]Error is occured in DrawRectInvert(erase)!\n" );
				goto	EXIT;
			}
		}
		if( ( ldata = PrevSub_ReleaseLeft( hgFlag, prevFlag, curPos, lpSub ) ) != CNMS_NO_ERR ){
			DBGMSG( "[Preview_Mouse_Button_Release]Error is occured in PrevSub_ReleaseLeft!\n" );
			goto	EXIT;
		}
		/* after process */
		if( PREVSUB_STATUS_CROP_NONE != lastStatus ){
			if( ( ldata = Preview_ChangeStatus( CNMSSCPROC_ACTION_CROP_CORRECT ) ) != CNMS_NO_ERR ){
				DBGMSG( "[Preview_Mouse_Button_Release]Error is occured in Preview_ChangeStatus( CNMSSCPROC_ACTION_CROP_CORRECT )!\n" );
				goto	EXIT;
			}
		}
		if( PREVSUB_STATUS_CROP_WRITE <= lpSub->status ){
			if( ( ldata = DrawRectInvert() ) != CNMS_NO_ERR ){
				DBGMSG( "[Preview_Mouse_Button_Release]Error is occured in DrawRectInvert(write)!\n" );
				goto	EXIT;
			}
		}
		/* show size to spinbox */
		if( ( ldata = SubSetShowSizeToSpin( lpSub ) ) != CNMS_NO_ERR ){
			DBGMSG( "[Preview_Mouse_Button_Release]Error is occured in SubSetShowSizeToSpin!\n" );
			goto	EXIT;
		}
		if( ( lastStatus == PREVSUB_STATUS_CROP_WRITE ) || ( lastStatus == PREVSUB_STATUS_CROP_START ) ){
			if( ( ldata = W1Ui_AspectInfoSet( CNMS_ASPECT_VARIED ) ) != CNMS_NO_ERR ){
				DBGMSG( "[Preview_Mouse_Button_Release]Error is occured in W1Ui_AspectInfoSet!\n" );
				goto	EXIT;
			}
		}
		lpPrevComp->clickedButton = PREVIEW_MOUSE_CLICKED_NONE;
	}
	/* right button is released */
	else if( ( lpPrevComp->clickedButton == PREVIEW_MOUSE_CLICKED_RIGHT ) && ( ( state & GDK_BUTTON3_MASK ) == 0 ) ){
		lpPrevComp->clickedButton = PREVIEW_MOUSE_CLICKED_NONE;
	}
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_Mouse_Button_Release(hgFlag:%d, prevFlag:%d, curPosR:%dx%d)].\n", hgFlag, prevFlag, lpCurPosR[ CNMS_DIM_H ], lpCurPosR[ CNMS_DIM_V ] );
#endif

	return;
}

CNMSVoid Preview_AspectSet(
		CNMSInt32		aspect )
{
	if( lpPrevComp == CNMSNULL ){
		goto	EXIT;
	}
	PrevSub_ChageAspect( aspect, &( lpPrevComp->subComp ) );
EXIT:
#ifdef	__CNMS_DEBUG_PREVIEW__
	DBGMSG( "[Preview_AspectSet(aspect:%d)].\n", aspect );
#endif
	return;
}

#ifdef	__CNMS_DEBUG__
CNMSVoid DebugPreviewComp( CNMSVoid )
{
	if( lpPrevComp == CNMSNULL ){
		DBGMSG( "[DebugPreviewComp]lpPrevComp == NULL\n" );
		goto	EXIT;
	}

	DBGMSG( "[DebugPreviewComp]clickedButton:%d\n", lpPrevComp->clickedButton );
	DBGMSG( "[DebugPreviewComp]realRes      :%d\n", lpPrevComp->realRes );
	DBGMSG( "[DebugPreviewComp]minSize      :%d x %d\n", lpPrevComp->minSize[ CNMS_DIM_H ], lpPrevComp->minSize[ CNMS_DIM_V ] );
	DBGMSG( "[DebugPreviewComp]maxSize      :%d x %d\n", lpPrevComp->maxSize[ CNMS_DIM_H ], lpPrevComp->maxSize[ CNMS_DIM_V ] );
	DBGMSG( "[DebugPreviewComp]previewRes   :%d\n", lpPrevComp->prevRes );
	DBGMSG( "[DebugPreviewComp]scale        :%d\n", lpPrevComp->scale );
	DBGMSG( "[DebugPreviewComp]imageSize    :%d x %d\n", lpPrevComp->imageSize[ CNMS_DIM_H ], lpPrevComp->imageSize[ CNMS_DIM_V ] );
	DBGMSG( "[DebugPreviewComp]lpImage      :%d\n", lpPrevComp->lpImage );
EXIT:
	return;
}
#endif	/* __CNMS_DEBUG__ */

#endif	/* _PREV_MAIN_C_ */

