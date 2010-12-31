
function GDLffDICOM__assoc_GenerateUID, length, EMPTY_ON_ERROR=eoe, ERROR=error
common GDLffDICOM__assoc, instance_id, last_time, root, seed

root = '1.2.826.4567.'
nr = strlen(root)
nl = 18ul
nt = 10ul
ni = 4ul
ns = nl + nt + ni
if (n_elements(length) gt 0) then begin
    if (length - nr gt 99) then begin
        error = "Cannot generate a UID that long ("+string(length,FORMAT="(I0)")+"), maximum length is " + string(nr+99,FORMAT="(I0)")
        if (keyword_set(eoe)) then return, '' $
        else message, error
    endif
    if (length - nr lt 6) then begin
        error = "Cannot generate a UID that short ("+string(length,FORMAT="(I0)")+"), minimum length is " + string(nr+6,FORMAT="(I0)")
        if (keyword_set(eoe)) then return, '' $
        else message, error
    endif
    ratio = float(length - nr)/ns
    nl = ceil(nl*ratio)
    nt = ceil(nt*ratio)
    ni= ceil(ni*ratio)
endif
nts = string(nt,FORMAT="(I0)")
nis = string(ni,FORMAT="(I0)")
byte = 0b
lmhostid = ''
result = lmgr(LMHOSTID=lmhostid_hex)
nlh = ceil(2.0*nl/3.0)
for i=strlen(lmhostid_hex)-nlh,strlen(lmhostid_hex)-2,2 do begin
    reads, strmid(lmhostid_hex,i,2),byte, FORMAT="(Z2)"
    lmhostid = lmhostid + string(byte,FORMAT="(I3.3)")
endfor
lmhostid = strmid(lmhostid,0,nl)
current_time = ulong(systime(1))
if (n_elements(instance_id) eq 0) then begin
    instance_id = ulong(randomu(seed)*10l^(nis-1)) ; Start somewhere in the first tenth of the instances
endif
if (n_elements(last_time) eq 0) then last_time = 0l
;print, current_time eq last_time, instance_id, ni
if ((instance_id gt 10ul^ni) and (current_time eq last_time)) then begin
                                ; This whole waiting business is going
                                ; to ensure that the programmer/tester
                                ; increased the length of the field
                                ; before there is any possibility of
                                ; non-unique IDs
    message, "GDLffDICOM needs to wait 2 seconds to generate a UID. Please try setting the length of the UID field to larger.",/CONTINUE
    wait, 2
    instance_id = ulong(randomu(seed)*10ul^(nis-1)) ; Start somewhere in the first tenth of the instances
    current_time = current_time + 1
endif
time = string(current_time mod 10ul^nt,FORMAT="(I"+nts+"."+nts+")")
;instance_id = instance_id mod 10ul^ni
;help, instance_id
instance = string(instance_id mod 10ul^ni,FORMAT="(I"+nis+"."+nis+")")
;print, instance, nis, instance_id
instance_id = instance_id + 1
uid = root + lmhostid + time + instance
offset = strlen(uid) - length
if (offset gt 0) then lmhostid = strmid(lmhostid,offset,nl-offset)
uid = root + lmhostid + time + instance
last_time = current_time
return, uid
end

