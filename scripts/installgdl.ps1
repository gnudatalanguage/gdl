$env:LOCALM32="c:/projects/gdl/mingw/mingw32"
$env:GCC="i686-6.4.0-posix-dwarf-rt_v5-rev0"
$env:PLPLOTDIR="c:/projects/gdl/mingw/plplot-5.13"
copy $env:LOCALM32\bin\*.dll c:\projects\gdl\install\gdl\bin
copy $env:PLPLOTDIR\bin\lib*.dll c:\projects\gdl\install\gdl\bin
#copy $env:wxWidgets_ROOT\lib\gcc_dll\*.dll c:\projects\gdl\install\gdl\bin
copy C:\mingw-w64\$env:GCC\mingw32\bin\*.dll c:\projects\gdl\install\gdl\bin
  # bring in the driver's dll to the installation directory.
  # there are also .driver_info files here the plplot needs to find.
  # also in share/plplot-5.13.0 there are mapping and color files.
cd c:/projects/gdl
copy $env:PLPLOTDIR/lib/plplot5.13.0/drivers/*.dll c:/projects/gdl/install/gdl/bin
copy -r $env:PLPLOTDIR/lib/plplot5.13.0 c:/projects/gdl/install/plplot5.13
copy -r $env:PLPLOTDIR/share/plplot5.13.0 c:/projects/gdl/install/plplot5.13/lib
copy INSTALL.plplot c:/projects/gdl/install
