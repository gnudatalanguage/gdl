cd C:\projects\gdl\win32libs
        cmake "plplot-5.13.0" "-Bplplot-5.13.0/build" -G "MinGW Makefiles" ^
          -DPL_HAVE_QHULL=OFF -DHAVE_SHAPELIB=OFF ^
          -DDEFAULT_NO_CAIRO_DEVICES=ON -DDEFAULT_NO_QT_DEVICES=ON -DDEFAULT_NO_BINDINGS=ON ^
          -DENABLE_cxx=ON -DOLD_WXWIDGETS=ON  -DENABLE_wxwidgets:BOOL=ON ^
          -DWITH_FREETYPE=OFF -DPLD_wxwidgets=ON -DPLD_pdf=OFF -DPLD_psttf=OFF -DPLD_wingdi=ON ^
          -DCMAKE_INSTALL_PREFIX=C:\projects\gdl\mingw\plplot-5.13 -DOLD_WXWIDGETS=ON ^
          -DENABLE_wxwidgets=ON -DwxWidgets_LIB_DIR=%WX_TOP%\lib\gcc_dll 

        cd plplot-5.13.0\build
        mingw32-make 
        mingw32-make install > makeinstall.out
rem This script does not build plplot because it can't open wxwidgets when building the test_dyndrivers.
rem Could not open driver module wxwidgets
rem libltdl error: No error information
