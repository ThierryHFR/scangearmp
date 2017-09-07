## Process this file with automake to produce Makefile.in
BACKEND_V_MAJOR=2
BACKEND_V_MINOR=3
BACKEND_V_REV=0

INCLUDES = \
	-I. \
	-I./include

exec_backenddir = $(enable_libpath)
exec_backend_LTLIBRARIES = libsane-canon_mfp.la

libsane_canon_mfp_la_SOURCES = \
	canon_mfp.c canon_mfp.h \
	canon_mfp_tools.c canon_mfp_tools.h \
	include/cnmslld.h \
	libcnnet.h canon_mfp_debug.h

libsane_canon_mfp_la_CFLAGS = -DV_MAJOR=$(BACKEND_V_MAJOR) -DV_MINOR=$(BACKEND_V_MINOR)
libsane_canon_mfp_la_LIBADD = $(LIBLTDL) -lusb -ldl -lcncpnet -lpthread
libsane_canon_mfp_la_LDFLAGS = -version-info $(BACKEND_V_MAJOR):$(BACKEND_V_MINOR):$(BACKEND_V_REV)

EXTRA_DIST = \
	canon_mfp.conf canon_mfp_opts.h

install-data-am:
	if test ! -d $(DESTDIR)$(exec_backenddir)/bjlib; then mkdir $(DESTDIR)$(exec_backenddir)/bjlib ; fi ; \
	$(INSTALL_DATA) canon_mfp.conf $(DESTDIR)$(exec_backenddir)/bjlib
