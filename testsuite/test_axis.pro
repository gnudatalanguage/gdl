;
; Alain Coulais
; Distributed version 2008/02/07
; Under GNU GPL
;
; Purpose: quickly check AXIS capabilities ...
; Some options are not effective now but should be fixed soon
; (tickness, ticklen, ...)
;
; --------------------
; FYI, SUBTIL_AXIS is not wroking as expected now ...
;
pro SUBTIL_AXIS
plot,findgen(10)
wdelete
axis
axis,/yax
axis,0,/yax
end
;
pro AXIS_COMPLEX
device,/dec
u=findgen(10)
plot, u-5, u^2-10.
axis, 2, col='ff'x,/ya
axis, 0, 7.5, col='ff00ff'x, xrange=[1,2]
end
;
pro AXIS_RANGE
;
device,/dec
;
plot, findgen(10)
;
axis, 5, /ya, col='ff'x
axis, 7, /ya, col='ff'x, yrange=[0,2]
axis, 5.5, 0., /ya, col='ff'x
axis, 7.5, 0., /ya, col='ff'x, yrange=[0,2]
;
axis, 5, col='ff00'x
axis, 7, col='ff00'x, xrange=[0,2]
axis, 0, 5.5, col='ff00'x
axis, 0, 7.5, col='ff00'x, xrange=[0,2]
;
oplot, 10-findgen(10)
;
end
;
pro AXIS_BASIC

device,/decomposed

!p.multi=[0,2,2]

plot, findgen(10), title='overwriting X axis'
axis, col='ff'x, xthick=2
oplot, 10-findgen(10)

plot, findgen(10), title='X axis at Y=5'
axis, 5,5,/xax, col='ff'x
oplot, 10-findgen(10)


plot, findgen(10), title='X axis at Y=1, Yaxis at x=1'
axis, 1, col='ff00'x
axis, 1, 0, /yax, col='ff00'x
oplot, 10-findgen(10)


contour, dist(25),/xstyl,/ystyle
axis, 12.5, xthick=2
axis, 12.5, 0, /yaxis, ythick=2, col='ff00'x
axis, 12.5, 0, /yaxis, ythick=2
oplot, 25-findgen(25)

!p.multi=0

end
;
pro TEST_AXIS
;
WINDOW, 0
AXIS_BASIC
;
WINDOW, 1
AXIS_RANGE
;
WINDOW, 2
AXIS_COMPLEX
;
s=''
READ, 'Please type any key to continue', s
WDELETE, 0, 1, 2

end
