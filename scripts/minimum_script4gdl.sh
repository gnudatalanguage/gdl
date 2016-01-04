#!/bin/bash
#
# Alain Coulais, 3 Mars 2015, script under GNU GPL v3
#
# Changes:
# 2015-11-06 : cmake is now in httpS, allow to junp to a given step via $1
# 2016-01-04 : move to 0.9.6
#
# The purpose of this shell script is to compile a minimum GDL
#
# To do that, the TGZ of useful dependancies are downloaded then compiled
# (with "curl" because Curl is by default on OSX)
# When a dependancy is downloaded but something goes bad,
# you can fix the script and run it again, downloading is not done again,
# and succesfull compilations not done again
#
# Since  Nov. 6, we can jump over steps already done
# no step : all is done
# step 1 : readline
# step 2 : GSL
# step 3 : CMake
# step 4 : Plplot
# step 5 : GDL 0.9.6 vanilla
# step 6 : GDL 0.9.6 CVS
#
step=$1
if [ -z "$1" ] ; then
    echo "No STEP (numerical) argument supplied"
    step=1
fi
#
export RACINE=$PWD
#
# switch it to 1 to have the CVS snapshot
gdl_cvs=0
if (( step == 6 )) ; then gdl_cvs=1 ; fi

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

# starting READLINE
cd $RACINE
if ((step <= 1 )) ; then 
    if [ ! -e readline-6.3.tar.gz ] ; then
	if [ $use_curl -eq 1 ] ; then
	    curl ftp://ftp.gnu.org/gnu/readline/readline-6.3.tar.gz -O
	else
	    wget ftp://ftp.gnu.org/gnu/readline/readline-6.3.tar.gz
	fi
    fi
    tar -zxf readline-6.3.tar.gz
    cd readline-6.3
    mkdir Compilation
    ./configure --prefix=$RACINE/readline-6.3/Compilation/
    make
    make install
    echo "readline done"
else
    echo "readline SKIPPED !"
fi

# starting GSL
cd $RACINE
if ((step <= 2 )) ; then 
    if [ ! -e gsl-1.16.tar.gz ] ; then
	if [ $use_curl -eq 1 ] ; then
	    curl ftp://ftp.gnu.org/gnu/gsl/gsl-1.16.tar.gz -O
	else
	    wget ftp://ftp.gnu.org/gnu/gsl/gsl-1.16.tar.gz
	fi
    fi
    tar -zxf gsl-1.16.tar.gz
    cd gsl-1.16
    mkdir Compilation
    ./configure --prefix=$RACINE/gsl-1.16/Compilation/
    make
    make install 
    echo "GSL done"
else
    echo "GSL SKIPPED !"
fi

# starting CMAKE
cd $RACINE
if ((step <= 3 )) ; then 
    if [ ! -e cmake-2.8.12.tar.gz ] ; then
	# since Nov. 6, we do have a problem with KitWare certificate ...
	if [ $use_curl -eq 1 ] ; then
	    curl --insecure https://cmake.org/files/v2.8/cmake-2.8.12.tar.gz -O
	else
	    wget --no-check-certificate https://cmake.org/files/v2.8/cmake-2.8.12.tar.gz
	fi
    fi
    # 
    OS="`uname`"
    if [ $OS = 'Darwin' ] ; then 
	md5value=`md5 cmake-2.8.12.tar.gz | cut -c -32`
    else
	md5value=`md5sum cmake-2.8.12.tar.gz | cut -c -32`
    fi
    if [ $md5value != '105bc6d21cc2e9b6aff901e43c53afea' ] ; then
	echo 'Bad MD5sum check for cmake-2.8.12.tar.gz'
	exit
    fi
    #
    tar -zxf cmake-2.8.12.tar.gz
    cd cmake-2.8.12
    ./bootstrap
    make
    echo "CMake done"
else
    echo "CMake SKIPPED !"
fi

# starting PLPLOT
cd $RACINE
if ((step <= 4 )) ; then 
    if [ ! -e plplot-5.9.11.tar.gz ] ; then
	if [ $use_curl -eq 1 ] ; then
	    curl -L http://sourceforge.net/projects/plplot/files/plplot/5.9.11%20Source/plplot-5.9.11.tar.gz -O
	else
	    wget http://sourceforge.net/projects/plplot/files/plplot/5.9.11%20Source/plplot-5.9.11.tar.gz
	fi
    fi
    tar -zxf plplot-5.9.11.tar.gz
    cd plplot-5.9.11/
    mkdir Compilation
    cd Compilation
    $RACINE/cmake-2.8.12/bin/cmake .. -DCMAKE_INSTALL_PREFIX=. \
	-DENABLE_python=OFF -DENABLE_java=off -DENABLE_qt=off \
	-DENABLE_tk=off -DENABLE_tcl=off \
	-DPLD_aqt=off -DPLD_psttf=off -DPLD_wxwidgets=OFF -DDEFAULT_NO_CAIRO_DEVICES=ON
    make -j $cpus
    make install 
    echo "Plplot done"
else
    echo "Plplot SKIPPED !"
fi

# starting GDL : 2 cases : with the CVS or the 0.9.6 vanilla version
# we don't need to manage the step here ... (always 5 or 6)
cd $RACINE

if [ "$gdl_cvs" -eq 1 ] ; then
    echo "preparing to compiled the CVS version"
    gdl_path='gdl-0.9.6cvs'`date +%y%m%d`
    gdl_name=${gdl_path}'.tgz'
    if [ ! -e $gdl_name ] ; then
	if [ $use_curl -eq 1 ] ; then
	    curl http://gnudatalanguage.cvs.sourceforge.net/viewvc/gnudatalanguage/?view=tar -o $gdl_name
	else
	    wget http://gnudatalanguage.cvs.sourceforge.net/viewvc/gnudatalanguage/?view=tar -O $gdl_name
	fi
    fi
# the GDL CVS TGZ file comes with a gnudatalanguage/gdl/ path inside ... we manage it
    tar -zxf $gdl_name
    mv gnudatalanguage/gdl $gdl_path
    rmdir gnudatalanguage
else 
    echo "preparing to compiled the 0.9.6 version"
    gdl_path='gdl-0.9.6'
    gdl_name=${gdl_path}'.tgz'
    if [ ! -e $gdl_name ] ; then
	if [ $use_curl -eq 1 ] ; then
	    curl -L http://sourceforge.net/projects/gnudatalanguage/files/latest/download?source=files -o $gdl_name
	else
	    wget http://sourceforge.net/projects/gnudatalanguage/files/latest/download?source=files -O $gdl_name
	fi
    fi
    tar -zxf $gdl_name
fi
#
cd $gdl_path
if [ -d "build" ]; then
  \rm build/
else
    mkdir build
fi
cd build
$RACINE/cmake-2.8.12/bin/cmake .. \
   -DREADLINEDIR=$RACINE/readline-6.3/Compilation/ \
   -DGSLDIR=$RACINE/gsl-1.16/Compilation/ \
   -DPLPLOTDIR=$RACINE/plplot-5.9.11/Compilation/ \
   -DWXWIDGETS=off -DMAGICK=OFF -DNETCDF=OFF -DHDF=OFF \
   -DHDF5=off -DFFTW=OFF -DEIGEN3=OFF -DPSLIB=OFF -DPYTHON=OFF
make -j $cpus
#
if [ "$gdl_check" -eq 1 ] ; then
    make check
fi
