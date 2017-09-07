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

#ifndef	_CNMSSTR_C_
#define	_CNMSSTR_C_

#include <gtk/gtk.h>
#include "support.h"

#include "cnmstype.h"
#include "cnmsdef.h"
#include "cnmsstr.h"
#include "cnmsfunc.h"
#include "cnmsstrings.h"

typedef struct{
	const CNMSInt32		Value;
	const CNMSLPSTR		Str;
}CNMSSTRSUB, *LPCNMSSTRSUB;

typedef struct{
	const CNMSInt32		Object;
	const LPCNMSSTRSUB	lpSub;
}CNMSSTRCOMP, *LPCNMSSTRCOMP;

CNMSByte	CNMS_NOT_SUPPORT_STR[] = "!!! NOT SUPPORT !!!";
/* not support type */
static CNMSSTRSUB STRSUB_NOT_SUPPORT[] = {
	{ CNMS_ERR,	CNMS_NOT_SUPPORT_STR },
};

static CNMSInt8 grayout[] = " ";

/* Select Source (Simple Mode) */
static CNMSSTRSUB STRSUB_S_SOURCE[] = {
	{ CNMS_S_SOURCE_PHOTO,			STR_CNMS_S_SOURCE_PHOTO         },
	{ CNMS_S_SOURCE_MAGAZINE,		STR_CNMS_S_SOURCE_MAGAZINE      },
	{ CNMS_S_SOURCE_NEWSPAPER,		STR_CNMS_S_SOURCE_NEWSPAPER     },
	{ CNMS_S_SOURCE_DOCUMENT,		STR_CNMS_S_SOURCE_DOCUMENT      },
	{ CNMS_S_SOURCE_GRAYNEWSPAPER,	STR_CNMS_S_SOURCE_GRAYNEWSPAPER },
	{ CNMS_S_SOURCE_TEXT_COL_SADF,	STR_CNMS_S_SOURCE_TEXT_COL_SADF },
	{ CNMS_S_SOURCE_TEXT_GRAY_SADF,	STR_CNMS_S_SOURCE_TEXT_GRAY_SADF},
	{ CNMS_S_SOURCE_TEXT_COL_DADF,	STR_CNMS_S_SOURCE_TEXT_COL_DADF },
	{ CNMS_S_SOURCE_TEXT_GRAY_DADF,	STR_CNMS_S_SOURCE_TEXT_GRAY_DADF},
	{ CNMS_S_SOURCE_COLDOCUMENT,	STR_CNMS_S_SOURCE_COLDOCUMENT   },
	{ CNMS_ERR,						CNMS_NOT_SUPPORT_STR            },
};

/* Destination (Simple Mode) */
static CNMSSTRSUB STRSUB_S_DESTINATION[] = {
	{ CNMS_S_DESTINATION_PRINT,			STR_CNMS_S_DESTINATION_PRINT        },
	{ CNMS_S_DESTINATION_DISPLAY,		STR_CNMS_S_DESTINATION_DISPLAY      },
	{ CNMS_S_DESTINATION_OCR,			STR_CNMS_S_DESTINATION_OCR          },
	{ CNMS_S_DESTINATION_PRINT_NDPI,	STR_CNMS_S_DESTINATION_PRINT_NDPI   },
	{ CNMS_S_DESTINATION_DISPLAY_NDPI,	STR_CNMS_S_DESTINATION_DISPLAY_NDPI },
	{ CNMS_S_DESTINATION_OCR_NDPI,		STR_CNMS_S_DESTINATION_OCR_NDPI     },
	{ CNMS_ERR,							CNMS_NOT_SUPPORT_STR                },
};

/* Output Size (Simple Mode) */
static CNMSSTRSUB STRSUB_S_OUTPUT_SIZE[] = {
	{ CNMS_S_OUTPUT_SIZE_FREE,		STR_CNMS_SIZE_FREE       },
	{ CNMS_S_OUTPUT_SIZE_L_LA,		STR_CNMS_SIZE_L_LA       },
	{ CNMS_S_OUTPUT_SIZE_L_PO,		STR_CNMS_SIZE_L_PO       },
	{ CNMS_S_OUTPUT_SIZE_LL_LA,		STR_CNMS_SIZE_LL_LA      },
	{ CNMS_S_OUTPUT_SIZE_LL_PO,		STR_CNMS_SIZE_LL_PO      },
	{ CNMS_S_OUTPUT_SIZE_4_6_LA,	STR_CNMS_SIZE_4_6_LA     },
	{ CNMS_S_OUTPUT_SIZE_4_6_PO,	STR_CNMS_SIZE_4_6_PO     },
	{ CNMS_S_OUTPUT_SIZE_POSTCARD,	STR_CNMS_SIZE_POSTCARD   },
	{ CNMS_S_OUTPUT_SIZE_A4,		STR_CNMS_SIZE_A4         },
	{ CNMS_S_OUTPUT_SIZE_LETTER,	STR_CNMS_SIZE_LETTER     },
	{ CNMS_S_OUTPUT_SIZE_640_480,	STR_CNMS_SIZE_640_480    },
	{ CNMS_S_OUTPUT_SIZE_800_600,	STR_CNMS_SIZE_800_600    },
	{ CNMS_S_OUTPUT_SIZE_1024_768,	STR_CNMS_SIZE_1024_768   },
	{ CNMS_S_OUTPUT_SIZE_1600_1200,	STR_CNMS_SIZE_1600_1200  },
	{ CNMS_S_OUTPUT_SIZE_2048_1536,	STR_CNMS_SIZE_2048_1536  },
	{ CNMS_ERR,						CNMS_NOT_SUPPORT_STR     },
};

/* Select Source : (Advanced Mode) */
static CNMSSTRSUB STRSUB_A_SOURCE[] = {
	{ CNMS_A_SOURCE_PLATEN,			STR_CNMS_A_SOURCE_PLATEN   },
	{ CNMS_A_SOURCE_TEXT_SADF,		STR_CNMS_A_SOURCE_TEXT_SADF},
	{ CNMS_A_SOURCE_TEXT_DADF,		STR_CNMS_A_SOURCE_TEXT_DADF},
	{ CNMS_ERR,						CNMS_NOT_SUPPORT_STR       },
};

/* Paper Size : (Advanced Mode) */
static CNMSSTRSUB STRSUB_A_PAPER_SIZE[] = {
	{ CNMS_A_PAPER_SIZE_A5,				STR_CNMS_SIZE_A5          },
	{ CNMS_A_PAPER_SIZE_B5,				STR_CNMS_SIZE_B5          },
	{ CNMS_A_PAPER_SIZE_A4,				STR_CNMS_SIZE_A4          },
	{ CNMS_A_PAPER_SIZE_LETTER,			STR_CNMS_SIZE_LETTER      },
	{ CNMS_A_PAPER_SIZE_LEGAL,			STR_CNMS_SIZE_LEGAL       },
	{ CNMS_A_PAPER_SIZE_FULL_PAGE,		STR_CNMS_SIZE_FULL_PAGE   },
	{ CNMS_ERR,							CNMS_NOT_SUPPORT_STR      },
};

/* Binding Location */
static CNMSSTRSUB STRSUB_A_BINDING_LOCATION[] = {
	{ CNMS_A_BINDING_LOCATION_LONG,		STR_CNMS_LONG_EDGE        },
	{ CNMS_A_BINDING_LOCATION_SHORT,	STR_CNMS_SHORT_EDGE       },
	{ CNMS_ERR,							CNMS_NOT_SUPPORT_STR      },
};

/* Color Mode : (Advanced Mode) */
static CNMSSTRSUB STRSUB_A_COLOR_MODE[] = {
	{ CNMS_A_COLOR_MODE_COLOR,				STR_CNMS_A_COLOR_MODE_COLOR           },
	{ CNMS_A_COLOR_MODE_COLOR_DOCUMENTS,	STR_CNMS_A_COLOR_MODE_COLOR_DOCUMENTS },
	{ CNMS_A_COLOR_MODE_GRAY,				STR_CNMS_A_COLOR_MODE_GRAY            }, /* add space */
	{ CNMS_A_COLOR_MODE_MONO,				STR_CNMS_A_COLOR_MODE_MONO            },
	{ CNMS_ERR,								CNMS_NOT_SUPPORT_STR                  },
};

/* Unit : (Advanced Mode) */
static CNMSSTRSUB STRSUB_A_UNIT[] = {
	{ CNMS_A_UNIT_PIXEL,				STR_CNMS_A_UNIT_PIXEL      },
	{ CNMS_A_UNIT_INCH,					STR_CNMS_A_UNIT_INCH       },
	{ CNMS_A_UNIT_CENTIMETER,			STR_CNMS_A_UNIT_CENTIMETER },
	{ CNMS_A_UNIT_MILLIMETER,			STR_CNMS_A_UNIT_MILLIMETER },
	{ CNMS_ERR,							CNMS_NOT_SUPPORT_STR },
};

/* Output Resolution : (Advanced Mode) */
static CNMSSTRSUB STRSUB_A_OUTPUT_RESOLUTION[] = {
	{ CNMS_A_OUTPUT_RESOLUTION_50,		  "50"               },
	{ CNMS_A_OUTPUT_RESOLUTION_75,		  "75"               },
	{ CNMS_A_OUTPUT_RESOLUTION_100,		 "100"               },
	{ CNMS_A_OUTPUT_RESOLUTION_150,		 "150"               },
	{ CNMS_A_OUTPUT_RESOLUTION_200,		 "200"               },
	{ CNMS_A_OUTPUT_RESOLUTION_300,		 "300"               },
	{ CNMS_A_OUTPUT_RESOLUTION_400,		 "400"               },
	{ CNMS_A_OUTPUT_RESOLUTION_600,		 "600"               },
	{ CNMS_A_OUTPUT_RESOLUTION_800,		 "800"               },
	{ CNMS_A_OUTPUT_RESOLUTION_1200,	"1200"               },
	{ CNMS_A_OUTPUT_RESOLUTION_1600,	"1600"               },
	{ CNMS_A_OUTPUT_RESOLUTION_2400,	"2400"               },
	{ CNMS_A_OUTPUT_RESOLUTION_4800,	"4800"               },
	{ CNMS_ERR,							CNMS_NOT_SUPPORT_STR },
};

/* Output Size : (Advanced Mode) */
static CNMSSTRSUB STRSUB_A_OUTPUT_SIZE[] = {
	{ CNMS_A_OUTPUT_SIZE_FREE,		STR_CNMS_SIZE_FREE        },
	{ CNMS_A_OUTPUT_SIZE_CARD,		STR_CNMS_SIZE_CARD        },
	{ CNMS_A_OUTPUT_SIZE_L_LA,		STR_CNMS_SIZE_L_LA        },
	{ CNMS_A_OUTPUT_SIZE_L_PO,		STR_CNMS_SIZE_L_PO        },
	{ CNMS_A_OUTPUT_SIZE_LL_LA,		STR_CNMS_SIZE_LL_LA       },
	{ CNMS_A_OUTPUT_SIZE_LL_PO,		STR_CNMS_SIZE_LL_PO       },
	{ CNMS_A_OUTPUT_SIZE_4_6_LA,	STR_CNMS_SIZE_4_6_LA      },
	{ CNMS_A_OUTPUT_SIZE_4_6_PO,	STR_CNMS_SIZE_4_6_PO      },
	{ CNMS_A_OUTPUT_SIZE_POSTCARD,	STR_CNMS_SIZE_POSTCARD    },
	{ CNMS_A_OUTPUT_SIZE_A5,		STR_CNMS_SIZE_A5          },
	{ CNMS_A_OUTPUT_SIZE_B5,		STR_CNMS_SIZE_B5          },
	{ CNMS_A_OUTPUT_SIZE_A4,		STR_CNMS_SIZE_A4          },
	{ CNMS_A_OUTPUT_SIZE_LETTER,	STR_CNMS_SIZE_LETTER      },
	{ CNMS_A_OUTPUT_SIZE_LEGAL,		STR_CNMS_SIZE_LEGAL       },
	{ CNMS_A_OUTPUT_SIZE_B4,		STR_CNMS_SIZE_B4          },
	{ CNMS_A_OUTPUT_SIZE_A3,		STR_CNMS_SIZE_A3          },
	{ CNMS_A_OUTPUT_SIZE_128_128,	STR_CNMS_SIZE_128_128     },
	{ CNMS_A_OUTPUT_SIZE_640_480,	STR_CNMS_SIZE_640_480     },
	{ CNMS_A_OUTPUT_SIZE_800_600,	STR_CNMS_SIZE_800_600     },
	{ CNMS_A_OUTPUT_SIZE_1024_768,	STR_CNMS_SIZE_1024_768    },
	{ CNMS_A_OUTPUT_SIZE_1280_1024,	STR_CNMS_SIZE_1280_1024   },
	{ CNMS_A_OUTPUT_SIZE_1600_1200,	STR_CNMS_SIZE_1600_1200   },
	{ CNMS_A_OUTPUT_SIZE_2048_1536,	STR_CNMS_SIZE_2048_1536   },
	{ CNMS_A_OUTPUT_SIZE_2272_1704,	STR_CNMS_SIZE_2272_1704   },
	{ CNMS_A_OUTPUT_SIZE_2592_1944,	STR_CNMS_SIZE_2592_1944   },
	{ CNMS_ERR,						CNMS_NOT_SUPPORT_STR      },
};

/* Auto Tone : (Advanced Mode) */
static CNMSSTRSUB STRSUB_A_AUTO_TONE[] = {
	{ CNMS_ERR,				CNMS_NOT_SUPPORT_STR },
};

/* Unsharp Mask : (Advanced Mode) */
static CNMSSTRSUB STRSUB_A_UNSHARP_MASK[] = {
	{ CNMS_A_UNSHARP_MASK_OFF,	STR_CNMS_OFF         },
	{ CNMS_A_UNSHARP_MASK_ON,	STR_CNMS_ON          },
	{ CNMS_ERR,					CNMS_NOT_SUPPORT_STR },
};

/* Descreen : (Advanced Mode) */
static CNMSSTRSUB STRSUB_A_DESCREEN[] = {
	{ CNMS_A_DESCREEN_OFF,	STR_CNMS_OFF         },
	{ CNMS_A_DESCREEN_ON,	STR_CNMS_ON          },
	{ CNMS_ERR,				CNMS_NOT_SUPPORT_STR },
};

/* Default Threshold : (Advanced Mode) */
static CNMSSTRSUB STRSUB_A_THRESHOLD_DEFAULT[] = {
	{ CNMS_A_THRESHOLD_DEFAULT_72,		 "72"		   		 },
	{ CNMS_A_THRESHOLD_DEFAULT_128,		"128"	    		 },
	{ CNMS_ERR,							CNMS_NOT_SUPPORT_STR },
};

/* Every Calibration : (Preference) */
static CNMSSTRSUB STRSUB_P_EVERY_CALIBRATION[] = {
	{ CNMS_P_EVERY_CALIBRATION_OFF,		STR_CNMS_OFF   		 },
	{ CNMS_P_EVERY_CALIBRATION_ON,		STR_CNMS_ON    		 },
	{ CNMS_ERR,							CNMS_NOT_SUPPORT_STR },
};

/* Auto Power On : (Preference) */
static CNMSSTRSUB STRSUB_P_AUTO_POWER_ON[] = {
	{ CNMS_P_AUTO_POWER_ON_DISABLE,		STR_PREF_DISABLE	 },
	{ CNMS_P_AUTO_POWER_ON_ENABLE,		STR_PREF_ENABLE		 },
	{ CNMS_ERR,							CNMS_NOT_SUPPORT_STR },
};

/* Auto Power Off : (Preference) */
static CNMSSTRSUB STRSUB_P_AUTO_POWER_OFF[] = {
	{ CNMS_P_AUTO_POWER_OFF_DISABLE,	STR_PREF_DISABLE	 },
	{ CNMS_P_AUTO_POWER_OFF_15MIN,		STR_PREF_15_MINUTES	 },
	{ CNMS_P_AUTO_POWER_OFF_30MIN,		STR_PREF_30_MINUTES	 },
	{ CNMS_P_AUTO_POWER_OFF_60MIN,		STR_PREF_60_MINUTES	 },
	{ CNMS_P_AUTO_POWER_OFF_120MIN,		STR_PREF_120_MINUTES },
	{ CNMS_P_AUTO_POWER_OFF_240MIN,		STR_PREF_240_MINUTES },
	{ CNMS_ERR,							CNMS_NOT_SUPPORT_STR },
};

static CNMSSTRCOMP CnmsStrComp[] = {
	{ CNMS_OBJ_S_SOURCE,				STRSUB_S_SOURCE                },
	{ CNMS_OBJ_S_DESTINATION,			STRSUB_S_DESTINATION           },
	{ CNMS_OBJ_S_OUTPUT_SIZE,			STRSUB_S_OUTPUT_SIZE           },
	{ CNMS_OBJ_A_SOURCE,				STRSUB_A_SOURCE                },
	{ CNMS_OBJ_A_PAPER_SIZE,			STRSUB_A_PAPER_SIZE            },
	{ CNMS_OBJ_A_COLOR_MODE,			STRSUB_A_COLOR_MODE            },
	{ CNMS_OBJ_A_UNIT,					STRSUB_A_UNIT                  },
	{ CNMS_OBJ_A_OUTPUT_RESOLUTION,		STRSUB_A_OUTPUT_RESOLUTION     },
	{ CNMS_OBJ_A_OUTPUT_SIZE,			STRSUB_A_OUTPUT_SIZE           },
	{ CNMS_OBJ_A_UNSHARP_MASK,			STRSUB_A_UNSHARP_MASK          },
	{ CNMS_OBJ_A_DESCREEN,				STRSUB_A_DESCREEN              },
	{ CNMS_OBJ_A_THRESHOLD_DEFAULT,		STRSUB_A_THRESHOLD_DEFAULT     },
	{ CNMS_OBJ_P_EVERY_CALIBRATION,		STRSUB_P_EVERY_CALIBRATION     },
	{ CNMS_OBJ_A_BINDING_LOCATION,		STRSUB_A_BINDING_LOCATION      },
	{ CNMS_OBJ_P_AUTO_POWER_ON,	 		STRSUB_P_AUTO_POWER_ON         },
	{ CNMS_OBJ_P_AUTO_POWER_OFF,		STRSUB_P_AUTO_POWER_OFF        },
	{ CNMS_ERR,							STRSUB_NOT_SUPPORT             },
};

static LPCNMSSTRCOMP SeekStrComp(
		CNMSInt32		object )
{
	LPCNMSSTRCOMP	lpStrComp;
	
	for( lpStrComp = CnmsStrComp ; lpStrComp->Object != CNMS_ERR ; lpStrComp ++ ){
		if( object == lpStrComp->Object ){
			break;
		}
	}
	if( lpStrComp->Object == CNMS_ERR ){
		lpStrComp = CNMSNULL;
	}

	return	lpStrComp;
}

static CNMSInt32 GetVal(
		CNMSLPSTR		str,
		LPCNMSSTRCOMP	lpStrComp )
{
	CNMSInt32		ret = CNMS_ERR, ldata;
	LPCNMSSTRSUB	lpSub;
	CNMSLPSTR		lpStr;
	
	if( ( str == CNMSNULL ) || ( lpStrComp == CNMSNULL ) ){
		goto	EXIT;
	}
	
	for( lpSub = lpStrComp->lpSub ; lpSub->Value != CNMS_ERR ; lpSub ++ ){
		lpStr = gettext( lpSub->Str );
		if( ( ldata = CnmsStrCompare( (const CNMSLPSTR)lpStr, (const CNMSLPSTR)str ) ) == CNMS_NO_ERR ){
			ret = lpSub->Value;
			break;
		}
	}
EXIT:
	return	ret;
}

static CNMSLPSTR GetStr(
		CNMSInt32		value,
		LPCNMSSTRCOMP	lpStrComp )
{
	LPCNMSSTRSUB	lpSub;
	CNMSLPSTR		lpRet = CNMSNULL;
	
	if( ( value == CNMS_ERR ) || ( lpStrComp == CNMSNULL ) ){
		goto	EXIT;
	}
	
	for( lpSub = lpStrComp->lpSub ; lpSub->Value != CNMS_ERR ; lpSub ++ ){
		if( value == lpSub->Value ){
			break;
		}
	}
	if( lpSub->Value != CNMS_ERR ){
		lpRet = (CNMSLPSTR)lpSub->Str;
	}
EXIT:
	return	lpRet;
}

CNMSLPSTR CnmsChgValToStr(
		CNMSInt32		value,
		CNMSInt32		object )
{
	LPCNMSSTRCOMP	lpStrComp;
	CNMSLPSTR		lpRet = CNMSNULL;

	if( ( lpStrComp = SeekStrComp( object ) ) == CNMSNULL ){
		goto	EXIT;
	}
	lpRet = gettext( GetStr( value, lpStrComp ) );
EXIT:
	return	lpRet;
}

CNMSInt32 CnmsChgStrToVal(
		CNMSLPSTR		str,
		CNMSInt32		object )
{
	LPCNMSSTRCOMP	lpStrComp;
	CNMSInt32		ret = CNMS_ERR;

	if( ( lpStrComp = SeekStrComp( object ) ) == CNMSNULL ){
		goto	EXIT;
	}
	ret = GetVal( str, lpStrComp );
EXIT:
	return	ret;
}

#ifdef	__CNMS_DEBUG__
typedef struct{
	CNMSInt32	val;
	CNMSLPSTR	str;
}CNMSSTRDEBUG, *LPCNMSSTRDEBUG;

CNMSVoid CnmsDbgUiInfo(
		CNMSBool			activeKey,
		LPCNMSUILINKCOMP	lpLink,
		CNMSInt32			linkNum )
{
	CNMSSTRDEBUG objComp[] = {
		{ CNMS_OBJ_S_SOURCE,				"S_SOURCE               " },
		{ CNMS_OBJ_S_DESTINATION,			"S_DESTINATION          " },
		{ CNMS_OBJ_S_OUTPUT_SIZE,			"S_OUTPUT_SIZE          " },
		{ CNMS_OBJ_A_SOURCE,				"A_SOURCE               " },
		{ CNMS_OBJ_A_PAPER_SIZE,			"A_PAPER_SIZE           " },
		{ CNMS_OBJ_A_COLOR_MODE,			"A_COLOR_MODE           " },
		{ CNMS_OBJ_A_INPUT_WIDTH,			"A_INPUT_WIDTH          " },
		{ CNMS_OBJ_A_INPUT_HEIGHT,			"A_INPUT_HEIGHT         " },
		{ CNMS_OBJ_A_RATIO_FIX,				"A_RATIO_FIX            " },
		{ CNMS_OBJ_A_UNIT,					"A_UNIT                 " },
		{ CNMS_OBJ_A_OUTPUT_RESOLUTION,		"A_OUTPUT_RESOLUTION    " },
		{ CNMS_OBJ_A_OUTPUT_SIZE,			"A_OUTPUT_SIZE          " },
		{ CNMS_OBJ_A_OUTPUT_WIDTH,			"A_OUTPUT_WIDTH         " },
		{ CNMS_OBJ_A_OUTPUT_HEIGHT,			"A_OUTPUT_HEIGHT        " },
		{ CNMS_OBJ_A_SCALE,					"A_SCALE                " },
		{ CNMS_OBJ_A_DATA_SIZE,				"A_DATA_SIZE            " },
		{ CNMS_OBJ_A_COLOR_ADJUST,			"A_COLOR_ADJUST         " },
		{ CNMS_OBJ_A_COLOR_CONFIG,			"A_COLOR_CONFIG         " },
		{ CNMS_OBJ_A_UNSHARP_MASK,			"A_UNSHARP_MASK         " },
		{ CNMS_OBJ_A_DESCREEN,				"A_DESCREEN             " },
	};
	CNMSSTRDEBUG modeComp[] = {
		{ CNMS_MODE_DELETE,		"DELETE" },
		{ CNMS_MODE_SELECT,		"SELECT" },
		{ CNMS_MODE_ADD,		"ADD   " },
		{ CNMS_MODE_CHANGE,		"CHANGE" },
		{ CNMS_MODE_HIDE,		"HIDE  " },
	};
	CNMSInt32		i, j, k;
	CNMSLPSTR		lpStr;

	for( i = 0 ; i < linkNum ; i ++ ){
		if( ( activeKey == CNMS_TRUE ) && ( ( lpLink[ i ].mode != CNMS_MODE_SELECT ) && ( lpLink[ i ].mode != CNMS_MODE_HIDE ) ) ){
			continue;
		}
		/* seek object str */
		for( j = 0 ; j < sizeof( objComp ) / sizeof( objComp[ 0 ] ) ; j ++ ){
			if( lpLink[ i ].object == objComp[ j ].val ){
				break;
			}
		}
		if( j == sizeof( objComp ) / sizeof( objComp[ 0 ] ) ){
			DBGMSG( "[CnmsDbgUiInfo]Don't support No.%d's object(%d).\n", i, lpLink[ i ].object );
			goto	EXIT;
		}
		/* seek mode str */
		for( k = 0 ; k < sizeof( modeComp ) / sizeof( modeComp[ 0 ] ) ; k ++ ){
			if( lpLink[ i ].mode == modeComp[ k ].val ){
				break;
			}
		}
		if( k == sizeof( modeComp ) / sizeof( modeComp[ 0 ] ) ){
			DBGMSG( "[CnmsDbgUiInfo]Don't support No.%d's mode(%d).\n", i, lpLink[ i ].mode );
			goto	EXIT;
		}
		/* seek value str */
		lpStr = CnmsChgValToStr( lpLink[ i ].value, lpLink[ i ].object );
		DBGMSG( "[CnmsDbgUiInfo]%2d:%s, %s, %s\n", i, objComp[ j ].str, modeComp[ k ].str, lpStr );
	}
	DBGMSG( "\n" );
EXIT:
	return;
};

#endif	/* __CNMS_DEBUG__ */
#endif	/* _CNMSSTR_C_ */
