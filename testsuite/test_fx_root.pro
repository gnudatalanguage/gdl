;
; under GNU GPL v2 or later
; Benjamin Laurent, 18-June-2012
; long term contact : Alain Coulais
; 
; -------------------------------------------------
; usage: in GDL CLI:
; GDL> test_fx_root
;
; -------------------------------------------------
;
; from IDL doc
function FUNC1, X  
  return, EXP(SIN(X)^2 + COS(X)^2 - 1) - 1.
end
;
; from GDL doc
function FUNC2, X
  return, X^2-5.
end
;
; from http://objectmix.com/idl-pvwave/169016-better-root-finder.html
; three roots, depending the range where we are computing
function FUNC3, x
return, x^3-8.0
end
;
; -------------------------------------------------
;
pro TEST_FX_ROOT_ON_FUNC, init=init, function_name=function_name, $
                          iter=iter, eps=eps, help=help, test=test
  
if KEYWORD_SET(help) then begin
    print, 'pro TEST_FX_ROOT_ON_FUNC, init=init, function_name=function_name, $'
    print, '                          iter=iter, eps=eps, help=help, test=test'
    return
endif
;
;Error tolerance
if (N_ELEMENTS(eps) EQ 0) then eps=1e-4
;
if N_ELEMENTS(function_name) EQ 0 then function_name='FUNC1'
;
nb_errors=0
;
;return one root
resuFX=FX_ROOT(init,function_name,ITMAX=iter,/DOUBLE,STOP=1,TOL=0.00001)
;  
print, resuFX
;
;comparing
;
if abs(CALL_FUNCTION(function_name,resuFX)) GT eps then nb_errors=nb_errors+1

if (nb_errors GT 0) then begin
    MESSAGE, /continue, STRING(nb_errors)+' Errors founded'
    if ~KEYWORD_SET(test) then EXIT, status=1
endif else begin
    MESSAGE, /continue, 'Function '+function_name+' : No Errors founded'
endelse
  
if KEYWORD_SET(test) then STOP

end
;
; depending the [x0,x1,x2] values, we can converge to different roots
;
pro WHERE_DO_WE_CONVERGE
;
tol=1.e-5
expected_module=2.
re_r1=-1
;
root1=FX_ROOT([-10,0,.5], 'FUNC3')
root2=FX_ROOT([0,.5,3], 'FUNC3')    
;
errors=0
; checking module values
if ABS(ABS(root1)-expected_module) GT tol then begin
   errors=errors+1
   MESSAGE,/continue, 'root1 inaccurate'
endif
if ABS(ABS(root2)-expected_module) GT tol then begin
   errors=errors+1
   MESSAGE,/continue, 'root2 inaccurate'
endif
;
; checking roots values
if ABS(real_part(root1)-re_r1) GT tol then begin
   errors=errors+1
   MESSAGE,/continue, 'unexpected root1'
endif
if ABS(real_part(root2)-expected_module) GT tol then begin
   errors=errors+1
   MESSAGE,/continue, 'unexpected root2'
endif
;
if errors EQ 0 then MESSAGE,/continue, 'convergence on expected roots well done'
;
end
;
pro TEST_FX_ROOT
;
;float precision for double DCOMPLEXARR
C = COMPLEXARR(1,3)
C[0] = complex(1,0)
C[1] = complex(2,0)
C[2] = complex(3,0)
c=reform(c)
;
TEST_FX_ROOT_ON_FUNC, init=C, function_name='FUNC1', iter=100
TEST_FX_ROOT_ON_FUNC, init=C, function_name='FUNC2', iter=100
;
; informational test
WHERE_DO_WE_CONVERGE
;
end
