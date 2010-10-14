; https://sourceforge.net/tracker/?func=detail&atid=618683&aid=3086851&group_id=97659
pro test_bug_3086851
  if n_elements(strsplit('a;:c', '[:;]', /extract, /regex, /preserve_null)) ne 3 then begin
    exit, status=1
  endif
end
