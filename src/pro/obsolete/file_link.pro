;+
; NAME: FILE_LINK
;
; PURPOSE: dummy routine to inform user
; The built-in file_lnk would return, " FILE_LINK is not featured for windows systems"
; But different methods of creating file_links may be used in a windows' system.
; So the C++ file_link is removed and this is placed in src/pro: 
; 
; CATEGORY: placeholder
;
; CALLING SEQUENCE:  FILE_LINK,SourcePath, DestPath [, /ALLOW_SAME] [, /HARDLINK] [, /NOEXPAND_PATH] [, /VERBOSE]
;
; One way I know how to do this: SPAWN,"ln -s target link " 
; If you run from an MSYS shell, and export MSYS=winsymlinks:nativestrict
;-
;
pro FILE_LINK, SourcePath, DestPath,   verbose=verbose, test=test
;
if ~(!version.os_family eq 'Windows') then $
  message,/continu,' file_link.pro is a dummy (placeholder) routine' $
  else $
  message,/continu," FILE_LINK is not featured for MS-windows systems"
;
    if (!version.os_family eq 'Windows') then $
    if (getenv('MSYS') eq 'winsymlinks:nativestrict') then symlink = 'MSYS' else $
    symlink = 'NO'
  if keyword_set(verbose) and (symlink eq 'MSYS') then $
    message,/continue,' in MSYS2, Running as administrator, can make a link by SPAWN,"ln -s target link " ''
;
if KEYWORD_SET(test) then STOP
return
;
end
