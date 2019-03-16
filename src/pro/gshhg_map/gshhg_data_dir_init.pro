;
pro GSHHG_DATA_DIR_INIT
;
; in IDL, this variable is not defined ...
;
DEFSYSV, '!GSHHG_DATA_DIR', exist=exist
;
if ~exist then DEFSYSV, '!GSHHG_DATA_DIR', ''
;
end
