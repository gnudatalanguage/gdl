#
# Alain C. and Ilia N. : June 2015
# A simple script to help end users who can be sudo to add 
# as much as possible dependances needed for GDL compilation
# BEFORE runing "cmake"
#
# On Debian or Ubuntu systems, we need the "dpkg-query"
# command to check wether the packages are installed.
#
if ( ! which dpkg-query > /dev/null ); then
   echo -e "<dpkg-query> not found! Install? (y/n) \c"
   read
   if "$REPLY" = "y"; then
      sudo apt-get install dpkg-query
   fi
fi
#
echo 'Please wait ! need some time.'
echo ' '
#
mandatory_dep_list=(g++ cmake libgsl0-dev
    libplplot-dev libncurses-dev zlib1g-dev 
    libpng-dev xorg-dev libreadline-gplv2-dev)
#
dep_to_aptget=""
#
for dep in ${mandatory_dep_list[*]}; do
    # echo $dep
    status=$(dpkg-query -W -f='${Status}' $dep 2>/dev/null | grep -c "ok installed")
    #echo $status $dep
    if [[ ${status} -eq 0 ]] ; then
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
    libgraphicsmagick++-dev libudunits2-dev libwxgtk2.8-dev plplot12-driver-xwin plplot13-driver-xwin libproj-dev)
#
for dep in ${extra_dep_list[*]}; do
    status=$(dpkg-query -W -f='${Status}' $dep 2>/dev/null | grep -c "ok installed")
    #echo $status $dep
    if [[ ${status} -eq 0 ]] ; then
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
