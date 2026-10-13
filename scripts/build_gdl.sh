#!/usr/bin/env bash
#
# script to download some files needed for mingw32, and build GDL.
#
# JP:
#  Since old packages are not available from msys2 repo anymore, let's try the latest versions here.
#  If they don't work (especially readline), we need to store the old ones in another location.
#  Below is the list of libraries had to be 'reversioned' in the previous version of this script:
#   - readline, zlib, libpng, libpcre
#
# 2021-03-20: Script has been updated to support Linux and macOS

LANG=C #script works only in english
ME="build_gdl.sh"
# Record caller overrides before assigning defaults. On reconfigure, only these
# values (and explicit CMake arguments) should replace the existing cache.
gdl_caller_settings=()
for gdl_setting in Configuration INSTALL_PREFIX PYTHONVERSION GSL_CBLAS_PROVIDER \
    WITH_WXWIDGETS WITH_X11 WITH_OPENMP WITH_PYTHON WITH_GRAPHICSMAGICK \
    WITH_NETCDF WITH_HDF5 WITH_MPI WITH_TIFF WITH_GEOTIFF WITH_LIBPROJ \
    WITH_FFTW WITH_UDUNITS2 WITH_GLPK WITH_HDF4 WITH_GRIB WITH_LLD; do
    if [[ -n ${!gdl_setting} ]]; then
        gdl_caller_settings+=("${gdl_setting}")
    fi
done
gdl_explicit_build_dir=false
if [[ -n ${BUILD_DIR:-} || -n ${ROOT_DIR:-} ]]; then
    gdl_explicit_build_dir=true
fi
Configuration=${Configuration:-"Release"}
DEPS=${DEPS:-"standard"}
real_path() {
    case $1 in
        /* | [A-Za-z]:/*) printf '%s\n' "$1" ;;
        *) printf '%s/%s\n' "$PWD" "${1#./}" ;;
    esac
}
# Resolve relative settings against the caller's directory before any cd.
export GDL_SRC_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)
if [[ -z ${BUILD_DIR:-} ]]; then
    if [[ -n ${ROOT_DIR:-} ]]; then
        BUILD_DIR="${ROOT_DIR}/build" # compatibility with existing CI/users
    elif [[ $(pwd -P) == "${GDL_SRC_DIR}" ]]; then
        BUILD_DIR="${GDL_SRC_DIR}/build"
    else
        BUILD_DIR=$PWD
    fi
fi
export ROOT_DIR=$(real_path "${ROOT_DIR:-$PWD}")
BUILD_DIR=$(real_path "${BUILD_DIR}")
INSTALL_PREFIX=$(real_path "${INSTALL_PREFIX:-${ROOT_DIR}/install}")
PYTHONVERSION=${PYTHONVERSION:-"3.0.0"}
GDLDE_VERSION=${GDLDE_VERSION:-"v1.0.0"} # needed by 'pack' (at the moment Windows only)
NTHREADS=${NTHREADS:-$(getconf _NPROCESSORS_ONLN)} # find nthreads for make -j
BUILD_OS=$(uname)
DRY_RUN=false
if [[ ${BUILD_OS} == *"MSYS"* ]] || [[ ${BUILD_OS} == *"MINGW"* ]]; then
    BUILD_OS="Windows"
	declare -x MSYSTEM="UCRT64"
	declare -x MSYSTEM_CARCH="x86_64"
	declare -x MSYSTEM_CHOST="x86_64-w64-mingw32"
	declare -x MSYSTEM_PREFIX="/ucrt64"
elif [[ ${BUILD_OS} == "Darwin" ]]; then
    BUILD_OS="macOS"
    Platform=${Platform:-$(arch)}
fi

# Build flags. Use the same WITH_* settings for prep and configure on macOS.
# WITH_LLD=ON installs and selects LLD (requires CMake >= 3.29).
# wxWidgets provides the graphics windows; X11 is OFF unless WITH_X11=ON, as in
# CMake. MPI and Python default to OFF on macOS, but can be enabled explicitly:
# the .app does not bundle Python, and NumPy brings its own BLAS into the process.
if [[ ${DEPS} == "headless" ]]; then
    WITH_WXWIDGETS=${WITH_WXWIDGETS:-OFF}
else
    WITH_WXWIDGETS=${WITH_WXWIDGETS:-ON}
fi
WITH_X11=${WITH_X11:-OFF}
WITH_OPENMP=${WITH_OPENMP:-ON}
if [[ ${BUILD_OS} == "macOS" ]]; then
    WITH_PYTHON=${WITH_PYTHON:-OFF}
else
    WITH_PYTHON=${WITH_PYTHON:-ON}
fi
WITH_GRAPHICSMAGICK=${WITH_GRAPHICSMAGICK:-ON}
WITH_NETCDF=${WITH_NETCDF:-ON}
WITH_HDF5=${WITH_HDF5:-ON}
if [[ ${DEPS} != "headless" || ${BUILD_OS} == "macOS" ]]; then
    WITH_MPI=${WITH_MPI:-OFF}
else
    WITH_MPI=${WITH_MPI:-ON}
fi
WITH_TIFF=${WITH_TIFF:-ON}
WITH_GEOTIFF=${WITH_GEOTIFF:-ON}
WITH_LIBPROJ=${WITH_LIBPROJ:-ON}
WITH_FFTW=${WITH_FFTW:-ON}
WITH_UDUNITS2=${WITH_UDUNITS2:-ON}
WITH_GLPK=${WITH_GLPK:-ON}
# Prefer an installed optimized CBLAS for GSL. CMake validates candidates.
# AUTO: FlexiBLAS, Accelerate (macOS), OpenBLAS, then bundled GSL CBLAS.
# Override with GSL_CBLAS_PROVIDER=FLEXIBLAS|ACCELERATE|OPENBLAS|GSL, or pass an explicit
# -DGSLCBLAS_LIBRARY=/path/to/library to configure. On macOS, prep installs
# FlexiBLAS for AUTO and FLEXIBLAS; no other BLAS packages are installed, and
# FlexiBLAS's runtime backend configuration is not changed.
GSL_CBLAS_PROVIDER=${GSL_CBLAS_PROVIDER:-AUTO}
if [[ ${BUILD_OS} == "macOS" ]]; then
    WITH_HDF4=${WITH_HDF4:-OFF}
    WITH_GRIB=${WITH_GRIB:-ON}
    WITH_LLD=${WITH_LLD:-OFF}
else
    zz=`grep -i opensuse /etc/*-release 2> /dev/null`
    if [[ -n $zz ]]; then
        # in case of openSUSE
        WITH_HDF4=${WITH_HDF4:-OFF}
        WITH_GRIB=${WITH_GRIB:-OFF}
    else
        # other distros
        WITH_HDF4=${WITH_HDF4:-ON}
        WITH_GRIB=${WITH_GRIB:-ON}
    fi
fi
if [[ ${BUILD_OS} == "Windows" ]]; then
    WITH_HDF4="OFF"
    WITH_PYTHON="OFF"
fi
function log {  # log is needded just below!
    echo "[${ME}] $@"
}

# Match CMake's false values when selecting optional Homebrew dependencies.
function feature_enabled {
    case "$1" in
        ""|0|[Oo][Ff][Ff]|[Nn][Oo]|[Ff][Aa][Ll][Ss][Ee]|[Nn]|[Ii][Gg][Nn][Oo][Rr][Ee]|[Nn][Oo][Tt][Ff][Oo][Uu][Nn][Dd]|*-[Nn][Oo][Tt][Ff][Oo][Uu][Nn][Dd]) return 1 ;;
        *) return 0 ;;
    esac
}

if [ ${BUILD_OS} == "Windows" ]; then
    BSDXDR_URL="https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/bsd-xdr/bsd-xdr-1.0.0.tar.gz"
    MSYS2_PACKAGES=(
        readline zlib libpng gsl wxwidgets3.2 libgd libtiff libgeotiff netcdf hdf4 hdf5 fftw proj msmpi udunits
        eigen3 eccodes glpk shapelib expat openssl qhull graphicsmagick
    )
    #if you add something in MSYS2_PACKAGES_REBUILD you may have to add special lines in main.yml to push the product in /var/cache/pacman/pkg
    MSYS2_PACKAGES_REBUILD=(
       
    )
elif [ ${BUILD_OS} == "Linux" ]; then
    # JP: Note the seperator ',' between the package name candidates below. The leftmost one has the highest priority.
    # Debian, Ubuntu, Linux Mint, Elementary OS, etc.
    APT_PACKAGES=(
        libtirpc-dev libncurses-dev libreadline-dev,libreadline-gplv2-dev zlib1g-dev libpng-dev libgsl-dev,libgsl0-dev
        libwxgtk3.2-dev,libwxgtk3.0-gtk3-dev,libwxgtk3.0-dev,libwxgtk2.8-dev  libgraphicsmagick++1-dev,libgraphicsmagick++-dev libtiff-dev
        libgeotiff-dev libnetcdf-dev libhdf4-alt-dev libhdf5-dev libfftw3-dev libproj-dev libopenmpi-dev libpython3-dev,libpython-dev python3-dev,python-dev
        python3-numpy,python-numpy libudunits2-dev libeigen3-dev libeccodes-dev libglpk-dev libshp-dev,shapelib libexpat1-dev libqhull-dev
    )
    # Redhat, Fedora, Scientific Linux, CentOS, openSuSE, SLES, etc.
    RPM_PACKAGES=(
        libtirpc-devel ncurses-devel readline-devel zlib-devel libpng-devel,libpng16-devel gsl-devel
	wxGTK3-devel,wxGTK-devel,wxWidgets-3_2-devel,wxWidgets-3_0-devel
        GraphicsMagick-c++-devel,libGraphicsMagick++-devel libtiff-devel libgeotiff-devel,libgeotiff5-devel,geotiff-devel 
        netcdf-devel hdf-devel hdf5-devel fftw-devel,fftw3-devel proj-devel openmpi-devel,openmpi4-devel,openmpi3-devel
	python39-devel,python38-devel,python3-devel,python-devel
        python39-numpy-devel,python38-numpy-devel,python3-numpy-devel,python-numpy-devel,python39-numpy,python38-numpy,python3-numpy,python-numpy
	udunits2-devel eigen3-devel eccodes-devel glpk-devel libshp-devel,shapelib-devel qhull
        expat-devel,libexpat-devel
    ) # JP 2021 Mar 21: SuSE lacks eccodes
elif [ ${BUILD_OS} == "macOS" ]; then
    # llvm supplies clang/clang++ and the archive tools. Homebrew manages
    # transitive dependencies, including any Fortran runtimes provided by gcc.
    # dylibbundler is used by the macOS pack action.
    BREW_PACKAGES=(
        llvm cmake pkgconf ncurses readline zlib libpng gsl eigen qhull shapelib expat dylibbundler
    )
    if feature_enabled "${WITH_LLD}"; then BREW_PACKAGES+=(lld); fi
    if feature_enabled "${WITH_X11}"; then BREW_PACKAGES+=(libx11); fi
    if feature_enabled "${WITH_OPENMP}"; then BREW_PACKAGES+=(libomp); fi
    if feature_enabled "${WITH_WXWIDGETS}"; then BREW_PACKAGES+=(wxwidgets); fi
    if feature_enabled "${WITH_GRAPHICSMAGICK}"; then BREW_PACKAGES+=(graphicsmagick); fi
    if feature_enabled "${WITH_TIFF}"; then BREW_PACKAGES+=(libtiff); fi
    if feature_enabled "${WITH_GEOTIFF}"; then BREW_PACKAGES+=(libgeotiff); fi
    if feature_enabled "${WITH_NETCDF}"; then BREW_PACKAGES+=(netcdf); fi
    if feature_enabled "${WITH_HDF5}"; then BREW_PACKAGES+=(hdf5); fi
    if feature_enabled "${WITH_FFTW}"; then BREW_PACKAGES+=(fftw); fi
    if feature_enabled "${WITH_LIBPROJ}"; then BREW_PACKAGES+=(proj); fi
    if feature_enabled "${WITH_MPI}"; then BREW_PACKAGES+=(open-mpi); fi
    if feature_enabled "${WITH_PYTHON}"; then BREW_PACKAGES+=(python numpy); fi
    if feature_enabled "${WITH_UDUNITS2}"; then BREW_PACKAGES+=(udunits); fi
    if feature_enabled "${WITH_GRIB}"; then BREW_PACKAGES+=(eccodes); fi
    if feature_enabled "${WITH_GLPK}"; then BREW_PACKAGES+=(glpk); fi
    # FlexiBLAS is the macOS CBLAS for GSL; Accelerate is one of its backends.
    case $(printf '%s' "${GSL_CBLAS_PROVIDER}" | tr '[:lower:]' '[:upper:]') in
        AUTO|FLEXIBLAS) BREW_PACKAGES+=(flexiblas) ;;
    esac
else
    log "Fatal error! Unknown OS: ${BUILD_OS}. This script only supports one of: Windows, Linux, macOS."
    exit 1
fi

function download_file {
    package_url=$1
    package_file=$(basename $package_url)
    log "Downloading ${package_file}..."
    if [ -f $package_file ]; then
       log "File exists! Skipping..."
    else
        curl -LOs --retry 5 $package_url
        if [ ! $? -eq 0 ]; then
            log "Failed to download ${package_file}!"
            exit 1
        fi
    fi
}

function decompress_file {
    log "Decompressing ${package_file}..."
    if [ ${package_file: -4} == ".zip" ]; then
        unzip -q $@ $package_file
    else
        tar xf $@ $package_file
    fi
    if [ ! $? -eq 0 ]; then
        log "Failed to decompress ${package_file}!"
        exit 1
    fi
}

function build_msys2_package {
    log "Building package $1..."
    pushd ${ROOT_DIR}
    if [[ ! -d "MINGW-packages" ]]; then
        git clone \
            --depth 1  \
            --filter=blob:none  \
            --sparse \
            https://github.com/msys2/MINGW-packages \
            ;
    fi
    cd MINGW-packages
    git sparse-checkout init --cone
    rm -fr mingw-w64-$1
    git sparse-checkout set mingw-w64-$1
    git checkout mingw-w64-$1
    cd mingw-w64-$1
    patch="${GDL_SRC_DIR}/scripts/deps/windows/mingw-patches/$1.patch"
    if [[ -f $patch ]]; then
        patch -p0 -i $patch
    fi
    if [[ ${arch} == "x86_64" ]]; then
        export MINGW_ARCH=mingw64
    else
        export MINGW_ARCH=mingw32
    fi
    makepkg-mingw --noconfirm --syncdeps --install
    popd
}

function find_architecture {
    if [ ${Platform:-`uname -m`} == "x86_64" ]; then
        export mname="mingw64"
        export MSYSTEM="MINGW64"
        export arch="x86_64"
    else
        export mname="mingw32"
        export MSYSTEM="MINGW32"
        export arch="i686"
    fi
    export PATH=/$mname/bin:$PATH
    log "Architecture: ${arch}"
}

function install_zstd {
    mkdir -p /c/projects/zstd
    cd /c/projects/zstd
    zstd_version="v1.4.8"
    if [ $arch == "i686" ]; then
        zstd_zip=zstd-${zstd_version}-win32.zip
    else
        zstd_zip=zstd-${zstd_version}-win64.zip
    fi
    curl -LOs https://github.com/facebook/zstd/releases/download/${zstd_version}/${zstd_zip}
    7z x ${zstd_zip}
    cp zstd.exe /usr/bin/
}

function query_package {
    if [ ${PKGMGR} == "apt" ]; then
        test_package=( `apt-cache policy $1 | grep Installed:` )
        if [ ! -n "$test_package" ]; then
            query_result=0 # invalid package name
        elif [[ ${test_package[1]} == "(none)" ]]; then
            query_result=1 # the package exists, but not installed
        else
            query_result=2 # the package is already installed
        fi
    elif [ ${PKGMGR} == "yum" ]; then
        test_package=`yum info -C --available $1 2>&1 | grep Error`
        if [ ! -n "$test_package" ]; then
            # the package exists
            test_package2=`yum info -C --installed $1 2>&1 | grep Error`
            if [ ! -n "$test_package2" ]; then
                query_result=2 # the package is already installed
            else
                query_result=1 # the package exists, but not installed
            fi
        else
            query_result=0 # invalid package name
        fi
    elif [ ${PKGMGR} == "zypper" ]; then
        test_package=`zypper info $1`
        if [[ ${test_package} == *"not found"* ]]; then
            query_result=0 # invalid package name
        else
            if [[ ${test_package} == *"not installed"* ]]; then
                query_result=1 # the package exists, but not installed
            else
                query_result=2 # the package is already installed
            fi
        fi
    fi
}

function find_pkgmgr {
    PKGINSTALLARG="install"
    if command -v apt >/dev/null 2>&1; then
        PACKAGES=( ${APT_PACKAGES[@]} )
        PKGMGR="apt"
    else
        PACKAGES=( ${RPM_PACKAGES[@]} )
        if command -v yum >/dev/null 2>&1; then
            PKGMGR="yum"
        elif command -v zypper >/dev/null 2>&1; then
            PKGMGR="zypper"
        else
            log "Fatal error! Cannot determine package manager on your system."
            exit 1
        fi
    fi
    log "Found package manager: $PKGMGR"
    if [ ${PKGMGR} == "apt" ]; then
        log "Updating apt package cache..."
        sudo apt update
    fi
}

function install_toolchain {
    log "Installing toolchain..."
    
    find_pkgmgr

    if [ ${BUILD_OS} == "Windows" ]; then
        pacman -S --noconfirm --needed mingw-w64-${arch}-toolchain mingw-w64-${arch}-cmake mingw-w64-${arch}-nsis unzip tar zstd make
    elif [ ${BUILD_OS} == "Linux" ]; then
        if [ ${PKGMGR} == "apt" ]; then
            INSTALL_PACKAGES="g++ cmake make"
        elif [ ${PKGMGR} == "yum" ]; then
            INSTALL_PACKAGES="gcc-c++ cmake make"
        elif [ ${PKGMGR} == "zypper" ]; then
            INSTALL_PACKAGES="gcc-c++ cmake make"
        else
            log "Fatal error! Cannot determine package manager on your system."
            exit 1
        fi
        eval "sudo ${PKGMGR} ${PKGINSTALLARG} -y ${INSTALL_PACKAGES}"
    fi
}

function prep_packages {
    if [ ${BUILD_OS} == "Windows" ]; then
        for pkgname in ${MSYS2_PACKAGES[@]}; do
            msys2_packages="${msys2_packages} mingw-w64-${arch}-${pkgname}"
        done
        
        log "Installing dependencies: ${msys2_packages}"
        if [ ${DRY_RUN} == "true" ]; then
            log "Please run below command to install required packages to build GDL."
            echo "pacman --noconfirm --needed -S ${msys2_packages}"
        else
            # Mirrors occasionally stall; --needed and pacman's package cache make a
            # retry cheap. Never continue with missing dependencies.
            local gdl_attempt
            for gdl_attempt in 1 2 3; do
                eval "pacman --noconfirm --needed -S ${msys2_packages}" && break
                if [[ ${gdl_attempt} == 3 ]]; then
                    log "Fatal error! pacman could not install the dependencies."
                    return 1
                fi
                log "pacman failed (attempt ${gdl_attempt} of 3); retrying in 30 seconds..."
                sleep 30
            done
        fi

#        for package_name in ${MSYS2_PACKAGES_REBUILD[@]}; do
#            build_msys2_package $package_name
#        done
	
        download_file ${BSDXDR_URL}
        decompress_file

        log "Building bsd-xdr..."
        pushd bsd-xdr-1.0.0
#		need a patch now with a new version of the mingw compiler... (GD)
        patch lib/xdr_rec.c ${GDL_SRC_DIR}/scripts/deps/windows/mingw-patches/bsdxdr_rec.patch
        sed -e 's/-Wall/-Wall -Wno-pointer-to-int-cast #/' -e 's/$(XDR_LIBRARIES) $(TEST_PROGS)/$(XDR_LIBRARIES)/' -e 's/libxdr/libbsdxdr/' -i Makefile
        mv lib/libxdr.def.in lib/libbsdxdr.def.in
        make || exit 1
        if [ ${DRY_RUN} == "true" ]; then
            log "Please run below command to install bsd-xdr prior to build GDL."
            echo cp -f mingw/*.dll /${mname}/bin/
            echo cp -f mingw/*.a /${mname}/lib/
            echo cp -rf rpc /${mname}/include/
        else
            cp -f mingw/*.dll /${mname}/bin/
            cp -f mingw/*.a /${mname}/lib/
            cp -rf rpc /${mname}/include/
        fi
        popd
    elif [ ${BUILD_OS} == "Linux" ]; then
        # JP: This part is based on `aptget4gdl.sh` and `rpm4gdl.sh` by Alain C. and Ilia N.
        find_pkgmgr
        log "Checking package availabilities..."
        for pkgnamecandidates in ${PACKAGES[@]}; do
            for pkgname in $(echo $pkgnamecandidates | tr ',' ' '); do
                query_package $pkgname
                if [[ ${query_result} == "1" ]] ; then
                    INSTALL_PACKAGES="${INSTALL_PACKAGES} $pkgname"
                    break # install only one package
                elif [[ ${query_result} == "2" ]] ; then
                    break # the package is already installed
                fi
            done
        done

        if [[ -z ${INSTALL_PACKAGES} ]]; then
            log "All required packages are already installed on your system."
	    else
            log "Installing packages:"
            log "${INSTALL_PACKAGES}"
            if [ ${DRY_RUN} == "true" ]; then
                log "Please run below command to install required packages to build GDL."
                echo "sudo ${PKGMGR} ${PKGINSTALLARG} -y ${INSTALL_PACKAGES}"
            else
                if ! eval "sudo ${PKGMGR} ${PKGINSTALLARG} -y ${INSTALL_PACKAGES}"; then
                    log "Fatal error! ${PKGMGR} could not install the dependencies."
                    return 1
                fi
            fi
        fi
    elif [ ${BUILD_OS} == "macOS" ]; then
        if ! command -v brew >/dev/null 2>&1; then
            log "Fatal error! Homebrew not found."
            exit 1
        fi
        log "Required Homebrew packages: ${BREW_PACKAGES[@]}"
        if [ ${DRY_RUN} == "true" ]; then
            log "Please run below command to install required packages to build GDL."
            echo "brew install ${BREW_PACKAGES[@]}"
        else
            # brew can fail after installing a formula (e.g. link conflicts with
            # files preinstalled on CI runners), so only missing formulae are fatal.
            if ! brew install "${BREW_PACKAGES[@]}"; then
                log "brew install reported errors; checking the required formulae..."
            fi
            local gdl_formula gdl_missing=()
            for gdl_formula in "${BREW_PACKAGES[@]}"; do
                brew list --formula "${gdl_formula}" >/dev/null 2>&1 || gdl_missing+=("${gdl_formula}")
            done
            if [[ ${#gdl_missing[@]} -gt 0 ]]; then
                log "Fatal error! Homebrew could not install: ${gdl_missing[*]}"
                return 1
            fi
        fi
    fi
}

function prep_packages_dryrun {
    DRY_RUN=true
    prep_packages
}

function find_dlls {
    for dll in $(strings "$1" | grep -o '\S*\.dll' | grep -v "$(basename "$1")"); do
        dll="/mingw64/bin/$dll"
        if [ -f "$dll" ] && [[ ! ${found_dlls[@]} =~ (^|[[:space:]])"$dll"($|[[:space:]]) ]]; then
            log "Found DLL dependency: $dll"
            found_dlls+=("$dll")
            find_dlls "$dll"
        fi
  done
}

# An explicitly selected BUILD_DIR/ROOT_DIR permits established CI layouts.
# Implicit configuration must not write into source subdirectories, $HOME or /.
function check_build_directory {
    if ${gdl_explicit_build_dir}; then return 0; fi
    local gdl_destination="${BUILD_DIR}" gdl_home
    if [[ -d "${BUILD_DIR}" ]]; then
        gdl_destination=$(cd "${BUILD_DIR}" && pwd -P) || return 1
    fi
    if [[ "${gdl_destination}" == "${GDL_SRC_DIR}/build" ]]; then return 0; fi
    gdl_home=""
    if [[ -n ${HOME:-} && -d "${HOME}" ]]; then
        gdl_home=$(cd "$HOME" && pwd -P) || return 1
    fi
    case "${gdl_destination}" in
        "${GDL_SRC_DIR}" | "${GDL_SRC_DIR}/"* | "${gdl_home}" | /)
            log "Refusing an implicit build in ${gdl_destination}. Select a build directory or set BUILD_DIR explicitly."
            return 1 ;;
    esac
}

function caller_set {
    local gdl_setting
    for gdl_setting in "${gdl_caller_settings[@]}"; do
        if [[ "${gdl_setting}" == "$1" ]]; then return 0; fi
    done
    return 1
}

function configure_gdl {
    check_build_directory || return 1
    local gdl_new_build=true
    if [[ -f "${BUILD_DIR}/CMakeCache.txt" ]]; then gdl_new_build=false; fi
    log "Configuring GDL in ${BUILD_DIR}..."

    if [[ ${DEPS} != "standard" && ${DEPS} != "headless" && ${DEPS} != "debug" ]]; then
        log "Fatal error! Unknown DEPS: ${DEPS}"
        return 1
    fi

    local gdl_generator_args=() gdl_config_args=() gdl_pair gdl_variable gdl_option
    if [[ -n ${GENERATOR:-${CMAKE_GENERATOR:-}} ]]; then
        gdl_generator_args=( -G "${GENERATOR:-${CMAKE_GENERATOR}}" )
    elif [[ ${BUILD_OS} == "Windows" ]] && ${gdl_new_build}; then
        gdl_generator_args=( -G "MSYS Makefiles" )
    fi
    for gdl_pair in Configuration:CMAKE_BUILD_TYPE INSTALL_PREFIX:CMAKE_INSTALL_PREFIX \
        PYTHONVERSION:PYTHONVERSION GSL_CBLAS_PROVIDER:GSL_CBLAS_PROVIDER \
        WITH_WXWIDGETS:WXWIDGETS WITH_X11:X11 WITH_GRAPHICSMAGICK:GRAPHICSMAGICK \
        WITH_NETCDF:NETCDF WITH_HDF4:HDF WITH_HDF5:HDF5 WITH_MPI:MPI \
        WITH_TIFF:TIFF WITH_GEOTIFF:GEOTIFF WITH_LIBPROJ:LIBPROJ WITH_PYTHON:PYTHON \
        WITH_FFTW:FFTW WITH_UDUNITS2:UDUNITS2 WITH_GLPK:GLPK WITH_GRIB:GRIB WITH_OPENMP:OPENMP; do
        gdl_variable=${gdl_pair%%:*}
        gdl_option=${gdl_pair#*:}
        if ${gdl_new_build} || caller_set "${gdl_variable}"; then
            gdl_config_args+=( "-D${gdl_option}=${!gdl_variable}" )
        fi
    done

    if [[ ${BUILD_OS} == "macOS" ]]; then
        # Apply automatic SDK, dependency and compiler defaults only to new
        # builds. Cached choices remain authoritative on reconfiguration.
        if ${gdl_new_build}; then
            local gdl_readline_prefix gdl_sdk gdl_arg gdl_custom_toolchain=false
            if [[ -n ${CC:-} || -n ${CXX:-} || -n ${CMAKE_TOOLCHAIN_FILE:-} ]]; then
                gdl_custom_toolchain=true
            fi
            for gdl_arg in "$@"; do
                case "${gdl_arg}" in
                    *CMAKE_C_COMPILER=* | *CMAKE_C_COMPILER:*=* | \
                    *CMAKE_CXX_COMPILER=* | *CMAKE_CXX_COMPILER:*=* | \
                    *CMAKE_TOOLCHAIN_FILE=* | *CMAKE_TOOLCHAIN_FILE:*=* | \
                    --toolchain | --toolchain=* | -C | -C?*) gdl_custom_toolchain=true ;;
                esac
            done
            gdl_readline_prefix=$(brew --prefix readline) || return 1
            gdl_sdk=$(xcrun --sdk macosx --show-sdk-path) || return 1
            gdl_config_args+=( "-DREADLINEDIR=${gdl_readline_prefix}" "-DCMAKE_OSX_SYSROOT=${gdl_sdk}" )
            if ! ${gdl_custom_toolchain}; then
                local gdl_llvm_prefix
                gdl_llvm_prefix=$(brew --prefix llvm) || return 1
                gdl_config_args+=( "-DCMAKE_CXX_COMPILER=${gdl_llvm_prefix}/bin/clang++"
                                   "-DCMAKE_C_COMPILER=${gdl_llvm_prefix}/bin/clang"
                                   "-DCMAKE_AR=${gdl_llvm_prefix}/bin/llvm-ar"
                                   "-DCMAKE_RANLIB=${gdl_llvm_prefix}/bin/llvm-ranlib" )
            fi
            local gdl_blas_package gdl_blas_prefix gdl_blas_roots=""
            for gdl_blas_package in flexiblas openblas; do
                gdl_blas_prefix=$(brew --prefix "${gdl_blas_package}" 2>/dev/null) || continue
                if [[ -d "${gdl_blas_prefix}" ]]; then
                    gdl_blas_roots+="${gdl_blas_roots:+;}${gdl_blas_prefix}"
                fi
            done
            if [[ -n "${gdl_blas_roots}" ]]; then
                gdl_config_args+=( "-DGSL_CBLAS_ROOTS=${gdl_blas_roots}" )
            fi
        fi
        if { ${gdl_new_build} || caller_set WITH_OPENMP; } && feature_enabled "${WITH_OPENMP}"; then
            local gdl_openmp_prefix
            gdl_openmp_prefix=$(brew --prefix libomp) || return 1
            gdl_config_args+=( "-DOpenMP_omp_LIBRARY=${gdl_openmp_prefix}/lib/libomp.dylib"
                               "-DOpenMP_C_INCLUDE_DIR=${gdl_openmp_prefix}/include"
                               "-DOpenMP_CXX_INCLUDE_DIR=${gdl_openmp_prefix}/include" )
        fi
        if ${gdl_new_build} || caller_set WITH_LLD; then
            if feature_enabled "${WITH_LLD}"; then
                gdl_config_args+=( "-DCMAKE_LINKER_TYPE=LLD" )
            elif caller_set WITH_LLD; then
                gdl_config_args+=( "-DCMAKE_LINKER_TYPE=DEFAULT" )
            fi
        fi
    fi

    if [[ ${BUILD_OS} == "Windows" ]]; then
        export WX_CONFIG="${GDL_SRC_DIR}/scripts/deps/windows/wx-config-wrapper"
        if ${gdl_new_build}; then gdl_config_args+=( "-DMACHINE_ARCH=${Platform}" ); fi
    fi
    # Explicit CMake arguments take precedence over helper settings.
    "${CMAKE_COMMAND:-cmake}" -S "${GDL_SRC_DIR}" -B "${BUILD_DIR}" "${gdl_generator_args[@]}" "${gdl_config_args[@]}" "$@"
}

function require_build_dir {
    if [[ ! -f "${BUILD_DIR}/CMakeCache.txt" ]]; then
        log "Fatal error! No configured build in ${BUILD_DIR}. Run configure first."
        return 1
    fi
    read_build_settings
}

# Packaging can also operate on an existing installation without a build cache.
# When a cache is available, use its settings unless the caller overrides them.
function read_build_settings {
    if [[ ! -f "${BUILD_DIR}/CMakeCache.txt" ]]; then return 0; fi
    # Single-configuration builds remember their build type. Multi-config builds
    # use Configuration (Release by default) to select a configuration at build time.
    local gdl_cache_line gdl_cache_name gdl_cache_value gdl_cached_type="" gdl_multi_config=false
    while IFS= read -r gdl_cache_line; do
        gdl_cache_line=${gdl_cache_line%$'\r'} # CMake writes CRLF caches on Windows
        gdl_cache_name=${gdl_cache_line%%:*}
        gdl_cache_value=${gdl_cache_line#*=}
        if [[ "${gdl_cache_name}" == CMAKE_BUILD_TYPE ]]; then
            gdl_cached_type=${gdl_cache_value}
        elif [[ "${gdl_cache_name}" == CMAKE_CONFIGURATION_TYPES && -n "${gdl_cache_value}" ]]; then
            gdl_multi_config=true
        elif [[ "${gdl_cache_name}" == CMAKE_INSTALL_PREFIX ]] && ! caller_set INSTALL_PREFIX; then
            INSTALL_PREFIX=${gdl_cache_value}
        fi
    done < "${BUILD_DIR}/CMakeCache.txt"
    if ! ${gdl_multi_config} && ! caller_set Configuration && [[ -n "${gdl_cached_type}" ]]; then
        Configuration=${gdl_cached_type}
    fi
}

function build_gdl {
    require_build_dir || return 1
    log "Building GDL in ${BUILD_DIR}..."
    "${CMAKE_COMMAND:-cmake}" --build "${BUILD_DIR}" --config "${Configuration}" --parallel "${NTHREADS}"
}

function install_gdl {
    require_build_dir || return 1
    log "Installing GDL..."
    "${CMAKE_COMMAND:-cmake}" --build "${BUILD_DIR}" --config "${Configuration}" --target install || return 1

    if [ ${BUILD_OS} == "Windows" ]; then
        cd "${INSTALL_PREFIX}" || return 1
        log "Copying DLLs to install directory..."
        found_dlls=()
        find_dlls "${INSTALL_PREFIX}/bin/gdl.exe"
        PYTHON_DLL=`ldd "${INSTALL_PREFIX}/bin/gdl.exe" | tr '[:upper:]' '[:lower:]' | grep python | xargs | cut -d' ' -f3`
        if [[ -n "${PYTHON_DLL}" ]]; then
            cp -f "${PYTHON_DLL}" bin/
        fi

        if [[ $WITH_UDUNITS2 == "ON" ]]; then
            log "Copying udunits xml files to install directory..."
            cp -rf /${mname}/share/udunits share/
        fi

        for f in ${found_dlls[@]}; do
            cp -f "$f" bin/
        done
        
        log "Copying plplot stuff (not drivers) to install directory..."
        mkdir -p share
#        cp -rf /${mname}/share/plplot* share/
#        rm -rf share/plplot*/examples
#        rm -rf share/plplot*/ss
#        rm -rf share/plplot*/tcl
#        rm -rf share/plplot*/*.shx
#        rm -rf share/plplot*/*.shp
#        rm -rf share/plplot*/*.tcl
#
        #with PROJ7, needs proj.db, and serachs for it at '"where libproj.dll is"/../share/proj so we do the same
        log "Copying PROJ database at correct location"
        cp -rf /${mname}/share/proj share/

        log "Copying GraphicsMagick drivers to install directory..."
        mkdir -p lib
        cp -rf /${mname}/lib/GraphicsMagick* lib/ # copy GraphicsMagick dlls
        cp -f ${GDL_SRC_DIR}/COPYING .
    fi
}

function test_gdl {
    log "Testing GDL..."
    require_build_dir || return 1
    if [[ -f "${GDL_SRC_DIR}/CMakeModules/CodeCoverage.cmake" ]]; then
        "${CMAKE_COMMAND:-cmake}" --build "${BUILD_DIR}" --config "${Configuration}" --target codecov
    else
        (cd "${BUILD_DIR}" && "${CTEST_COMMAND:-ctest}" -C "${Configuration}" --output-on-failure)
    fi
}

#function copy_dylibs_recursive {
#    install_name_tool -add_rpath $2 $1
#    #copy libraries in the form /usr/local/lib/xxx ...
#    for dylib in $(otool -L $1 | grep $(brew --prefix) | sed 's; \(.*\);;' | xargs); do
#        install_name_tool -change $dylib @rpath/$(basename ${dylib}) $1
#        if [[ ! ${found_dylibs[@]} =~ (^|[[:space:]])"$dylib"($|[[:space:]]) ]]; then
#            found_dylibs+=("${dylib}")
#            echo "Copying $(basename ${dylib})..."
#            cp $dylib $3/
#            copy_dylibs_recursive $3/$(basename ${dylib}) @executable_path/. $3
#        fi
#    done
#}

# dylibbundler points every rpath at the bundle's libs directory, but dyld
# refuses Mach-O files with duplicate LC_RPATH entries.
function remove_duplicate_rpaths {
    local gdl_file gdl_rpath gdl_changed
    for gdl_file in "$@"; do
        if [[ -L "${gdl_file}" ]] || ! file -b "${gdl_file}" | grep -q '^Mach-O'; then continue; fi
        gdl_changed=false
        while IFS= read -r gdl_rpath; do
            install_name_tool -delete_rpath "${gdl_rpath}" "${gdl_file}" || return 1
            gdl_changed=true
        done < <(otool -l "${gdl_file}" | awk '$1 == "cmd" && $2 == "LC_RPATH" { getline; getline;
            sub(/^ *path /, ""); sub(/ \(offset [0-9]+\)$/, ""); if (seen[$0]++) print }')
        if ${gdl_changed}; then codesign --force --sign - "${gdl_file}" || return 1; fi
    done
}

# FlexiBLAS loads its backends at run time and finds them by name in Homebrew's
# configuration, so bundle the backends with a configuration naming them, and
# make bin/gdl a launcher that passes it in FLEXIBLAS_CONFIG. Run in Contents.
function bundle_flexiblas {
    local gdl_brew gdl_flexiblas gdl_main gdl_file gdl_ref gdl_backends=()
    gdl_brew=$(brew --prefix) || return 1
    gdl_flexiblas=$(brew --prefix flexiblas) || return 1
    gdl_main=$(otool -L Resources/bin/gdl | awk '/\/libflexiblas[.0-9]*\.dylib / { print $1; exit }')
    log "Bundling FlexiBLAS backends..."
    mkdir -p Resources/libs/flexiblas || return 1
    cp "${gdl_flexiblas}/lib/flexiblas/"*.dylib Resources/libs/flexiblas/ || return 1
    chmod u+w Resources/libs/flexiblas/*.dylib
    for gdl_file in Resources/libs/flexiblas/*.dylib; do gdl_backends+=( -x "${gdl_file}" ); done
    # Without -od, the libraries already bundled for gdl are kept.
    dylibbundler -of -b -s "${gdl_brew}/lib/" -s "${gdl_flexiblas}/lib/" "${gdl_backends[@]}" \
        -d Resources/libs || return 1
    # Backends must use gdl's libflexiblas; a second copy would be initialized separately.
    for gdl_file in Resources/libs/flexiblas/*.dylib; do
        while IFS= read -r gdl_ref; do
            if [[ "${gdl_ref}" != "${gdl_main}" ]]; then
                install_name_tool -change "${gdl_ref}" "${gdl_main}" "${gdl_file}" || return 1
                codesign --force --sign - "${gdl_file}" || return 1
            fi
        done < <(otool -L "${gdl_file}" | awk 'NR > 2 && /\/libflexiblas[.0-9]*\.dylib / { print $1 }')
    done
    for gdl_file in Resources/libs/libflexiblas[.0-9]*.dylib; do
        if [[ "${gdl_file##*/}" != "${gdl_main##*/}" ]]; then rm -f "${gdl_file}"; fi
    done
    # Only name the backends: the default stays FlexiBLAS's or the user's choice.
    cat "${gdl_brew}/etc/flexiblasrc.d/"*.conf > Resources/libs/flexiblas/flexiblasrc || return 1
    # GDL locates its data relative to the bin directory, so keep the executable there.
    mv Resources/bin/gdl Resources/bin/gdl-bin || return 1
    cat > Resources/bin/gdl <<'GDL_FLEXIBLAS_LAUNCHER'
#!/bin/sh
# Run GDL with the FlexiBLAS backends bundled in this application.
gdl_self=$0
while [ -h "$gdl_self" ]; do
    gdl_link=$(readlink "$gdl_self")
    case $gdl_link in
        /*) gdl_self=$gdl_link ;;
        *) gdl_self=$(dirname "$gdl_self")/$gdl_link ;;
    esac
done
gdl_bin=$(cd "$(dirname "$gdl_self")" && pwd -P) || exit 1
FLEXIBLAS_CONFIG=${FLEXIBLAS_CONFIG:-$gdl_bin/../libs/flexiblas/flexiblasrc}
export FLEXIBLAS_CONFIG
exec "$gdl_bin/gdl-bin" "$@"
GDL_FLEXIBLAS_LAUNCHER
    chmod +x Resources/bin/gdl
}

function pack_gdl {
    read_build_settings || return 1
    log "Packaging GDL..."
    if [ ${BUILD_OS} == "Windows" ]; then
        if [ $arch == "x86_64" ]; then
            cd "${ROOT_DIR}" || return 1
            download_file "https://github.com/gnudatalanguage/gdlde/releases/download/${GDLDE_VERSION}/gdlde.product-win32.win32.x86_64.zip"
            decompress_file -o -d gdlde
        fi

        mkdir -p "${ROOT_DIR}/package"
        cd "${ROOT_DIR}/package" || return 1

        export GDL_INSTALL_DIR=$(cygpath -w "${INSTALL_PREFIX}")
        export GDL_VERSION=$(grep -oP 'set\(VERSION "\K.+(?="\))' "${GDL_SRC_DIR}/CMakeLists.txt")
        makensis -V3 "${GDL_SRC_DIR}/scripts/deps/windows/gdlsetup.nsi"
    elif [ ${BUILD_OS} == "macOS" ]; then
        mkdir -p "${ROOT_DIR}/package/GNU Data Language.app/Contents"
        cd "${ROOT_DIR}/package/GNU Data Language.app/Contents"

        mkdir MacOS
        echo "#!/bin/sh" > MacOS/gdl
        echo 'export ARCHPREFERENCE="arm64,x86_64" ' >> MacOS/gdl
        echo 'SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"' >> MacOS/gdl
        echo 'open -a Terminal "${SCRIPTPATH}/../Resources/bin/gdl"' >> MacOS/gdl
        chmod +x MacOS/gdl

        mkdir Resources
        cp -R "${INSTALL_PREFIX}/"* Resources/
        cp -R "${GDL_SRC_DIR}/resource/"* Resources/

        mkdir Resources/libs
        dylibbundler -od -b -s $(brew --prefix)/lib/ -x Resources/bin/gdl -d Resources/libs || return 1
        if otool -L Resources/bin/gdl | grep -q '/libflexiblas[.0-9]*\.dylib '; then
            bundle_flexiblas || return 1
        fi
        remove_duplicate_rpaths Resources/bin/* Resources/libs/*.dylib Resources/libs/flexiblas/*.dylib || return 1
#        mkdir Frameworks
#        found_dylibs=()
#        copy_dylibs_recursive Resources/bin/gdl @executable_path/../../Frameworks Frameworks
#        cp  $(brew --prefix)/lib/libsz.*dylib Frameworks #copy link impossible
#
        echo '<?xml version="1.0" encoding="UTF-8"?>' > Info.plist
        echo '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> Info.plist
        echo '<plist version="1.0">' >> Info.plist
        echo '  <dict>' >> Info.plist
        echo '    <key>CFBundleExecutable</key>' >> Info.plist
        echo '    <string>gdl</string>' >> Info.plist
        echo '    <key>CFBundleIconFile</key>' >> Info.plist
        echo '    <string>gdl</string>' >> Info.plist
        echo '  </dict>' >> Info.plist
        echo '</plist>' >> Info.plist

        cd "${ROOT_DIR}/package"
        download_file "https://github.com/gnudatalanguage/gdlde/releases/download/${GDLDE_VERSION}/gdlde.product-macosx.cocoa.x86_64.zip"
        decompress_file
        rm gdlde.product-macosx.cocoa.x86_64.zip
        mv Eclipse.app "GDL Workbench.app" # TODO: this should not be necessary
    fi
}

function prep_deploy {
    read_build_settings || return 1
    cd "${GDL_SRC_DIR}" || return 1
    if [ ${BUILD_OS} == "macOS" ]; then
        hdiutil create "gdl-${BUILD_OS}-${arch}-${DEPS}.dmg" -ov -volname "GNU Data Language" -fs HFS+ -srcfolder "${ROOT_DIR}/package"
    else
        if [ ${BUILD_OS} == "Windows" ]; then
            mv gdlsetup.exe gdlsetup-${BUILD_OS}-${arch}-${DEPS}.exe
        fi
        cd "${INSTALL_PREFIX}" || return 1
        zip -qr "${GDL_SRC_DIR}/gdl-${BUILD_OS}-${arch}-${DEPS}.zip" *
    fi
}

function test_antlr {
    if ! which runantlr &> /dev/null; then
        log "ANTLR could not be found, try installing..."
        find_pkgmgr
        eval "sudo ${PKGMGR} ${PKGINSTALLARG} -y antlr"
    fi
    pushd ${GDL_SRC_DIR}/src
        for i in *.g; do
            runantlr $i
        done
        git diff -G -b '(^[ANTLR 2.7.7 (])' | tee diff.log
        if [ `cat diff.log | wc -l` -gt 0 ]; then
            log "Error: Compiled ANTLR files do not match!"
            exit 1
        fi
        log "Compiled ANTLR files match!"
    popd
}

AVAILABLE_OPTIONS="prep prep_dryrun configure config build install check test antlr pack prep_deploy"
AVAILABLE_OPTIONS_prep=prep_packages
AVAILABLE_OPTIONS_prep_dryrun=prep_packages_dryrun
AVAILABLE_OPTIONS_configure=configure_gdl
AVAILABLE_OPTIONS_config=configure_gdl
AVAILABLE_OPTIONS_build=build_gdl
AVAILABLE_OPTIONS_install=install_gdl
AVAILABLE_OPTIONS_check=test_gdl
AVAILABLE_OPTIONS_test=test_gdl
AVAILABLE_OPTIONS_antlr=test_antlr
AVAILABLE_OPTIONS_pack=pack_gdl
AVAILABLE_OPTIONS_prep_deploy=prep_deploy

if [ "$#" -lt 1 ] || { [ "$#" -gt 1 ] && [ "$1" != "configure" ] && [ "$1" != "config" ]; }; then
    echo "Usage: $ME `echo ${AVAILABLE_OPTIONS} | tr ' ' '|'`"
    echo "       $ME configure [CMake options...]"
    echo "       BUILD_DIR=/path/to/build (default: current directory; build/ when in the source root)"
    echo "       ROOT_DIR=/path/to/work selects ROOT_DIR/build for compatibility"
    echo "       GENERATOR=Ninja (or CMAKE_GENERATOR); CC/CXX select compilers for new builds"
    echo "       Configure preserves the cache; use a new BUILD_DIR for another toolchain."
    echo "       GSL_CBLAS_PROVIDER=AUTO|FLEXIBLAS|ACCELERATE|OPENBLAS|GSL (default: AUTO)"
    exit 1
else
    for optkey in ${AVAILABLE_OPTIONS}; do
        if [ $optkey == "$1" ]; then
            if [ ${BUILD_OS} == "Windows" ]; then
                find_architecture
            else
                export arch=$(uname -m)
            fi
            cmd=AVAILABLE_OPTIONS_$optkey
            shift
            "${!cmd}" "$@"
            exit $?
        fi
    done
fi

log "Fatal error! Unknown action: $1"
exit 1
