;
; AC 02 April 2010
;
; We put here some well known problems in order to help
; beginners to setup well installed/compiled GDL 
;
; this program can be used in any versions of GDL
; It should help you to found important bugs and
; solution to avoid them.
;
; -----------------------------------------------
;
pro TEST_BUG_LENNY, verbose=verbose
;
; tested aldo in test_bug_2974380.pro 
if KEYWORD_SET(verbose) then begin
    print, 'A bug related to ImageMagick and Plplot exists in'
    print, 'Debian Lenny and (maybe) in recent Ubuntu (9.10, 10.4).'
    print, 'If GDL crashs here, please re-compiled PLPlot library'
    print, 'using option -DENABLE_DYNDRIVERS=OFF in cmake'
endif
;
WINDOW
WDELETE
;
if KEYWORD_SET(verbose) then begin
    print, 'will crash here if plplot not well compiled (on some plateforms !)'
endif
;
WINDOW
WDELETE
;
print, 'test TEST_BUG_LENNY passed'
;
end
;
; -----------------------------------------------
;
pro TEST_BUG_FFT, verbose=verbose
;
if KEYWORD_SET(verbose) then begin
    print, 'Old version of GDL do have, in the important FFT procedure,'
    print, 'some bugs and bad management of some type (Integer, Complex) in input'
    print, 'This call helps you to quickly know if you must upgrade to version >= gdl-0.9rc4'
endif
;
a=L64INDGEN(16)
b=FFT(a)
if TOTAL(ABS(a-b)) EQ 0 then begin
    print, 'You are using an old version of GDL, please upgrade to gdl >= gdl-0.9rc4'
    print, '(using FFTw library is better)'
    print, 'test TEST_BUG_FFT *NOT* passed'
    exit, status=1
endif
;
print, 'test TEST_BUG_FFT passed'
;
end
;
; -----------------------------------------------
;
pro TEST_BUG_SAVE_RESTORE, verbose=verbose
;
if KEYWORD_SET(verbose) then begin
    print, 'A change in the CMSR library in May 2009 affects the behavior of SAVE/RESTORE in GDL'
    print, 'If this test failed, please use an old version of the library'
    print, 'or change line 286c279 in CMRESTORE '
    print, 'bad    if nnames EQ 0 AND n_par EQ 0 then kall = 1'
    print, 'good   if nnames EQ 0 then kall = 1'
endif
;
a=1
b=1.
SAVE, file='DemoGDL.xdr', a, b
RESTORE, file='DemoGDL.xdr', /verbose
;
print, 'test TEST_BUG_SAVE_RESTORE passed'
;
end
;
; -----------------------------------------------
;
pro TEST_KNOWN_BUGS, verbose=verbose, quiet=quiet
;
if NOT(KEYWORD_SET(quiet)) then begin
    print, 'Despite strong efforts, GDL cannot take into account'
    print, 'all tricks and problems in dependances and related softwares (libraries).'
    print, 'In some cases, on some distributions, we know we have'
    print, 'reproducible crashs and, with those programs, we try to help you to know' 
    print, 'wether you will suffer from some problems'
    print, ''
    print, 'If GDL crashes when running first time this procedure, please re-run GDL and call:'
    print, 'TEST_KNOWN_BUGS, /verbose'
    print, 'in order to receive detailed informations about your problem'
endif
;
TEST_BUG_LENNY, verbose=verbose
TEST_BUG_FFT, verbose=verbose
TEST_BUG_SAVE_RESTORE, verbose=verbose
;
end

