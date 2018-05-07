;+
; NAME: FILE_MOVE
; 
; RESTRICTIONS:  only for Unix (Unix, Linux and Mac OS X) systems
;
;-
; LICENCE:
; Copyright (C) 2015, NATCHKEBIA Ilia; contact: Alain Coulais
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;-
; ------------------------
;
; Modifications history :
;
; - 2018-04-10 : AC 
;  * remove of no-more-need of ESCAPE_SPECIAL_CHAR()
;  * few cleaning ...
;
; ------------------------
;
pro FILE_MOVE, source, dest, require_directory=require_directory, $
               allow_same=allow_same, noexpand_path=noexpand_path, $
               overwrite=overwrite, verbose=verbose, help=help, test=test

;
if KEYWORD_SET(help) then begin
   print, 'pro FILE_MOVE, source, dest, $'
   print, '            allow_same=allow_same, noexpand_path=noexpand_path, recursive=recursive, $'
   print, '            overwrite=overwrite, require_directory=require directory, $'
   print, '            verbose=verbose, help=help, test=test'
   return
endif
;
if (N_PARAMS() LT 2) then BEGIN
   MESSAGE, 'Incorrect number of arguments.'
endif
;
if ~KEYWORD_SET(noexpand_path) then begin  
   ;;
   flist=''
   for ii=0, N_ELEMENTS(source)-1 do begin
      ;; 
      dir=FILE_SEARCH(FILE_DIRNAME(source[ii]),/fully)
      res=FILE_SEARCH(dir, FILE_BASENAME(source[ii]), /fully)
      ;;
      for jj=0,N_ELEMENTS(res)-1 do begin
         if (FILE_DIRNAME(res[jj]) eq dir) then flist=[flist, res[jj]] ;
         ;; print, 'dir ', dir, ' res ', FILE_DIRNAME(res[jj])
      endfor
   endfor
   ;;
   if (N_ELEMENTS(flist) gt 1) then source=flist[1:*] ; print, flist ;;
   ;; ...
   for ii=0,N_ELEMENTS(dest)-1 do begin
      struct=FILE_INFO(dest[ii])
      if (struct.exists) then dest[ii]=ESCAPE_SPECIAL_CHAR(struct.name)
   endfor
   ;;
endif


; either dest has the same size as source or it is a directory
destIsDir=0                     ; 0 if dest is an array, 1 if it is a directory


if ( N_ELEMENTS(source) ne N_ELEMENTS(dest)) then begin
   if( (N_ELEMENTS(dest) gt 1) or (FILE_TEST(dest[0], /directory) eq 0) ) then begin    
      MESSAGE, 'Arrays source and dest must have same size, or dest must be a directory!'
   endif
   ;; N_elements(dest) = 1 and dest is a directory
   destIsDir=1
endif 
;
if KEYWORD_SET(require_directory) then begin
   ;; verify if dest contains only directories
   for ii=0, N_ELEMENTS(dest)-1 do begin
      if (FILE_TEST(dest[ii], /directory) eq 0 ) then begin 
         MESSAGE, 'require_directory, ', dest[ii], ' is not a valid directory '
      endif
   endfor
endif
;
if KEYWORD_SET(verbose) then begin
   option=option+'v'
endif
;
;
command='\cp '
option='-rf'
;
if (STRLEN(option) GT 1 ) then begin
   command=command+option
   ;;print, 'opt >', option, ' Comm : ', command
endif
;

for ii=0, N_ELEMENTS(source)-1 do begin
   ;;
   ;;test if file exists
   if (FILE_TEST(source[ii]) eq 0) then begin
      MESSAGE, 'file >>'+source[ii]+'<< does not exist! '
   endif
   ;; escape special characters
   snameesc=ESCAPE_SPECIAL_CHAR(source[ii])
   ;;
   ;; dest is a directory
   destname=dest[0]
   ;;
   ;; dest is an array with several destinatioN
   if(destIsDir eq 0) then destname=dest[ii]
   ;;
   ;;escape space characters
   dnameesc=ESCAPE_SPECIAL_CHAR(destname)
   ;;
   if (KEYWORD_SET(allow_same) and (snameesc eq dnameesc)) then return
   ;;
   ;;verify if it is possible to copy, without keyword overwrite
   cp=1
   if ~KEYWORD_SET(overwrite) then begin
      nb=FILE_TEST(destname)
      if (nb eq 0) then cp=0    ; there is not same files in directory
   endif
                                ;copy
   if((cp eq 0) or KEYWORD_SET(overwrite) and destIsDir eq 0) then begin
      SPAWN, command+' '+snameesc+' '+dnameesc
      SPAWN, 'rm '+option+' '+snameesc
   endif else begin
      if (destIsDir eq 1) then begin
         SPAWN, command+' '+snameesc+' '+dnameesc+'/'
         SPAWN, 'rm '+option+' '+snameesc
         
;NEEDS TO IMPLEMENT!!!!
;If DestPath specifies an existing directory and SourcePath also names a directory, FILE_MOVE checks for the existence of a subdirectory of DestPath with the same name as the source directory. If this subdirectory does not exist, the source directory is moved to the specified location. If the subdirectory does exist, an error is issued, and the rename operation is not carried out.
      endif else begin
         if(not KEYWORD_SET(quiet)) then print, '% FILE_MOVE: ', 'Destination file already exists. File: ', dest[ii]
      endelse
   endelse

endfor

;

;
if KEYWORD_SET(test) then STOP
;
end




