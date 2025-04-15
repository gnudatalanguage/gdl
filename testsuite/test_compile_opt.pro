pro define_as_int
  a=33
  print,'A               INT       =       33'
  help,a
end
pro define_as_long
  compile_opt IDL2
  b=44
  print,'B               LONG       =          44'
  help,b
end
pro test_compile_opt
  print,"start without DEFINT32"
   define_as_long
   define_as_int
  print,"setting DEFINT32 (should not change anything)"
   compile_opt IDL2
   define_as_long
   define_as_int
  print,"in execute:"
  void=execute("define_as_long")
  void=execute("define_as_int")

  print,"now interactively type 'compile_opt IDL2' and replay me."

end
