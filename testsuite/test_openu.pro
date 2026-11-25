; Switching direction on an update file must preserve the current byte offset (#2288).
pro TEST_OPENU
catch, err
if err ne 0 then begin
   catch, /cancel
   print, !error_state.msg
   exit, status=1
endif

file = 'test_openu.bin'
expected = bindgen(16)
openw, u, file, /get_lun
writeu, u, expected
free_lun, u

openu, u, file, /get_lun
prefix = bytarr(4)
readu, u, prefix
writeu, u, [255b, 255b]
free_lun, u

openu, u, file, /get_lun, /delete
expected[4:5] = 255b
actual = bytarr((fstat(u)).size)
readu, u, actual
if ~array_equal(actual, expected) then message, 'WRITEU after READU changed the wrong bytes'

point_lun, u, 2
writeu, u, [99b, 98b]
next = bytarr(4)
readu, u, next
if ~array_equal(next, expected[4:7]) then message, 'READU after WRITEU read the wrong bytes'

; A record past EOF extends the file with zeros from its end, not from the current offset.
expected[2:3] = [99b, 98b]
record = assoc(u, bytarr(4))
record[6] = [1b, 2b, 3b, 4b]
point_lun, u, 0
actual = bytarr((fstat(u)).size)
readu, u, actual
if ~array_equal(actual, [expected, bytarr(8), 1b, 2b, 3b, 4b]) then $
   message, 'ASSOC write past EOF changed existing data'
free_lun, u
end
