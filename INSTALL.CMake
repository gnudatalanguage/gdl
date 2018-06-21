Be sure that you have at least CMake 3.1 (cmake --version)
(CMake 3.2 required for OSX) (but: see note at end)

Create a new directory where the objects and executables 
will be built, e.g.
$ mkdir build; cd !$

Then run cmake pointing it to the root directory of gdl:
$ cmake ..

You can optionally specify a custom install prefix, e.g.:
$ cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/../install

The default build mode is "Release" which enables compiler
optimisations and disables storing debugging symbols in 
the GDL binary. To use the "Debug" mode type:
$ cmake .. -DCMAKE_BUILD_TYPE=Debug

To use a non-default C++ compiler, one needs to call e.g.:
$ cmake .. -DCMAKE_CXX_COMPILER=clang++

Numerous features of GDL rely on external libraries. These
features can be enabled or disabled using CMake options.

Most of the optional features have a flag that controls
enabling/disabling the feature (e.g. EIGEN3, FFTW, HDF5, 
MAGICK, PSLIB, GRAPHICSMAGICK, GRIB, GSHHS, HDF, ...) 
and a flag that allows to specify a custom location of a
given package (e.g. EIGEN3DIR, FFTWDIR, HDF5DIR, ...). 
Both the on/off and the path-spec options are passed 
to CMake as command=line options prefixed with "-D", e.g.:
$ cmake .. -DREADLINE=OFF'         # to disable readline
$ cmake .. -DFFTWDIR=/opt/local/   # alternative FFTW path

Other options include:
- PYTHONVERSION to chose a particular Python version if 
  multiple are installed on the system,
- GDL_DATA_DIR (default: /share/gnudatalanguage) to specify
  a custom installation location for GDL files 
  (a subdirectory of the main installation prefix)

The list of all GDL-related options accepted by CMake along
with their default values can be obtained by calling:
$ cmake .. -LAH | grep -A1 "// GDL: "

Results are cached. So if you need to specify a new 
libraries that have already been found you have to 
delete CMakeCache.txt and rerun cmake.

If you are on a Unix system, you can run `make' to begin
compilation (optionally with the number of concurrent
processes specified using the -j option):
$ make -j 3

For other systems, a native project file will be produced.

To launch the tests, run:
$ make check

To install, type:
$ make install

Other useful commands include `make help' to view a list
of targets and `make edit_cache' to edit cache results 
(variables defined in advanced mode shouldn't be edited).

examples:
mkdir gdl-clang
cd gdl-clang
cmake -DCMAKE_CXX_COMPILER=clang++  -DCMAKE_BUILD_TYPE=Release -DPYTHON=NO -DCMAKE_CXX_FLAGS_RELEASE=" -Wno-return-type -Wno-switch -Wno-format -O3 " -DPLPLOTDIR="somewhere"  -DLIBPROJ4=YES -DGSHHS=YES -DGSHHSDIR="/where/is/gshhs" -DHDF=YES -DUDUNITS=YES  -DUDUNITSDIR=/usr/include/udunits2 -DGLPK=OFF  ../gdl-lastversionfromcvs
make -j 4
sudo make install

or
mkdir gdl-gcc
cd gdl-gcc
cmake -DCMAKE_BUILD_TYPE=Release -DPYTHON=YES -DCMAKE_CXX_FLAGS_RELEASE=" -Wno-return-type -Wno-switch -Wno-format " -DPLPLOTDIR="somewhere"  -DLIBPROJ4=YES -DGSHHS=YES -DGSHHSDIR="/where/is/gshhs" -DHDF=YES -DUDUNITS=YES  -DUDUNITSDIR=/usr/include/udunits2 -DGLPK=OFF ../gdl-lastversionfromcvs
make -j 4
sudo make install

For a special compilation with all the options for fast code, including targeting the machine's processor type, use for example:
-DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="-O3 -fno-math-errno -fno-signaling-nans -msse2 -march=native -DNDEBUG"
more aggressive optimisations may invalidate tests about NaNs for example.

Note: CMakeLists.txt enforces use of CMAKE > 3.1 (3.2 for OsX) because some parts of our code needs a c++11 compliant compiler.
It is sometimes possible to remove this requirement from the CMakeLists.txt ("cmake_minimum_required(VERSION 3.1 FATAL_ERROR)")
provided one adds " -DCMAKE_CXX_FLAGS='-std=c++11' " to the cmake command.
