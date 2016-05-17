;+
;
; NAME:
;   IDL_CONTAINER__Define
;
; PURPOSE:
;    define a compatible idl_container by inheriting from the GDL_CONTAINER
; :Categories:
;    Object Programming, Graphics
;
;
;   This is the class definition module. 
;
;---------------------------------------------------------------------------
PRO IDL_CONTAINER__Define, class

   Compile_Opt hidden
   class = { IDL_CONTAINER, $
              INHERITS GDL_CONTAINER $
           }

END  
