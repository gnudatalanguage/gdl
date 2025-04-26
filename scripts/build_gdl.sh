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
Configuration=${Configuration:-"Release"}
DEPS=${DEPS:-"standard"}
real_path() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}
export GDL_DIR=$(real_path "$(dirname $0)/..")
export ROOT_DIR=${ROOT_DIR:-"${GDL_DIR}"}
INSTALL_PREFIX=${INSTALL_PREFIX:-"${ROOT_DIR}/install"}
PYTHONVERSION=${PYTHONVERSION:-"3.0.0"}
GDLDE_VERSION=${GDLDE_VERSION:-"v1.0.0"} # needed by 'pack' (at the moment Windows only)
NTHREADS=${NTHREADS:-$(getconf _NPROCESSORS_ONLN)} # find nthreads for make -j
BUILD_OS=$(uname)
DRY_RUN=false
if [[ ${BUILD_OS} == *"MSYS"* ]] || [[ ${BUILD_OS} == *"MINGW"* ]]; then
    BUILD_OS="Windows"
elif [[ ${BUILD_OS} == "Darwin" ]]; then
    BUILD_OS="macOS"
    Platform=${Platform:-$(arch)}
fi

# Build flags
if [[ ${DEPS} == "headless" ]]; then
    WITH_WXWIDGETS=${WITH_WXWIDGETS:-OFF}
    WITH_X11=${WITH_X11:-OFF}
else
    WITH_WXWIDGETS=${WITH_WXWIDGETS:-ON}
    WITH_X11=${WITH_X11:-ON}
fi
WITH_OPENMP=${WITH_OPENMP:-ON}
WITH_GRAPHICSMAGICK=${WITH_GRAPHICSMAGICK:-ON}
WITH_NETCDF=${WITH_NETCDF:-ON}
WITH_HDF=${WITH_HDF:-ON}
WITH_HDF5=${WITH_HDF5:-ON}
if [[ ${DEPS} != "headless" ]]; then
    WITH_MPI=${WITH_MPI:-OFF}
else
    WITH_MPI=${WITH_MPI:-ON}
fi
WITH_TIFF=${WITH_TIFF:-ON}
WITH_GEOTIFF=${WITH_GEOTIFF:-ON}
WITH_LIBPROJ=${WITH_LIBPROJ:-ON}
WITH_PYTHON=${WITH_PYTHON:-ON}
WITH_PYTHONVERSION=${WITH_PYTHONVERSION:-ON}
WITH_FFTW=${WITH_FFTW:-ON}
WITH_UDUNITS2=${WITH_UDUNITS2:-ON}
WITH_GLPK=${WITH_GLPK:-ON}
WITH_OPENMP=${WITH_OPENMP:-ON}
if [[ ${BUILD_OS} == "macOS" ]]; then
    WITH_HDF4=${WITH_HDF4:-OFF}
    WITH_GRIB=${WITH_GRIB:-ON}
    WITH_PYTHON="OFF"
    WITH_PYTHONVERSION="OFF"
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
    WITH_PYTHONVERSION="OFF"
fi
function log {  # log is needded just below!
    echo "[${ME}] $@"
}

if [ ${BUILD_OS} == "Windows" ]; then
    BSDXDR_URL="https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/bsd-xdr/bsd-xdr-1.0.0.tar.gz"
    MSYS2_PACKAGES=(
        readline zlib libpng gsl wxWidgets libgd libtiff libgeotiff netcdf hdf4 hdf5 fftw proj msmpi udunits
        eigen3 eccodes glpk shapelib expat openssl qhull
    )
# GD: dunno why graphicsmagick has this special treatment????
    #if you add something in MSYS2_PACKAGES_REBUILD you may have to add special lines in main.yml to push the product in /var/cache/pacman/pkg
    MSYS2_PACKAGES_REBUILD=(
        graphicsmagick
    )
elif [ ${BUILD_OS} == "Linux" ]; then
    # JP: Note the seperator ',' between the package name candidates below. The leftmost one has the highest priority.
    # Debian, Ubuntu, Linux Mint, Elementary OS, etc.
    APT_PACKAGES=(
        libtirpc-dev libncurses-dev libreadline-dev,libreadline-gplv2-dev zlib1g-dev libpng-dev libgsl-dev,libgsl0-dev
        libwxgtk3.0-gtk3-dev,libwxgtk3.0-dev,libwxgtk2.8-dev  libgraphicsmagick++1-dev,libgraphicsmagick++-dev libtiff-dev
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
    BREW_PACKAGES=(
        libx11 libomp ncurses readline zlib libpng gsl wxwidgets graphicsmagick libtiff libgeotiff netcdf hdf5 fftw proj open-mpi python numpy udunits eigen
        eccodes glpk shapelib expat gcc qhull dylibbundler cmake
    ) # JP 2021 Mar 21: HDF4 isn't available - not so critical I guess
      # JP 2021 May 25: Added GCC 10 which includes libgfortran, which the numpy tap relies on.
      # J-KL 2022 July 30: GCC 10 didn't work with apple silicon mac. So I replaced it with GCC 11
      # GD added dylibbundler that simplify building correct apps.
	  # GD 25/04/2025 do not use this gcc for compilation. Do as mantioned by homebrew for libomp 
	  # "For compilers to find libomp you may need to set:"
	  # "  export LDFLAGS="-L/usr/local/opt/libomp/lib"  "
	  # " export CPPFLAGS="-I/usr/local/opt/libomp/include"  "
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
    patch="${GDL_DIR}/scripts/deps/windows/mingw-patches/$1.patch"
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
            eval "pacman --noconfirm --needed -S ${msys2_packages}"
        fi

        for package_name in ${MSYS2_PACKAGES_REBUILD[@]}; do
            build_msys2_package $package_name
        done
	
#	pushd ${ROOT_DIR}
#        download_file "https://github.com/qhull/qhull/archive/refs/tags/2020.2.zip"
#        decompress_file
#        log "Building qhull..."
#        pushd qhull-2020.2
#        make SO=dll || exit 1
#        popd
#	popd

        download_file ${BSDXDR_URL}
        decompress_file

        log "Building bsd-xdr..."
        pushd bsd-xdr-1.0.0
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

#	pushd ${ROOT_DIR}
#        download_file "https://github.com/qhull/qhull/archive/refs/tags/2020.2.zip"
#        decompress_file
#        log "Building qhull..."
#        pushd qhull-2020.2
#        make || exit 1
#        popd
#	popd

        if [[ -z ${INSTALL_PACKAGES} ]]; then
            log "All required packages are already installed on your system."
	    else
            log "Installing packages:"
            log "${INSTALL_PACKAGES}"
            if [ ${DRY_RUN} == "true" ]; then
                log "Please run below command to install required packages to build GDL."
                echo "sudo ${PKGMGR} ${PKGINSTALLARG} -y ${INSTALL_PACKAGES}"
            else
                eval "sudo ${PKGMGR} ${PKGINSTALLARG} -y ${INSTALL_PACKAGES}"
            fi
        fi
    elif [ ${BUILD_OS} == "macOS" ]; then
        if ! command -v brew >/dev/null 2>&1; then
            log "Fatal error! Homebrew not found."
            exit 1
        fi
        brew update-reset
        log "Installing packages: ${BREW_PACKAGES[@]}"
        if [ ${DRY_RUN} == "true" ]; then
            log "Please run below command to install required packages to build GDL."
            echo "brew install ${BREW_PACKAGES[@]}"
        else
            eval "brew install ${BREW_PACKAGES[@]}"
        fi
    fi
}

function prep_packages_dryrun {
    DRY_RUN=true
    prep_packages
}

function find_dlls {
    for dll in $(strings $1 | grep -o '\S*\.dll' | grep -v $(basename $1)); do
        dll="/mingw64/bin/$dll"
        if [ -f "$dll" ] && [[ ! ${found_dlls[@]} =~ (^|[[:space:]])"$dll"($|[[:space:]]) ]]; then
            log "Found DLL dependency: $dll"
            found_dlls+=("$dll")
            find_dlls "$dll"
        fi
  done
}

function configure_gdl {
    log "Configuring GDL (${Configuration})..."

    if [[ ${DEPS} != "standard" && ${DEPS} != "headless" && ${DEPS} != "debug" ]]; then
        log "Fatal error! Unknown DEPS: ${DEPS}"
        exit 1
    fi
    
    rm -r ${ROOT_DIR}/build
    mkdir -p ${ROOT_DIR}/build
    cd ${ROOT_DIR}/build
    rm -f CMakeCache.txt  #each 'build' resets cmake, which is safer!

    if [ ${BUILD_OS} == "Windows" ]; then
        GENERATOR="MSYS Makefiles"
    else
        GENERATOR="Unix Makefiles"
    fi
    
    if [[ ${BUILD_OS} == "macOS" ]]; then
        if [[ ${Platform} == "arm64" ]]; then
	# suggested by homebrew
	        LDFLAGS="-L/opt/homebrew/opt/libomp/lib -lomp"
            CMAKE_ADDITIONAL_ARGS=( "-DOpenMP_CXX_FLAGS='-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include' -DMPI=OFF -DREADLINEDIR=/opt/homebrew/opt/readline") 
        else
	        LDFLAGS="-L/usr/local/opt/libomp/lib -lomp"
            CMAKE_ADDITIONAL_ARGS=( "-DOpenMP_CXX_FLAGS='-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include' -DMPI=OFF -DREADLINEDIR=/usr/local/opt/readline")
        fi
    fi

#    if [[ ${BUILD_OS} != "macOS" ]]; then
#        CMAKE_QHULLDIR_OPT="-DQHULLDIR="${ROOT_DIR}"/qhull-2020.2"
#    else
#        CMAKE_QHULLDIR_OPT=""
#    fi

    if [ ${BUILD_OS} == "Windows" ]; then
        export WX_CONFIG=${GDL_DIR}/scripts/deps/windows/wx-config-wrapper
    fi
    # The INTERACTIVE_GRAPHICS option is removed. 
    # Now plplot drivers is part of GDL
    cmake ${GDL_DIR} -G"${GENERATOR}" \
        -DCMAKE_BUILD_TYPE=${Configuration} \
        -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG" \
        -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} -DOPENMP=${WITH_OPENMP} \
        -DWXWIDGETS=${WITH_WXWIDGETS} -DX11={WITH_X11} -DGRAPHICSMAGICK=${WITH_GRAPHICSMAGICK} \
        -DNETCDF=${WITH_NETCDF} -DHDF=${WITH_HDF4} -DHDF5=${WITH_HDF5} \
        -DMPI=${WITH_MPI} -DTIFF=${WITH_TIFF} -DGEOTIFF=${WITH_GEOTIFF} \
        -DLIBPROJ=${WITH_LIBPROJ} -DPYTHON=${WITH_PYTHON} -DPYTHONVERSION=${PYTHONVERSION} -DFFTW=${WITH_FFTW} \
        -DUDUNITS2=${WITH_UDUNITS2} -DGLPK=${WITH_GLPK} -DGRIB=${WITH_GRIB} -DOPENMP=${WITH_OPENMP} \
         ${CMAKE_ADDITIONAL_ARGS[@]} ${CMAKE_QHULLDIR_OPT} \
        -DMACHINE_ARCH=${Platform}
}

function build_gdl {
    if [ ! -d "${ROOT_DIR}/build" ]; then 
        log "Fatal error! Build directory not found."
        exit 1
    fi

    cd ${ROOT_DIR}/build

    log "Building GDL..."
    make -j${NTHREADS} || exit 1
}


function install_gdl {
    if [ ! -d "${ROOT_DIR}/build" ]; then 
        log "Fatal error! Build directory not found."
        exit 1
    fi

    cd ${ROOT_DIR}/build

    log "Installing GDL..."
    make install || exit 1

    cd ${ROOT_DIR}/install
    echo "ROOT_DIR="${ROOT_DIR} 
    if [ ${BUILD_OS} == "Windows" ]; then
        log "Copying DLLs to install directory..."
        found_dlls=()
        find_dlls ${ROOT_DIR}/build/src/gdl.exe
        PYTHON_DLL=`ldd ${ROOT_DIR}/build/src/gdl.exe | tr '[:upper:]' '[:lower:]' | grep python | xargs | cut -d' ' -f3`
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
        cp -f ${GDL_DIR}/COPYING .
    fi
}

function test_gdl {
    log "Testing GDL..."
    cd ${ROOT_DIR}/build
    if [ -f ${GDL_DIR}/CMakeModules/CodeCoverage.cmake ]; then
        make codecov || exit 1
    else
        CTEST_OUTPUT_ON_FAILURE=1 make test || exit 1
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

function pack_gdl {
    log "Packaging GDL..."
    if [ ${BUILD_OS} == "Windows" ]; then
        if [ $arch == "x86_64" ]; then
            cd ${ROOT_DIR}
            download_file "https://github.com/gnudatalanguage/gdlde/releases/download/${GDLDE_VERSION}/gdlde.product-win32.win32.x86_64.zip"
            decompress_file -o -d gdlde
        fi

        mkdir -p ${ROOT_DIR}/package
        cd ${ROOT_DIR}/package

        export GDL_INSTALL_DIR=`cygpath -w ${ROOT_DIR}/install`
        export GDL_VERSION=`grep -oP 'set\(VERSION "\K.+(?="\))' ${GDL_DIR}/CMakeLists.txt`
        makensis -V3 ${GDL_DIR}/scripts/deps/windows/gdlsetup.nsi
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
        cp -R ${ROOT_DIR}/install/* Resources/
        cp -R ${GDL_DIR}/resource/* Resources/

        mkdir Resources/libs
        dylibbundler -od -b -s $(brew --prefix)/lib/ -x Resources/bin/gdl -d Resources/libs
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
    cd ${GDL_DIR}
    if [ ${BUILD_OS} == "macOS" ]; then
        hdiutil create "gdl-${BUILD_OS}-${arch}-${DEPS}.dmg" -ov -volname "GNU Data Language" -fs HFS+ -srcfolder "${ROOT_DIR}/package"
    else
        if [ ${BUILD_OS} == "Windows" ]; then
            mv gdlsetup.exe gdlsetup-${BUILD_OS}-${arch}-${DEPS}.exe
        fi
        cd ${ROOT_DIR}/install
        zip -qr ${GDL_DIR}/gdl-${BUILD_OS}-${arch}-${DEPS}.zip *
    fi
}

function test_antlr {
    if ! which runantlr &> /dev/null; then
        log "ANTLR could not be found, try installing..."
        find_pkgmgr
        eval "sudo ${PKGMGR} ${PKGINSTALLARG} -y antlr"
    fi
    pushd ${GDL_DIR}/src
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

AVAILABLE_OPTIONS="prep prep_dryrun configure build install check test antlr pack prep_deploy"
AVAILABLE_OPTIONS_prep=prep_packages
AVAILABLE_OPTIONS_prep_dryrun=prep_packages_dryrun
AVAILABLE_OPTIONS_configure=configure_gdl
AVAILABLE_OPTIONS_build=build_gdl
AVAILABLE_OPTIONS_install=install_gdl
AVAILABLE_OPTIONS_check=test_gdl
AVAILABLE_OPTIONS_test=test_gdl
AVAILABLE_OPTIONS_antlr=test_antlr
AVAILABLE_OPTIONS_pack=pack_gdl
AVAILABLE_OPTIONS_prep_deploy=prep_deploy

if [ "$#" -ne 1 ]; then
    echo "Usage: $ME `echo ${AVAILABLE_OPTIONS} | tr ' ' '|'`"
else
    for optkey in ${AVAILABLE_OPTIONS}; do
        if [ $optkey == "$1" ]; then
            if [ ${BUILD_OS} == "Windows" ]; then
                find_architecture
            else
                export arch=$(uname -m)
            fi
            cmd=AVAILABLE_OPTIONS_$optkey
            eval ${!cmd}
            break
        fi
    done
fi
