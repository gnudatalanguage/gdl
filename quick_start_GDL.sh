#!/bin/bash
#
# must be run as a BASH Shell
#
# AC: improved shell to avoid "big" when sourcing (or ".") this file ...
# echo "(ba)sh or source ? "$BASH_SOURCE
#
# AC 01/10/2010: we can keep the current $GDL_PATH using -g [--gdl_path]
#

Absolute_PATH="`dirname \"$0\"`" 
Absolute_PATH="`( cd \"$Absolute_PATH\" && pwd )`"
#
if [ $BASH_SOURCE != $0 ] ; then
   echo "Please run this script throw one in the 2 next commands:"
   echo "1/    ./quick_run_GDL.sh"
   echo "2/    bash ./quick_run_GDL.sh"
   return
fi
#
# AC 2011/08/19 to avoid conflict with other IDL/GDL environmments
# which have been setup before
#
REF_IDL_PATH=${IDL_PATH}
REF_GDL_PATH=${GDL_PATH}
REF_IDL_STARTUP=${IDL_STARTUP}
REF_GDL_STARTUP=${GDL_STARTUP}
#
IDL_PATH=""
GDL_PATH=""
IDL_STARTUP=""
GDL_STARTUP=""
#
if [ -z $flag_help ] ;     then flag_help=0 ; fi
if [ -z $flag_quiet ] ;    then flag_quiet=0 ; fi
if [ -z $flag_valgrind ] ; then flag_valgrind=0 ; fi
if [ -z $flag_gdl_path ] ; then flag_gdl_path=0 ; fi
# 
while getopts ":ghqv-:" opt ; do
    case $opt in 
	- ) case $OPTARG in
	    quiet* )    echo "Quiet keyword swith ON"
	                flag_quiet=1 ;;
	    gdl_path)   echo "Keeping GDL_PATH swith ON"
	                flag_gdl_path=1 ;;
	    help* )     echo "Help keyword swith ON"
                        flag_help=1 ;;
	    valgrind* ) echo "Valgrind keyword swith ON"
                        flag_valgrind=1 ;;
	    ? )         echo "option illegal -$OPTARG
	                exit ;;
	    * )         echo "option illegal -$OPTARG
	                exit
	esac ;;
    q )  echo "Quiet keyword swith ON"
         flag_quiet=1 ;;
    g )  echo "Keeping GDL_PATH swith ON"
         flag_gdl_path=1 ;; 
    h )  echo "Help keyword swith ON"
         flag_help=1 ;;
    v* ) echo "Valgrind keyword swith ON"
         flag_valgrind=1 ;;
    ? )  echo "option illegal -$OPTARG"
         exit ;;
    esac
done
#
if [ "$flag_help" -eq 1 ] ; then
    echo "Usage: gdl -g [--gdl_path] -h [--help] -q [--quiet] -v [--valgrind]"
    exit
fi
#
if [ "$flag_quiet" -eq 0 ] ; then
    echo "Running current GDL with ./src/pro and ./testsuite in GDL_PATH"
fi
#
if [ "$flag_gdl_path" -eq 0 ] ; then
    export GDL_PATH='+'${Absolute_PATH}'/src/pro/:+'${Absolute_PATH}'/testsuite/'
else
    export GDL_PATH='+'${Absolute_PATH}'/src/pro/:+'${Absolute_PATH}'/testsuite/:'${REF_GDL_PATH}
fi
#
if [ "$flag_quiet" -eq 0 ] ; then
    echo $GDL_PATH
fi
#
if [ "$flag_valgrind" -eq 1 ] ; then
    valgrind ${Absolute_PATH}/src/gdl
else
    ${Absolute_PATH}/src/gdl
fi
#
# restoring initial envionment
#
export IDL_PATH=${REF_IDL_PATH}
export GDL_PATH=${REF_GDL_PATH}
export IDL_STARTUP=${REF_IDL_STARTUP}
export GDL_STARTUP=${REF_GDL_STARTUP}
