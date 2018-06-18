;
; Alain, 28 March 2014: 
; first draft for a testsuite for LIST
; AC July: adding IsEmpty tests
;
pro MYMESS, errors, message
errors=errors+1
MESSAGE, /continue, message
end
;
pro TEST_LIST, help=help, verbose=verbose, short=short, $
               no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_LIST, help=help, verbose=verbose, short=short, $'
   print, '               no_exit=no_exit, test=test'
   return
endif
;
nb_errors=0
;
alist=LIST(1, 2, 3)
;
; testing basic counting
;
known_nbe=3
nbe1=N_ELEMENTS(alist)
nbe2=alist.count()
;
txt0='bad counting of elements number '
txt=txt0
if (nbe1 NE known_nbe) then MYMESS, nb_errors, txt+'(N_ELEMENTS)'
if (nbe2 NE known_nbe) then MYMESS, nb_errors, txt+'(LIST.COUNT())'
if KEYWORD_SET(verbose) then print, 'OK after basic countings'
;
; adding 2 elements and counting again
;
alist.Add, 4
alist.Add, 5
;
known_nbe=5
nbe1=N_ELEMENTS(alist)
nbe2=alist.count()
;
txt=txt0+'after LIST.ADD (singleton)'
if (nbe1 NE known_nbe) then MYMESS, nb_errors, txt+'(N_ELEMENTS)'
if (nbe2 NE known_nbe) then MYMESS, nb_errors, txt+'(LIST.COUNT())'
if KEYWORD_SET(verbose) then print, 'counting OK after basic add'
;
; adding a array of 2 strings
;
gdlstr = ["gdl1","gdl2"]
alist.Add, ["gdl1","gdl2"]
known_nbe=6
nbe1=N_ELEMENTS(alist)
nbe2=alist.count()
;
txt=txt0+'after LIST.ADD (array)'
if (nbe1 NE known_nbe) then MYMESS, nb_errors, txt+'(N_ELEMENTS)'
if (nbe2 NE known_nbe) then MYMESS, nb_errors, txt+'(LIST.COUNT())'
if KEYWORD_SET(verbose) then print, 'counting OK after array add'
; ::REMOVE
lastin = alist.Remove() & --known_nbe & txt=txt0+'after LIST.REMOVE()'
nbe1=N_ELEMENTS(alist)
nbe2=alist.count()
if (nbe1 NE known_nbe) then MYMESS, nb_errors, txt+'(N_ELEMENTS)'
if (nbe2 NE known_nbe) then MYMESS, nb_errors, txt+'(LIST.COUNT())'
if KEYWORD_SET(verbose) then print, 'counting still OK after remove()'
jj=where(lastin ne gdlstr, nc)
if nc ne 0 then $
	MYMESS, nb_errors,' list.remove() did not work (item not the same)'
;
;
; Empty List ?
;
empty_list=LIST()
known_nbe=0
nbe1=N_ELEMENTS(empty_list)
;
txt=txt0+'after LIST.IsEmpty()'
if (nbe1 NE known_nbe) then MYMESS, nb_errors, txt+'(N_ELEMENTS)'
if (empty_list.IsEmpty() NE 1) then MYMESS, nb_errors, txt+'(it is LIST.IsEmpty())'
if (alist.IsEmpty() NE 0) then MYMESS, nb_errors, txt+'(not LIST.IsEmpty())'
;
; ---
;
luppers = list(6+findgen(5),/extract)
if luppers.count() ne 5 then $
		mymess,nb_errors,txt0+' list(/extract)'
;
nalist = alist.count()
; Add two lists.
lcombo = alist + luppers & nlcombo = lcombo.count()
if(nlcombo ne 5+nalist) then $
		mymess,nb_errors,txt0+' lcombo' 
iscvs = 0
defsysv,"!GDL",exists=isgdl
if isgdl then $
	isgit = strpos(!GDL.release,'svn') gt 0

if(isgit and keyword_set(verbose)) then begin
  print,' TEST_HASH: GDL/svn is detected so some tests of LIST will be excused,'
  print,' GIT list will only make simple 1-D arrays with TOARRAY'
  print,'  (the new version is nearly full-featured)'
  print,"  direct access to data contained in a list is also lacking in CVS"
endif

if KEYWORD_SET(verbose) then $
	print, 'WARNING: There are more complicated pitfalls in <LIST>.toarray()'
; if KEYWORD_SET(test) then stop,' stop 0'

;
luppers=0 & lcombo=0
;
txt = ' left insertion or right extraction '
if KEYWORD_SET(verbose) then message,/continue,' Checking '+txt
;
alist[2:4]= 3 + indgen(3)
igen=1+indgen(5)
tg=igen[2:3] & tl = intarr(2) & for k=0,1 do tl[k]= alist[2+k]	
txt0=' right index extraction'
if tg[0] ne tl[0] or tg[1] ne tl[1] then MYMESS, nb_errors, txt0+'- simple [2:3]'

nl = nalist
alist.add,igen
if(~isgit) then begin
	tl = alist[nl,2:4]
	jj = where(tl ne igen[2:4],nc)
	if(nc ne 0) then MYMESS, nb_errors, txt0+'- simple [nl,2:4]'
	endif else begin
		if keyword_set(verbose) then $
			message,/continue,' git: legacylist not expected to left insert correctly'
	endelse
if KEYWORD_SET(test) then stop,' stop 1'
;alist=0
empty_list = 0
ll = list(igen,1+indgen(3,10), 10*indgen(4,5)+11,{a: 'a', b: 'b'})
ii1 = 1+indgen(3,10) & ii2 = 10*indgen(4,5)+11
stab=ll.remove()
if(stab.a ne 'a' or stab.b ne 'b') then $
  mymess, nb_errors," ::remove() didn't get the structure."
if(~isgit) then begin
	ll.add,10*indgen(5,5,8)+2 & ii3 = 10*indgen(5,5,8)+2
	ll[1,2,1:4] = 11+indgen(6) 		& ii1[2,1:4] = 11+indgen(6) 
	ll[2,2:3,1:3] = 21 - indgen(6)	& ii2[2:3,1:3]=21 - indgen(6)
	ll[3,1,*,*] = 2*indgen(40)		& ii3[3,1,*,*]=2*indgen(40)
	jj=where(ii1[2,1:4]   ne ll[1,2,1:4], nc1)
	jj=where(ii2[2:3,1:3] ne ll[2,2:3,1:3], nc2)
	jj=where(ii3[3,1,*,*] ne ll[3,1,*,*], nc3)
	if nc1+nc2+nc3 ne 0 then MYMESS, nb_errors, txt
	endif
stab.a = 'This is line of text for A'
stab.b = 'This is, of course, B'
ll.add,stab
ll.reverse
lines=ll[0]
if KEYWORD_SET(test) then stop,' stop 2'
ll=0
if lines.a ne stab.a or lines.b ne stab.b then $
	MYMESS, nb_errors, ' moving a structure item'
;
aa=strsplit(['This is an example',$
' of a string array that will go','for a test'],/extract,length=len)
nline = n_elements(aa)
strall = aa.toarray(dim=1)
wordlength = len.toarray(dim=1)
slen = strlen(aa[0])
for k=1,nline-1 do slen = [slen,strlen(aa[k])]
ko = where(wordlength ne slen, nc) & if nc ne 0 then message,' (wordlength ne slen'
ll=list()
for k=0,14 do ll.add,k+1
;       legacy bug for k=0,14 do ll[k]=k+1
igen = 1+indgen(15)
jj=where((ll.toarray()) ne igen, nj) & if(nj ne 0) then $
        MYMESS, nb_errors, ' for k=0,14 do ll[k]=k+1'
lnew=ll[2:5]
jj=where((lnew.toarray()) ne igen[2:5], nj) & if(nj ne 0) then $
        MYMESS, nb_errors, ' lnew=ll[2:5]'
lnew = 0
subList = LIST('zero', 1, 2.0)
for k=0,sublist.count()-1 do ll.add,subList[k],k
                        ;       ll.Add, subList, 0, /EXTRACT
if KEYWORD_SET(test) then stop,' testing list: sublist, ll'
itst = intarr(sublist.count())
for k=0,sublist.count()-1 do itst[k] = (sublist[k] eq ll[k])

;% LIST::_OVERLOADEQ (internal): LIST container node ID <0> not found.
;alttst = sublist eq ll

if KEYWORD_SET(verbose) then print,' Done checking '+txt
;
if KEYWORD_SET(verbose) then $
  print,' testing list::add,/extract "+" Op and "=" Op'
if(isgit) then begin
    print,' Git/cvs version early return (move this up as more methods are added)'
BANNER_FOR_TESTSUITE, 'TEST_LIST(legacy)', nb_errors, short=short
    return
    endif
; ~isgit can handle this:
alttst = sublist eq ll

; Change the values in the list
nalist = alist.count()
;
for k=0,nalist-1 do alist[k] = 10*k + indgen(10)
ta = intarr(10,nalist)
for k=0,nalist-1 do ta(*,k) = alist[k]
;
jj = where( ta ne findgen(nalist,10), nc)
if nc ne 0 then $
	MYMESS, nb_errors,' alist[k] = a did not work '

list1 = LIST('zero', 1, 2.0)
list2 = LIST(!PI, COMPLEX(4,4), [5,5,5,5,5])
list3 = list1 + list2
if KEYWORD_SET(test) then stop,' stop 2b'
ll3=list()
;	if KEYWORD_SET(test) then stop,' stop 2b.'
;for k=0,list1.count()-1 do ll3.add,list1[k]
;for k=0,list2.count()-1 do ll3.add,list2[k]
ll3.add,list1,/extract ; <<< === Offensive statement(s)
ll3.add,list2,/extract
	if KEYWORD_SET(test) then stop,' stop 2b..'
req = ll3 eq list3
jj=where(req,nj)
if nj ne ll3.count() or total(req) ne nj then $
	MYMESS, nb_errors, ' comparing list3 eq ll3'
if( total(ll3 ne list3) ne 0)  then $
	MYMESS, nb_errors, ' comparing list3 ne ll3'
 if KEYWORD_SET(test) then stop,' stop 2c'
ll3=list()
ll3.add,list2,/extract
ll3.add,list1,/extract,0
if( total(ll3 ne list3) ne 0 or ll3.count() ne list3.count())  then $
	MYMESS, nb_errors, ' list.add, <list to insert>,/extract , 0 "'
ll3=list()
ll3.add,list2,/extract
ll3.add,list1,/extract,1
for k=0,list1.count()-1 do ll3.swap,k,k+1
if( total(ll3 ne list3) ne 0 or ll3.count() ne list3.count())  then $

	MYMESS, nb_errors, ' list1,list2 swapping'
if KEYWORD_SET(verbose) then $
  foreach el,ll3 do print,el
if KEYWORD_SET(test) then stop,' stop 3'
;
; TOARRAY tested:
ll=list()
for k=0,14 do ll.add,-1
for k=0,14 do ll[k]=k+1
igen = 1+indgen(15)
jj=where((ll.toarray()) ne igen, nj) & if(nj ne 0) then $
	MYMESS, nb_errors, ' for k=0,14 do ll[k]=k+1'

if KEYWORD_SET(verbose) then $
   print,' ll.Add, subList, 0, /EXTRACT & print, sublist eq ll >>', alttst


ll=list(indgen(20),/extract)
vv=[2,3,6,8]
dd=ll[vv]
jj=where(dd.toarray() ne vv, nj) & if(nj ne 0) then $
	MYMESS, nb_errors, ' dd=ll[[2,3,6,8]]'
	; remove at a position, then insert an array at pos, then 
	; check for equality using "=" OP.
if KEYWORD_SET(test) then stop,' testing list: dd=0'
	dd=0
listin = list()
if KEYWORD_SET(verbose) then $
  print,' testing list'
for k=0,3 do listin.add,findgen(5)
;
for k=0,3 do begin & devnull=ll.remove(vv[k]) & ll.add,listin[k],vv[k] & endfor
listin=0
;	if KEYWORD_SET(test) then stop,' testing list'
jj=where(ll eq findgen(5),nj)
if(nj ne n_elements(vv) ) then $
	MYMESS, nb_errors, ' ll[vv]= findgen(5)'
ll = 0
   
; ----------------- final messages ----------
;
BANNER_FOR_TESTSUITE, 'TEST_LIST', nb_errors, short=short
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

pro track_update,track,obj
nobj = n_elements(track)
track.ref = heap_refcount(obj)
return
end

verbose = 1
; Our prototype structure:
lifecycle = { name: "", status: "unused", class: "LIST", ref: fix(0), count: fix(0)} 

lc=list() & for k=5,6 do lc.add,list(indgen(k),/extract)
lc0 = lc[0] & lc1 = lc[1]
lout = list(indgen(5))

tracklc = lifecycle
status = "born"
tracklc0 = tracklc
tracklc1 = tracklc
tracklout = tracklc

track_update,tracklc,lc & tracklc.name = "lc"
track_update,tracklc0,lc0 & tracklc0.name = "lc0"
track_update,tracklc1,lc1 & tracklc1.name = "lc1"
track_update,tracklout,lout & tracklout.name = "lout"
lca = objarr(2)
lca = [lc0,lc1]
tracklca = replicate(tracklc,2)
track_update,tracklca,lca & tracklca.name = ["lca[0]","lca[1]"]
;
ahist = [tracklc, tracklc0, tracklc1, tracklout,tracklca]
if keyword_set(verbose) then print,format='(30x,10A8)',ahist.name
ahist.status = status
if keyword_set(verbose) then print,format='(5x,A25,10I8)',status,ahist.ref
;
nhist = 5
ntrack = n_elements(ahist)
refhist = replicate(tracklc, nhist, ntrack)
;
refhist(0,*) = ahist
;
;
oc = idl_container()
status = "first contained"
oc.add,lc & track_update,tracklc,lc
oc.add,lc0 & track_update,tracklc0,lc0
oc.add,lc1 & track_update,tracklc1,lc1
oc.add,lout & track_update,tracklout,lout
oc.add,lca & track_update,tracklca,lca
;
ahist = [tracklc, tracklc0, tracklc1, tracklout,tracklca]
ahist.status = status
if keyword_set(verbose) then print,format='(5x,A25,10I8)',status,ahist.ref
;
refhist(1,*) = ahist
;
oc.remove,/all
track_update,tracklc,lc
track_update,tracklc0,lc0
track_update,tracklc1,lc1
track_update,tracklout,lout
track_update,tracklca,lca
;
status = "removed, then again contained"
oc.add,lc & track_update,tracklc,lc
oc.add,lc0 & track_update,tracklc0,lc0
oc.add,lc1 & track_update,tracklc1,lc1
oc.add,lout & track_update,tracklout,lout
oc.add,lca & track_update,tracklca,lca
;
ahist = [tracklc, tracklc0, tracklc1, tracklout,tracklca]
ahist.status = status
if keyword_set(verbose) then print,format='(5x,A25,10I8)',status,ahist.ref
;
refhist(2,*) = ahist
;
status = "duplicate entry into OC"
oc.add,lc & track_update,tracklc,lc
oc.add,lc0 & track_update,tracklc0,lc0
oc.add,lc1 & track_update,tracklc1,lc1
oc.add,lout & track_update,tracklout,lout
oc.add,lca & track_update,tracklca,lca
;
ahist = [tracklc, tracklc0, tracklc1, tracklout,tracklca]
ahist.status = status
if keyword_set(verbose) then print,format='(5x,A25,10I8)',status,ahist.ref
;
refhist(3,*) = ahist
;
oc.remove,lc & track_update,tracklc,lc
oc.remove,lc0 & track_update,tracklc0,lc0
oc.remove,lc1 & track_update,tracklc1,lc1
oc.remove,lout & track_update,tracklout,lout
oc.remove,lca & track_update,tracklca,lca
status = "removed lists"
;
ahist = [tracklc, tracklc0, tracklc1, tracklout,tracklca]
ahist.status = status
if keyword_set(verbose) then print,format='(5x,A25,10I8)',status,ahist.ref
;
refhist(4,*) = ahist
;
oc.add,lc & track_update,tracklc,lc
oc.add,lc0 & track_update,tracklc0,lc0
oc.add,lc1 & track_update,tracklc1,lc1
oc.add,lout & track_update,tracklout,lout
oc.add,lca & track_update,tracklca,lca
ahist = [tracklc, tracklc0, tracklc1, tracklout,tracklca]

status = "re-populated "
ahist.status = status
if keyword_set(verbose) then print,format='(5x,A25,10I8)',status,ahist.ref
;
refhist=reform([refhist,reform(ahist,1,ntrack)],++nhist,ntrack)
;
status ="ocall=oc.get(/all)"
ocall = oc.get(/all)
tracklc.name = "ocall[0]"
	track_update,tracklc,ocall[0]
	track_update,tracklc0,ocall[1]
	track_update,tracklc1,ocall[2]
	track_update,tracklout,ocall[3]
	track_update,tracklca,[ocall[4],ocall[5]]
;
ahist = [tracklc, tracklc0, tracklc1, tracklout,tracklca]
ahist.status = status
ahist.name = ["ocall[0]","ocall[1]","ocall[2]","ocall[3]","ocall[4]","ocall[5]"]
if keyword_set(verbose) then print,format='(5x,A25,10I8)',status,ahist.ref
;
refhist=reform([refhist,reform(ahist,1,ntrack)],++nhist,ntrack)

;	delvar,lc
;	status = "lc gone (delvar)"
;	track_update,tracklc,lc
; == note for delvar: the above 3 lines have induced a call to track_update with status as prm1.
lc=0 & status = ' lc = 0' & tracklc.name = '-0-'
	track_update,tracklc,ocall[0]
	track_update,tracklc0,ocall[1]
	track_update,tracklc1,ocall[2]
	track_update,tracklout,ocall[3]
	track_update,tracklca,[ocall[4],ocall[5]]

;
ahist = [tracklc, tracklc0, tracklc1, tracklout,tracklca]
ahist.name = ["ocall[0]","ocall[1]","ocall[2]","ocall[3]","ocall[4]","ocall[5]"]
ahist.status = status
if keyword_set(verbose) then print,format='(5x,A25,10I8)',status,ahist.ref
;
refhist=reform([refhist,reform(ahist,1,ntrack)],++nhist,ntrack)
;
status ="lc0=0 & lca = 0"
lc0=0 & lca = 0
	track_update,tracklc,ocall[0]
	track_update,tracklc0,ocall[1]
	track_update,tracklc1,ocall[2]
	track_update,tracklout,ocall[3]
	track_update,tracklca,[ocall[4],ocall[5]]

;
ahist = [tracklc, tracklc0, tracklc1, tracklout,tracklca]
ahist.name = ["ocall[0]","ocall[1]","ocall[2]","ocall[3]","ocall[4]","ocall[5]"]
ahist.status = status
if keyword_set(verbose) then print,format='(5x,A25,10I8)',status,ahist.ref
;
refhist=reform([refhist,reform(ahist,1,ntrack)],++nhist,ntrack)
;
;
status ="lct=oc.get(posit=[1,4,5])"
lct=oc.get(posit=[1,4,5])
	track_update,tracklc,ocall[0]
	track_update,tracklc0,ocall[1]
	track_update,tracklc1,ocall[2]
	track_update,tracklout,ocall[3]
	track_update,tracklca,[ocall[4],ocall[5]]
;
ahist = [tracklc, tracklc0, tracklc1, tracklout,tracklca]
ahist.name = ["ocall[0]","ocall[1]","ocall[2]","ocall[3]","ocall[4]","ocall[5]"]
ahist.status = status
if keyword_set(verbose) then print,format='(5x,A25,10I8)',status,ahist.ref
;
refhist=reform([refhist,reform(ahist,1,ntrack)],++nhist,ntrack)
;
kind = indgen(nhist,ntrack)
kk = kind(*,0)
for k = 0,nhist-1 do begin & kindex = kk[k]  &$
 print,format='(A10,I5,5x,A)',$
	(refhist[kindex]).name,(refhist[kindex]).ref,(refhist[kindex]).status  &$
	endfor

end
