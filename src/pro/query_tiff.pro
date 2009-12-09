; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_tiff, filename, info, image_index=image_index
  ; TODO: GEOTIFF keyword
  return, magick_ping(filename, 'TIFF', info=info, image_index=image_index)
end
