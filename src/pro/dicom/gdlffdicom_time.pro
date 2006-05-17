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

; function gdlffdicom_seconds, date_str, time_str
; READS, date_str, year, imon, day, FORMAT="(I04,I02,I02)" 
; READS, time_str, hour,minute,second,FORMAT="(I02,I02,I02)"
; dow = 'Mon'
; s1 = ':'
; s2 = ':'
; mon = (['',' Jan',' Feb',' Mar',' Apr',' May',' Jun',' Jul',' Aug',' Sep',' Oct',' Nov',' Dec'])[imon]
; str = string(dow, mon, day, hour, s1, minute, s2, second, year, FORMAT="(A3,A4,I3,I3,A1,I02,A1,I02,I5)")
; help, str, year, imon, day, hour, minute, second
; return, systime(0,str)
; ; help, gdlffdicom_seconds(gdlffdicom_date(),gdlffdicom_time())
; end


function gdlffdicom_time, elapsed_seconds, UTC=utc
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
;imon = where(mon eq ['',' Jan',' Feb',' Mar',' Apr',' May',' Jun',' Jul',' Aug',' Sep',' Oct',' Nov',' Dec'],count)
;help, mon
return, string(hour,minute,second,FORMAT="(I02,I02,I02,'.000000')") 
;print, dow, mon, day, hour, s1, minute, s2, second, year, FORMAT="(A3,A4,I3,I3,A1,I02,A1,I02,I5)"
;help, dow, mon, day, hour, minute, second, year
end
