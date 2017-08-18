;
; AC, July 2017, GPL V3+
;
; Collecting informations on the CPU
; -- processor name
; -- cores number  <<-- tricks on OSX due to OpenMP
; -- bogomips   <<-- does not exist on OSX
; -- cpu MHz
;
function BENCHMARK_GENERATE_CPUINFO, test=test, verbose=verbose, help=help
;
if KEYWORD_SET(help) then begin
   print, 'function BENCHMARK_GENERATE_CPUINFO, test=test, verbose=verbose, help=help'
   return, -1
endif
;
os_name=STRLOWCASE(!version.os_name)
;
if (os_name EQ 'linux') then begin
   ;;
   SPAWN, 'more /proc/cpuinfo  | grep MHz', resu_Mhz
   resu_mhz=STRMID(resu_Mhz[0], STRPOS(resu_Mhz[0],':')+2)
   resu_mhz=FLOAT(resu_mhz)
   ;;
   SPAWN, 'more /proc/cpuinfo  | grep bogo', resu_bogo
   resu_bogo=STRMID(resu_bogo[0], STRPOS(resu_bogo[0],':')+2)
   resu_bogo=FLOAT(resu_bogo)
   ;;
   SPAWN, 'more /proc/cpuinfo  | grep "model name"', model_name
   model_name=STRMID(model_name[0], STRPOS(model_name[0],':')+2)
   ;;
endif
;
if (os_name EQ 'darwin') then begin
   ;;
   SPAWN, 'sysctl -n hw.cpufrequency', resu_Hz
   resu_Mhz=resu_Hz*1.e6
   ;;
   ;; no known equivalent of BogoMIPS on OSX :(
   resu_bogo=-1
   ;;
   SPAWN, 'sysctl -n machdep.cpu.brand_string', model_name
   ;;
endif
;
cpu_info={MHz : resu_Mhz, $
          Bogo : resu_bogo, $
          model : model_name, $
          nb_cores : !cpu.HW_NCPU} ; not fully clear ...
;
if KEYWORD_SET(verbose) then HELP,/struct, cpu_info
if KEYWORD_SET(test) then STOP
;
return, cpu_info
;
end
;
