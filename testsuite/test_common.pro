;
; Alain Coulais 2010, Nov 30.
;
; When compiling MPFIT, current CVS of GDL (0.9) crashes.
; MPFIT was running smoothly before ...
; It fact, it is only due to a problem related to "common" 
; (regression happens between 2010 April 07 and 19)
;
; Problem is at COMPILATION level.
;
pro TEST_PRO_COMMON_ONLY
common c_pro_only, aa
end
;
function TEST_FUN_COMMON_ONLY
common c_fun_only, aa
end
;
pro TEST_PRO_COMMON
common c_pro, aa
z=1
end
;
function TEST_FUN_COMMON
common c_fun, aa
y=2
end
; just to avoid an error message
pro test_common
end
