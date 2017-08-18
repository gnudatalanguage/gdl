;
; AC, July 2017
;
; The difficulty here is we don't know the name of
; the vectors for X and Y axis before calling the code !
;
; --------------------------------------------------------------
;
function BENCH_UPDATE_RANGE, range, new_data
;
if (MIN(new_data) LT range[0]) then range[0]=MIN(new_data)
if (MAX(new_data) GT range[1]) then range[1]=MAX(new_data)
;
return, range
;
end
;
; --------------------------------------------------------------
;
; Restoring a liste of XDR files containing "x_name" and "y_name"
; vectors of data. Extracting extremal ranges.
;
pro BENCHMARK_COMPUTE_RANGE, liste_xdr_files, xrange, yrange, x_name, y_name, $
                         xmax=xmax, ymax=ymax, verbose=verbose, test=test, debug=debug
;
xrange=[0.,0.]
yrange=[0.,0.]
;
;print, xmax, ymax
for ii=0, N_ELEMENTS(liste_xdr_files)-1 do begin
   ;;
   if KEYWORD_SET(verbose) then print, 'Restoring '+liste_xdr_files[ii]
   RESTORE, liste_xdr_files[ii]
   ;;
   ;; because we don't know the names before
   ;;
   cmd="x_data="+x_name
   res=EXECUTE(cmd)
   ;;
   cmd="y_data="+y_name
   res=EXECUTE(cmd)
   ;;
   update_flag=1
   ;;
   if KEYWORD_SET(xmax) then begin
      ok=WHERE(x_data LE xmax, nbpx)
      if nbpx GT 0 then begin
         x_data=x_data[ok]
         y_data=y_data[ok]
      endif else update_flag=0
      ;;print, nbpx, y_data
   endif
   ;;
   if KEYWORD_SET(ymax) then begin
      ok=WHERE(y_data LE ymax, nbpy)
      if nbpy GT 0 then begin
         x_data=x_data[ok]
         y_data=y_data[ok]
      endif  else update_flag=0
      ;;print,nbpy,  y_data
   endif
   ;;
   ;; do we still have one point ?!
   ;;
   if (update_flag EQ 1) then begin
      xrange=BENCH_UPDATE_RANGE(xrange,x_data)
      yrange=BENCH_UPDATE_RANGE(yrange,y_data)
   endif
   ;;print, xrange
   ;;print, yrange
   ;;
endfor
;
if KEYWORD_SET(test) then STOP
;
end
