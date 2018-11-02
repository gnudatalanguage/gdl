@echo off
cd c:\projects\gdl || echo c:\projects\gdl does not exist
C:
mkdir build
cd c:\projects\gdl\build

rem C:\msys64\usr\bin\bash.exe -lc "cd /c/projects/gdl/build && rm -rf CM* src testsuite"
rem -DCMAKE_SYSTEM_PREFIX_PATH=C:\projects\gdl\mingw\%MNAME%
set LOCALM32=C:\projects\gdl\mingw\%MNAME%
rem set PATH=%systemroot%\system32;%systemroot%;%systemroot%\System32\Wbem
rem set PATH=C:\Program Files (x86)\CMake\bin;%PATH%
rem echo %PATH%
rem GCC=i686-6.3.0-posix-dwarf-rt_v5-rev1
set PATH=C:\mingw-w64\%GCC%\%MNAME%\bin;%PATH%
rem GMname=graphicsmagick-1.3.27
set GMDIR=C:\projects\gdl\mingw\graphicsmagick-1.3.27-Q8
rem #   set GMDIR=C:\projects\gdl\mingw\GM-Q32
copy %GMDIR%\bin\*.dll C:\projects\gdl\mingw\%MNAME%\bin
rem I used C:\projects/gdl/mingw/mingw64 as the "-DCMAKE_SYSTEM_PREFIX_PATH" location,
rem which I thought would trump things found in the %PATH%, but that is not so now, evidently.  
set PATH=C:\projects\gdl\mingw\%MNAME%\bin;%PATH%

rem cmake-12 is issuing a warning about the use of <package>_ROOT logicals.
 set WXWIDGETS_ROOT=C:\projects\gdl\win32libs\wxwidgets-3.0.4
 
 set PLPLOTDIR=C:\projects\gdl\mingw\plplot-5.13
 cd c:\projects\gdl\build
echo %PATH%
cmake --version
cmake  c:\projects\gdl -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
      -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG" ^
      -DWXWIDGETS=OFF -DWXWIDGETSDIR=%WXWIDGETS_ROOT% ^
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
 set PATH=C:\projects\gdl\mingw\%MNAME%\bin;%PATH%
 set PATH=%PLPLOTDIR%\bin;%PATH%
 set PLPLOT_DRV_DIR=%PLPLOTDIR%\lib\plplot5.13.0\drivers
 set PLPLOT_LIB=%PLPLOTDIR%\share\plplot5.13.0

rem mingw32-make check
 cd c:\projects\gdl
 del build\src\CMakeFiles\gdl.dir\*.obj /Q || echo "error del build"
 Xcopy  .\install\gdl\share\gnudatalanguage\lib .\install\gdl\gdllib /I /Y /E /Q || echo error Xcopy
 echo %CONFIGURATION%
 if %CONFIGURATION%==Debug exit 0
exit
