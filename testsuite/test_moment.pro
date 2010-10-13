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
pro TEST_MOMENT, test=test
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
if KEYWORD_SET(test) then STOP
;
if (nb_pb EQ 0) then begin 
   MESSAGE, 'No problem found in TEST_MOMENT', /continue
endif else begin
   MESSAGE, STRING(nb_pb)+' problem(s) found in TEST_MOMENT', /continue
   EXIT, status=1
endelse
;
end
