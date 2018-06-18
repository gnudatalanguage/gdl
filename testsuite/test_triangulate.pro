;
; AC 11/11/2016
; Very preliminary tests on TRIANGULATE, TRIGRID, ...
;
; ----------------------
;
; Regridding a Gaussian computed on a Spiral ...
;
pro TEST_GAUSS2D_ON_SPIRAL, nbp, no_display=no_display, no_fit=no_fit, $
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
WINDOW, xsize=1000, ysize=500
!p.multi=[0,2,0]
;
PLOT, x, y, psym=-1, symsize=0.3, /iso, title=STRING(nbp)+ ' npbs in Spiral'
SURFACE, trigrid_gauss2d
;
if ~KEYWORD_SET(no_fit) then begin
   ;;
   is_code_around=EXECUTE("a=GAUSS2DFIT(RANDOMU(seed, 12,12))")
   if is_code_around then begin
      g=GAUSS2DFIT(trigrid_gauss2d, g_params)
      print, g_params
   endif
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
                no_exit=no_exit, no_display=no_display
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_TRIANGULATE, help=help, test=test, verbose=verbose, benchmark=benchmark, $'
   print, '                no_exit=no_exit, no_display=no_display'
   return
endif
;
TEST_GAUSS2D_ON_SPIRAL, 100, no_display=no_display, $
                        benchmark=benchmark, no_fit=no_fit
;
TEST_GAUSS2D_ON_SPIRAL, 1000, no_display=no_display, $
                        benchmark=benchmark, no_fit=no_fit
;
TEST_GAUSS2D_ON_SPIRAL, 10000, no_display=no_display, $
                        benchmark=benchmark, no_fit=no_fit
;

end


