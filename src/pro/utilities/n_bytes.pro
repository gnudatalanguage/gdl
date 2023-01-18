function N_bytes,a
;+
; NAME:
;       N_bytes()
;
; PURPOSE:
;       To return the total number of bytes in data element
;
; CALLING SEQUENCE:
;       result = N_bytes(a)
;
; INPUTS:
;       a - any idl data element, scalar or array
;
; OUTPUTS:
;       total number of bytes in a is returned as the function value
;       (64bit longword scalar)
; NOTES:
;       (1) Not valid for object or pointer data types
;       (2) For a string array, the number of bytes is computed after conversion
;           with the BYTE() function, i.e. each element has the same length,
;           equal to the maximum individual string length.
;
; MODIFICATION HISTORY:
;       Version 1  By D. Lindler  Oct. 1986
;       Include new IDL data types    W. Landsman          June 2001
;       Now return a 64bit integer    W. Landsman          April 2006
;-
;-----------------------------------------------------
;
 dtype = size(a,/type)                      ;data type
 if dtype EQ 0 then return,0            ;undefined
 nel = N_elements(a)
 case dtype of
        1: nb = 1                            ;Byte
        2: nb = 2                            ;16 bit signed integer
        3: nb = 4                            ;32 bit signed integer
        4: nb = 4                            ;Float
        5: nb = 8                            ;Double
        6: nb = 8                            ;Complex
        7: nb = max(strlen(a))               ;String                      
        8: nb = N_tags(a,/length)            ;Structure
        9: nb = 16                           ;Double Complex
       12: nb = 2                            ;Unsigned 16 bit Integer
       13: nb = 4                            ;Unsigned 32 bit Integer
       14: nb = 8                            ;64 bit signed integer
       15: nb = 8                            ;64 bit unsigned integer
     else: message,'ERROR - Object or Pointer data types not valid'
 endcase

 return,long64(nel)*nb
 end
