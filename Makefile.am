SUBDIRS = checkmate tests

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS  = ${ACLOCAL_FLAGS}

checkmate_includedir = $(includedir)/checkmate-$(CHECKMATE_API_VERSION)
nobase_checkmate_include_HEADERS = checkmate/tester.h \
                                   checkmate/checkmate.h \
                                   checkmate/easy/easy.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = checkmate-$(CHECKMATE_API_VERSION).pc

dist_noinst_SCRIPTS = autogen.sh
