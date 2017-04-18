# libccd [![Build Status](https://travis-ci.org/danfis/libccd.svg?branch=master)](https://travis-ci.org/danfis/libccd)

***libccd*** is library for a collision detection between two convex shapes.
libccd implements variation on Gilbert–Johnson–Keerthi algorithm plus Expand
Polytope Algorithm (EPA) and also implements algorithm Minkowski Portal
Refinement (MPR, a.k.a. XenoCollide) as described in Game Programming Gems 7.

libccd is the only available open source library of my knowledge that include
MPR algorithm working in 3-D space.  However, there is a library called
[mpr2d](http://code.google.com/p/mpr2d/), implemented in D programming
language, that works in 2-D space.

libccd is currently part of:

1. [ODE](http://www.ode.org/) library (see ODE's *./configure --help* how to enable it),
2. [FCL](http://www.ros.org/wiki/fcl) library from [Willow Garage](http://www.willowgarage.com/),
3. [Bullet3](http://bulletphysics.org/) library (https://github.com/bulletphysics/bullet3).

For implementation details on GJK algorithm, see
http://www.win.tue.nl/~gino/solid/jgt98convex.pdf.


## Dependencies

This library is currently based only on standard libraries.
The only exception are testsuites that are built on top of CU
(https://github.com/danfis/cu) library licensed under LGPL, however only
testing depends on it and libccd library itself can be distributed without it.


## License

libccd is licensed under OSI-approved 3-clause BSD License, text of license
is distributed along with source code in BSD-LICENSE file.
Each file should include license notice, the rest should be considered as
licensed under 3-clause BSD License.


## Compile And Install

libccd contains several mechanisms for compiling and installing. Using a simple Makefile, using autotools, and using CMake.

### 1. Using Makefile

Directory src/ contains Makefile that should contain everything needed for compilation and installation:
```sh
  $ cd src/
  $ make
  $ make install
```

Library libccd is by default compiled in double precision of floating point numbers - you can change this by options *USE_SINGLE/USE_DOUBLE*, i.e.:
```sh
  $ make USE_SINGLE=yes
```
will compile library in single precision.

Installation directory can be changed by options PREFIX, INCLUDEDIR and LIBDIR. 
For more info type 'make help'.

### 2. Using Autotools

libccd also contains support for autotools:
Generate configure script etc.:
```sh
  $ ./bootstrap
```

Create new build/ directory:
```sh
  $ mkdir build && cd build
```

Run configure script:
```sh
  $ ../configure
```

Run make and make install:
```sh
  $ make && make install
```

configure script can change the way libccd is compiled and installed, most significant option is *--enable-double-precision* which enables double precision (single is default in this case).

### 3. Using CMake

To build using `make`:
```sh
  $ mkdir build && cd build
  $ cmake -G "Unix Makefiles" ..
  $ make && make install
```

To build using `ninja`:
```sh
  $ mkdir build && cd build
  $ cmake -G Ninja ..
  $ ninja && ninja install
```

Other build tools may be using by specifying a different generator. For example:
```sh
  $ cmake -G Xcode ..
```

```bat
  > cmake -G "Visual Studio 14 2015" ..
```

To compile using double precision, set the `ENABLE_DOUBLE_PRECISION` option:
```sh
  $ mkdir build && cd build
  $ cmake -G "Unix Makefiles" -DENABLE_DOUBLE_PRECISION=ON ..
  $ make && make install
```

To build libccd as a shared library, set the `BUILD_SHARED_LIBS` option:
```sh
  $ mkdir build && cd build
  $ cmake -G "Unix Makefiles" -DBUILD_SHARED_LIBS=ON ..
  $ make && make install
```

To build the test suite, set the `BUILD_TESTING` option:
```sh
  $ mkdir build && cd build
  $ cmake -G "Unix Makefiles" -DBUILD_TESTING=ON ..
  $ make && make test
```

The installation directory may be changed using the `CMAKE_INSTALL_PREFIX` variable:
```sh
  $ mkdir build && cd build
  $ cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/path/to/install ..
  $ make && make install
```

## GJK - Intersection Test

This section describes how to use libccd for testing if two convex objects intersects (i.e., 'yes/no' test) using Gilbert-Johnson-Keerthi (GJK) algorithm.

Procedure is very simple (and is similar for usages of library):

1. Include *<ccd/ccd.h>* file.
2. Implement support function for specific shapes. Support function is function that returns furthest point from object (shape) in specified direction.
3. Set up *ccd_t* structure.
4. Run ccdGJKIntersect() function on desired objects.

Here is skeleton of simple program:
```cpp
  #include <ccd/ccd.h>
  #include <ccd/quat.h> // for work with quaternions

  /** Support function for box */
  void support(const void *obj, const ccd_vec3_t *dir, ccd_vec3_t *vec)
  {
      // assume that obj_t is user-defined structure that holds info about
      // object (in this case box: x, y, z, pos, quat - dimensions of box,
      // position and rotation)
      obj_t *obj = (obj_t *)_obj;
      ccd_vec3_t dir;
      ccd_quat_t qinv;

      // apply rotation on direction vector
      ccdVec3Copy(&dir, _dir);
      ccdQuatInvert2(&qinv, &obj->quat);
      ccdQuatRotVec(&dir, &qinv);

      // compute support point in specified direction
      ccdVec3Set(v, ccdSign(ccdVec3X(&dir)) * box->x * CCD_REAL(0.5),
                    ccdSign(ccdVec3Y(&dir)) * box->y * CCD_REAL(0.5),
                    ccdSign(ccdVec3Z(&dir)) * box->z * CCD_REAL(0.5));

      // transform support point according to position and rotation of object
      ccdQuatRotVec(v, &obj->quat);
      ccdVec3Add(v, &obj->pos);
  }

  int main(int argc, char *argv[])
  {
      ...

      ccd_t ccd;
      CCD_INIT(&ccd); // initialize ccd_t struct

      // set up ccd_t struct
      ccd.support1       = support; // support function for first object
      ccd.support2       = support; // support function for second object
      ccd.max_iterations = 100;     // maximal number of iterations

      int intersect = ccdGJKIntersect(obj1, obj2, &ccd);
      // now intersect holds true if obj1 and obj2 intersect, false otherwise
  }
```

## GJK + EPA - Penetration Of Two Objects

If you want to obtain also penetration info about two intersection objects ccdGJKPenetration() function can be used.

Procedure is almost same as for previous case:
```cpp
  #include <ccd/ccd.h>
  #include <ccd/quat.h> // for work with quaternions

  /** Support function is same as in previous case */

  int main(int argc, char *argv[])
  {
      ...
      ccd_t ccd;
      CCD_INIT(&ccd); // initialize ccd_t struct

      // set up ccd_t struct
      ccd.support1       = support; // support function for first object
      ccd.support2       = support; // support function for second object
      ccd.max_iterations = 100;     // maximal number of iterations
      ccd.epa_tolerance  = 0.0001;  // maximal tolerance fro EPA part

      ccd_real_t depth;
      ccd_vec3_t dir, pos;
      int intersect = ccdGJKPenetration(obj1, obj2, &ccd, &depth, &dir, &pos);
      // now intersect holds 0 if obj1 and obj2 intersect, -1 otherwise
      // in depth, dir and pos is stored penetration depth, direction of
      // separation vector and position in global coordinate system
  }
```

## MPR - Intersection Test

libccd also provides MPR - Minkowski Portal Refinement algorithm that can be used for testing if two objects intersects.

Procedure is similar to the one used for GJK algorithm. Support function is same but also function that returns center (or any point near center) of given object must be implemented:
```cpp
  #include <ccd/ccd.h>
  #include <ccd/quat.h> // for work with quaternions

  /** Support function is same as in previous case */

  /** Center function - returns center of object */
  void center(const void *_obj, ccd_vec3_t *center)
  {
      obj_t *obj = (obj_t *)_obj;
      ccdVec3Copy(center, &obj->pos);
  }

  int main(int argc, char *argv[])
  {
      ...
      ccd_t ccd;
      CCD_INIT(&ccd); // initialize ccd_t struct

      // set up ccd_t struct
      ccd.support1       = support; // support function for first object
      ccd.support2       = support; // support function for second object
      ccd.center1        = center;  // center function for first object
      ccd.center2        = center;  // center function for second object
      ccd.mpr_tolerance  = 0.0001;  // maximal tolerance

      int intersect = ccdMPRIntersect(obj1, obj2, &ccd);
      // now intersect holds true if obj1 and obj2 intersect, false otherwise
  }
```


## MPR - Penetration Of Two Objects

Using MPR algorithm for obtaining penetration info about two intersection objects is equally easy as in previous case instead ccdMPRPenetration() function is used:
```cpp
  #include <ccd/ccd.h>
  #include <ccd/quat.h> // for work with quaternions

  /** Support function is same as in previous case */
  /** Center function is same as in prevous case */

  int main(int argc, char *argv[])
  {
      ...
      ccd_t ccd;
      CCD_INIT(&ccd); // initialize ccd_t struct

      // set up ccd_t struct
      ccd.support1       = support; // support function for first object
      ccd.support2       = support; // support function for second object
      ccd.center1        = center;  // center function for first object
      ccd.center2        = center;  // center function for second object
      ccd.mpr_tolerance  = 0.0001;  // maximal tolerance

      ccd_real_t depth;
      ccd_vec3_t dir, pos;
      int intersect = ccdMPRPenetration(obj1, obj2, &ccd, &depth, &dir, &pos);
      // now intersect holds 0 if obj1 and obj2 intersect, -1 otherwise
      // in depth, dir and pos is stored penetration depth, direction of
      // separation vector and position in global coordinate system
  }
```
