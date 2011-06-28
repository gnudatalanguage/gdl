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



;+
;<P>Convert a GDLffDICOM References into GDLffDICOM__assoc indexes</P>
;@private
;-
function GDLffDICOM::Indexes, references
if (self.size lt self.ntags) then begin
    inds = where(references gt self.size,count)
    if (count gt 0) then references[inds] = self.pixel_index
endif
return, references
end

;+
;<P>Find the indexes of DICOM tags which match the ith Dictionary entry</P>
;@private
;@param inds {in}{required} A subset of indexes to search
;@param find_inds {out}{required} The indicies to the indexes which
;match
;@param i {in}{required} The dictionary entry to search
;-
pro GDLffDICOM::FindDefinedElement, inds, find_inds, i
group_inds = where((self.dictionary[i]).group_number eq (*self.group_numbers)[inds],group_count)
if (group_count gt 0) then begin
    element_inds = where((self.dictionary[i]).element_number eq (*self.element_numbers)[inds[group_inds]],element_count)
    if (element_count gt 0) then begin
        if (n_elements(find_inds) gt 0) then $
          find_inds = [find_inds,group_inds[element_inds]] $
        else $
          find_inds = [group_inds[element_inds]]
    endif
endif
end

;+
;<P>Find the Dictionary entries for and matching DICOM tags</P>
;@private
;-
function GDLffDICOM::GetDictionary, group_number, element_number, REFERENCE=references
if (n_elements(references) eq 0) then begin
    references = self -> GetReference(group_number, element_number)
endif
if (references[0] eq -1) then return, [-1]
dictionaries = replicate({GDLffDICOMDictionary},n_elements(references))
for i=0l,n_elements(references)-1l do begin
    group_number = (*self.group_numbers)[self -> indexes(references[i])]
    element_number = (*self.element_numbers)[self -> indexes(references[i])]
    group_inds = where(self.dictionary.group_number eq group_number,group_count)
    if (group_count gt 0) then begin
        element_inds = where((self.dictionary.element_number)[group_inds] eq element_number,element_count)
        if (element_count gt 0) then begin
            dictionaries[i] = (self.dictionary)[group_inds[element_inds[0]]]
        endif
    endif
endfor
return, dictionaries
end

;+
;<P>Open the file for updating and initialise dicom tag buffer</P>
;@private
;-
function GDLffDICOM::Open2, filename, _REF_EXTRA=ex
self -> reset
result = self -> GDLffDICOM__assoc::Open(filename,_EXTRA=ex, /INDEX_SEQUENCES, /INDEX_TAGS)
if (self.size gt 0) then begin
    *self.pixel_assoc = self -> assoc(INDEX=index, COUNT=count)
    if (count gt 0) then begin
        self.pixel_index = index
        self.frame_count = count
        self.ntags = self.size + count -1l
    endif else self.ntags = self.size
    (*self.dicom_tags) = replicate({GDLffDICOMTag},self.ntags)
endif
; Set up the extra pixel indicies here
return, result
end


;+
;<P>Destroy the object</P>
;-
pro GDLffDICOM::Cleanup
if (n_elements(*self.dicom_tags) gt 0) then  $
  for i=0l,self.ntags-1l do ptr_free, (*self.dicom_tags)[i].value
ptr_free, self.dicom_tags
ptr_free, self.pixel_assoc
self -> GDLffDICOM__assoc::Cleanup
end

;+
;<P>Dump a description of all elements</P>
;-
pro GDLffDICOM::DumpElements, filename
fmt = "(I4,' : (',Z04,',',Z04,') : ',A2,' : ',A,' : ',I0,' : ',A)"

if (n_elements(filename) gt 0) then openw, lun, filename, /GET_LUN
values = self -> getValue(/NO_COPY)
for i=0l,self.ntags-1l do begin
    dicom_tag = (*self.dicom_tags)[i]
    if ((ptr_valid(dicom_tag.value) && (n_elements(*dicom_tag.value) gt 0))) then value = *dicom_tag.value $
    else value = ""
    case (size(value,/type)) of
        else: begin
            if (n_elements(value) gt 12) then $
              value = strjoin(strtrim((string(value[0:11])),2),' ') + ' ...' $
            else $
              value = strjoin(strtrim((string(value)),2),' ')
        end
    endcase
    if (n_elements(lun) gt 0) then begin
	    printf, lun, i, dicom_tag.group_number, dicom_tag.element_number, $
	           dicom_tag.vr, dicom_tag.description, dicom_tag.len, value, $
	           FORMAT=fmt
	endif else begin
	    print, i, dicom_tag.group_number, dicom_tag.element_number, $
	           dicom_tag.vr, dicom_tag.description, dicom_tag.len, value, $
	           FORMAT=fmt
	endelse
endfor
if (n_elements(lun) gt 0) then free_lun, lun
end

;+
;<P>Get references to all child elements of this reference.</P>
;-
function GDLffDICOM::GetChildren, reference
if (self.index_sequences and (n_elements(reference) gt 0)) then begin
    references = where((*self.parent_sequences) eq self -> indexes(reference[0]),count)
    if (count gt 0) then return, references $
    else return, -1
endif else $
  return, -1
end

;+
;<P>Return an array of string descriptions, as defined in the DICOM
;dictionary of DCMTK by OFFIS software</P>
;-
function GDLffDICOM::GetDescription, group_number, element_number, REFERENCE=references
dictionaries = self -> GetDictionary(group_number, element_number, REFERENCE=references)
if (size(dictionaries,/type) eq 8) then return, [dictionaries.name]
return, [-1]
end

;+
;<P>Return an array of DICOM element numbers</P>
;-
function GDLffDICOM::GetElement, group_number, element_number, REFERENCE=references
if (n_elements(references) eq 0) then begin
    references = self -> GetReference(group_number, element_number)
endif else references = [references]
if (references[0] eq -1) then return, [-1]
return, long((*self.element_numbers)[self -> indexes(references)])
end

;+
;<P>Return an array of DICOM group numbers</P>
;-
function GDLffDICOM::GetGroup, group_number, element_number, REFERENCE=references
if (n_elements(references) eq 0) then begin
    references = self -> GetReference(group_number, element_number)
endif else references = [references]
if (references[0] eq -1) then return, [-1]
return, long((*self.group_numbers)[self -> indexes(references)])
end

;+
;<P>Return an array of the length of elements in bytes</P>
;-
function GDLffDICOM::GetLength, group_number, element_number, REFERENCE=references
if (n_elements(references) eq 0) then begin
    references = self -> GetReference(group_number, element_number)
endif else references = [references]
if (references[0] eq -1) then return, [-1]
return, (*self.lens)[self -> indexes(references)]
end

;+
;<P>Get references to all parent element of this reference.</P>
;-
function GDLffDICOM::GetParent, references
if (self.index_sequences) then $
    return, [(*self.parent_sequences)[self -> indexes(references)]] $
else $
  return, [-1]
end

;+
;<P>Return the preamble of the DICOM file</P>
;-
function GDLffDICOM::GetPreamble
preamble = bytarr(128)
if (self.lun gt 0) then begin
    point_lun, self.lun, 0
    readu, self.lun, preamble
    return, preamble
endif
end

;+
;<P>Return an array of references which match the arguments</P>
;-
function GDLffDICOM::GetReference, group_number, element_number, DESCRIPTION=description, VR=vr
inds = indgen(self.ntags)
if (n_elements(group_number) gt 0) then begin
    group_inds = where(group_number eq (*self.group_numbers)[inds],group_count)
    if (group_count gt 0) then inds = inds[group_inds] else return, -1
endif
if (n_elements(element_number) gt 0) then begin
    element_inds = where(element_number eq (*self.element_numbers)[inds],element_count)
    if (element_count gt 0) then inds = inds[element_inds] else return, -1
endif
if ((n_elements(description) gt 0)) then begin
    for i=0,n_elements(self.dictionary)-1l do begin
        if (strpos(strlowcase((self.dictionary[i]).name),strlowcase(description)) ge 0) then begin
            self -> FindDefinedElement, inds, desc_inds, i
        endif
    endfor
    if (n_elements(desc_inds) gt 0) then inds = inds[desc_inds] else return, -1
endif
if ((n_elements(vr) gt 0)) then begin
;    if (self.explicit_vr) then begin
    vr_inds = where(vr eq (*self.vrs)[inds],vr_count)
    if (vr_count gt 0) then inds = inds[vr_inds] else return, -1
;   endif else begin
;    dict_inds = where((self.dictionary.vr) eq vr,dict_count)
;    for j=0l,n_elements(dict_inds)-1l do $
;      self -> FindDefinedElement, inds, vr_inds, dict_inds[j]
;    if (n_elements(vr_inds) gt 0) then inds = inds[vr_inds] else return, -1
;    endelse
endif
pixel_inds = where(inds eq self.pixel_index, pixel_count)
if ((pixel_count gt 0) and (self.ntags gt self.size)) then begin
    inds = [inds,self.size+indgen(self.ntags-self.size)]
endif
return, inds
end

;+
;<P>Return an array of pointers to DICOM tag values</P>
;@keyword pixeldata Use this keyword to return all the pixeldat tags
;-
function GDLffDICOM::GetValue, group_number, element_number, REFERENCE=references, NO_COPY=no_copy


if (n_elements(references) eq 0) then begin
    references = self -> GetReference(group_number, element_number)
endif else references = [references]
if (references[0] eq -1) then return, [-1]

;help, references
dictionaries = self -> GetDictionary(REFERENCE=references)
;help, references, *self.dicom_tags
if (self.explicit_vr) then vrs = (*self.vrs)[references] $
else vrs =  dictionaries.vr
for i=0,n_elements(references)-1l do begin
     if (~ptr_valid((*self.dicom_tags)[references[i]].value)) then begin
         (*self.dicom_tags)[references[i]].description = dictionaries[i].name
         (*self.dicom_tags)[references[i]].vr = vrs[i]
         (*self.dicom_tags)[references[i]].group_number = (*self.group_numbers)[references[i]]
         (*self.dicom_tags)[references[i]].element_number = (*self.element_numbers)[references[i]]
         (*self.dicom_tags)[references[i]].index = references[i]
         (*self.dicom_tags)[references[i]].commit = 0b
         if ((references[i] eq self.pixel_index)) then begin ;  and (self.ntags gt self.size)
             (*self.dicom_tags)[references[i]].len = (*self.lens)[references[i]]/self.frame_count
             (*self.dicom_tags)[references[i]].value = ptr_new((*self.pixel_assoc)[0])
             for j=self.size,self.ntags-1l do begin
                 (*self.dicom_tags)[j] = (*self.dicom_tags)[references[i]]
                 (*self.dicom_tags)[j].value = ptr_new((*self.pixel_assoc)[j-self.size+1l])
             endfor
         endif else begin
             inds = where(vrs[i] eq ['','SQ','DL'],unsupported_count)
             if (unsupported_count eq 0) then begin
                 if (self -> readelement(group_number, element_number, value_out, OFFSET=offset, INDEX=references[i], VR=vrs[i], /SKIP_UNSUPPORTED)) then begin
                     (*self.dicom_tags)[references[i]].len = (*self.lens)[references[i]]
                     (*self.dicom_tags)[references[i]].value = ptr_new(value_out)
                 endif else begin
                     (*self.dicom_tags)[references[i]].len = 0
                     (*self.dicom_tags)[references[i]].value = ptr_new(/ALLOCATE_HEAP)
                 endelse
             endif else begin
                 (*self.dicom_tags)[references[i]].len = (*self.lens)[references[i]]
                 (*self.dicom_tags)[references[i]].value = ptr_new(/ALLOCATE_HEAP)
             endelse
         endelse
    endif
;    help,  (*self.dicom_tags)[references[i]].value
;    help,  *(*self.dicom_tags)[references[i]].value
endfor
if (keyword_set(no_copy)) then begin
    return, [((*self.dicom_tags)[references]).value]
endif else begin
    ptrcopy = ptrarr(n_elements(references),/allocate_heap)
    for i=0l,n_elements(references)-1l do begin
        ptr = ((*self.dicom_tags)[references[i]]).value
        if (n_elements(*ptr) gt 0) then *ptrcopy[i] = *ptr
    endfor
    return, ptrcopy
endelse
end

;+
;<P>Return an array of the VR or DICOM tags</P>
;-
function GDLffDICOM::GetVR, group_number, element_number, REFERENCE=references
if (self.explicit_vr) then begin
    if (n_elements(references) eq 0) then begin
        references = self -> GetReference(group_number, element_number)
    endif else references = [references]
    if (references[0] eq -1) then return, [-1]
   return, (*self.vrs)[self -> indexes(references)]
endif else begin
    dictionaries = self -> GetDictionary(group_number, element_number,REFERENCE=references)
    if (size(dictionaries,/type) eq 8) then return, (dictionaries.vr)
    return, [-1]
endelse
end

;+
;<P>Initialise the object</P>
;-
function GDLffDICOM::Init, filename, VERBOSE=verbose
if (~ self -> GDLffDICOM__assoc::Init()) then return, 0
self.dicom_tags = ptr_new(/ALLOCATE_HEAP)
self.pixel_assoc = ptr_new(/ALLOCATE_HEAP)
if (n_elements(filename)) then return, self -> Read(filename)
return, 1
end

;+
;<P>Open a file for reading and writing</P>
;-
function GDLffDICOM::Read, filename, ENDIAN=endian, _REF_EXTRA=ex
if (n_elements(endian) gt 0) then begin
    case (endian) of
        1: return, self -> Open2(filename, /IMPLICIT_VR, /LITTLE_ENDIAN, _EXTRA=ex)
        2: return, self -> Open2(filename, /EXPLICIT_VR, /LITTLE_ENDIAN, _EXTRA=ex)
        3: return, self -> Open2(filename, /IMPLICIT_VR, /BIG_ENDIAN, _EXTRA=ex)
        4: return, self -> Open2(filename, /EXPLICIT_VR, /BIG_ENDIAN, _EXTRA=ex)
    endcase
endif else return, self -> Open2(filename, _EXTRA=ex)
end

;+
;<P>Close the file and reset all buffers</P>
;-
pro GDLffDICOM::Reset
if (n_elements(*self.dicom_tags) gt 0) then  $
  for i=0l,self.ntags-1l do ptr_free, (*self.dicom_tags)[i].value
self -> close
end


;+
;<P>Get the byte offset of DICOM tags in the file</P>
;-
function GDLffDICOM::GetOffset, group_number, element_number, REFERENCE=references
if (n_elements(references) eq 0) then begin
    references = self -> GetReference(group_number, element_number)
endif else references = [references]
if (references[0] eq -1) then return, [-1]
return, (*self.offsets)[self -> indexes(references)]
end

;+
;<P>Set the value the first matching DICOM element if it exists, but do not write it until the commit
;method is called</P>
;@returns The reference for the element written
;-
function GDLffDICOM::SetElement, group_number, element_number, value, REFERENCE=reference
if (n_elements(value) eq 0) then message, "Setting element to a null value is not supported"
if (n_elements(reference) eq 1) then begin
    self -> SetValue, reference, value
    return, reference
endif else begin
    references = self -> GetReference(group_number, element_number)
    if (references[0] gt 0) then $
      self -> SetValue, references[0], value
    return, references[0]
endelse
end


;+
;<P>Copy a values of a group from the specified dicom object into this
;object. The tag must exist in this object for it to be copied.
; All elements in the group are copied unless a list of element
;numbers is explicitly provided.
;</P>
;-

pro GDLffDICOM::CopyGroup, dicom_obj, group_number, element_numbers

if (n_elements(element_numbers) gt 0) then begin
    src_references = lonarr(n_elements(element_numbers))
    dst_references = lonarr(n_elements(element_numbers))
    for i=0l,n_elements(element_numbers)-1l do begin
        src_references[i] = (dicom_obj -> GetReference(group_number, element_numbers[i]))[0]
        dst_references[i] = (self -> GetReference(group_number, element_numbers[i]))[0]
    endfor
endif else begin
    dst_references = self -> GetReference(group_number)
    element_numbers = self -> GetElement(REFERENCE=dst_references)
    src_references = lonarr(n_elements(element_numbers))
    for i=0l,n_elements(element_numbers)-1l do begin
        src_references[i] = (dicom_obj -> GetReference(group_number, element_numbers[i]))[0]
    endfor
endelse

for i=0l,n_elements(dst_references)-1l do begin
;    print, "Copying element ", src_references[i], " to ", dst_references[i]
    if ((dst_references[i] ge 0) and (src_references[i] ge 0)) then begin
        ptr = (dicom_obj -> GetValue(REFERENCE=src_references[i]))[0]
        if (n_elements(*ptr) gt 0) then self -> SetValue, dst_references[i], ptr,/USE_PTR
    endif
endfor

end

;+
;<P>Set the value of a DICOM tag, but do not write it until the commit
;method is called</P>
;-
pro GDLffDICOM::SetValue, reference, value, USE_PTR=use_ptr

if (n_elements(reference) ne 1) then message, "Must specify one and only one reference"

if ((reference gt 0) and (reference lt self.size) and (reference ne self.pixel_index)) then begin
    if (~ptr_valid((*self.dicom_tags)[reference].value)) then (*self.dicom_tags)[reference].value = ptr_new(/ALLOCATE_HEAP)
    vr = (self -> GetVR(REFERENCE=reference))[0]
    group_number = (self -> GetGroup(REFERENCE=reference))[0]
    element_number = (self -> GetElement(REFERENCE=reference))[0]
    vrs =   ['AE','AS','AT','CS','DA','DL','DS','DT','FL','FD','IS','LO','LT','OB','OF','OW','PN','SH','SL','SQ','SS','ST','TM','UI','UL','UN','US','UT']
    types = [7   ,7   ,13  ,7   ,7   ,0   ,7   ,7   ,4   ,5   ,7   ,7   , 7  ,1   ,5   ,2   ,7   ,7   ,3   ,0   ,2   ,7   ,7   ,7   ,13  ,1   ,12  ,7]
    vr_inds = where(vrs eq vr,count)
    if (count gt 0) then begin
        type = types[vr_inds[0]]
        ; Temporarily dereference the pointer without copying the data
        if (keyword_set(use_ptr)) then vvalue = temporary(*value) $
        else vvalue = temporary(value)
        if (type ne size(vvalue,/type)) then message, "IDL type does not match VR"
        case (type) of
            1: len = n_elements(vvalue)
            2: len = n_elements(vvalue) * 2l
            3: len = n_elements(vvalue) * 4l
            4: len = n_elements(vvalue) * 2l
            5: len = n_elements(vvalue) * 4l
            7: begin
                len = strlen(vvalue)
                if (len mod 2 eq 1) then begin
                    vvalue = vvalue + ' '
                    len = len + 1l
                endif
            end
            12: len = n_elements(vvalue) * 2l
            13: len = n_elements(vvalue) * 4l
            else: message, "Unsupported VR type"
        endcase
        if (keyword_set(use_ptr)) then *value = temporary(vvalue) $
        else value = temporary(vvalue)
    endif else message, 'Unsupported VR'
    (*self.dicom_tags)[reference].group_number = group_number
    (*self.dicom_tags)[reference].element_number = element_number
    (*self.dicom_tags)[reference].vr = vr
    (*self.dicom_tags)[reference].len = len
    if (keyword_set(use_ptr)) then (*self.dicom_tags)[reference].value = value $
    else *(*self.dicom_tags)[reference].value = value
    (*self.dicom_tags)[reference].commit = 1b
    (*self.dicom_tags)[reference].index = reference
endif else message, 'Cannot set value'
end

;+
;<P>Write all DICOM tags to a new file</P>
;-
function GDLffDICOM::Commit, filename
inds = where(((*self.dicom_tags)[0:self.size-1l]).commit,count)
if (count gt 0) then values = (*self.dicom_tags)[inds]
self -> write, filename, values
return, 1
end


;+
;<P>A DICOM reader and writer</P>
;-
pro GDLffDICOM__define, struct
struct = {GDLffDICOM, inherits GDLffDICOM__assoc, $
          dicom_tags: ptr_new(), $
          pixel_index: 0l, $
          pixel_assoc: ptr_new(), $
          frame_count: 0l, $
          ntags: 0l $
}
end
