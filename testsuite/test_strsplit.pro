;
; some tests for STRPLIT
;
; Lea Noreskal, June 2010, under GNU GPL V2 or later
;
; Alain Coulais, October 2010: 
;  -- few extra cases. Also adding tests on count= and lenght= keywords
;
pro IPRINT, texte, indice
print, texte, indice
indice=indice+1
end
;
pro TEST_STRSPLIT, quiet=quiet, test=test, debug=debug
;
nb_pbs=0
indice=0
;
str='$$$$$$$$$$' ; LONG 0
str1='$chops &up str*ings.' ; [1]
str2='ch$ops &up str*ings.' ; [0,3]
str3='$ch$ops &up str*ings.' ; [1,4]
str4='../foo.txt' ; [2,7]
str5=' t e s t '
str6='qwerty'
;
; When Search Pattern is not in Input String (default Search pattern
; is white space ' ')
;
tab=STRSPLIT(str6)
if (tab ne 0) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str basic 0', /continue
   nb_pbs=nb_pbs+1
endif
tab=STRSPLIT(str6,/extract)
if (tab ne str6) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str basic 0 bis', /continue
   nb_pbs=nb_pbs+1
endif
;
if KEYWORD_SET(debug) then IPRINT, 'ici', indice
;
; When Search Pattern is a stupid void input ''
;

tab=strsplit(str5,'')
if (tab ne 0) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str basic 1', /continue
   nb_pbs=nb_pbs+1
endif
tab=strsplit(str5,'',/extract)
if (tab ne '') then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str basic 1 bis', /continue
   nb_pbs=nb_pbs+1
endif
;
if KEYWORD_SET(debug) then IPRINT, 'ici', indice
;
; When Search Pattern is not in Input String
;
tab=strsplit(str5,'a')
if (tab ne 0) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str basic 2', /continue
   nb_pbs=nb_pbs+1
endif
tab=strsplit(str5,'a',/extract)
if (tab ne str5) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str basic 2 bis', /continue
   nb_pbs=nb_pbs+1
endif
;
;
if KEYWORD_SET(debug) then IPRINT, 'ici', indice
;
tab=STRSPLIT(str,'$')
if(tab ne 0) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str', /continue
   nb_pbs=nb_pbs+1
endif

ext=STRSPLIT(str,'$',/ext)
;print , str , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)

if(N_ELEMENTS(ext) ne 1) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str extract', /continue
   nb_pbs=nb_pbs+1
endif

tab=STRSPLIT(str,'$',/preserve)
res=indgen(11)
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str preserve_null', /continue
   nb_pbs=nb_pbs+1
endif

ext=STRSPLIT(str,'$',/ext,/preserve)
;print , str , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if(N_ELEMENTS(ext) ne 11) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str extract', /continue
   nb_pbs=nb_pbs+1
endif
;
if KEYWORD_SET(debug) then IPRINT, 'ici', indice
;
tab=STRSPLIT(str1,'$')
res=[1]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str1', /continue
   nb_pbs=nb_pbs+1
endif

ext=STRSPLIT(str1,'$',/ext)
;print , str1 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)

res=['chops &up str*ings.']
if( (N_ELEMENTS(ext) ne 1) AND (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0 )) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str extract', /continue
   nb_pbs=nb_pbs+1
endif

ext=STRSPLIT(str1,'$',/ext,/preserve)
;print , str1 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if((N_ELEMENTS(ext) ne 2)) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str1 extract preserves', /continue
   nb_pbs=nb_pbs+1
endif
;
if KEYWORD_SET(debug) then IPRINT, 'ici', indice

tab=STRSPLIT(str2,'$')
res=[0,3]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str2', /continue
   nb_pbs=nb_pbs+1
endif

ext=STRSPLIT(str2,'$',/ext)
;print , str2 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)

res=['ch' , 'ops &up str*ings.' ]
if( (N_ELEMENTS(ext) ne 2) AND (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0 )) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str extract', /continue
   nb_pbs=nb_pbs+1
endif

ext=STRSPLIT(str2,'$',/ext,/preserve)
;print , str2 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if((N_ELEMENTS(ext) ne 2)) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str2 extract preserve', /continue
   nb_pbs=nb_pbs+1
endif



tab=STRSPLIT(str2,'$',/preserve)
res=[0,3]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str2 preserve_null', /continue
   nb_pbs=nb_pbs+1
endif

tab=STRSPLIT(str3,'$')
res=[1,4]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str3', /continue
   nb_pbs=nb_pbs+1
endif

ext=STRSPLIT(str3,'$',/ext)
;print , str3 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)

res=['ch' , 'ops &up str*ings.' ]
if( (N_ELEMENTS(ext) ne 2) AND (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0 )) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str extract', /continue
   nb_pbs=nb_pbs+1
endif
;

ext=STRSPLIT(str3,'$',/ext,/preserve)
;print , str3 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if((N_ELEMENTS(ext) ne 3)) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str3 extract preserve', /continue
   nb_pbs=nb_pbs+1
endif
;
if KEYWORD_SET(debug) then IPRINT, 'ici', indice
;
tab=STRSPLIT(str4,'.')
res=[2,7]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str3', /continue
   nb_pbs=nb_pbs+1
endif

tab=STRSPLIT(str4,'.',/preserve)
res=[0,1,2,7]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str4 preserve_null', /continue
   nb_pbs=nb_pbs+1
endif

ext=STRSPLIT(str4,'.',/ext,/preserve)
;print , str4 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if((N_ELEMENTS(ext) ne 4)) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str4 extract , preserve', /continue
   nb_pbs=nb_pbs+1
endif

tab=STRSPLIT(str5,/preserve)
res=[0,1,3,5,7,9]
if(ARRAY_EQUAL(tab,res) eq 0) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str5 preserve_null', /continue
   nb_pbs=nb_pbs+1
endif

ext=STRSPLIT(str5,/preserve,/extract)
if((N_ELEMENTS(ext) ne 6)) then begin
   if ~KEYWORD_SET(quiet) then MESSAGE, 'error str5 extract , preserve', /continue
   nb_pbs=nb_pbs+1
endif
;
mess=' errors encoutered during STRSPLIT tests'
if (nb_pbs GT 0) then mess=STRING(nb_pbs)+mess else mess='NO'+mess
MESSAGE, /Continue, mess
;
if KEYWORD_SET(test) then STOP
;
if (nb_pbs GT 0) then EXIT, status=1
;
end



