; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_pict, filename, info
  on_error, 2
  return, magick_ping(filename, 'PICT', info=info) 
end
