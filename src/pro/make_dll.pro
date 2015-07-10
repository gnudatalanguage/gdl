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
pro MAKE_DLL, input_files, output_file, Exported_Routine_Names, $
              CC=CC, LD=LD, COMPILE_DIRECTORY=COMPILE_DIRECTORY, $
              DLL_PATH=DLL_PATH, EXPORTED_DATA=EXPORTED_DATA, $
              EXTRA_CFLAGS=EXTRA_CFLAGS, EXTRA_LFLAGS=EXTRA_LFLAGS, $
              INPUT_DIRECTORY=INPUT_DIRECTORY, $
              NOCLEANUP=NOCLEANUP, OUTPUT_DIRECTORY=OUTPUT_DIRECTORY, $
              REUSE_EXISTING=REUSE_EXISTING, SHOW_ALL_OUTPUT=SHOW_ALL_OUTPUT, $
              VERBOSE=VERBOSE, $
              test=test, help=help
;
print, 'this code is not finished !'
return
;
if ~KEYWORD_SET(cc) then cc=!make_dll.cc
if KEYWORD_SET(LD) then ld=!make_dll.ld
;
locate="locate 'idl_export.h'"
spawn, locate, result, exit=exit, count=count
if ((exit_status EQ 1) of (count EQ 0)) then begin
   MESSAGE, "header file <idl_export.h> is mandatory ..."
endif
;
; if fact, we just need the path to this header file
;
path=result[0]
path=STRSPLIT(path, 'idl_export.h', /extract, /regex)
path=path[0]
;
ref_cc=cc
step1=STRSPLIT(command, '%Z', /extract, /regex)
ref_cc=step1[0]+' '+path+' '+step1[1]
;
; if not, use first in input files list
if ~KEYWORD_SET(output_file) then output_file=input_files[0]
;
for ii=0, N_ELEMENTS(input_files)-1 do begin
   command=ref_cc
   ;; inserting the C-filename
   step1=STRSPLIT(command, '%X', /extract, /regex)
   command=step1[0]+' '+input_files[ii]+' '+step1[1]
   ;;
endfor
;
if KEYWORD_SET(test) then STOP
;
end
