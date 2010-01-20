; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_tiff, filename, info, image_index=image_index
  on_error, 2
  ; TODO: GEOTIFF keyword
  return, magick_ping(filename, 'TIFF', info=info, image_index=image_index)
end
