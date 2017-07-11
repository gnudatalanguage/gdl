;+
; NAME:
; GDL_MULTIMIN_FDF
; PURPOSE:
; Helper for internal function DFPMIN
; DFPMIN needs 2 functions, f and df (derivatives of f)
; whereas GSL's minimizer used in our version of DFPMIN uses a 3rd
; function, fdf, that returns both f and df at the same time (clever).
; this function calls appropriately f, df and 'f and df' and solves
; the problem of adapting GSL to DFPMIN.
; MODIFICATION HISTORY:
;    Written by:  Gilles Duvert
;    10 Jul 2017: first version
;-
; LICENCE:
; Copyright (C) 2017
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;-
function gdl_multimin_fdf,x,f,df,code
; called from gdl itself.
; if code=0, return result of 'f' at x
; if code=1, return result of 'df' at x
; if code=2, return both results as []
case code of
0: return,call_function(f,x)
1: return, call_function(df,x)
2: return, [call_function(f,x),call_function(df,x)]
endcase
end
