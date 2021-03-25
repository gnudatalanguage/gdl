; obsoleted in 5.4
function polyfitw,x,y,measure_errors,degree,yfit,yband,sigma,covar, DOUBLE=double,STATUS=status
 return,poly_fit(x,y,degree,yfit=yfit,measure_errors=measure_errors,yband=yband,sigma=sigma,covar=covar, DOUBLE=double,STATUS=status)
end
