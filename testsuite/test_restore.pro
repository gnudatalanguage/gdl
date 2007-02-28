; AC, le 27-fev-2007
;
; very simple tests, expecially on filenames
;
pro TEST_RESTORE
;
print, 'We are testing to read back missing files'
;
filename1='zxdrtyu'
print, 'Reading back missing file : ', filename1 
RESTORE, filename1, /verbose
;
print, 'We are testing to read back 2 files simultaneously'
;
filename1='zxdrtyu1'
filename2='zxdrtyu2'
print, 'Reading back 2 files : <', filename1, '>, <', filename2, '>'
RESTORE, filename1, filename=filename2, /verbose
;
; 
filename='demo_save_GDL.xdr'
print, 'Reading back a demo SAVE file in GDL: ', filename
restore, filename, /verbose
;
end

