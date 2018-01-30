;
; Alain C. + Ilia N., 8 July 2015, back from the Nika2 pipeline meeting
; Quick and dirty to provide useful tool.
; (large experience of *.so files compiled by hand or via IDL working
; in GDL (e.g. PIOlib, Nika, ...))
;
; The trouble is to have a "idl_export.h" around
;
; under GNU GPL V2 or later
;
; ---------------------------------------
; Modifications history :
;
; - 2018-JAN-25 : completely rewritten by Gilles Duvert, using most of
;   the parameters. Tested on Nika pipeline.
;
; - 2018-JAN-26 : AC : trying to improve & simplify the default interface.
;
; ---------------------------------------
;
; Do we have access to a "idl_export.h" file ? It is mandatory !
; the logic ? 1: local, 2: in an 'include/', 3: anywhere
;
function LOOKFOR_IDL_EXPORT, test=test, verbose=verbose
;
ON_ERROR, 2
;
; do we have the file in the current directory ?!
;
ok=FILE_TEST('idl_export.h')
if KEYWORD_SET(verbose) then print, 'result of step 1 : ', ok
if ok then result='idl_export.h'
;
if ~ok then begin
   ;;
   ;; file "idl_export.h" usualy under sub-dir include/
   ;;
   locate="locate 'include/idl_export.h'"
   SPAWN, locate, result, exit=exit_status, count=count
   ;;
   if (exit_status GT 1) then begin
      MESSAGE, 'Sorry, GDL procedure "make_dll.pro" is not working on this computer.'
   endif
   if ~((exit_status EQ 1) or (count EQ 0) ) then ok=1
   if KEYWORD_SET(verbose) then print, 'result of step 2 : ', ok
endif
if ~ok then begin
   ;;
   ;; let try again on less conventional place
   locate="locate 'idl_export.h'"
   SPAWN, locate, result, exit=exit_status, count=count
   if (exit_status GT 1) then begin
      MESSAGE, 'Sorry, GDL procedure "make_dll.pro" is not working on this computer.'
   endif
   if ~((exit_status EQ 1) or (count EQ 0) ) then ok=1
   if KEYWORD_SET(verbose) then print, 'result of step 3 : ', ok
endif
if ~ok then begin
   txt='header file "idl_export.h" is mandatory! "idl_export.h" '
   txt=txt+'is part of IDL and not included in this distribution.'
   MESSAGE, txt+' It can be found on the web.'
endif
;
; We just need the path to this header file
;
idlInclude=result[0]
;
if KEYWORD_SET(verbose) then print, 'header located here : ', idlInclude
;
if KEYWORD_SET(test) then stop
;
return, idlInclude
;
end
;
; ---------------------------------------
;
pro MAKE_DLL, input_files, output_file, Exported_Routine_Names, $
              CC=CC, LD=LD, COMPILE_DIRECTORY=COMPILE_DIRECTORY, $
              DLL_PATH=DLL_PATH, EXPORTED_DATA=EXPORTED_DATA, $
              EXTRA_CFLAGS=EXTRA_CFLAGS, EXTRA_LFLAGS=EXTRA_LFLAGS, $
              INPUT_DIRECTORY=INPUT_DIRECTORY, $
              NOCLEANUP=NOCLEANUP, OUTPUT_DIRECTORY=OUTPUT_DIRECTORY, $
              REUSE_EXISTING=REUSE_EXISTING, SHOW_ALL_OUTPUT=SHOW_ALL_OUTPUT, $
              verbose=verbose, test=test, help=help, debug=debug
;
if KEYWORD_SET(help) then begin
   print, 'pro MAKE_DLL, input_files, output_file, Exported_Routine_Names, $'
   print, '              CC=CC, LD=LD, COMPILE_DIRECTORY=COMPILE_DIRECTORY, $'
   print, '              DLL_PATH=DLL_PATH, EXPORTED_DATA=EXPORTED_DATA, $'
   print, '              EXTRA_CFLAGS=EXTRA_CFLAGS, EXTRA_LFLAGS=EXTRA_LFLAGS, $'
   print, '              INPUT_DIRECTORY=INPUT_DIRECTORY, $'
   print, '              NOCLEANUP=NOCLEANUP, OUTPUT_DIRECTORY=OUTPUT_DIRECTORY, $'
    print, '             REUSE_EXISTING=REUSE_EXISTING, SHOW_ALL_OUTPUT=SHOW_ALL_OUTPUT, $'
   print, '              verbose=verbose, test=test, help=help, debug=debug'
   return
endif
;
objext='.o'
cext='.c'
libext='.so'
;
; AC 2018-01-29 : to be generalized
; see also "mg_idl_export.h" for OSX
;
do_include=0
if do_include then begin
   idlInclude=LOOKFOR_IDL_EXPORT(verbose=verbose, test=test)
   idlInclude=STRSPLIT(idlInclude, 'idl_export.h', /extract, /regex)
   idlInclude=idlInclude[0]
endif else begin
   idlInclude=''
endelse
;
if ~KEYWORD_SET(cc) then ref_cc=!make_dll.cc
if ~KEYWORD_SET(LD) then ref_ld=!make_dll.ld
if ~KEYWORD_SET(EXTRA_CFLAGS) then EXTRA_CFLAGS=' '
if ~KEYWORD_SET(EXTRA_LFLAGS) then EXTRA_LFLAGS=' '
if ~KEYWORD_SET(COMPILE_DIRECTORY) then COMPILE_DIRECTORY=!make_dll.COMPILE_DIRECTORY
if ~KEYWORD_SET(INPUT_DIRECTORY) then INPUT_DIRECTORY=COMPILE_DIRECTORY
if ~KEYWORD_SET(OUTPUT_DIRECTORY) then OUTPUT_DIRECTORY= COMPILE_DIRECTORY
;
if (N_PARAMS() EQ 2) then begin 
   Exported_Routine_Names=output_file
   output_file=input_files[0] 
endif
;
; real files
;
source=PATH_SEP_ADD(input_directory)+input_files+cext
obj=PATH_SEP_ADD(compile_directory)+input_files+objext
lib=PATH_SEP_ADD(output_directory)+output_file+libext
;
print, source
print, obj
print, lib
;
;stop
; dll_path is used also to test if exist.
givedll=ARG_PRESENT(DLL_PATH)
;
if KEYWORD_SET(REUSE_EXISTING) then begin
   if (FILE_TEST(lib)) then begin
      if (givedll) then DLL_PATH=lib
      return
   end
end
;
; insert location of "idl_export.h"
;
cc=ref_cc
if STRLEN(idlInclude) GT 0 then begin
   step1=STRSPLIT(cc, '%Z', /extract, /regex)
   cc=step1[0]+' '+idlInclude+' '+step1[1]
endif else begin
   step1=STRSPLIT(cc, '-I%Z', /extract, /regex)
   cc=step1[0]+' '+step1[1]   
endelse
;stop
;
;
; CFLAGS parameter
step1=STRSPLIT(cc, '%X', /extract, /regex)
cc=step1[0]+' '+extra_cflags+' '+step1[1]
;
ref_cc=cc
for i=0, N_ELEMENTS(source)-1 do begin
   cc=ref_cc
   ;; inserting the C-filename
   step1=STRSPLIT(cc, '%C', /extract, /regex)
   cc=step1[0]+' '+source[i]+' '+step1[1]
   ;;insert the obj %O
   step1=STRSPLIT(cc, '%O', /extract, /regex)
   cc=step1[0]+' '+obj[i]
   if KEYWORD_set(debug) then print, cc
   ;; spawn
   spawn, cc
endfor
;
if KEYWORD_set(debug) then STOP

;link
ld=ref_ld
;
if (!version.os_name EQ 'darwin') then begin
   step1=STRSPLIT(ld, '-shared ', /extract, /regex)
   ld=step1[0]+' -dylib '+step1[1]
endif
;
;%L : library name.
step1=STRSPLIT(ld, '%L', /extract, /regex)
ld=step1[0]+lib+step1[1]
;%X by EXTRA_LFLAGS
step1=STRSPLIT(ld, '%X', /extract, /regex)
ld=step1[0]+EXTRA_LFLAGS
;%O by all the .o:
step1=STRSPLIT(ld, '%O', /extract, /regex)
ld=step1[0]+strjoin(obj,' ')+step1[1]
;
if KEYWORD_set(debug) then print, ld
; spawn
spawn, ld
if (givedll) then DLL_PATH=lib
;
if KEYWORD_SET(test) then STOP
;
end
