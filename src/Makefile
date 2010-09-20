###
# libgjk
# ---------------------------------
# Copyright (c)2010 Daniel Fiser <danfis@danfis.cz>
#
#
#  This file is part of libgjk.
#
#  Distributed under the OSI-approved BSD License (the "License");
#  see accompanying file BDS-LICENSE for details or see
#  <http://www.opensource.org/licenses/bsd-license.php>.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even the
#  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#  See the License for more information.
##

-include Makefile.include

CFLAGS += -I.

TARGETS = libgjk.a
OBJS = gjk.o support.o gjk_support.o vec3.o alloc.o polytope.o

all: $(TARGETS)

libgjk.a: $(OBJS)
	ar cr $@ $(OBJS)
	ranlib $@

config.h: config.h.in
	$(M4) $(CONFIG_FLAGS) $< >$@

%.o: %.c %.h config.h
	$(CC) $(CFLAGS) $(DEFS) -c -o $@ $<
%.o: %.c config.h
	$(CC) $(CFLAGS) $(DEFS) -c -o $@ $<
%.h: config.h
%.c: config.h

install:
	mkdir -p $(PREFIX)/$(INCLUDEDIR)/gjk
	mkdir -p $(PREFIX)/$(LIBDIR)
	cp *.h $(PREFIX)/$(INCLUDEDIR)/gjk/
	cp libgjk.a $(PREFIX)/$(LIBDIR)

clean:
	rm -f $(OBJS)
	rm -f $(TARGETS)
	rm -f config.h
	if [ -d testsuites ]; then $(MAKE) -C testsuites clean; fi;
	
check:
	$(MAKE) -C testsuites check
check-valgrind:
	$(MAKE) -C testsuites check-valgrind

help:
	@echo "TODO"

.PHONY: all clean check check-valgrind help
