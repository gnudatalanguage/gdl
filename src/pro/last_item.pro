;+
; Project     : SOHO - CDS     
;                   
; Name        : LAST_ITEM
;               
; Purpose     : Returns the last element of the input variable.
;               
; Explanation : Returns the last element of whatever the input variable is.
;               
; Use         : IDL> print,last_item(indgen(10))
;                     ---> 9
;
;               Find the latest archive telemetry file:
;               IDL> print,last_item(findfile(concat_dir('$CDS_TM_DATA','tm*')))
;    
; Inputs      : item - variable/array/structure to extract from
;               
; Opt. Inputs : None
;               
; Outputs     : Function returns last value.
;               
; Opt. Outputs: None
;               
; Keywords    : None
;
; Calls       : None
;
; Common      : None
;               
; Restrictions: None
;               
; Side effects: None
;               
; Category    : Util, numerical
;               
; Prev. Hist. : None
;
; Written     : C D Pike, RAL, 9-Dec-94
;               
; Modified    : 
;
; Version     : Version 1, 9-Dec-94
;-            

function last_item, item
  on_error, 2

  ;  check input
  if n_params() lt 1 then begin
    message, 'Use: xlast = last_item(x)'
  endif

  return,item[n_elements(item)-1]

end
