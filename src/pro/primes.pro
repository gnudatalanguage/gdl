;$Id: primes.pro,v 1.1.1.1 2004-12-09 15:10:19 m_schellens Exp $
function primes, n

;
;
;
; NAME:
;     primes
;
; PURPOSE: 
;     Return the first n primes
;
;
; CATEGORY:
;    Mathematics: General
;
; CALLING SEQUENCE:
;    p=primes(n)
;
;
; OUTPUTS:
;   p is an long integer array containing the first n prime numbers
;
;
;
; RESTRICTIONS:
;   n < some number..
;
; PROCEDURE:
;   The first two primes (2,3) are fixed. The odd integers are looped
;   through in order and tested against the current primes to check if
;   they are prime. Primes are added to the output array and returned
;
; EXAMPLE:
;    p=prime(10)
;    print, p
;    2  3  5  7  11  13  17  19   23  29
;
; 
; MODIFICATION HISTORY:
; 	Written by:	
;
;
;
;-
; LICENCE:
; Copyright (C) 2004,
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-

On_error, 2

if(n eq 1) then return, [2L]

p=intarr(n)
p[0]=2L
c=1L ; prime count
p[c]=3L

t=p[c] ; test number

c=c+1L

while(c lt n) do begin

    t=t+2. ;odd numbers only

    j=1L
    while(j lt c) do begin ;only test the primes > 2

        r=t mod p[j] ;remainder
        q=t / p[j] ; quotient
        if(r eq 0) then break ;not a prime

        if(q le p[j]) then begin ; i.e if p[j] > sqrt(t) (implicit r ne 0)
            p[c]=t
            j=c
            c=c+1L
        endif
        j=j+1L;
    endwhile
    
endwhile

return, p

end





