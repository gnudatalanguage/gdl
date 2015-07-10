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
; ToDo : PS or SVG outputs
;
pro INIT_DATA, x, y, range
;
x=FINDGEN(100)+1
y=2*x
;
midx=(MIN(x)+MAX(x))/2.
midy=(MIN(y)+MAX(y))/2.
logmidx=ceil(ALOG(midx))+1
logmidy=ceil(ALOG(midy))+1
;logmidy=midy;ALOG(midy)
;
range=[midx,midy,logmidx,logmidy]
;
end

pro DO_IT, x, y, range, proc, com

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
!p.charsize=pcs_ref
;
end
;
pro CALL_X_PLOT, proc, winid=winid, title=title
;
!p.multi=[0,3,3]
if ~KEYWORD_SET(winid) then winid=!D.window+1
WINDOW, winid, xsize=400, ysize=400, title=title
;
INIT_DATA, x, y, range
;
DO_IT, x, y, range, proc, ', x, y'
DO_IT, x, y, range, proc, ', x, y, xtype=0'
DO_IT, x, y, range, proc, ', x, y, xtype=1'
;
DO_IT, x, y, range, proc, ', x, y, xlog=0'
DO_IT, x, y, range, proc, ', x, y, xlog=0, xtype=0'
DO_IT, x, y, range, proc, ', x, y, xlog=0, xtype=1'
;
DO_IT, x, y, range, proc, ', x, y, xlog=1'
DO_IT, x, y, range, proc, ', x, y, xlog=1, xtype=0'
DO_IT, x, y, range, proc, ', x, y, xlog=1, xtype=1'
;
end
;
pro TEST_PLOT_OO, test=test
;
save_p=!p
;
DEFSYSV, '!gdl', exists=exists
if exists then ttl='GDL : ' else ttl='IDL : '
;
device,/decomposed
;
CALL_X_PLOT, 'plot', winid=1, title=ttl+'X and PLOT'
CALL_X_PLOT, 'plot_io', winid=2, title=ttl+'X and PLOT_IO'
CALL_X_PLOT, 'plot_oi', winid=3, title=ttl+'X and PLOT_OI'
CALL_X_PLOT, 'plot_oo', winid=4, title=ttl+'X and PLOT_OO'
;
if KEYWORD_SET(test) then STOP
;
!p=save_p
;
end

