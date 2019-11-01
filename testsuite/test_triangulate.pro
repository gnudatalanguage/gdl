;
; AC 11/11/2016
; Very preliminary tests on TRIANGULATE, TRIGRID, ...
; GD 06/05/2019: test only triangulate and trigrid by default. No gaussfit dependency.
; GD 08/05/2019: gaussfit can be tested as we have it now.
; ----------------------
;
; Regridding a Gaussian computed on a Spiral ...
;
pro TEST_GAUSS2D_ON_SPIRAL, nbp, display=display, fit=fit, $
                            help=help, verbose=verbose, test=test, $
                            no_wait=no_wait, benchmark=benchmark
;
if KEYWORD_SET(benchmark) then TIC
;
if N_PARAMS() LT 1 then nbp=1000
if nbp LT 10 then nbp=1000
;
; waiting time
if ~KEYWORD_SET(no_wait) then wait=1. else wait=no_wait
;
t=100.*FINDGEN(nbp)/nbp
x=t*COS(t)
y=t*SIN(t)
z=EXP(-(x^2+y^2)/1000)
;
TRIANGULATE, x, y, tr
;
if KEYWORD_SET(benchmark) then begin
    TOC & print, 'After TRIANGULATE and before TRIGRID' & TIC
endif
;
trigrid_gauss2d=TRIGRID(x, y, z, tr)
;
if KEYWORD_SET(benchmark) then begin
    TOC & print, 'After TRIGRID  and before Gauss 2D FIT (if)' & TIC
endif
;
if KEYWORD_SET(display) then begin
 WINDOW, xsize=1000, ysize=500
 !p.multi=[0,2,0]
 ;
 PLOT, x, y, psym=-1, symsize=0.3, /iso, title=STRING(nbp)+ ' npbs in Spiral'
 SURFACE, trigrid_gauss2d
 ;
endif
if KEYWORD_SET(fit) then begin
   ;;
   g=GAUSS2DFIT(trigrid_gauss2d, g_params)
   print, g_params
   if KEYWORD_SET(benchmark) then begin
       TOC & print, 'After Gauss 2D FIT (if)' & TIC
   endif
endif
;
if ~KEYWORD_SET(no_wait) then wait=1. else wait=no_wait
if (wait GT 0) then WAIT, wait
;
if KEYWORD_SET(test) then STOP
;
end
;
pro TEST_TRIANGULATE, help=help, test=test, verbose=verbose, benchmark=benchmark, $
                no_exit=no_exit, display=display, fit=fit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_TRIANGULATE, help=help, test=test, verbose=verbose, benchmark=benchmark, $'
   print, '                no_exit=no_exit, display=display', fit=fit
   return
endif
;
if (n_elements(display) eq 0 ) then display=0
if (n_elements(fit) eq 0 ) then fit=1
TEST_GAUSS2D_ON_SPIRAL, 100, display=display, $
                        benchmark=benchmark, fit=fit
;
TEST_GAUSS2D_ON_SPIRAL, 1000, display=display, $
                        benchmark=benchmark, fit=fit
;
TEST_GAUSS2D_ON_SPIRAL, 10000, display=display, $
                        benchmark=benchmark, fit=fit
;

end


