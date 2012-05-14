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
pro TEST_STANDARDIZE, liste_var, nb_var=nb_var, nb_obs=nb_obs, $
                      no_exit=no_exit, double=double, $
                      test=test, help=help, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_STANDARDIZE, liste_var, nb_var=nb_var, nb_obs=nb_obs, $'
    print, '                      no_exit=no_exit, double=double, $'
    print, '                      test=test, help=help, verbose=verbose'
    return
endif
;
if N_PARAMS() EQ 0 then liste_var=[-100., -10., 1e-3, 25., 1e3]
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
; first column must be close to Zero
pb=WHERE(ABS(std_res[0,*]) GT error, nb_pb)
if nb_pb GT 0 then begin
   if KEYWORD_SET(verbose) then MESSAGE,/continue, 'pb with Zero''s'
   total_pb=total_pb+nb_pb
endif
; second column must be close to One
pb=WHERE(ABS(std_res[1,*]-1.0) GT error, nb_pb)
if nb_pb GT 0 then begin
   if KEYWORD_SET(verbose) then MESSAGE,/continue, 'pb with One''s'
   total_pb=total_pb+nb_pb
endif
; third and fourth columns must remain equal
pb=WHERE(ABS(std_res[2,*]-raw_res[2,*]) GT error, nb_pb)
if nb_pb GT 0 then begin
   if KEYWORD_SET(verbose) then MESSAGE,/continue, 'pb with Two''s'
   total_pb=total_pb+nb_pb
endif
pb=WHERE(ABS(std_res[3,*]-raw_res[3,*]) GT error, nb_pb)
if nb_pb GT 0 then begin
   if KEYWORD_SET(verbose) then MESSAGE,/continue, 'pb with Three''s'
   total_pb=total_pb+nb_pb
endif
;
if KEYWORD_SET(verbose) then begin
   print, "Stats on input array:"
   print, raw_res
   print, "Stats on STANDARDIZEd array:"
   print, std_res
endif
;
if KEYWORD_SET(test) then STOP
;
if (total_pb GT 0) then begin
    if KEYWORD_SET(no_exit) then begin
        MESSAGE, /continue, ' tests failed (but base on RANDOMN ...'
    endif else begin
        EXIT, status=1
    endelse
endif else begin
    MESSAGE,/continue, 'tests successful'
endelse
;
end

