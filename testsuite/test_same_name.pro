function TOTO, x
  return, x+1
end

pro test_same_name
  ; 1st case
  dist = dist(10)

  ; 2nd case
  x=0
  toto=TOTO(x)
  print, x, toto
end
