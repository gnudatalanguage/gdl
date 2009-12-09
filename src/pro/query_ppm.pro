; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_ppm, filename, info
  ; TODO: MAXVAL keyword
  return, magick_ping(filename, 'PNM', info=info)
end
