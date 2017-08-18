;
; AC, August 4th 2017
;
; Design to be run in IDL, GDL or FL
; Output files to be written
;
; for FL & GDL, the CMSV lib must be in the PATH (tested)
;
pro RUN_ALL_BENCHMARK
;
ON_ERROR, 2
;
CHECK_SAVE_RESTORE
;
BENCH_FFT, /save
BENCH_FFT, /save, /double
;
BENCH_MATRIX_INVERT, /save
BENCH_MATRIX_INVERT, /save, /double
;
BENCH_MATRIX_MULTIPLY, /save
;
BENCH_MEDIAN, /save
;
end

