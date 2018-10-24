;
; AC July 2017
;
; generic graphic style for Benchmarks plots
;
pro BENCHMARK_GRAPHIC_STYLE, liste, colors, mypsym, myline, flags, $
                             languages, select=select, $
                             verbose=verbose, test=test
;
mypsym=[-2,-1,-4]
myline=[2,3,4]
;;colors=['ff00'x,'ff'x,'ff00ff'x]
colors=[COLOR2COLOR(!color.green), $
        COLOR2COLOR(!color.red), $
        COLOR2COLOR(!color.magenta), $
        COLOR2COLOR(!color.blue), $
        COLOR2COLOR(!color.yellow), $
        COLOR2COLOR(!color.orange)]
;;
if ~KEYWORD_SET(select) then begin
   ;; GDL (green), IDL (red), FL (magenta)
   languages=['GDL','IDL','FL']
   colors=colors[0:2]
   ;;
   ;; create flags for fields ...
   flags=STREGEX(liste,'_GDL_',/bool)
   flags=flags+STREGEX(liste,'_IDL_',/bool)*2
   flags=flags+STREGEX(liste,'_FL_',/bool)*3
   flags=flags-1
endif else begin
   nbps=N_ELEMENTS(select)
   languages=select
   flags=REPLICATE(0, N_ELEMENTS(liste))
   for ii=0, nbps-1 do begin
      flags=flags+STREGEX(liste,select[ii],/bool)*(1+ii)        
   endfor
   flags=flags-1
   ;;
   if nbps GT N_ELEMENTS(colors) then begin
      colors=[colors, colors]
   endif
   colors=colors[0:nbps-1]
endelse
;
if KEYWORD_SET(verbose) then begin
   print, 'Colors ? GDL : green, IDL : red, FL : magenta !'
endif
;
if KEYWORD_SET(test) then STOP
;
end
