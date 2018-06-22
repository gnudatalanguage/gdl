;
; TEST_FILE_LINK
pro test_file_link,test=test,noexit=noexit

if keyword_set(test) then stop,' inside test_filelnk near the end'
;
BANNER_FOR_TESTSUITE,' file link ',0

return
end
