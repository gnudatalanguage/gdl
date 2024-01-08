pro TEST_GH00178
;
cmd = "pm, [1,2], format='(I2)', title='PM'"
;
path_to_exe=GET_PATH_TO_EXE()
SPAWN, path_to_exe+' -quiet -e "' + cmd + '"',res, count=cnt
;;  spawn, '../src/gdl -quiet -e "' + cmd + '"',res, count=cnt
;
if STRMATCH(res[0], '*error*') eq 1 then exit, status=1 
;
end
