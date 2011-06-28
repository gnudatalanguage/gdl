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
;<P>Definition of each DICOM tag in the buffer</P>
;@private
;-
pro GDLffDICOMTag__define, struct
struct = {GDLffDICOMTag, $
          group_number: 0u, $
          element_number: 0u, $
          index:0l,$
          vr:'', $
          len:0l, $
          description: '', $
          commit: 0b, $
          value: ptr_new()}
end

;+
;<P>Definition of each DICOM Dictionary entry</P>
;@private
;-
pro GDLffDICOMDictionary__define, struct, INIT=init
struct = replicate({GDLffDICOMDictionary, $
                    group_number: 0u, $
                    element_number: 0u,$
                    vr: '', $
                    vm: '', $
                    name: '', $
                    version: ''},2085)
if (keyword_set(init)) then begin
; The Dictionary procedure is generated code from dicom.dic in the GPL
; release of DCMTK by OFFIS software
gdlffdicom__dictionary, struct
endif
end

;+
;<P>Open a DICOM file and parse DICOM tags</P>
;@param filename {in}{required}{type=String} Open and parse this filename
;@keyword auto_syntax {in}{optional} Automatically determine the
;transfer syntax (recommended)
;@keyword raise {in}{optional} Raise an exception in the calling
;program if an error occurs (useful for debugging)
;@keyword index_tags {in}{optional} Index group and element numbers
;(necessary for accessing DICOM tags by group and element number)
;@keyword index_sequences {in}{optional} Index sequence information
;(necessary for accessing DICOM tags in nested sequences)
;@keyword implicit_vr {in}{optional} Parse using the Implicit VR transfer syntax
;@keyword explicit_vr {in}{optional} Parse using the Explicit VR transfer syntax
;@keyword big_endian {in}{optional} Parse as a Big Endian file
;@keyword little_endian {in}{optional} Parse as a Little Endian file
;@keyword limit {in}{optional}{type=long} Only parse a limited number of bytes
;@keyword meta_only {in}{optional} Only parse the meta header
;@keyword access_time {out}{optional} The number of seconds taken to open and
;parse the file.
;@keyword no_catch {private} Do not catch any exceptions
;@returns 1 if file was opened and parsed, 0 if there was an error
;-
function GDLffDICOM__assoc::Open, filename, IMPLICIT_VR=implicit_vr, EXPLICIT_VR=explicit_vr, ACCESS_TIME=access_time, BIG_ENDIAN=big_endian, LITTLE_ENDIAN=little_endian, $
LIMIT=limit, INDEX_TAGS=index_tags, META_ONLY=meta_only, AUTO_SYNTAX=auto_syntax, NO_CATCH=no_catch, RAISE=raise, INDEX_SEQUENCES=index_sequences, READ_ONLY=read_only

; Check keywords
if (keyword_set(auto_syntax) and keyword_set(meta_only)) then message, LEVEL=-1, "Auto syntax and meta only are mutually exclusive keywords."
if (~ keyword_set(auto_syntax)) then begin
    if (~keyword_set(little_endian) and ~keyword_set(big_endian)) then auto_syntax = 1b
    ; message, LEVEL=-1, "Neither big nor little endian is set"
    if (~keyword_set(implicit_vr) and ~keyword_set(explicit_vr)) then auto_syntax = 1b
    ; message, LEVEL=-1, "Neither explicit of implicit VR is set"
endif
if (keyword_set(little_endian) and keyword_set(big_endian)) then message, LEVEL=-1, "Cannot set both big and little endian"
if (keyword_set(big_endian) and keyword_set(implicit_vr)) then message, LEVEL=-1, "Implicit big endian is not a vlaid transfer syntax"

if (self.lun ne 0) then message, LEVEL=-1, "Cannot open another file while one is already open. Try GDLffDICOM__assoc::close"

if (~ keyword_set(no_catch)) then begin
    CATCH, Error_status
    IF Error_status NE 0 then begin
        ; Close the file and generate an error message
        Error_message = !ERR_STRING
        CATCH,/CANCEL
        self -> close
        if (keyword_set(raise)) then begin
            on_error, 2
            Message, Error_message, LEVEL=-1 ; Generate message to caller
        endif else return, 0b
    endif
endif

if (arg_present(access_time) or (n_elements(access_time) gt 0)) then start_time = systime(1)

if (keyword_set(auto_syntax)) then begin
    index_tags = 1
    ; Just open the meta header of the file at little endian
    if (self -> open(filename,  /IMPLICIT_VR, /LITTLE_ENDIAN, /META_ONLY, INDEX_TAGS=index_tags, /NO_CATCH, /READ_ONLY)) then begin
        ; Read the tansfer syntax tag
        if (self -> readelement('0002'x,'0010'x, TransferSyntaxUID, VR='UI')) then begin
            TransferSyntaxUID = (strtrim(gdlffdicom_trim(strjoin(TransferSyntaxUID,'')),2))
             case (TransferSyntaxUID) of
                '1.2.840.10008.1.2.2': begin
                    little_endian=0
                    implicit_vr=0
                end
                '1.2.840.10008.1.2.1': begin
                    little_endian=1
                    implicit_vr=0
                end
                '1.2.840.10008.1.2': begin
                    little_endian=1
                    implicit_vr=1
                end
                else: message, "Transfer syntax unknown '" + TransferSyntaxUID + "' for file " +  filename
            endcase
            big_endian = ~ little_endian
            explicit_vr = ~ implicit_vr
        endif else message, "Transfer Syntax UID not found"
    endif
    self -> close
endif

if(keyword_set(explicit_vr)) then self.explicit_vr = 1b
if(keyword_set(implicit_vr)) then self.explicit_vr = 0b

self.index_tags = keyword_set(index_tags)
self.index_sequences = keyword_set(index_sequences)

if (keyword_set(big_endian)) then self.big_endian = 1b
if (keyword_set(little_endian)) then self.big_endian = 0b

self.read_only = keyword_set(read_only)
if (self.read_only) then begin
   if (self.big_endian) then $
      openr, lun, filename, /get_lun, /swap_if_little_endian $ ; This is a big endian file
   else $
      openr, lun, filename, /get_lun, /swap_if_big_endian ; This is a little endian file
endif else begin
   if (self.big_endian) then $
      openu, lun, filename, /get_lun, /swap_if_little_endian $ ; This is a big endian file
   else $
      openu, lun, filename, /get_lun, /swap_if_big_endian ; This is a little endian file
endelse

self.lun = lun
; The standard DICOM header is 132 bytes
offset = 128l
point_lun, self.lun, offset
chars = '    '
readu, self.lun, chars
if (chars ne 'DICM') then message, "Not a valid DICOM file " + filename
offset = offset + 4l
inside_metadata = 1b
index = 0l
bufgrow = 1024l
bufsize = bufgrow
(*self.offsets) = lonarr(bufgrow)
(*self.lens) = lonarr(bufgrow)
(*self.vrs) = strarr(bufgrow)
if (self.index_tags) then begin
    (*self.group_numbers) = uintarr(bufgrow)
    (*self.element_numbers) = uintarr(bufgrow)
endif
if (self.index_sequences) then begin
    (*self.parent_sequences) = lonarr(bufgrow)
    (*self.item_numbers) = lonarr(bufgrow)
    (*self.parent_items) = lonarr(bufgrow)
    current_sequence = replicate(-1l,20)
    current_item = replicate(-1l,20)
    current_item_number = replicate(-1l,20)
    current_nesting = 0l
    stop_offset = replicate('FFFFFFFF'x,20)
endif
stat = fstat(lun)
if (n_elements(limit) eq 0) then limit =  stat.size
while (offset + 4l lt limit) do begin
    is_delimiter = 0b

    if (inside_metadata or self.index_tags) then begin
        point_lun, self.lun, offset
        tag = uintarr(2)
        readu, self.lun, tag
        is_delimiter = (tag[0] eq 'FFFE'x) and ((tag[1] eq 'E000'x) or (tag[1] eq 'E00D'x) or (tag[1] eq 'E0DD'x))

        if (inside_metadata) then begin
                                ; metadata is always little endian
                                ; a big endian file will need to be swapped again
            if (self.big_endian) then $
              swap_endian_inplace, tag

            ; No more 0002 group tags indicates the end of the meta-data set
            if (tag[0] ne '0002'x) then begin
                inside_metadata = 0b
                if (self.big_endian) then $
                  swap_endian_inplace, tag

                if (keyword_set(meta_only)) then begin
                    self.size = index
                    if (arg_present(access_time) or (n_elements(access_time) gt 0)) then access_time = systime(1) - start_time

                    return, 1b
                endif
            endif
        endif
    endif

    if (~is_delimiter and (self.explicit_vr or inside_metadata)) then begin

        offset = offset + 4l
        point_lun, self.lun, offset
        vr = '  '
        readu, self.lun, vr

        vr_inds = where(vr eq ['OB', 'OW', 'OF', 'SQ', 'UT', 'UN'],vr_count)
        if (vr_count gt 0) then begin
            offset = offset + 4l
            point_lun, self.lun, offset
            len = 0ul
            readu, self.lun, len
            offset = offset + 4l
        endif else begin
            offset = offset + 2l
            point_lun, self.lun, offset
            len = 0u
            readu, self.lun, len
            offset = offset + 2l
        endelse
    endif else begin
        vr = ''
        dict_inds = where((self.dictionary.group_number eq tag[0]) and (self.dictionary.element_number eq tag[1]),dict_count)
        if (dict_count gt 0) then vr =  self.dictionary[dict_inds[0]].vr
        offset = offset + 4l
        point_lun, self.lun, offset
        len = 0l
        readu, self.lun, len
        offset = offset + 4l
    endelse

    if (inside_metadata) then begin
        ; metadata is always little endian
        ; a big endian file will need to be swapped again
        if (self.big_endian) then $
          swap_endian_inplace, len
    endif

    (*self.offsets)[index] = offset
    (*self.lens)[index] = len
    (*self.vrs)[index] = vr
    if (self.index_tags) then begin
        (*self.group_numbers)[index] = tag[0]
        (*self.element_numbers)[index] = tag[1]
    endif
    if (self.index_sequences) then begin
        (*self.parent_sequences)[index] = current_sequence[current_nesting]
        if (is_delimiter and (tag[1] eq 'E000'x)) then begin
            len = 0l
            current_item_number[current_nesting] = current_item_number[current_nesting] + 1l
            (*self.item_numbers)[index] = current_item_number[current_nesting]
            current_item[current_nesting] = index
            if ((*self.parent_sequences)[index] ge 0) then $
              (*self.parent_items)[index] = (*self.parent_items)[(*self.parent_sequences)[index]]
        endif else begin
            (*self.parent_items)[index] = current_item[current_nesting]
            (*self.item_numbers)[index] = current_item_number[current_nesting]
            if (vr eq 'SQ') then begin
                current_nesting = current_nesting + 1
                if (current_nesting ge 20) then message, "No more than 20 sequences can be nested"
                current_sequence[current_nesting] = index
                current_item_number[current_nesting] = 0l
                if (len ne 'FFFFFFFF'x) then stop_offset[current_nesting] = offset + len $
                else stop_offset[current_nesting] = 'FFFFFFFF'x
                len = 0l
            endif
            if (is_delimiter and (tag[1] eq 'E0DD'x) and (stop_offset[current_nesting] eq 'FFFFFFFF'x)) then $
              stop_offset[current_nesting] = offset + len
        endelse
        while ((current_nesting gt 0) and (stop_offset[current_nesting] ne 'FFFFFFFF'x) and $
               (stop_offset[current_nesting] le offset + len)) do begin
            current_nesting = current_nesting -1
        endwhile
    endif

    if (len ne 'FFFFFFFF'x) then offset = offset + len


    index = index + 1
    if (index ge bufsize) then begin
        (*self.offsets) = [(*self.offsets),lonarr(bufgrow)]
        (*self.lens) = [(*self.lens),lonarr(bufgrow)]
        (*self.vrs) = [(*self.vrs),strarr(bufgrow)]
        if (self.index_tags) then begin
            (*self.group_numbers) = [(*self.group_numbers),bytarr(bufgrow)]
            (*self.element_numbers) =  [(*self.element_numbers),bytarr(bufgrow)]
        endif
        if (self.index_sequences) then begin
            (*self.parent_sequences) = [(*self.parent_sequences),lonarr(bufgrow)]
            (*self.parent_items) = [(*self.parent_items),lonarr(bufgrow)]
            (*self.item_numbers) = [(*self.item_numbers),lonarr(bufgrow)]
        endif
        bufsize = bufsize + bufgrow
    endif
endwhile
self.size = index
if (arg_present(access_time) or (n_elements(access_time) gt 0)) then access_time = systime(1) - start_time
return, 1b
end

;+
;<P>Dump all parsed information to the console</P>
;-
pro GDLffDICOM__assoc::Dump, LUN=lun

for index=0l,self.size-1l do begin
    ; Need to make it look like DumpElements
    str = string(index, (*self.offsets)[index], (*self.lens)[index], FORMAT="(I5,I10,I10)")
    if (self.index_tags) then $
      str = str + string((*self.group_numbers)[index], (*self.element_numbers)[index],FORMAT="(' [',Z04,',',Z04,'] ')")
    if (self.explicit_vr) then $
      str = str + ' ' + (*self.vrs)[index]
    if (self.index_sequences) then $
      str = str + string((*self.parent_sequences)[index],(*self.parent_items)[index], FORMAT="(I10,I10)")
    if (n_elements(lun) gt 0) then printf, lun, str $
    else print, str
endfor
end

;+
;<P>Find a DICOM tag by group and element numbers</P>
;@param group_number {in}{type=int}{required} An UINT specifying the group number of the
;tag
;@param element_number {in}{type=int}{required} An UINT specifying the element number of the
;tag
;@keyword count {out}{type=lonarr}{optional} The number of matching DICOM
;tags.
;@returns The indicies of the matching dicom tags in the file.
;-
function GDLffDICOM__assoc::findtaginds, group_number, element_number, COUNT=count
group_inds = where(group_number eq *self.group_numbers,count)
if (count gt 0) then begin
    element_inds = where(element_number eq (*self.element_numbers)[group_inds],count)
    if (count gt 0) then return, group_inds[element_inds]
endif
count = 0l
return, [-1l]
end

;+
;<P>Read a DICOM tag which has the given group and element number. The
;VR must be specified for an implicit VR file.</P>
;@param group_number {in}{type=int}{optional} An UINT specifying the group number of the
;tag
;@param element_number {in}{type=int}{optional} An UINT specifying the element number of the
;tag
;@keyword vr {in}{optional} Read field using this VR. This must be
;specified for an implicit VR file.
;@param value_out {out}{required} The value read from the DICOM file
;@keyword offset {out}{optional} The byte offset where the value was
;found
;@keyword index {in}{optional} Read the element given at this position
;rather than searching for the group, element pair
;@returns 1 if the element was read. 0 if it wasn't found
;-
function GDLffDICOM__assoc::readelement, group_number, element_number, value_out, OFFSET=offset, INDEX=index, VR=vr, SKIP_UNSUPPORTED=skip_unsupported

if ((n_elements(index) eq 0)) then begin
    inds = self -> findtaginds(group_number, element_number,COUNT=count)
    if ((count gt 0)) then index = inds[0]
endif
if (n_elements(index) gt 0) then begin
    offset = (*self.offsets)[index]
    point_lun, self.lun, offset
    len = (*self.lens)[index]
    if (len eq 0) then return, 0b
    vr = (*self.vrs)[index]
    if (n_elements(vr) gt 0) then begin
    	value_out = self -> generatevalue(vr, len,SKIP_UNSUPPORTED=skip_unsupported)
	    readu, self.lun, value_out
	    return, 1b
    endif else begin
    	if (~keyword_set(skip_unsupported)) then $
    		message, "VR keyword must be specified for implicit files"
		return, 0b
    endelse
endif
return, 0b
end


;+
;<P>Generate an IDL variable for the given VR and byte length</P>
;@private
;@param vr {required}{in} The Value Representation for the variable
;@param len {required}{in} The length of the value
;@returns The IDL variable
;-
function GDLffDICOM__assoc::generatevalue, vr, len, SKIP_UNSUPPORTED=skip_unsupported

vrs =   ['AE','AS','AT','CS','DA','DL','DS','DT','FL','FD','IS','LO','LT','OB','OF','OW','PN','SH','SL','SQ','SS','ST','TM','UI','UL','UN','US','UT','xs']
types = [7   ,7   ,13  ,7   ,7   ,0   ,7   ,7   ,4   ,5   ,7   ,7   , 7  ,1   ,5   ,2   ,7   ,7   ,3   ,0   ,2   ,7   ,7   ,7   ,13  ,1   ,12  ,7, 12]
; xs could be SS or US depending on context

vr_inds = where(vrs eq vr,count)
if (count gt 0) then begin
    type = types[vr_inds[0]]
    case (type) of
        1: return, bytarr(len)
        2: return, intarr(len/2l)
        3: return, lonarr(len/4l)
        4: return, fltarr(len/2l)
        5: return, dblarr(len/4l)
        7: return, strjoin(replicate(' ', len),'')
        12: return, uintarr(len/2l)
        13: return, ulonarr(len/4l)
        else: message, "Unsupported VR type " + vr
    endcase
endif else begin
	if (~keyword_set(skip_unsupported)) then message, 'Unsupported VR ' + vr
	return, 0b
endelse


end

function GDLffDICOM__assoc::NewSOPInstanceUID, group_number, element_number
if (n_elements(group_number) eq 0) then group_number = '0008'x
if (n_elements(element_number) eq 0) then element_number = '0018'x
return, self -> NewUID(group_number, element_number)
end

;+
;<P>Generate and write a new SOP Instance UID. This generates a UID
;such that it doesn't change the length of the file.</P>
;@returns The SOP Instance UID String or an empty string if no UID
;could be generated.
;-
function GDLffDICOM__assoc::NewUID, group_number, element_number

if (n_elements(time_multiplier) eq 0) then time_multiplier = 1l $
else time_multiplier = time_multiplier+1l


if (self -> readelement(group_number,element_number, SOPInstanceUID, OFFSET=offset, VR='UI')) then begin
    SOPInstanceUID = strtrim(SOPInstanceUID) ; Remove trailing blanks only
    SOPInstanceUID = GDLffDICOM__assoc_GenerateUID(strlen(SOPInstanceUID),/EMPTY_ON_ERROR,ERROR=error)
    if ((SOPInstanceUID ne '')) then begin
        point_lun, self.lun, offset
        if (~ self.read_only) then $
            writeu, self.lun, SOPInstanceUID $
        else return, ''
    endif else errm = dialog_message(["GDLffDICOM Error: Cannot set UID of " + (fstat(self.lun)).name,error])
    return, SOPInstanceUID
endif

return, ''
end

;+
;<P>Write a new SOP Instance UID. This generates a UID
;must be the same length such that it doesn't change the length of the file.</P>
;-
function GDLffDICOM__assoc::WriteElement, group_number, element_number, newValue, VR=vr
if (self -> readelement(group_number,element_number, value, OFFSET=offset, VR=vr)) then begin
    value = strtrim(value) ; Remove trailing blanks only
    if (strlen(value) ne strlen(newValue)) then return, 0b
    if ((value ne '')) then begin
        point_lun, self.lun, offset
        if (~ self.read_only) then $
            writeu, self.lun, newValue $
        else return, 0b
    endif else errm = dialog_message(["GDLffDICOM Error: Cannot set value of " + (fstat(self.lun)).name,error])
    return, 1b
endif

return, 0b
end

;+
;<P>Write a new SOP Instance UID. This generates a UID
;must be the same length such that it doesn't change the length of the file.</P>
;-
function GDLffDICOM__assoc::WriteUID, group_number, element_number, NewInstanceUID
if (self -> readelement(group_number,element_number, SOPInstanceUID, OFFSET=offset, VR='UI')) then begin
    SOPInstanceUID = strtrim(SOPInstanceUID) ; Remove trailing blanks only
    if (strlen(SOPInstanceUID) ne strlen(NewInstanceUID)) then return, 0b
    if ((NewInstanceUID ne '')) then begin
        point_lun, self.lun, offset
        if (~ self.read_only) then $
            writeu, self.lun, NewInstanceUID
    endif else errm = dialog_message(["GDLffDICOM Error: Cannot set UID of " + (fstat(self.lun)).name,error])
    return, 1b
endif

return, 0b
end



;+
; <P>Associate the contents of the pixeldata tag in the DICOM file with
; an image. This will automatically determine the location of the
; pixeldata tag and its dimensions.</P>
; @keyword count {out}{optional} The number of images contained in the associated variable
; @keyword index {in}{optional} The index of the pixeldata tag
; @keyword image {in}{optional} Associate using the dimensions of this
; value
; @keyword true {out}{optional} If greater than 1 use this value as an argument for
; rendering the image using tv
;@keyword access_time {out}{optional} The number of seconds taken to
;assocate the value.
;@keyword raise {in}{optional} Raise an exception in the calling
;program if an error occurs (useful for debugging)
;@keyword no_catch {private} Do not catch any exceptions
;@returns The associated variable
;-
function GDLffDICOM__assoc::assoc, INDEX=index, IMAGE=value, COUNT=count, ACCESS_TIME=access_time, $
                          NO_CATCH=no_catch, RAISE=raise, TRUE=true, OFFSET=offset


if (~ keyword_set(no_catch)) then begin
    CATCH, Error_status
    IF Error_status NE 0 then begin
        Error_message = !ERR_STRING
        CATCH,/CANCEL
        if (keyword_set(raise)) then begin
            on_error, 2
            Message, Error_message, LEVEL=-1 ; Generate message to caller
        endif else begin
            count = 0
            return, [0]
        endelse
    endif
endif

if (arg_present(access_time) or (n_elements(access_time) gt 0)) then start_time = systime(1)

; Useful DICOM elements
;(0028,0010) US 2, 1 Rows
;(0028,0011) US 2, 1 Columns
;(0028,0100) US 2, 1 BitsAllocated
;(0028,0101) US 2, 1 BitsStored
;(0028,0102) US 2, 1 HighBit
;(0028,0103) US 2, 1 PixelRepresentation

true = 0b


if (n_elements(index) eq 0) then begin
    inds = self -> findtaginds('7fe0'x,'0010'x,COUNT=count)
    if (count gt 0) then index = inds[0] else message, "No pixeldata tag found"
endif

if (n_elements(value) eq 0) then begin
    if (self -> readelement('0028'x,'0002'x,samples,VR='US') and $
        self -> readelement('0028'x,'0010'x,rows,VR='US') and $
        self -> readelement('0028'x,'0011'x,columns,VR='US') and $
        self -> readelement('0028'x,'0100'x,bitsallocated,VR='US') and $
        self -> readelement('0028'x,'0101'x,bitsstored,VR='US') and $
        self -> readelement('0028'x,'0103'x,pixelrepresentation,VR='US')) then begin

        ; if (bitsallocated ne bitsstored) then  message, "Only understands same number of stored and allocated bits" + string(bitsallocated,bitsstored)
        case (bitsallocated) of
            8: begin
                if (pixelrepresentation eq 1u) then  message, "Bytes cannot be signed"
                if (samples gt 1u) then begin
                    if (~ self -> readelement('0028'x,'0006'x,planar,VR='US')) then planar = 0u
                    if (planar) then begin
                        value = bytarr(columns,rows,samples)
                        if (samples eq 3u) then true = 3b
                    endif else begin
                        value = bytarr(samples,columns,rows)
                        if (samples eq 3u) then true = 1b
                    endelse
                endif else  $
                  value = bytarr(columns,rows)
            end
            16: begin
                if (pixelrepresentation eq 1u) then value = intarr(columns,rows) $
                else value = uintarr(columns,rows)
            end
            else: message, "Unknown image depth"
        endcase
    endif else message, "Image dimensions not found"
endif


s = size(value)
case (s[s[0l]+1l]) of
    1: nbytes = s[s[0l]+2l] ; byte
    2: nbytes = s[s[0l]+2l]*2l ; integer
    3: nbytes = s[s[0l]+2l]*4l ; long
    4: nbytes = s[s[0l]+2l]*2l ; float
    5: nbytes = s[s[0l]+2l]*4l ; double
    12: nbytes = s[s[0l]+2l]*2l ; unsigned integer
    13: nbytes = s[s[0l]+2l]*4l ; unsigned long
    14: nbytes = s[s[0l]+2l]*8l ; 64-bit int
    15: nbytes = s[s[0l]+2l]*8l ; 64-bit uint
    else: message, "Unsupported type"
endcase

if ((index ge 0) and (index lt self.size)) then begin
    count = long((*self.lens)[index] / nbytes)
    if (count * nbytes ne (*self.lens)[index]) then $
      message, "The size of the value doesn't wholly fit inside the dicom field. " + string( nbytes, (*self.lens)[index], FORMAT="('value[',I0,'], dicom[',I0,']')")
    if (arg_present(access_time) or (n_elements(access_time) gt 0)) then access_time = systime(1) - start_time
    offset = (*self.offsets)[index]
    return, assoc(self.lun, value, offset)
endif else count = 0


end


;+
; <P>Write a new DICOM file tag by tag. The output file will
; contain values specified in the argument, otherwise the values are
; copied from the existing file. The argument should be a
; struct array containing the following fields</P>
;  <UL>
;         <LI>group_number (UINT)
;         <LI>element_number (UINT)
;         <LI>index (LONG)
;         <LI>vr (STRING)
;         <LI>len (LONG)
;         <LI>value (PTR)
;  </UL>
; @param values {in}{optional} A struct array with new values to write
;-
pro GDLffDICOM__assoc::write, filename, values

if ((fstat(self.lun)).name eq filename) then message, "Cannot commit to a file from which we are reading"

if (self.big_endian) then $
  openw, write_lun, filename, /get_lun, /swap_if_little_endian $ ; This is a big endian file
else $
  openw, write_lun, filename, /get_lun, /swap_if_big_endian ; This is a little endian file

writeu, write_lun, bytarr(128)
writeu, write_lun, 'DICM'

inside_metadata = 1b
is_delimiter = 0b
adj_lens = (*self.lens)

; Adjust sequence lengths to handle modified elements
for i=0,n_elements(values)-1l do begin
    index = values[i].index
    ;help, values[i], /structure, adj_lens, index, adj_lens[index],  values[i].len
    ;print, adj_lens[index] ne values[i].len
    if (adj_lens[index] ne values[i].len) then begin
        diff =  values[i].len - adj_lens[index]
  ;      help, diff
        if (self.index_sequences) then begin
            parent_index = (*self.parent_sequences)[index]
            parent_item = (*self.parent_items)[index]
            while (parent_index ge 0) do begin
                if (adj_lens[parent_index] ne ulong('FFFFFFFF'x)) then $
                  adj_lens[parent_index] = adj_lens[parent_index] + diff
                if (parent_item ge 0) then begin
                    if (adj_lens[parent_item] ne ulong('FFFFFFFF'x)) then $
                      adj_lens[parent_item] = adj_lens[parent_item] + diff
                endif
                parent_index = (*self.parent_sequences)[parent_index]
            endwhile
        endif
    endif
endfor

; Write file tag by tag
for index=0l,self.size-1l do begin
    if (n_elements(values) gt 0) then $
      v_inds = where(index eq values.index,v_count) $
    else v_count = 0
    if (v_count gt 0) then begin
        i = v_inds[0]
        tag = uint([values[i].group_number,values[i].element_number])
        len = values[i].len
        vr = values[i].vr
    endif else begin
        tag = uint([(*self.group_numbers)[index],(*self.element_numbers)[index]])
        len = adj_lens[index]
        vr = (*self.vrs)[index]
    endelse

    ;inds = where((*self.parent_items) eq index,count)
    ;if (count gt 0) then begin
    ;    total_len = ulong(total(adj_lens[inds]))
    ;    if (total_len ne len) then print, "Length mismatch ", len, total_len,count
    ;    ;else  print, "Length match ", total_len, len
    ;endif

    is_delimiter = (tag[0] eq 'FFFE'x) and ((tag[1] eq 'E000'x) or (tag[1] eq 'E00D'x) or (tag[1] eq 'E0DD'x))
    if (~self.explicit_vr and inside_metadata) then begin
        case (tag[1]) of
            '0000'x: vr = 'UL'
            '0001'x: vr = 'OB'
            '0002'x: vr = 'UI'
            '0003'x: vr = 'UI'
            '0010'x: vr = 'UI'
            '0012'x: vr = 'UI'
            '0013'x: vr = 'SH'
            '0016'x: vr = 'AE'
            '0100'x: vr = 'UI'
            '0102'x: vr = 'OB'
            else: vr = 'UN'
        endcase
    endif
    if (inside_metadata) then begin
                                ; metadata is always little endian
                                ; a big endian file will need to be swapped again
                                ; No more 0002 group tags indicates the end of the meta-data set
        if (tag[0] ne '0002'x) then $
          inside_metadata = 0b
    endif
    if (inside_metadata) then begin
         if (self.big_endian) then $
          swap_endian_inplace, tag
    endif

    writeu, write_lun, tag[0:1]

    if (~is_delimiter and (self.explicit_vr or inside_metadata)) then begin

        writeu, write_lun, strmid(vr,0,2)

        vr_inds = where(vr eq ['OB', 'OW', 'OF', 'SQ', 'UT', 'UN'],vr_count)
        if (vr_count gt 0) then begin

            writeu, write_lun, uint('0000'x)

            len = ulong(len)
        endif else begin
            len = uint(len)
        endelse
    endif else begin
        len = ulong(len)
    endelse
        write_len  = len[0]
    if (inside_metadata) then begin
                                ; metadata is always little endian
                                ; a big endian file will need to be swapped again
        if (self.big_endian) then $
          swap_endian_inplace, write_len
    endif

    ;if (write_len eq ulong('FFFFFFFF'x)) then print, "undefined length" $
    ;else begin
    ;endelse


    writeu, write_lun, write_len


    if ((vr ne 'SQ') and ~is_delimiter and self.index_sequences) then begin
        if (v_count gt 0) then begin
            writeu, write_lun, *values[i].value
        endif else begin
            offset = (*self.offsets)[index]
            if (long64(offset) + len le (fstat(self.lun)).size) then begin
                point_lun, self.lun, offset
                gdlffdicom_copy_lun, self.lun, write_lun, len
            endif else begin
                print, "Warning, truncating bytes"
                point_lun, self.lun, offset
                gdlffdicom_copy_lun, self.lun, write_lun, (fstat(self.lun)).size - offset
            endelse
        endelse
    endif
    endfor
free_lun, write_lun
end


;+
;<P>Close the file and free the lun</P>
;-
pro GDLffDICOM__assoc::close
if (self.lun gt 0) then begin
    free_lun, self.lun
    self.lun = 0
endif
self.size = 0l
end


;+
;<P>Cleanup pointers</P>
;-
pro GDLffDICOM__assoc::cleanup
if (self.lun gt 0) then free_lun, self.lun
ptr_free, self.lens
ptr_free, self.offsets
ptr_free, self.vrs
ptr_free, self.group_numbers
ptr_free, self.element_numbers
ptr_free, self.parent_sequences
ptr_free, self.parent_items
ptr_free, self.item_numbers
end


;+
;<P>Initialise the object</P>
;-
function GDLffDICOM__assoc::init
GDLffDICOMDictionary__define, dictionary, /INIT
self.dictionary  = dictionary
self.lens = ptr_new(/ALLOCATE_HEAP)
self.offsets = ptr_new(/ALLOCATE_HEAP)
self.vrs = ptr_new(/ALLOCATE_HEAP)
self.group_numbers = ptr_new(/ALLOCATE_HEAP)
self.element_numbers = ptr_new(/ALLOCATE_HEAP)
self.parent_sequences = ptr_new(/ALLOCATE_HEAP)
self.parent_items = ptr_new(/ALLOCATE_HEAP)
self.item_numbers = ptr_new(/ALLOCATE_HEAP)
return, 1
end

;+
;<H3>The DICOM update object</H3>
;<P>This object allows you to parse a DICOM file and associate the
;pixel data with an IDL variable. This means that you can make changes
;to the data in a DICOM file easily and efficently without touching
;any other parts of the file. It is recommended that you use
; dcmodify from the DICOM toolkit (www.dcmtk.org) to change any
; non-pixel DICOM tags. A wrapper class for dcmtk is provided in this release.
;</P>
;-
pro GDLffDICOM__assoc__define, struct
GDLffDICOMDictionary__define, struct

struct = {GDLffDICOM__assoc, $
          dictionary: struct, $
          lun: 0l, $
          lens: ptr_new(), $
          offsets: ptr_new(), $
          vrs: ptr_new(), $
          group_numbers: ptr_new(), $
          element_numbers: ptr_new(), $
          parent_sequences: ptr_new(), $
          parent_items: ptr_new(), $
          item_numbers: ptr_new(), $
          index_sequences: 0B, $
          size: 0l, $
          index_tags: 0B, $
          explicit_vr: 0B, $
          big_endian: 0B, $
          read_only: 0b $
}
end
