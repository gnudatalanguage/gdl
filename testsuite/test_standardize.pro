;
; Alain Coulais, 13 May 2012, under GNU GPL v2 or later
;
; a very basic test for STANDARDIZE
;
; after STANDARDIZE, we check that:
; -- values in first column are very close to Zero
; -- values in second column are very close to One
; -- values in third and fourth column are close to initial values
;
; ------------------------------------------
; Modification history:
;
; 2025-04-23 : AC going to standard way in 2025 ! (ERRORS_ADD, ...)
;
; ------------------------------------------
;
pro TEST_STANDARDIZE_BASIC, cumul_errors, $
                            liste_var=liste_var, nb_var=nb_var, nb_obs=nb_obs, $
                            double=double, test=test, verbose=verbose
;
nb_pb=0
;
if ~KEYWORD_SET(liste_var) then liste_var=[-100., -10., 1e-3, 25., 1e3]
;
if N_ELEMENTS(nb_var) EQ 0 then nb_var=N_ELEMENTS(liste_var)
;
if N_ELEMENTS(nb_obs) EQ 0 then nb_obs=1000
;
data=FLTARR(nb_var,nb_obs)
;
for ii=0, nb_var-1 do begin
   data[ii,*]=REPLICATE(liste_var[ii],nb_obs)+RANDOMN(seed,nb_obs)
endfor
;
; Compute the mean and variance of each variable using the MOMENT   
; function. The skewness and kurtosis are also computed:
;
raw_res=FLTARR(4,nb_var)
std_res=FLTARR(4,nb_var)
;
for ii=0, nb_var-1 do raw_res[*,ii]=MOMENT(data[ii,*])  
  
; Compute the standardized variables:  
std_data = STANDARDIZE(data, double=double)  
  
; Compute the mean and variance of each standardized variable using   
; the MOMENT function. The skewness and kurtosis are also computed:  
for ii=0, nb_var-1 do std_res[*,ii]=MOMENT(std_data[ii,*])  
;
; now we can check the results 
; (column "0" must be close to 0, column "0" must be equal to 1,
; other 2 columns should remain the sames)
;
total_pb=0
error=1e-2
;
; first column must be close to Zero
pb=WHERE(ABS(std_res[0,*]) GT error, nb_pb)
if nb_pb GT 0 then ERRORS_ADD, 'pb with Zero''s', nb_pb
;
; second column must be close to One
pb=WHERE(ABS(std_res[1,*]-1.0) GT error, nb_pb)
if nb_pb GT 0 then ERRORS_ADD, 'pb with One''s', nb_pb
;
; third and fourth columns must remain equal
pb=WHERE(ABS(std_res[2,*]-raw_res[2,*]) GT error, nb_pb)
if nb_pb GT 0 then ERRORS_ADD, 'pb with Two''s', nb_pb

pb=WHERE(ABS(std_res[3,*]-raw_res[3,*]) GT error, nb_pb)
if nb_pb GT 0 then ERRORS_ADD, 'pb with Three''s', nb_pb
;
if KEYWORD_SET(verbose) then begin
   print, "Stats on input array:"
   print, raw_res
   print, "Stats on STANDARDIZEd array:"
   print, std_res
endif
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_STANDARDIZE_BASIC", nb_pb, /short
ERRORS_CUMUL, cumul_errors, nb_pb
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------
;
pro TEST_STANDARDIZE, verbose=verbose, no_exit=no_exit, $
                      test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_STANDARDIZE, verbose=verbose, no_exit=no_exit, $'
   print, '                      test=test, help=help'
   return
endif
;
cumul_errors=0
;
TEST_STANDARDIZE_BASIC, cumul_errors, verbose=verbose
TEST_STANDARDIZE_BASIC, cumul_errors, verbose=verbose, /double
;
liste_var=[-100., -10., 1e-3, 25., 1e3]
TEST_STANDARDIZE_BASIC, cumul_errors, liste_var=liste_var
;
TEST_STANDARDIZE_BASIC, cumul_errors, nb_obs=10
TEST_STANDARDIZE_BASIC, cumul_errors, nb_obs=10, /double
;
TEST_STANDARDIZE_BASIC, cumul_errors, nb_obs=100
TEST_STANDARDIZE_BASIC, cumul_errors, nb_obs=100, /double
;
; ----------------- final MESSAGE ----------
;
BANNER_FOR_TESTSUITE, 'TEST_STANDARDIZE', cumul_errors
;
if (cumul_errors gt 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
if KEYWORD_SET(test) then stop
;
end
