; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_pict, filename, info
  return, magick_ping(filename, 'PICT', info=info) 
end
