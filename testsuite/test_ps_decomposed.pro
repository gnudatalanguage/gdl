;
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
;
; Modification by Alain C. (25-Oct-2011) because
; this code was failing on a computer with "bad" locale (fr_FR.utf8)
;
pro CHECK_PS_COLOR, file=file, delete=delete, decomposed=decomposed, $
                    color=color, expected_rgb=expected_rgb

DEVICE, /color, decomposed=decomposed, file=file
POLYFILL, [0,1,1,0], [0,0,1,1], color=color, /normal
DEVICE, /close

command="grep -e '\([0-1]\.[0-9]\+ \)\{3\}\+[setrgbcolor|C]' "
command=command+ file + "|tail -1"
SPAWN, command, output
;
if KEYWORD_SET(delete) then FILE_DELETE, tmpfile, /quiet
;
READS, output, r,g,b
if ~ARRAY_EQUAL(BYTE(255*[r,g,b]), expected_rgb) then begin
   MESSAGE, /continue, $
            'rgb triple read from the PS file does not math the expected one'
   EXIT, status=1
endif
;
end
;
; ------------------------------
;
pro TEST_PS_DECOMPOSED, no_delete=no_delete
;
if LOCALE_GET() NE 'C' then begin
   print, '-----------------'
   MESSAGE,/Continue, 'problem expected due to bad LOCALE'
   print, '-----------------'
endif
;
delete=0
if ~KEYWORD_SET(no_delete) then delete=1
;
tmpfile = 'test_ps_decomposed'
suffix='.ps'
;
SET_PLOT, 'ps'
DEVICE, get_decomposed=isdecomposed
if isdecomposed then begin
   MESSAGE, 'PS terminal use indexed colours by default', /conti
   EXIT, status=1
endif
DEVICE,/close
;
CHECK_PS_COLOR, file=tmpfile+'_R'+suffix, delete=delete, $
                color='ff'x, /decomposed, expected_rgb=[255,0,0]
CHECK_PS_COLOR, file=tmpfile+'_G'+suffix, delete=delete, $
                color='ff00'x, /decomposed, expected_rgb=[0,255,0]
CHECK_PS_COLOR, file=tmpfile+'_B'+suffix, delete=delete, $
                color='ff0000'x, /decomposed, expected_rgb=[0,0,255]
;
end
