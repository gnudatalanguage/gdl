;
; AC, le 21/02/2006, MaJ le 10/10/2006
;
; write a 2D array mixing Int and Float
; then read back the array
; Can be used to check if f77u is OK between differents computers
;
pro TEST_UF77, nbp=nbp, test=test, help=help, $
               reread=reread, filename=filename
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_UF77, nbp=nbp, test=test, help=help, $'
    print, '               reread=reread, filename=filename'
    print, ''
    print, '/reread   --> only read back the binary file'
    print, 'filename= --> change filename'
    print, 'nbp=      --> chnage array size'
    return
endif
;
if (N_ELEMENTS(nbp) NE 1) then nbp=128
;
if (N_ELEMENTS(filename) NE 1) then begin
    filename='test_u77.bindata'
    print, 'Default file name is : ', filename
endif
;
; de temps en temps, up, un petit Goto !
;
if KEYWORD_SET(reread) then goto, label_reread
;
; ecriture du fichier
;
get_lun, nblun
openw, nblun, filename
writeu, nblun, nbp
writeu, nblun, dist(nbp)
close, nblun
free_lun, nblun
;
print, 'Writing is done, file name is :', filename
;
label_reread:
;
; relecture du ficheir
;
get_lun, nblun
openr, nblun, filename
readu, nblun, nbp
data=fltarr(nbp, nbp)
readu, nblun, data
close, nblun
free_lun, nblun
;
print, 'Reading is done'
;
TVSCL, data
;
if KEYWORD_SET(test) then STOP
;
end

