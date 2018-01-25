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
; January 2018: completely rewritten by Gilles Duvert, using most of
; the parameters. Tested on Nika pipeline.
;
pro MAKE_DLL, input_files, output_file, Exported_Routine_Names, $
              CC=CC, LD=LD, COMPILE_DIRECTORY=COMPILE_DIRECTORY, $
              DLL_PATH=DLL_PATH, EXPORTED_DATA=EXPORTED_DATA, $
              EXTRA_CFLAGS=EXTRA_CFLAGS, EXTRA_LFLAGS=EXTRA_LFLAGS, $
              INPUT_DIRECTORY=INPUT_DIRECTORY, $
              NOCLEANUP=NOCLEANUP, OUTPUT_DIRECTORY=OUTPUT_DIRECTORY, $
              REUSE_EXISTING=REUSE_EXISTING, SHOW_ALL_OUTPUT=SHOW_ALL_OUTPUT, $
              VERBOSE=VERBOSE, $
              test=test, help=help

objext='.o'
cext='.c'
libext='.so'

locate="locate 'idl_export.h'"
spawn, locate, result, exit=exit_status, count=count
if (exit_status GT 1) then begin
   MESSAGE, 'Sorry, GDL procedure "make_dll.pro" is not working on this computer.'
endif
if ( (exit_status EQ 1) or (count EQ 0) ) then begin
   MESSAGE, 'header file "idl_export.h" is mandatory! "idl_export.h" is part of IDL and not included in this distribution. It can be found on the web.'
endif
;
; We just need the path to this header file
;
idlInclude=result[0]
idlInclude=STRSPLIT(idlInclude, 'idl_export.h', /extract, /regex)
idlInclude=idlInclude[0]
;
;
if ~KEYWORD_SET(cc) then ref_cc=!make_dll.cc
if ~KEYWORD_SET(LD) then ref_ld=!make_dll.ld
if ~KEYWORD_SET(EXTRA_CFLAGS) then EXTRA_CFLAGS=' '
if ~KEYWORD_SET(EXTRA_LFLAGS) then EXTRA_LFLAGS=' '
if ~KEYWORD_SET(COMPILE_DIRECTORY) then COMPILE_DIRECTORY=!make_dll.COMPILE_DIRECTORY
if ~KEYWORD_SET(INPUT_DIRECTORY) then INPUT_DIRECTORY=COMPILE_DIRECTORY
if ~KEYWORD_SET(OUTPUT_DIRECTORY) then OUTPUT_DIRECTORY= COMPILE_DIRECTORY


if ( n_params() eq 2 ) then begin 
   Exported_Routine_Names=output_file
   output_file=input_files[0] 
endif

; real files
source=input_directory+input_files+cext
obj=compile_directory+input_files+objext
lib=output_directory+output_file+libext

; dll_path is used also to test if exist.
givedll=arg_present(DLL_PATH)

if KEYWORD_SET(REUSE_EXISTING) then begin
   if (file_test(lib)) then begin
      if (givedll) then DLL_PATH=lib
      return
   end
end

;
; insert location of "idl_export.h"
cc=ref_cc
step1=STRSPLIT(cc, '%Z', /extract, /regex)
cc=step1[0]+' '+idlInclude+' '+step1[1]
;
; CFLAGS parameter
step1=STRSPLIT(cc, '%X', /extract, /regex)
cc=step1[0]+' '+extra_cflags+' '+step1[1]

ref_cc=cc
for i=0, N_ELEMENTS(source)-1 do begin
   cc=ref_cc
   ;; inserting the C-filename
   step1=STRSPLIT(cc, '%C', /extract, /regex)
   cc=step1[0]+' '+source[i]+' '+step1[1]
   ;;insert the obj %O
   step1=STRSPLIT(cc, '%O', /extract, /regex)
   cc=step1[0]+' '+obj[i]
   ;print,cc
   ; spawn
   spawn, cc
endfor
;link
ld=ref_ld
;%L : library name.
step1=STRSPLIT(ld, '%L', /extract, /regex)
ld=step1[0]+lib+step1[1]
;%X by EXTRA_LFLAGS
step1=STRSPLIT(ld, '%X', /extract, /regex)
ld=step1[0]+EXTRA_LFLAGS
;%O by all the .o:
step1=STRSPLIT(ld, '%O', /extract, /regex)
ld=step1[0]+strjoin(obj,' ')+step1[1]

   ;print,ld
; spawn
spawn, ld
if (givedll) then DLL_PATH=lib
end
