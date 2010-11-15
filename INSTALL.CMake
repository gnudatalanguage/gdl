Be sure that you have at least cmake v. 2.6 (cmake --version)

Create a new directory where the objets and executables will be built.
e.g. `mkdir build; cd !$'

Then you can run cmake on the root directory of gdl.
e.g. `cmake ..'
You can also specify an install directory `cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/../install'

The default buid mode is "Release", to use the "Debug" mode try `cmake ... -DCMAKE_BUILD_TYPE=Debug'

You can choose to pass parameters to cmake in the manner of configure.
For each non-mandatory module, you can choose to enable/disable it
(default values are the same than in configure)
e.g. `cmake .. -DREADLINE=OFF' to disable readline.
The following lists the parameters that can be used as boolean values
(ON,YES,Y,OFF,NO,N, case insensitive)
FFTW
GRIB
GSHHS
HDF5
HDF
LIBPROJ4
MAGICK
MPICH
NETCDF
OPENMP
PYTHON
PYTHON_MODULE
READLINE
UDUNITS
WXWIDGETS

You can also specify a path to different libraries :
FFTWDIR
GRIBDIR
GSHHSDIR
HDF5DIR
HDFDIR
LIBPROJ4DIR
MAGICKDIR
MPICHDIR
NETCDFDIR
PYTHONDIR
READLINEDIR
UDUNITSDIR
WXWIDGETSDIR

Few other options are available in order to help cmake to find
(extra) libraries (if you don't know what these libraries are,
then you probably don't have to care about it)
JASPERDIR
JPEGDIR
SZIPDIR
PYTHONVERSION
OLDPLPLOT

Results are cached. So if you need to specify a new libraries that have already been
found you have to delete CMakeCache.txt and rerun cmake.

If you are on a Unix system, you can run `make'
For other systems, a native project file should be produced.

To launch the tests, run `make test'.
To install, type `make install'.

Usefull commands :
`make help' to view different targets.
`make edit_cache' to edit cache results (variables defined in advanced mode shouldn't be edited)
