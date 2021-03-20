FUNCTION LABEL_DATE, axis, index, x, level, DATE_FORMAT = format, MONTHS = months
;+
; NAME:
;	LABEL_DATE
;
; PURPOSE:
;	A function to label axes with dates.
; CATEGORY:
;	Plotting.
; CALLING SEQUENCE:
;	To set up:
;		dummy = LABEL_DATE(DATE_FORMAT='string')
;	Then to use:
;		PLOT, x, y, XTICKFORMAT='LABEL_DATE'
; INPUTS:
;	No explicit user defined inputs.  When called from the plotting
;	routines, the input parameters are (Axis, Index, Value)
; KEYWORD PARAMETERS:
;	DATE_FORMAT = a format string which may contain the following:
;		%M for month (3 character abbr)
;		%N for month (2 digit abbr)
;		%D for day of month,
;		%Y for 4 digit year.
;		%Z for last two digits of year.
;		%% is %.
;		Other characters are passed directly thru.
;		For example, '%M %D, %Y' prints DEC 11, 1993
;			'%M %2Y' yields DEC 93
;			'%D-%M' yields 11-DEC
;			'%D/%N/%Y' yields 11/12/1993
;			'%M!C%Y' yields DEC on the top line, 1993 on
;			the bottom (!C is the new line graphic command).
;	MONTHS = The names of the months, a twelve element string array.
;		If omitted, use Jan, Feb, ..., Dec.
; OUTPUTS:
;	The date string which is then plotted.
; COMMON BLOCKS:
;	LABEL_DATE_COM.
; SIDE EFFECTS:
;	None.
; RESTRICTIONS:
;	Only one date axis may be simultaneously active.
; PROCEDURE:
;	Straightforward.
; EXAMPLE:
;	For example, to plot from Jan 1, 1993, to July 12, 1994:
;	Start_date = julday(1, 1, 1993)
;	End_date = julday(7, 12, 1994)
;	Dummy = LABEL_DATE(DATE_FORMAT='%N/%D')  ;Simple mm/dd
;	x = findgen(end_date+1 - start_date) + start_date ;Time axis
;	PLOT, x, sqrt(x), XTICKFORMAT = 'LABEL_DATE', XSTYLE=1
;	(Plot with X axis style set to exact.)
;	
; MODIFICATION HISTORY:
;	DMS, RSI.	April, 1993.	Written.
;       GD              March 2021 added level
;-
 
COMMON label_date_com, fmt, month_chr
 
if keyword_set(format) then begin	;Save format string?
   if keyword_set(months) then month_chr = months else month_chr = ['Jan','Feb','Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
   fmt = format
   return, 0
endif

IF (n_elements(level) lt 1) then level = 0

if n_elements(month_chr) ne 12 or n_elements(fmt) le 0 then $
   message,' Not initialized.'

caldat, long(x), month, day, year ;Get the calendar date from julian
n = strlen(fmt[level])
out = ''

for i=0, n-1 do begin           ;Each format character...
   c = strmid(fmt[level], i, 1)        ;The character.
   if c eq '%' then begin
      i = i + 1
      c = strmid(fmt[level], i, 1)     ;The function
      case c of                 ;format character?
         'M' : out = out + month_chr(month-1)
         'N' : out = out + string(format='(i2.2)', month)
         'D' : out = out + string(format='(i2.2)', day)
         'Y' : out = out + string(format='(i4)', year)
         'Z' : out = out + string(format='(i2.2)', year  mod 100)
         '%' : out = out + '%'
         else : message, 'Illegal character in date format string: '+fmt[level]
      endcase
   endif else out = out + c
endfor
return, out
end
;DISCLAIMER:
;
;This software was written at the Cosmology Data Analysis Center in
;support of the Cosmic Background Explorer (COBE) Project under NASA
;contract number NAS5-30750.
;
;This software may be used, copied, modified or redistributed so long
;as it is not sold and this disclaimer is distributed along with the
;software.  If you modify the software please indicate your
;modifications in a prominent place in the source code.  
;
;All routines are provided "as is" without any express or implied
;warranties whatsoever.  All routines are distributed without guarantee
;of support.  If errors are found in this code it is requested that you
;contact us by sending email to the address below to report the errors
;but we make no claims regarding timely fixes.  This software has been 
;used for analysis of COBE data but has not been validated and has not 
;been used to create validated data sets of any type.
;
;Please send bug reports to CGIS@ZWICKY.GSFC.NASA.GOV.


