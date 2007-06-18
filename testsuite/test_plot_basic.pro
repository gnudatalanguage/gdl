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

pro TEST_PLOT_BASIC
;
window, 0, title='basic plotting tests'
TEST_AUTOSCALE
;
window, 1, title='/Ynozero plotting tests'
TEST_YNOZERO
;
window, 2, title='Equal data and /ynozero (scale 1)'
TEST_ALL_EQUAL
window, 3, title='Equal data and /ynozero (scale 1e-12)'
TEST_ALL_EQUAL, scale=1.e-12
;
end


