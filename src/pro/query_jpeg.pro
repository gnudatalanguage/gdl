; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_jpeg, filename, info
  return, magick_ping(filename, 'JPEG', info=info)
end
