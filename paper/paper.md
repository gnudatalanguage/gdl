---
title: 'GNU Data Language 1.0: a free/libre and open-source drop-in replacement for IDL/PV-WAVE'
tags:
  - GNU Data Language
  - Interactive Data Language
  - GDL
  - IDL
  - PV-WAVE
authors:
  - name: Jeongbin Park
    orcid: 0000-0002-9064-4912
    affiliation: 1
  - name: Gilles Duvert
    orcid: 0000-0001-8769-3660 
    affiliation: 2
  - name: Alain Coulais
    orcid: 0000-0001-6492-7719
    affiliation: "3, 4"
  - name: Gregory V. Jung
    affiliation: 5
  - name: Sylwester Arabas
    orcid: 0000-0003-2361-0082
    affiliation: "6, 7"
  - name: Brian Barker
    affiliation: 8
  - name: Takeshi Enomoto
    affiliation: 9
  - name: Sylvain Flinois
    affiliation: 10
  - name: Oliver Gressel
    affiliation: 11
  - name: Tomas Hillberg
    affiliation: 12
  - name: Thibault Huillet
    affiliation: 13
  - name: Jan Kohnert
    affiliation: 14
  - name: Orion Poplawski
    affiliation: 15
  - name: Eloi Rozier de Linage
    affiliation: 3
  - name: Remi A. Solås
    affiliation: 16
  - name: Luke Stagner 
    orcid: 0000-0001-5516-3729
    affiliation: 17
  - name: Ole Streicher
    orcid: 0000-0001-7751-1843
    affiliation: 11
  - name: James Tappin
    affiliation: 18
  - name: Thierry Thomas
    affiliation: 19
  - name: Jingwei Wang
    affiliation: 20
  - name: Christian Wimmer
    affiliation: 14

affiliations:
 - name: School of Biomedical Convergence Engineering, Pusan National University, Republic of Korea
   index: 1
 - name: Univ. Grenoble Alpes, CNRS, IPAG, 38000 Grenoble, France 
   index: 2
 - name: LERMA, Observatoire de Paris, CNRS, Paris, France 
   index: 3
 - name: AIM, CEA, CNRS, Université Paris-Saclay, Université Paris Diderot, Sorbonne Paris Cité, Gif-sur-Yvette, France
   index: 4
 - name: Independent Researcher, USA
   index: 5
 - name: Jagiellonian University, Kraków, Poland
   index: 6
 - name: University of Illinois at Urbana–Champaign, USA
   index: 7
 - name: Ursinus College, PA, USA
   index: 8
 - name: Kyoto University, Japan
   index: 9
 - name: Kumullus, Paris, France
   index: 10
 - name: Leibniz-Institut für Astrophysik Potsdam (AIP), Potsdam, Germany 
   index: 11
 - name: Department of Astronomy, Stockholm University, Sweden
   index: 12
 - name: Independent Researcher, France
   index: 13
 - name: Independent Researcher, Germany
   index: 14
 - name: NorthWest Research Associates, Boulder, CO, USA
   index: 15
 - name: Independent Researcher, Norway
   index: 16
 - name: General Atomics, San Diego, CA, USA
   index: 17
 - name: RAL Space, STFC Rutherford Appleton Laboratory, UK 
   index: 18
 - name: EDF, Lyon, France
   index: 19
 - name: MINES ParisTech, Paris, France
   index: 20
bibliography: paper.bib

---

# Summary

We present GNU Data Language (GDL), an open-source free incremental compiler for programs written in Interactive Data
  Language (IDL<sup>*</sup>) and Precision Visuals - Workstation Analysis and Visualization Environment (PV-WAVE<sup>**</sup>),
  two computer languages used for scientific data analysis. GDL is highly compatible with the IDL and PV-WAVE and aims to run
  any existing IDL codes without any modifications. GDL comes with its dedicated IDE GDL Workbench and Jupyter kernel `gdl_kernel` 
  to provide a comfortable development environment. In addition, GDL supports interoperability with Python. GDL is freely
  available at https://gnudatalanguage.github.io/.

\* IDL is a registered trademark of [L3HARRIS](http://l3harrisgeospatial.com). \
** PV-WAVE is a product of [Perforce](http://perforce.com).

# Statement of Need

Interactive Data Language (IDL) is a commercial, domain-specific language used for processing data in various scientific
  fields, including astronomy, geosciences, biology, hyperspectral, medical imaging (@burrage2021cardiac, @coates2019high,
  @xiao2022superheating, @lee2022transboundary, @castro2021very), and even COVID-19 research (@chen2021orf3a). Although
  IDL is not so widely used these days for scientific computing as the rise of modern computing languages, many legacy IDL
  codes are still being used these days. Undoubtedly, porting such legacy codes to modern languages would require a huge
  effort, it is much more economic to run them with IDL. To run such IDL codes without any financial constraints, there
  have been several efforts to create a freeware clone of IDL, including Fawlty Language (<https://www.flxpert.hu/fl/>).
  Fawlty Language is a very successful implementation as it supports nearly 100% of the latest IDL 8.0 syntax. However,
  Faulty Language is neither open-source nor free as in free speach and thus users cannot report or fix problems in its 
  source code. Neither IDL nor FL fulfill modern requirements for freely reproducible research. 

The GDL project is an international effort to create a free software clone of Interactive Data Language (IDL) or Precision
  Visuals - Workstation Analysis and Visualization Environment (PV-WAVE), preserving the capability to run the vast body of
  scientific legacy codes without any technical, legal or financial constraints. GDL has been developed for over four decades
  with public funding throughout academic institutions around the world. The initial development was done by **Marc Schellens**
  almost 20 years ago (commit history preserved on GitHub dates back to 2004) and since then a continuous development has
  been carried out by an evolving team of volunteer contributors -- both freelance and affiliated with academic institutions
  (@Coulais_et_al_2010, @Coulais_et_al_2012, @Coulais_et_al_2014, @Coulais_et_al_2019, @Duvert_et_al_2020). As a result,
  we have recently announced GDL 1.0.

In terms of compatibility, GDL aims at a full compatibility with modern IDL language specification, including partial
  support of the IDL 8 specification and above. In addition, GDL is partially compatible with PV-WAVE, another data
  analysis framework forked from IDL and sharing parts of IDL syntax and library interface. GDL aims to closely mimic
  the behaviour of the IDL compiler and libraries distributed by Harris Geospatial Solutions, so that the existing IDL
  scripts to be compatible with GDL without any modifications. The development has been done by relying on IDL and PV-WAVE
  documentation which has been publicly available on the IDL and PV-WAVE proprietors' websites, IDL documentation which
  had been publicly available on NASA websites (currently available at the Internet Archive, see
  <https://web.archive.org/web/20090423093625/http://idlastro.gsfc.nasa.gov/idl_html_help/>), and several published books which
  describes IDL syntax and library routines (@Fanning_2003, @Bowman_2005, @Gumley_2010, @Galloy_2015).

For developer convenience, GDL comes with its own integrated development environment (IDE), GDL Workbench, based on Eclipse
  Rich Client Platform (RCP), to aid users to simply edit IDL scripts and run them with GDL. In addition, there is an
  ongoing effort to maintain a GDL Jupyter kernel `gdl_kernel` providing GDL data analysis and plotting functionalities via
  familiar interactive notebook interface.
  
GDL is interoperable with Python, by featuring a bi-directional Python bridge offering access to IDL/GDL code from
  Python and vice versa.

GDL, GDL Workbench and `gdl_kernel` are free/libre and open-source software publicly available under the terms of the GNU General
  Public License v2 or above.

# Highlights in GDL 1.0

From GDL 1.0, the plotting part has been completely revamped based on wxWidgets by default for all platforms, which provides widget support and also guarantees the same behavior across different operating systems. GDL 1.0 also provides improved file format support, including reading/writing geoTIFF and writing HDF files. From the release of 1.0, the GDL project is automatically compiled and released via Github Actions, providing source code distribution and a convenient installer for Windows systems. In addition to new features, many performances and threading optimizations were also done. Map projections support in GDL is also much more mature compared to earlier major releases of GDL. Finally, the transition from SourceForge to GitHub that preceded the release of GDL 1.0 helped to consolidate the community efforts, streamline development, and support workflows.

# Design and Dependencies

GDL interpreter is written in C++ using the ANTLR framework.
The library routines are written either in C++ or in GDL itself.
Build and test automation is handled with CMake.
Continuous integration is set up with GitHub Actions.

GDL command-line interface is built on top of readline and ncurses.
Basic array-handling, numerical processing, and I/O functionalities are implemented using Eigen,
  GSL, FFTW, OpenMP and zlib.
Graphical output (screen and files) and widget handling is realised using 
  plplot and wxWidgets.
Support for various file formats is implemented using netCDF, HDF4,
  HDF5, Shapelib, Magick++ or GraphicsMagick and Expat.
Map projections are handled using PROJ.
An evolving list of all project dependencies is maintained at the project website
  and can also be extracted from the CMake configuration files.

# Availability

GDL can be installed via the package managing system on Linux (Arch, Debian, Fedora, Gentoo, Mageia, Ubuntu), 
  FreeBSD and macOS (Homebrew and Macports).
On Windows, we provide a precompiled Windows binary, wrapped as a convenient installer based on the Nullsoft Scriptable
  Install System (NSIS).
The source code of GDL, GDL Workbench, and `gdl_kernel` is available at our GitHub repository (<https://github.com/gnudatalanguage>).
The weekly unstable builds can be obtained from <https://github.com/gnudatalanguage/gdl/releases>.

# Support and Contributions

GDL resources are being catalogued at the GDL website (<https://gnudatalanguage.github.io/>).
  The preferred and effective way to report requests for support, missing features or bugs present
  is through the GitHub issue tracker (<https://github.com/gnudatalanguage/gdl/issues>).

To streamline review and incorporation of code contributions, pull requests on GitHub are preferred.
All contributed code must comply with the GNU General Public License.

# Author Contributions

JP has served as release manager for GDL 1.0 and has been the key contributor to Windows OS support,
the GDL Workbench IDE for GDL and the continuous integration setup.
GD has contributed a major rework of widgets and plotting subsystems to the 1.0 release.
GD and AC have been the key maintainers and developers of GDL over the last decade.
GVJ has contributed support for Windows OS, newer IDL datatypes, and library routines.
LS started the `gdl_kernel` project.
BB, SF, OG, THi, THu, JK, ERdL, JT, JW, and RAS contributed library routines and tests.
OS, TT, OP, and TE maintain GDL packages.
SA had been an active contributor in years 2009-2015 and has since kept contributing to project maintenance.
JP and SA wrote the paper draft, and all the other authors have reviewed and confirmed the text.

# Acknowledgements
GDL development benefited from code contributions, bug reports, and feedback from numerous
other developers and users.

GDL development had been hosted at Sourceforge over 2003-2018 and has moved to Github.

# References
