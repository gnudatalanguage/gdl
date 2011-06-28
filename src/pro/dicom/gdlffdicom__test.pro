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
;    Free Software Foundation, Inc.
;    51 Franklin Street, Suite 500
;    Boston, MA 02110-1335, USA
;
; Please send queries to:
; Robbie Barnett
; Nuclear Medicine and Ultrasound
; Westmead Hospital
; +61 2 9845 7223
;L-

; The test data for this routine can be obtained from
; http://www.creatis.insa-lyon.fr/~jpr/PUBLIC/gdcm/gdcmData.tar.gz

pro  GDLffDICOM__test0

dicom_obj = obj_new('GDLffDICOM')



starttime =systime(1)
result = dicom_obj -> Read('NM-MONO2-16-13x-heart.dcm')
if (result) then begin
    description = dicom_obj -> GetDescription(REFERENCE=42)
    element = dicom_obj -> GetElement(REFERENCE=42)
    group = dicom_obj -> GetGroup(REFERENCE=42)
    vr = dicom_obj -> GetVR(REFERENCE=42)
    len = dicom_obj -> GetLength(REFERENCE=42)
    parent = dicom_obj -> GetParent(42)
    value = dicom_obj -> GetValue(REFERENCE=42,/no_copy)
    preamble = dicom_obj -> GetPreamble()
    print, "Time", systime(1)-starttime
;    help, description[0], element[0], group[0], vr[0], len[0], parent[0], (*value[0])[0], preamble
    print, "Description", dicom_obj -> GetReference(DESCRIPTION='BitsAllocated')
    print, "VR", dicom_obj -> GetReference(VR='US')
    print, "Tag", dicom_obj -> GetReference('0028'x,'0100'x)
    print, "ALL", dicom_obj -> GetReference('0028'x,'0100'x,DESCRIPTION='BitsAllocated',VR='US')
endif
obj_destroy, dicom_obj
; help, /heap
end

pro  GDLffDICOM__test1


dicom_obj = obj_new('GDLffDICOM')

result = dicom_obj -> Read('NM-MONO2-16-13x-heart.dcm',/no_catch)
if (result) then begin
    reference = dicom_obj -> GetReference('0028'x,'0100'x)
    value = dicom_obj -> GetValue(REFERENCE=reference,/no_copy)
;    help, (*(value[0]))
    result = dicom_obj -> Commit('NM-MONO2-16-13x-heart.dcm_copy')
endif
obj_destroy, dicom_obj
;help, /heap

end


pro  GDLffDICOM__test2


dicom_obj = obj_new('GDLffDICOM')

result = dicom_obj -> Read('NM-MONO2-16-13x-heart.dcm')
if (result) then begin
;    help, result
    reference = dicom_obj -> GetReference('0010'x,'0010'x)
    value = dicom_obj -> GetValue(REFERENCE=reference,/no_copy)
;    help, (*(value[0]))
    dicom_obj -> SetValue, reference, 'Robert Barnett'
    value = dicom_obj -> GetValue(REFERENCE=reference,/no_copy)
;    help, (*(value[0]))
    result = dicom_obj -> Commit('NM-MONO2-16-13x-heart.dcm_copy')
endif
obj_destroy, dicom_obj
;help, /heap

end




pro  GDLffDICOM__test3


dicom_obj = obj_new('GDLffDICOM')

result = dicom_obj -> Read('NM-MONO2-16-13x-heart.dcm')
if (result) then begin
    ;help, result
    reference = dicom_obj -> GetReference('0010'x,'0010'x)
    dicom_obj -> SetValue, reference, 'Robert Barnett'
    value = dicom_obj -> GetValue(REFERENCE=reference,/no_copy)
    ;help, (*(value[0]))
    reference = dicom_obj -> GetReference('0018'x,'0015'x)
    dicom_obj -> SetValue, reference, 'Chicken'
    value = dicom_obj -> GetValue(REFERENCE=reference,/no_copy)
    ;help, (*(value[0]))
    result = dicom_obj -> Commit('NM-MONO2-16-13x-heart.dcm_copy')
endif
obj_destroy, dicom_obj
;help, /heap
end

pro  GDLffDICOM__test4


dicom_obj = obj_new('GDLffDICOM')

result = dicom_obj -> Read('GE_LOGIQBook-8-RGB-HugePreview.dcm',/NO_CATCH)
if (result) then begin
    ;help, result
;    reference = dicom_obj -> GetReference('0008'x,'2111'x)
;    dicom_obj -> SetValue, reference[0], 'Robert Barnett'
;    value = dicom_obj -> GetValue(REFERENCE=reference[0],/no_copy)
;    help, (*(value[0]))
;    reference = dicom_obj -> GetReference('7fe1'x,'0010'x)
;    y = 0
;    dicom_obj -> SetValue, reference[y], 'Chicken'
;    value = dicom_obj -> GetValue(REFERENCE=reference[y],/no_copy)
;    help, (*(value[0]))
    result = dicom_obj -> Commit('GE_LOGIQBook-8-RGB-HugePreview.dcm_copy')
    ;dicom_obj -> dump
endif
obj_destroy, dicom_obj
;help, result
;help, /heap
end


pro GDLffDICOM__test, inds, ALL=all
;/farmyard/robert/download/gdcmdata

if (keyword_set(all)) then inds = indgen(5)
for i=0,n_elements(inds)-1 do begin
    call_procedure, STRING('GDLffDICOM__test',inds[i],FORMAT="(A,I0)")
endfor

end

pro GDLffDICOM__test5
file = '\\a3printer2\farmyard\wmidl\nmtkdualrenal0\test\2008.0.14125138.dcm'
dicom_obj = obj_new('GDLffDICOM')

result = dicom_obj -> Read(file);,/NO_CATCH)
if (result) then begin
	dicom_obj -> dumpElements
endif
obj_destroy, dicom_obj
end


pro  GDLffDICOM__test6

cd, '~/Public/IDL/nmaprototype'
;cd, 'E:\public\IDL\nmaprototype'
dicom_obj = obj_new('GDLffDICOM')

result = dicom_obj -> Read('snapshot_template.dcm')
if (result) then begin
    sop_uid = GDLffDICOM__assoc_GenerateUID(40)
    help, sop_uid
    reference = dicom_obj -> GetReference('0008'x,'0018'x)
    value = dicom_obj -> GetValue(REFERENCE=reference,/no_copy)
    help, (*(value[0]))

    dicom_obj -> SetValue, reference, sop_uid
    value = dicom_obj -> GetValue(REFERENCE=reference,/no_copy)
    help, (*(value[0]))
    result = dicom_obj -> Commit('output0.dcm')
endif
obj_destroy, dicom_obj
;help, /heap
end
