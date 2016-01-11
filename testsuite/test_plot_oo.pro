;
; Alain C., July 2015. During the Nika2 workshop, 
; a bug was found in the way of PLOT_OO was plotting Logaritmic scales.
;
; regression on procedures and keywords related to Logarithmic outputs:
; procudes : PLOT, PLOT_IO, PLOT_OI, PLOT_OO
; keywords : XTYPE, XLOG; YTYPE, YLOG
;
; Since the C++ is very symetrical
; we do not test the Y case and the XY case
;
; September :
; - a way to count the type is used
; - PS or SVG outputs
;
; 2016-01-11 AC : better way to manage if GDL not compile with X11
;                 preparing WIN tests (not checked)
;
pro INIT_DATA, x, y, range
;
x=FINDGEN(100)+1
y=2*x
;
midx=(MIN(x)+MAX(x))/2.
midy=(MIN(y)+MAX(y))/2.
logmidx=CEIL(ALOG(midx))+1
logmidy=CEIL(ALOG(midy))+1
;
range=[midx,midy,logmidx,logmidy]
;
end
;
; -------------------------------------------------
;
pro DO_IT, x, y, range, proc, count, com
;
a=EXECUTE(proc+com)
;
;print, !x.type, !y.type
;
midx=range[0] & midy=range[1]
logmidx=range[2] & logmidy=range[3]
;
type=!x.type+2*!y.type
;
DEFSYSV, '!gdl', exists=exists
;
pcs_ref=!p.charsize
if (exists) then fsiz=4 else fsiz=2
!p.charsize=fsiz
;
colr='ff'x
coly='ffff'x
colg='ff00'x
colb='ffff00'x
;
if (type EQ 0) then XYOUTS, midx, midy, 'LIN', align=0.5, col=coly
if (type EQ 1) then XYOUTS, logmidx, midy, 'XLOG', align=0.5, col=colg
if (type EQ 2) then XYOUTS, midx, logmidy, 'YLOG', align=0.5, col=colb
if (type EQ 3) then XYOUTS, logmidx, logmidy, 'XYLOG', align=0.5, col=colr
;

if (type EQ 0) then count++
if (type EQ 1) then count=count+10
if (type EQ 2) then count=count+100
if (type EQ 3) then count=count+1000
;
!p.charsize=pcs_ref
;
end
;
; -------------------------------------------------
;
pro CALL_X_PLOT, proc, count, winid=winid, title=title
;
!p.multi=[0,3,3]
;
if (!D.name EQ 'X') then begin
    DEVICE, /decomposed
    if ~KEYWORD_SET(winid) then winid=!D.window+1
    WINDOW, winid, xsize=400, ysize=400, title=title
endif else begin
    ;; SVG or PS : preparing output file name
    prefix=GDL_IDL_FL()
    suffix=STRlowCASE('.'+!D.name)
    DEVICE, file=prefix+'_test_plot_oo_'+proc+suffix
endelse
;
INIT_DATA, x, y, range
;
DO_IT, x, y, range, proc, count, ', x, y'
DO_IT, x, y, range, proc, count, ', x, y, xtype=0'
DO_IT, x, y, range, proc, count, ', x, y, xtype=1'
;
DO_IT, x, y, range, proc, count, ', x, y, xlog=0'
DO_IT, x, y, range, proc, count, ', x, y, xlog=0, xtype=0'
DO_IT, x, y, range, proc, count, ', x, y, xlog=0, xtype=1'
;
DO_IT, x, y, range, proc, count, ', x, y, xlog=1'
DO_IT, x, y, range, proc, count, ', x, y, xlog=1, xtype=0'
DO_IT, x, y, range, proc, count, ', x, y, xlog=1, xtype=1'
;
if (!D.name NE 'X') then DEVICE, /close
;
end
;
; -------------------------------------------------
;
pro TEST_PLOT_OO_CALL, errors
;
print, 'running the tests for DEVICE type : '+!D.name
;
if ~ISA(errors) then errors=0
;
DEFSYSV, '!gdl', exists=exists
if exists then ttl='GDL : ' else ttl='IDL : '
;
count=0
CALL_X_PLOT, 'plot', count, winid=1, title=ttl+'X and PLOT'
if (count NE 45) then errors++
;
count=0
CALL_X_PLOT, 'plot_io', count, winid=2, title=ttl+'X and PLOT_IO'
if (count NE 4500) then errors++
;
count=0
CALL_X_PLOT, 'plot_oi', count, winid=3, title=ttl+'X and PLOT_OI'
if (count NE 54) then errors++
;
count=0
CALL_X_PLOT, 'plot_oo', count, winid=4, title=ttl+'X and PLOT_OO'
if (count NE 5400) then errors++
;
end
;
; -------------------------------------------------
;
pro TEST_PLOT_OO, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_PLOT_OO, help=help, verbose=verbose, $'
    print, '                  no_exit=no_exit, test=test'
    return
endif
;
save_p=!p
save_dname=!d.name
;
errors=0
;
test=EXECUTE('SET_PLOT, ''X''')
if (test) then begin
   if (!d.name NE 'NULL') OR (GETENV('DISPLAY') NE '') then begin
      TEST_PLOT_OO_CALL, errors
      print, 'X11 tests done !'
   endif else begin
      print, 'GDL compiled with X11 support but we got a problem (Display ?)'
   endelse
endif else begin
   print, 'X11 tests not done, GDL compiled without X11 support'
endelse
;
test=EXECUTE('SET_PLOT, ''WIN''')
if (test) then begin
   if (!d.name NE 'NULL') OR (GETENV('DISPLAY') NE '') then begin
      TEST_PLOT_OO_CALL, errors
      print, 'WIN tests done !'
   endif else begin
      print, 'GDL compiled with WIN support but we got a problem (Display ?)'
   endelse
endif else begin
   print, 'WIN tests not done, GDL compiled without WIN support'
endelse
;
; IDL/FL don't have SVG
DEFSYSV, '!gdl', exists=exists
if (exists) then begin
   SET_PLOT, 'svg'
   TEST_PLOT_OO_CALL, errors
   print, 'SVG tests done !'
endif else begin
   print, 'no test within SVG output'
endelse
;
; this mode should be always available
SET_PLOT, 'ps'
TEST_PLOT_OO_CALL, errors
print, 'PS tests done !'
;
; reset the initial !p env.
;
!p=save_p
SET_PLOT, save_dname
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_PLOT_OO', errors
;
if (errors GT 0) then begin
    print, 'The errors count is global and reveals usually'
    print, 'more than one error in a given configuration'
endif
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
