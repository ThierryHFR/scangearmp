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


#ifndef	_CNMSSTRINGS_H_
#define	_CNMSSTRINGS_H_

/* N_ macro */
#include "support.h"


/* for about dialog */
#define	STR_ABOUT_SG			"ScanGear MP"
#define	STR_ABOUT_VERSION		N_( "Version %s" )					//localize
#define	STR_ABOUT_VER_NUM		"2.30"
#define	STR_ABOUT_CR			N_( "Copyright CANON INC. %s-%s" )	//localize
#define	STR_ABOUT_YEAR_BEGIN	"2007"
#define	STR_ABOUT_YEAR_END		"2014"
#define	STR_ABOUT_ARR			""
/* for gimp plugin  */
#define	STR_ABOUT_GIMP_AUTHOR	"CANON INC."
#define	STR_ABOUT_GIMP_CR		"Copyright "
#define	STR_ABOUT_GIMP_LINUX	" for Linux"


/* cnmsstr.c */
	/* STRSUB_S_SOURCE */
	#define	STR_CNMS_S_SOURCE_PHOTO				N_( "Photo (Color)" )			/* Ver.1.80 modified */
	#define	STR_CNMS_S_SOURCE_MAGAZINE			N_( "Magazine (Color)" )		/* Ver.1.80 modified */
	#define	STR_CNMS_S_SOURCE_NEWSPAPER			N_( "Newspaper(B&W)" )
	#define	STR_CNMS_S_SOURCE_DOCUMENT			N_( "Document (Grayscale)" )	/* Ver.1.80 modified */
	#define	STR_CNMS_S_SOURCE_GRAYNEWSPAPER		N_( "Newspaper (Grayscale)" )	/* Ver.1.80 modified */
	#define	STR_CNMS_S_SOURCE_TEXT_COL_SADF		N_( "Document (Color) ADF Simplex" )		/* Ver.1.80 modified */
	#define	STR_CNMS_S_SOURCE_TEXT_GRAY_SADF	N_( "Document (Grayscale) ADF Simplex" )	/* Ver.1.80 modified */
	#define	STR_CNMS_S_SOURCE_TEXT_COL_DADF		N_( "Document (Color) ADF Duplex" )			/* Ver.1.80 modified */
	#define	STR_CNMS_S_SOURCE_TEXT_GRAY_DADF	N_( "Document (Grayscale) ADF Duplex" )		/* Ver.1.80 modified */
	#define	STR_CNMS_S_SOURCE_COLDOCUMENT		N_( "Document (Color)" )		/* Ver.1.80 added */

	/* STRSUB_S_DESTINATION */
	#define	STR_CNMS_S_DESTINATION_PRINT		N_( "Print(300dpi)" )
	#define	STR_CNMS_S_DESTINATION_DISPLAY		N_( "Image display(150dpi)" )
	#define	STR_CNMS_S_DESTINATION_OCR			N_( "OCR(300dpi)" )
	#define	STR_CNMS_S_DESTINATION_PRINT_NDPI	N_( "Print" )
	#define	STR_CNMS_S_DESTINATION_DISPLAY_NDPI	N_( "Image display" )
	#define	STR_CNMS_S_DESTINATION_OCR_NDPI		N_( "OCR" )

	/* STRSUB_S_OUTPUT_SIZE,STRSUB_A_PAPER_SIZE,STRSUB_A_OUTPUT_SIZE */
	#define	STR_CNMS_SIZE_FREE				N_( "Flexible" )
	#define	STR_CNMS_SIZE_CARD				N_( "Card" )
	#define	STR_CNMS_SIZE_L_LA				N_( "L Landscape" )
	#define	STR_CNMS_SIZE_L_PO				N_( "L Portrait" )
	#define	STR_CNMS_SIZE_LL_LA				N_( "2L Landscape" )
	#define	STR_CNMS_SIZE_LL_PO				N_( "2L Portrait" )
	#define	STR_CNMS_SIZE_4_6_LA			N_( "4\" x 6\" Landscape" )
	#define	STR_CNMS_SIZE_4_6_PO			N_( "4\" x 6\" Portrait" )
	#define	STR_CNMS_SIZE_POSTCARD			N_( "Hagaki" )
	#define	STR_CNMS_SIZE_A5				N_( "A5" )
	#define	STR_CNMS_SIZE_B5				N_( "B5" )
	#define	STR_CNMS_SIZE_A4				N_( "A4" )
	#define	STR_CNMS_SIZE_LETTER			N_( "Letter" )
	#define	STR_CNMS_SIZE_LEGAL				N_( "Legal" )
	#define	STR_CNMS_SIZE_B4				N_( "B4" )
	#define	STR_CNMS_SIZE_A3				N_( "A3" )
	#define	STR_CNMS_SIZE_128_128			N_( "128 x 128 pixels" )
	#define	STR_CNMS_SIZE_640_480			N_( "640 x 480 pixels" )
	#define	STR_CNMS_SIZE_800_600			N_( "800 x 600 pixels" )
	#define	STR_CNMS_SIZE_1024_768			N_( "1024 x 768 pixels" )
	#define	STR_CNMS_SIZE_1280_1024			N_( "1280 x 1024 pixels" )
	#define	STR_CNMS_SIZE_1600_1200			N_( "1600 x 1200 pixels" )
	#define	STR_CNMS_SIZE_2048_1536			N_( "2048 x 1536 pixels" )
	#define	STR_CNMS_SIZE_2272_1704			N_( "2272 x 1704 pixels" )
	#define	STR_CNMS_SIZE_2592_1944			N_( "2592 x 1944 pixels" )
	/* only STRSUB_A_PAPER_SIZE */
	#define	STR_CNMS_SIZE_FULL_PAGE			N_( "Full Platen" )

	/* STRSUB_A_SOURCE */
	#define	STR_CNMS_A_SOURCE_PLATEN		N_( "Platen" )
	#define	STR_CNMS_A_SOURCE_TEXT_SADF		N_( "Document (ADF Simplex)" )
	#define	STR_CNMS_A_SOURCE_TEXT_DADF		N_( "Document (ADF Duplex)" )

	/* STRSUB_A_BINDING_LOCATION */
	#define	STR_CNMS_LONG_EDGE				N_( "Long Edge" )
	#define	STR_CNMS_SHORT_EDGE				N_( "Short Edge" )

	/* STRSUB_A_COLOR_MODE */
	#define	STR_CNMS_A_COLOR_MODE_COLOR				N_( "Color" )
	#define	STR_CNMS_A_COLOR_MODE_COLOR_DOCUMENTS	N_( "Color(Documents)" )
	/* add space */
	#define	STR_CNMS_A_COLOR_MODE_GRAY				N_( "Grayscale " )
	#define	STR_CNMS_A_COLOR_MODE_MONO				N_( "Black and White" )

	/* STRSUB_A_UNIT */
	#define	STR_CNMS_A_UNIT_PIXEL			N_( "pixels" )
	#define	STR_CNMS_A_UNIT_INCH			N_( "inches" )
	#define	STR_CNMS_A_UNIT_CENTIMETER		N_( "centimeters" )
	#define	STR_CNMS_A_UNIT_MILLIMETER		N_( "mm" )

	/* ON,OFF */
	#define	STR_CNMS_ON						N_( "ON" )
	#define	STR_CNMS_OFF					N_( "OFF" )

/* coloradjuststr.h */
	/* Channel list strings */
	#define	STR_CH_LIST_ITEM_MASTER			N_("Master")
	#define	STR_CH_LIST_ITEM_RED			N_("Red")
	#define	STR_CH_LIST_ITEM_GREEN			N_("Green")
	#define	STR_CH_LIST_ITEM_BLUE			N_("Blue")
	#define	STR_CH_LIST_ITEM_GRAYSCALE		N_("Grayscale")

	/* Tone Curve strings */
	#define	STR_TC_LIST_ITEM_NO_AJUST		N_("No correction")
	#define	STR_TC_LIST_ITEM_EXP_OVER		N_("Overexposure")
	#define	STR_TC_LIST_ITEM_EXP_UNDER		N_("Underexposure")
	#define	STR_TC_LIST_ITEM_HIGH_CONT		N_("High contrast")

/* save_dialog.c */
	#define	MSG_SAVE_ERROR_NO_INPUT_FNAME	N_("The file name is not specified.")
	#define	MSG_SAVE_ERROR_NO_ACCESS_PERM	N_("You do not have permission to access the specified file.\nSpecify another file name or save the file in another directory.")
	#define	MSG_SAVE_ERROR_INVALID_DIR		N_("The specified directory does not exist or you do not have permission to access the directory.\nSave the file in another directory.")
	#define	MSG_SAVE_ERROR_SERIOUS_ERROR1	N_("The specified file name is invalid.\nSpecify another file name.")

/* child_dialog.c */
	#define	MSG_CHILD_DLG_NO_MESS			N_( "Do not display this message again." )
	
	#define	MSG_CHILD_DLG_RESET				N_( "Resets the settings to default." )
	#define	MSG_CHILD_DLG_100MBOVER			N_( "You are trying to scan an image exceeding 100 MB. \nScanning images of this size puts a considerable load on the computer, \nand some applications may be unable to load the image. \nAre you sure you want to continue?" )
	#define	MSG_CHILD_DLG_OVER_PIXEL		N_( "Scanning cannot be performed unless the crop size, \noutput size or output resolution is reduced to \n%s x %s pixels or less." )
	#define	MSG_CHILD_DLG_WARN_MOIRE		N_( "If you keep Unsharp Mask ON, Descreen may not work." )
	#define	MSG_CHILD_DLG_NO_PAPER			N_( "The ADF (Auto Document Feeder) is empty. \nPlace paper in the ADF." )
	#define	MSG_CHILD_DLG_NOT_FOUND			N_( "Cannot find available scanners.\n\nCable may be disconnected or scanner may be turned off.\nCheck the scanner status, and then try again." )
	#define	MSG_CHILD_DLG_OVER_DATASIZE		N_( "You are trying to scan a large image.\nReduce image size or output resolution,\nwhen you want to scan this image." )
	#define	MSG_CHILD_DLG_SETTING_APPLY		N_( "Apply settings to printer?" )
	#define	MSG_CHILD_DLG_AUTOPOWEROFF_APPLY		N_( "Apply the setting to printer?\nIf the printer is connected to a network, it will not turn off automatically even if Auto Power Off is set." )

/* progress_bar.c */
	#define	STR_PROGRESSBAR_ID_SCAN				N_( "Scanning..." )
	#define	STR_PROGRESSBAR_ID_SAVE				N_( "Saving..." )
	#define	STR_PROGRESSBAR_ID_TRANSFER_GIMP	N_( "Transferring..." )
	#define	STR_PROGRESSBAR_PAGES				N_( "%d page(s)" )
	/* Ver.1.40 */
	#define	STR_PROGRESSBAR_ID_SEARCH			N_( "Searching..." )
	#define	STR_PROGRESSBAR_MESS_SEARCH			N_( "Searching for scanners." )
	#define	STR_PROGRESSBAR_MESS_FINISH_SEARCH	N_( "Updating the scanner list." )
	#define	STR_PROGRESSBAR_BUTTON_CANCEL		N_( "Cancel" )

/* scanmsg.h */
	#define	MSG_ERR_CANCELD_BY_STOP				N_( "Scanning canceled by user." )
	#define	MSG_ERR_SCANNER_BUSY_COPYING		N_( "Copying...  Scan after it is complete." )
	#define	MSG_ERR_SCANNER_BUSY_PRINTING		N_( "Printing...  Scan after it is complete." )
	#define	MSG_ERR_SCANNER_BUSY_PRINTERMNT		N_( "Printer maintenance is in progress. Scan after it is complete." )
	#define	MSG_ERR_NOT_FIND_SCANNER			N_( "Cannot communicate with scanner.\nCable may be disconnected or scanner may be turned off.\nCheck status.\nScanner driver will be closed." )
	#define	MSG_ERR_DISK_FULL					N_( "There is not enough free disk space to complete the scan.\nScanner driver will be closed." )
	#define	MSG_ERR_INTERNAL					N_( "Internal Error occurred.\nScanner driver will be closed." )
	/* Ver.1.40 modified */
	#define	MSG_ERR_FATAL						N_( "Error in scanner.\nTurn off scanner and follow instructions in User's Guide.\nScanner driver will be closed." )
	/* Ver.1.20, Ver.1.90 modified */
	#define	MSG_ERR_DEVICE_NOT_AVAILABLE		N_( "Printer is in use or an error has occurred. Check status.\nScanner driver will be closed." )
	#define	MSG_ERR_DEVICE_INITIALIZE			N_( "Printer initialization is in progress. Scan after it is complete." )
	#define	MSG_ERR_SYSTEM						N_( "An error has occurred.\nTurn the device off and then back on.\nScanner driver will be closed." )
	/* Ver.1.30, Ver.1.70 modified */
	#define	MSG_ERR_CANCELED_ADF				N_( "Scanning canceled by user. \nPress the OK button on the printer's operation panel \nto eject the document from the ADF (Auto Document Feeder)." )
	#define	MSG_ERR_ADF_JAM						N_( "The paper is jammed in the ADF (Auto Document Feeder). \nOpen the cover and clear the paper jam, then close the document cover.\nPress the OK button to clear the status on the printer's operation panel." )
	#define	MSG_ERR_ADF_COVER_OPEN				N_( "The ADF (Auto Document Feeder) cover is open. \nClose the document cover and press the OK button \nto clear the status on the printer's operation panel." )
	#define	MSG_ERR_NO_PAPER					N_( "No document in the ADF (Auto Document Feeder). \nPress the OK button on the printer's operation panel to cancel the error, \nthen place paper in the ADF." )
	#define	MSG_ERR_PAPER_FEED					N_( "Document is too long.\nCheck that the ADF (Auto Document Feeder) cover is closed, \nand press the OK button on the printer's operation panel \nto eject the document from the ADF." )
	#define	MSG_ERR_MISMATCH_SIZE				N_( "The paper size is not suitable for the duplex scanning. \nCheck that the ADF (Auto Document Feeder) cover is closed, \nand press the OK button on the printer's operation panel \nto eject the document from the ADF." )
	/* Ver.1.40 */
	#define	MSG_ERR_NOT_FIND_SCANNER_LAN		N_( "Cannot communicate with scanner for these reasons:\n- Scanner is turned off.\n- Scanner is disconnected.\n(It is disconnected from LAN, if using on LAN.)\n\nPlease check and try again.\nScanner driver will be closed." )
	/* Ver.1.60 */
	#define	MSG_ERR_NOT_FIND_SCANNER_LAN_LOCK	N_( "Cannot communicate with scanner for these reasons:\n- Scanner is turned off.\n- The lock switch is not released.\n- Scanner is disconnected.\n(It is disconnected from LAN, if using on LAN.)\n\nPlease check and try again.\nScanner driver will be closed." )
	#define	MSG_ERR_SCANNER_LOCKED				N_( "Release the lock switch, turn the device off and then back on.\nScanner driver will be closed." )
	/* Ver.1.70, Ver.1.90 modified */
	#define	MSG_SCANNER_CONNECT_FAILED_USB		N_( "Cannot communicate with scanner for these reasons:\n- Scanner is turned off.\n- USB cable is disconnected.\n\nPlease check and try again.\nScanner driver will be closed." )
	#define	MSG_SCANNER_CONNECT_FAILED_LAN		N_( "Cannot communicate with scanner for these reasons:\n- Scanner is turned off.\n- (If using LAN connection) It is disconnected from LAN.\n- (If using wireless LAN connection) Signal strength is poor due to obstructions.\n- Network connection is prohibited by security software.\n- Different scanner on the network is selected.\n\nPlease check and try again.\nScanner driver will be closed." )

/* scanflow.c */
	#define	MSG_EVERY_CALIBRATION_ON			N_( "Processing calibration data from a scanner.\nDo not open the document cover." )
	#define	MSG_EVERY_CALIBRATION_OFF			N_( "Scanner is warming up...\nDo not open the document cover." )

/* w1_main.c */
	#define	MSG_CLEAR_PREVIEW_NORMAL			N_( "Delete preview images." )
	#define	MSG_CLEAR_PREVIEW_SOURCE_CHANGE		N_( "Select Source is altered, so deletes preview image." )

/* preference.c */
	#define	MSG_PREF_GET_SETTINGS					N_( "Collecting printer status about maintenance.\nPlease wait." )
	#define	MSG_PREF_SET_SETTINGS					N_( "Sending printer settings about maintenance.\nPlease wait." )
	#define	MSG_PREF_QUIET_RADIO_OFF				N_( "Do not use quiet mode" )
	#define	MSG_PREF_QUIET_RADIO_ON					N_( "Always use quiet mode" )
	#define	MSG_PREF_QUIET_RADIO_TIMER				N_( "Use quiet mode during specified hours" )
	#define	MSG_PREF_QUIET_TIMER_START				N_( "Start time :" )
	#define	MSG_PREF_QUIET_TIMER_END				N_( "End time :" )
	#define	MSG_PREF_QUIET_MESS						N_( "When the hours are specified, quiet mode is not applied to operations\n(copy, direct print, etc.) performed directly from the printer." )

	#define	STR_PREF_ENABLE							N_( "Enable" )
	#define	STR_PREF_DISABLE						N_( "Disable" )
	#define	STR_PREF_15_MINUTES						N_( "15 minutes" )
	#define	STR_PREF_30_MINUTES						N_( "30 minutes" )
	#define	STR_PREF_60_MINUTES						N_( "60 minutes" )
	#define	STR_PREF_120_MINUTES					N_( "120 minutes" )
	#define	STR_PREF_240_MINUTES					N_( "240 minutes" )


#endif /* _CNMSSTRINGS_H_ */

