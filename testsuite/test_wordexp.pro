;
; Alain C., 27 November 2017
;
; In order to try to avoid to make too much mess,
; we work in IDL_TMPDIR since it does exist
; in IDL, FL & GDL
;
; Before applying patch by Greg Jung (2017-NOV-27), we know that :
;
; Don't work well before 2018-MAR-22 :
; - FILE_LINE, FILE_TEST
;
; May work well (not a garanty it works always well) :
; - CD
; - FILE_DIRNAME, FILE_BASENAME, FILE_MKDIR
; - FILE_INFO, FILE_SEARCH
;
; ---------------------------------------
; Modifications history :
;
; - 2018-MAR-21 : AC. rewriting and expand of initial version
;
; ---------------------------------------
;
; create files with suffix equal to lines number inside
; (for later tests)
;
pro TEST_WE_CREATE_FILES, files, test=test, verbose=verbose
;
for jj=0, N_ELEMENTS(files)-1 do begin
   GET_LUN, nblun
   OPENW, nblun, files[jj]
   for ii=1, jj+1 do PRINTF, nblun, ii
   CLOSE, nblun
   FREE_LUN, nblun
endfor
;
if KEYWORD_SET(verbose) then begin
   print, 'Trying to create : ', files
   print, 'Succeed to create : ', FILE_TEST(files)
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
;pro TEST_WE_TEST, nb_errors, fields, flags, test=test, verbose=verbose
;
;for ii=0, N_ELEMENTS(files)-1 do begin
;   if flag[ii] EQ 0 then $
;      ERRORS_ADD, nb_errors, 'Problem with subdir : '+subdirs[ii]      
;endfor
;
;if KEYWORD_SET(test) then STOP
;
;end
; -----------------------------------------------

pro TEST_WORDEXP, help=help, test=test, verbose=verbose, $
                  main_subdir=main_subdir
;
if KEYWORD_SET(test) then begin
   print, 'pro TEST_WORDEXP, help=help, test=test, verbose=verbose, $'
   print, '                  main_subdir=main_subdir'
   return
endif
;
if STRLOWCASE(!version.os_name) eq 'windows' then begin
   message, /continue, 'This code is not design for MSwin plateform'
   message, /continue, 'not sure this code can survive on MS-window OS'
   message, /continue, 'Please report any problem (or success) and improvments'
   message, /continue, 'please report bugs on SF GDL site'
endif
;
nb_errors=0
;
tmpdir=GETENV('IDL_TMPDIR')
;
; does this directory realy exist ?
; if not, working in /tmp
if ~FILE_TEST(tmpdir, /directory) then begin
   MESSAGE, 'IDL_TMPDIR dir. does not exist ?!'
endif
;
CD, tmpdir, current=initial_dir
;
if ~KEYWORD_SET(main_subdir) then main_subdir='test_'+GDL_IDL_FL()+'_WordExp'
;
FILE_MKDIR, main_subdir
CD, main_subdir
CD, cur=working_dir
print, 'Working here : ', working_dir
;
; creating three SubDirs
;
subdirs=['normal','spa ce','q!mark','mix,ing :s']
FILE_MKDIR, subdirs
print, 'creating subdirs : ', subdirs+PATH_SEP()
;
; testing whether the dirs were created
; (but we don't check the names now ...)
;
res=FILE_TEST(working_dir+PATH_SEP()+subdirs)
;
if TOTAL(res) EQ N_ELEMENTS(subdirs) then begin
   print, 'All subdirs well created'
endif else begin
   for ii=0, N_ELEMENTS(subdirs)-1 do begin
      if res[ii] EQ 0 then $
         ERRORS_ADD, nb_errors, 'Problem with subdir : '+subdirs[ii]      
   endfor
endelse
;
; creating files in those SubDirs
;
files=['a1','a 2','a  3','a;4','a.5','a&6']
print, 'Creating some files in each subdirs : ', files
;
for ii=0, N_ELEMENTS(subdirs)-1 do begin
   CD, working_dir+PATH_SEP()+subdirs[ii]
   TEST_WE_CREATE_FILES, files
endfor
;
; testing the created files
;
; 1 Local (going *before* in the sub-dirs
;
for ii=0, N_ELEMENTS(subdirs)-1 do begin
   ;;
   CD, working_dir+PATH_SEP()+subdirs[ii]
   ;;
   ;; do we have created all the files ?
   ;; or maybe the name(s) is/are wrong
   res_test=FILE_TEST(files)
   if TOTAL(res_test) EQ N_ELEMENTS(files) then begin
      print, 'All files have been created (Local)'
   endif else begin
      for ii=0, N_ELEMENTS(files)-1 do begin
         if res_test[ii] EQ 0 then $
            ERRORS_ADD, nb_errors, 'Problem Local with file : '+files[ii]      
      endfor
   endelse
   ;;
   ;; do the files having the good number of lines ?!
   ;; or maybe the name(s) is/are wrong
   ok=EXECUTE('res_lines=FILE_LINES(files)')
   if ~ok then res_lines=REPLICATE(-1, N_ELEMENTS(files))
   for ii=0, N_ELEMENTS(files)-1 do begin
      if res_lines[ii] NE ii+1 then $
         ERRORS_ADD, nb_errors, 'Problem NbLines Local with file : '+files[ii]
   endfor 
endfor
;
CD, working_dir
CD, current=current
if KEYWORD_SET(verbose) then print, 'We are here : ', current
;
; 2 Global (not going into sub-dirs)
;
for ii=0, N_ELEMENTS(subdirs)-1 do begin
   ;;
   path=working_dir+PATH_SEP()+subdirs[ii]+PATH_SEP()
   ;;
   ;; do we have created all the files ?
   ;; or maybe the name(s) is/are wrong
   res_test=FILE_TEST(path+files)
   if TOTAL(res_test) EQ N_ELEMENTS(files) then begin
      print, 'All files have been created (Global)'
   endif else begin
      for ii=0, N_ELEMENTS(files)-1 do begin
         if res_test[ii] EQ 0 then $
            ERRORS_ADD, nb_errors, 'Problem Global with file : '+files[ii]      
      endfor
   endelse
   ;;
   ;; do the files having the good number of lines ?!
   ;; or maybe the name(s) is/are wrong
   ok=EXECUTE('res_lines=FILE_LINES(path+files)')
   if ~ok then res_lines=REPLICATE(-1, N_ELEMENTS(files))
   for ii=0, N_ELEMENTS(files)-1 do begin
      if res_lines[ii] NE ii+1 then $
         ERRORS_ADD, nb_errors, 'Problem NbLines Global with file : '+files[ii]
   endfor
endfor
;
; ----------------------------------
; testing again FILE_BASENAME
;
errors_base=0
base=FILE_BASENAME(files)
expected_base=files
if ~ARRAY_EQUAL(base,expected_base) then $
   ERRORS_ADD, errors_base, 'basic FILE_BASENAME'
;
for ii=0, N_ELEMENTS(subdirs)-1 do begin
   ;; full
   path=working_dir+PATH_SEP()+subdirs[ii]+PATH_SEP()
   base=FILE_BASENAME(path+files)
   if ~ARRAY_EQUAL(base,expected_base) then $
      ERRORS_ADD, errors_base, 'full add FILE_BASENAME'
   ;; relative
   path=subdirs[ii]+PATH_SEP()
   base=FILE_BASENAME(path+files)
   if ~ARRAY_EQUAL(base,expected_base) then $
      ERRORS_ADD, errors_base, 'relative add FILE_BASENAME'
endfor
BANNER_FOR_TESTSUITE, 'testing FILE_BASENAME', errors_base, /short
ERRORS_CUMUL, nb_errors, errors_base
;
; ----------------------------------
; testing again FILE_DIRNAME
;
errors_dir=0
dir=FILE_DIRNAME(files)
expected_dir=REPLICATE('.',N_ELEMENTS(files))
if ~ARRAY_EQUAL(dir,expected_dir) then $
   ERRORS_ADD, errors_dir, 'basic FILE_DIRNAME'
;
for ii=0, N_ELEMENTS(subdirs)-1 do begin
   ;; full
   path=working_dir+PATH_SEP()+subdirs[ii]+PATH_SEP()
   dir=FILE_DIRNAME(path+files)
   expected_dir=REPLICATE(working_dir+PATH_SEP()+subdirs[ii],N_ELEMENTS(files))
   if ~ARRAY_EQUAL(dir,expected_dir) then $
      ERRORS_ADD, errors_dir, 'full add FILE_DIRNAME '+path
   ;; relative
   path=subdirs[ii]+PATH_SEP()
   dir=FILE_DIRNAME(path+files)
   expected_dir=REPLICATE(subdirs[ii],N_ELEMENTS(files))
   if ~ARRAY_EQUAL(dir,expected_dir) then $
      ERRORS_ADD, errors_dir, 'relative add FILE_DIRNAME '+path
endfor
BANNER_FOR_TESTSUITE, 'testing FILE_DIRNAME', errors_dir, /short
ERRORS_CUMUL, nb_errors, errors_dir
;
; ----------------------------------
; testing again FILE_SEARCH
;
errors_search=0
;
for ii=0, N_ELEMENTS(subdirs)-1 do begin
   ;;
   path=working_dir+PATH_SEP()+subdirs[ii]+PATH_SEP()
   res_search=FILE_SEARCH(path, 'a*')
   exp_search=path+files
   res_search=res_search(SORT(res_search))
   exp_search=exp_search(SORT(exp_search))
   ;;
   if ~ARRAY_EQUAL(res_search,exp_search) then $
      ERRORS_ADD, errors_search, 'names in FILE_SEARCH'
endfor
;
; global
path=working_dir+PATH_SEP()
res_search=FILE_SEARCH(path, 'a*2')
exp_search=path+subdirs+PATH_SEP()+files[1]
res_search=res_search(SORT(res_search))
exp_search=exp_search(SORT(exp_search))
;
if ~ARRAY_EQUAL(res_search,exp_search) then $
   ERRORS_ADD, errors_search, 'size in FILE_SEARCH'
;
BANNER_FOR_TESTSUITE, 'testing FILE_SEARCH', errors_search, /short
ERRORS_CUMUL, nb_errors, errors_search
;
; ----------------------------------
; testing again FILE_INFO
;
errors_info=0
;
for ii=0, N_ELEMENTS(subdirs)-1 do begin
   ;; full
   path=working_dir+PATH_SEP()+subdirs[ii]+PATH_SEP()
   info=FILE_INFO(path+files)
   ;;
   res_names=info[*].name
   res_size=info[*].size
   expected_names=path+files
   expected_size=9*(1+L64INDGEN(N_ELEMENTS(files)))
   ;;
   if ~ARRAY_EQUAL(res_names,expected_names) then $
      ERRORS_ADD, errors_info, 'names in FILE_INFO '
   ;;
   if ~ARRAY_EQUAL(res_size,expected_size,/no_type) then $
      ERRORS_ADD, errors_info, 'size in FILE_INFO '
endfor
BANNER_FOR_TESTSUITE, 'testing FILE_INFO', errors_info, /short
ERRORS_CUMUL, nb_errors, errors_info
;
; -----------------
;
print, 'to be extended ...'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_WORDEXP', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then begin
   print, 'Current Dir. : ', GETENV('PWD')
   STOP
endif
;
CD, initial_dir
;
end


