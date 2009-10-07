; Alain Coulais
; 6 octobre 2009 a Sapporo, Hokkaido
; Under GNU GPL V2 or later
;
; Since CHECK_MATH() does contains undocumented features,
; we need a clear check of its behavior
;
;
pro DO_ERROR, type
if type EQ 0 then DO_ERROR_INT
if type EQ 1 then DO_ERROR_FLT
if type EQ 2 then DO_ERROR_ZERO
end
pro DO_ERROR_INT
a=1/0
end
pro DO_ERROR_FLT
a=1./0.
end
pro DO_ERROR_ZERO
a=0./0.
end
;
pro TEST_CHECK_MATH, type=type
;
if N_ELEMENTS(type) EQ 0 then type=1
type=ROUND(type)
if ((type LT 0) OR (type GT 2)) then begin
   print, 'TYPE is in {0,1,2}'
   return
endif
;
print, 'init:          ', check_math()
DO_ERROR, type
print, 'check_math()   ', check_math()
DO_ERROR, type
print, 'check_math(0,0)', check_math(0,0)
DO_ERROR, type
print, 'check_math(1,0)', check_math(1,0)
DO_ERROR, type
print, 'check_math(0,1)', check_math(0,1)
DO_ERROR, type
print, 'check_math(1,1)', check_math(1,1)
;

end
;
pro TEST_CHECK_MATH_CUMUL
;
; cleaning the memory
a=check_math(noclear=0)
;
a=1/0  & print,'(expected   1)', check_math(/pri,/noc)
a=1/0. & print,'(expected  17)', check_math(/pri,/noc)
a=0/0. & print,'(expected 145)', check_math(/pri,/noc)
a=check_math()
a=0/0. & print,'(expected 128)', check_math(/pri,/noc)
a=0
print,'(expected 128)', check_math()
a=0
print,'(expected   0)', check_math()
end

pro MINI_PRO
print, 'mini_pro'
a=0./0.
print, check_math()
end

print, 'program'
a=check_math()
a=0./0.
print, check_math()
MINI_PRO
end
