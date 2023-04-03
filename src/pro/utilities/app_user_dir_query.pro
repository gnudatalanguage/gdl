; this is a stub
function app_user_dir_query, authordirname, appdirname, $
   count=count, exclude_current=exclude_current, $
   query_appversion=query_appversion, query_arch=query_arch,$
   query_family=query_family, $
   query_file_offset_bits=query_file_offset_bits,$
   query_idl_release=query_idl_release, $
     query_memory_bits=query_memory_bits, query_os= query_os, $
     author_readme_text=author_readme_text , $
     author_readme_version=author_readme_version, $
     restrict_appversion=restrict_appversion, $
     restrict_arch=restrict_arch , $
     restrict_family=restrict_family ,$
     restrict_file_offset_bits=restrict_file_offset_bits , $
     restrict_idl_release=restrict_idl_release , $
     restrict_memory_bits=restrict_memory_bits , $
   restrict_os=restrict_os

  count=1
; returns only one path, .gdl where it is
  h=getenv("HOME")
  if strlen(h) lt 1 then h=getenv("HOMEPATH")
  if  strlen(h) lt 1 then return, ""
  return, h+path_sep()+".gdl"
end
