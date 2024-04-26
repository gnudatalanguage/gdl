;+
; NAME: MEDABSDEV
;
; PURPOSE: compute the (renormalized) median aboslute deviation
;          robust way to estimate STDDEV() in presence of few outliers
;
; CATEGORY: statistic
;
; CALLING SEQUENCE: res=MEDABSDEV(input)
;
; INPUTS: an input array. Any dim (dims if any not enforced)
;
; OPTIONAL INPUTS: none
;
; KEYWORD PARAMETERS: none
;
; OUTPUTS: a single number
;
; OPTIONAL OUTPUTS: none
;
; COMMON BLOCKS: none
;
; SIDE EFFECTS: none
;
; RESTRICTIONS: none
;
; PROCEDURE: staightfoward. Warning : we do use a normalization
; factor for 1.4826, as mentionned in
; https://en.wikipedia.org/wiki/Median_absolute_deviation
;
; EXAMPLE:
;
; input=RANDOMU(seed, 1000)
; print, STDDEV(input), MEDABSDEV(input)
;       0.289110     0.377378
; input[100]=100000.
; print, STDDEV(input), MEDABSDEV(input)
;      3162.30     0.377378
; input[500]=!values.f_nan
; input[501]=!values.f_infinity
; GDL> print, STDDEV(input), MEDABSDEV(input)
;      3165.46     0.377378
;
; asymptotic to STDDEV() ...
; GDL> in=randomu(seed,100000) & print, medabsdev(in), stddev(in)
;     0.370835     0.288611
; GDL> in=randomn(seed,100000) & print, medabsdev(in), stddev(in)
;      1.00326      1.00175
;
; MODIFICATION HISTORY: initial import 30 January 2024
;
; Writtent by AC & René Gastaud.
; Under GNU GPL v2+
; 
;-
;
function MEDABSDEV, input
return, 1.4826*MEDIAN(ABS(input-MEDIAN(input)))
end
