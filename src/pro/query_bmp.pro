; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_bmp, filename, info
  return, magick_ping(filename, 'BMP', info=info)
end
