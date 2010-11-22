; reported by: Bob Jones (bogdanovist(at)users.sf.net)

function foo::init,p
  self.n = (*(p[0]))
  print,self.n
end

pro foo__define
  struct = {FOO,n:0}
end

pro gdl_test
  p = ptrarr(2)
  n=1
  p[0] = ptr_new(n)
  p[1] = ptr_new(n)
  inst = obj_new('foo',p)
end

pro test_bug_3081887
  if execute('gdl_test') ne 1 then exit, status=1
end
