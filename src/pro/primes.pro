;+
; NAME: PRIMES
;
; PURPOSE:
;       Returns an array with the requested number of prime numbers starting
;       from 2.
;
;
; CATEGORY:
;       arithmetic
;
;
; CALLING SEQUENCE:
;       p = primes(n)
;
;
; INPUTS:
;       n      Number of requested prime numbers
;
; OUTPUTS:
;       p      Array of the n first prime numbers starting from 2
;
;
; MODIFICATION HISTORY:
;   12-Jan-2006 : written by Pierre Chanial
;
; LICENCE:
; Copyright (C) 2006, P. Chanial
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-

function primes, n

 on_error, 2
 
 if n eq 1 then return, [2l]
 
 primes = lonarr(n)
 primes[0] = [2, 3]
 
 candidate = 5l
 
 ; number of prime numbers already calculated
 nprimes = 2l
 
 while nprimes lt n do begin
    for iprimes=1l, nprimes do begin           ; upper limit is never reached
       prime = primes[iprimes]
       if candidate mod prime eq 0 then break  ; it is not a prime number
       if prime*prime gt candidate then begin  ; it is a prime number
          primes[nprimes++] = candidate
          break
       endif
    endfor
    candidate += 2l                            ; get next candidate
 endwhile
 
 return, primes
 
end
