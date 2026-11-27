GDL requires CMake 3.13.3 or newer, a C/C++ compiler, and its development
library dependencies. Initialize submodules with git submodule update --init.
Separate build directories are recommended so one checkout can serve multiple
independent builds. In-source builds remain supported for existing workflows
and CI; CMake emits a warning when the source and build directories are the same.

Using the build helper
----------------------
Run the script from the directory that should hold the build:

  mkdir mydir
  cd mydir
  /path/to/gdl/scripts/build_gdl.sh prep
  /path/to/gdl/scripts/build_gdl.sh config
  /path/to/gdl/scripts/build_gdl.sh build
  ./run-gdl
  /path/to/gdl/scripts/build_gdl.sh test

prep installs system dependencies and may require administrator privileges.
It need only be run when dependencies are missing. prep_dryrun previews it.
Both can be run from any directory and do not create a GDL build directory.
config is an alias for configure; both accept additional CMake options.
No installation step is needed to run GDL or its tests. install is optional.

Select compilers and options independently in each build directory, e.g.:

  mkdir ../clang-debug
  cd ../clang-debug
  export Configuration=Debug
  export CC=clang CXX=clang++ NTHREADS=8
  /path/to/gdl/scripts/build_gdl.sh config -DOPENMP=OFF
  /path/to/gdl/scripts/build_gdl.sh build
  ./run-gdl -quiet -e 'print, findgen(4)'

Directory selection (highest priority first):
  BUILD_DIR: explicitly selects the build directory.
  ROOT_DIR: preserves the older ROOT_DIR/build convention.
  Otherwise: the current directory, or ./build when invoked in the source root.
Relative settings are resolved against the caller's directory.
Implicit selection refuses source subdirectories (except the conventional
source-root/build), the home directory, and the filesystem root. Other directories
may already contain files, including a redirected config.log. These checks apply
only to configuration; prep does not select or mark a build directory. Explicit
BUILD_DIR or ROOT_DIR settings permit existing CI layouts.
To explicitly request an in-source build through the helper, run it from the
source root with BUILD_DIR=. set for config, build and test (e.g. export BUILD_DIR=.).
INSTALL_PREFIX defaults to ROOT_DIR/install, with ROOT_DIR defaulting to the
caller's directory. Set it explicitly when a different install location is needed.
install, pack and prep_deploy read the configured install prefix from the cache;
an explicit INSTALL_PREFIX takes precedence for packaging. Packaging can also
use an existing installation without a build cache.

Configure preserves the cache and existing objects. CMake reads CC, CXX,
CFLAGS, CXXFLAGS and LDFLAGS when first creating a build. Use a new build
directory for a different compiler/toolchain; use -D options to update cached
settings. Helper defaults are applied only when creating a build. On later
config invocations, cached options, the build type and the install prefix are
preserved unless their corresponding environment variables are explicitly set
or CMake arguments change them. CMake arguments take precedence over environment
settings. DEPS chooses initial defaults; it does not reset an existing build.
GENERATOR or CMAKE_GENERATOR selects a generator, e.g. Ninja. build/install/test
use CMake/CTest and work with Makefiles, Ninja and multi-configuration generators.
CMAKE_COMMAND and CTEST_COMMAND can select specific executables; otherwise
the helper uses cmake and ctest from PATH. CTest passes its configured tools to
the build-helper regression tests.
Configuration defaults to Release for new builds. Existing single-configuration
builds use the cached build type. For multi-configuration generators, set
Configuration for build/test/install to select a configuration (default Release).
On macOS the helper defaults to Homebrew LLVM for new builds unless CC/CXX or
explicit CMake compiler options select another compiler.
The helper exports GDL_SRC_DIR for its source location; GDL_DIR is reserved for
runtime data and is set by run-gdl.

Platform behavior retained from the refactor: X11 is disabled by default, as in
CMake, since wxWidgets provides the graphics windows (use WITH_X11=ON to enable
it; on macOS X11 also needs XQuartz). DEPS=headless also disables wxWidgets.
On macOS Python is disabled by default (use
WITH_PYTHON=ON when creating a build to enable it): the .app does not bundle
Python, so it would require the same Homebrew Python, and NumPy loads its own
BLAS (OpenBLAS) next to the one GDL uses. The helper obtains the
SDK from xcrun and does not reset Homebrew or change xcode-select's global setting.

Running an uninstalled build
----------------------------
Every executable build generates <build_dir>/run-gdl. It works from any working
directory and forwards all arguments and the exit status to this build's GDL.
The source and build directories must remain in their configured locations.
GDL_DIR selects the build's share/gnudatalanguage tree, containing links to the
source procedures and resources (copies refreshed at build time if directory
symlinks are unavailable). GDL_DIR also controls mandatory startup procedures.
GDL_PATH prepends this tree's lib directory to an existing GDL_PATH, falling back
to IDL_PATH. GDL_DLM_PATH prepends the build's src/dlm and data/dlm directories.
GDL_MAPS_DIR and PLPLOT_LIB select this checkout's maps and PLplot fonts/palettes.
Startup files and unrelated environment settings are preserved. Shared library
search paths use CMake's normal build RPATH; do not disable it when linking
against dependencies in nonstandard locations.

Multi-configuration builds also generate <build_dir>/<config>/run-gdl. The root
launcher uses GDL_BUILD_CONFIG (default Release when available, otherwise the
first configured configuration):

  cmake --build . --config Debug
  GDL_BUILD_CONFIG=Debug ./run-gdl
  ctest -C Debug --output-on-failure

The launchers require a POSIX shell (including MSYS on Windows). Python-module
builds (PYTHON_MODULE=ON) do not generate an executable launcher.
run-gdl replaces quick_start_GDL.sh. Old generated quick_start_GDL.sh files in
existing build directories can be discarded.

Using CMake directly
--------------------
The same isolated builds and launchers are available without the helper:

  cmake -S /path/to/gdl -B gcc-release -DCMAKE_BUILD_TYPE=Release
  cmake --build gcc-release --parallel 8
  gcc-release/run-gdl
  (cd gcc-release && ctest --output-on-failure)

For another compiler, use another -B directory and set CMAKE_C_COMPILER and
CMAKE_CXX_COMPILER on the first configure. Most optional features have an ON/OFF
flag (EIGEN3, FFTW, HDF5, MAGICK, GRAPHICSMAGICK, GRIB, HDF, etc.) and a package
location flag (FFTWDIR, HDF5DIR, etc.). Examples:

  cmake -S /path/to/gdl -B gcc-release -DREADLINE=OFF
  cmake -S /path/to/gdl -B gcc-release -DFFTWDIR=/opt/local
  cmake -LAH -N gcc-release

PYTHONVERSION chooses a Python version. CMAKE_INSTALL_PREFIX and GDL_DATA_DIR
(default /share/gnudatalanguage relative to the prefix) control installation.
To install using any supported generator:

  cmake --build gcc-release --target install

Existing in-source CMake workflows (cmake ., followed by make or cmake --build .)
continue to work, including the generated ./run-gdl launcher. They can migrate
to separate build directories gradually.

PLplot's generated configuration headers live under <build_dir>/src/plplot.
For out-of-source builds, configuration and compilation do not modify the source
tree. When migrating from an in-source build, remove its CMakeCache.txt and
CMakeFiles directory before configuring elsewhere. Out-of-source builds ignore
old source-tree PLplot headers.

CBLAS selection
---------------
GSL requires a CBLAS provider. scripts/build_gdl.sh configure defaults to
GSL_CBLAS_PROVIDER=AUTO: it checks FlexiBLAS first, Accelerate on macOS,
OpenBLAS, then GSL's bundled CBLAS. It uses installed libraries and checks
the optimized providers' LP64 CBLAS headers and link interface. On macOS the
prep action installs FlexiBLAS for AUTO and FLEXIBLAS, so AUTO selects it;
it installs no other BLAS providers. To require one provider:
$ GSL_CBLAS_PROVIDER=FLEXIBLAS scripts/build_gdl.sh configure

Direct CMake builds retain the bundled GSL CBLAS default. To enable the
same automatic selection, use -DGSL_CBLAS_PROVIDER=AUTO. Valid choices
are AUTO, FLEXIBLAS, ACCELERATE (macOS), OPENBLAS, and GSL. Extra library
prefixes can be supplied with -DGSL_CBLAS_ROOTS='/prefix1;/prefix2'.
An explicit GSLCBLAS_LIBRARY path takes precedence, including old cache
entries; remove it with -UGSLCBLAS_LIBRARY to enable automatic selection.
This includes entries cached by older builds, which are reported as CUSTOM even
if the old build selected them automatically. Custom libraries are checked for
the required CBLAS symbols. Native OpenBLAS overrides are also checked against
the actual library's integer ABI and rejected if they report USE64BITINT.
Other custom libraries must be verified by the caller to use LP64. Cross builds
without an emulator cannot run the OpenBLAS ABI check and emit a warning.

On Windows and Cygwin, AUTO selects GSL's CBLAS only: the GSL DLL's existing
libgslcblas bindings cannot be redirected by linking another library into GDL.
Explicit alternative selections emit a warning for this reason. Windows tests
use MSYS sh; MSYS path conversion and packaging still need native CI validation.

Selecting FlexiBLAS does not change its runtime backend configuration or
retarget an installed NumPy. The Accelerate fallback uses the legacy LP64
CBLAS interface expected by prebuilt GSL, not the Fortran BLAS interface.
Defining ACCELERATE_NEW_LAPACK when compiling GDL cannot redirect calls
inside a prebuilt GSL library to Apple's new symbols.
On macOS AUTO can select Accelerate when FlexiBLAS is absent. Floating-point
results may differ in the last bits from those produced by GSL's CBLAS.

When GDL uses FlexiBLAS, the macOS pack action bundles all FlexiBLAS backends
in the .app with a configuration that names them, because FlexiBLAS otherwise
looks them up in Homebrew's configuration. bin/gdl is then a launcher that sets
FLEXIBLAS_CONFIG and runs bin/gdl-bin. FlexiBLAS's default backend is used
(APPLE, i.e. Accelerate, for Homebrew's FlexiBLAS); select another one with
FLEXIBLAS=NETLIB or FLEXIBLAS=OPENBLASOPENMP.

Build infrastructure regression tests (Python 3, CMake, Ninja and C/C++ compilers):
  python3 testsuite/cmake/test_build_helpers.py
They are also registered as test_build_helpers in CTest on POSIX hosts when
Python 3 and Ninja are available. Multi-configuration checks require CMake 3.17
or newer and are skipped with older CMake versions.
