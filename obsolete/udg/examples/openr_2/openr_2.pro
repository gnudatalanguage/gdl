$ echo "GDL rocks!" > file.txt
$ gzip -f file.txt
openr, u, 'file.txt.gz', /get_lun, /compress
s = '          ' 
readu, u, s
free_lun, u
print, s
$ rm file.txt.gz
