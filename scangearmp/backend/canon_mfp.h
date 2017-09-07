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


#ifndef _canon_mfp_h_
#define _canon_mfp_h_


#include <sys/types.h>

typedef enum
{
	OPT_NUM_OPTS = 0,

	OPT_MODE,
	OPT_PREVIEW,

	OPT_RESOLUTION_GROUP,
	OPT_RESOLUTION_BIND,
	OPT_X_RESOLUTION,
	OPT_Y_RESOLUTION,

	OPT_MARGINS_GROUP,
	OPT_TL_X,
	OPT_TL_Y,
	OPT_BR_X,
	OPT_BR_Y,

	OPT_EXTEND_GROUP,
	
	OPT_EX_STATUS,
	OPT_EX_CALIBRATION_STATUS,
	OPT_EX_ABILITY_VER,
	OPT_EX_ABILITY,
	
	OPT_EX_USE_AREA_PIXEL,
	OPT_EX_TL_X_PIXEL,
	OPT_EX_TL_Y_PIXEL,
	OPT_EX_BR_X_PIXEL,
	OPT_EX_BR_Y_PIXEL,
	
	OPT_EX_GAMMA,
	
	/* Ver.1.20 - */
	OPT_EX_EVERY_CALIBRATION,
	OPT_EX_SILENT,
	
	/* Ver.1.30 - */
	OPT_EX_SCAN_METHOD,
	
	/* Ver.1.90 - */
	OPT_EX_DEVICE_SETTINGS,
	
	NUM_OPTIONS
} CANON_Option;

typedef union
{
  SANE_Bool b;
  SANE_Word w;
  SANE_Word *wa;		/* word array */
  SANE_String s;
} Option_Value;

typedef struct {
	SANE_Word	product_id;
	SANE_Word	speed;
	
	SANE_Range	xres_range;
	SANE_Range	yres_range;
	SANE_Range	x_range;
	SANE_Range	y_range;
	SANE_Range	x_range_pixel;
	SANE_Range	y_range_pixel;
	
	SANE_Int xres_default;
	SANE_Int yres_default;
	SANE_Int bmu;
	SANE_Int mud;
} CANON_Info;

typedef struct CANON_Device {
	struct CANON_Device *next;
	SANE_Device sane;
	CANON_Info info;
	void *cnmslldapi;
	SANE_Word	api_ver;	/* add Ver.1.20 */
} CANON_Device;

typedef struct CANON_Scanner
{
    struct CANON_Scanner *next;
    int fd;
    CANON_Device *hw;
    SANE_Option_Descriptor opt[NUM_OPTIONS];
    Option_Value val[NUM_OPTIONS];
    unsigned short gamma_table[65536];
    SANE_Parameters params;

    SANE_Int	xres;
    SANE_Int	yres;
    SANE_Int	ulx;
    SANE_Int	uly;
    SANE_Int	width;
    SANE_Int	length;

    SANE_Int	image_composition;
    SANE_Int	bpp;

    SANE_Word	xres_word_list [8];
    SANE_Word	yres_word_list [8];

    size_t		bytes_to_read;
    int			scanning;
    
    /* add Ver.1.30 */
    int			scanMethod;
    int			sane_start_status;
  }
CANON_Scanner;


char *option_name[]=
{
	"OPT_NUM_OPTS",

	"OPT_MODE",
	"OPT_PREVIEW",

	"OPT_RESOLUTION_GROUP",
	"OPT_RESOLUTION_BIND",
	"OPT_X_RESOLUTION",
	"OPT_Y_RESOLUTION",

	"OPT_MARGINS_GROUP",
	"OPT_TL_X",
	"OPT_TL_Y",
	"OPT_BR_X",
	"OPT_BR_Y",

	"OPT_EXTEND_GROUP",
	
	"OPT_EX_STATUS",
	"OPT_EX_CALIBRATION_STATUS",
	"OPT_EX_ABILITY_VER",
	"OPT_EX_ABILITY",
	
	"OPT_EX_USE_AREA_PIXEL",
	"OPT_EX_TL_X_PIXELS",
	"OPT_EX_TL_Y_PIXELS",
	"OPT_EX_BR_X_PIXELS",
	"OPT_EX_BR_Y_PIXELS",
	
	"OPT_EX_GAMMA",
	
	/* Ver.1.20 - */
	"OPT_EX_EVERY_CALIBRATION",
	"OPT_EX_SILENT",
	
	/* Ver.1.30 - */
	"OPT_EX_SCAN_METHOD",
	
	/* Ver.1.90 - */
	"OPT_EX_DEVICE_SETTINGS",
	
	"NUM_OPTIONS"
};

#endif /* not _canon_mfp_h_ */

