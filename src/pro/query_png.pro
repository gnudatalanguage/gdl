; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_png, filename, info
  on_error, 2
  return, magick_ping(filename, 'PNG', info=info)
end
