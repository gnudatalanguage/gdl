;L+
; LICENSE:
;
; IDL user contributed source code
; Copyright (C) 2006 Robbie Barnett
;
;    This library is free software;
;    you can redistribute it and/or modify it under the
;    terms of the GNU Lesser General Public License as published
;    by the Free Software Foundation;
;    either version 2.1 of the License,
;    or (at your option) any later version.
;
;    This library is distributed in the hope that it will
;    be useful, but WITHOUT ANY WARRANTY;
;    without even the implied warranty of MERCHANTABILITY
;    or FITNESS FOR A PARTICULAR PURPOSE.
;    See the GNU Lesser General Public License for more details.
;
;    You should have received a copy of the GNU Lesser General Public License
;    along with this library; if not, write to the
;    Free Software Foundation, Inc., 59 Temple Place,
;    Suite 330, Boston, MA 02111-1307 USA
;
; Please send queries to:
; Robbie Barnett
; Nuclear Medicine and Ultrasound
; Westmead Hospital
; +61 2 9845 7223
;L-


; The test data for this routine can be obtained from
; http://www.creatis.insa-lyon.fr/~jpr/PUBLIC/gdcm/gdcmData.tar.gz

pro gdlffdicom__assoc__test0

files = file_search('*dcm',COUNT=nfiles)
nprocessed = 0
nidlffdicom = 0
du = obj_new('gdlffdicom__assoc')
for i=0l,nfiles-1l do begin

    if (du -> open( files[i], ACCESS_TIME=at)) then begin
        print, "Open " + files[i] + " access time", at
        images = du -> assoc(COUNT=count, ACCESS_TIME=at, TRUE=true)
        print, "Associate Acess time", at
        if (count gt 0) then begin
             nprocessed = nprocessed + 1
             window, nprocessed
              if (true) then begin
                 for j=0l,count-1l do  tv, images[j], TRUE=true
             endif else begin
                 for j=0l,count-1l do tvscl, images[j], j mod 4
             endelse
        endif else print, "Could not read " + files[i]
        result = du -> NewSOPInstanceUID()
        print, result
        du -> close

                                ; Warning - Causes segmentation fault for some invalid files
    endif

endfor
obj_destroy, du
print, "Number of files successfully processed by gdlffdicom__assoc: ", nprocessed, ".  Total number of files ", nfiles, FORMAT="(A,I0,A,I0)"
print, "Number of files successfully opened by IDLffDICOM: ", nidlffdicom, ".  Total number of files ", nfiles, FORMAT="(A,I0,A,I0)"
end



pro gdlffdicom__assoc__test1
files = file_search('*dcm',COUNT=nfiles)

for i=0l,nfiles-1l do begin
    du = obj_new('gdlffdicom__assoc')
    print, "Opening " + files[i]

    if (du -> open( files[i],ACCESS_TIME=at,/INDEX_SEQUENCES)) then begin
        du -> dump
        du -> close
    endif
endfor
obj_destroy, du

end

pro gdlffdicom__assoc__test2

file = 'GE_LOGIQBook-8-RGB-HugePreview.dcm'
du = obj_new('gdlffdicom__assoc')
if (du -> open( file,ACCESS_TIME=at,/INDEX_SEQUENCES, /INDEX_TAGS, /NO_CATCH)) then begin
    print, "Open " + file + " access time", at
    du -> dump
    du -> close
endif
obj_destroy, du
end

pro gdlffdicom__assoc__test3

file = 'PHILIPS_Intera-16-MONO2-Uncompress.dcm' ; The files swithes endian mid-sequence
du = obj_new('gdlffdicom__assoc')
if (du -> open( file,ACCESS_TIME=at, /INDEX_TAGS, /NO_CATCH)) then begin ;/INDEX_SEQUENCES, /INDEX_TAGS, /NO_CATCH)) then begin
    print, "Open " + file + " access time", at
   ;du -> dump
    du -> close
endif
obj_destroy, du
end



pro gdlffdicom__assoc__test4

file = 'GE_LOGIQBook-8-RGB-HugePreview.dcm'
du = obj_new('gdlffdicom__assoc')
if (du -> open( file,ACCESS_TIME=at,/INDEX_SEQUENCES, /INDEX_TAGS, /NO_CATCH)) then begin
    print, "Open " + file + " access time", at
    ;help, du
    du -> write, file + '_copy'
    du -> close
endif
obj_destroy, du
end

pro gdlffdicom__assoc__test5
files = file_search('*dcm',COUNT=nfiles)

du = obj_new('gdlffdicom__assoc')
for i=0l,nfiles-1l do begin
    print, "Opening " + files[i]

    if (du -> open( files[i],ACCESS_TIME=at,/INDEX_SEQUENCES)) then begin
        du -> write, files[i] + '_copy'
        du -> close
        spawn, strjoin(['diff',files[i],  files[i] + '_copy'],' '), EXIT_STATUS=exit_status
        if (exit_status NE 0) then print, "Could not write to file " + files[i]
    endif

endfor
obj_destroy, du
;help, /heap
end

pro gdlffdicom__assoc__test, inds, ALL=all

if (keyword_set(all)) then inds = indgen(5)
for i=0,n_elements(inds)-1 do begin
    call_procedure, STRING('gdlffdicom__assoc__test',inds[i],FORMAT="(A,I0)")
endfor

end


pro GDLffDICOM__assoc_GenerateUID_test1

for i=124,31,-1 do begin
    uid = GDLffDICOM__assoc_GenerateUID(i)
    print, uid
    if (strlen(uid) ne i) then message
endfor

for i=31,124 do begin
    uid = GDLffDICOM__assoc_GenerateUID(i)
    print, uid
    if (strlen(uid) ne i) then message
endfor

for i=124,31,-1 do begin
    uid = GDLffDICOM__assoc_GenerateUID(i)
    print, uid
    if (strlen(uid) ne i) then message
endfor


end

pro GDLffDICOM__assoc_GenerateUID_test2
s = 31

ni = 10000l ; 0000l
nj = 1l
uids = strarr(ni)

for j=0l,nj-1 do begin
    for i=0l,ni-1 do uids[i] = GDLffDICOM__assoc_GenerateUID(s)
    inds = sort(uids)

    if (n_elements(uniq(uids[inds],inds)) ne ni) then begin
    	print, uids[inds]
    	message, 'UIDs not unique'
    endif
endfor

end

pro GDLffDICOM__assoc_GenerateUID_test3
s = 64

ni = 100000l
nj = 1l
uids = strarr(ni)

for j=0l,nj-1 do begin
    for i=0l,ni-1 do uids[i] = GDLffDICOM__assoc_GenerateUID(s)
    inds = sort(uids)
    if (n_elements(uniq(uids[inds],inds)) ne ni) then message
endfor

end

pro GDLffDICOM__assoc_test6
file = '\\a3printer2\farmyard\wmidl\nmtkdualrenal0\test\2008.0.14125138.dcm'
du = obj_new('gdlffdicom__assoc')
if (du -> open( file,ACCESS_TIME=at,/INDEX_SEQUENCES, /INDEX_TAGS, /NO_CATCH)) then begin
    print, "Open " + file + " access time", at
    du -> dump
    du -> close
endif
obj_destroy, du
end



pro GDLffDICOM__assoc_test7
file = 'E:\public\esoft\gastric_emptying\2006.0.5699771.dcm'
du = obj_new('gdlffdicom__assoc')
if (du -> open( file,ACCESS_TIME=at,/INDEX_SEQUENCES, /INDEX_TAGS, /NO_CATCH)) then begin
    print, "Open " + file + " access time", at
    du -> dump
    du -> close
endif
obj_destroy, du
end



pro GDLffDICOM__assoc_test8
file = 'E:\public\esoft\gastric_emptying\2006.0.5699771.dcm'
du = obj_new('gdlffdicom')
if (du -> read( file,ACCESS_TIME=at,/INDEX_SEQUENCES, /INDEX_TAGS, /NO_CATCH)) then begin
    print, "Open " + file + " access time", at
    du -> dumpElements
    du -> close
endif
obj_destroy, du
end


pro gdlffdicom__assoc__test_spec, spec

files = file_search(spec,COUNT=nfiles)
nprocessed = 0
nidlffdicom = 0
du = obj_new('gdlffdicom__assoc')
for i=0l,nfiles-1l do begin

    if (du -> open( files[i], ACCESS_TIME=at)) then begin
        print, "Open " + files[i] + " access time", at
        images = du -> assoc(COUNT=count, ACCESS_TIME=at, TRUE=true)
        print, "Associate Acess time", at
        if (count gt 0) then begin
             nprocessed = nprocessed + 1
             window, nprocessed
              if (true) then begin
                 for j=0l,count-1l do  tv, images[j], TRUE=true
             endif else begin
                 for j=0l,count-1l do tvscl, images[j], j mod 4
             endelse
        endif else print, "Could not read " + files[i]
        result = du -> NewSOPInstanceUID()
        print, result
        du -> close

                                ; Warning - Causes segmentation fault for some invalid files
    endif

endfor
obj_destroy, du
print, "Number of files successfully processed by gdlffdicom__assoc: ", nprocessed, ".  Total number of files ", nfiles, FORMAT="(A,I0,A,I0)"
print, "Number of files successfully opened by IDLffDICOM: ", nidlffdicom, ".  Total number of files ", nfiles, FORMAT="(A,I0,A,I0)"
end
