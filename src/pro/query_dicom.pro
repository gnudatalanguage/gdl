; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_dicom, filename, info
  on_error, 2
  return, magick_ping(filename, 'DCM', info=info)
end
