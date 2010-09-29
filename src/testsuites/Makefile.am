SUBDIRS = cu

AM_CPPFLAGS = -I $(srcdir)/.. -I $(srcdir)/cu

LDADD = $(builddir)/cu/libcu.la $(builddir)/../libgjk.la


check_PROGRAMS = test bench

test_SOURCES = main.c \
		common.c common.h \
		vec3.c vec3.h \
		polytope.c polytope.h \
		boxbox.c boxbox.h \
		spheresphere.c spheresphere.h \
		cylcyl.c cylcyl.h \
		boxcyl.c boxcyl.h

bench_SOURCES = bench.c

