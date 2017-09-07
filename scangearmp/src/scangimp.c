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

#ifndef	_SCANGIMP_C_
#define	_SCANGIMP_C_

#ifdef	__GIMP_PLUGIN_ENABLE__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/*
	#define	__CNMS_DEBUG__
	#define	__CNMS_DEBUG_GIMP__
*/

#include <limits.h>
//#include <stdio.h>
//#include <string.h>
//#include <gtk/gtk.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpcompat.h>
#include <dlfcn.h>

#include "cnmstype.h"
#include "cnmsfunc.h"
#include "w1.h"
#include "cnmsstrings.h"
#include "scangimp.h"
#include "file_control.h"
#include "progress_bar.h"

#include "errors.h"
#include "scanmsg.h"

#define	STATUS_TRANSFER_CANCELED	( -1 )

extern int init( int argc, char *argv[] );

gboolean	plug_in = FALSE;
static CNMSInt32	gimpScanStatus;

static void query( void );
static void run( const gchar *name, gint nparams, const GimpParam *param, gint *nreturn_vals, GimpParam **return_vals );
int init( int argc, char **argv );

/* libgimp APIs */
typedef void ( *FP_gimp_install_procedure ) (
			const gchar			*name,
			const gchar			*blurb,
			const gchar			*help,
			const gchar			*author,
			const gchar			*copyright,
			const gchar			*date,
			const gchar			*menu_label,
			const gchar			*image_types,
			GimpPDBProcType		type,
			gint				n_params,
			gint				n_return_vals,
			const GimpParamDef	*params,
			const GimpParamDef	*return_vals );
typedef void ( *FP_gimp_extension_ack ) ( void );
typedef gint ( *FP_gimp_main ) (
			const				GimpPlugInInfo *info,
			gint				argc,
			gchar				*argv[]);
typedef const gchar* ( *FP_gimp_gtkrc ) ( void );
typedef gint32 ( *FP_gimp_image_new ) (
			gint				width,
			gint				height,
			GimpImageBaseType	base_type );
typedef gint32 ( *FP_gimp_layer_new ) (
			gint32					image_ID,
			const gchar				*name,
			gint					width,
			gint					height,
			GimpImageType			type,
			gdouble					opacity,
			GimpLayerModeEffects	mode );
typedef gboolean ( *FP_gimp_image_add_layer ) (
			gint32				image_ID,
			gint32				layer_ID,
			gint				position );
typedef GimpDrawable* ( *FP_gimp_drawable_get ) ( gint32 drawable_ID );
typedef void ( *FP_gimp_pixel_rgn_init ) (
			GimpPixelRgn		*pr,
			GimpDrawable		*drawable,
			gint				x,
			gint				y,
			gint				width,
			gint				height,
			gint				dirty,
			gint				shadow );
typedef void ( *FP_gimp_pixel_rgn_set_rect ) (
			GimpPixelRgn		*pr,
			const guchar		*buf,
			gint				x,
			gint				y,
			gint				width,
			gint				height );
typedef void ( *FP_gimp_drawable_flush ) ( GimpDrawable *drawable );
typedef gint32 ( *FP_gimp_display_new ) ( gint32 image_ID );
typedef void ( *FP_gimp_drawable_detach ) ( GimpDrawable *drawable );
typedef gboolean ( *FP_gimp_image_delete ) ( gint32 image_ID );

/* libgimp APIs struct */
typedef struct {
	void 							*handle;
	FP_gimp_install_procedure		p_gimp_install_procedure;
	FP_gimp_extension_ack			p_gimp_extension_ack;
	FP_gimp_main					p_gimp_main;
	FP_gimp_gtkrc					p_gimp_gtkrc;
	FP_gimp_image_new				p_gimp_image_new;
	FP_gimp_layer_new				p_gimp_layer_new;
	FP_gimp_image_add_layer		p_gimp_image_add_layer;
	FP_gimp_drawable_get			p_gimp_drawable_get;
	FP_gimp_pixel_rgn_init			p_gimp_pixel_rgn_init;
	FP_gimp_pixel_rgn_set_rect		p_gimp_pixel_rgn_set_rect;
	FP_gimp_drawable_flush			p_gimp_drawable_flush;
	FP_gimp_display_new			p_gimp_display_new;
	FP_gimp_drawable_detach		p_gimp_drawable_detach;
	FP_gimp_image_delete			p_gimp_image_delete;
} CNMSGIMPAPI;

static CNMSGIMPAPI *gimpapi = CNMSNULL;

const GimpPlugInInfo PLUG_IN_INFO ={
	CNMSNULL,	/* init_proc( GimpInitProc )[ OPTIONAL ] */
	CNMSNULL,	/* quit_proc( GimpQuitProc )[ OPTIONAL ] */
	query,		/* query_proc( GimpQueryProc ) */
	run,		/* run_proc( GimpRunProc ) */
};

static CNMSVoid create_cnmsgimp_api( CNMSVoid );
static CNMSVoid dispose_cnmsgimp_api( CNMSGIMPAPI **papi );

static char *libname = "libgimp-2.0.so.0";

/* if ( ( gimpapi->p_x = ( FP_x )dlsym( gimpapi->handle, "x" ) ) == NULL ) goto _ERROR; */
#define DLSYM_CNMSGIMPAPI(x) { if ( ( gimpapi->p_ ## x = ( FP_ ## x )dlsym( gimpapi->handle, #x ) ) == NULL ) goto _ERROR; }

static CNMSVoid create_cnmsgimp_api( CNMSVoid )
{
	if ( gimpapi != CNMSNULL ) {
		dispose_cnmsgimp_api( &gimpapi );
	}
	gimpapi = (CNMSGIMPAPI *)CnmsGetMem( sizeof( CNMSGIMPAPI ) );
	if ( !gimpapi ) {
		goto _ERROR;
	}
	
	DBGMSG("<create_cnmsgimp_api> dlopen( \"%s\", RTLD_LAZY )\n", libname );
	if ( ( gimpapi->handle = dlopen( libname, RTLD_LAZY ) ) == NULL ) {
		DBGMSG("ERROR : dlopen( \"%s\", RTLD_LAZY ) -> %s\n", libname, dlerror() );
		goto _ERROR;
	}
	DLSYM_CNMSGIMPAPI( gimp_install_procedure );
	DLSYM_CNMSGIMPAPI( gimp_extension_ack );
	DLSYM_CNMSGIMPAPI( gimp_main );
	DLSYM_CNMSGIMPAPI( gimp_gtkrc );
	DLSYM_CNMSGIMPAPI( gimp_image_new );
	DLSYM_CNMSGIMPAPI( gimp_layer_new );
	DLSYM_CNMSGIMPAPI( gimp_image_add_layer );
	DLSYM_CNMSGIMPAPI( gimp_drawable_get );
	DLSYM_CNMSGIMPAPI( gimp_pixel_rgn_init );
	DLSYM_CNMSGIMPAPI( gimp_pixel_rgn_set_rect );
	DLSYM_CNMSGIMPAPI( gimp_drawable_flush );
	DLSYM_CNMSGIMPAPI( gimp_display_new );
	DLSYM_CNMSGIMPAPI( gimp_drawable_detach );
	DLSYM_CNMSGIMPAPI( gimp_image_delete );
	
	return ;
	
_ERROR:
	dispose_cnmsgimp_api( &gimpapi );
}

static CNMSVoid dispose_cnmsgimp_api( CNMSGIMPAPI **papi )
{
	if ( papi ) {
		if ( *papi ) {
			if ( (*papi)->handle ) {
				dlclose( (*papi)->handle );
			}
			CnmsFreeMem( (CNMSLPSTR)(*papi) );
		}
		*papi = CNMSNULL;
	}
}

static void query( void )
{
	static GimpParamDef	param[] = {
		{ GIMP_PDB_INT32,				/* type( GimpPDBArgType ) */
		  "run_mode",					/* name( gchar * ) */
		  "Interactive, non-interactive"	/* description( gchar * ) */
		},
	};
	static GimpParamDef	*return_vals = CNMSNULL;
	int					nreturn_vals;
	
#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[query()]->\n" );
#endif
	if ( gimpapi == CNMSNULL ) {
		goto EXIT;
	}
	
	/* init */
	nreturn_vals = 0;

	gimpapi->p_gimp_install_procedure(
			STR_ABOUT_SG STR_ABOUT_GIMP_LINUX,
										/* name(const gchar *) */
			STR_GIMP_BLURB,				/* blurb(const gchar *) */
			STR_GIMP_HELP,				/* help(const gchar *) */
			STR_ABOUT_GIMP_AUTHOR,		/* author(const gchar *) */
			STR_ABOUT_GIMP_CR STR_ABOUT_GIMP_AUTHOR STR_ABOUT_YEAR_BEGIN "-" STR_ABOUT_YEAR_END,
										/* copy right(const gchar *) */
			STR_ABOUT_YEAR_BEGIN "-" STR_ABOUT_YEAR_END,
										/* date(const gchar *) */
			STR_GIMP_MENU_LABEL,		/* menu_label(const gchar *) */
			0,							/* image_types(const gchar *) */
			GIMP_EXTENSION,				/* type(GimpPDBProcType) */
			( sizeof( param ) / sizeof( param[ 0 ] ) ),
										/* n_params(gint) */
			nreturn_vals,				/* n_return_vals(gint) */
			param,						/* params(const GimpParamDef *) */
			return_vals );				/* return_vals(const GimpParamDef *) */

EXIT:
#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[query()].\n" );
#endif
	return;
}

static void run(
		const gchar		*name,
		gint			nparams,
		const GimpParam	*param,
		gint			*nreturn_vals,
		GimpParam		**return_vals )
{
	static GimpParam	value[ 1 ];
	GimpRunMode			run_mode;
	char				*argv[ 2 ];
	int					argc = 2;

#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[run()]->\n" );
#endif
	if ( gimpapi == CNMSNULL ) {
		goto EXIT;
	}
	
	*nreturn_vals = 1;
	*return_vals = value;
	value[ 0 ].type = GIMP_PDB_STATUS;
	value[ 0 ].data.d_status = GIMP_PDB_CALLING_ERROR;
	run_mode = param[ 0 ].data.d_int32;

	argv[0] = "scangearmp";
	argv[1] = "dummy";

	if( run_mode == GIMP_RUN_INTERACTIVE ){
		gimpapi->p_gimp_extension_ack();
		plug_in = TRUE;
		init( argc, argv );
		value[ 0 ].data.d_status = GIMP_PDB_SUCCESS;
	}
#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[run()].\n" );
#endif
EXIT:
	return;
}

CNMSVoid ScanGimpOpen( CNMSVoid )
{
#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[ScanGimpOpen]->\n" );
#endif
	
	create_cnmsgimp_api();
	
#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[ScanGimpOpen].\n" );
#endif
}

CNMSInt32 ScanGimpMain(
		CNMSInt32		argc,
		CNMSLPSTR		argv[] )
{
	CNMSInt32	ret = CNMS_ERR, ldata;

#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[ScanGimpMain]\n" );
#endif
	if ( gimpapi == CNMSNULL ) {
		ret = CNMS_NO_ERR_2;	/* no libgimp */
		goto EXIT;
	}
	if( ( ldata = gimpapi->p_gimp_main( &PLUG_IN_INFO, argc, argv ) ) != TRUE ){
		goto	EXIT;
	}
	ret = CNMS_NO_ERR;
EXIT:
#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[ScanGimpMain(argc:%d)]=%d.\n", argc, ret );
#endif
	return	ret;
}

CNMSInt32 ScanGimpInit( CNMSVoid )
{
#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[ScanGimpInit]\n" );
#endif
	if( plug_in == TRUE ){
		if ( gimpapi != CNMSNULL ) {
			gtk_rc_parse( gimpapi->p_gimp_gtkrc() );
			gdk_set_use_xshm( TRUE );
		}
	}
#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[ScanGimpInit()]=%d.\n", CNMS_NO_ERR );
#endif
	return	CNMS_NO_ERR;
}

CNMSVoid ScanGimpClose( CNMSVoid )
{
	dispose_cnmsgimp_api( &gimpapi );
#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[ScanGimpClose()].\n" );
#endif
	return;
}

CNMSBool ScanGimpGetStatus( CNMSVoid )
{
	CNMSBool	ret = CNMS_FALSE;

	if( plug_in == TRUE ){
		ret = CNMS_TRUE;
	}
#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[ScanGimpGetStatus()]=%s.\n", ( ret == CNMS_TRUE ) ? "[TRUE]" : "[FALSE]" );
#endif
	return	ret;
}


#define BUF_HEIGHT	(32)

CNMSInt32 ScanGimpExec( CNMSVoid )
{
	CNMSInt32		ret = CNMS_NO_ERR, ldata, i, rasSize, bpp, width, height, buf_h, j, errorQuit = ERROR_QUIT_NO_ERR;
	CNMSFd			srcFd = CNMS_FILE_ERR;
	CNMSByte		tmpPath[ PATH_MAX ];
	CNMSLPSTR		lpBuf = CNMSNULL;
	CNMSUIReturn	uiRtn;
	gint32			image_ID = -1, layer_ID = -1;
	GimpPixelRgn	region;
	GimpDrawable	*drawable;
	LPCNMS_ROOT		root = CNMSNULL;

	gimpScanStatus = CNMS_NO_ERR;

	CNMSVoid	*fcex = CNMSNULL;
	CNMSInt32	colors, page_progress;;

	if ( gimpapi == CNMSNULL ) {
		goto EXIT;
	}
	/* get param */
	if( ( bpp = CnmsUiGetRealValue( CNMS_OBJ_A_COLOR_MODE, lpW1Comp->lpLink, lpW1Comp->linkNum ) ) <= 0 ){
		DBGMSG( "[ScanGimpExec]Error is occured in CnmsUiGetRealValue( CNMS_OBJ_A_COLOR_MODE )!\n" );
		goto	EXIT;
	}
	/* make scan file */
	if( ( root = CnmsCreateRoot() ) == CNMSNULL ){
		DBGMSG( "[ScanGimpExec]Error is occured in CnmsCreateRoot!\n" );
		goto	EXIT;
	}
	else if( ( ldata = CnmsScanChangeStatus( CNMSSCPROC_ACTION_SCAN, CNMS_FILE_ERR, root, &uiRtn ) ) != CNMS_NO_ERR ){
		if( root->head != CNMSNULL ){
			errorQuit = ShowErrorDialog();
		}
		else{
			goto	EXIT_ERR;
		}
	}
	
	width = uiRtn.ResultImgSize.Width;
	height = uiRtn.ResultImgSize.Height;

	rasSize = ( bpp == 24 ) ? ( width * 3 ) : width;
	colors = ( bpp == 24 ) ? 3 : 1;

	if( ( lpBuf = CnmsGetMem( rasSize * BUF_HEIGHT ) ) == CNMSNULL ){
		DBGMSG( "[ScanGimpExec]Can't get memory!\n" );
		goto	EXIT;
	}

	if( ( ldata = ProgressBarStart( PROGRESSBAR_ID_TRANSFER_GIMP, 0 ) ) != CNMS_NO_ERR ){	/* <- page */
		DBGMSG( "[ScanGimpExec]Error is occured in ProgressBarStart.\n" );
		goto	EXIT;
	}

	while( root->head != CNMSNULL ){
		page_progress = root->head->show_page ? root->head->page : 0;
		
		if( ( srcFd = FileControlOpenFile( FILECONTROL_OPEN_TYPE_READ, root->head->file_path ) ) == CNMS_FILE_ERR ){
			DBGMSG( "[ScanGimpExec]Can't open file( %s )!\n", root->head->file_path );
			goto	EXIT_1;
		}
		else if( ( fcex = FileControlReadFileExOpen( srcFd, root->head->rotate, width, height, colors ) ) == CNMSNULL ) {
			DBGMSG( "[ScanGimpExec]Error is occured in FileControlReadFileExOpen.\n" );
			goto	EXIT;
		}
		if( ( image_ID = gimpapi->p_gimp_image_new( width, height, ( bpp == 24 ) ? GIMP_RGB : GIMP_GRAY ) ) < 0 ){
			DBGMSG( "[ScanGimpExec]Error is occured in gimp_image_new.\n" );
			goto	EXIT_1;
		}
		layer_ID = gimpapi->p_gimp_layer_new( image_ID, "Background", width, height, ( bpp == 24 ) ? GIMP_RGB_IMAGE : GIMP_GRAY_IMAGE, 100, GIMP_NORMAL_MODE );
		gimpapi->p_gimp_image_add_layer( image_ID, layer_ID, 0 );
		drawable = gimpapi->p_gimp_drawable_get( layer_ID );
		gimpapi->p_gimp_pixel_rgn_init( &region, drawable, 0, 0, drawable->width, drawable->height, TRUE, FALSE );

		for( i = 0 ; i < height ; i += BUF_HEIGHT ){
			if( gimpScanStatus != CNMS_NO_ERR ){
				goto	EXIT_2;
			}
			buf_h = ( height - i < BUF_HEIGHT ) ? ( height - i ) : BUF_HEIGHT;
			
			if( ( ldata = FileControlReadFileExRead( fcex, lpBuf, i, buf_h ) ) == CNMS_ERR ){
				DBGMSG( "[ScanGimpExec]Error is occured in FileControlReadFileExRead.\n" );
				goto	EXIT;
			}
			gimpapi->p_gimp_pixel_rgn_set_rect( &region, ( unsigned char* )lpBuf, 0, i, width, buf_h );

			if( ( ldata = ProgressBarUpdate( ( 100 * i ) / height, page_progress ) ) != CNMS_NO_ERR ){	/* <- page */
				DBGMSG( "[ScanGimpExec]Error is occured in ProgressBarUpdate!\n" );
				goto	EXIT_2;
			}
		}
		ProgressBarUpdate( 100, page_progress );
		
		gimpapi->p_gimp_drawable_flush( drawable );
		gimpapi->p_gimp_display_new( image_ID );
		gimpapi->p_gimp_drawable_detach( drawable );
		
		FileControlDeleteFile( root->head->file_path, srcFd );
		CnmsDisposeQueue( root, CNMS_NODE_HEAD );
		FileControlReadFileExClose( &fcex );
	}
	
	ret = CNMS_NO_ERR;
EXIT_2:
	if( ret != CNMS_NO_ERR ) {
		if( image_ID != -1 ) {
			gimpapi->p_gimp_image_delete( image_ID );
		}
	}
EXIT_1:
	ProgressBarEnd();
EXIT:
	if( fcex ) {
		FileControlReadFileExClose( &fcex );
	}
	if( lpBuf != CNMSNULL ){
		CnmsFreeMem( lpBuf );
	}
	CnmsAllScanDataDispose( root );

	W1_MainWindowSetSensitiveTrue();

	if( errorQuit == ERROR_QUIT_TRUE ){
		W1_Close();
	}
#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[ScanGimpExec]= %d.\n", ret );
#endif
	return	ret;

EXIT_ERR:
	if( ldata != STATUS_TRANSFER_CANCELED ){
		errorQuit = ShowErrorDialog();
	}

	goto	EXIT_2;
}

CNMSVoid ScanGimpCancel( CNMSVoid )
{
	gimpScanStatus = CNMS_ERR;

#ifdef	__CNMS_DEBUG_GIMP__
	DBGMSG( "[ScanGimpCancel].\n" );
#endif
	return;
}


#endif	/* __GIMP_PLUGIN_ENABLE__ */

#endif	/* _SCANGIMP_C_ */
