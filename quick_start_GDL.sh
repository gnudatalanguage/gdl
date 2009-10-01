#/bin/sh
#
if [ -z $flag_help ] ;     then flag_help=0 ; fi
if [ -z $flag_quiet ] ;    then flag_quiet=0 ; fi
if [ -z $flag_valgrind ] ; then flag_valgrind=0 ; fi
# 
while getopts ":hqv-:" opt ; do
    case $opt in 
	- ) case $OPTARG in
	    quiet* )    echo "Quiet keyword swith ON"
	                flag_quiet=1 ;;
	    help* )     echo "Help keyword swith ON"
                        flag_help=1 ;;
	    valgrind* ) echo "Valgrind keyword swith ON"
                        flag_valgrind=1 ;;
	    ? )         echo "option illegal -$OPTARG
	                exit ;;
	    * )         echo "option illegal -$OPTARG
	                exit
	esac ;;
    q )   echo "Quiet keyword swith ON"
          flag_quiet=1 ;;
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
    echo "Usage: gdl -h [--help] -q [--quiet] -v [--valgrind]"
    exit
fi
#
if [ "$flag_quiet" -eq 0 ] ; then
    echo "Running current GDL with ./src/pro and ./testsuite in GDL_PATH"
fi
#
export GDL_PATH='+'${PWD}'/src/pro/:+'${PWD}'/testsuite/'
if [ "$flag_quiet" -eq 0 ] ; then
    echo $GDL_PATH
fi
#
if [ "$flag_valgrind" -eq 1 ] ; then
    valgrind ./src/gdl
else
    ./src/gdl
fi

