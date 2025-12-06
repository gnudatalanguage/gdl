;
; Script : test-read_ascii
;
pro TEST_READ_ASCII, test=test
;
;; we need a way to know whether IDL or GDL is running...
DEFSYSV, '!gdl', exists=isGDL
filename = (isGDL?'gdl':'idl'+strtrim(!version.release,1))+$
           '-test-read_ascii.txt'
JOURNAL, filename
;
;;------------------------------------------------------------------------------
;; IDL 6.0 hangs on this one
print
print, '1--'
goto, skip1
t = {version:1.0, fieldnames : strsplit('fa,fb,fc,fd,fe,ff',',', /extr), $
     fieldtypes : [7, 4, 7, 2, 1, 5], fieldgroups : [0, 1, 2, 3, 4, 5], $
     fieldcount: 6, fieldlocations:[0, 5, 9, 11, 14, 16], datastart:0, $
     delimiter:'', missingvalue:'NaN', commentsymbol:';'}
a = READ_ASCII('test-read_ascii.txt', template=t, header=header, data_start=2, count=count)
help, header, count
help, a, /structure
print, a.fa, a.fb, a.fc, a.fd, a.fe, a.ff
skip1:

;;------------------------------------------------------------------------------

print
print, '2--'
t = {version:1.0, fieldnames : strsplit('fa,fb,fc,fd,fe,ff',',', /extr), $
     fieldtypes : [7, 4, 7, 2, 1, 5], fieldgroups : [0, 1, 2, 3, 4, 5], $
     fieldcount: 6, fieldlocations:[0, 5, 9, 11, 14, 16], datastart:0, $
     delimiter:'', missingvalue:!values.f_nan, commentsymbol:';'}
a = READ_ASCII('test-read_ascii.txt', template=t, header=header, data_start=2, count=count)
help, count, header
for i=0, n_elements(header)-1 do print, header[i]

help, a, /structure
print, a.fa, a.fb, a.fc, a.fd, a.fe, a.ff

;;------------------------------------------------------------------------------

print
print, '3--'
t = {version:1.0, fieldnames : strsplit('fa,fb,fc,fd,fe,ff',',', /extr), $
     fieldtypes : [7, 4, 7, 2, 1, 5], fieldgroups : [0, 1, 2, 3, 4, 5], $
     fieldcount: 6, fieldlocations:[0, 5, 9, 11, 14, 16], datastart:0, $
     delimiter:'', missingvalue:-999, commentsymbol:';'}
a = READ_ASCII('test-read_ascii.txt', template=t, header=header, data_start=2, count=count)
help, a, /structure
print, a.fa, a.fb, a.fc, a.fd, a.fe, a.ff

;;------------------------------------------------------------------------------

print
print, '4--'
;; IDL incorrectly prints 4.4000001 instead of 4.4000000
t = {version:1.0, fieldnames : strsplit('fa,fb,fc,fd,fe,ff',',', /extr), $
     fieldtypes : [7, 4, 7, 2, 1, 5], fieldgroups : [15, 1, 2, 1, 0, 1], $
     fieldcount: 6, fieldlocations:[0, 5, 9, 11, 14, 16], datastart:0, $
     delimiter:'', missingvalue:-999, commentsymbol:';'}
a = READ_ASCII('test-read_ascii.txt', template=t, header=header, data_start=2, count=count)
help, a, /structure
print, a.fa, a.fb, a.fc, a.fe

;;------------------------------------------------------------------------------

print
print, '5--'
;; IDL bug: type of group "1" should be string
goto, skip2
t.fieldtypes  = [7 , 7, 7, 2, 1, 5]
a = READ_ASCII('test-read_ascii.txt', template=t, header=header, data_start=2, count=count)
help, a, /structure
print, a.fa, a.fb, a.fc, a.fe
skip2:
;
;;------------------------------------------------------------------------------
;
if KEYWORD_SET(test) then STOP

JOURNAL

end
