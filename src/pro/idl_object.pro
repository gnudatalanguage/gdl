;+
;
; NAME:
;   IDL_OBJECT
;
; PURPOSE:
;    create an 'IDL_OBJECT' object 
;
; :Categories:
;    Object Programming, Graphics
;
; this provides the procedure interface to creating an IDL_OBJECT.
;
; IDL_OBJECT is a built-in reference to GDL_OBJECT
;
;---------------------------------------------------------------------------
Function IDL_OBJECT

   Compile_Opt hidden
   return, obj_new('idl_object')
END  
