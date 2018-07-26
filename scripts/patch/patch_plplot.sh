#!bin/bash.exe 
# apply patches to the baseline plplot-5.13 distro for the windows' drivers. 
patch -p3 -i /c/projects/gdl/scripts/patch/Findwx.cmake.patch
patch -p3 -i /c/projects/gdl/scripts/patch/drivers_wingcc.c.patch
patch -p3 -i /c/projects/gdl/scripts/patch/drivers_wingdi.c.patch
