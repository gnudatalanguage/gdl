;
; limited version of DIAG_MATRIX
; please report problems / examples / extensions
;
; Alain C., 23-JAN-2012, under GNU GPL v2 or later
;
function DIAG_MATRIX, input, position, $
                      debug=debug, help=help, test=test
;
if N_PARAMS() EQ 0 then MESSAGE, 'Incorrect number of arguments.'
;
if KEYWORD_SET(help) then begin
   print, 'function DIAG_MATRIX, input, position, $'
   print, '                      debug=debug, help=help, test=test'
   return, -1
endif
;
if SIZE(input,/n_dim) GT 2 then begin
   MESSAGE, 'Only 1 or 2 dimensions allowed '+input
endif
;
if (SIZE(input,/n_dim) EQ 2) then begin
   if N_PARAMS() EQ 1 then position=0
   info_size=SIZE(input,/dim)
   x=info_size[0]
   y=info_size[1]
   ;;   
   diag=INDGEN(MIN([x,y]))
   ;;
   if (position EQ 0) then begin
      xx=diag
      yy=diag
   endif
   ;;
   if (position GT 0) then begin
      if (position GE x) then begin
         MESSAGE, '% Specified offset to array is out of range: '+STRING(position)+' versus: '+STRING(x)
      endif
      xx=diag+position
      xx=xx[WHERE(xx LT x)]
      yy=diag[0:N_ELEMENTS(xx)-1]
   endif  
   if (position LT 0) then begin
      if (ABS(position) GE y) then begin
         MESSAGE, '% Specified offset to array is out of range: '+STRING(position)+' versus: '+STRING(y)
      endif
      yy=diag+ABS(position)
      yy=yy[WHERE(yy LT y)]
      xx=diag[0:N_ELEMENTS(yy)-1]
   endif
   resu=REFORM(input[xx,yy])
   if KEYWORD_SET(debug) then begin
      print, '-----------------------'
      print, 'x :', x, ', y :', y, ', Position indice: ', position
      print, 'input matrix :'
      print, input
      print, 'position XX :', xx
      print, 'position YY :', yy
      print, 'extracted vector :', resu
   endif
endif
;
if (SIZE(input,/n_dim) LE 1) then begin
   if N_PARAMS() EQ 1 then position=0
   nbp=N_ELEMENTS(input)+ABS(position)
   resu=FLTARR(nbp,nbp)
   diag=INDGEN(nbp)
   if (position EQ 0) then begin
      resu[diag,diag]=input
      xx=diag
      yy=diag
   endif
   if (position GT 0) then begin
      xx=diag+position
      xx=xx[WHERE(xx LT nbp)]
      yy=diag[0:N_ELEMENTS(xx)-1]
      resu[xx,yy]=input
   endif
   if (position LT 0) then begin
      yy=diag+ABS(position)
      yy=yy[WHERE(yy LT nbp)]
      xx=diag[0:N_ELEMENTS(yy)-1]
      resu[xx,yy]=input
   endif
   if KEYWORD_SET(debug) then begin
      print, '-----------------------'
      print, 'Position indice: ', position
      print, 'input vector :', input
      print, 'position XX :', xx
      print, 'position YY :', yy
      print, 'computed matrix :'
      print, resu
   endif      
endif
;
if KEYWORD_SET(test) then STOP
;
return, resu
;
end
