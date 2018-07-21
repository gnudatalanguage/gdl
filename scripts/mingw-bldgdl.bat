@echo off
cd c:\projects\gdl || echo c:\projects\gdl does not exist
C:
mkdir build
cd c:\projects\gdl\build
set OLDPATH=%PATH%
rem C:\msys64\usr\bin\bash.exe -lc "cd /c/projects/gdl/build && rm -rf CM* src testsuite"
rem -DCMAKE_SYSTEM_PREFIX_PATH=C:\projects\gdl\mingw\mingw32
set LOCALM32=C:\projects\gdl\mingw\mingw32
set PATH=%systemroot%\system32;%systemroot%;%systemroot%\System32\Wbem
set PATH=C:\Program Files (x86)\CMake\bin;%PATH%
echo %PATH%
set GCC=i686-6.3.0-posix-dwarf-rt_v5-rev1
set PATH=C:\mingw-w64\%GCC%\mingw32\bin;%PATH%

 set PATH=C:\projects\gdl\mingw\mingw32\bin;%PATH%
 set WXWIDGETS_ROOT=C:\projects\gdl\win32libs\wxwidgets-3.0.4
 set PLPLOTDIR=C:\projects\gdl\mingw\plplot-5.13
 cd c:\projects\gdl\build
cmake  c:\projects\gdl -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
      -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG" ^
      -DWXWIDGETS=OFF -DWXWIDGETSDIR=%WXWIDGETS_ROOT% ^
      -DPLPLOTDIR=%PLPLOTDIR% ^
      -DCMAKE_INSTALL_PREFIX:PATH=c:\projects\gdl\install\gdl ^
      -DGRAPHICSMAGICK=ON -DMAGICK=ON ^
      -DPSLIB=OFF -DNETCDF=OFF -DHDF=OFF -DHDF5=OFF ^
      -DTIFF=OFF -DGEOTIFF=OFF -DLIBPROJ4=ON ^
      -DFFTW=ON -DGSHHS=OFF -DPYTHON=OFF ^
      -DUSE_WINGDI_NOT_WINGCC=ON > cmake.out
 cd c:\projects\gdl
 move .\build\plplot .\plplot || echo "There was no plplot directory to move !!"
 mkdir .\gdl
 Xcopy  .\build .\gdl\  /I /Y /E /Q
 move gdl build\gdl
 move plplot build\plplot
 cd c:\projects\gdl\build
 mingw32-make -j4
 mingw32-make install > gdlinstall.out
 set PATH=%PLPLOTDIR%\bin;%PATH%
 set PLPLOT_DRV_DIR=%PLPLOTDIR%\lib\plplot5.13.0\drivers
 set PLPLOT_LIB=%PLPLOTDIR%\share\plplot5.13.0
echo %PATH%
 mingw32-make check
 cd c:\projects\gdl
 del build\src\CMakeFiles\gdl.dir\*.obj /Q
 Xcopy  .\install\gdl\share\gnudatalanguage\lib .\install\gdl\gdllib /I /Y /E /Q
exit
