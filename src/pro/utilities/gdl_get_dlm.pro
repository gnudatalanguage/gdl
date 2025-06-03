function gdl_get_dlm_info, file
COMPILE_OPT idl2, HIDDEN  
;  print,file
  nlines=FILE_LINES(file)
  if nlines lt 1 then return,""
  lines=strarr(nlines)
  openr,lun,file,/get
  readf,lun,lines
  free_lun,lun
  
  return, lines
end

pro decipher_dlm_line,subline,rtname,minargs,maxargs,option,gdl_kw
  COMPILE_OPT idl2, HIDDEN
  blank=string([32b,9b])
;  print,"subline: "+subline
  z=strsplit(subline,blank,/extract)
  nn=n_elements(z)
  rtname=strlowcase(z[0])       ;
  minargs=0
  maxargs=0
  option=""
  if nn gt 1 then reads,z[1],minargs
  ; maxargs can be IDL_MAXPARAMS (65535) or IDL_MAX_ARRAY_DIM (8)
  if nn gt 2 then begin
     if z[2] eq 'IDL_MAXPARAMS' then maxargs=65535 else $
     if z[2] eq 'IDL_MAX_ARRAY_DIM' then maxargs=8 else reads,z[2],maxargs
  endif
  if nn gt 3 then if z[3] ne 'OBSOLETE' then option=z[3]
  if nn gt 4 then begin
     gdl_kw=strarr(nn-4)
     for k=4,nn-1 do gdl_kw[k-4]=z[k]
  endif
end
  

 
; used either with a single file, a file list (must be fully qualified) or nothing

function gdl_get_dlm,file
  COMPILE_OPT idl2, HIDDEN
  if n_elements(file) eq 0 then filelist=file_search(STRSPLIT(!DLM_PATH, PATH_SEP(/SEARCH_PATH),/extract)+'/*.dlm') else filelist=file
  nfiles = n_elements(filelist)
  if n_elements(filelist[0]) eq 0 then return, 0
  for ifile=0,nfiles-1 do begin
;     print,'FILE: '+filelist[ifile] & print
     sl=strlen(filelist[ifile])-4   ; .dlm
     image=strmid(filelist[ifile],0,sl)+".so"
     s=gdl_get_dlm_info(filelist[ifile])
     n=n_elements(s)
     if s[0] eq "" then break ; return,0
     ; trim # comments
     findpos=strpos(s , "#")
     w=where(findpos eq -1, /null)
     theend=findpos
     theend[w]=1000000                          ; just to make it simple
     for j=0,n-1 do s[j]=strmid(s[j],0,theend[j]) ; s is without comments

     ; module   
     findpos=strpos(s , "MODULE")
     w=where(findpos gt -1, count)
     if (count ne 1) then begin
        message,"dlm file "+file+" is not a dlm file."
        break
     endif

     ; find module name
     modulename=strtrim(strmid(s[w],findpos[w]+7,strlen(s[w])),2)
;     print,"module name: "+modulename

     ;get functions or procedures
     findpos=strpos(s , "PROCEDURE")
     w=where(findpos gt -1, count)
     if (count ge 1) then begin
        for ipro=0,count-1 do begin
           CATCH, Error_status
           IF Error_status NE 0 THEN BEGIN
              CATCH, /CANCEL
              BREAK ; a problem occured, this file has problems
           ENDIF
           iline=w[ipro]
           isfunct=0
           subline=strmid(s[iline],findpos[iline]+10,strlen(s[iline]))
           decipher_dlm_line,subline,rtname,minargs,maxargs,option,gdl_kw
           linkimage,rtname,image,funct=isfunct,min_args=minargs,max_args=maxargs,keywords=strlen(option)
nextpro:
        endfor
     endif
     
     findpos=strpos(s , "FUNCTION")
     w=where(findpos gt -1, count)
     if (count ge 1) then begin
        for ifun=0,count-1 do begin
           CATCH, Error_status
           IF Error_status NE 0 THEN BEGIN
              CATCH, /CANCEL
              BREAK ; a problem occured, this file has problems
           ENDIF
           iline=w[ifun]
           isfunct=1
           subline=strmid(s[iline],findpos[iline]+9,strlen(s[iline]))
           decipher_dlm_line,subline,rtname,minargs,maxargs,option,gdl_kw
           linkimage,rtname,image,funct=isfunct,min_args=minargs,max_args=maxargs,keywords=strlen(option)
nextfunc:
        endfor
     endif
  endfor


  return,1
end

