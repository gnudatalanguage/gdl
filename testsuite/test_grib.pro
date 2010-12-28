pro test_grib


  file = 'example.grib'

  has_grib = ~ (execute("a=gribapi_open_file(file)") eq 0)
  if ~has_grib then exit, status=77

  message, /info, "opening " + file + "..."
  grib_f = gribapi_open_file(file)

  message, /info, "number of messages in file: " + string(gribapi_count_in_file(grib_f))

  message, /info, "getting message from file..."
  grib_m = gribapi_new_from_file(grib_f)

  message, /info, "closing file..."
  gribapi_close_file, grib_f

  message, /info, "size of ,,numberOfPointsAlongAParallel'' array: " + string(gribapi_get_size(grib_m, 'numberOfPointsAlongAParallel'))
  message, /info, "size of ,,values'' array: " + string(gribapi_get_size(grib_m, 'values'))

  message, /info, "trying to get a string key ,,discipline''..."
  gribapi_get, grib_m, 'discipline', disc
  help, disc

  message, /info, "retrieving ,,values''..."
  gribapi_get, grib_m, 'values', data
  help, data

  message, /info, "retrieving ,,numberOfPointsAlongAParallel''..."
  gribapi_get, grib_m, 'numberOfPointsAlongAParallel', data
  help, data

  message, /info, "retrieving data together with lat/lon arrays using gribapi_get_data..."
  gribapi_get_data, grib_m, lats, lons, data
  help, lats, lons, data

  data = reform(data, 21, 21, /over) 
  lons = (temporary(lons))[0:20]
  lats = (temporary(lats))[indgen(21) * 21]

  ;map_set, limit=[min(lats), min(lons), max(lats), max(lons)]
  set_plot, 'z'
  contour, data, lons, lats;, /overplot, /fill, nlev=10
  contour, data, lons, lats;, /overplot, /follow, nlev=10
  ;map_continents, /countries, /coasts

  message, /info, "clearing message from memory..."
  gribapi_release, grib_m

end
