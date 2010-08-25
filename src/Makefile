-include Makefile.include

CFLAGS += -I.

TARGETS = libgjk.a
OBJS = gjk.o gjk_support.o

all: $(TARGETS)

libgjk.a: $(OBJS)
	ar cr $@ $(OBJS)
	ranlib $@

%.o: %.c %.h
	$(CC) $(CFLAGS) $(DEFS) -c -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) $(DEFS) -c -o $@ $<

clean:
	rm -f $(OBJS)
	rm -f $(TARGETS)
	if [ -d testsuites ]; then $(MAKE) -C testsuites clean; fi;
	
check:
	$(MAKE) -C testsuites check
check-valgrind:
	$(MAKE) -C testsuites check-valgrind

help:
	@echo "TODO"

.PHONY: all clean check check-valgrind help
