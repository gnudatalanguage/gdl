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
;    Free Software Foundation, Inc.
;    51 Franklin Street, Suite 500
;    Boston, MA 02110-1335, USA
;
; Please send queries to:
; Robbie Barnett
; Nuclear Medicine and Ultrasound
; Westmead Hospital
; +61 2 9845 7223
;L-

;+
; Return a DICOM formatted date string
; @field elsapsed_seconds
;-
function gdlffdicom_date, elapsed_seconds, UTC=utc
if (n_elements(elapsed_seconds)) then begin
    if (size(elapsed_seconds,/type) eq 7) then date_str = elapsed_seconds $
    else date_str = systime(0,double(elapsed_seconds), UTC=utc) 
endif else $
  date_str = systime(UTC=utc)
dow = ''
mon = ''
day = 0l
hour = 0l
minute = 0l
second = 0l
year = 0l
s1 = ':'
s2 = ':'
READS, date_str, dow, mon, day, hour, s1, minute, s2, second, year, FORMAT="(A3,A4,I3,I3,A1,I02,A1,I02,I5)"
imon = where(mon eq ['',' Jan',' Feb',' Mar',' Apr',' May',' Jun',' Jul',' Aug',' Sep',' Oct',' Nov',' Dec'],count)
if (count gt 0) then return, string(year,imon[0],day,FORMAT="(I04,I02,I02)") $
else message, 'No such month'
;  for i=0l,10000 do help, gdlffdicom_date(systime(1)+randomu(seed)*100000000.)
end
