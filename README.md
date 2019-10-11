# libccddbl [![Build Status](https://travis-ci.org/danfis/libccddbl.svg?branch=master)](https://travis-ci.org/danfis/libccddbl)

***libccddbl*** is library for a collision detection between two convex shapes.
libccddbl implements variation on Gilbert–Johnson–Keerthi algorithm plus Expand
Polytope Algorithm (EPA) and also implements algorithm Minkowski Portal
Refinement (MPR, a.k.a. XenoCollide) as described in Game Programming Gems 7.

libccddbl is the only available open source library of my knowledge that include
MPR algorithm working in 3-D space.  However, there is a library called
[mpr2d](http://code.google.com/p/mpr2d/), implemented in D programming
language, that works in 2-D space.

libccddbl is currently part of:

1. [ODE](http://www.ode.org/) library (see ODE's *./configure --help* how to enable it),
2. [FCL](http://www.ros.org/wiki/fcl) library from [Willow Garage](http://www.willowgarage.com/),
3. [Bullet3](http://bulletphysics.org/) library (https://github.com/bulletphysics/bullet3).

For implementation details on GJK algorithm, see
http://www.win.tue.nl/~gino/solid/jgt98convex.pdf.


## Dependencies

This library is currently based only on standard libraries.
The only exception are testsuites that are built on top of CU
(https://github.com/danfis/cu) library licensed under LGPL, however only
testing depends on it and libccddbl library itself can be distributed without it.


## License

libccddbl is licensed under OSI-approved 3-clause BSD License, text of license
is distributed along with source code in BSD-LICENSE file.
Each file should include license notice, the rest should be considered as
licensed under 3-clause BSD License.


## Compile And Install

libccddbl contains several mechanisms for compiling and installing. Using a simple Makefile, using autotools, and using CMake.

### 1. Using Makefile

Directory src/ contains Makefile that should contain everything needed for compilation and installation:
```sh
  $ cd src/
  $ make
  $ make install
```

Library libccddbl is by default compiled in double precision of floating point numbers - you can change this by options *USE_SINGLE/USE_DOUBLE*, i.e.:
```sh
  $ make USE_SINGLE=yes
```
will compile library in single precision.

Installation directory can be changed by options PREFIX, INCLUDEDIR and LIBDIR. 
For more info type 'make help'.

### 2. Using Autotools

libccddbl also contains support for autotools:
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

configure script can change the way libccddbl is compiled and installed, most significant option is *--enable-double-precision* which enables double precision (single is default in this case).

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

To build libccddbl as a shared library, set the `BUILD_SHARED_LIBS` option:
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

This section describes how to use libccddbl for testing if two convex objects intersects (i.e., 'yes/no' test) using Gilbert-Johnson-Keerthi (GJK) algorithm.

Procedure is very simple (and is similar for usages of library):

1. Include *<ccddbl/ccddbl.h>* file.
2. Implement support function for specific shapes. Support function is function that returns furthest point from object (shape) in specified direction.
3. Set up *ccddbl_t* structure.
4. Run ccddblGJKIntersect() function on desired objects.

Here is skeleton of simple program:
```cpp
  #include <ccddbl/ccddbl.h>
  #include <ccddbl/quat.h> // for work with quaternions

  /** Support function for box */
  void support(const void *obj, const ccddbl_vec3_t *dir, ccddbl_vec3_t *vec)
  {
      // assume that obj_t is user-defined structure that holds info about
      // object (in this case box: x, y, z, pos, quat - dimensions of box,
      // position and rotation)
      obj_t *obj = (obj_t *)_obj;
      ccddbl_vec3_t dir;
      ccddbl_quat_t qinv;

      // apply rotation on direction vector
      ccddblVec3Copy(&dir, _dir);
      ccddblQuatInvert2(&qinv, &obj->quat);
      ccddblQuatRotVec(&dir, &qinv);

      // compute support point in specified direction
      ccddblVec3Set(v, ccddblSign(ccddblVec3X(&dir)) * box->x * CCDDBL_REAL(0.5),
                    ccddblSign(ccddblVec3Y(&dir)) * box->y * CCDDBL_REAL(0.5),
                    ccddblSign(ccddblVec3Z(&dir)) * box->z * CCDDBL_REAL(0.5));

      // transform support point according to position and rotation of object
      ccddblQuatRotVec(v, &obj->quat);
      ccddblVec3Add(v, &obj->pos);
  }

  int main(int argc, char *argv[])
  {
      ...

      ccddbl_t ccddbl;
      CCDDBL_INIT(&ccddbl); // initialize ccddbl_t struct

      // set up ccddbl_t struct
      ccddbl.support1       = support; // support function for first object
      ccddbl.support2       = support; // support function for second object
      ccddbl.max_iterations = 100;     // maximal number of iterations

      int intersect = ccddblGJKIntersect(obj1, obj2, &ccddbl);
      // now intersect holds true if obj1 and obj2 intersect, false otherwise
  }
```

## GJK + EPA - Penetration Of Two Objects

If you want to obtain also penetration info about two intersection objects ccddblGJKPenetration() function can be used.

Procedure is almost same as for previous case:
```cpp
  #include <ccddbl/ccddbl.h>
  #include <ccddbl/quat.h> // for work with quaternions

  /** Support function is same as in previous case */

  int main(int argc, char *argv[])
  {
      ...
      ccddbl_t ccddbl;
      CCDDBL_INIT(&ccddbl); // initialize ccddbl_t struct

      // set up ccddbl_t struct
      ccddbl.support1       = support; // support function for first object
      ccddbl.support2       = support; // support function for second object
      ccddbl.max_iterations = 100;     // maximal number of iterations
      ccddbl.epa_tolerance  = 0.0001;  // maximal tolerance fro EPA part

      ccddbl_real_t depth;
      ccddbl_vec3_t dir, pos;
      int intersect = ccddblGJKPenetration(obj1, obj2, &ccddbl, &depth, &dir, &pos);
      // now intersect holds 0 if obj1 and obj2 intersect, -1 otherwise
      // in depth, dir and pos is stored penetration depth, direction of
      // separation vector and position in global coordinate system
  }
```

## MPR - Intersection Test

libccddbl also provides MPR - Minkowski Portal Refinement algorithm that can be used for testing if two objects intersects.

Procedure is similar to the one used for GJK algorithm. Support function is same but also function that returns center (or any point near center) of given object must be implemented:
```cpp
  #include <ccddbl/ccddbl.h>
  #include <ccddbl/quat.h> // for work with quaternions

  /** Support function is same as in previous case */

  /** Center function - returns center of object */
  void center(const void *_obj, ccddbl_vec3_t *center)
  {
      obj_t *obj = (obj_t *)_obj;
      ccddblVec3Copy(center, &obj->pos);
  }

  int main(int argc, char *argv[])
  {
      ...
      ccddbl_t ccddbl;
      CCDDBL_INIT(&ccddbl); // initialize ccddbl_t struct

      // set up ccddbl_t struct
      ccddbl.support1       = support; // support function for first object
      ccddbl.support2       = support; // support function for second object
      ccddbl.center1        = center;  // center function for first object
      ccddbl.center2        = center;  // center function for second object
      ccddbl.mpr_tolerance  = 0.0001;  // maximal tolerance

      int intersect = ccddblMPRIntersect(obj1, obj2, &ccddbl);
      // now intersect holds true if obj1 and obj2 intersect, false otherwise
  }
```


## MPR - Penetration Of Two Objects

Using MPR algorithm for obtaining penetration info about two intersection objects is equally easy as in previous case instead ccddblMPRPenetration() function is used:
```cpp
  #include <ccddbl/ccddbl.h>
  #include <ccddbl/quat.h> // for work with quaternions

  /** Support function is same as in previous case */
  /** Center function is same as in prevous case */

  int main(int argc, char *argv[])
  {
      ...
      ccddbl_t ccddbl;
      CCDDBL_INIT(&ccddbl); // initialize ccddbl_t struct

      // set up ccddbl_t struct
      ccddbl.support1       = support; // support function for first object
      ccddbl.support2       = support; // support function for second object
      ccddbl.center1        = center;  // center function for first object
      ccddbl.center2        = center;  // center function for second object
      ccddbl.mpr_tolerance  = 0.0001;  // maximal tolerance

      ccddbl_real_t depth;
      ccddbl_vec3_t dir, pos;
      int intersect = ccddblMPRPenetration(obj1, obj2, &ccddbl, &depth, &dir, &pos);
      // now intersect holds 0 if obj1 and obj2 intersect, -1 otherwise
      // in depth, dir and pos is stored penetration depth, direction of
      // separation vector and position in global coordinate system
  }
```
