function pref_get,p, names_all=names_all, num_pending=num_pending, structure=structure
  if keyword_set(num_pending) then return,0
  if keyword_set(structure) then Message,"STRUCTURE keyword for PREF_GET not yet implemented, FIXME"

  ret=ptrarr(45)
  preferences=[$
     "IDL_CPU_TPOOL_MAX_ELTS","IDL_CPU_TPOOL_MIN_ELTS","IDL_CPU_TPOOL_NTHREADS","IDL_CPU_VECTOR_ENABLE","IDL_DATAFILE_USER_COMMENTS","IDL_DEVICE","IDL_DIR","IDL_DLM_PATH","IDL_EDIT_INPUT","IDL_EXCEPT","IDL_GR_TILECACHESIZE","IDL_GR_TTCACHESIZE","IDL_GR_X_COLORS","IDL_GR_X_DEPTH","IDL_GR_X_HEIGHT","IDL_GR_X_ONTOP","IDL_GR_X_QSCREEN","IDL_GR_X_RENDERER","IDL_GR_X_RETAIN","IDL_GR_X_VISUAL","IDL_GR_X_WIDTH","IDL_HELP_PATH","IDL_MAKE_DLL_COMPILE_DIRECTORY","IDL_MDE_EDIT_BACKUP","IDL_MDE_EDIT_COMPILE_OPTION","IDL_MDE_EDIT_CWD","IDL_MDE_EDIT_READONLY","IDL_MDE_EXIT_CONFIRM","IDL_MDE_LOG_LINES","IDL_MDE_LOG_TRIM","IDL_MDE_SPLASHSCREEN","IDL_MDE_START_DIR","IDL_MORE","IDL_MSG_PREFIX","IDL_PATH","IDL_PATH_CACHE_DISABLE","IDL_PREF_OBSOLETE_MIGRATE","IDL_PREF_OBSOLETE_WARN","IDL_PROMPT","IDL_QUIET","IDL_RBUF_PERSIST","IDL_RBUF_SIZE","IDL_STARTUP","IDL_START_DIR","IDL_TMPDIR"]

ret[0]=ptr_new(!CPU.TPOOL_MAX_ELTS)
ret[1]=ptr_new(!CPU.TPOOL_MIN_ELTS)
ret[2]=ptr_new(!CPU.TPOOL_NTHREADS)
ret[3]=ptr_new(!CPU.VECTOR_ENABLE)
ret[4]=ptr_new(1) ; getenv("IDL_DATAFILE_USER_COMMENTS"))
ret[5]=ptr_new(!D.NAME)
ret[6]=ptr_new(!DIR)
ret[7]=ptr_new("<IDL_DEFAULT>") ; getenv("IDL_DLM_PATH")
ret[8]=ptr_new(!EDIT_INPUT)
ret[9]=ptr_new(!EXCEPT)
ret[10]=ptr_new(512) ;getenv("IDL_GR_TILECACHESIZE")
ret[11]=ptr_new(256) ;getenv("IDL_GR_TTCACHESIZE")
ret[12]=ptr_new(-10) ;getenv("IDL_GR_X_COLORS")
ret[13]=ptr_new(24)  ;getenv("IDL_GR_X_DEPTH")
ret[14]=ptr_new(512) ;getenv("IDL_GR_X_HEIGHT")
ret[15]=ptr_new(1)   ;getenv("IDL_GR_X_ONTOP")
ret[16]=ptr_new(1)   ;getenv("IDL_GR_X_QSCREEN")
ret[17]=ptr_new(0)   ;getenv("IDL_GR_X_RENDERER")
ret[18]=ptr_new(2)   ;getenv("IDL_GR_X_RETAIN")
ret[19]=ptr_new(5)   ;getenv("IDL_GR_X_VISUAL:           5
ret[20]=ptr_new(640) ;getenv("IDL_GR_X_WIDTH")
ret[21]=ptr_new("<IDL_DEFAULT>") ;!HELP_PATH)
ret[22]=ptr_new(getenv("IDL_MAKE_DLL_COMPILE_DIRECTORY"))
ret[23]=ptr_new(1)   ;getenv("IDL_MDE_EDIT_BACKUP")
ret[24]=ptr_new(0)   ;getenv("IDL_MDE_EDIT_COMPILE_OPTION")
ret[25]=ptr_new(0)   ;getenv("IDL_MDE_EDIT_CWD")
ret[26]=ptr_new(0)   ;getenv("IDL_MDE_EDIT_READONLY")
ret[27]=ptr_new(1)   ;getenv("IDL_MDE_EXIT_CONFIRM")
ret[28]=ptr_new(1000);getenv("IDL_MDE_LOG_LINES:        1000
ret[29]=ptr_new(250) ;getenv("IDL_MDE_LOG_TRIM:         250
ret[30]=ptr_new(1)   ;getenv("IDL_MDE_SPLASHSCREEN:           1
ret[31]=ptr_new(getenv("IDL_MDE_START_DIR"))
ret[32]=ptr_new(!MORE)
ret[33]=ptr_new("%")
ret[34]=ptr_new("<IDL_DEFAULT>")
ret[35]=ptr_new(0)   ;getenv("IDL_PATH_CACHE_DISABLE:           0
ret[36]=ptr_new(1)   ;getenv("IDL_PREF_OBSOLETE_MIGRATE:           1
ret[37]=ptr_new(1)   ;getenv("IDL_PREF_OBSOLETE_WARN:           1
ret[38]=ptr_new(!PROMPT)
ret[39]=ptr_new(!QUIET)
ret[40]=ptr_new(1)   ;getenv("IDL_RBUF_PERSIST")
ret[41]=ptr_new(1000);getenv("IDL_RBUF_SIZE:       10000
ret[42]=ptr_new(getenv("IDL_STARTUP"))
ret[43]=ptr_new(getenv("IDL_START_DIR"))
ret[44]=ptr_new(getenv("IDL_TMPDIR"))
if keyword_set(names_all) then begin
   answer=strarr(45)
   for i=0,44 do answer[i]=preferences[i]+":"+strtrim(*ret[i],2)
   return, answer
endif
if n_elements(p) eq 0 then Message,"Incorrect number of arguments."
p=strupcase(temporary(p))
w=where(preferences eq p, count)
if count lt 1 then Message,"Unknown preference: "+p
return,*ret[w[0]]
end
