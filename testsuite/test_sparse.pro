; no a test, really. Just a suggestion for test, plu sthe fact
; that it exercises the coverage.
pro test_sparse
  
  a = [[ 2.0,  1.0,  1.0],[ 4.0, -6.0,  0.0],[-2.0,  7.0,  2.0]]
  z=sprsin(a, thresh=0.5)
  zz=sprstp(z)
  q=fulstr(z)
  if abs(total(a-q)) gt 1E-6 then exit,status=1
  res=sprsab(z,zz)
  result=fulstr(res)
  if total(result) ne 29 then exit,status=1
  if total(sprsax(z,[1,1,1]))  ne 9 then exit,status=1
  aludc=a
  LUDC, aludc, index
  b = [3,-8.0,10]
  x = LUSOL(aludc, index, b)
  r= LINBCG(SPRSIN(a), B, X)
  if abs(total(r-x)) gt 1E-6 then exit,status=1
end
