;
; Alain C., 31 Aout 2017, at JSC during Harvey
;
; Goal : an easy way to use new !color fiels in classic plotting world 
; plot, findgen(10), color=COLOR2COLOR(!color.red)
;
; bilateral conversion between :
; hexidecimal 24-bit RGB color specification
; and 3-byte !color specification (BYTE = Array[3])
;
; How to test :
; 1: print, COLOR2COLOR(COLOR2COLOR(1193046))
; 2: print, COLOR2COLOR(COLOR2COLOR(byte([12,34,56])))
;
function COLOR2COLOR, color_in, quiet=quiet, $
                      help=help, test=test, verbose=verbose
;
; conversion from !color.field into 
;
if ARRAY_EQUAL(SIZE(color_in), [1,3,1,3]) then begin
   color_out=color_in[2]*256L^2+color_in[1]*256L+color_in[0]
endif else begin
   color_out=BYTARR(3)
   blue = color_in / (256L^2)
   reste= color_in - blue* (256L^2)
   green= reste / 256
   red= reste -green*256
   ;;
   color_out[0]=BYTE(red)
   color_out[1]=BYTE(green)
   color_out[2]=BYTE(bleu)
endelse
;
return, color_out
;
end
