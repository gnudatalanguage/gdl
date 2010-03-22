pro test_gc

  a = total(memory() * [0,1,-1])                            ; n_alloc - n_free
  if total(memory() * [0,1,-1]) - a ne 1 then begin         ; 1 for storing the 1 (? - hope I'm right ;))  
    message, 'A (problem with memory() ?)', /conti
    exit, status=1
  endif

  b = ptr_new(a)
  if total(memory() * [0,1,-1]) - a ne 3 then begin         ; 3 for storing the 3, the copy of a, and the b
    message, 'B (problem with memory() ?)', /conti
    exit, status=1
  endif

  b = ptr_new(a, /no_copy)
  if total(memory() * [0,1,-1]) - *b ne 2 then begin        ; 2 for storing the 2, and the b
    message, 'C (problem with GC)', /conti
    exit, status=1
  endif

  c = ptr_new(ptr_new(1))
  c = 0.
  if total(memory() * [0,1,-1]) - *b ne 3 then begin        ; 3 for storing the 3, the b, and the c
    message, 'D (problem with GC)', /conti
    exit, status=1
  endif

end
