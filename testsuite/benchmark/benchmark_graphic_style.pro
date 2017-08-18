;
; AC July 2017
;
; generic graphic style for Benchmarks plots
;
pro BENCHMARK_GRAPHIC_STYLE, liste, colors, mypsym, myline, flags, languages, $
                         verbose=verbose, test=test
;
; GDL (green), IDL (red), FL (magenta)
;
languages=['GDL','IDL','FL']
colors=['ff00'x,'ff'x,'ff00ff'x]
;
mypsym=[-2,-3,-4]
myline=[2,3,4]
;
; create flags for fields ...
flags=STREGEX(liste,'_GDL_',/bool)
flags=flags+STREGEX(liste,'_IDL_',/bool)*2
flags=flags+STREGEX(liste,'_FL_',/bool)*3
flags=flags-1
;
if KEYWORD_SET(verbose) then begin
   print, 'Colors ? GDL : green, IDL : red, FL : magenta !'
endif
;
if KEYWORD_SET(test) then STOP
;
end
