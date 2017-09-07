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

#ifndef _SCANMSG_H_
#define _SCANMSG_H_

#include <errno.h>
#include <gtk/gtk.h>
#include "support.h"
#include "cnmstype.h"
#include "cnmsstrings.h"

#include "errors.h"

#define ERR_CODE_ENOSPC		(-ENOSPC)
#define ERR_CODE_INT		(-999)

enum {
	ERROR_MSG_ENUMS		 = 0,
	ERR_CANCELD_BY_STOP,
	ERR_SCANNER_BUSY_COPYING,		/* Scanner is busy for copying */
	ERR_SCANNER_BUSY_PRINTING,		/* Scanner is busy for printing */
	ERR_SCANNER_BUSY_PRINTERMNT,	/* Scanner is busy for printer maintenance */
	ERR_NOT_FIND_SCANNER,
	ERR_DISK_FULL,
	ERR_INTERNAL,
	ERR_FATAL,
	/* Ver.1.20 */
	ERR_DEVICE_NOT_AVAILABLE,		/* Device is not available while is use. */
	ERR_DEVICE_INITIALIZE,			/* Device is initializing. */
	ERR_SYSTEM,
	/* Ver.1.30 */
	ERR_CANCELED_ADF,
	ERR_ADF_JAM,
	ERR_ADF_COVER_OPEN,
	ERR_ADF_NO_PAPER,
	ERR_ADF_PAPER_FEED,
	ERR_ADF_MISMATCH_SIZE,
	/* Ver.1.40 */
	ERR_NOT_FIND_SCANNER_LAN,
	/* Ver.1.60 */
	ERR_NOT_FIND_SCANNER_LAN_LOCK,
	ERR_SCANNER_LOCKED,
	/* Ver.1.70 */
	ERR_SCANNER_CONNECT_FAILED_USB,
	ERR_SCANNER_CONNECT_FAILED_LAN,
};

/* error message -> cnmsstring.h */

typedef struct {
	const CNMSInt32		type;
	const CNMSLPSTR		msg;
	const CNMSInt32		quit;
} ERROR_MSG_TABLE;

typedef struct {
	const CNMSInt32		code;
	const CNMSInt32		type;
} ERROR_TYPE_INDEX_TABLE;

#define	ERROR_QUIT_TRUE		(1)		/* OK -> Quit ScanGearMP */
#define	ERROR_QUIT_FALSE	(0)
#define	ERROR_QUIT_NO_ERR	(2)

static ERROR_MSG_TABLE error_msg_table[] = {
	{ ERR_CANCELD_BY_STOP,			MSG_ERR_CANCELD_BY_STOP,			ERROR_QUIT_FALSE },
	{ ERR_SCANNER_BUSY_COPYING,		MSG_ERR_SCANNER_BUSY_COPYING,		ERROR_QUIT_TRUE },
	{ ERR_SCANNER_BUSY_PRINTING,	MSG_ERR_SCANNER_BUSY_PRINTING,		ERROR_QUIT_TRUE },
	{ ERR_SCANNER_BUSY_PRINTERMNT,	MSG_ERR_SCANNER_BUSY_PRINTERMNT,	ERROR_QUIT_TRUE },
	{ ERR_NOT_FIND_SCANNER,			MSG_ERR_NOT_FIND_SCANNER,			ERROR_QUIT_TRUE },
	{ ERR_DISK_FULL,				MSG_ERR_DISK_FULL,					ERROR_QUIT_TRUE },
	{ ERR_INTERNAL,					MSG_ERR_INTERNAL,					ERROR_QUIT_TRUE },
	{ ERR_FATAL,					MSG_ERR_FATAL,						ERROR_QUIT_TRUE },
	/* Ver.1.20 */
	{ ERR_DEVICE_NOT_AVAILABLE,		MSG_ERR_DEVICE_NOT_AVAILABLE,		ERROR_QUIT_TRUE },
	{ ERR_DEVICE_INITIALIZE,		MSG_ERR_DEVICE_INITIALIZE,			ERROR_QUIT_TRUE },
	{ ERR_SYSTEM,					MSG_ERR_SYSTEM,						ERROR_QUIT_TRUE },
	/* Ver.1.30 */
	{ ERR_CANCELED_ADF,				MSG_ERR_CANCELED_ADF,				ERROR_QUIT_FALSE },
	{ ERR_ADF_JAM,					MSG_ERR_ADF_JAM,					ERROR_QUIT_TRUE },
	{ ERR_ADF_COVER_OPEN,			MSG_ERR_ADF_COVER_OPEN,				ERROR_QUIT_TRUE },
	{ ERR_ADF_NO_PAPER,				MSG_ERR_NO_PAPER,					ERROR_QUIT_TRUE },
	{ ERR_ADF_PAPER_FEED,			MSG_ERR_PAPER_FEED,					ERROR_QUIT_TRUE },
	{ ERR_ADF_MISMATCH_SIZE,		MSG_ERR_MISMATCH_SIZE,				ERROR_QUIT_TRUE },
	/* Ver.1.40 */
	{ ERR_NOT_FIND_SCANNER_LAN,		MSG_ERR_NOT_FIND_SCANNER_LAN,		ERROR_QUIT_TRUE },
	/* Ver.1.60 */
	{ ERR_NOT_FIND_SCANNER_LAN_LOCK,	MSG_ERR_NOT_FIND_SCANNER_LAN_LOCK,		ERROR_QUIT_TRUE },
	{ ERR_SCANNER_LOCKED,				MSG_ERR_SCANNER_LOCKED,					ERROR_QUIT_TRUE },
	/* Ver.1.70 */
	{ ERR_SCANNER_CONNECT_FAILED_USB,	MSG_SCANNER_CONNECT_FAILED_USB,			ERROR_QUIT_TRUE },
	{ ERR_SCANNER_CONNECT_FAILED_LAN,	MSG_SCANNER_CONNECT_FAILED_LAN,			ERROR_QUIT_TRUE },
	{ -1, NULL, ERROR_QUIT_TRUE },
};

static ERROR_TYPE_INDEX_TABLE error_type_index_table[] = {
	{ BERRCODE_CANCELD_BY_STOP,				ERR_CANCELD_BY_STOP },
	{ BERRCODE_SCANNER_BUSY_COPYING,		ERR_SCANNER_BUSY_COPYING },
	{ BERRCODE_SCANNER_BUSY_PRINTING,		ERR_SCANNER_BUSY_PRINTING },
	{ BERRCODE_SCANNER_BUSY_PRINTERMNT,		ERR_SCANNER_BUSY_PRINTERMNT },
	{ 101,									ERR_NOT_FIND_SCANNER },
	{ BERRCODE_CONNECT_FAILED,				ERR_NOT_FIND_SCANNER },
	{ 162,									ERR_NOT_FIND_SCANNER },
	{ 172,									ERR_NOT_FIND_SCANNER },
	{ 173,									ERR_NOT_FIND_SCANNER },
	{ ERR_CODE_ENOSPC,						ERR_DISK_FULL },
	{ ERR_CODE_INT,							ERR_INTERNAL },
	/* Ver.1.20 */
	{ BERRCODE_DEVICE_NOT_AVAILABLE,		ERR_DEVICE_NOT_AVAILABLE },
	{ BERRCODE_DEVICE_INITIALIZE,			ERR_DEVICE_INITIALIZE },
	{ BERRCODE_SYSTEM,						ERR_SYSTEM },
	/* Ver.1.30 */
	{ BERRCODE_CANCELED_ADF,				ERR_CANCELED_ADF },
	{ BERRCODE_ADF_JAM,						ERR_ADF_JAM },
	{ BERRCODE_ADF_COVER_OPEN,				ERR_ADF_COVER_OPEN },
	{ BERRCODE_ADF_NO_PAPER,				ERR_ADF_NO_PAPER },
	{ BERRCODE_ADF_PAPER_FEED,				ERR_ADF_PAPER_FEED },
	{ BERRCODE_ADF_MISMATCH_SIZE,			ERR_ADF_MISMATCH_SIZE },
	{ BERRCODE_DEVICE_NOT_AVAILABLE_LAN,	ERR_NOT_FIND_SCANNER_LAN },/* Ver.1.40 */
	/* Ver.1.60 */
	{ BERRCODE_DEVICE_NOT_AVAILABLE_LAN_LOCK,	ERR_NOT_FIND_SCANNER_LAN_LOCK },
	{ BERRCODE_SCANNER_LOCKED,					ERR_SCANNER_LOCKED },
	/* Ver.1.70 */
	{ BERRCODE_SCANNER_CONNECT_FAILED_USB,		ERR_SCANNER_CONNECT_FAILED_USB },
	{ BERRCODE_SCANNER_CONNECT_FAILED_LAN,		ERR_SCANNER_CONNECT_FAILED_LAN },
	{  -1,										ERR_FATAL },
};


#endif	/* _SCANMSG_H_ */
