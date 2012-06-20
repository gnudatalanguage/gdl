;
; AC 14-Oct-2010.
; A basic bug in MOMENT was reported by Christophe B.
; during students lesson ...
;
; ToDo: testing /NaN ?!
;
function ERREUR, x1, x2
return, SQRT(TOTAL((x1-x2)^2))
end
;
pro TEST_MOMENT, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_MOMENT, help=help, test=test, no_exit=no_exit, verbose=verbose'
    return
endif
;
nb_pb=0
;
a=FINDGEN(100)/10.
expected_resu1=[4.95,8.41667,0.0,-1.23605]
resu1=MOMENT(a)
e1=ERREUR(expected_resu1, resu1)
if (e1 GT 1e-5) then nb_pb=nb_pb+1
;
; -----
;
a=((FINDGEN(100)-20)^2-20)/10000.
expected_resu2=[0.168350,0.0349073,0.929600,-0.457650]
resu2=MOMENT(a)
e2=ERREUR(expected_resu2, resu2)
if (e2 GT 1e-5) then nb_pb=nb_pb+1
;
; -----
;
a=FINDGEN(3, 2, 3)^2
expected_resu3=[[[60.0000, 73.0000, 88.0000], $
                 [105.000, 124.000, 145.000]], $
                [[5616.00, 7488.00, 9648.00], $
                 [12096.0, 14832.0, 17856.0]], $
                [[0.287409, 0.256015, 0.229751], $
                 [0.207827, 0.189413, 0.173812]], $
                [[-2.33333, -2.33333, -2.33333], $
                 [-2.33333, -2.33333, -2.33333]]]
resu3=MOMENT(a, DIMENSION=3)
e3=ERREUR(expected_resu3, resu3)
if (e3 GT 1e-5) then nb_pb=nb_pb+1
;
; -----
;
a=[1,4,5,!VALUES.F_NAN]
expected_resu4=[3.33333, 4.33333, -0.28741, -2.33333]
resu4=MOMENT(a, /NAN)
e4=ERREUR(expected_resu4, resu4)
if (e4 GT 1e-5) then nb_pb=nb_pb+1
;
; -----
;
a=[[1,4,5,!VALUES.F_NAN],[6,8,!VALUES.F_NAN, 9]]
expected_resu5=[[3.33333,7.66667], $
                [4.33333,2.33333],[-0.28741,-0.20783], $
                [-2.33333,-2.33333]]
resu5=MOMENT(a, DIMENSION=1, /NAN)
e5=ERREUR(expected_resu5, resu5)
if (e5 GT 1e-5) then nb_pb=nb_pb+1
;
; -----------------
;
if (nb_pb EQ 0) then begin 
   MESSAGE, 'No problem found in TEST_MOMENT', /continue
endif else begin
   MESSAGE, STRING(nb_pb)+' problem(s) found in TEST_MOMENT', /continue
endelse
;
if KEYWORD_SET(test) then STOP
;
if (nb_pb GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
