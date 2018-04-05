pro test::printXY
  print, self.x, self.y
end
function test::init
  self.x = 10
  return, 1
end
pro test__define
  struct = {test, x : 5, y : 5} 
end
