function TOTO, x
  return, x+1
end

pro test_same_name
  ; 1st case
  if execute("dist = dist(10)") ne 1 then exit, status=1

  ; 2nd case
  x=0
  toto=TOTO(x)
  print, x, toto
end
