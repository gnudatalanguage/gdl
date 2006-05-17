;L+
; LICENSE:
; Copyright 2004/2005 Robbie Barnett
; Robbie's Tools (RT)
; Tools written for applications at Westmead Hospital, Sydney. 
; These tools come with absolutley no warranty and are not necessarily
; built with other applications in mind.
; Please send queries to:
; Robbie Barnett
; Nuclear Medicine and Ultrasound
; Westmead Hospital
; +61 2 9845 7223
; The library is distributed under the terms of the Gnu 
; general public license. A copy of the GPL (gpl.txt) should be  
; available in this distribution
; You're free to copy, modify and further distribute the library
; itself as a whole (incl. this README.txt and a copy of the
; GPL) under the terms of the license. However I'd be pleased to
; hear from you - any feedback is welcome.
;L-


; Introduced for compatibility with IDL
pro gdlffdicom_copy_lun, fromUnit, toUnit, Num
if (num gt 0) then begin
    buffer = bytarr(Num)
    readu, fromUnit, buffer
    writeu, toUnit, temporary(buffer)
endif
end
