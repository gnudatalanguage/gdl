@echo off
cd c:\projects\gdl || echo c:\projects\gdl does not exist
C:
if not exist build mkdir build
cd c:\projects\gdl\build
if exist CMakeCache.txt del CMakeCache.txt
rem #   This script needs Extensions (cmd /E:ON)
if not defined GCC (
  echo " mingw-bldgdl.bat: GCC Needs to exist !! exiting..."
  exit 1
  )
if not defined MNAME (
  set MNAME=mingw32
  echo " mingw-bldgdl.bat: MNAME not defined. set =mingw32"
  ) else (
  echo " mingw-bldgdl.bat: %MNAME% build of GDL using mingw."
  )
rem C:\msys64\usr\bin\bash.exe -lc "cd /c/projects/gdl/build && rm -rf CM* src testsuite"
rem -DCMAKE_SYSTEM_PREFIX_PATH=C:\projects\gdl\mingw\%MNAME%
set LOCALM32=C:\projects\gdl\mingw\%MNAME%
set PATH=C:\mingw-w64\%GCC%\%MNAME%\bin;%PATH%
rem
rem ###### if this is set then it will be the source in the next copy using %GMDIR%
rem set GMDIR=C:\projects\gdl\mingw\graphicsmagick-1.3.27-Q8
rem
set DOGM=:BOOL=OFF
if defined GMNAME (
  echo " mingw-bldgdl.bat: GraphicsMagickdir build is ON !!"
  set GMDIR=C:\projects\gdl\mingw\GM-Q32
  set DOGM=:BOOL=ON
  copy /Y %GMDIR%\bin\*.dll C:\projects\gdl\mingw\%MNAME%\bin
  ) else (
  echo " mingw-bldgdl.bat: GraphicsMagickdir build is OFF !!"
  )
set PATH=C:\projects\gdl\mingw\%MNAME%\bin;%PATH%
echo "  mingw-bldgdl.bat:-DGRAPHICSMAGICK%DOGM% -DMAGICK%DOGM% -DGRAPHICSMAGICKDIR=%GMDIR% "
rem cmake-12 is issuing a warning about the use of <package>_ROOT logicals.
set wxWidgets_ROOT_DIR=
set WX_TOP=
if %HOMEPATH%==\Users\greg (
  set WXPREFIX=F:\plplot
  ) else (
  set WXPREFIX=C:\projects\gdl\win32libs
  )
set DOWX=:BOOL=ON
if defined WXNAME (
  echo " mingw-bldgdl.bat: wxWidgets build is ON !!"
  set wxWidgets_ROOT_DIR=%WXPREFIX%\%WXNAME%
  set WX_TOP=%wxWidgets_ROOT_DIR%
  ) else (
   echo " mingw-bldgdl.bat: wxWidgets build is OFF !!"
  set DOWX=:BOOL=OFF
  )

 set PLPLOTDIR=C:\projects\gdl\mingw\plplot-5.13
 cd c:\projects\gdl\build
echo %PATH%
cmake --version
cmake  c:\projects\gdl -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
      -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG" ^
      -DWXWIDGETS%DOWX% -DWXWIDGETSDIR=%WX_TOP% ^
      -DPLPLOTDIR=%PLPLOTDIR% ^
      -DCMAKE_INSTALL_PREFIX:PATH=c:\projects\gdl\install\gdl ^
      -DCMAKE_SYSTEM_PREFIX_PATH=%LOCALM32% ^
      -DGRAPHICSMAGICK%DOGM% -DMAGICK%DOGM% -DGRAPHICSMAGICKDIR=%GMDIR% ^
      -DPSLIB=OFF -DNETCDF=OFF -DHDF=OFF -DHDF5=OFF ^
      -DTIFF=OFF -DGEOTIFF=OFF -DLIBPROJ4=OFF ^
      -DFFTW=ON -DGSHHS=OFF -DPYTHON=OFF ^
      -DUSE_WINGDI_NOT_WINGCC=ON
 cd c:\projects\gdl
 move .\build\plplot .\plplot || echo "There was no plplot directory to move !!"
 mkdir .\gdl
 Xcopy  .\build .\gdl\  /I /Y /E /Q
 move gdl build\gdl
 move plplot build\plplot
 cd c:\projects\gdl\build
 mingw32-make -j4
mingw32-make install > gdlinstall.out
 set PATH=C:\projects\gdl\mingw\%MNAME%\bin;%PATH%
 set PATH=%PLPLOTDIR%\bin;%PATH%
 set PLPLOT_DRV_DIR=%PLPLOTDIR%\lib\plplot5.13.0\drivers
 set PLPLOT_LIB=%PLPLOTDIR%\share\plplot5.13.0

 mingw32-make check
 cd c:\projects\gdl
 del build\src\CMakeFiles\gdl.dir\*.obj /Q || echo "error del build"
 Xcopy  .\install\gdl\share\gnudatalanguage\lib .\install\gdl\gdllib /I /Y /E /Q || echo error Xcopy
 echo %CONFIGURATION%
 if %CONFIGURATION%==Debug exit 0
exit
