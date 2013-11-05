;
; Josh Sixsmith 5 Jul 2012, under GNU GPL v3 or later
;
; A simple test that checks the dimensions of the read and written image
; 
pro TEST_WRITE_READ_ENVI, columns__=columns, rows=rows, bands=bands, $
                          out_name=out_name, help=help, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_WRITE_READ_ENVI, columns=columns, rows=rows, $'
   print, '                          bands=bands, out_name=out_name, $' 
   print, '                          help=help, test=test'
   return
endif
;
; create a dummy image
if N_ELEMENTS(columns) eq 0 then columns = 400
if N_ELEMENTS(rows) eq 0 then rows = 400
if N_ELEMENTS(bands) eq 0 then begin 
   img = RANDOMN(sd, columns, rows)
   nb = 1
   ns = columns
   nl = rows
endif else begin
   img = RANDOMN(sd, columns, rows, bands)
   nb = bands
   ns = columns
   nl = rows
endelse
;
if (N_ELEMENTS(out_name) eq 0) then out_name='envi_test_img'
;
; begin by writing the image
;
WRITE_ENVI_FILE, img, out_name=out_name, ns=ns, nl=nl, nb=nb
;
; now to read in the image
;
READ_ENVI_FILE, out_name, image=r_img, info=img_info
;
; testing by checking the equivalency of the read image with the
; written image
;
check = SIZE(img, /dimensions) eq SIZE(r_img, /dimensions)
wh = WHERE(check eq 0, count)
if (count eq 0) then begin
   MESSAGE, 'Test Passed', /continue
endif else begin
   MESSAGE, 'Test Failed', /continue
endelse
;
if KEYWORD_SET(test) then STOP
;
end
