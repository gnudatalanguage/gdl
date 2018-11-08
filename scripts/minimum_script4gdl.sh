#!/bin/bash
#
# Alain Coulais, 3 Mars 2015, script under GNU GPL v3
#
# option "5" --> Vanilla; option "6" --> Git Master
#
# Changes:
# 2015-11-06 : cmake is now in httpS, allow to jump to a given step via $1
# 2016-01-04 : move to 0.9.6
# 2017-01-21 : move to 0.9.7
# 2017-01-31 : fixing various improvments, 
#              properly skipping Readline/GSL/Cmake if available
# 2017-02-01 : plplot 5.9.11 had critical problem with CMake 3+
#              --> if CMake 3.2 around (OSX), use plplot 5.11.1
#              (different policy on Linux)
# 2018-01-19 : move from CVS (deprecated at SF) to SVN
# 2018-08-01 : move from SVN to GIThub
#              change plplot to 5.13.0 (OSX) or 5.12 (other)
#              flag for OpenMP (OSX)
# 2018-08-07 : force cmake 3+, force plplot 5.12 or 5.13
#
# The purpose of this shell script is to automaticaly compile a minimum GDL
# as a basic user even if mandatory packages are not available
#
# Since FFTw and Eigen3 are not used here, it does not reflect
# the best performances you can have with GDL.
#
# To do that, the TGZ of useful dependancies are downloaded then compiled
# (with "curl" because Curl is by default on OSX)
# When a dependancy is downloaded but something goes bad,
# you can fix the script and run it again, downloading is not done again,
# and succesfull compilations not done again
#
# We prefer to strongly limit the use of options and flags ...
# Please remove manually local directories if needed (if troubles)
#
# Since 2015 Nov. 6, we can jump over steps already done
# no step : all is done
# step 1 : readline
# step 2 : GSL
# step 3 : CMake
# step 4 : Plplot
# step 5 : GDL 0.9.8 vanilla
# step 6 : GDL 0.9.8 Git
#
# $1 == $use_curl, $2 = $URL, $3 = $filename
run_wget_or_curl(){
    if [ $1 -eq 1 ] ; then
	curl $2 -O
    else
	wget $2
    fi
}
run_wget_or_curl_no_check(){
    if [ $1 -eq 1 ] ; then
	curl -L --insecure $2 -o $3
    else
	wget --no-check-certificate $2 -O $3
    fi
}
run_wget_or_curl_v2(){
    if [ $1 -eq 1 ] ; then
	curl -L $2 -o $3
    else
	wget $2 -O $3
    fi
}
#
# $1 = $filename, $2 = $expected_md5sum
check_md5sum(){
    OS="`uname`"
    if [ $OS = 'Darwin' ] ; then 
	md5value=`md5 -r $1 | cut -c -32`
    else
	md5value=`md5sum $1 | cut -c -32`
    fi
    if [ $md5value != $2 ] ; then
	echo 'Bad MD5sum check for : '$1
	exit
    fi
}

# URL we use (revised on 2018 Aug. 7)
#
READLINE_URL="ftp://ftp.gnu.org/gnu/readline/readline-6.3.tar.gz"
GSL_URL="ftp://ftp.gnu.org/gnu/gsl/gsl-1.16.tar.gz"
#
PLPLOT_URL512="https://sourceforge.net/projects/plplot/files/plplot/5.12.0%20Source/plplot-5.12.0.tar.gz/download?use_mirror=autoselect"
PLPLOT_URL513="https://sourceforge.net/projects/plplot/files/plplot/5.13.0%20Source/plplot-5.13.0.tar.gz/download?use_mirror=autoselect"
#
GDL_VANILLA_URL="http://downloads.sourceforge.net/project/gnudatalanguage/gdl/0.9.8/gdl-0.9.8.tgz"
GDL_CVS_URL="http://gnudatalanguage.cvs.sourceforge.net/viewvc/gnudatalanguage/gdl/?view=tar"
GDL_SVN_URL="https://svn.code.sf.net/p/gnudatalanguage/svn/trunk/gdl"
GDL_GIT_URL="https://codeload.github.com/gnudatalanguage/gdl/zip/master"
#
step=$1
if [ -z "$1" ] ; then
    echo "No STEP (numerical) argument supplied"
    step=1
fi
#
export RACINE=$PWD
#
# AC 2018-08-01 : obsolete : no more SVN
#
# switch it to 1 to have the SVN snapshot
#gdl_svn=0
#if [[ $step == 6 ]] ; then
#    command -v svn >/dev/null 2>&1 || { echo >&2 "SVN (Subversion) not found." #; exit;}
#    gdl_svn=1
#fi
gdl_git=0
if [[ $step == 6 ]] ; then
    # we don't need "git" program, just "unzip"
    gdl_git=1
fi

# switch it to 1 to have the final checks for GDL
gdl_check=0 

# CPU Core number, switch it to 1 if you don't want to overload
cpus=`getconf _NPROCESSORS_ONLN`

# curl or wget
# (curl seems to be always present on OSX, wget on GNU/Linux)

use_curl=1
use_wget=1
command -v curl >/dev/null 2>&1 || { echo >&2 "Curl not found." ; use_curl=0;}
command -v wget >/dev/null 2>&1 || { echo >&2 "Wget not found." ; use_wget=0;}

echo "Curl ? : " $use_curl
echo "Wget ? : " $use_wget

#
# take care of "dash" on Debian/Ubuntu
# https://wiki.ubuntu.com/DashAsBinSh

if [ "${use_curl}" = "0" ] &&  [ "${use_wget}" = "0" ] ; then
    echo "both Curl and Wget are missing ... you must provide at least one of them !"
    exit
fi

# ----------------------------------- READLINE -----------------------
# starting READLINE : if READLINE not found in default places or
# already compile locally, we compile it
echo "** preparing READLINE"
cd $RACINE
#
READLINE_PATH=""
#
# "official" READLINE on OSX is a mess
#
os_type=`uname -s`
if [ $os_type != Darwin ] ; then 
    if [ -d "/usr/include/readline" ] ; then
	READLINE_PATH=/usr
    elif [ -d "/usr/local/include/readline" ] ; then
	READLINE_PATH=/usr/local
    fi
fi
# Do we have already locally compile Readline ?
if [ -n $READLINE_PATH ] ; then
    if [ -d $RACINE/readline-6.3/Compilation/ ] ; then
	READLINE_PATH=$RACINE/readline-6.3/Compilation/
    fi
fi
if [ -n $READLINE_PATH ] ; then
    echo "READLINE PATH : "$READLINE_PATH
    echo "READLINE (re)compilation SKIPPED !"
fi
#
if [[ $step -le 1 && -z $READLINE_PATH ]] ; then
    if [ ! -e readline-6.3.tar.gz ] ; then
	run_wget_or_curl $use_curl $READLINE_URL
    fi
    tar -zxf readline-6.3.tar.gz
    cd readline-6.3
    mkdir Compilation
    ./configure --prefix=$RACINE/readline-6.3/Compilation/
    make -s
    make install
    READLINE_PATH=$RACINE/readline-6.3/Compilation/
    echo "readline Compilation done"
fi

# ----------------------------------- GSL -----------------------
# starting GSL : if GSL not found in default places or 
# already locally compiled, we compile it locally.
#
echo "** preparing GSL"
cd $RACINE
GSL_CONFIG=`which -a gsl-config`
if [ -z $GSL_CONFIG ] ; then 
    if [ -x $RACINE/gsl-1.16/Compilation/bin/gsl-config ] ; then
	GSL_PATH=`$RACINE/gsl-1.16/Compilation/bin/gsl-config --prefix`
    else
	GSL_PATH=""
    fi
else
    GSL_PATH=`gsl-config --prefix`
fi
if [ -n $GSL_PATH ] ; then
    echo "GSL PATH : "$GSL_PATH
    echo "GSL found, version : "`$GSL_PATH/bin/gsl-config --version`
    echo "GSL (re)compilation SKIPPED !"
fi
#
if [[ $step -le 2 && -z $GSL_PATH ]] ; then
    if [ ! -e gsl-1.16.tar.gz ] ; then
	run_wget_or_curl $use_curl $GSL_URL
    fi
    tar -zxf gsl-1.16.tar.gz
    cd gsl-1.16
    mkdir Compilation
    ./configure --prefix=$RACINE/gsl-1.16/Compilation/
    make -s
    make install 
    GSL_PATH=$RACINE/gsl-1.16
    echo "GSL compilation done, version : 1.16"
fi

# ----------------------------------- CMAKE -----------------------
# starting CMAKE : since mid-2018, we force Cmake to be > 3.0.2
echo "** preparing CMAKE"
cd $RACINE
#
do_cmake_compil=1
CmakeEXE=`which -a cmake`
#echo "CMake exe : " $CmakeEXE
if [ -x $CmakeEXE ] ; then 
    cmake_version=`cmake --version | head -1 | awk -F " " '{print $3}'`
    if [[ ${cmake_version:0:1} < "3" ]] ; then
	echo "old CMake version ("$cmake_version") found, a new one must be used"
    else
	do_cmake_compil=0
    fi
fi
#
if [[ $do_cmake_compil -eq 1 ]] ; then
    CMAKE_URL="https://cmake.org/files/v3.2/cmake-3.2.3.tar.gz"
    CMAKE_MD5="d51c92bf66b1e9d4fe2b7aaedd51377c"
    CMAKE_NAME=cmake-3.2.3.tar.gz
    CMAKE_DIR=cmake-3.2.3
fi
#
if [[ $do_cmake_compil -eq 1 && -x $RACINE/$CMAKE_DIR/bin/cmake ]] ; then
    CmakeEXE=$RACINE/$CMAKE_DIR/bin/cmake
    cmake_version=`$CmakeEXE --version | head -1 | awk -F " " '{print $3}'`
    do_cmake_compil=0
fi
if [ -n $CmakeEXE ] ; then
    echo "CMake EXE : "$CmakeEXE
    echo "CMake found, version : "$cmake_version
    echo "CMake (re)compilation SKIPPED !"
fi
#
if [[ $step -le 3 && $do_cmake_compil -eq 1 ]] ; then
    if [ ! -e $CMAKE_NAME ] ; then
	## since Nov. 6, we do have a problem with KitWare certificate ...
	run_wget_or_curl_no_check $use_curl $CMAKE_URL $CMAKE_NAME
    fi
    check_md5sum $CMAKE_NAME $CMAKE_MD5
    #
    tar -zxf $CMAKE_NAME
    cd $CMAKE_DIR
    ./bootstrap
    make -s
    echo "CMake compilation done, version $CMAKE_DIR"
    CmakeEXE=$PWD/bin/cmake
    cmake_version=`$CmakeEXE --version | head -1 | awk -F " " '{print $3}'`
fi
# ----------------------------------- PLPLOT -----------------------
# starting PLPLOT : we don't want to use packaged PLplot version
# because of various issues. The options used here ensure stable results.
echo "** preparing PLPLOT"
cd $RACINE
#
# AC 2018-08-07 : succesfully tested on OSX 10.11, 10.12 & 10.13 
# with cmake 3.11.3 from Brew
if [ $os_type == Darwin ] ; then
    PLPLOT_URL=$PLPLOT_URL513
    PLPLOT_MD5="bfefeae7fb9a23377c6dc37b44a7da8a"
    PLPLOT_NAME=plplot-5.13.0.tar.gz
    PLPLOT_DIR=plplot-5.13.0
else
    PLPLOT_URL=$PLPLOT_URL512
    PLPLOT_MD5="998a05be218e5de8f2faf988b8dbdc51"
    PLPLOT_NAME=plplot-5.12.0.tar.gz
    PLPLOT_DIR=plplot-5.12.0
fi
#
if [[ $step -le 4 ]] ; then 
    if [ ! -e $PLPLOT_NAME ] ; then
	run_wget_or_curl_no_check $use_curl $PLPLOT_URL $PLPLOT_NAME
    fi
    check_md5sum $PLPLOT_NAME $PLPLOT_MD5
    #
    tar -zxf $PLPLOT_NAME
    cd $PLPLOT_DIR
    mkdir Compilation
    cd Compilation
    $CmakeEXE .. -DCMAKE_INSTALL_PREFIX=.  -DENABLE_ADA=off \
	-DENABLE_f95=off -DENABLE_fortran=off \
	-DENABLE_python=OFF -DENABLE_java=off -DENABLE_qt=off \
	-DENABLE_tk=off -DENABLE_tcl=off -DPLD_aqt=off \
	-DPLD_psttf=off -DPLD_wxwidgets=OFF -DDEFAULT_NO_CAIRO_DEVICES=ON
    make -s -j $cpus
    make install 
    echo "Plplot done"
else 
    echo "Plplot SKIPPED !"
fi

# ----------------------------------- GDL -----------------------
# starting GDL : 2 cases : with the SVN or the 0.9.7 vanilla version
# we don't need to manage the step here ... (always 5 or 6)
echo "** preparing GDL"
cd $RACINE
#
# SVN Obsolete
#if [ "$gdl_svn" -eq 1 ] ; then
#    echo "preparing to compiled GDL 0.9.7 SVN version"
#    gdl_path='gdl-0.9.7svn'`date +%y%m%d`
#    svn checkout $GDL_SVN_URL $gdl_path
if [ "$gdl_git" -eq 1 ] ; then
    echo "preparing to compiled GDL 0.9.8 Git version"
    gdl_path='gdl-0.9.8git'`date +%y%m%d`
    gdl_name=${gdl_path}'.tgz'
    if [ ! -e $gdl_name ] ; then
	run_wget_or_curl_v2 $use_curl $GDL_GIT_URL $gdl_name
    fi
    unzip $gdl_name
    mv gdl-master $gdl_path
else 
    echo "preparing to compiled GDL 0.9.8 VANILLA version"
    gdl_path='gdl-0.9.8'
    gdl_name=${gdl_path}'.tgz'
    if [ ! -e $gdl_name ] ; then
	run_wget_or_curl_v2 $use_curl $GDL_VANILLA_URL $gdl_name
    fi
    tar -zxf $gdl_name
fi
#
# 2018-08-01 The default OSX CLang version does not support OpenMP 
if [ $os_type == Darwin ] ; then
    flag_openmp=OFF
else
    flag_openmp=ON
fi
#
cd $gdl_path
if [ -d "build" ]; then
    \rm build/
else
    mkdir build
fi
cd build
$CmakeEXE .. \
   -DREADLINEDIR=$READLINE_PATH -DGEOTIFF=off \
   -DGSLDIR=$GSL_PATH -DOPENMP=$flag_openmp \
   -DPLPLOTDIR=$RACINE/$PLPLOT_DIR/Compilation/ \
   -DWXWIDGETS=off -DMAGICK=OFF -DNETCDF=OFF -DHDF=OFF \
   -DHDF5=off -DFFTW=OFF -DEIGEN3=OFF -DPSLIB=OFF -DPYTHON=OFF
make -s -j $cpus
#
if [ "$gdl_check" -eq 1 ] ; then
    make check
fi
#
cd $RACINE
#
if [ -x $gdl_path/src/gdl ] ; then
    echo -e "\nCompilation of GDL is finished"
    echo "Please remember it does not reflect the full capabilities of GDL"
    echo -e "\nYou can run GDL calling : \n"
    echo -e "\tsh "$gdl_path"/quick_start_GDL.sh\n"
else 
    echo -e "\nIt seems a problem occured during the compilation of GDL"
    echo "Please check carrefully the outputs"
fi
