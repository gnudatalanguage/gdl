;
; AC, August 8, 2017, under GNU GPL v3+
;
; IDL does have a native SAVE/RESTORE lib.
;
; For GDL and FL, we do used CMSV lib. It must be in the path ...
; explanations in "save.pro" in src/pro
;
; Since Version 0.79.38, Fawlty Language does have
; an interface (SAVE & RESTORE) with the CMSVlib.  
; Unfortunately we have no simple way to check the version number in
; FL and FL 0.79.32 crashes when trying to know whether SAVE is around ...
;
; For GDL and FL, we only check if SAVE around (assuming RESTORE
; around too !)
;
; For older versions of FL (before 0.79.38) we use an heuristic:
; - testing CROSSP to know the version
; - if old version, request to compile SAVE before re-run this code ...
;
; -----------
; Modifications history :
;
; * AC 2016-03-03 : we assume FL version is >= 0.7.9.38, managing
; older too.
;
pro CHECK_SAVE_RESTORE, verbose=verbose, test=test
;
ON_ERROR, 2
;
; shortcut : when we found one time, we keep the info
;
DEFSYSV, '!save_restore', exist=exist
if (exist EQ 1) then begin
   if (!save_restore EQ 1) then return
   flag_sr=!save_restore
endif else begin
   flag_sr=0
endelse
;
soft=GDL_IDL_FL(/upper)
;
; nothing to do since IDL has a native one :)
if (soft EQ 'IDL') then begin
   DEFSYSV, '!save_restore', 1
   if KEYWORD_SET(verbose) then print, 'IDL detected, native lib.'
   return
endif
;
if (soft EQ 'GDL') THEN BEGIN
   ;; 26-dec-2017 : we changed CVS to SVN in !GDL.release ...
   if ((GDL_VERSION() GE 908) OR (STRPOS(!GDL.release, 'SVN') GT 0)) then begin
      DEFSYSV, '!save_restore', 1
      if KEYWORD_SET(verbose) then print, 'GDL with S/R detected as native lib.'
      return
   endif
endif
;
if (soft EQ 'FL') then begin
   ;; we need to assume we use FL >= 0.79.38
   if KEYWORD_SET(verbose) then $
      print, 'FL detected, native wrapper assuming FL >= 0.79.38'
endif
;
; is the external CMSVlib present ?
if (EXECUTE('res=CMSVLIB(/QUERY)') EQ 0) then begin
   MESSAGE,  /CONTINUE, "Missing CMSVlib in your GDL_PATH or IDL_PATH"
   MESSAGE,  "please read GDL SAVE header for help."
endif else begin
   MESSAGE,  /CONTINUE, "CMSVlib succesfully detected !"
endelse
;
; This is not working with FL 0.79.32, which is the case it
; would have been usefull !!!
;
;; is the SAVE wrapper (provided by GDL) present ?
if (soft EQ 'GDL') then begin
   if (EXECUTE('SAVE') EQ 0) then begin
      MESSAGE,  /CONTINUE, "Missing SAVE in your GDL_PATH or IDL_PATH"
      MESSAGE,  "please read GDL SAVE header for help."
   endif
endif
;
; CROSSP was introduced in FL at the same time than the
;  SAVE/RESTORE wrapper. In FL 0.79.32, calling directly
;  EXECUTE('SAVE') crashes, we test CROSSP instead ...
;
if (soft EQ 'FL') then begin
   ;;
   ;; the first we run this code, if an old version of FL
   ;; is detected, we ask to compile before SAVE & RESTORE
   ;; to avoid crash (FL 0.79.32)
   if (flag_sr EQ -1) then begin
      if (EXECUTE('SAVE') EQ 1) then begin
         MESSAGE,  /CONTINUE, "old FL with GDL wrapper for SAVE/RESTORE"
         DEFSYSV, '!save_restore', 1
      endif else begin
         MESSAGE, "SAVE GDL wrapper not found"
      endelse
   endif else begin
      bili='Z=CROSSP(INDGEN(3),indgen(3))'
      if (EXECUTE(bili) EQ 0) then begin
         DEFSYSV, '!save_restore', -1
         MESSAGE,  /CONTINUE, "old FL detected (before 0.79.38)"
         MESSAGE,  "You must compiled SAVE & RESTORE as provided by GDL before"
      endif else begin
         MESSAGE,  /CONTINUE, "recent FL detected (>= 0.79.38)"
         MESSAGE,  /CONTINUE, "with internal wrapper to SAVE/RESTORE"
      endelse
   endelse
endif
;
; if we are here, CMSVlib found and SAVE too
; (since RESTORE usually in the same PATH than SAVE, we don't
; check for it !)
;
MESSAGE,  /CONTINUE, "SAVE/RESTORE assumed to be operationnal !"
;
DEFSYSV, '!save_restore', 1
;
if KEYWORD_SET(test) then STOP
;
end
