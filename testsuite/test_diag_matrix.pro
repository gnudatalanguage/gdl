;
; Alain C.
; very preliminary tests (not really tested ...)
;
pro TEST_DIAG_MATRIX_EXTRACT
;
debug=1
;
m3x4=REFORM(INDGEN(12),3,4)
;
a=DIAG_MATRIX(m3x4, debug=debug)
a=DIAG_MATRIX(m3x4, 1, debug=debug)
a=DIAG_MATRIX(m3x4, -1, debug=debug)
;
m4x3=REFORM(INDGEN(12),4,3)
;
a=DIAG_MATRIX(m4x3, debug=debug)
a=DIAG_MATRIX(m4x3, 1, debug=debug)
a=DIAG_MATRIX(m4x3, -1, debug=debug)
;
end

pro TEST_DIAG_MATRIX_INSERT
;
debug=1
;
diag=10+FINDGEN(5)
;
a=DIAG_MATRIX(diag, debug=debug)
a=DIAG_MATRIX(REVERSE(10+findgen(4)), 1, debug=debug)
a=DIAG_MATRIX(-REVERSE(10+findgen(4)), -1, debug=debug)
;
end
;
pro TEST_DIAG_MATRIX
;
TEST_DIAG_MATRIX_EXTRACT
TEST_DIAG_MATRIX_INSERT
;
end
