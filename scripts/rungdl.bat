@echo off
@set cur=%cd%
@set path=%cd%\gdl\bin;%path%
@set GDL_PATH=+%cd%\gdl\gdllib
@set PLPLOT_DRV_DIR=%cd%\plplot5.13\drivers
@set PLPLOT_LIB=%cd%\plplot5.13\lib
gdl.exe
