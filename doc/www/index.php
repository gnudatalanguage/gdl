<?php require('_header.inc.php'); ?>
			
<h2>Introduction</h2>

<p>
GNU Data Language (GDL) is a free/libre/open source incremental compiler
  compatible with IDL and to some extent with PV-WAVE.
Together with its library routines it serves as a tool for data analysis 
  and visualization in such disciplines as astronomy, geosciences and 
  medical imaging. 
GDL is free software licensed under the <a href="http://www.gnu.org/licenses/gpl.html">GPL</a>.
GDL is developed by an international <a href="credits.php">team</a> of volunteers led by Marc Schellens - the project's founder
</p>

<p>
GDL as a language is dynamically-typed, vectorized and has 
  object-oriented programming capabilities. 
GDL library routines handle numerical calculations, data visualisation, 
  signal/image processing, interaction with host OS and data input/output. 
GDL supports several data formats such as netCDF, HDF4, HDF5, GRIB, PNG, TIFF, 
  DICOM, etc. 
Graphical output is handled by X11, PostScript, SVG or z-buffer terminals, 
  the last one allowing output graphics (plots) to be saved in a variety of  
  raster graphics formats.
GDL features integrated debugging facilities. 
GDL has also a Python bridge (Python code can be called from GDL; GDL can be compiled 
  as a Python module). 
</p>

<p>
Packaged versions of GDL are available for several Linux and BSD flavours as well as Mac&nbsp;OS&nbsp;X. 
The source code compiles as well on other UNIX systems, including Solaris.
</p>

<p>
Other open-source numerical data analysis tools similar to GDL include
<a href="http://www.gnu.org/software/octave/">GNU&nbsp;Octave</a>,
<a href="http://www.ncl.ucar.edu/">NCL&nbsp;-&nbsp;NCAR&nbsp;Command&nbsp;Language</a>,
<a href="http://pdl.perl.org/">PDL&nbsp;-&nbsp;Perl&nbsp;Data&nbsp;Language</a>,
<a href="http://www.r-project.org/">R</a>,
<a href="http://www.scilab.org/">Scilab</a>,
<a href="http://www.scipy.org/">SciPy</a>,
<a href="http://yorick.sourceforge.net/">Yorick</a> ...
</p>

<h2>Feature summary</h2>

<p>Full syntax compatibility with IDL up to version 7.1 (for &gt;8.0 see below):</p>

<ul>
    <li>objects, pointers, structs and arrays,</li>
    <li>system, common block and assoc variables,</li>
    <li>all operators and datatypes,</li>
    <li>_EXTRA, _STRICT_EXTRA and _REF_EXTRA keywords...</li>
</ul>

<p>Supported IDL 8.0 language elements:</p>

<ul>
    <li>FOREACH loop</li>
    <li>negative array indices</li>
    <li>garbage collection pointers and objects</li>
    <li>call methods on an object using "." (e. g. object.aMemberProcedure,arg1)</li>
</ul>

<p>The file input output system is fully implemented<br/>
(Exception: For formatted I/O the C() sub-codes are not supported yet)</p>

<p>Supported file formats include:</p>
<ul>
  <li>netCDF 3 (read/write)</li>
  <li>HDF4 (read/write)</li>
  <li>HDF5 (read-only, limited support)</li>
  <li>IDL SAVE files (supported using the Craig Markwardt's <a href="http://cow.physics.wisc.edu/~craigm/idl/down/cmsvlib.tar.gz">CMSVLIB</a>)</li>
  <li>FITS files, when Astro Lib. is in the path (<a href="http://www.cv.nrao.edu/fits/data/tests/pg93/NRAO pg93"> test suite</a> OK)</li>
  <li>various graphic formats (e.g. PNG if compiled with ImageMagick)</li>
  <li>...</li>
</ul>

<p>
Graphical output is partially implemented. The PLOT, OPLOT, PLOTS,
 XYOUTS, CONTOUR, SURFACE, TVRD and TV commands
(along with WINDOW, WDELETE, SET_PLOT, WSET, TVLCT) work 
(important keywords, some !P
system variable tags and multi-plots are supported) 
for X windows, z-buffer and postscript output.
</p>

<p>GUI support (widgets) is officially provided since the 0.9 version (but it's not complete yet).</p>
 
<?php require('_footer.inc.php'); ?>
