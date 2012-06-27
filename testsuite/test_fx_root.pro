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
function FUNC1, X  
  return, EXP(SIN(X)^2 + COS(X)^2 - 1) - 1 ; IDL doc
end
;
function FUNC2, X;GDL doc
  return, X^2-5
end
;
; -------------------------------------------------
;
pro TEST_FX_ROOT_ON_FUNC, init=init, function_name=function_name, $
                          iter=iter, eps=eps, help=help, test=test
  
if KEYWORD_SET(help) then begin
    print, 'pro TEST_FX_ROOT_ON_FUNC, init=init, function_name=function_name, $
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

pro TEST_FX_ROOT
;float precision for double DCOMPLEXARR
C = COMPLEXARR(1,3)
C[0] = complex(1,0)
C[1] = complex(2,0)
C[2] = complex(3,0)
;  
TEST_FX_ROOT_ON_FUNC, init=C, function_name='FUNC1', iter=100
TEST_FX_ROOT_ON_FUNC, init=C, function_name='FUNC2', iter=100
;
end
