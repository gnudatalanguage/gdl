[![Linux/OSX Build Status](https://travis-ci.org/gnudatalanguage/gdl.svg?branch=master)](https://travis-ci.org/gnudatalanguage/gdl) 
[![Windows Build status](http://ci.appveyor.com/api/projects/status/github/gnudatalanguage/gdl?branch=master&svg=true)](https://ci.appveyor.com/project/gnudatalanguage/gdl/branch/master)
[![Coverage Status](https://img.shields.io/codecov/c/github/gnudatalanguage/gdl/master.svg)](https://codecov.io/github/gnudatalanguage/gdl?branch=master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/e5de1c1571c649279dad18d5d8590789)](https://www.codacy.com/app/slayoo/gdl?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=gnudatalanguage/gdl&amp;utm_campaign=Badge_Grade)
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

GDL - GNU Data Language
=======================

GDL is a free/libre/open source incremental compiler compatible with IDL and to some extent with PV-WAVE. 
Together with its library routines it serves as a tool for data analysis and visualization in such disciplines 
  as astronomy, geosciences and medical imaging. 
GDL development had been started by **Marc Schellens** back in early noughties and has since continued 
  with help of a team of maintainers, developers, packagers and thanks to feedback from users.

IDL is a registered trademark of [Harris Geospatial Solutions](https://www.harrisgeospatial.com)
PV-WAVE is a product of [Rogue Wave Software](https://www.roguewave.com)

Overview
--------

GDL as a language is dynamically-typed, vectorized and has object-oriented programming capabilities. 
GDL library routines handle numerical calculations, data visualisation, signal/image processing, 
  interaction with host OS and data input/output. 
GDL supports several data formats such as netCDF, HDF4, HDF5, GRIB, PNG, TIFF, DICOM, etc. 
Graphical output is handled by X11, PostScript, SVG or z-buffer terminals, the last one allowing 
  output graphics (plots) to be saved in a variety of raster graphics formats. 
GDL features integrated debugging facilities. 
Build-in widget functionality enables development of GUI-based software.
GDL has also a Python bridge (Python code can be called from GDL; GDL can be compiled as a Python module). 

Other open-source numerical data analysis tools similar to GDL include
[SciPy](http://www.scipy.org/),
[GNU Octave](http://www.gnu.org/software/octave/),
[Scilab](http://www.scilab.org/),
[PDL](http://pdl.perl.org/),
[NCL](http://www.ncl.ucar.edu/),
[R](http://www.r-project.org/),
[Yorick](http://yorick.sourceforge.net/), ...

Package availability
--------------------

Packaged versions of GDL are available for several Linux distributions, BSD and Mac OS X. 
The source code compiles on Windows (mingw), as well on other UNIX systems, including Solaris. 

Please note that several features of GDL depend on compile-time configuration, and might not 
  be available in pre-built or pre-configured packages. 
The table below summarises GDL mandatory and optional dependencies along with the availability 
  of optional functionalities in packages (as of April 2018):

| | [Fedora](https://src.fedoraproject.org/rpms/gdl) | [Debian](https://packages.debian.org/sid/gnudatalanguage) | [Ubuntu](http://packages.ubuntu.com/gnudatalanguage) | [Gentoo](https://packages.gentoo.org/packages/dev-lang/gdl) | [Arch](https://aur.archlinux.org/packages/gnudatalanguage/)  | [FreeBSD](https://svnweb.freebsd.org/ports/head/science/gnudatalanguage/) | [Homebrew](http://braumeister.org/repos/Homebrew/homebrew-science/formula/gnudatalanguage) | [Macports](https://github.com/macports/macports-ports/tree/master/math/gnudatalanguage) |
| --------------------------------------------------------------------------------------- | :----: | :--------: | :----: | :----: | :---: | :-------: | :------: | :------: |
| lastest release                                                                         | 0.9.7  | **0.9.8**  | 0.9.7  | 0.9.6   | 0.9.7 | **0.9.8** | 0.9.7 | 0.9.7 |
| [readline](http://thrysoee.dk/editline/)/[editline](http://thrysoee.dk/editline/)       | + (r)  |    + (r)   |  + (r) | + (r)   | + (r) |   + (r)   | + (r) |   +   |
| [\[n\]curses](https://www.gnu.org/software/ncurses/)                                    | + (n)  |    + (n)   |  + (n) | + (n)   |   +   |   + (n)   | + (n) | + (n) |
| [zlib](https://zlib.net/)                                                               |   +    |    +       |   +    | +       |   +   |    +      |   +   |   +   |
| [GSL](https://www.gnu.org/software/gsl/)                                                |   +    |    +       |   +    | +       |   +   |    +      |   +   |   +   |
| [plplot](https://plplot.sourceforge.net)                                                |   +    |    +       |   +    | +       |   +   |    +      |   +   |   +   |
| [OpenMP](http://www.openmp.org/)                                                        |   +    |    +       |   +    | +       |   +   |    +      |   -   |   +   |
| \[[Image](https://imagemagick.org/)/[Graphics](http://graphicsmagick.org/)\]Magick      | + (G)  |  + (G)     | + (G)  | + (G/I) |   -   |  + (G)    | + (G) | + (G) |
| [wxWidgets](https://www.wxwidgets.org/)                                                 |   +    |    +       |   +    | +       |   +   |    +      |   -   |   -   |
| [netCDF](https://www.unidata.ucar.edu/software/netcdf/)                                 |   +    |    +       |   +    | +       |   +   |    +      |   +   |   +   |
| [HDF4](https://support.hdfgroup.org/products/hdf4/)                                     |   +    |    +       |   +    | +       |   -   |    -      |   -   |   +   |
| [HDF5](https://support.hdfgroup.org/HDF5/)                                              |   +    |    +       |   +    | +       |   -   |    +      |   +   |   +   |
| [FFTW](http://www.fftw.org/)                                                            |   +    |    +       |   +    | +       |   +   |    +      |   +   |   +   |
| [PROJ.4](http://proj4.org/)                                                             |   -    |    -       |   -    | +       |   -   |    +      |   -   |   +   |
| [GSHHG](http://www.soest.hawaii.edu/wessel/gshhg/)                                      |   -    |    -       |   -    | +       |   -   |    -      |   -   |   +   |
| [MPICH](https://www.mpich.org/)                                                         |   -    |    -       |   -    | -       |   -   |    +      |   -   |   +   |
| [Python](https://www.python.org/)/[NumPy](http://www.numpy.org/)                        |   +    |    +       |   +    | +       |   +   |    +      |   -   |   +   |
| [udunits](https://www.unidata.ucar.edu/software/udunits/)                               |   +    |    +       |   +    | +       |   +   |    +      |   +   |   +   |
| [Eigen](https://eigen.tuxfamily.org/)                                                   |   +    |    +       |   +    | +       |   +   |    +      |   +   |   +   |
| [pslib](http://pslib.sourceforge.net/)                                                  |   +    |    +       |   +    | +       |   +   |    +      |   -   |   +   |
| [GRIB](https://software.ecmwf.int/wiki/display/GRIB)                                    |   +    |    +       |   +    | +       |   +   |    +      |   -   |   +   |
| [QHULL](http://www.qhull.org/)                                                          |   +    |    +       |   +    | -       |   -   |    +      |   -   |   -   |
| [GLPK](https://www.gnu.org/software/glpk/)                                              |   -    |    -       |   -    | -       |   -   |    +      |   -   |   -   |
| [Python](https://www.python.org/) module                                                |   +    |    +       |   +    | +       |   -   |    -      |   -   |   -   |


