;
; some tests for STRPLIT
;
; Lea Noreskal, June 2010, under GNU GPL V2 or later
;
pro TEST_STRSPLIT

str='$$$$$$$$$$' ; LONG 0
str1='$chops &up str*ings.' ; [1]
str2='ch$ops &up str*ings.' ; [0,3]
str3='$ch$ops &up str*ings.' ; [1,4]
str4='../foo.txt' ; [2,7]
str5=' t e s t '

;strsplit(str,'$',/ext)


tab=strsplit(str,'$')
if(tab ne 0) then begin
   MESSAGE, 'error str', /continue
   EXIT, status=1
endif

ext=strsplit(str,'$',/ext)
;print , str , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)

if(N_ELEMENTS(ext) ne 1) then begin
   MESSAGE, 'error str extract', /continue
   EXIT, status=1
endif

tab=strsplit(str,'$',/preserve)
res=indgen(11)
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   MESSAGE, 'error str preserve_null', /continue
   EXIT, status=1
endif

ext=strsplit(str,'$',/ext,/preserve)
;print , str , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if(N_ELEMENTS(ext) ne 11) then begin
   MESSAGE, 'error str extract', /continue
   EXIT, status=1
endif


tab=strsplit(str1,'$')
res=[1]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   MESSAGE, 'error str1', /continue
   EXIT, status=1
endif

ext=strsplit(str1,'$',/ext)
;print , str1 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)

res=['chops &up str*ings.']
if( (N_ELEMENTS(ext) ne 1) AND (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0 )) then begin
   MESSAGE, 'error str extract', /continue
   EXIT, status=1
endif

ext=strsplit(str1,'$',/ext,/preserve)
;print , str1 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if((N_ELEMENTS(ext) ne 2)) then begin
   MESSAGE, 'error str1 extract preserves', /continue
   EXIT, status=1
endif



tab=strsplit(str2,'$')
res=[0,3]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   MESSAGE, 'error str2', /continue
   EXIT, status=1
endif

ext=strsplit(str2,'$',/ext)
;print , str2 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)

res=['ch' , 'ops &up str*ings.' ]
if( (N_ELEMENTS(ext) ne 2) AND (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0 )) then begin
   MESSAGE, 'error str extract', /continue
   EXIT, status=1
endif

ext=strsplit(str2,'$',/ext,/preserve)
;print , str2 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if((N_ELEMENTS(ext) ne 2)) then begin
   MESSAGE, 'error str2 extract preserve', /continue
   EXIT, status=1
endif



tab=strsplit(str2,'$',/preserve)
res=[0,3]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   MESSAGE, 'error str2 preserve_null', /continue
   EXIT, status=1
endif

tab=strsplit(str3,'$')
res=[1,4]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   MESSAGE, 'error str3', /continue
   EXIT, status=1
endif

ext=strsplit(str3,'$',/ext)
;print , str3 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)

res=['ch' , 'ops &up str*ings.' ]
if( (N_ELEMENTS(ext) ne 2) AND (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0 )) then begin
   MESSAGE, 'error str extract', /continue
   EXIT, status=1
endif
;

ext=strsplit(str3,'$',/ext,/preserve)
;print , str3 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if((N_ELEMENTS(ext) ne 3)) then begin
   MESSAGE, 'error str3 extract preserve', /continue
   EXIT, status=1
endif

;

tab=strsplit(str4,'.')
res=[2,7]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   MESSAGE, 'error str3', /continue
   EXIT, status=1
endif

tab=strsplit(str4,'.',/preserve)
res=[0,1,2,7]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   MESSAGE, 'error str4 preserve_null', /continue
   EXIT, status=1
endif

ext=strsplit(str4,'.',/ext,/preserve)
;print , str4 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if((N_ELEMENTS(ext) ne 4)) then begin
   MESSAGE, 'error str4 extract , preserve', /continue
   EXIT, status=1
endif

tab= STRSPLIT(str5,/preserve)
res=[0,1,3,5,7,9]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   MESSAGE, 'error str5 preserve_null', /continue
   EXIT, status=1
endif

ext= STRSPLIT(str5,/preserve,/extract)
if((N_ELEMENTS(ext) ne 6)) then begin
   MESSAGE, 'error str5 extract , preserve', /continue
   EXIT, status=1
endif

print, 'All tests done'
;
end


