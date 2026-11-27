; part of GDL - GNU Data Language (GPL)
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
;
; ------------------------------------------
; Modification history:
;
; 2026-11-16 : Alain C. limited clean-up
; - standard end of code
;
; ------------------------------------------
;
function ASSERT, str, xpr, bool, fold_case=fold_case
if ~KEYWORD_SET(fold_case) then fold_case=0
if STRMATCH(str, xpr, fold_case=fold_case) ne bool then begin
   MESSAGE, 'strmatch(''' + str + ''', ''' + xpr + ''', fold=' + STRTRIM(fold_case,2) + ') ne ' + STRTRIM(bool,2)
   return, 1
endif
return, 0
end
;
; ------------------------------------------
;
pro TEST_STRMATCH, no_exit=no_exit, help=help, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_STRMATCH, no_exit=no_exit, help=help, test=test'
    return
endif
;
err = 0
err += assert('gdl', 'gdl', 1)
err += assert('gpl', 'gdl', 0)
err += assert('gdl', 'gd', 0)
err += assert('GDL', 'gdl', 0)
err += assert('GDL', 'gdl', 1, /fold)
err += assert('GDL', 'gd*', 0)
err += assert('GDL', 'gd*', 1, /fold)
err += assert('GDL', '*dl', 0)
err += assert('GDL', '*dl', 1, /fold)
err += assert('GDL', 'g?l', 0)
err += assert('GDL', 'g?l', 1, /fold)
err += assert('GDL', 'g?', 0)
err += assert('GDL', 'g?', 0, /fold)
err += assert('GDLGDL', 'GD?G*L', 1)
err += assert('GDLGDL', 'G?G*L', 0)
err += assert('GDLGDL', '*DL*', 1)
err += assert('GDLGDL', '\*DL\*', 0)
err += assert('A*DL*A', 'A\*DL\*A', 1)
err += assert('A?DL?A', 'A\?DL\?A', 1)
err += assert('*DL*', '\*DL\*', 1)
err += assert('?DL?', '\?DL\?', 1)
err += assert('GDLGDL', '?DL?', 0)
err += assert('GDLGDL', 'G[ABCD]LGDL', 1)
err += assert('GDLGDL', 'G[!ABCD]LGDL', 0)
err += assert('GDLGDL', 'G[A-D]LGDL', 1)
err += assert('.()+{}|^$', '.()+{}|^$', 1)
err += assert('.()+{}|^$', '.()+{*^$', 1)
err += assert('.()+{}|^$', '.()+{?^$', 0)
err += assert('foot', 'f??t', 1)
err += ~ARRAY_EQUAL(STRMATCH(['gdl', 'GDL'], 'gdl'), [1,0])
;
if err ne 0 then exit, status=1
;
;
cumul_errors=err
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_STRMATCH', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

