; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_dicom, filename, info
  return, magick_ping(filename, 'DCM', info=info)
end
