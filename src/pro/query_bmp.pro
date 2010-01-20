; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_bmp, filename, info
  on_error, 2
  return, magick_ping(filename, 'BMP', info=info)
end
