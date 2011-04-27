if n_elements(routine_names('a', fetch=0)) gt 0 $
  then value = routine_names('a', fetch=0) $
  else message, 'a is not defined!'
