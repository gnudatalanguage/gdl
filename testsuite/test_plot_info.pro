;
; AC, June 2007
;
; For memory, list of available test procedures for PLOT
;
; 1/ How are managed NaN and Inf in plot
; 2/ benchmark of plot, with and without NaN and Inf
; 3/ options like Psym and linestyle
;
; please run the last procedure "TEST_PLOT_INFO" to have informations ...
;
;
pro TEST_PLOT_INFO
;
; some informations about the programs ...
;
print, ' -------- very basic tests for PLOT (listyle, psym ...) -------------'
print, ''
print, 'Call TEST_PLOT_BASIC'
print, 'This procedure shows significant differences with IDL'
print, 'When all X or Y data have same values'
print, ''
print, 'Call TEST_LINESTYLE'
;
print, ' -------- testing NaN and INF -------------'
print, ''
print, 'You have to compile : TEST_PLOT_INF_NAN.pro'
print, ''
print, 'Call  TEST_PLOT_INF_NAN if you woul like to see how GDL'
print, 'manage plots with NaN or Inf (4 cases). (4 Calls to PLOT_INF_NAN)'
print, ''
print, 'Call TEST_PLOT_TRICK, /nan or TEST_PLOT_TRICK, /inf'
print, 'if you would like to see few tricky cases for Nan and Inf plots'
print, ''
print, ' -------- Benchmarking PLOT -------------'
print, ''
print, 'You have to compile : TEST_PLOT_BENCHMARK.pro'
print, ''
print, 'Call TEST_PLOT_BENCH_ONE to have time test for any number of points (keyword nbp=)'
print, 'on a randomU signal (worst know case)'
print, ''
print, 'Call TEST_PLOT_BENCHMARK to have time test for 6 number of points (10^i, i=1 ... 6)'
print, 'for 4 different cases ((1) findgen, (2) randomU, (3) random and '
print, '(4) findgen containing 10 or 110 "NaN" at randomU position)'
print, 'Or with /all keyword !'
end
