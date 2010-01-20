; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_image, filename, info, _ref_extra=ex
  on_error, 2
  return, magick_ping(filename, info=info, _strict_extra=ex)
end
