;
; Testing IDLffxmlSaX
; 2019-05-06 : GD 
; ---------------------
;
PRO TEST_XMLSAX
a=obj_new("ob_xml_to_struct")
a->ParseFile,"obxml.xml"
; should print: "Diameter: 0.448"
END
