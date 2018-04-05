rng_x = [-1, 2.3]
rng_y = [-1.3, 1.3]
appleman, rng_x, rng_y, result=fractal, xsize=165, ysize=130
device, /color
plot, [0], /nodata, xrange=rng_x, yrange=rng_y
loadct, 15
tvscl, fractal, rng_x[0], rng_y[0], $
  xsize=rng_x[1]-rng_x[0], ysize=rng_y[1]-rng_y[0]
