;
; AC 2017-dec-26
;
; we expect the GDL version is as : A.B.C (major.minor.dev)
; with A, B and C supposed to be integers
;
; 0.9.9 -->   909
; 8.4   --> 80400
; 8.4.1 --> 80401
;
; OK with IDL, OK with FL since 0.79.46
; ---------------------------------------
; Modifications history :
;
; AC 2019-Feb-08 : * generalisation to manage A.B too
; (A.B should be lower than A.B.C)
; * should work with IDL too (!version.release)
;
; ---------------------------------------
;
pro APRINT, tab3nbps
;
tmp=FIX(tab3nbps, type=2)
;
fmt2='(A,i3,A,i3)'
fmt3='(A,i3,A,i3,A,i3)'

if N_ELEMENTS(tmp) EQ 2 then $
   print, format=fmt2, 'major :', tmp[0], ', minor :', tmp[1]
if N_ELEMENTS(tmp) EQ 3 then $
   print, format=fmt3, 'major :', tmp[0], ', minor :', tmp[1], ', dev :', tmp[2]
;
end
;
; ---------------------------------------
;
function GDL_VERSION, version=version, test=test, debug=debug
;
suite=''
if KEYWORD_SET(version) then begin
   suite=version 
endif else begin
   case GDL_IDL_FL() of
      'IDL' : suite=!version.release
      'GDL' : suite=!gdl.release
      ;; for FL before, 
      'FL' : begin
         DEFSYSV, '!fl', exists=isFVrecent
         if isFVrecent then suite=!fl.release else return, -1
      end
      else : MESSAGE, 'strange that no software detected !'
   endcase
endelse
;
if KEYWORD_SET(debug) then print, suite
;
indices=STRSPLIT(suite,'.',/extract)
if KEYWORD_SET(debug) then APRINT, indices
;
if  N_ELEMENTS(indices) EQ 2 then indices=[indices,'0']
int_indices=FIX(indices)
;
if KEYWORD_SET(debug) then APRINT, int_indices
;
value=TOTAL([10000L,100,1]*int_indices,/integer)
;
if KEYWORD_SET(debug) then PRINT, 'resulting value : ', value
if KEYWORD_SET(test) then STOP
;
return, value
end
