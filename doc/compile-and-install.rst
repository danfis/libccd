Compile And Install
====================

libccd contains several mechanisms for compiling and installing.
Using a simple Makefile, using autotools, and using CMake.


1. Using Makefile
------------------
Directory ``src/`` contains Makefile that should contain everything needed for compilation and installation:

.. code-block:: bash

    $ cd src/
    $ make
    $ make install

Library libccd is by default compiled in double precision of floating point
numbers - you can change this by options ``USE_SINGLE``/``USE_DOUBLE``, i.e.:

.. code-block:: bash

    $ make USE_SINGLE=yes

will compile library in single precision.
Installation directory can be changed by options ``PREFIX``, ``INCLUDEDIR``
and ``LIBDIR``. 
For more info type '``make help``'.


2. Using Autotools
-------------------
libccd also contains support for autotools:
Generate configure script etc.:

.. code-block:: bash

    $ ./bootstrap

Create new ``build/`` directory:

.. code-block:: bash

    $ mkdir build && cd build

Run configure script:

.. code-block:: bash

    $ ../configure

Run make and make install:

.. code-block:: bash

    $ make && make install

configure script can change the way libccd is compiled and installed, most
significant option is ``--enable-double-precision`` which enables double
precision (single is default in this case).

3. Using CMake
---------------

To build using ``make``:

.. code-block:: bash

    $ mkdir build && cd build
    $ cmake -G "Unix Makefiles" ..
    $ make && make install

To build using ``ninja``:

.. code-block:: bash

    $ mkdir build && cd build
    $ cmake -G Ninja ..
    $ ninja && ninja install

Other build tools may be using by specifying a different generator. For example:

.. code-block:: bash

    $ cmake -G Xcode ..

.. code-block:: batch

    > cmake -G "Visual Studio 14 2015" ..

To compile using double precision, set the ``ENABLE_DOUBLE_PRECISION`` option:

.. code-block:: bash

    $ mkdir build && cd build
    $ cmake -G "Unix Makefiles" -DENABLE_DOUBLE_PRECISION=ON ..
    $ make && make install

To build libccd as a shared library, set the ``BUILD_SHARED_LIBS`` option:

.. code-block:: bash

    $ mkdir build && cd build
    $ cmake -G "Unix Makefiles" -DBUILD_SHARED_LIBS=ON ..
    $ make && make install

To build the test suite, set the ``BUILD_TESTING`` option:

.. code-block:: bash

    $ mkdir build && cd build
    $ cmake -G "Unix Makefiles" -DBUILD_TESTING=ON ..
    $ make && make test


The installation directory may be changed by specifying the ``CMAKE_INSTALL_PREFIX`` variable:

.. code-block:: bash

    $ mkdir build && cd build
    $ cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/path/to/install ..
    $ make && make install
