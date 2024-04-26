; this is a stub
function app_user_dir, authordirname, authordesc, appdirname, appdesc,$
   appreadmetext, appreadmeversion, $
     author_readme_text=author_readme_text , $
     author_readme_version=author_readme_version, $
     restrict_appversion=restrict_appversion, $
     restrict_arch=restrict_arch , $
     restrict_family=restrict_family ,$
     restrict_file_offset_bits=restrict_file_offset_bits , $
     restrict_idl_release=restrict_idl_release , $
     restrict_memory_bits=restrict_memory_bits , $
     restrict_os=restrict_os
; returns only one path, .gdl where it is
  h=getenv("HOME")
  if strlen(h) lt 1 then h=getenv("HOMEPATH")
  if  strlen(h) lt 1 then return, ""
  return, h+path_sep()+".gdl"
end
