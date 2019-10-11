SUBDIRS = . testsuites

lib_LTLIBRARIES = libccddbl.la

libccddbl_la_SOURCES = alloc.h \
		ccddbl/compiler.h \
		dbg.h \
		ccddbl.c ccddbl/ccddbl.h \
		ccddbl/ccddbl_export.h \
		list.h \
		polytope.c polytope.h \
		ccddbl/quat.h \
		ccddbl/simplex.h \
		support.c ccddbl/support.h \
		vec3.c ccddbl/vec3.h \
        mpr.c

libccddbl_la_CFLAGS = -fvisibility=hidden
