;+
;
; please report problems / examples / extensions
;
; NAME:      DIAG_MATRIX
;
; PURPOSE:   1/ returning the diagonal of the input matrix
;            2/ generating a square matrix with a given diagonal.
;
; CATEGORY:   Matrix utilities
;
; CALLING SEQUENCE:
;  - case 1:  my_diagonal=DIAG_MATRIX(matrix)
;  - case 2:  matrix=DIAG_MATRIX(a_diag_vector, an_offset)
;
; INPUTS:
;  - case 1:  a matrix (square or not)
;  - case 2:  a vector (might be a singleton)
;
; OPTIONAL INPUTS: 
;  - case 1:  none
;  - case 2:  an offset
;
; KEYWORD PARAMETERS:
;
; OUTPUTS:
;  - case 1: a vector, the diagonal
;  - case 2: a square matrix
;
; OPTIONAL OUTPUTS: none
;
; COMMON BLOCKS: none
;
; SIDE EFFECTS: none
;
; RESTRICTIONS: none known !
;
; PROCEDURE: straightforward
;
; EXAMPLE:
;
; - case 1: print, DIAG_MATRIX(DIST(10))
;
; - case 2:
; ** generating a NxN rotation matrix:
;           matrix=DIAG_MATRIX(REPLICATE(1.,nbp-1),1)
;           matrix[0,nbp-1]=1.
; ** generating a IDENTITY matrix (equal to: identity=IDENTITY(nbp)
;           identity=DIAG_MATRIX(REPLICATE(1.,nbp))
;
; MODIFICATION HISTORY:
;
; * 23-JAN-2012: initial version by Alain C.
;
; * 11-APR-2013: - in fact, when creating the output matrix,
;                  we have to derive the type from the input diagonal !
;                - more documentation 
;
;-
; LICENCE:  Copyright (C) 2012, 2013, Alain Coulais, under GNU GPL v2 or later
;
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;-
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
; case 1, first usage: returning the diagonal
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
    txt='% Specified offset to array is out of range: '
    ;;
    if (position GT 0) then begin
        if (position GE x) then begin
            MESSAGE, txt+STRING(position)+' versus: '+STRING(x)
        endif
        xx=diag+position
        xx=xx[WHERE(xx LT x)]
        yy=diag[0:N_ELEMENTS(xx)-1]
    endif  
    if (position LT 0) then begin
        if (ABS(position) GE y) then begin
            MESSAGE, txt+STRING(position)+' versus: '+STRING(y)
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
; case 2: second usage: generating a square matrix populated
; by "diagonals" passed by argument, with a given "offset".
;
if (SIZE(input,/n_dim) LE 1) then begin
    ;; determining the size of the output matrix
    if N_PARAMS() EQ 1 then position=0
    nbp=N_ELEMENTS(input)+ABS(position)
    ;; creating wthe output matrix with adequate type
    type=SIZE(input,/type)
    resu=MAKE_ARRAY(nbp,nbp, type=type)
    ;;
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
