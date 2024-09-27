;
; AC 2024-JAN-08, under GNU GPL v2+
;
; Return the absolute path of the current interpretor
; * OK on Linux with GDL, IDL & FL
; * OK on OSX with GDL & IDL
;
function GET_PATH_TO_EXE, verbose=verbose, test=test
;
known_case=0
;
; On Linux, this is fine with GDL, IDL & FL
;
if (STRLOWCASE(!version.os) EQ 'linux') then begin
   get_pid=FILE_READLINK('/proc/self')
   get_abspath_to_exe=FILE_READLINK('/proc/'+get_pid+'/exe')
   known_case=1
endif
;
; On OSX/Darwin, more tricky
;
if (STRLOWCASE(!version.os) EQ 'darwin') then begin
   known_case=1
   version=GDL_IDL_FL()
   if version EQ 'GDL' then begin
      tmp=!GDL_MAPS_DIR
      tmp=FILE_DIRNAME(FILE_DIRNAME(FILE_DIRNAME(FILE_DIRNAME(tmp))))
      get_abspath_to_exe=FILE_WHICH(tmp+'/bin', 'gdl')
   endif
   if version EQ 'IDL' then begin
      get_abspath_to_exe=FILE_WHICH(FILE_DIRNAME(!dlm_path), 'idl')
   endif
   if version EQ 'LL' then begin
      the_path=FILE_DIRNAME(!dlm_path)+'/bin'
      get_abspath_to_exe=FILE_WHICH(the_path, 'fl64_cmd')
   endif
endif
;
if KEYWORD_SET(verbose) then begin
   print, 'Running EXE : ', get_abspath_to_exe
endif
;
if ~FILE_TEST(get_abspath_to_exe) then print, 'full path to exe not detected :('
;
if (known_case EQ 0) then begin
   MESSAGE, 'Sorry, this is working only for GNU/Linux & OSX', /continue
   MESSAGE, 'Please contribute !', /continue
   if ~KEYWORD_SET(stop) then EXIT, status=1 else STOP
endif
;
if KEYWORD_SET(stop) then STOP
;
return, get_abspath_to_exe
;
end
;
