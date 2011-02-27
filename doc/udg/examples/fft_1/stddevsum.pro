function stddevsum, x
  return, sqrt(mean(x^2) - mean(x)^2)
end
