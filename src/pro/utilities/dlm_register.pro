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

pro decipher_dlm_line,subline,rtname,entry,minargs,maxargs,option,gdl_kw
  COMPILE_OPT idl2, HIDDEN
  blank=string([32b,9b])
;  print,"subline: "+subline
  z=strsplit(subline,blank,/extract)
  nn=n_elements(z)
  entry=z[0]       ; no lowcase or upcase
  rtname=strupcase(entry)
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
; silent is a GDL extension
pro dlm_register,filein,silent=silent,verbose=verbose
  COMPILE_OPT idl2, HIDDEN
  if n_elements(filein) eq 0 then filelist=file_search(STRSPLIT(!DLM_PATH, PATH_SEP(/SEARCH_PATH),/extract)+'/*.dlm') else filelist=filein
  nfiles = n_elements(filelist)
  if nfiles eq 1 and filelist[0] eq "" then begin
     if keyword_set(silent) then return
     Message,"Incorrect number of arguments."
  endif
  

  case !version.os of
     "linux": ext=".so"
     "darwin": ext=".so"
     "windows": ext=".dll"
  endcase

  for ifile=0,nfiles-1 do begin
     file=filelist[ifile]
     file=file_expand_path(file)
     sl=strlen(file)-4 ; .dlm
     image=strmid(file,0,sl)+ext
if keyword_set(verbose)  then      print,'image: '+image & print
     s=gdl_get_dlm_info(file)
     n=n_elements(s)
     if s[0] eq "" then break
     ; check if this is a GDL-native DLL
     findpos=strpos(s , "#%GDL_DLM")
     is_gdl = findpos[0] gt -1
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
if keyword_set(verbose)  then      print,"module name: "+modulename
     ; prepare dlm_info to pass to linkimage
     dlm_info=strarr(4)
     dlm_info[0]=modulename
     ; description
     findpos=strpos(s , "DESCRIPTION")
     w=where(findpos gt -1, count)
     if (count ge 1) then begin
        info=strtrim(strmid(s[w[0]],findpos[w[0]]+12,strlen(s[w[0]])),2)
        dlm_info[1]=info
     endif
     ; version
     findpos=strpos(s , "VERSION")
     w=where(findpos gt -1, count)
     if (count ge 1) then begin
        version=strtrim(strmid(s[w[0]],findpos[w[0]]+8,strlen(s[w[0]])),2)
        dlm_info[2]="Version: "+version
     endif
     ; build
     findpos=strpos(s , "BUILD")
     w=where(findpos gt -1, count)
     if (count ge 1) then begin
        build=strtrim(strmid(s[w[0]],findpos[w[0]]+6,strlen(s[w[0]])),2)
        dlm_info[2]+=", Build_Date: "+build
     endif
     ; source
     findpos=strpos(s , "SOURCE")
     w=where(findpos gt -1, count)
     if (count ge 1) then begin
        source=strtrim(strmid(s[w[0]],findpos[w[0]]+7,strlen(s[w[0]])),2)
        dlm_info[2]+=", Source: "+source
     endif
     dlm_info[3]="Path: "+image
     
     ;get functions or procedures
     findpos=strpos(s , "PROCEDURE")
     w=where(findpos gt -1, count)
     if (count ge 1) then begin
        for ipro=0,count-1 do begin
	       if dlm_info ne !NULL and ipro gt 0 then dlm_info=!NULL
if not keyword_set(verbose) then begin
           CATCH, Error_status
           IF Error_status NE 0 THEN BEGIN
              CATCH, /CANCEL
              BREAK ; a problem occured, this file has problems
           ENDIF
endif
           iline=w[ipro]
           isfunct=0
           subline=strmid(s[iline],findpos[iline]+10,strlen(s[iline]))
           decipher_dlm_line,subline,rtname,entry,minargs,maxargs,option,gdl_kw
          if keyword_set(verbose)  then print,"linkimage,""",rtname,""",""",image,'",',strtrim(isfunct,2),',"',entry,""",min_args=",strtrim(minargs,2),",max_args=",strtrim(maxargs,2)
           if (is_gdl) then begin
              linkimage,rtname,image,isfunct,entry,min_args=minargs,max_args=maxargs,keywords=gdl_kw,dlm_info=dlm_info,/NATIVE
           endif else linkimage,rtname,image,isfunct,entry,min_args=minargs,max_args=maxargs,keywords=strlen(option),dlm_info=dlm_info
nextpro:
        endfor
     endif
     
     findpos=strpos(s , "FUNCTION")
     w=where(findpos gt -1, count)
     if (count ge 1) then begin
        for ifun=0,count-1 do begin
	       if dlm_info ne !NULL and ifun gt 0 then dlm_info=!NULL
if not keyword_set(verbose) then begin
           CATCH, Error_status
           IF Error_status NE 0 THEN BEGIN
              CATCH, /CANCEL
              BREAK ; a problem occured, this file has problems
           ENDIF
endif
           iline=w[ifun]
           isfunct=1
           subline=strmid(s[iline],findpos[iline]+9,strlen(s[iline]))
           decipher_dlm_line,subline,rtname,entry,minargs,maxargs,option,gdl_kw
          if keyword_set(verbose)  then print,"linkimage,""",rtname,""",""",image,'",',strtrim(isfunct,2),',"',entry,""",min_args=",strtrim(minargs,2),",max_args=",strtrim(maxargs,2)
          if (is_gdl) then begin
           linkimage,rtname,image,isfunct,entry,min_args=minargs,max_args=maxargs,keywords=gdl_kw,dlm_info=dlm_info,/NATIVE
        endif else linkimage,rtname,image,isfunct,entry,min_args=minargs,max_args=maxargs,keywords=strlen(option),dlm_info=dlm_info
nextfunc:
        endfor
     endif
  endfor
end

