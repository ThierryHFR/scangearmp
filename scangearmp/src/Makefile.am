## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/backend \
	@PACKAGE_CFLAGS@ \
	@GIMP_CFLAGS@

bin_PROGRAMS = scangearmp

scangearmp_SOURCES = \
	main.c \
	callbacks.c callbacks.h \
	interface.c interface.h \
	support.c support.h \
	coloradjust.c coloradjust.h coloradjuststr.h \
	coloradjust_bc.c coloradjust_bc.h \
	coloradjust_hg.c coloradjust_hg.h \
	coloradjust_tc.c coloradjust_tc.h \
	coloradjust_fr.c coloradjust_fr.h \
	coloradjust_mc.c coloradjust_mc.h \
	com_gtk.c com_gtk.h \
	cnmsstr.c cnmsstr.h \
	w1_main.c w1_size.c w1_ui.c w1.h \
	preference.c preference.h \
	save_dialog.c save_dialog.h \
	prev_main.c prev_main.h \
	prev_sub.c prev_sub.h \
	scanfile.c scanfile.h \
	scangimp.c scangimp.h \
	scanmain.c scanmain.h \
	scanopts.c scanopts.h \
	scanstdlg.c scanstdlg.h \
	scanimage.h scanmsg.h \
	scanflow.c scanflow.h \
	cnmsfunc.c cnmsfunc.h \
	progress_bar.c progress_bar.h \
	keep_setting.c keep_setting.h \
	child_dialog.c child_dialog.h \
	file_control.c file_control.h \
	raw2pdf.c raw2pdf.h \
	cnmsnetwork.c cnmsnetwork.h


scangearmp_LDADD = @PACKAGE_LIBS@ -lcncpmsui -lsane-canon_mfp -lcncpnet $(INTLLIBS) -ldl -lm

scangearmp_CFLAGS = -DV_MAJOR=1 -DV_MINOR=0

scangearmp_LDFLAGS = -L../backend/.libs

