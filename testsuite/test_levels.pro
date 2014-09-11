;
; AC 12-sep-2014
; Now we can follow the "level" using HELP, /traceback
; Later, when HELP, /traceback, output=out
; will be OK, we will be able to test the numerical values (lines)
;
; ----------------------------
;
pro LEVEL3
;
print, 'we are in LEVEL 3'
HELP, /traceback
stop
;
end
;
; ----------------------------
;
pro LEVEL2, test=test
;
print, 'we are in LEVEL 2'
HELP, /traceback
LEVEL3
HELP, /traceback
if KEYWORD_SET(test) then stop
;
end
;
; ----------------------------
;
pro LEVEL1
;
print, 'we are in LEVEL 1'
HELP, /traceback
LEVEL2
HELP, /traceback
;
if KEYWORD_SET(test) then stop
;
end
;
; ----------------------------
;

pro TEST_LEVELS
print, 'we are at MAIN LEVEL'
HELP, /traceback
LEVEL1
HELP, /traceback
if KEYWORD_SET(test) then stop

end
