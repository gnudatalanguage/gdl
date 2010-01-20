; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function rstrpos, haystack, needle, pos
  on_error, 2
 
  if ~keyword_set(haystack) || ~keyword_set(needle) then $
    message, 'First and second arguments must not be undefined' 
  if n_elements(needle) ne 1 then $
    message, 'Second argument must be a scalar or one element array'
  if keyword_set(pos) then begin
    return, strpos(haystack, needle, pos, /reverse_search) 
  endif else begin
    return, strpos(haystack, needle, /reverse_search)
  endelse
  compile_opt obsolete
end
