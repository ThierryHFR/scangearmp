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

#ifndef _SCANOPTS_C_
#define _SCANOPTS_C_

#include <stdio.h>
#include <string.h>
#include "scanopts.h"
#include "canon_mfp_opts.h"
#include "cnmsfunc.h"

static CNMSInt32 CnmsCheckOpts( LPCNMSOPTSNUM lpOpts )
{
	CNMSInt32	ret = CNMS_ERR;
	
	if ( !lpOpts->num ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->num = %d )\n", lpOpts->num );
		goto EXIT_ERR;
	}else if ( !lpOpts->preview ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->preview = %d )\n", lpOpts->preview );
		goto EXIT_ERR;
	}else if ( !lpOpts->scanMode ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->scanMode = %d )\n", lpOpts->scanMode );
		goto EXIT_ERR;
	}else if ( !lpOpts->resBind ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->resBind = %d )\n", lpOpts->resBind );
		goto EXIT_ERR;
	}else if ( !lpOpts->resX ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->resX = %d )\n", lpOpts->resX );
		goto EXIT_ERR;
	}else if ( !lpOpts->resY ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->resY = %d )\n", lpOpts->resY );
		goto EXIT_ERR;
	}else if ( !lpOpts->mm_tl_X ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->mm_tl_X = %d )\n", lpOpts->mm_tl_X );
		goto EXIT_ERR;
	}else if ( !lpOpts->mm_tl_Y ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->mm_tl_Y = %d )\n", lpOpts->mm_tl_Y );
		goto EXIT_ERR;
	}else if ( !lpOpts->mm_br_X ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->mm_br_X = %d )\n", lpOpts->mm_br_X );
		goto EXIT_ERR;
	}else if ( !lpOpts->mm_br_Y ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->mm_br_Y = %d )\n", lpOpts->mm_br_Y );
		goto EXIT_ERR;
	}else if ( !lpOpts->getStatus ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->getStatus = %d )\n", lpOpts->getStatus );
		goto EXIT_ERR;
	}else if ( !lpOpts->getCalStatus ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->getCalStatus = %d )\n", lpOpts->getCalStatus );
		goto EXIT_ERR;
	}else if ( !lpOpts->getAbilityVer ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->getAbilityVer = %d )\n", lpOpts->getAbilityVer );
		goto EXIT_ERR;
	}else if ( !lpOpts->getAbility ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->getAbility = %d )\n", lpOpts->getAbility );
		goto EXIT_ERR;
	}else if ( !lpOpts->useAreaPixel ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->useAreaPixel = %d )\n", lpOpts->useAreaPixel );
		goto EXIT_ERR;
	}else if ( !lpOpts->pixel_tl_X ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->pixel_tl_X = %d )\n", lpOpts->pixel_tl_X );
		goto EXIT_ERR;
	}else if ( !lpOpts->pixel_tl_Y ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->pixel_tl_Y = %d )\n", lpOpts->pixel_tl_Y );
		goto EXIT_ERR;
	}else if ( !lpOpts->pixel_br_X ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->pixel_br_X = %d )\n", lpOpts->pixel_br_X );
		goto EXIT_ERR;
	}else if ( !lpOpts->pixel_br_Y ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->pixel_br_Y = %d )\n", lpOpts->pixel_br_Y );
		goto EXIT_ERR;
	}else if ( !lpOpts->setGamma ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->setGamma = %d )\n", lpOpts->setGamma );
		goto EXIT_ERR;
	}else if ( !lpOpts->setEveryCalibration ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->setEveryCalibration = %d )\n", lpOpts->setEveryCalibration );
		goto EXIT_ERR;
	}else if ( !lpOpts->setSilent ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->setSilent = %d )\n", lpOpts->setSilent );
		goto EXIT_ERR;
	}else if ( !lpOpts->scanMethod ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->scanMethod = %d )\n", lpOpts->scanMethod );
		goto EXIT_ERR;
	}else if ( !lpOpts->deviceSettings ) {
		DBGMSG( "[CnmsCheckOpts] invalid value. ( lpOpts->deviceSettings = %d )\n", lpOpts->deviceSettings );
		goto EXIT_ERR;
	}

	ret = CNMS_NO_ERR;
EXIT_ERR:
	return	ret;
}

CNMSInt32 CnmsSetOptsNum( SANE_Handle h, LPCNMSOPTSNUM lpOpts )
{
	const SANE_Option_Descriptor	*opt;
	CNMSInt32						ret = CNMS_ERR;
	SANE_Status						status;
	SANE_Int						num, i;
	
	CnmsSetMem( (CNMSLPSTR)lpOpts, 0, sizeof( CNMSOPTSNUM ) );
	
	if ( ( status = sane_control_option( h, 0, SANE_ACTION_GET_VALUE, &num, CNMSNULL ) ) != SANE_STATUS_GOOD ){
		DBGMSG( "[CnmsSetOptNum]sane_control_option( num_opts ) is returned error value.\n" );
		goto	EXIT_ERR;
	}
	lpOpts->num = num;
	
	/* search opts name */
	for(i = 0; i < num; i++){
		opt = sane_get_option_descriptor( h, i );
		if( opt->name ){
			if( strcmp( opt->name, CANON_NAME_PREVIEW ) == 0 ){
				lpOpts->preview = i;
			}else if( strcmp( opt->name, CANON_NAME_SCAN_MODE ) == 0 ){
				lpOpts->scanMode = i;
			}else if( strcmp( opt->name, CANON_NAME_RES_BIND ) == 0 ){
				lpOpts->resBind = i;
			}else if( strcmp( opt->name, CANON_NAME_RES_X ) == 0 ){
				lpOpts->resX = i;
			}else if( strcmp( opt->name, CANON_NAME_RES_Y ) == 0 ){
				lpOpts->resY = i;
			}else if( strcmp( opt->name, CANON_NAME_MARGINS_TL_X ) == 0 ){
				lpOpts->mm_tl_X = i;
			}else if( strcmp( opt->name, CANON_NAME_MARGINS_TL_Y ) == 0 ){
				lpOpts->mm_tl_Y = i;
			}else if( strcmp( opt->name, CANON_NAME_MARGINS_BR_X ) == 0 ){
				lpOpts->mm_br_X = i;
			}else if( strcmp( opt->name, CANON_NAME_MARGINS_BR_Y ) == 0 ){
				lpOpts->mm_br_Y = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_GET_STATUS ) == 0 ){
				lpOpts->getStatus = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_GET_CAL_STATUS ) == 0 ){
				lpOpts->getCalStatus = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_GET_ABILITY_VER ) == 0 ){
				lpOpts->getAbilityVer = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_GET_ABILITY ) == 0 ){
				lpOpts->getAbility = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_USE_AREA_PIXEL ) == 0 ){
				lpOpts->useAreaPixel = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_TL_X_PIXEL ) == 0 ){
				lpOpts->pixel_tl_X = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_TL_Y_PIXEL ) == 0 ){
				lpOpts->pixel_tl_Y = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_BR_X_PIXEL ) == 0 ){
				lpOpts->pixel_br_X = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_BR_Y_PIXEL ) == 0 ){
				lpOpts->pixel_br_Y = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_GAMMA ) == 0 ){
				lpOpts->setGamma = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_EVERY_CALIBRATION ) == 0 ){
				lpOpts->setEveryCalibration = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_SILENT ) == 0 ){
				lpOpts->setSilent = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_SCAN_METHOD ) == 0 ){
				lpOpts->scanMethod = i;
			}else if( strcmp( opt->name, CANON_NAME_EX_DEVICE_SETTINGS ) == 0 ){
				lpOpts->deviceSettings = i;
			}
		}
	}
	ret = CnmsCheckOpts( lpOpts );
	
EXIT_ERR:
	return	ret;
}

#endif	/* _SCANOPTS_C_ */
