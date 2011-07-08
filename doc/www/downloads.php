<?php require('_header.inc.php'); ?>

<h2><a href="index.html">Obtaining GDL</a></h2>
			
<p>
  GDL compiles "out of the box" on Linux, Mac OS X and several other UNIX systems
  (see <a href="requirements.php">requirements</a> for details).
</p>

<p>
  The source code distribution is available from
  <a href="http://sourceforge.net/projects/gnudatalanguage/">SourceForge</a>.
</p>

<p>
  The latest development version of GDL can be obtained via CVS - consult the
  <a href="http://sourceforge.net/projects/gnudatalanguage/develop/">SF.net project development page</a>
  for details. It is also possible to 
  <a href="http://gnudatalanguage.cvs.sourceforge.net/viewvc/gnudatalanguage/gdl/">
    browse the CVS repository using a web-based interface</a>.
</p>

<p>
  There are numerous packaged versions of GDL available for various OSes:
</p>
  <ul>
    <li>Linux
      <ul>
        <li><a href="http://koji.fedoraproject.org/koji/packageinfo?packageID=1830">Fedora</a></li>
        <li><a href="http://packages.debian.org/search?keywords=gnudatalanguage">Debian GNU/Linux</a></li>
        <li><a href="http://gentoo-portage.com/dev-lang/gdl">Gentoo</a></li>
        <li><a href="http://aur.archlinux.org/packages.php?ID=2493">Arch Linux</a></li>
        <li><a href="http://packages.ubuntu.com/gnudatalanguage">Ubuntu</a></li>
      </ul>
    </li>
    <li>Mac OS X
      <ul>
        <li><a href="<?php echo(htmlspecialchars('http://www.macports.org/ports.php?by=name&substr=gnudatalanguage'));?>">Macports</a></li>
<!--        <li><a href="http://hpc.sourceforge.net/">HPC page</a></li> -->
        <li><a href="http://www.hmug.org/pub/MacOS_X/X/Applications/Science/gdl/">HMUG</a></li>
        <li><a href="http://pdb.finkproject.org/pdb/package.php/gdl">Fink</a></li>
      </ul>
    </li>
    <li>BSD
      <ul>
        <li><a href="http://www.freebsd.org/cgi/cvsweb.cgi/ports/science/gnudatalanguage/">FreeBSD</a></li>
        <li><a href="http://packages.debian.org/search?keywords=gnudatalanguage">Debian GNU/kFreeBSD</a></li>
      </ul>
    </li>
  </ul>

<p>
  Please note that several features of GDL depend on compile-time configuration, 
    and might not be available in pre-built or pre-configured packages.
</p>

<?php require('_footer.inc.php'); ?>
