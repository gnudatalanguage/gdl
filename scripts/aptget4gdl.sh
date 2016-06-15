#
# Alain C. and Ilia N. : June 2015
# A simple script to help end users who can be sudo to add 
# as much as possible dependances needed for GDL compilation
# BEFORE runing "cmake"
#
# On Debian or Ubuntu systems, we need the "dpkg-query"
# command to check wether the packages are installed.
#
# Alain C. June 2016
# We switch from "dpkg-query" to "apt-cache"
# which is more realiable (would have to use "dpkg-query --print-avail")
#
if ( ! which apt-cache > /dev/null ); then
   echo -e "<apt-cache> not found! Install? (y/n) \c"
   read
   if "$REPLY" = "y"; then
      sudo apt-get install apt
   fi
fi
#
echo 'Please wait ! need some time.'
echo ' '
#
mandatory_dep_list=(g++ cmake libgsl0-dev
    libplplot-dev libncurses-dev zlib1g-dev 
    libpng-dev xorg-dev libreadline-gplv2-dev libreadline-dev)
#
dep_to_aptget=""
#
for dep in ${mandatory_dep_list[*]}; do
    # echo $dep
    test_installed=( `LANG=C apt-cache policy $dep | grep "Installed:" ` )
    if [ "${test_installed[1]}" == "(none)" ] ; then
	#echo $test_installed $dep $can_instal
	dep_to_aptget=$dep_to_aptget" "$dep
    fi
done
#
#echo ${mandatory_dep_list[*]}
if [[ ${#dep_to_aptget} -gt 0 ]] ; then
    echo "Missing Mandatory packages you need to install :"
    echo "sudo apt-get install"${dep_to_aptget}
else
    echo "NO Missing Mandatory packages"	
fi
#
dep_to_aptget=""
extra_dep_list=(libgrib-api-dev libnetcdf-dev libfftw3-dev
    libeigen3-dev libhdf4-alt-dev libhdf5-dev pslib-dev 
    libgraphicsmagick++-dev libudunits2-dev
    libwxgtk2.8-dev plplot12-driver-xwin plplot13-driver-xwin libproj-dev)
#
for dep in ${extra_dep_list[*]}; do
    test_installed=( `LANG=C apt-cache policy $dep | grep "Installed:" ` )
    if [ "${test_installed[1]}" == "(none)" ] ; then
	dep_to_aptget=$dep_to_aptget" "$dep
    fi
done
#
echo ""
if [[ ${#dep_to_aptget} -gt 0 ]] ; then
    echo "Missing packages you may would like to install :"
    echo "sudo apt-get install"${dep_to_aptget}
else
    echo "NO missing extra packages"
fi
