;
pro MYMESS, errors, message
errors=errors+1
MESSAGE, /continue, message
end
;
pro test_HASH,debug=debug, verbose=verbose
isgit = 0
defsysv,"!GDL",exists=isgdl
if isgdl then $
	isgit = strpos(!GDL.release,'svn') gt 0
if isgit then $
  message,/cont,' GDL/SVN is detected so some tests will be excused,'

if(isgit and keyword_set(verbose)) then begin
  print,' Principally, those that traverse beyond a 1-Dimensional hash access'
  print," 	h = HASH('a', HASH('b', HASH('c', 5))) "
  print,' 	we cannot access  '+" h['a', 'b', 'c'] = 5"
endif

; create hash1 as a foldcase hash, using all lowercase for keys.
hash1 = HASH('key1', 1, 'key2', 2, 'key3', 3, 'badpi', 3.14)
struchash = { key1: 1, key2: 2, key3: 3, badpi: 3.14}

if keyword_set(verbose) then begin
	print,' extract a struct into a hash:'
	print," hash1 = HASH('key1', 1, 'key2', 2, 'key3', 3, 'badpi', 3.14) & print,hash1 "
	print,hash1
	print,' struchash = { key1: 1, key2: 2, key3: 3, badpi: 3.14} & print,struchash '
	print,struchash
	help,/st,struchash
	endif
; cvs cannot make a hash from a structure
nb_errors = 0
if ~isgit then begin
	; make a comparison hash from the structure.
	if ~isgit then $
		hcomp = hash(struchash,/lower,/fold) $
		else hcomp = hash(struchash)
	nstash = n_tags(struchash)
	if(hcomp.count() ne nstash) then $
	MYMESS, nb_errors,$
	   ' structure was not properly stashed into the hash <hcomp = hash(struchash,/lower)> '
	   
	hhtest =  hcomp eq hash1

	if keyword_set(verbose) then begin
		print,' hcomp = hash(struchash,/lower)'
		hhtest =  hcomp eq hash1
		help,hhtest & print, hhtest
		endif
	hcomp = hash(struchash,/fold)

	if keyword_set(verbose) then begin
		print,' hcomp = hash(struchash,/FOLD_CASE) & help, hcomp eq hash1 '
		hcomp = hash(struchash,/FOLD_CASE)
		complist = hcomp eq hash1
		help,complist[*]
		print," keys = [ 'key1', 'key3' ] & print, hash1[keys] "
		keys = [ 'key1', 'key3' ]
		print,hash1[keys]
		message,/continue,' End verbose block'
		endif
endif
; COPY a hash:
	if keyword_set(verbose) then $
print," hash1 = HASH('key1', 1, 'key2', 2) & hash2 = hash1 & hash2['key1'] = 'hello' "
hash1 = HASH('key1', 1, 'key2', 2)
hash2 = hash1
hash2['key1'] = 'hello'
	if keyword_set(verbose) then $
print," hash1['key1']: ", hash1['key1'], "	 hash2['key1']: ", hash2['key1']
if( ~isgit) then begin
	hnew = hash2[*]
	keq = (hnew eq hash1).toarray()
	if n_elements(keq) ne hash1.count() then $
		MYMESS, nb_errors,' error (hash1[*] eq hash1).toarray() '
endif

keys = ['A', 'B', 'C', 'D', 'E', 'F', 'G']
values = LIST('one', 2.0, 3, 4l, PTR_NEW(5), {n:6}, COMPLEX(7,0))
htest = HASH(keys, values)
IF N_ELEMENTS(htest) ne 7 then $
	MYMESS, nb_errors,$
	' N_ELEMENTS(htest) ne 7  .. fail '
	
; cvs does not take a scalar in value position where #elements(key) > 1/
if ~isgit then begin
	 keys = ['A', 'B', 'C', 'D', 'E', 'F', 'G']
	 scalars=hash(keys,0)
	 eq7 = scalars.count(0)
	scalars[keys[1:4]] = 4
	eq4 = scalars.count(4)
if eq7 ne 7 then $
	MYMESS, nb_errors,$
		' eq7 = scalars.count(0) is not 7'
if eq4 ne 4 then 	MYMESS, nb_errors,$
		' scalars[keys[1:4]] = 4 scalars.count(4) is not 4'
endif

; cvs does not do most of this:
if ~isgit then begin

	chk = 2*indgen(20)+1
	struct = {FIELD1: 4.0, FIELD2: {SUBFIELD1: "hello", SUBFIELD2: 3.14}}


	htest = HASH(struct, /EXTRACT,/fold,/lower)

	if keyword_set(verbose) then $
		print,' htest = HASH(struct, /EXTRACT,/fold,/lower) ',htest

	if htest['FIELD2','SUBFIELD2'] ne 3.14 then 	MYMESS, nb_errors,$
				" hash['FIELD2','SUBFIELD2'] ne 3.14"
	htest['FIELD2','SUBFIELD2'] = chk

	if htest['FIELD2','SUBFIELD2',2] ne 5 then 	MYMESS, nb_errors,$
				" htest['FIELD2','SUBFIELD2',2] ne 3" 
	htest['field2','subfield2',3] = 101
	if htest['FIELD2','SUBFIELD2',3] ne 101 then 	MYMESS, nb_errors,$
				" htest['FIELD2','SUBFIELD2',3] ne 101"

	htest = hash('field1', 4.0, 'field2', hash(/fold),/fold)
	htest['field2','subfield1'] = "hello"
	htest['field2','subfield2'] = chk
	if htest['FIELD2','SUBFIELD2',3] ne chk[3] then $
			MYMESS, nb_errors,$
				" htest['FIELD2','SUBFIELD2',3] ne chk[3]"
	rt = htest['field2']
	if keyword_set(verbose) then begin 
		help,rt & print,rt & endif

	endif else $
	print,' Limited tests for legacy HASH in git'

; git should be able to do HasKey()
hbw = HASH('black', 0, 'gray', 128, 'grey', 128, 'white', 255)

	if keyword_set(verbose) then $
print,[ hbw.HasKey('gray'), hbw.HasKey(['grey','red','white'])]

keys = ['a','b','c','d','e','f','g']
if ~isgit then $
	scalars = hash(keys,0) else scalars=hash(keys,intarr(n_elements(keys)))
scalars[keys]=100+indgen(n_elements(keys))

if keyword_set(verbose) then $
	foreach value, scalars, key do print," key:",key," =",value
if keyword_set(verbose) then $
	print,'scalars[keys]=100+indgen(n_elements(keys))'
more = ['h','j','k']
scalars += hash(more,more)
if keyword_set(verbose) then $
	foreach value, scalars[more], key do print," key:",key," =",value
allkeys= scalars.keys()
scalars[allkeys]=allkeys.toarray()
if keyword_set(verbose) then $
	foreach value, scalars, key do print," key:",key," =",value

; ----------------- final messages ----------
;
BANNER_FOR_TESTSUITE, 'TEST_HASH', nb_errors, short=short
;
if KEYWORD_SET(test) then STOP,'keyword TEST set: stop'
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
return
end
