#!/bin/bash

# Creates .tar.gz package of specified version.
# Takes one argument - identification of commit

NAME=libgjk
COMMIT=""
NOTEST=0
NODOC=0
CMD="git archive"

FILES_SRC="
COPYING
COPYING.LESSER
src/alloc.c
src/alloc.h
src/compiler.h
src/dbg.h
src/.gitignore
src/gjk
src/gjk.c
src/gjk.h
src/gjk_support.c
src/gjk_support.h
src/list.h
src/Makefile
src/Makefile.include
src/polytope.c
src/polytope.h
src/quat.h
src/simplex.h
src/vec3.c
src/vec3.h
"

FILES_DOC="
doc/DistancePoint3Triangle3.pdf
doc/jgt98convex.pdf
"

FILES_TEST="
src/testsuites/boxbox.c
src/testsuites/boxbox.h
src/testsuites/cu/check-regressions
src/testsuites/cu/COPYING
src/testsuites/cu/COPYING.LESSER
src/testsuites/cu/cu.c
src/testsuites/cu/cu.h
src/testsuites/cu/.dir
src/testsuites/cu/.gitignore
src/testsuites/cu/latest.sh
src/testsuites/cu/Makefile
src/testsuites/cylcyl.c
src/testsuites/cylcyl.h
src/testsuites/.gitignore
src/testsuites/main.c
src/testsuites/Makefile
src/testsuites/polytope.c
src/testsuites/polytope.h
src/testsuites/regressions/.dir
src/testsuites/regressions/TSBoxBox.err
src/testsuites/regressions/TSBoxBox.out
src/testsuites/regressions/TSCylCyl.err
src/testsuites/regressions/TSCylCyl.out
src/testsuites/regressions/TSPt.err
src/testsuites/regressions/TSPt.out
src/testsuites/regressions/TSSphereSphere.err
src/testsuites/regressions/TSSphereSphere.out
src/testsuites/regressions/TSVec3.err
src/testsuites/regressions/TSVec3.out
src/testsuites/spheresphere.c
src/testsuites/spheresphere.h
src/testsuites/vec3.c
src/testsuites/vec3.h
"


# read arguments
COMMIT="$1"
while shift; do
    [ "$1" = "--notest" ] && NOTEST=1;
    [ "$1" = "--nodoc" ] && NODOC=1;
done;

if [ "$COMMIT" = "" ]; then
    echo "Usage: $0 commit [--notest] [--nodoc]"
    echo "Error: you must specify commit which should be packed"
    exit -1;
fi;


PREFIX=${NAME}-$COMMIT/
FN=${NAME}-$COMMIT.tar.gz

if echo "$COMMIT" | grep '^v[0-9]\.[0-9]\+' >/dev/null 2>&1; then
    tmp=$(echo "$COMMIT" | sed 's/^v//')
    PREFIX=${NAME}-$tmp/
    FN=${NAME}-$tmp.tar.gz
fi

FILES=$FILES_SRC
[ "$NODOC" != "1" ] && FILES="$FILES $FILES_DOC";
[ "$NOTEST" != "1" ] && FILES="$FILES $FILES_TEST";

$CMD --prefix="$PREFIX" --format=tar $COMMIT -- $FILES | gzip >"$FN"
echo "Package: $FN"

