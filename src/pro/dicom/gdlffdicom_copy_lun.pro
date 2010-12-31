;L+
; LICENSE:
;
; IDL user contributed source code
; Copyright (C) 2006 Robbie Barnett
;
;    This library is free software;
;    you can redistribute it and/or modify it under the
;    terms of the GNU Lesser General Public License as published
;    by the Free Software Foundation; 
;    either version 2.1 of the License, 
;    or (at your option) any later version.
;
;    This library is distributed in the hope that it will
;    be useful, but WITHOUT ANY WARRANTY;
;    without even the implied warranty of MERCHANTABILITY
;    or FITNESS FOR A PARTICULAR PURPOSE. 
;    See the GNU Lesser General Public License for more details.
;
;    You should have received a copy of the GNU Lesser General Public License
;    along with this library; if not, write to the
;    Free Software Foundation, Inc., 59 Temple Place,
;    Suite 330, Boston, MA 02111-1307 USA 
;
; Please send queries to:
; Robbie Barnett
; Nuclear Medicine and Ultrasound
; Westmead Hospital
; +61 2 9845 7223
;L-


; Introduced for compatibility with IDL
pro gdlffdicom_copy_lun, fromUnit, toUnit, Num
if (num gt 0) then begin
    buffer = bytarr(Num)
    readu, fromUnit, buffer
    writeu, toUnit, temporary(buffer)
endif
end
