SUBDIRS = . testsuites

lib_LTLIBRARIES = libccd.la

libccd_la_SOURCES = alloc.h \
		ccd/compiler.h \
		dbg.h \
		ccd.c ccd/ccd.h \
		ccd/ccd_export.h \
		list.h \
		polytope.c polytope.h \
		ccd/quat.h \
		simplex.h \
		support.c support.h \
		vec3.c ccd/vec3.h \
        mpr.c

nobase_include_HEADERS = \
	ccd/ccd_export.h \
	ccd/ccd.h \
	ccd/compiler.h \
	ccd/config.h \
	ccd/quat.h \
	ccd/vec3.h

libccd_la_CFLAGS = -fvisibility=hidden
