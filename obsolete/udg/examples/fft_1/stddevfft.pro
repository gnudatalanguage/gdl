function stddevfft, x
  return, sqrt(total((abs(fft(x))^2)[1:-1]))
end
