; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
; tests: ATAN, LL_ARC_DISTANCE and values of !PI, !DPI, !DTOR, !RADEG

pro test_angles
  
  ; testing values of !PI, !DPI, !DTOR, !RADEG in full precision
  if !PI ne atan(1.) * 4 then begin
    message, '!PI != atan(1) * 4', /conti
    ;exit, status=1
  endif
  if !DPI ne atan(1d) * 4 then begin
    message, '!DPI != atan(1) * 4', /conti
    ;exit, status=1
  endif
  if !DTOR ne !PI / 180. then begin
    message, '!DTOR != !PI / 180.', /conti
    ;exit, status=1
  endif
  if !RADEG ne 180. / !PI then begin
    message, '!RADEG != 180. / !PI', /conti
    ;exit, status=1
  endif

  types = [1, 2, 3, 4, 5, 6, 9, 12, 13, 14, 15]
  ;     :  B  I  L  F  D  C  D  U   U   L   U  :
  ;     :  Y  N  O  L  O  O  C  I   L   O   L  :
  ;     :  T  T  N  O  U  M  O  N   O   N   O  :
  ;     :  E  :  G  A  B  P  M  T   N   G   N  :
  ;     :  :  :  :  T  L  L  P  :   G   6   G  :
  ;     :  :  :  :  :  E  E  L  :   :   4   6  :
  ;     :  :  :  :  :  :  X  E  :   :   :   4  :
  ;     :  :  :  :  :  :  :  X  :   :   :   :  : ... matrix ;)

  ; testing ATAN
  seed = !PI
  len = 10
  rand_byte = byte(255 * randomu(seed, len))
  rand_float = float(1e38 * (randomu(seed, len) - .5))
  rand_complex = complex(float(1e38 * (randomu(seed, len) - .5)), float(1e38 * (randomu(seed, len) - .5)))
  for t1 = 0, n_elements(types) - 1  do begin   ; datatype of the first argument
    for t2 = 0, n_elements(types) - 1 do begin  ; datatype of the second argument
      for d1 = 0, 3 do begin                    ; dimension of the first argument (0 - scalar)
        for d2 = 0, 3 do begin                  ; dimension of the second argument (0 - scalar)
          for ph = 0, 1 do begin                ; PHASE keyword presence
            ; A. single-argument case
            if d1 gt 0 then a1 = make_array(d1, ty=types[t1]) else a1 = (make_array(1, ty=types[t1]))[0]
            a1 += rand_byte[fix(len * randomu(seed))]
            if types[t1] gt 3 && types[t1] lt 12 then a1 /= rand_float[fix(len * randomu(seed))]
            if types[t1] eq 6 || types[t1] eq 9 then a1 *=  rand_complex[fix(len * randomu(seed))]
            v = atan(a1, phase=ph)
            s = size(v, /stru)
            help, a1, out=o
            e = string(10b) + 'in:  ' + o + string(10b)
            help, v, out=o
            e += 'out: ' + o
            ; A.01 for real input atan() should return values between (-pi/2, pi/2)
            if (types[t1] ne 6 && types[t1] ne 9) && $
              total(v lt -!DPI/2 or v gt !DPI/2) gt 0  then begin
                                                         message, 'FAILED (A.01)' + e, /conti
                                                         ;exit, status=1
                                                       endif
            ; A.02 for any input Z = tan(atan(Z)) (not for /PHASE)
            if ~ph && total(abs(fix(a1, ty=s.type) - tan(v)) gt max([abs(a1/.5e3), .5e-3])) gt 0 $
                                                       then begin
                                                         message, 'FAILED (A.02)' + e, /conti
                                                         ;exit, status=1
                                                       endif
            ; A.03 for complex input output should be complex (if PHASE not set)
            if (~ph && types[t1] eq 6 && s.type ne 6) $
                                                       then begin
                                                         message, 'FAILED (A.03)' + e, /conti
                                                         ;exit, status=1
                                                       endif $
            ; A.04 for complexdbl input output should be complexdbl (if PHASE not set)
            else if (~ph && types[t1] eq 9 && s.type ne 9) $
                                                       then begin
                                                         message, 'FAILED (A.04)' + e, /conti
                                                         ;exit, status=1
                                                       endif $
            ; A.05 for double input output should be double
            else if (types[t1] eq 5 && s.type ne 5)    then begin
                                                         message, 'FAILED (A.05)' + e, /conti
                                                         ;exit, status=1
                                                       endif $
            ; A.06 for any other input output should be float
            else if ((types[t1] ne 6 && types[t1] ne 9 && types[t1] ne 5) $
              && s.type ne 4)                          then begin
                                                         message, 'FAILED (A.06)' + e, /conti
                                                         ;exit, status=1
                                                       endif
            ; A.07 for any input output has the same shape (arrays)
            if (~array_equal(s.dimensions, (size(a1, /stru)).dimensions)) $
                                                       then begin
                                                         message, 'FAILED (A.07)' + e, /conti
                                                         ;exit, status=1
                                                       endif
            ; A.08 if /PHASE is set and the argument is complex the output is float
            if (ph && types[t1] eq 6 && s.type ne 4)   then begin
                                                         message, 'FAILED (A.08)' + e, /conti
                                                         ;exit, status=1
                                                       endif
            ; A.09 if /PHASE is set and the argument is complexdbl the output is double
            if (ph && types[t1] eq 9 && s.type ne 5)   then begin
                                                         message, 'FAILED (A.09)' + e, /conti
                                                         ;exit, status=1
                                                       endif
            ; A.10 if /PHASE is present and the argument is complex[dbl], the result 
            ;      is equal atan(imaginary(Z), real_part(Z))
            if (ph && (types[t1] eq 9 || types[t1] eq 6) && $
              ~array_equal(v, atan(imaginary(a1), real_part(a1)))) $
                                                       then begin
                                                         message, 'FAILED (A.10)' + e, /conti
                                                         ;exit, status=1
                                                       endif

            ; B. two-argument case
            if d2 gt 0 then a2 = make_array(d2, ty=types[t2]) else a2 = (make_array(1, ty=types[t2]))[0]
            a2 += rand_byte[fix(len * randomu(seed))]
            if types[t2] gt 3 && types[t2] lt 12 then a2 /= rand_float[fix(len * randomu(seed))]
            if types[t2] eq 6 || types[t2] eq 9 then a2 *= rand_complex[fix(len * randomu(seed))]
            v = atan(a1, a2, phase=ph)
            s = size(v, /stru)
            help, a1, out=o
            e = string(10b) + 'in:  ' + o + string(10b)
            help, a2, out=o
            e += '     ' + o + string(10b)
            help, v, out=o
            e += 'out: ' + o
            ; B.01 for real input atan() should return values between (-pi, pi)
            if (types[t1] ne 6 && types[t1] ne 9 && types[t2] ne 6 && types[t2] ne 9) && $ 
              total(v lt -!PI or v gt !PI) gt 0      then begin
                                                       message, 'FAILED (B.01)' + e, /conti
                                                       ;exit, status=1
                                                     endif
            ; B.02 for complex input output should be complex (while the other != double)
            if (((types[t2] eq 6 && types[t1] ne 5 && types[t1] ne 9) $
              || (types[t1] eq 6 && types[t2] ne 5 && types[t2] ne 9)) $
              && s.type ne 6)                          then begin
                                                         message, 'FAILED (B.02)' + e, /conti
                                                         ;exit, status=1
                                                       endif $
            ; B.03 for complexdbl input output should be complexdbl 
            else if ((types[t2] eq 9 || types[t1] eq 9) && s.type ne 9) $
                                                       then begin
                                                         message, 'FAILED (B.03)' + e, /conti
                                                         ;exit, status=1
                                                       endif $
            ; B.04 for double input output should be double
            else if (((types[t2] eq 5 && types[t1] ne 9 && types[t1] ne 6) $
                   || (types[t1] eq 5 && types[t2] ne 9 && types[t2] ne 6)) $
                   && s.type ne 5)                     then begin
                                                         message, 'FAILED (B.04)' + e, /conti   
                                                         ;exit, status=1
                                                       endif $
            ; B.05 for any other input output should be float
            else if ((types[t1] ne 9 && types[t2] ne 9 && types[t1] ne 5 && $
              types[t2] ne 5 && types[t1] ne 6 && types[t2] ne 6) && s.type ne 4) $
                                                       then begin
                                                         message, 'FAILED (B.05)' + e, /conti
                                                         ;exit, status=1
                                                       endif
            ; B.06 for two-array input output has the size of the smaller array
            if (d1 gt 0 && d2 gt 0 && ~ array_equal(s.dimensions, (size(d1 lt d2 ? $
              a1 : a2, /stru)).dimensions))            then begin
                                                         message, 'FAILED (B.06)' + e, /conti
                                                         ;exit, status=1
                                                       endif
            ; B.07 for array/scalar input output has the size of the array
            if (((d1 eq 0 && d2 gt 0) || (d1 gt 0 && d2 eq 0)) && $
              ~array_equal(s.dimensions, (size(d1 gt d2 ? a1 : a2, /stru)).dimensions)) $
                                                       then begin
                                                         message, 'FAILED (B.07)' + e, /conti
                                                         ;exit, status=1
                                                       endif
            ; C. testing LL_ARC_DISTANCE
            if d1 gt 1 && d2 lt 2 then begin
              lr = ll_arc_distance(a1, a2, a2) 
              ld = ll_arc_distance(a1, a2, a2, /degrees) 
              ; C.01 always expecting 2-element array
              if n_elements(lr) ne 2 || n_elements(ld) ne 2 $
                                                       then begin
                                                         message, 'FAILED (C.01)', /conti
                                                         ;exit, status=1
                                                       endif
              ; C.02 if any input param is dcomplex -> dcomplex
              if (types[t1] eq 9 || types[t2] eq 9) && $
                (size(lr, /ty) ne 9 || size(ld, /ty) ne 9) $
                                                       then begin
                                                         message, 'FAILED (C.02)', /conti
                                                         ;exit, status=1
                                                       endif
              ; C.03 if any input param is complex -> complex
              if ((types[t1] eq 6 && types[t2] ne 9) || (types[t2] eq 6 && types[t1] ne 9)) && $
                (size(lr, /ty) ne 6 || size(ld, /ty) ne 6) $
                                                       then begin
                                                         message, 'FAILED (C.03)', /conti
                                                         ;exit, status=1
                                                       endif
              ; C.04 if any input param is double -> double

              if (types[t1] eq 5 || types[t2] eq 5) && $
                total(imaginary(a1) ne 0 or imaginary(a2) ne 0) eq 0 && $
                (size(lr, /ty) ne 5 || size(ld, /ty) ne 5) $
                                                       then begin
                                                         message, 'FAILED (C.04)', /conti
                                                         ;exit, status=1
                                                       endif
              ; C.05 otherwise expecting float output
              if (types[t1] ne 9 && types[t2] ne 9 && types[t1] ne 6 && types[t2] ne 6 $
                && types[t1] ne 5 && types[t2] ne 5) $
                && (size(ld, /ty) ne 4 || size(lr, /ty) ne 4) $
                                                       then begin
                                                         message, 'FAILED (C.05)', /conti
                                                         ;exit, status=1
                                                       endif
              ; C.06 output lat should lay between (-pi, pi) if talking radians
              if (real_part(lr[1]) lt -!PI || real_part(lr[1]) gt !PI) $
                                                       then begin
                                                         message, 'FAILED (C.06)', /conti
                                                         ;exit, status=1
                                                       endif
              ; C.07 output lat should lay between (-180, 180) if talking degrees
              if (real_part(ld[1]) lt -180 || real_part(ld[1]) gt 180) $
                                                       then begin
                                                         message, 'FAILED (C.07)' 
                                                         ;exit, status=1
                                                       endif
            endif
          endfor
        endfor
      endfor
    endfor
  endfor

  message, 'PASSED', /continue

end
