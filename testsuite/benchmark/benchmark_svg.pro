;
;
;
pro BENCHMARK_SVG, svg=svg, on=on, off=off, filename=filename, infosvg=infosvg
;
if KEYWORD_SET(svg) then begin
   if KEYWORD_SET(on) then begin
      infosvg={name : !d.name, file: filename}
      SET_PLOT, 'SVG'
      DEVICE, filename=filename      
   endif
   if KEYWORD_SET(off) then begin
      device, /close      
      SET_PLOT, infosvg.name
      print, 'Output SVG file generated : '+infosvg.file
   endif
endif
end

