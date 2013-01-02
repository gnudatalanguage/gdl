<?php require('_header.inc.php'); ?>

<h2><a href="index.html">Frequently Asked Questions</a></h2>

<h3>Installation and portability issues</h3>
<ul>
<li>
<a>Is it possible to run GDL on MS-Windows?</a>
<br>Yes.
<br>Solution 1: It's possible to compile GDL under Cygwin or e.g. to run GDL under the coLinux platform.
<br>Solution 2: since GDL 0.9.3, GDL should be compilable under MS-windows thanks to <a>Jeong Bin Park</a>
Please follow the <a href="http://cyfinity.egloos.com/1727847">instructions described here</a>.
Feedback realy welcome. Please notice SPAWN functionnalities currently not working, which explain large
number of FAIL in testsuite.
<br>N.B.: Due to lack of Windows developer- or user-base there is virtually no testing done on Windows, though. Help welcome!
    </li>
  </ul>

<h3>Compatibility with IDL</h3>
  <ul>
    <li>
      <a>Is there an equivalent of IDL Virtual Machine in GDL?</a>
      <br />
      No. 
      GDL is free/libre/open-source software. 
      The distinction between a developers' (costly) version and a users' (free) version is not applicable.
      Any user is free to use the full power of GDL.
    </li>
    <li>
      <a>Is there a way to save/distribute pre-compiled GDL routines (as&nbsp;.sav&nbsp;files in IDL)?</a>
      <br />
      No. GDL does not support saving/loading pre-compiled routines yet.
      It does support saving/loading variables into IDL-compatible .sav files, though.
      Since compatibility with IDL-saved routines is not going to be possible anyhow, 
      and since the virtual-machine context is not applicable to GDL (see above),
      implementing routine saving has a low priority.
    </li>
    <li>
      <a>Does GDL support the so-called ''object graphics'', ''function graphics''/''new graphics''?</a>
      <br />
      No. GDL supports ''direct graphics'' only.
    </li>
    <li>
      <a>Why does the CATCH statement not work properly in GDL?</a>
      <br />
      It's one of very few IDL syntax elements not implemented in GDL yet.
    </li>
  </ul>

<h3>Mapping</h3>
  <ul>
    <li>
      <a>Why the continent/country/shoreline boundaries have so poor resolution in comparison with IDL</a><br />
      Try the HIRES keyword to MAP_CONTINENTS!
      </li>
  </ul>

<?php require('_footer.inc.php'); ?>
