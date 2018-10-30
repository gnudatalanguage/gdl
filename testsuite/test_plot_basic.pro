;
; revised by AC, 24-Oct-2018
;
; This is a visual inspection only
; we do have some descrepencies :(
;
pro TEST_AUTOSCALE
;
a=findgen(20)
;
!p.multi=[0,2,2]
plot, a, title='positive only'
plot, a-4, title='positive and negative'
plot, a+4, title='positive only, no fit'
plot, -a, title='negative only, fit'
!p.multi=0
end
;
pro TEST_YNOZERO
;
a=findgen(20)
;
!p.multi=[0,2,2]
plot, a+10, title='positive only, no fit'
plot, a+10, /ynozero, title='positive + /Ynozero'
plot, a-40, title='negative only, no fit'
plot, a-40, title='negative only,  + /Ynozero'
!p.multi=0
;
end
;
pro TEST_ALL_EQUAL, scale=scale
;
if N_ELEMENTS(scale) EQ 0 then scale=1.
;
!p.multi=[0,2,2]
a=replicate(1., 5)*scale
plot,a, title='default positive'
plot, a, /ynozero, title='positive and /Ynozero'
;
plot,-a, title='default negative'
plot, -a, /ynozero, title='negative and /Ynozero'
;
!p.multi=0
end
;
pro CUSTOM_WINDOW_DIM, xsize, ysize
;
xy=GET_SCREEN_SIZE()
xsize=FIX(xy[0]*0.45)
ysize=FIX(xy[1]*0.45)
;
end
;
pro TEST_PLOT_BASIC, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_PLOT_BASIC, help=help, test=test, no_exit=no_exit'
   return
endif
;
rname=ROUTINE_NAME()
;
if ~CHECK_IF_DEVICE_IS_OK(rname, /force) then begin
   if ~KEYWORD_SET(no_exit) then EXIT, status=77 else STOP
endif
;
version=GDL_IDL_FL()
txt=version+' : '
;
device, /decomposed
ref_p=!p
if version EQ 'FL' then !p.background=COLOR2COLOR(!color.purple)
if version EQ 'IDL' then !p.background=COLOR2COLOR(!color.maroon)
if version EQ 'GDL' then !p.background=COLOR2COLOR(!color.black)
;
CUSTOM_WINDOW_DIM, xsize, ysize
;
WINDOW, 0, xsize=xsize, ysize=ysize, title=txt+'basic plotting tests'
TEST_AUTOSCALE
;
WINDOW, 1, xsize=xsize, ysize=ysize, title=txt+'/Ynozero plotting tests'
TEST_YNOZERO
;
WINDOW, 2, xsize=xsize, ysize=ysize, title=txt+'Equal data and /ynozero (scale 1)'
TEST_ALL_EQUAL
;
WINDOW, 3, xsize=xsize, ysize=ysize, title=txt+'Equal data and /ynozero (scale 1e-12)'
TEST_ALL_EQUAL, scale=1.e-12
;
!p=ref_p
;
if KEYWORD_SET(no_exit) OR KEYWORD_SET(test) then STOP
;
end
