# this is a script for making a WIN32-compatible mingw wxwidgets for GDL
# (lifted from an appveyor build)
cd c:\projects\gdl\win32libs
appveyor DownloadFile https://github.com/wxWidgets/wxWidgets/releases/download/v3.0.4/wxWidgets-3.0.4.7z
7z x wxWidgets-3.0.4.7z -y -o"wxWidgets-3.0.4"
cd c:\projects\gdl\win32libs\wxWidgets-3.0.4\build\msw
mingw32-make SHELL=cmd -f makefile.gcc setup_h BUILD=release SHARED=1 USE_GUI=1 USE_XRC=0 USE_HTML=0 USE_WEBVIEW=0 USE_MEDIA=0 USE_AUI=0 USE_RIBBON=0 USE_PROPGRID=0 USE_RICHTEXT=0 USE_STC=0 USE_OPENGL=0 VENDOR=gdl DEBUG_FLAG=1
mingw32-make SHELL=cmd -f makefile.gcc -j4 BUILD=release SHARED=1 USE_GUI=1 USE_XRC=0 USE_HTML=0 USE_WEBVIEW=0 USE_MEDIA=0 USE_AUI=0 USE_RIBBON=0 USE_PROPGRID=0 USE_RICHTEXT=0 USE_STC=0 USE_OPENGL=0 VENDOR=gdl DEBUG_FLAG=1
#Below 2 lines are required for wxWidgets-3.0.4, don't know why
copy c:\projects\gdl\win32libs\wxWidgets-3.0.4\build\msw\gcc_mswudll\coredll_headerctrlg.o c:\projects\gdl\win32libs\wxWidgets-3.0.4\build\msw\gcc_mswudll\coredll_headerctrlgo
mingw32-make SHELL=cmd -f makefile.gcc -j4 BUILD=release SHARED=1 USE_GUI=1 USE_XRC=0 USE_HTML=0 USE_WEBVIEW=0 USE_MEDIA=0 USE_AUI=0 USE_RIBBON=0 USE_PROPGRID=0 USE_RICHTEXT=0 USE_STC=0 USE_OPENGL=0 VENDOR=gdl DEBUG_FLAG=1
