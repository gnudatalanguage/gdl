;
; Alain Coulais and Jingwei Wang, during the 2022 in Paris.
; Copyright 2022. 
;
; * TEST_LEGENDRE
;
;
;
; -----------------------------------------------
;
pro TEST_LEGENDRE, help=help, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_LEGENDRE, nb_pbs_locale, help=help, test=test, verbose=verbose'
   print, ''
   return
endif
;
nb_pbs_legendre=0
;
x3 = [0.3,0.6,0.9]
l3 = [3,6,9]
m3 = [1,2,3]
x2 = [0.3,0.6]
l2 = [3,6]
m2 = [1,2]
x1 = [0.3]
l1 = [3]
m1 = [1]
x0 = 0.3
l0 = 3
m0 = 1
;
resu_dim3 = LEGENDRE(x3, l3, m3)
resu_dim3_x0 = LEGENDRE(x0, l3, m3)
resu_dim3_l0= LEGENDRE(x3, l0, m3)
resu_dim3_m0 = LEGENDRE(x3, l3, m0)
resu_dim2_x2 = LEGENDRE(x2, l3, m3)
resu_dim2_l2 = LEGENDRE(x3, l2, m3)
resu_dim2_m2 = LEGENDRE(x3, l3, m2)
resu_dim2_x0 = LEGENDRE(x0, l2, m3)
resu_dim2_l0 = LEGENDRE(x3, l0, m2)
resu_dim2_m0 = LEGENDRE(x2, l3, m0)
resu_dim1_x1 = LEGENDRE(x1, l3, m3)
resu_dim1_l1 = LEGENDRE(x3, l1, m3)
resu_dim1_m1 = LEGENDRE(x3, l3, m1)
resu_dim1 = LEGENDRE(x0, l0, m0)
;
resu_expected_dim3 = [0.78699982,-10.106877,-360.48489]
resu_expected_dim3_x0 = [0.78699982,-4.2125626,-205.21660]
resu_expected_dim3_l0 = [0.78699982,5.7599993,-1.2422863]
resu_expected_dim3_m0 = [0.78699982,1.9192321,1.5924634]
resu_expected_dim2 = [0.78699982,-10.106877]
resu_expected_dim2_x0 = [0.78699982,-4.2125626]
resu_expected_dim2_l0 = [0.78699982,5.7599993]
resu_expected_dim2_m0 = [0.78699982,1.9192321]
resu_expected_dim1 = [0.78699982]
;
; test dimension
if (N_elements(resu_dim3) NE 3) then ERRORS_ADD, nb_pbs_legendre,'case dimension'
if (N_elements(resu_dim3_x0) NE 3) then ERRORS_ADD, nb_pbs_legendre,'case dimension x0'
if (N_elements(resu_dim3_l0) NE 3) then ERRORS_ADD, nb_pbs_legendre,'case dimension l0'  
if (N_elements(resu_dim3_m0) NE 3) then ERRORS_ADD, nb_pbs_legendre,'case dimension m0'
;
if (N_elements(resu_dim2_x2) NE 2) then ERRORS_ADD, nb_pbs_legendre,'case dimension x2'
if (N_elements(resu_dim2_l2) NE 2) then ERRORS_ADD, nb_pbs_legendre,'case dimension l2'  
if (N_elements(resu_dim2_m2) NE 2) then ERRORS_ADD, nb_pbs_legendre,'case dimension m2'
if (N_elements(resu_dim2_x0) NE 2) then ERRORS_ADD, nb_pbs_legendre,'case dimension x0 l2'
if (N_elements(resu_dim2_l0) NE 2) then ERRORS_ADD, nb_pbs_legendre,'case dimension l0 m2'  
if (N_elements(resu_dim2_m0) NE 2) then ERRORS_ADD, nb_pbs_legendre,'case dimension m0 x2'
;
if (N_elements(resu_dim1_x1) NE 1) then ERRORS_ADD, nb_pbs_legendre,'case dimension x1'
if (N_elements(resu_dim1_l1) NE 1) then ERRORS_ADD, nb_pbs_legendre,'case dimension l1'
if (N_elements(resu_dim1_m1) NE 1) then ERRORS_ADD, nb_pbs_legendre,'case dimension m1'
if (N_elements(resu_dim1) NE 1) then ERRORS_ADD, nb_pbs_legendre,'case dimension x0 l0 m0'
;
eps=1.e-3
;
for ii=0, 2 do begin 
      if (ABS(resu_dim3[ii]-resu_expected_dim3[ii]) GT eps) then $
            ERRORS_ADD, nb_pbs_legendre, 'case normal, case : '+string(ii)
      if (ABS(resu_dim3_x0[ii]-resu_expected_dim3_x0[ii]) GT eps) then $
            ERRORS_ADD, nb_pbs_legendre, 'case x0, l3, m3, case : '+string(ii)
      if (ABS(resu_dim3_l0[ii]-resu_expected_dim3_l0[ii]) GT eps) then $
            ERRORS_ADD, nb_pbs_legendre, 'case x3, l0, m3, case : '+string(ii)
      if (ABS(resu_dim3_m0[ii]-resu_expected_dim3_m0[ii]) GT eps) then $
            ERRORS_ADD, nb_pbs_legendre, 'case x3, l3, m0, case : '+string(ii)
endfor
;
;
for ii=0, 1 do begin 
      if (ABS(resu_dim2_x2[ii]-resu_expected_dim2[ii]) GT eps) then $
            ERRORS_ADD, nb_pbs_legendre, 'case x2,l3,m3, case : '+string(ii)
      if (ABS(resu_dim2_l2[ii]-resu_expected_dim2[ii]) GT eps) then $
            ERRORS_ADD, nb_pbs_legendre, 'case x3,l2,m3, case : '+string(ii)
      if (ABS(resu_dim2_m2[ii]-resu_expected_dim2[ii]) GT eps) then $
            ERRORS_ADD, nb_pbs_legendre, 'case x3,l3,m2, case : '+string(ii)
      if (ABS(resu_dim2_x0[ii]-resu_expected_dim2_x0[ii]) GT eps) then $
            ERRORS_ADD, nb_pbs_legendre, 'case x0,l2,m3, case : '+string(ii)
      if (ABS(resu_dim2_l0[ii]-resu_expected_dim2_l0[ii]) GT eps) then $
            ERRORS_ADD, nb_pbs_legendre, 'case x3,l0,m2, case : '+string(ii)
      if (ABS(resu_dim2_m0[ii]-resu_expected_dim2_m0[ii]) GT eps) then $
            ERRORS_ADD, nb_pbs_legendre, 'case x2,l3,m0, case : '+string(ii)
endfor
;
;
; for dim 1
if (ABS(resu_dim1_x1-resu_expected_dim1) GT eps) then $
      ERRORS_ADD, nb_pbs_legendre, 'case x1,l3,m3'
if (ABS(resu_dim1_l1-resu_expected_dim1) GT eps) then $
      ERRORS_ADD, nb_pbs_legendre, 'case x3,l1,m3'
if (ABS(resu_dim1_m1-resu_expected_dim1) GT eps) then $
      ERRORS_ADD, nb_pbs_legendre, 'case x3,l3,m1'
if (ABS(resu_dim1-resu_expected_dim1) GT eps) then $
      ERRORS_ADD, nb_pbs_legendre, 'case x0,l0,m0'

;if KEYWORD_SET(verbose) then begin
 ;  print, 'first two Lines should be equal, except may be in the "second" value !' 
 ;  print, 'resu_GDL     : ', resu_GDL
 ;  print, 'resu_LC_ALL  : ', resu_LC_ALL
 ;  print, 'resu (no LC) : ', resu
;endif
;
;
BANNER_FOR_TESTSUITE, "TEST_LEGENDRE", nb_pbs_legendre, /short, verb=verbose
;
;
if KEYWORD_SET(test) then STOP
;
;
end 
