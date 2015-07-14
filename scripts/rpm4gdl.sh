#
# Alain C. and Ilia N. : June 2015
# A simple script to help end users who can be sudo to add 
# as much as possible dependances needed for GDL compilation
# BEFORE runing "cmake"
#
# On Debian or Ubuntu systems, we need the "dpkg-query"
# command to check wether the packages are installed.
#
# On RPM-based systems (CentOs, Fedora, RH, Suse, Mageia ...)
# we try to use just "rpm -qa"
#
# know problem : CentOS 5: libX11-devel, Suse 11 : xorg-x11-devel
#
echo 'Please wait ! need some time.'
echo ' '
#
mandatory_dep_list=("readline-devel" "zlib-devel" "libpng-devel" "gsl-devel"
    "plplot-devel" "libX11-devel" "xorg-x11-devel")
#
dep_to_aptget=""
#
for dep in ${mandatory_dep_list[*]}; do
    # echo $dep
    status=$(rpm -qa $dep 2>/dev/null | grep -c "$dep")
    #| grep -c "$dep")
    #echo $status $dep
    if [[ ${status} -eq 0 ]] ; then
	dep_to_aptget=$dep_to_aptget" "$dep
    fi
done
#
#echo ${mandatory_dep_list[*]}
if [[ ${#dep_to_aptget} -gt 0 ]] ; then
    echo "Missing Mandatory packages you need to install :"
    echo "sudo rpm -i"${dep_to_aptget}
else
    echo "NO Missing Mandatory packages"	
fi
#
dep_to_aptget=""
extra_dep_list=(GraphicsMagick-c++-devel ImageMagick-c++-devel wxGTK-devel
    netcdf-devel hdf5-devel fftw-devel python-devel udunits2-devel
    eigen3-devel pslib-devel grib_api-devel)
#
for dep in ${extra_dep_list[*]}; do
    status=$(rpm -qa $dep 2>/dev/null | grep -c "$dep")
    #echo $status $dep
    if [[ ${status} -eq 0 ]] ; then
	dep_to_aptget=$dep_to_aptget" "$dep
    fi
done
#
echo ""
if [[ ${#dep_to_aptget} -gt 0 ]] ; then
    echo "Missing packages you may would like to install :"
    echo "sudo rpm -i"${dep_to_aptget}
else
    echo "NO missing extra packages"
fi
