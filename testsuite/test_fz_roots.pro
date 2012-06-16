;
; under GNU GPL v2 or later
; Benjamin Laurent, 15-Juin-2012
; 
; -------------------------------------------------
;
; to be extended ...
;
; -------------------------------------------------
;
;usage:
;GDL> test_fz_roots

function poly, m, c , r , index, double;Return the maximum error between 'exact' roots and fz_roots 
  resp=0
  resn=0
  epsref=1e-6
  s=fz_roots(*c[m],/double)  
  ;Sorting, index gives the relation
  ;between 'exact' roots and fz_roots (cause to different orders...)
  for j = 0,n_elements(*r[m])-1 do begin
     for i = 0,n_elements(*r[m])-1 do begin
        if ( abs(s[j]-(*r[m])[i]) lt epsref ) then begin
           (*index[m])[j]=i
        endif
     endfor
  endfor
  
  for j = 0,n_elements(*r[m])-1 do begin
     resn=abs(s[j]-(*r[m])[(*index[m])[j]])
     if (resn GT resp) then begin resp = resn
     endif
  endfor
  return, resp
end

pro TEST_FZ_ROOTS_ON_POLY, numberpoly=numberpoly, eps=eps, help=help, test=test , c=c, r=r, index=index, double=double

  if KEYWORD_SET(help) then begin
     print, 'pro TEST_FZ_ROOTS_POLY, numberpoly=m, eps=eps, help=help, test=test, c=c, r=r, index=index, double=double'
     return
  endif

;Error tolerance
  if (N_ELEMENTS(eps) EQ 0) then begin eps=1e-6
  endif
  if (N_ELEMENTS(numberpoly) EQ 0) then begin m=0
  endif
  nb_errors=0

  resuFZ=poly(numberpoly,c,r,index,double);maximum error

  if resuFZ GT eps then begin nb_errors=nb_errors+1 ;comparing
  endif

  if (nb_errors GT 0) then begin
     MESSAGE, /continue, STRING(nb_errors)+' Errors founded'
     if ~KEYWORD_SET(test) then EXIT, status=1
  endif else begin
     MESSAGE, /continue, 'Polynomial '+string(format='(I1)',numberpoly)+' : No Errors founded'
  endelse


;
  if KEYWORD_SET(test) then begin STOP
  endif
;
end



pro TEST_FZ_ROOTS

n=5;number of polynomials
c=ptrarr(n);polynomial coefficients
r=ptrarr(n);polynomial roots

c[0]=ptr_new([-1,0,0,0,0,1])
r[0]=ptr_new(exp([0,2,4,6,8] * !PI * complex(0,1) / 5))

c[1]=ptr_new([-1,0,0,0,0,1])
r[1]=ptr_new(exp([0,2,4,6,8] * !PI * complex(0,1) / 5))

; IDL-doc example (IMSL_ZEROPOLY)
c[2] = ptr_new([-2, 4, -3, 1])
r[2] = ptr_new([1, complex(1,-1), complex(1,1)])

; IDL-doc examples (FZ_ROOTS)
c[3] = ptr_new([-2, -9, -7, 6])
r[3] = ptr_new([-.5, -1./3, 2.])

c[4] = ptr_new([2, 0, 3, 0, 1])
r[4] = ptr_new(complex(0,[-sqrt(2), sqrt(2), -1, 1]))

;c[5]=ptr_new([1]);To check error handling of degree zero polynomial
;c[6]=ptr_new([complex(0,1),0,0,1]);To check error handling of complex coefficients polynomial
   
index=ptrarr(5)
index[0]=ptr_new([0,0,0,0,0])
index[1]=ptr_new([0,0,0,0,0])
index[2]=ptr_new([0,0,0])
index[3]=ptr_new([0,0,0])
index[4]=ptr_new([0,0,0,0])

TEST_FZ_ROOTS_ON_POLY, numberpoly=0, c=c, r=r, index=index, double=double
TEST_FZ_ROOTS_ON_POLY, numberpoly=1, c=c, r=r, index=index, double=double
TEST_FZ_ROOTS_ON_POLY, numberpoly=2, c=c, r=r, index=index, double=double
TEST_FZ_ROOTS_ON_POLY, numberpoly=3, c=c, r=r, index=index, double=double
TEST_FZ_ROOTS_ON_POLY, numberpoly=4, c=c, r=r, index=index, double=double
end




