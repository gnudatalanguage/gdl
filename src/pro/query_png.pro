; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
function query_png, filename, info
  return, magick_ping(filename, 'PNG', info=info)
end
