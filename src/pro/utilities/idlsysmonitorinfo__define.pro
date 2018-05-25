;
; Under GNU GPL V3
; Alain C., 22 February 2017
;
; Very preliminary version of IDLSYSMONITORINFO,
; just what is needed by some examples in Coyote lib.
; (which is provided by Debian ...)
;
; Please post improvments or comments at:
; https://github.com/gnudatalanguage/gdl/issues
;
function idlsysmonitorinfo::GetPrimaryMonitorIndex
return, 0
end
;
function idlsysmonitorinfo::GetRectangles, Exclude_Taskbar=Exclude_Taskbar
;
size=GET_SCREEN_SIZE()
xmax=size[0]
ymax=size[1]
;
return, [0,0, xmax, ymax]
;
end
;
; --------------------------
;
function idlsysmonitorinfo::GetResolutions
;
size=GET_SCREEN_SIZE(resolution=resolution)
;
return, resolution
;
end
;
; --------------------------
;
function IDLsysMonitorInfo::IsExtendedDesktop
return, 0
end
;
; --------------------------
;
function idlsysmonitorinfo::Init, filename, VERBOSE=verbose
;
;self._rectangles = ptr_new(/ALLOCATE_HEAP)
;self._DISPLAY_NAMES = ''
;self._RESOLUTIONS = ptr_new(/ALLOCATE_HEAP)
;self.IDLSYSMONITORINFO_TOP=0L
return, 1
end

pro idlsysmonitorinfo__define, struct
struct = {idlsysmonitorinfo, $
          _rectangles: ptr_new(), $
          _DISPLAY_NAMES: '', $
          _RESOLUTIONS: ptr_new(), $
          IDLSYSMONITORINFO_BOTTOM: 0l, $
          IDLSYSMONITORINFO_TOP: 0l $
}
end

