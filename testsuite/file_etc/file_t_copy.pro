;+
; NAME: FILE_T_COPY
; 
; RESTRICTIONS:  only for Unix (Unix, Linux and Mac OS X) systems
;
;-
; LICENCE:
; Copyright (C) 2010, Lea Noreskal; contact: Alain Coulais
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;-
; ---------------------------------
;
pro FILE_T_COPY, source, dest, require_directory=require_directory, $
               allow_same=allow_same, noexpand_path=noexpand_path, recursive=recursive, $
               overwrite=overwrite, quiet=quiet, verbose=verbose, help=help, test=test ;, $
;not ready : force=force, copy_named_pipe=copy_named_pipe, copy_symlink=copy_symlink
;
if (N_PARAMS() LT 2) then BEGIN
    print, '% FILE_T_COPY: Incorrect number of arguments.'
    return
endif
;

if (Not KEYWORD_SET(noexpand_path)) then begin  
    
    flist=''
    for ii=0,N_ELEMENTS(source)-1 do begin
        
        dir=FILE_SEARCH(FILE_DIRNAME(source[ii]),/fully)
        res=FILE_SEARCH(dir, FILE_BASENAME(source[ii]), /fully)

        for jj=0,N_ELEMENTS(res)-1 do begin
            if(FILE_DIRNAME(res[jj], /MARK_DIRECTORY) eq dir) then flist=[flist, res[jj]] ;
            ;print, 'dir ', dir, ' res ', FILE_DIRNAME(res[jj])
        endfor

    endfor

    if(N_ELEMENTS(flist) gt 1) then source=flist[1:*]; print, flist ;;

; ...
    for ii=0,N_ELEMENTS(dest)-1 do begin
      struct=FILE_INFO(dest[ii])
      if(struct.exists) then dest[ii]=escape_special_char(struct.name)
    endfor
;... 


endif



; either dest has the same size as source or it is a directory
destIsDir=0 ; 0 if dest is an array, 1 if it is a directory

if ( N_ELEMENTS(source) ne N_ELEMENTS(dest)) then begin
   if( (N_ELEMENTS(dest) gt 1) $
 or (FILE_TEST(dest[0], /directory) eq 0) ) then begin    
      print, '% FILE_T_COPY:  Arrays source and dest must have same size, or dest must be a directory'
      return
   endif
   ; N_elements(dest) = 1 and dest is a directory
   destIsDir=1
endif 
;

;
if KEYWORD_SET(help) then begin
   print, 'FUNCTION FILE_DELETE, source, dest,'
   print, '          allow_same=allow_same, noexpand_path=noexpand_path, recursive=recursive, $'
   print, '          overwrite=overwrite, require_directory=require directory, quiet=quiet, verbose=verbose, help=help, test=test'
   ;print, '          UNIX-Only Keywords: [/COPY_NAMED_PIPE] [, /COPY_SYMLINK] [, /FORCE] '
   return
endif
;

command='cp '
option='-'

;if KEYWORD_SET(allow_same) then begin
;   print, 'Sorry, Keyword allow_same is not available now.'
;   return
;endif
;



if KEYWORD_SET(require_directory) then begin

   ; verify if dest contains only directories
   for ii=0, N_ELEMENTS(dest)-1 do begin
      if ( FILE_TEST(dest[ii], /directory) eq 0 ) then begin 
         print, '% FILE_T_COPY: require_directory, ', dest[ii], ' is not a valid directory '
         return
      endif
   endfor
endif
;
if KEYWORD_SET(recursive) then begin
   option=option+'r'
endif
;
;if KEYWORD_SET(noexpand_path) then begin
;   print, 'Sorry, Keyword NOEXPAND_PATH is not available now.'
;   return
;endif
;
;cp=0;
;if KEYWORD_SET(overwrite) then begin
;endif
;
if KEYWORD_SET(copy_named_pipe) then begin
   print, 'Sorry, Keyword copy_named_pipe is not available now.'
   return
endif
;
if KEYWORD_SET(copy_symlink) then begin
   option=option+'d'
endif
;
if KEYWORD_SET(force) then begin
   option=option+'f'
endif
;
;if KEYWORD_SET(quiet) then begin
;   print, 'Sorry, Keyword quiet is not available now.'
;   return
;endif
;
if KEYWORD_SET(verbose) then begin
   option=option+'v'
endif
;


;
if (STRLEN(option) GT 1 ) then begin
   command=command+option
   ;print, 'opt >', option, ' Comm : ', command
endif
;

for ii=0, N_ELEMENTS(source)-1 do begin
 
   if not KEYWORD_SET(recursive) then begin
      if (FILE_TEST(source[ii], /directory) eq 1 ) then begin
         if (KEYWORD_SET(verbose) or not KEYWORD_SET(quiet)) then $
			print, '% FILE_T_COPY: ', source[ii], ' is a directory, use option /recursive'
         return
      endif
   endif 

   ; escape special characters
   snameesc=escape_special_char(source[ii])
   destname=dest[0] ; dest is a directory
    
   ;
   if(destIsDir eq 0) then destname=dest[ii] ; dest is an array with several destination
   ;
   dnameesc=escape_special_char(destname)

   ;if(KEYWORD_SET(allow_same) and (snameesc eq dnameesc)) > ne rien faire


    ;verify if it is possible to copy, without keyword overwrite
    cp=1;
    if ((Not KEYWORD_SET(overwrite)) and FILE_TEST(destname,/directory)) then begin  
        ;sname=get_filename(source[ii]) ;
        sname=FILE_BASENAME(source[ii]) 

        res = FILE_SEARCH(destname, sname,  COUNT=nb)
        ;print ,'RES : ' , res, ' sname : ', sname
        if (nb ne 0) then cp=0 ; there is not same files in directory
    endif
;	if !version.OS_family ne 'unix' then $
;		print, command+' '+snameesc+' '+dnameesc
   ;copy
   if((cp eq 1) and (not KEYWORD_SET(allow_same) or (snameesc ne dnameesc))) then $
		SPAWN, command+' '+snameesc+' '+dnameesc
   if(cp eq 0 and (KEYWORD_SET(verbose) or not KEYWORD_SET(quiet))) then $
	print, '% FILE_T_COPY: ', source[ii] ,' overwrite not allowed '

endfor



;
if KEYWORD_SET(test) then STOP
;
end




