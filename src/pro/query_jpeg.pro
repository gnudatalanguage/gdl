; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_jpeg, filename, info
  on_error, 2
  return, magick_ping(filename, 'JPEG', info=info)
end
