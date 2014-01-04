pro test_bug_3285659
  ; caused a segfault up to 0.9.3
  help, execute("help, routine_names(store=name, fetch=0)")
end
