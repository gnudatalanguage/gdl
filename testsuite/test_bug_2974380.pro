pro test_bug_2974380
  if GETENV('DISPLAY') eq '' then exit, status=77
  window,xsize=512,ysize=512
  wdelete
  window,xsize=512,ysize=512
end
