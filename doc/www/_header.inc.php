<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <title>GDL - GNU Data Language</title>
  <meta http-equiv="content-type" content="application/xhtml+xml; charset=UTF-8" />
<?php /*
<meta name="author" content="Erwin Aligam - styleshout.com" />
<meta name="description" content="Site Description Here" />
<meta name="keywords" content="keywords, here" />
<meta name="robots" content="index, follow, noarchive" />
<meta name="googlebot" content="noarchive" />
*/ ?>
  <link rel="stylesheet" href="images/Colourise.css" type="text/css" />
</head>

<body>

<!-- wrap starts here -->
<div id="wrap">

	<!--header -->
	<div id="header">			
				
		<h1 id="logo-text"><a>GDL - GNU Data Language</a></h1>		
		<p id="intro">
                  A free and open-source IDL<sup>&reg;*</sup>/PV-WAVE<sup>**</sup> compiler<br />
                  <small>
                    * IDL is a registered trademark of <a href="http://exelisvis.com/">Exelis Visual Information Solutions</a><br />
                    ** PV-WAVE is a product of <a href="http://roguewave.com/">Rogue Wave Software</a>
                  </small><br />
		</p>	
		
		<div  id="nav">
			<ul>
<?php 
  $menu = array(
    'index.php' => 'Features',
    'screenshots.php' => 'Screenshots',
    'downloads.php' => 'Downloads',
    'requirements.php' => 'Requirements',
    'documentation.php' => 'Documentation',
    'support.php' => 'Support &amp; Feedback',
    'resources.php' => 'Resources',
    'faq.php' => 'FAQ',
    'credits.php' => 'Credits'
  );
  foreach ($menu as $file => $name) 
  {
    echo('<li');
    if (basename($_SERVER['SCRIPT_FILENAME']) == $file) echo(' id="current"');
    echo('><a href="' . $file . '">' . $name . '</a></li>');
  }
?>
			</ul>		
		</div>	
		
<!--
		<form id="quick-search" action="index.html" method="get" >
			<p>
			<label for="qsearch">Search:</label>
			<input class="tbox" id="qsearch" type="text" name="qsearch" value="Search..." title="Start typing and hit ENTER" />
			<input class="btn" type="submit" value="Submit" />
			</p>
		</form>			
-->
				
	<!--header ends-->					
	</div>
	
	<!-- content-wrap starts -->
	<div id="content-wrap">
		<div id="main">
			<a name="TemplateInfo"></a>
