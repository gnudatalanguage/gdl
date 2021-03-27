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

ME="build_gdl.sh"
Configuration=${Configuration:-"Debug"}
DEPS=${DEPS:-"full"}
real_path() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}
GDL_DIR=$(real_path "$(dirname $0)/..")
ROOT_DIR=${ROOT_DIR:-"${GDL_DIR}/.."}
BUILD_OS=$(uname)
if [[ ${BUILD_OS} == *"MSYS"* ]]; then
    BUILD_OS="Windows"
elif [[ ${BUILD_OS} == "Darwin" ]]; then
    BUILD_OS="macOS"
fi

if [ ${BUILD_OS} == "Windows" ]; then
    BSDXDR_URL="https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/bsd-xdr/bsd-xdr-1.0.0.tar.gz"
    MSYS2_PACKAGES=(
        readline zlib libpng gsl wxWidgets plplot libgd libtiff libgeotiff netcdf hdf4 hdf5 fftw proj msmpi python-numpy udunits
        eigen3 eccodes glpk shapelib expat
    )
    MSYS2_PACKAGES_REBUILD=(
        graphicsmagick
    )
elif [ ${BUILD_OS} == "Linux" ]; then
    # JP: Note the seperator ',' between the package name candidates below. The leftmost one has the highest priority.
    # Debian, Ubuntu, Linux Mint, Elementary OS, etc.
    APT_PACKAGES=(
        libncurses-dev libreadline-dev,libreadline-gplv2-dev zlib1g-dev libpng-dev libgsl-dev,libgsl0-dev
        libwxgtk3.0-gtk3-dev,libwxgtk3.0-dev,libwxgtk2.8-dev libplplot-dev libgraphicsmagick++1-dev,libgraphicsmagick++-dev libtiff-dev
        libgeotiff-dev libnetcdf-dev libhdf4-alt-dev libhdf5-dev libfftw3-dev libproj-dev libopenmpi-dev libpython3-dev,libpython-dev
        python3-numpy,python-numpy libudunits2-dev libeigen3-dev libeccodes-dev,libgrib-api-dev libglpk-dev shapelib libexpat1-dev
    )
    # Redhat, Fedora, Scientific Linux, CentOS, openSuSE, SLES, etc.
    RPM_PACKAGES=(
        ncurses-devel readline-devel zlib-devel libpng-devel,libpng16-devel gsl-devel wxGTK3-devel,wxGTK-devel,wxWidgets-3_2-devel,wxWidgets-3_0-devel
        plplot-wxGTK-devel,plplotwxwidgets-devel GraphicsMagick-c++-devel,libGraphicsMagick++-devel libtiff-devel libgeotiff-devel,libgeotiff5-devel
        netcdf-devel hdf-devel hdf5-devel fftw-devel,fftw3-devel proj-devel openmpi-devel python3-devel,python-devel
        python3-numpy-devel,python3-numpy,python-numpy udunits2-devel eigen3-devel eccodes-devel,grib_api-devel glpk-devel shapelib-devel,shapelib
        expat-devel,libexpat-devel
    ) # JP 2021 Mar 21: SuSE lacks udunits2 and eccodes
elif [ ${BUILD_OS} == "macOS" ]; then
    BREW_PACKAGES=(
        llvm libomp ncurses readline zlib libpng gsl wxmac graphicsmagick libtiff libgeotiff netcdf hdf5 fftw proj open-mpi numpy udunits eigen
        eccodes glpk shapelib expat
    ) # JP 2021 Mar 21: HDF4 isn't available - not so critical I guess
else
    log "Fatal error! Unknown OS: ${BUILD_OS}. This script only supports one of: Windows, Linux, macOS."
    exit 1
fi

function log {
    echo "[${ME}] $@"
}

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
    patch="${GDL_DIR}/.ci/$1.patch"
    if [[ -f $patch ]]; then
        patch -p0 -i $patch
    fi
    if [[ ${arch} == "x86_64" ]]; then
        makepkg_conf=/etc/makepkg_mingw64.conf
    else
        makepkg_conf=/etc/makepkg_mingw32.conf
    fi
    eval `cat ${makepkg_conf} | grep PKGEXT=`
    makepkg --config ${makepkg_conf} --noconfirm --syncdeps --install
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
        if [ test_package[1] == "(none)" ]; then
            return 1
        fi
    elif [ ${PKGMGR} == "yum" ]; then
        test_package=`yum info -C --available $1 2>&1 | grep Error`
        if [ ! -n "$test_package" ]; then
            return 1
        fi
    elif [ ${PKGMGR} == "zypper" ]; then
        test_package=`zypper info $1 | grep "not found"`
        if [ ! -n "$test_package" ]; then
            return 1
        fi
    fi
    return 0 # not found
}

function prep_packages {
    if [ ${BUILD_OS} == "Windows" ]; then
        for pkgname in ${MSYS2_PACKAGES[@]}; do
            msys2_packages="${msys2_packages} mingw-w64-${arch}-${pkgname}"
        done
        
        log "Installing dependencies: ${msys2_packages}"
        eval "pacman --noconfirm -S ${msys2_packages}"

        for package_name in ${MSYS2_PACKAGES_REBUILD[@]}; do
            build_msys2_package $package_name
        done

        log "Patching wx-config..."
        sed -e "s;-Wl,--subsystem,windows -mwindows;;" -i /${mname}/bin/wx-config

        download_file ${BSDXDR_URL}
        decompress_file

        log "Building bsd-xdr..."
        pushd bsd-xdr-1.0.0
        sed -e 's/-Wall/-Wall -Wno-pointer-to-int-cast #/' -e 's/$(XDR_LIBRARIES) $(TEST_PROGS)/$(XDR_LIBRARIES)/' -i Makefile
        make || exit 1
        cp -f mingw/*.dll /${mname}/bin/
        cp -f mingw/libxdr.dll.a /${mname}/lib/
        cp -rf rpc /${mname}/include/
        popd
    elif [ ${BUILD_OS} == "Linux" ]; then
        # JP: This part is based on `aptget4gdl.sh` and `rpm4gdl.sh` by Alain C. and Ilia N.
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
        log "Checking package availabilities..."
        for pkgnamecandidates in ${PACKAGES[@]}; do
            for pkgname in $(echo $pkgnamecandidates | tr ',' ' '); do
                if query_package $pkgname; then
                    INSTALL_PACKAGES="${INSTALL_PACKAGES} $pkgname"
                    break
                fi
            done
        done
        log "Installing packages:"
        log "${INSTALL_PACKAGES}"
        eval "sudo ${PKGMGR} ${PKGINSTALLARG} -y ${INSTALL_PACKAGES}"
    elif [ ${BUILD_OS} == "macOS" ]; then
        if ! command -v brew >/dev/null 2>&1; then
            log "Fatal error! Homebrew not found."
            exit 1
        fi
        brew update-reset
        brew unlink python@2
        log "Installing packages: ${BREW_PACKAGES[@]}"
        eval "brew install ${BREW_PACKAGES[@]}"
        log "Installing plplot..."
        brew --cache plplot
        bash $GDL_DIR/scripts/brew_enable_wxwidgets
    fi
}

function build_gdl {
    log "Building GDL (${Configuration})..."
    mkdir -p ${ROOT_DIR}/build
    cd ${ROOT_DIR}/build

    if [ ${BUILD_OS} == "Windows" ]; then
        GENERATOR="MSYS Makefiles"
    else
        GENERATOR="Unix Makefiles"
    fi
    
    if [[ ${BUILD_OS} == "macOS" ]]; then
        export LIBRARY_PATH=$LIBRARY_PATH:/usr/local/opt/llvm/lib
        CMAKE_ADDITIONAL_ARGS=( "-DREADLINEDIR=/usr/local/opt/readline"
                                "-DCMAKE_CXX_COMPILER=/usr/local/opt/llvm/bin/clang++"
                                "-DCMAKE_C_COMPILER=/usr/local/opt/llvm/bin/clang" )
    fi
    echo $PATH
    if [[ ${DEPS} == *"full"* ]]; then
        if [[ ${DEPS} == *"msmpi"* || ! ${BUILD_OS} == "Windows" ]]; then
            WITH_MPI="ON"
        else
            WITH_MPI="OFF"
        fi
        if [[ ${BUILD_OS} == "macOS" ]]; then
            WITH_HDF4="OFF"
        else
            WITH_HDF4="ON"
        fi
        cmake ${GDL_DIR} -G"${GENERATOR}" \
          -DCMAKE_BUILD_TYPE=${Configuration} \
          -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG" \
          -DCMAKE_INSTALL_PREFIX="${ROOT_DIR}/install" \
          -DWXWIDGETS=ON -DGRAPHICSMAGICK=ON \
          -DNETCDF=ON -DHDF=${WITH_HDF4} -DHDF5=ON \
          -DMPI=${WITH_MPI} -DTIFF=ON -DGEOTIFF=ON \
          -DLIBPROJ=ON -DPYTHON=ON -DFFTW=ON \
          -DUDUNITS2=ON -DGLPK=ON -DGRIB=ON \
          -DUSE_WINGDI_NOT_WINGCC=ON ${CMAKE_ADDITIONAL_ARGS[@]}
    else
        cmake ${GDL_DIR} -G"${GENERATOR}" \
          -DCMAKE_BUILD_TYPE=${Configuration} \
          -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG" \
          -DCMAKE_INSTALL_PREFIX="${ROOT_DIR}/install" \
          -DREADLINE=OFF -DPNGLIB=OFF -DOPENMP=OFF \
          -DGRAPHICSMAGICK=OFF -DWXWIDGETS=OFF \
          -DINTERACTIVE_GRAPHICS=OFF \
          -DNETCDF=OFF -DHDF=OFF -DHDF5=OFF -DFFTW=OFF \
          -DLIBPROJ=OFF -DMPI=OFF -DPYTHON=OFF -DUDUNITS2=OFF \
          -DEIGEN3=OFF -DGRIB=OFF -DGLPK=OFF -DTIFF=OFF \
          -DGEOTIFF=OFF -DSHAPELIB=OFF -DEXPAT=OFF \
          -DUSE_WINGDI_NOT_WINGCC=ON ${CMAKE_ADDITIONAL_ARGS[@]}
    fi
    
    make -j2 || exit 1
    
    if [ ${BUILD_OS} == "Windows" ]; then
        # Copy dlls and libraries to src directory
        mkdir -p share
        cp -rf /${mname}/share/plplot* share/
        if [[ ${DEPS} == *"full"* ]]; then
            mkdir -p lib
            cp -rf /${mname}/lib/GraphicsMagick* lib/ # copy GraphicsMagick dlls
        fi
    fi

    log "Installing GDL..."
    make install || exit 1

    cd ${ROOT_DIR}/install

    if [ ${BUILD_OS} == "Windows" ]; then
        # Copy dlls and libraries to install directory
        for f in $(ldd ${ROOT_DIR}/build/src/gdl.exe | grep "/mingw" | awk '{print $3}'); do
            cp -f $f bin/
        done
        cp -rf ${ROOT_DIR}/build/share .
        if [[ ${DEPS} == *"full"* ]]; then
            cp -rf ${ROOT_DIR}/build/lib .
        fi
        cp -f ${GDL_DIR}/COPYING .
    fi
}

function test_gdl {
    log "Testing GDL..."
    cd ${ROOT_DIR}/build
    make check
}

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
        makensis -V3 ${GDL_DIR}/.ci/gdlsetup.nsi && mv ${GDL_DIR}/.ci/gdlsetup.exe .
    fi
}

function prep_deploy {
    if [ ${BUILD_OS} == "Windows" ]; then
        cd ${ROOT_DIR}/gdl
        mv ../package/gdlsetup.exe gdlsetup-${BUILD_OS}-${arch}-${DEPS}.exe
    fi
    cd ${ROOT_DIR}/install
    zip -qr ../gdl/gdl-${BUILD_OS}-${arch}-${DEPS}.zip *
    cd ${ROOT_DIR}/gdl
}

AVAILABLE_OPTIONS="prep build check pack prep_deploy"
AVAILABLE_OPTIONS_prep=prep_packages
AVAILABLE_OPTIONS_build=build_gdl
AVAILABLE_OPTIONS_check=test_gdl
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
                export arch="x86_64"
            fi
            cmd=AVAILABLE_OPTIONS_$optkey
            eval ${!cmd}
            break
        fi
    done
fi