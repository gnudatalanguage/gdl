PRO test_formats,no_exit=no_exit, test=test
struct={byte:-1b,short:-1us,ushort:-1us,long:-1l,ulong:-1ul,long64:-1ll,ulong64:-1ull,float:-1,double:-1d,cmplx:complex(-1,-1),dcmplx:dcomplex(-1,-1)}
names=tag_names(struct)
openw,1,'formats.gdl'
form=['(b)','(o)','(z)','(i)']
for i=0,n_tags(struct)-1 do for j=0,n_elements(form)-1 do printf,1, string(names[i],format='(a12)')+"["+form[j]+']="'+string(struct.(i),format=form[j])+'"'
form=['(b0)','(o0)','(z0)','(i0)']
for i=0,n_tags(struct)-1 do for j=0,n_elements(form)-1 do printf,1, string(names[i],format='(a12)')+"["+form[j]+']="'+string(struct.(i),format=form[j])+'"'
form=['(b6)','(o6)','(z6)','(i6)']
for i=0,n_tags(struct)-1 do for j=0,n_elements(form)-1 do printf,1, string(names[i],format='(a12)')+"["+form[j]+']="'+string(struct.(i),format=form[j])+'"'
form=['(b017)','(o017)','(z017)','(i017)']
for i=0,n_tags(struct)-1 do for j=0,n_elements(form)-1 do printf,1, string(names[i],format='(a12)')+"["+form[j]+']="'+string(struct.(i),format=form[j])+'"'
form=['(b043)','(o043)','(z043)','(i043)']
for i=0,n_tags(struct)-1 do for j=0,n_elements(form)-1 do printf,1, string(names[i],format='(a12)')+"["+form[j]+']="'+string(struct.(i),format=form[j])+'"'
form=['(b064)','(o064)','(z064)','(i064)']
for i=0,n_tags(struct)-1 do for j=0,n_elements(form)-1 do printf,1, string(names[i],format='(a12)')+"["+form[j]+']="'+string(struct.(i),format=form[j])+'"'
close,1
openr,1,"formats.idl"
openr,2,"formats.gdl"
ref=string("")
val=string("")
nb_errors=0
for i=0L,6*n_tags(struct)*n_elements(form)-1 do begin
 readf,1,ref
 readf,2,val
; for complex, print adds a supplementary blank in some cases. try to
; remove it
 val=strtrim(val)
 if ~(strcmp(ref,val)) then begin 
  nb_errors++
 end
end
close,1
close,2
; ----------------- final messages ----------
;
BANNER_FOR_TESTSUITE, 'TEST_LIST', nb_errors, short=short
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP

end
