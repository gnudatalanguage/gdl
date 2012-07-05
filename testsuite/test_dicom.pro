;
; just tests if gdlffdicom is in path
;
pro TEST_DICOM, version=version
;
; bug 3150207 (I was not able to reproduce it on July 2012)
;
if ~KEYWORD_SET(version) then begin
    w=OBJ_NEW('gdlffdicom')
    print, 'hello post OBJ_NEW'
    help, w
endif else begin
    HELP, OBJ_NEW('gdlffdicom')
endelse
;
; dates ...
;
dicom_dat = GDLFFDICOM_DATE()
SPAWN, 'date +%Y%m%d', unix_date
;
if (unix_date[0] NE dicom_dat) then EXIT, status=1
;
MESSAGE, /continue, 'very basic tests OK on DICOM'
MESSAGE, /continue, 'please contribute extending the test cases'
;
end
