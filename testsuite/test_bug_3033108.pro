; causes segfault, reportd by Joanna (cokhavim)
; cf. http://sourceforge.net/tracker/index.php?func=detail&aid=3033108&group_id=97659&atid=618683
pro test_bug_3033108
  x = 5
  if 0 lt 1 then x = (x)
  y = x
end
