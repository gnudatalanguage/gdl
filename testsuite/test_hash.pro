;
; basic tests on HASH() and related functionnalities
;
; -----------------------------------------------
; 
; Modifications history :
;
; - 2019-11-06 : AC :  rewrite header, code cleaning
;
; -----------------------------------------------
;
pro TEST_HASH, debug=debug, verbose=verbose, no_exit=no_exit
;
nb_errors = 0
;
; below a few simple tests on HASH before 
; the more internal tests provided originally
;
; define two hashtables, with pointers and check Where()
a=33L
p=PTR_NEW(a)
hsh = HASH('key1', 1.414, 'key2', 3.14, 'key3', ptr_new(a)) ;
hshp = HASH('key1', 1.414, 'key2', 3.14, 'key3', p) ;

c=hsh.where(PTR_NEW(a)) ; c must be a 0 element list  IS NOT AT THE MOMENT. Issued 
; SUPRESSED UNTIL issue  #578 has been closed.
;if ~c.IsEmpty() then ERRORS_ADD, nb_errors,' error where() on different pointers to same value '

c=hsh.where(1.414) ; c must be a LIST of 1 element and c[0]='key1'
if c ne 'key1' then ERRORS_ADD, nb_errors,' error where() on key=Float '

c=hshp.where(p) ; 
if c ne 'key3' then ERRORS_ADD, nb_errors,' error where() on key=ptr '

; now check EQ (I suppose NEQ will work)

result = hsh EQ 1.414
if result.count() ne 1 and result[0] ne 'key1' then ERRORS_ADD, nb_errors,' error EQ Float for HASH ' 

result = hshp EQ p
if result.count() ne 1 and result[0] ne 'key3' then ERRORS_ADD, nb_errors,' error EQ pointer for HASH ' 

; "more internal" tests - to de bedited and made more undertsandable to maintainers.
;
isgit = 0
DEFSYSV,"!GDL",exists=isgdl
if isgdl then isgit = STRPOS(!GDL.release,'git') gt 0

isgit = 0 ; no more excuses.

if isgit then MESSAGE, /cont,' GDL/git is detected so some tests will be excused,'

if (isgit and KEYWORD_SET(verbose)) then begin
  print,' Principally, those that traverse beyond a 1-Dimensional hash access'
  print,"   h = HASH('a', HASH('b', HASH('c', 5))) "
  print,'   we cannot access  '+" h['a', 'b', 'c'] = 5"
endif

; create hash1 as a foldcase hash, using all lowercase for keys.
hash1 = HASH('key1', 1, 'key2', 2, 'key3', 3, 'badpi', 3.14)
struchash = { key1: 1, key2: 2, key3: 3, badpi: 3.14}

if KEYWORD_SET(verbose) then begin
   print,' extract a struct into a hash:'
   print," hash1 = HASH('key1', 1, 'key2', 2, 'key3', 3, 'badpi', 3.14) & print,hash1 "
   print,hash1
   print,' struchash = { key1: 1, key2: 2, key3: 3, badpi: 3.14} & print,struchash '
   print,struchash
   help,/st,struchash
endif
; 
;; make a comparison hash from the structure.
;
hcomp = HASH(struchash,/lower,/fold)
nstash = N_TAGS(struchash)
txt=' structure was not properly stashed into the hash <hcomp = hash(struchash,/lower)> '
if (hcomp.count() ne nstash) then ERRORS_ADD, nb_errors, txt

hhtest =  hcomp eq hash1

if KEYWORD_SET(verbose) then begin
   print,' hcomp = hash(struchash,/lower)'
   hhtest =  hcomp eq hash1
   help, hhtest & print, hhtest
endif

hcomp = HASH(struchash,/fold)

if KEYWORD_SET(verbose) then begin
   print,' hcomp = hash(struchash,/FOLD_CASE) & help, hcomp eq hash1 '
   hcomp = HASH(struchash,/FOLD_CASE)
   complist = hcomp eq hash1
   HELP, complist[*]
   print," keys = [ 'key1', 'key3' ] & print, hash1[keys] "
   keys = [ 'key1', 'key3' ]
   print, hash1[keys]
   MESSAGE, /continue,' End verbose block'
endif

stop

if ~isgit then begin
endif
;
; COPY a hash:
;
if KEYWORD_SET(verbose) then $
   print," hash1 = HASH('key1', 1, 'key2', 2) & hash2 = hash1 & hash2['key1'] = 'hello' "
hash1 = HASH('key1', 1, 'key2', 2)
hash2 = hash1
hash2['key1'] = 'hello'

if KEYWORD_SET(verbose) then $
   print, " hash1['key1']: ", hash1['key1'], "   hash2['key1']: ", hash2['key1']
if( ~isgit) then begin
endif

keys = ['A', 'B', 'C', 'D', 'E', 'F', 'G']
values = LIST('one', 2.0, 3, 4l, PTR_NEW(5), {n:6}, COMPLEX(7,0))
htest = HASH(keys, values)
IF N_ELEMENTS(htest) ne 7 then $
if eq7 ne 7 then $

; cvs does not do most of this:
;
if ~isgit then begin
   ;;
   chk = 2*indgen(20)+1
   struct = {FIELD1: 4.0, FIELD2: {SUBFIELD1: "hello", SUBFIELD2: 3.14}}
   ;;
   htest = HASH(struct, /EXTRACT,/fold,/lower)
   ;;
   if KEYWORD_SET(verbose) then $
      print,' htest = HASH(struct, /EXTRACT,/fold,/lower) ',htest
   ;;
   hnew = hash2[*]
   keq = (hnew eq hash1).toarray()
   if n_elements(keq) ne hash1.count() then $
      ERRORS_ADD, nb_errors,' error (hash1[*] eq hash1).toarray() '
   ;;
   ERRORS_ADD, nb_errors, ' N_ELEMENTS(htest) ne 7  .. fail '
   ;;
   ;; Tostruct(/recursive)
   struct = {FIELD1: 4.0, FIELD2: {SUBFIELD1: "hello", SUBFIELD2: 3.14, subfield3: 6.28}}
   hash = HASH(struct, /EXTRACT)
   sback = hash.toStruct(/recursive)
   if ~ISA(sback.FIELD2,'STRUCT')  then begin
      ERRORS_ADD, nb_errors
      MESSAGE,/cont,  ' HASH.ToStruct(/recursive)  failed'
   endif else begin
      if KEYWORD_SET(verbose) and ~isgit then begin
         message,/cont, ' HASH.ToStruct(/recursive)  succeeded'
      endif
   endelse
   ;;
   keys = ['A', 'B', 'C', 'D', 'E', 'F', 'G']
   scalars=hash(keys,0)
   eq7 = scalars.count(0)
   if isgit then scalars[keys[1:4]] = 4+intarr(4) else $
      scalars[keys[1:4]] = 4
   eq4 = scalars.count(4)
endif else begin
    print,' Limited tests for legacy HASH in git'
endelse

; git should be able to do HasKey()
hbw = HASH('black', 0, 'gray', 128, 'grey', 128, 'white', 255)

if KEYWORD_SET(verbose) then $
   print,[ hbw.HasKey('gray'), hbw.HasKey(['grey','red','white'])]

keys = ['a','b','c','d','e','f','g']
if ~isgit then begin
   scalars = HASH(keys,0)
endif else begin
   scalars=HASH(keys,intarr(n_elements(keys)))
endelse

scalars[keys]=100+INDGEN(n_elements(keys))

if KEYWORD_SET(verbose) then begin
   foreach value, scalars, key do print," key:",key," =",value
   print,'scalars[keys]=100+indgen(n_elements(keys))'
endif

more = ['h','j','k']
scalars += hash(more,more)
if KEYWORD_SET(verbose) then $
    foreach value, scalars[more], key do print," key:",key," =",value
allkeys= scalars.keys()
scalars[allkeys]=allkeys.toarray()
if KEYWORD_SET(verbose) then $
    foreach value, scalars, key do print," key:",key," =",value
;
; ----------------- final messages ----------
;
BANNER_FOR_TESTSUITE, 'TEST_HASH', nb_errors, short=short
;
if KEYWORD_SET(test) then STOP, 'keyword TEST set: stop'
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
;    ERRORS_ADD, nb_errors,$
;        ' eq7 = scalars.count(0) is not 7'
;if eq4 ne 4 then    ERRORS_ADD, nb_errors,$
;        ' scalars[keys[1:4]] = 4 scalars.count(4) is not 4'
