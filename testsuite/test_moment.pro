;
; AC 14-Oct-2010.
; A basic bug in MOMENT was reported by Christophe B.
; during students lesson ...
;
; ToDo: testing /NaN ?!
;
; Since 16 June 2012, we also have DIMENSION for MOMENT() (Mathieu Pinter)
; Since 14 July 2012, we also have DIMENSION for related codes:
;                     MEAN(), STDDEV(), VARIANCE(), SKEWNESS(), KURTOSIS()
;
; -------------------------------------------------------------
;
function ERREUR, x1, x2
return, SQRT(TOTAL((x1-x2)^2))
end
;
; -------------------------------------------------------------
;
function CHECK_DIMENSION_CAPABILITY, verbose=verbose
;
; IDL before version 8 cannot manage DIMENSION keyword
;
; Since 16 June 2012, we also have DIMENSION for MOMENT()
; Since 14 July 2012, we also have DIMENSION for related codes:
;    MEAN(), STDDEV(), VARIANCE(), SKEWNESS(), KURTOSIS()
;
if KEYWORD_SET(verbose) then begin
   txt1='dimension keyword is available in MOMENT() and related codes:'
   txt2='MEAN(), STDDEV(), VARIANCE(), SKEWNESS(), KURTOSIS()'
endif
;
DEFSYSV, '!gdl', exists=isGDL
;
check_dim=0
;
if (isGDL) then begin
   check_dim=1
   if KEYWORD_SET(verbose) then begin
      MESSAGE,/cont, 'in GDL, '+txt1
      MESSAGE,/cont, txt2
   endif
endif else begin
   ;; when IDL, which major version ? we assume "." is the separator !
   version=!version.release
   if (FIX(STRMID(version,0, STRPOS(version,'.'))) GE 8) then check_dim=1
   if KEYWORD_SET(verbose) then begin
      if (check_dim EQ 0) then begin
         MESSAGE,/cont, 'in IDL, before 8.0, dimension keyword not available !'
      endif else begin
         MESSAGE,/cont, 'in IDL, since 8.0, '+txt1
         MESSAGE,/cont, txt2
      endelse
   endif
endelse
;
return, check_dim
;
end
;
; -------------------------------------------------------------
;
pro CHECK_VISUAL_MOMENT, data, force_dim=force_dim, no_exit=no_exit, $
                         help=help, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro CHECK_VISUAL_MOMENT, data, force_dim=force_dim, no_exit=no_exit, $'
   print, '                         help=help, test=test, verbose=verbose'
   return
endif
;
; do we have the dimension keyword capability ?
;
if ~KEYWORD_SET(force_dim) then begin
   check_dim=CHECK_DIMENSION_CAPABILITY()
endif else check_dim=1
;
if (check_dim EQ 0) then begin
   MESSAGE, 'this test requires Dimension keyword capability'
endif
;
if N_PARAMS() EQ 0 then data=DIST(115, 128)
;
nb_nan=N_ELEMENTS(data)/10
pos=FIX(RANDOMU(seed,nb_nan)*N_ELEMENTS(data))
data_NaN=data
data_NaN[pos]=!values.f_nan
;
for ii=0,1 do begin
   if ii EQ 0 then begin
      tmp=data 
      title='no NaN'
   endif else begin
      tmp=data_NaN
      title='with 10% NaN'
   endelse
   WINDOW, ii, xsize=400, ysize=750, title=title
   !p.multi=[0,2,4]
   ;;
   plot, MEAN(tmp, dim=1,/nan), title='MEAN, dim=1'
   plot, MEAN(tmp, dim=2,/nan), title='MEAN, dim=1'
   plot, STDDEV(tmp, dim=1,/nan), title='STDDEV, dim=1'
   plot, STDDEV(tmp, dim=2,/nan), title='STDDEV, dim=2'
   plot, SKEWNESS(tmp, dim=1,/nan), title='SKEWNESS, dim=1'
   plot, SKEWNESS(tmp, dim=2,/nan), title='SKEWNESS, dim=2'
   plot, KURTOSIS(tmp, dim=1,/nan), title='KURTOSIS, dim=1'
   plot, KURTOSIS(tmp, dim=2,/nan), title='KURTOSIS, dim=2'
endfor
;
!p.multi=0
;
end
;
pro TEST_NUMERICAL_MOMENT, force_dim=force_dim, no_exit=no_exit, $
                           help=help, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_NUMERICAL_MOMENT, force_dim=force_dim, no_exit=no_exit, $'
    print, '                           help=help, test=test, verbose=verbose'
    return
endif
;
; do we have the dimension keyword capability ?
;
if ~KEYWORD_SET(force_dim) then begin
   check_dim=CHECK_DIMENSION_CAPABILITY(/verbose)
endif else check_dim=1
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
if (check_dim) then begin
    resu3=MOMENT(a, DIMENSION=3)
    e3=ERREUR(expected_resu3, resu3)
    if (e3 GT 1e-5) then nb_pb=nb_pb+1
endif else MESSAGE, /continue, 'skipping /DIM test'
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
if (check_dim) then begin
    resu5=MOMENT(a, DIMENSION=1, /NAN)
    e5=ERREUR(expected_resu5, resu5)
    if (e5 GT 1e-5) then nb_pb=nb_pb+1
endif else MESSAGE, /continue, 'skipping /DIM test'
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
;
; ------------------------------------
;
pro TEST_MOMENT, force_dim=force_dim, $
                 help=help, test=test, no_exit=no_exit, verbose=verbose
;
TEST_NUMERICAL_MOMENT, force_dim=force_dim, help=help, $
                       test=test, no_exit=no_exit, verbose=verbose
;
CHECK_VISUAL_MOMENT, force_dim=force_dim, help=help, $
                     test=test, no_exit=no_exit, verbose=verbose
;
end

