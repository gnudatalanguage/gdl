; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_gif, filename, info
  return, magick_ping(filename, 'GIF', info=info) 
end
