[![Build status](https://github.com/gnudatalanguage/gdl/workflows/build/badge.svg)](https://github.com/gnudatalanguage/gdl/actions)
[![Coverage Status](https://img.shields.io/codecov/c/github/gnudatalanguage/gdl/master.svg)](https://codecov.io/github/gnudatalanguage/gdl?branch=master)
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)
[![DOI](https://joss.theoj.org/papers/10.21105/joss.04633/status.svg)](https://doi.org/10.21105/joss.04633)

GDL - GNU Data Language
=======================

GDL is a free/libre/open source incremental compiler compatible with IDL (Interactive Data Language) and to some extent with PV-WAVE. 
Together with its library routines it serves as a tool for data analysis and visualization in such disciplines 
as astronomy, geosciences and medical imaging. 
GDL development had been started by **Marc Schellens** back in early noughties and has since continued 
with help of a team of maintainers, developers, packagers and thanks to feedback from users.

IDL is a registered trademark of [NV5 Geospatial Software, Inc.](https://www.nv5geospatialsoftware.com/).
PV-WAVE is a product of [Perforce](https://www.perforce.com/products/pv-wave).

Overview
--------

GDL is a domain-specific programming language and a data analysis environment.
As a language, it is dynamically-typed, array-oriented, vectorised and has 
object-oriented programming capabilities. 
GDL library routines handle numerical calculations, data visualisation, signal/image processing, 
interaction with host OS and data input/output. 
GDL supports several data formats such as netCDF, HDF4, HDF5, GRIB, PNG, TIFF, DICOM, etc. 
Graphical output is handled by X11, PostScript, SVG or z-buffer terminals, the last one allowing 
output graphics (plots) to be saved in a variety of raster graphics formats. 
GDL features integrated debugging facilities. 
The built-in widget functionality enables development of GUI-based software.
GDL has also a Python bridge (Python code can be called from GDL; GDL can be compiled as a Python module). 
Development and maintenance of GDL is carried out targeting Linux, BSD, OSX and Windows (MinGW, Cygwin).

GDL is invoked just by typing `gdl` but see `gdl -h` as it has a number of commandline options.
GDL may be known as `gnudl` or `gnudatalanguage` on some operating systems.

Other open-source numerical data analysis tools similar to GDL include
[SciPy](http://www.scipy.org/),
[GNU Octave](http://www.gnu.org/software/octave/),
[Scilab](http://www.scilab.org/),
[PDL](http://pdl.perl.org/),
[NCL](http://www.ncl.ucar.edu/),
[R](http://www.r-project.org/),
[Yorick](http://yorick.sourceforge.net/).

Getting GDL
-------------------------------------

See:
- [Cloning GDL](https://github.com/gnudatalanguage/gdl/wiki/Cloning-gnudatalanguage-gdl) (new!)
- [GDL on Linux](https://github.com/gnudatalanguage/gdl/wiki/GDL-on-Linux)
- [GDL on OSX](https://github.com/gnudatalanguage/gdl/wiki/GDL-on-OSX)
- [GDL on BSD](https://github.com/gnudatalanguage/gdl/wiki/GDL-on-BSD)
- [GDL on Windows](https://github.com/gnudatalanguage/gdl/wiki/GDL-on-Windows)

To download a tarball with GDL source code, including git submodules and auto-generated `version.hpp` file, please 
pick the `gdl-unstable-*.tar.gz` file from the weekly release at: https://github.com/gnudatalanguage/gdl/releases/tag/weekly-release

Find specific information on GDL
-------------------------------------
- Browse the [WIKI](https://github.com/gnudatalanguage/gdl/wiki)
- Be aware of current problems/limitations: Check the [issues](https://github.com/gnudatalanguage/gdl/wiki/Known-issues).

Dependencies 
-------------------------------------

Packaged versions of GDL are available for several Linux distributions, BSD and Mac OS X. 
Please note that several features of GDL depend on compile-time configuration, and might not 
be available in pre-built or pre-configured packages. 

GDL has numerous dependencies, most of the optional but highly recommended if you want it to be areally useful tool.
- [readline](https://tiswww.cwru.edu/php/chet/readline/rltop.html) mandatory. For easy command line editing, recalling, history. 
- [\[n\]curses](https://www.gnu.org/software/ncurses/) mandatory. Terminal management.
- [zlib](https://zlib.net/) mandatory. compressed file access.
- [GSL](https://www.gnu.org/software/gsl/) mandatory, for many math functions.
- [OpenMP](http://www.openmp.org/) optional, but speed will suffer if not present
- [Magick++](https://imagemagick.org/) / [GraphicsMagick](http://graphicsmagick.org/) optional, but don't you want to read/write many image formats?
- [wxWidgets](https://www.wxwidgets.org/) mandatory unless you do not want graphic outputs and widgets?
- [Xlib/X11](https://sourceforge.net/projects/libx11/) not used unless you explictly ask for it (replaced by wxWidgets for sake of compatibility on Windows, linux and MacOSX. 
- [netCDF](https://www.unidata.ucar.edu/software/netcdf/) optional, but useful for reading this kind of data.
- [HDF4](https://support.hdfgroup.org/products/hdf4/)  optional, but useful for reading this kind of data.
- [HDF5](https://support.hdfgroup.org/HDF5/)   optional, but useful for reading this kind of data.
- [FFTW](http://www.fftw.org/) optional, but don't you need a fast fft at times?
- [PROJ](http://proj.org/) optional but forget about mapping capabilities if absent.
- [Shapelib](http://shapelib.maptools.org/) optional but forget about mapping capabilities if absent.
- [Expat](https://libexpat.github.io/) optional but helps implement IDLffXMLSAX parser objects. 
- [MPI](https://en.wikipedia.org/wiki/Message_Passing_Interface) optional but provides clustering facilities.
- [Python](https://www.python.org/)/[NumPy](http://www.numpy.org/) optional but add python bridge and jupyter notebook.
- [udunits](https://www.unidata.ucar.edu/software/udunits/) optional, units conversion
- [Eigen](https://eigen.tuxfamily.org/) optional but provides inordinate speed enhancements...
- [ecCodes](https://confluence.ecmwf.int/display/ECC/ecCodes+Home) optional, for GRIB support.
- [GLPK](https://www.gnu.org/software/glpk/) optional, provides the SIMPLEX command.

Besides, for optimal use (speed mainly), GDL incorporates slightly edited code of
- [plplot](https://sourceforge.net/projects/plplot/) of which we keep only a bare minimum (and patched for bugs as the projects seems unmaintained now).
- [dSFMT](http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT) as our parallel random Generator.
- [delaunator](https://github.com/mapbox/delaunator) as our new hyperfast triangulation.
- [ANTLR3](https://www.antlr3.org/) as interpretor.
- [Median Filtering (S. Perreault)](http://nomis80.org/ctmf.html )
- [Median Filtering (J. Suomela)](http://users.ics.aalto.fi/suomela)
- [Radix Sorting](https://github.com/Pierre-Terdiman/RadixRedux) (we have written all variants up to doubles).
- [whereami](https://github.com/gpakosz/whereami) 

Build-time dependencies
-----------------------

Build and test automation is carried out using [CMake](http://cmake.org/).

GDL interpreter has been developed using [ANTLR v2](http://www.antlr2.org) but unless you want 
to change the grammar (\*.g files) you don't need ANTLR. 
All relevant ANTLR files are included in the source tree.

Support, feedback and contributions
-----------------------------------

Your comments are welcome! Let us know what you use GDL for. Or if you don't, why not. 
Which functionality are you missing/would appreciate most for coming versions. 
Please use the github issue-tracking system to report 
bugs, complaints, suggestions and comments.

Code enhancements in the form of pull requests are very welcome!
Note that contributions can be made in C++, IDL/GDL or Python, as well as
by providing enhancements and extensions of the README files, diagnostic messages, etc.

Among the major challenges GDL development is facing currently, there are:
- [enhancing test coverage](https://codecov.io/github/gnudatalanguage/gdl?branch=master) by writing test programs in GDL
- streamlining development and maintenance of GDL reference docs and examples (using the [Jupyter kernel](https://github.com/gnudatalanguage/idl_kernel)?)
- bringing in into the team the needed know-how to address the [backlog of ANTLR-related issues](https://github.com/gnudatalanguage/gdl/labels/antlr)
- increasing presence within and interoperability with the Python ecosystem, including adding support for Python 3 (calling GDL from Python 2 and calling Python 2 from GDL is already implemented!)

Help welcome!

Information resources
---------------------
GDL does not maintain a proper documentation: as GDL is aimed as a drop-in replacement for IDL,
resources for IDL constitute the valuable sources of information for GDL users as well. GDL MUST behave (at least) as IDL, and any discrepancy should be reported by opening an issue.
Conversely, the GDL issues and discussion forum on GitHub are not the good place for beginners to ask for advice on how to use IDL (or GDL). Use the forum below.
IDL freely available resources include:
- the [official IDL documentation](https://www.nv5geospatialsoftware.com/docs/using_idl_home.html)
- the [idl-pvwave Google Group](https://groups.google.com/forum/#!forum/idl-pvwave)
- the [comp.lang.idl-pvwave usenet group archives](http://www.idlcoyote.com/comp.lang.idl-pvwave/) (dating back to 1991!)
- Wikipedia article on [IDL](https://en.wikipedia.org/wiki/IDL_\(programming_language\))
- Wikipedia article on [PV-WAVE](https://en.wikipedia.org/wiki/PV-Wave)
- websites of IDL gurus including [David Fanning](http://www.idlcoyote.com/) and [Michael Galloy](http://michaelgalloy.com/)
- numerous [tutorials and lecture notes](https://www.google.com/search?q=interactive+data+language) introducing IDL
- old, used, but still very valid IDL booklets can be found in various libraries, second-hand bookstores etc.

There are several open source packages compatible or interoperable with GDL, including:
- the [MPFIT](https://pages.physics.wisc.edu/~craigm/idl/cmpfit.html) curve fitting library written in IDL (also available as a [Debian package](https://packages.debian.org/gdl-mpfit))
- the [IDL Astronomy User's Library](https://github.com/wlandsman/IDLAstro) written in IDL (also available as a [Debian package](https://packages.debian.org/gdl-idlastro))
- the [Coyote](https://www.idlcoyote.com) library of IDL-written utilities (also available as a [Debian package](https://packages.debian.org/gdl-coyote))
- the [TeXtoIDL](http://physics.mnstate.edu/craig/textoidl/) package 
- the [gdlde](https://github.com/gnudatalanguage/gdlde) IDE
- the [IDL/GDL Jupyter kernel](https://github.com/gnudatalanguage/idl_kernel)
- the [IDLWAVE Emacs mode](https://www.gnu.org/software/emacs/manual/html_mono/idlwave.html)
- IDL [syntax highlighting module for Vim](https://github.com/vim/vim/blob/master/runtime/syntax/idlang.vim)
- the [SingleCompile extension for Vim](https://github.com/vim-scripts/SingleCompile)

Alain Coulais maintains the [GDL-announces mailing list](https://sympa.obspm.fr/wws/info/gdl-announces).

There have been quite some [mentions of GDL in scientific literature](https://scholar.google.com/scholar?q="gnu+data+language") 
which also provide example use cases.
The Coulais et al. papers from the ADASS conferences are the best way to cite GDL as of now.

Acknowledgements
----------------

GDL development had been carried out at [SourceForge](http://sourceforge.net/) in years 2003-2018 - thank you!
