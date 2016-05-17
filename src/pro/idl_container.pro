;+
;
; NAME:
;   IDL_CONTAINER
;
; PURPOSE:
;    create an IDL container 
;
; :Categories:
;    Object Programming, Graphics
;
; this provides the procedure interface to creating an IDL_container.
;
; idl_container__define.pro will further defines this to be fully inherited from 
; GDL_CONTAINER, which is a built-in object of GDL.
;
;---------------------------------------------------------------------------
Function IDL_CONTAINER

   Compile_Opt hidden
   return, obj_new('idl_container')
END  
