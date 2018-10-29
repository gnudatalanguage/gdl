@echo off
cd c:\projects\gdl || echo c:\projects\gdl does not exist
C:
mkdir build || echo c:\projects\gdl\build already existed

rem C:\msys64\usr\bin\bash.exe -lc "cd /c/projects/gdl/build && rm -rf CM* src testsuite"

REM move all that was loaded/built under win32libs/, to mingw/mingw32
set LOCALM32=C:\projects\gdl\mingw\%MNAME%
mkdir %LOCALM32%\bin || echo %LOCALM32%\bin already existed
move win32libs\bin\* %LOCALM32%\bin\
mkdir %LOCALM32%\lib || echo %LOCALM32%\lib already existed
move win32libs\lib\* %LOCALM32%\lib\
mkdir %LOCALM32%\include || echo %LOCALM32%\include already existed
move win32libs\include\* %LOCALM32%\include\
set PATH=C:\mingw-w64\%GCC%\%MNAME%\bin;%PATH%
rem GMname=graphicsmagick-1.3.27
set GMDIR=C:\projects\gdl\mingw\graphicsmagick-1.3.27-Q8
rem #   set GMDIR=C:\projects\gdl\mingw\GM-Q32
copy %GMDIR%\bin\*.dll %LOCALM32%\bin
echo "WX_TOP="%WX_TOP% ", PLPLOTDIR="%PLPLOTDIR% ", LOCALM32="%LOCALM32% 
 set PLPLOTDIR=C:\projects\gdl\mingw\plplot-5.13

cmake --version
cmake  c:\projects\gdl -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
      -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG" ^
      -DWXWIDGETS=ON -DWXWIDGETSDIR=%WX_TOP% ^
      -DPLPLOTDIR=%PLPLOTDIR% ^
      -DCMAKE_INSTALL_PREFIX:PATH=c:\projects\gdl\install\gdl ^
      -DCMAKE_SYSTEM_PREFIX_PATH=%LOCALM32% ^
      -DGRAPHICSMAGICK=ON -DMAGICK=ON -DGRAPHICSMAGICKDIR=%GMDIR% ^
      -DPSLIB=OFF -DNETCDF=OFF -DHDF=OFF -DHDF5=OFF ^
      -DTIFF=ON -DGEOTIFF=OFF -DLIBPROJ4=ON ^
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
 set PATH=%LOCALM32%\bin;%PATH%
 set PATH=%PLPLOTDIR%\bin;%PATH%
 set PLPLOT_DRV_DIR=%PLPLOTDIR%\lib\plplot5.13.0\drivers
 set PLPLOT_LIB=%PLPLOTDIR%\share\plplot5.13.0
rem make sure t get artifact, whatever the result. mingw32-make check
 cd c:\projects\gdl
 del build\src\CMakeFiles\gdl.dir\*.obj /Q || echo "error del build"
 Xcopy  .\install\gdl\share\gnudatalanguage\lib .\install\gdl\gdllib /I /Y /E /Q || echo error Xcopy
 echo %CONFIGURATION%
 if %CONFIGURATION%==Debug exit 0
exit
