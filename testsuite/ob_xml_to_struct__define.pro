; very simple test: prints content of an obxml file
;
; Init method.
; Called when the ob_xml_to_struct object is created.

FUNCTION ob_xml_to_struct::Init
  RETURN, self->IDLffXMLSAX::Init()
END

;---------------------------------------------------------------------------
; Cleanup method.
; Called when the ob_xml_to_struct object is destroyed.

PRO ob_xml_to_struct::Cleanup
                                ; Call superclass cleanup method
  self->IDLffXMLSAX::Cleanup
END

; commented out: no need to overload the method.
; ;---------------------------------------------------------------------------
; ; StartDocument method
; ; Called when parsing of the document data begins.
; 
; PRO ob_xml_to_struct::StartDocument
; END

;---------------------------------------------------------------------------
; Characters method
; Called when parsing character data within an element.
; Adds data to the element field.

PRO ob_xml_to_struct::characters, data
  self.element = self.element + data
END

;---------------------------------------------------------------------------
; StartElement
; Called when the parser encounters the start of an element.
; just reset element

PRO ob_xml_to_struct::startElement, URI, local, strName, attr, value
  self.element = ''
END


;---------------------------------------------------------------------------
; EndElement method
; Called when the parser encounters the end of an element.
; just print element

PRO ob_xml_to_struct::EndElement, URI, Local, strName
   case strName of
   "DIAMETER": self.print=1
   else: self.print=0
   endcase

  if (self.print) then begin
     string=strtrim(self.element,2)
     self.element = ''
     print, "Diameter: "+string
  ENDIF

END

;---------------------------------------------------------------------------
; Object class definition method.

PRO ob_xml_to_struct__define

   void = {ob_xml_to_struct, $
           INHERITS IDLffXMLSAX, $
           element: '', print: 0}
    
END
