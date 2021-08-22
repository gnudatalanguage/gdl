---
title: 'GNU Data Language 1.0: an IDL-compatible open-source free incremental compiler'
tags:
  - GNU Data Language
  - Interactive Data Language
  - GDL
  - IDL
authors:
  - name: Jeongbin Park
    orcid: 0000-0002-9064-4912
    affiliation: "1"
  - name: Gilles Duvert
    orcid: 0000-0001-8769-3660 
    affiliation: "2"
  - name: Alain Coulais
    orcid: 0000-0001-6492-7719
    affiliation: "3"
  - name: Gregory V. Jung
    affiliation: "4"
  - name: Sylwester Arabas
    orcid: 0000-0003-2361-0082
    affiliation: "5"
affiliations:
 - name: Division of Computational Genomics and Systems Genetics, German Cancer Research Center(DKFZ), Heidelberg, Germany
   index: 1
 - name: TODO
   index: 2
 - name: TODO
   index: 2
 - name: TODO
   index: 3
 - name: TODO
   index: 4
 - name: Jagiellonian University, Kraków, Poland
   index: 5
bibliography: paper.bib

---

# Summary

We present GNU Data Language (GDL), an open-source free incremental compiler for the scripts written in Interactive Data
Language (IDL), a computer language widely used for scientific data analysis, especially in the field of Astronomy,
Geoscience, and Biology. GDL 1.0 is fully compatible with IDL 7.1 language specification, including partial supports
of the IDL 8.0 specification and above. GDL also partially supports PV-WAVE, another data analysis language that shares some
similarities to IDL. GDL has been developed to perfectly mimic the behavior of the official IDL compiler distributed by
Harris Geospatial Solutions, so that the existing IDL scripts to be compatible with GDL without any modifications. GDL
also comes with an integrated development environment (IDE), GDL Workbench, based on Eclipse Rich Client Platform (RCP),
to aid users to simply edit IDL scripts and run them with GDL.

*Disclaimer: IDL is a registered trademark of Harris Geospatial Solutions. PV-WAVE is a product of Perforce.*

# Statement of Need

Interactive Data Language (IDL) is an interactive, array-oriented, and vectorized computer language widely used in the
field of astronomy, geoscience, and biology[needs **many** citations]. [here needs more explanation of the language itself.
e.g. how easy it is, or similarity to other programming languages, etc.].

GNU Data Langauge (GDL) is an open-source free incremental compiler for IDL. The development of GDL is aiming to achieve
full compatibility with the official IDL compiler so that the IDL scripts can be compiled and run by GDL without
modifications. GDL supports full IDL 7.1 language specifications, also partially supports IDL 8.0 and above. Not only
IDL, GDL is also capable to run some [please be more specific here - instead of saying 'some'] PV-Wave, another data
analysis language similar to IDL [better if we can include more explanation of PV-Wave here]. [status of Widgets, etc].
Also, GDL supports Python: 1) GDL can directly run Python codes with GDL, and 2) GDL can be compiled as a Python
module.

GDL is fast, [how fast? do we have recent benchmarks?].

[Any highlights? I don't have any highlights from the biology side...]

GDL can be installed via the package managing system on Linux (for example, `apt` on Debian-based systems), also via
Homebrew, Macports, or Fink on macOS. On Windows, we provide a precompiled Windows binary, as well as a Nullsoft Scriptable
Install System (NSIS) based installer. For develop environments, we provide GDL Workbench, an Eclipse Rich Client
Platform (RCP) based integrated development environment (IDE), and GDL Jupyter kernel.

# Acknowledgements and author contributions

GDL development had been started in early aughties by Marc Schellens who developed the interpreter core.

JP has served as release manager for GDL 1.0 and had major contributions to Windows OS support and the `gdlde` IDE for GDL. 
GD and AC have been the key maintainers and developers of GDL over the last decade.
GD has contributed a major rework of widgets and plotting subsystems to the 1.0 release.
GJ has ...
SA has ...
The paper was composed by ...

Development of GDL had been hosted at Sourceforge in years 2003-2018 and has since moved to Github.

# References
