FUNCTION file_lines, filename
   nf = n_elements(filename)
   result = lon64arr(nf)
	for k=0,nf-1 do begin
	   OPENR, unit, filename[k], /GET_LUN
	   str = ''
	   count = 0ll
	   WHILE ~ EOF(unit) DO BEGIN
		  READF, unit, str
		  count = count + 1
	   ENDWHILE
	   FREE_LUN, unit
	   result[k] = count
   endfor
   if(nf eq 1) then return, count else  $
		RETURN, result
END
