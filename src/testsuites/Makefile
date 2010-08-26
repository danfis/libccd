# force some options
DEBUG = yes

-include ../Makefile.include
CFLAGS += -I./ -I../ -Icu/
LDFLAGS += -L./ -Lcu/ -lcu -lm -L../ -lgjk

CHECK_REG=cu/check-regressions

OBJS = boxbox.o spheresphere.o


all: test

test: cu $(OBJS) main.c
	$(CC) $(CFLAGS) -o $@ main.c $(OBJS) $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

check: all
	@echo ""
	@echo "----------------------------------------";
	./test
	@echo "----------------------------------------";
	@echo "Checking regressions:";
	./$(CHECK_REG) regressions
	@echo ""

check-valgrind: all
	valgrind -q --leak-check=full --show-reachable=yes --trace-children=yes \
             --suppressions=opengl.supp \
             --suppressions=fglrx.supp \
             --suppressions=gen.supp \
             --gen-suppressions=all --log-file=out --error-limit=no \
             ./test

check-valgrind-gen-suppressions: all
	valgrind -q --leak-check=full --show-reachable=yes --trace-children=yes \
             --suppressions=opengl.supp \
             --suppressions=fglrx.supp \
             --suppressions=gen.supp \
             --gen-suppressions=all --log-file=out --error-limit=no \
             ./test

cu:
	$(MAKE) -C cu/

clean:
	rm -f *.o
	rm -f objs/*.o
	rm -f test
	rm -f tmp.*
	rm -f regressions/tmp.*

.PHONY: all clean check check-valgrind cu
	
