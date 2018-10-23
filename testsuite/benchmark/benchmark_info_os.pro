;
; AC, Octobre 201_, GPL V3+
;
; Collecting informations on the Machine
; -- hostname
; -- detailed info on the OS
;
; Help welcome (MSwin ... BSD ...)
;
; ----------------------------------------
;
function BENCHMARK_INFO_OS, test=test, verbose=verbose, help=help
;
if KEYWORD_SET(help) then begin
   print, 'function BENCHMARK_INFO_OS, test=test, verbose=verbose, help=help'
   return, -1
endif
;
info_os={hostname :'', $
         generic_osname : 'TBD', $
         family_osname : 'TBD', $
         accurate_osname : 'TBD'}
;
machine=GET_LOGIN_INFO()
info_os.hostname=machine.machine_name
;
os_name=STRLOWCASE(!version.os)
;
if (os_name EQ 'linux') then begin
   ;;
   info_os.generic_osname='Linux'
   ;;
   txt='' & restxt=''
   filename='/etc/os-release'
   if FILE_TEST(filename) then begin
      txt=READ_ASCII_BASIC(filename)
   endif else begin
      MESSAGE, /continue, 'missing file : '+filename
      MESSAGE, /continue, 'no Linux OS specific info collected'
   endelse
   ;;
   ok=WHERE(STRPOS(txt,'NAME=') EQ 0, nbp_ok)
   if (nbp_ok GE 1) then begin
      tmp=txt[ok]
      tmp=strmid(tmp, STRLEN('NAME='))
      info_os.family_osname=STRJOIN(STRSPLIT(tmp,'"', /EXTRACT),'')
   endif else info_os.family_osname=os_name+' (unknown)'
   ;;
   ok=WHERE(STRPOS(txt,'VERSION=') GE 0, nbp_ok)
   if (nbp_ok GE 1) then begin
      tmp=txt[ok]
      tmp=strmid(tmp, STRLEN('VERSION='))
      info_os.accurate_osname=STRJOIN(STRSPLIT(tmp,'"', /EXTRACT),'')
   endif else info_os.accurate_osname=os_name+' (unknown)'
   ;;
endif
;
if (os_name EQ 'darwin') then begin
   ;;
   info_os.generic_osname='OSX'
   ;;
   SPAWN, 'sw_vers', txt
   tmp=txt[1]
   tmp=strmid(tmp, STRLEN('ProductVersion:'))
   info_os.accurate_osname=tmp
   ;;
endif
;
if KEYWORD_SET(verbose) then HELP,/struct, info_os
if KEYWORD_SET(test) then STOP
;
return, info_os
;
end
;
