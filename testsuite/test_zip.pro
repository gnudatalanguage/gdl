;
; Testing the /compress keyword !
;
; Maxime Lenoir, June 2010
; under GNU GPL 2 or later
;
; AC May 2012:
; -- clarification (options, Upcases, identations)
; -- on Linux, "stat --printf=%s " does not work on few systems ...
;
; Some commands called using SPAWN may not work. Please submit
; an alternative via GDL Bug Traker on SourceForge.
; You can see the cammands used by running /verbose
;
; by default, we exit at first pb, maybe to be less strict later
; --but no problem today with /compress files ...--
; 
; GD, Nov 2013: test that seek works 
;
pro CATCH_EXIT, nb_problems, no_exit=no_exit
;
if KEYWORD_SET(no_exit) then begin
   nb_problems=nb_problems+1
endif else begin
   EXIT, status=1
endelse
end
;
; ---------------------------------
;
pro TEST_ZIP, no_exit=no_exit, help=help, test=test, verbose=verbose, quiet=quiet
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_ZIP, no_exit=no_exit, help=help, test=test, verbose=verbose, quiet=quiet'
   return
end
;
nb_pbs=0
;
file='test_zip.gz'
;
txt='"GDL - GNU Data Language\nA free IDL (Interactive Data Language) compatible incremental\n'
txt=txt+'compiler (i. e. runs IDL programs)."'
;
;SPAWN, 'echo -e '+txt+' | gzip > '+file
SPAWN, 'printf '+txt+' | gzip > '+file
OPENR, fd, file, /get, /compress, /delete
;
; Size test (FSTAT)
if STRLOWCASE(!version.os_name) eq 'linux' then begin
   commande='stat --printf=%s '+file
   if KEYWORD_SET(verbose) then print, 'commande: ', commande
   SPAWN, commande, size, error_mess, exit_status=exit_status
   if (exit_status GT 0) then begin
      commande='stat -c %s '+file
      if KEYWORD_SET(verbose) then print, 'commande: ', commande
      SPAWN, 'stat -c %s '+file, size, error_mess, exit_status=exit_status
   endif
endif else begin
   ;; this is OK on OSX
   commande='stat -f%z '+file
   if KEYWORD_SET(verbose) then print, 'commande: ', commande
   SPAWN, commande, size, error_mess, exit_status=exit_status
endelse
;
if (exit_status GT 0) then begin
   ;; we skip the test if no result received !
   MESSAGE, /continue, 'the option used by STAT is not working, please report'
endif else begin
   if ((FSTAT(fd)).size ne size) then begin
      MESSAGE, /continue, 'Bad SIZE returned for zipped file'
      CATCH_EXIT, nb_pbs, no_exit=no_exit
   endif
endelse
;
;; READ TESTS
;
; Readf test
;
str=''
READF, fd, str
;
head1='GDL - GNU Data Language'
; no need to depend on these external commands -- we know the result
;commande='cat '+file+' | gunzip - | head -1'
if KEYWORD_SET(verbose) then print, 'simple readf;'
;SPAWN, commande, head1
if (str ne head1) then begin
   MESSAGE, /continue, 'Readf doesn''t work on zipped file'
   if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif
;
READF, fd, str
head2='A free IDL (Interactive Data Language) compatible incremental'
;commande='cat '+file+' | gunzip - | head -2 | tail -1'
if KEYWORD_SET(verbose) then print, 'simple readf, twice; '
;SPAWN, commande, head2
;
if (str ne head2) then begin
   MESSAGE, /continue, 'Readf doesn''t work twice on zipped file'
   CATCH_EXIT, nb_pbs, no_exit=no_exit
endif
;
; Check that a stat() does not move the pointer position:
head3='compiler (i. e. runs IDL programs).'
if KEYWORD_SET(verbose) then print, 'check stat() does not move pointer position; '
zz=(FSTAT(fd)).size
READF, fd, str
if (str ne head3) then begin
   MESSAGE, /continue, 'stat() on a gzipped file changes pointer position in file --- vey bad!'
   CATCH_EXIT, nb_pbs, no_exit=no_exit
endif
;
if KEYWORD_SET(verbose) then print, 'readf after point_lun(position=0); '
POINT_LUN, fd, 0
READF, fd, str ; At this point, readf works well, so we can test point_lun (except that stat() uses an equivalent of point_lun!!!)
if (str ne head1) then begin
   MESSAGE, /continue, 'Point_lun doesn''t work on zipped file (read mode)'
   CATCH_EXIT, nb_pbs, no_exit=no_exit
endif
if KEYWORD_SET(verbose) then print, 'readf after point_lun(position=end of first line); '
POINT_LUN, fd, strlen(str) ; position at end of first line
READF, fd, str ; should read 2nd line --- was not the case before!
if (str ne head2) then begin
   MESSAGE, /continue, 'Point_lun doesn''t work on zipped file (read mode) when point is not 0!'
   CATCH_EXIT, nb_pbs, no_exit=no_exit
endif
;
; Assoc test
POINT_LUN, fd, 0
a=ASSOC(fd, BYTARR(5))
cut15=['GDL -']
;commande='cat '+file+' | gunzip - | head -1 | cut -b 1-5'
;if KEYWORD_SET(verbose) then print, 'commande: ', commande
;SPAWN, commande, cut15
;
if KEYWORD_SET(verbose) then print, 'test assoc() on zipped file; '
if STRING(a[0]) ne cut15 then begin
   MESSAGE, /continue, 'Assoc doesn''t work on zipped file'
   CATCH_EXIT, nb_pbs, no_exit=no_exit
   if ~KEYWORD_SET(no_exit) then EXIT, status=1
   exit, status=1
endif
;
; ----------
;
if KEYWORD_SET(verbose) then print, 'test assoc() on zipped file when shifting to point_lun(5); '
POINT_LUN, fd, 5
if STRING(a[0]) ne cut15 then begin
   MESSAGE, /continue, 'Assoc doesn''t work twice on zipped file'
   CATCH_EXIT, nb_pbs, no_exit=no_exit
endif
;
; ----------
;
if KEYWORD_SET(verbose) then print, 'test assoc() (index 1 in table) on zipped file; '
if STRING(a[1]) ne ' GNU ' then begin
   MESSAGE, /continue, 'ASSOC doesn''t work with differents indexes on zipped file'
   CATCH_EXIT, nb_pbs, no_exit=no_exit
endif
;
; ----------
;
; Readu test
if KEYWORD_SET(verbose) then print, 'test readu on zipped file; '
POINT_LUN, fd, 0
str='123'
READU, fd, str
;
if (str ne 'GDL') then begin
   MESSAGE, /continue, 'READU doesn''t work on zipped file'
   CATCH_EXIT, nb_pbs, no_exit=no_exit
endif
;
READU, fd, str
;
if KEYWORD_SET(verbose) then print, 'test readu (twice) on zipped file; '
if (str ne ' - ') then begin
   MESSAGE, /continue, 'READU doesn''t work twice on zipped file'
   CATCH_EXIT, nb_pbs, no_exit=no_exit
endif
;
CLOSE, fd
FREE_LUN, fd
;
; ------------------------------
;
;; WRITE TESTS
;
if KEYWORD_SET(verbose) then print, 'various write tests.'
file='test_zipw.gz'
OPENW, fd, file, /get, /compress
PRINTF, fd, 'GNU Data Language'
CLOSE, fd
FREE_LUN, fd
;
OPENR, fd, file, /get, /compress
READF, fd, str
;
if (str ne 'GNU Data Language') then begin
   MESSAGE, /continue, 'Printf doesn''t work on zipped file'
   CATCH_EXIT, nb_pbs, no_exit=no_exit
endif
;
CLOSE, fd
FREE_LUN, fd
;
; ----------
;
OPENW, fd, file, /get, /compress
PRINTF, fd, 'GNU Data Language'
POINT_LUN, fd, 10
; point_lun used in write mode is pretty useless cause you can only move forward and it cause premature EOF.
; Also, you can't open a file in write mode, with compress and append keywords.
; So, if you don't specify append, you truncate the file, and point_lun become useless.
; To summarize, you can't write data and go backward to overwrite it. (Just don't use it)
;
PRINTF, fd, 'rocks' ; GNU Data Language\nrocks\n
CLOSE, fd
FREE_LUN, fd
;
OPENR, fd, file, /get, /compress, /delete
a=ASSOC(fd, BYTARR(10))
;
if (STRING(a[0]) ne 'GNU Data L') OR (STRMID(STRING(a[1]), 0, 7) ne 'anguage') then begin
   MESSAGE, /continue, 'Point_lun doesn''t work on zipped file (write mode)'
   CATCH_EXIT, nb_pbs, no_exit=no_exit
endif
;
CLOSE, fd
FREE_LUN, fd
;
; if we are here and nb_pbs > 0 then /No_exit was set.
;
if ~KEYWORD_SET(quiet) then begin
   if (nb_pbs EQ 0) then begin
      MESSAGE, /continue, 'All ZIP/COMPRESS related tests successful'
   endif else begin
      MESSAGE, /continue, STRING(nb_pbs)+' tests failed'
   endelse
endif
;
if KEYWORD_SET(test) then STOP
;
end
