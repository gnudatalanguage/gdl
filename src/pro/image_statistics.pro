;+
; NAME:    IMAGE_STATISTICS
;
; PURPOSE:
;
; CATEGORY:
;
;
;
; CALLING SEQUENCE:
;
;
;
; INPUTS:
;
;
;
; OPTIONAL INPUTS:
;
;
;
; KEYWORD PARAMETERS:
;
;
;
; OUTPUTS:
;
;
;
; OPTIONAL OUTPUTS:
;
;
;
; COMMON BLOCKS: none
;
; SIDE EFFECTS:
;
;
;
; RESTRICTIONS:
;
;
;
; PROCEDURE:
;
;
;
; EXAMPLE:
;
;
; LICENCE: this code is under GNU GPL v2 or later. (C) 2011
; 
; MODIFICATION HISTORY:
; -- first draft created by Alain Coulais, 10-Nov-2011
; -- 15-Nov-2011 : AC : better managmenet of output types
;
;-
;
pro ImSt_MESS, keyword_name
MESSAGE, /continue, 'This keyword '+STRUPCASE(keyword_name)+' is not available'
MESSAGE, /continue, 'Please consider to contribute (by submitting Patches on SF.net)'
end
;
pro IMAGE_STATISTICS, input_data, mask=mask, count=count, $
                      data_sum=data_sum, maximum=maximum, $
                      mean=mean_, minimum=minimum, $
                      stddev=stddev_, sum_of_squares=sum_of_squares, $
                      variance=variance_, $
                      lut=lut, vector=vector, $
                      weight_sum=weight_sum, weighted=weighted, $
                      help=help, test=test, verbose=verbose
;
if N_PARAMS() NE 1 then MESSAGE, 'Incorrect number of arguments.'
;
if ((SIZE(input_data))[0] LT 1) then MESSAGE, 'Expression must be an array in this context'
;
if KEYWORD_SET(help) then begin
   print, 'pro IMAGE_STATISTICS, data, mask=mask, count=count, $'
   print, '                data_sum=data_sum, maximum=maximum, $'
   print, '                mean=mean_, minimum=minimum, $'
   print, '                stddev=stddev_, sum_of_squares=sum_of_squares, $'
   print, '                variance=variance_, $'
   print, '                lut=lut, vector=vector, $'
   print, '                weight_sum=weight_sum, weighted=weighted, $'
   print, '                help=help, test=test, verbose=verbose'
   return
endif
;
if KEYWORD_SET(lut) then ImSt_MESS, 'lut'
if KEYWORD_SET(vector) then ImSt_MESS, 'vector'
if KEYWORD_SET(weight_sum) then ImSt_MESS, 'weight_sum'
if KEYWORD_SET(weighted) then ImSt_MESS, 'weighted'
;
image=input_data
if KEYWORD_SET(mask) then begin
   OK=WHERE(mask NE 0, nbp_ok)
   if (nbp_ok GT 0) then image=input_data[OK]
endif
;
count=ULONG(N_ELEMENTS(image))
data_sum=FLOAT(TOTAL(image,/double))
mean_=FLOAT(MEAN(image,/double))
maximum=MAX(image, min=minimum)
maximum=FLOAT(maximum)
minimum=FLOAT(minimum)
sum_of_squares=FLOAT(TOTAL(image^2.D,/double))
;
if N_ELEMENTS(image) GT 1 then begin
   stddev_=FLOAT(STDDEV(image,/double))
   variance_=FLOAT(VARIANCE(image,/double))
endif else begin
   stddev_=0.0
   variance_=0.0
endelse
;
if KEYWORD_SET(verbose) then begin
   if KEYWORD_SET(mask) then begin
      print, 'Statistics on MASKED Image:'
   endif else begin
      print, 'Statistics on Image:'
   endelse
   print, 'Total Number of Pixels             = ', count
   print, 'Total of Pixel Values              = ', data_sum
   print, 'Maximum Pixel Value                = ', maximum
   print, 'Mean of Pixel Values               = ', mean_
   print, 'Minimum Pixel Value                = ', minimum
   print, 'Standard Deviation of Pixel Values = ', stddev_
   print, 'Total of Squared Pixel Values      = ', sum_of_squares
   print, 'Variance of Pixel Values           = ', variance_
endif
;
if KEYWORD_SET(test) then STOP
;
end
