;; NAME:
;; L_GetDim
;;
;; PURPOSE:
;; translates a one-dimensional index (like given by where() function)
;; into a multidimensional one (i.e. the array indices according to
;; the multidimensional array)
;;
;; PARAMETERS:
;; a   the array
;; ix  the onedimensional index (or array of indices)
;; if ix is omitted, the eight dimensions of a are returned
;;
;; KEYWORDS:
;; MINDIM  if set, only the number of dimensions of a is returned,
;;         else 8 dimensions (what is better in some degenerated
;;         cases, i.e the calling program can rely on that there is
;;         always a second(third...) dimension given)
;;
;; returns a 8 by n_elements(ix) array
;;
;; example:
;;IDL> a=intarr(23,24,27,33)
;;IDL> a[13,19,2,11]=1 
;;IDL> ix=where(a) 
;;IDL> print,l_getdim(a,ix)
;;          13          19           2          11           0           0           0           0 
;;IDL> print,l_getdim(a)
;;          23          24          27          33           1           1           1           1  
;;
;; MODIFICATION HISTORY:
;; Marc Schellens 01.2002



function L_GetDim,a,ix,MINDIM=minDim

sz=size(a)

if n_params() eq 1 then begin
    ;; maximum of eight dimensions in IDL
    if keyword_set(minDim) then return,size(a,/dim)
    r=lonarr(8)
    r[*]=1
    if sz[0] ge 1 then r[0:sz[0]-1]=sz[1:sz[0]]
    return,r
endif

nConv=n_elements(ix)

;; maximum of eight dimensions in IDL
r=lonarr(keyword_set(minDim)?sz[0]>1:8,nConv)

;; index 1
r[0,*]=ix mod sz[1]

;; index 2..n-1
sum=1L
for i=1,sz[0]-2 do begin
    sum=sum*sz[i]
    r[i,*]=(ix / sum) mod sz[i+1]
endfor

;; index n
if sz[0] ge 2 then begin
    i=sz[0]-1

    sum=sum*sz[i]
    r[i,*]=ix / sum
endif 

return,r
end



