;
; Testing IDLffxmlSaX
;
; Initial version : 2019-05-06 : GD 
; ---------------------
; Modification History
; - 2026-11-18 : dasergatskov : first corrections for relative path
; - 2026-11-25 : AC : add a method to return value, full path management
; (see PR 2281)
; ---------------------
;
PRO TEST_XMLSAX, no_exit=no_exit, test=test
;
errors=0
txt='Missing mandatoty file : '
;
xml_def_file='ob_xml_to_struct__define.pro'
ob_xml_def=FILE_SEARCH_FOR_TESTSUITE(xml_def_file)
if STRLEN(ob_xml_def) EQ 0 then ERRORS_ADD, errors, txt+xml_def_file
;
xml_data_file='obxml.xml'
ob_xml_data=FILE_SEARCH_FOR_TESTSUITE(xml_data_file)
if STRLEN(ob_xml_data) EQ 0 then ERRORS_ADD, errors, txt+xml_data_file
;
; We continue only if both files are around !
;
if (errors EQ 0) then begin
   ;;
   ;; should work id the file "def_file" is in the PATH
   a=OBJ_NEW("ob_xml_to_struct")
   ;;
   ;; testing if "a" is really defined
   if ~ISA(a,'objref') then ERRORS_ADD, errors, 'Bad init for ObjRef'
   ;;
   print, 'Should print : Diameter: 0.448'
   a->ParseFile, ob_xml_data
   ;;
   res = a->GetStruct()
   if (res.diameter NE '0.448') then ERRORS_ADD, errors, 'Bad Diameter 1'
   ;;
endif
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_XMLSAX', errors
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end


END
