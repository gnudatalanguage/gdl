;
; Testing IDLffxmlSaX
; 2019-05-06 : GD 
; ---------------------
;
PRO TEST_XMLSAX
a=obj_new("ob_xml_to_struct")
; Locate the fixture beside this test, independently of the working directory.
a->ParseFile, FILEPATH("obxml.xml", ROOT_DIR=FILE_DIRNAME(ROUTINE_FILEPATH()))
; should print: "Diameter: 0.448"
END
