FUNCTION LABEL_DATE, axis, index, x, level, DATE_FORMAT = date_format, MONTHS = months, AM_PM=ampm,  DAYS_OF_WEEK=dow, OFFSET=offset, ROUND_UP=rndup
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
;       ROUND_UP, OFFSET currently not implemented.
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
;       GD              March 2021 added level & many options.
;-
 
COMMON label_date_com, fmt, month_chr, dow_chr, ampm_chr, defmonth, defdow, defampm, definit
COMPILE_OPT idl2, hidden
ON_ERROR, 2

if n_elements(definit) eq 0 then begin
   defmonth = ['Jan','Feb','Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
   defdow = ['Mon','Tue','Wed','Thu','Fri','Sat','Sun']
   defampm = ['am','pm']
   definit=1
endif

; NOTE: it will be more efficient to rewrite internally 'fmt' as a string
; with Time Format Codes (CMOI etc) and use this format code for all
; values of X. (This is what IDL does.)
; in other terms, the clumsiness of this procedure is largely intended to avoid copyright problems.

IF (N_PARAMS() LT 3) THEN IF NOT KEYWORD_SET(date_format) THEN date_format='' ;

IF n_elements(date_format) gt 0 then fmt = date_format
IF n_elements(months) gt 0 then month_chr = months else month_chr=0
IF n_elements(dow) gt 0 then dow_chr=dow else dow_chr=0
IF n_elements(ampm) gt 0 then ampm_chr=ampm else ampm_chr=0

if n_params() LT 3 then return, 0
if n_elements(fmt) eq 0 then fmt=''
if (n_elements(level) lt 1) then level = 0

;GDL_DOW and ICAP options are undocumented (!) GDL expensions 
caldat, x, month, day, year, hour , minute , second, gdl_dow=dow, gdl_icap=icap
level_index=level mod n_elements(fmt)
curr_fmt=fmt[level_index]
n = strlen(curr_fmt)
if n eq 0 then begin
   curr_fmt='%W %M %D %H:%I:%S %Y'
   n = strlen(curr_fmt)
endif
out = ''
for i=0, n-1 do begin           ;Each format character...
   c = strmid(curr_fmt, i, 1)        ;The character.
   if c eq '%' then begin
      i = i + 1
      c = strmid(curr_fmt, i, 1)     ;The function
      case c of                        ;format character?
         'M' : if n_elements(month_chr) eq 12 then out+=month_chr(month-1) else out+=defmonth(month-1) 
         'N' : out += string(format='(i2.2)', month)
         'D' : out += string(format='(i2.2)', day)
         'Y' : begin
                  if year ge 0 then subformat='(i4)' else subformat='(i5)'
                  out += string(format=subformat, year)
               end
         'Z' : begin
                 if year ge 0 then subformat='(i2.2)' else subformat='(i3)'
                 modyear=year mod 100
                 out += string(format=subformat, modyear)
               end
         'H' : begin ; if %A is requested, convert 0-24 h in 0-12 am pm
               if strpos(curr_fmt,"%A") ge 0 then begin
                 if hour gt 12 then hour-=12 ; pm
                 if hour eq 0 then hour=12 ; midnight
                 out = out + string(format='(i2)', hour) ; yes. not I2.2
               endif else   out = out + string(format='(i2.2)', hour)
               end
         'I' : out = out + string(format='(i2.2)', minute)
         'S' : begin
            if strmid(curr_fmt,i+1,1) eq '%' then begin
               ndigits= strmid(curr_fmt,i+2,1)
               test = strpos('0123456789', ndigits)
               if test ge 0 then begin
                  i+=2
                  subformat='(f0'+strtrim(3+test,2)+'.'+strtrim(test,2)+')'
                  out = out + string(format=subformat, second)
               endif else  out = out + string(format='(i2.2)', fix(second))
            endif else  out = out + string(format='(i2.2)', fix(second))
          end
         
         
         'W' : if n_elements(dow_chr) eq 7 then out+=dow_chr(dow-1) else out+=defdow(dow-1) 
         'A' : if n_elements(ampm_chr) eq 2 then out+=ampm_chr(icap) else out+=defampm(icap)
         '%' : out = out + '%'
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


