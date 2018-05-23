;
; Gilles Duvert, March 2015
;
; write a GDL or a FL file, or regenerate the IDL version
; read it back and compare with IDL reference version stored
; (file 'formats.IDL')
;
; Notes on pbs found by AC, 15 May 2015:
; -- adding a positive case ...
; -- errors in FL 0.79.32 http://www.fawlty.uhostall.com/
;    just due to shift/cut in lines for formats "17"
; -- as is, at this date, this code shows the format management in GDL
;    is OK for negative input, wrong for positive one !!
; -- extra "\ ^J" added in GDL between the Re/Im parts for (D)Complex
;
; Changes: 
; 2016-01612 : AC various changes for better managing paths,
;              expecially for Cmake automatic tests !
;
; md5sum of current version of "formats.IDL" (-1 et 12 ...)
; 9f26db168d3d4d304db8651e5ef1d5d1  formats.IDL
;
; ------------------------------------------------------------
;
pro INTERNAL_FORMAT_PRINTING, lun, struct
;
names=TAG_NAMES(struct)
;
form=[$
['(E)', '(D)', '(g)', '(b)','(o)','(z)','(i)'],$
['(E0)', '(D0)', '(g0)', '(b0)','(o0)','(z0)','(i0)'],$
['(E+)', '(D+)', '(g+)', '(b+)','(o+)','(z+)','(i+)'],$
['(E-)', '(D-)', '(g-)', '(b-)','(o-)','(z-)','(i-)'],$
['(E27)', '(D27)', '(g27)', '(b27)','(o27)','(z27)','(i27)'],$
['(E027)', '(D027)', '(g027)', '(b027)','(o027)','(z027)','(i027)'],$
['(E+27)', '(D+27)', '(g+27)', '(b+27)','(o+27)','(z+27)','(i+27)'],$
['(E-27)', '(D-27)', '(g-27)', '(b-27)','(o-27)','(z-27)','(i-27)'],$
['(E27.12)', '(D27.12)', '(g27.12)', '(b27.12)','(o27.12)','(z27.12)','(i27.12)'],$
['(E+27.12)', '(D+27.12)', '(g+27.12)', '(b+27.12)','(o+27.12)','(z+27.12)','(i+27.12)'],$
['(E-27.12)', '(D-27.12)', '(g-27.12)', '(b-27.12)','(o-27.12)','(z-27.12)','(i-27.12)'],$
['(E+027)', '(D+027)', '(g+027)', '(b+027)','(o+027)','(z+027)','(i+027)'],$
['(E43)', '(D43)', '(g43)', '(b43)','(o43)','(z43)','(i43)']$
] & nformy=13
;form=['(G)', '(G27)', '(G43)', '(G0)','(G+0)','(G-)','(G-0)','(G-27)','(G+27)','(G-032)','(G+032)','(G032)'] & nformy=1
mult=[0.001d,0.01d,0.1d,1.d,10.d,100.d,1000.d]
   for i=0, N_TAGS(struct)-1 do  begin 
      printf, lun, '----------------------------------------------------------------------------------------'
      printf, lun, names[i],struct.(i)
      mytype=size(struct.(i),/type)
      ident_mult=fix(mult, type=mytype)
      myval=(struct.(i))*ident_mult

      for f=0,nformy-1 do begin 
        for j=0, N_ELEMENTS(form[*,0])-1 do begin
          if ~finite(myval[0]) then nloop=0 else nloop=6 
          for k=0,nloop do begin 
            printf, lun, form[j,f]
            printf,lun,myval[k],format=form[j,f]
          end                                       ; form[j,f]+' = "'+STRING(myval[k],format=form[j,f])+'"'
        end
      end
   end
end
;
; ------------------------------------------------------------
;
pro GENERATE_FORMATS_FILE, nb_cases, verbose=verbose, test=test
;
identity=GDL_IDL_FL()
filename='formats.'+identity
if FILE_TEST(filename) then begin
    FILE_COPY, filename, filename+'_old', /overwrite
    MESSAGE,/cont, 'Copy of old file <<'+filename+'_old'+'>> done.'
endif
;
struct = {BYTE:0b,short:-0s,ushort:0us, $
              long:0l,ulong:0ul,long64:0ll, $
              ulong64:0ull,float:0.0,double:0.0d, nand:0.0d, infd:0.0d,$
              cmplx:complex(0,0),dcmplx:dcomplex(0d,0d)}
;
GET_LUN, lun1
OPENW, lun1, filename
np=20 ; do not modify without recomputing save file below with idl8.
a=float(randomn(33,np,/double)*1D8) ; needed: same randomn values for IDL8 and GDL only in this case (for the moment).
if (identity eq 'IDL') then begin ; overwrite a in some rare case
   vers=0.0 & reads,!version.release,vers
   if (vers < 8.2) then restore,filename='test_formats_random_input.sav' ; actually it is 8.2.2
endif
for i=0,n_tags(struct)-1 do struct.(i)=a[i]
struct.nand=!values.d_nan
struct.infd=!values.d_infinity
struct.cmplx=complex(a[np-1],a[np-2])
struct.dcmplx=dcomplex(a[np-3],a[np-4])
;
INTERNAL_FORMAT_PRINTING, lun1, struct
a=-a
for i=0,n_tags(struct)-1 do struct.(i)=a[i]
struct.nand=-!values.d_nan
struct.infd=-!values.d_infinity
struct.cmplx=complex(a[np-1],a[np-2])
struct.dcmplx=dcomplex(a[np-3],a[np-4])

INTERNAL_FORMAT_PRINTING, lun1, struct
printf,lun1,"special case of simple (round) values that must be  shortened by the 'g0' format"
b=10010010LL 
printf,lun1,b,format='(g)' 
printf,lun1,b,format='(g+)' 
printf,lun1,b,format='(g0)' 
printf,lun1,b,format='(g+0)' 
printf,lun1,b,format='(g014)' 
printf,lun1,"special case of strings"
printf,lun1, 'zzzzzzzzzz','!', FORMAT = '(2(A0))'
printf,lun1, 'zzzzzzzzzz','!', FORMAT = '(2(A30))'
printf,lun1, 'zzzzzzzzzz','!', FORMAT = '(2(A-30))'
printf,lun1,"special case of calendar format"
printf,lun1,2.455555555D6,format='(c())'
CLOSE, lun1
FREE_LUN, lun1
;
; nb_cases= nb_values * nb_formats * n_elements(struct)
nb_formats_type=6
nb_formats_formats=4
nb_cases= (N_TAGS(struct_neg)+N_TAGS(struct_pos))*nb_formats_type*nb_formats_formats
;
if KEYWORD_SET(verbose) then MESSAGE,/cont, 'File <<'+filename+'>> written.'
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------------------------
;
; This code can be used in 3 softs : IDL, GDL, FL
; if IDL, it is used to generate the reference
;
pro TEST_FORMATS, help=help, no_exit=no_exit, test=test, debug=debug
;
;ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_FORMATS, help=help, no_exit=no_exit, test=test, debug=debug'
    return
endif
;
; which soft do we use ??
soft=GDL_IDL_FL(/verbose)
;
GENERATE_FORMATS_FILE, nb_cases, verbose=verbose
; if we are IDL, stop, our job is done.
if (soft eq 'IDL') then begin
 print,"reference format file written"
 return
endif
;
; locating then read back the reference:
;
; we need to add the current dir. into the path because new file(s)
; are writtent in it. Do we have a simple way to check whether a dir
; is already in !PATH ?
;
CD, current=current
new_path=!path+PATH_SEP(/SEARCH_PATH)+current
list_of_dirs=STRSPLIT(new_path, PATH_SEP(/SEARCH_PATH), /EXTRACT)
;
; only this reference file is mandatory !
;
filename='formats.IDL'
file_fmt_idl=FILE_SEARCH(list_of_dirs+PATH_SEP()+filename)
;
if N_ELEMENTS(file_fmt_idl) GT 1 then print, 'multiple reference file <<'+filename+'>> found !'
file_fmt_idl=file_fmt_idl[0]
if (soft NE 'IDL') AND (STRLEN(file_fmt_idl[0]) EQ 0) then begin
    MESSAGE, 'reference file <<'+filename+'>> not found in the !PATH', /continue
    if KEYWORD_SET(no_exit) OR KEYWORD_SET(test) then STOP
    EXIT, status=1
endif
;
filename='formats.GDL'
file_fmt_gdl=FILE_SEARCH(list_of_dirs+PATH_SEP()+filename)
if N_ELEMENTS(file_fmt_gdl) GT 1 then begin
    print, 'multiple reference file <<'+filename+'>> found ! First used !!'
    print, TRANSPOSE(file_fmt_gdl)
    file_fmt_gdl=file_fmt_gdl[0]
endif
;
filename='formats.FL'
file_fmt_fl=FILE_SEARCH(list_of_dirs+PATH_SEP()+filename)
if N_ELEMENTS(file_fmt_fl) GT 1 then begin
    print, 'multiple reference file <<'+filename+'>> found !'
    print, TRANSPOSE(file_fmt_fl)
    file_fmt_fl=file_fmt_fl[0]
endif
;
if (soft EQ 'IDL') then begin
    soft=''
    if ~FILE_TEST(file_fmt_fl) then MESSAGE, /cont, "missing file <<formats.FL>>" else soft='FL'
    if ~FILE_TEST(file_fmt_gdl) then MESSAGE, /cont, "missing file <<formats.GDL>>" else soft='GDL'
    if (soft EQ '') then begin
        MESSAGE, /cont, "No useful file found for comparison. Just Reference file written."
        return
    endif
endif
;
; reading back the 2 files : one created ("formats.GDL" or
; "formats.FL") and one reference ("formats.IDL")
;
print, "Files to be compared : formats.IDL, formats."+soft
;
GET_LUN, lun1
OPENR, lun1, file_fmt_idl
GET_LUN, lun2
if (soft EQ 'GDL') then OPENR, lun2, file_fmt_gdl
if (soft EQ 'FL') then OPENR, lun2, file_fmt_fl
;
ref=STRING("")
val=STRING("")
nb_errors=0
;
nlinesidl=(file_lines(file_fmt_idl))[0]
nlinesgdl=(file_lines(file_fmt_gdl))[0]
if (nlinesgdl ne nlinesidl) then Message,"number of lines differ between "+file_fmt_idl+" and "+file_fmt_gdl
line=0
for i=0L, nlinesidl-1 do begin
   READF, lun1, ref
   READF, lun2, val
   line++
   if KEYWORD_SET(debug) then begin
      print, ref
      print, val
   endif
   if ~(STRCMP(ref,val)) then begin
      if KEYWORD_SET(test) then begin
         print, "at line "+strtrim(string(line),2)+":"
         print, "in <<formats.IDL>> : ", ref
         print, "in <<formats."+soft+">> : ", val
      endif
      nb_errors++
   endif
endfor
;
CLOSE, lun1, lun2
FREE_LUN, lun1, lun2
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_FORMATS', nb_errors;, short=short
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP

end
