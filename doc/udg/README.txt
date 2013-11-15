
Sylwester Arabas and Alain Coulais
24 Jan 2012

How to compile the documentation ?

0/ You must have compiled GDL before

1/ You must collect some external files before 
   (or better obtain them using your LaTeX or OS package manager):

-- forloop.sty
http://www.ctan.org/tex-archive/macros/latex/contrib/forloop

-- stringstrings.sty
http://www.ctan.org/tex-archive/macros/latex/contrib/stringstrings

-- pdfdraftcopy.sty  
see http://sarovar.org/projects/pdfdraftcopy/
(wget http://sarovar.org/projects/pdfdraftcopy/pdfdraftcopy.sty)

-- nnfootnote.sty   
see http://ctan.org/pkg/nnfootnote/
(wget http://ctan.org/pkg/nnfootnote/nnfootnote.sty)

-- Perl Script authorindex
see http://mirrors.ctan.org/indexing/authorindex/
(wget http://mirrors.ctan.org/indexing/authorindex/authorindex)

Take care that you must put this self-executable script in a directory see by your PATH
(maybe the current directory is not allowed to contain executables)

The LaTeX files should go to some place like ~/texmf

2/ Then you can start to compile the doc.
You have to do in the current directory: ./makeall
(and wait for a while)

Please report any broken link(s)
