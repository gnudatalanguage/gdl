;
; AC, 17 Aout 2015, Shizuoka
;
; In fact, this code is testing background value in WINDOW via TVRD
; and ERASE.
; Then, it may be broken if WINDOW, ERASE or TVRD became broken !!
;
; Note: the tests clearly show problems for old GDL on
; device, /decomposed, but no warning for device, decomposed=0
;
; -----------------------------------------------
;
pro ADD_ERROR, nb_errors, message
;
print, 'Error on operation : '+message
nb_errors=nb_errors+1
;
end
;
; -----------------------------------------------
;
function MINIMAXI, array, channel, verbose=verbose, test=test
;
if (n_params() EQ 1) then res=[MIN(array),MAX(array)]
if (n_params() EQ 2) then res=[MIN(array[channel,*,*]),MAX(array[channel,*,*])]
;
if KEYWORD_SET(verbose) then $
   print, 'Min in Array : ', res[0], ', Max in Array : ', res[1]
;
if KEYWORD_SET(test) then STOP
;
return, res
end
;
; -----------------------------------------------
;
pro TEST_WIN_BACK_DECOMPOSED, cumul_errors, test=test, verbose=verbose
;
errors=0
;
DEVICE, /decomposed
;
!P.background='ff'x
back_ref=!P.background
back_ref_mod=back_ref 
;
WINDOW, 0, xsize=100, ysize=100
;
resu=TVRD(/true)
mm=MINIMAXI(resu,0)
if mm[0] NE mm[1] then ADD_ERROR, errors, '(1) more than one value in plane RED !'
if mm[0] NE back_ref then ADD_ERROR, errors, '(1) read value not EQ to back'
mm=MINIMAXI(resu,1)
if mm[0] NE mm[1] then ADD_ERROR, errors, '(1) more than one value in plane GREEN !'
mm=MINIMAXI(resu,2)
if mm[0] NE mm[1] then ADD_ERROR, errors, '(1) more than one value in plane BLUE !'
;
; Since Background color not change,
; result should be the same after an ERASE !!
;
erase
resu=TVRD(/true)
mm=MINIMAXI(resu,0)
if mm[0] NE mm[1] then ADD_ERROR, errors, '(2) more than one value !'
if mm[0] NE back_ref then ADD_ERROR, errors, '(2) read value not EQ to back'
;
; changing the back via ERASE
;
back_new='ee00'x
back_new_mod=back_new/256
erase, color=back_new
resu=TVRD(/true)
mm=MINIMAXI(resu, 1)
if mm[0] NE mm[1] then ADD_ERROR, errors, '(3) more than one value !'
if mm[0] NE back_new_mod then ADD_ERROR, errors, '(3) read value not EQ to back'
;
; change the back via !p.back
;
!P.background=back_new
WINDOW, xsize=100, ysize=100
resu=TVRD(/true)
mm=MINIMAXI(resu,1)
if mm[0] NE mm[1] then ADD_ERROR, errors, '(4) more than one value !'
if mm[0] NE back_new_mod then ADD_ERROR, errors, '(4) read value not EQ to back'
;
; one more window
;
!P.background=back_new
WINDOW, /free, xsize=100, ysize=100
resu=TVRD(/true)
mm=MINIMAXI(resu,1)
if mm[0] NE mm[1] then ADD_ERROR, errors, '(5) more than one value !'
if mm[0] NE back_new_mod then ADD_ERROR, errors, '(5) read value not EQ to back'
;
; one more window
;
!P.background=back_ref
WINDOW, /free, xsize=100, ysize=100
resu=TVRD(/true)
mm=MINIMAXI(resu,0)
if mm[0] NE mm[1] then ADD_ERROR, errors, '(6) more than one value !'
if mm[0] NE back_ref_mod then ADD_ERROR, errors, '(6) read value not EQ to back'
;
; back to first window
;
!P.background=back_ref
WSET, 0
ERASE
;WINDOW, /free, xsize=100, ysize=100
RESU=TVRD(/true)
mm=MINIMAXI(resu,0)
if mm[0] NE mm[1] then ADD_ERROR, errors, '(7) more than one value !'
if mm[0] NE back_ref_mod then ADD_ERROR, errors, '(7) read value not EQ to back'
;
; final message
;
if ~KEYWORD_SET(verbose) then short=1 else short=0
BANNER_FOR_TESTSUITE, 'TEST_WIN_BACK_DECOMPOSED', errors, short=short
;
if ~ISA(cumul_errors) then cumul_errors=0
cumul_errors=cumul_errors+errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
; This example was provided by Eric Hivon, HEAPix
; (see mails 2015/08/12)
; the color map is changed to setup RED for color "0",
; background is white.
;
pro TEST_RED_WHITE_NO_DECOMPOSED, cumul_errors, test=test, verbose=verbose
;
errors=0
;
DEVICE, decomposed=0
LOADCT,0
TVLCT, /get, rgb
back=255B
rgb[0,*] = [back,0,0]
rgb[1,*] = [0,0,back]
TVLCT, rgb 
;
WINDOW, /free, xsize=250, ysize=200
PLOT, FINDGEN(10), back=255, col=0, title='RED on WHITE'
;
; some "no trivial" tests can be made !
;
resu=TVRD()
mm=MINIMAXI(resu)
if mm[0] NE mm[1] then ADD_ERROR, errors, '(RW) more than one value for plane 0 !'
if mm[0] NE back then ADD_ERROR, errors, '(RW) wrong value for Back !'
;
resu=TVRD(/true)
;
max_diff=MAX(ABS(resu(1,*,*)-resu(2,*,*)))
if (max_diff GT 0) then ADD_ERROR, errors, 'something''s wrong in planes 1-2'
;
; final message
;
if ~KEYWORD_SET(verbose) then short=1 else short=0
BANNER_FOR_TESTSUITE, 'TEST_RED_WHITE_NO_DECOMPOSED', errors, short=short
;
if ~ISA(cumul_errors) then cumul_errors=0
cumul_errors=cumul_errors+errors
;
end
;
; ----------------------------------------------------------------------
;
pro TEST_WIN_BACK_NO_DECOMPOSED, cumul_errors, test=test, verbose=verbose
;
print, 'this case is not finished'
;
errors=0
;
DEVICE, decomposed=0
LOADCT,0
back=255B
!p.background=back
;
plot, findgen(10)
;
; final message
;
if ~KEYWORD_SET(verbose) then short=1 else short=0
BANNER_FOR_TESTSUITE, 'TEST_RED_WHITE_NO_DECOMPOSED', errors, short=short
;
if ~ISA(cumul_errors) then cumul_errors=0
cumul_errors=cumul_errors+errors
;
end
;
; -----------------------------------------------
;
pro TEST_WINDOW_BACKGROUND, short=short, verbose=verbose, debug=debug, $
                            test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_WINDOW_BACKGROUND, short=short, verbose=verbose, debug=debug, $'
    print, '                            test=test, no_exit=no_exit'
    return
endif
;
cumul_errors=0
;
TEST_WIN_BACK_DECOMPOSED, cumul_errors, test=test, verbose=verbose
TEST_WIN_BACK_NO_DECOMPOSED, cumul_errors, test=test, verbose=verbose
TEST_RED_WHITE_NO_DECOMPOSED, cumul_errors, test=test, verbose=verbose
;
BANNER_FOR_TESTSUITE, 'TEST_WINDOW_BACKGROUND', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

