;+
; NAME:
;   CMSV_RDATA
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Read SAVE-formatted data variable record from input block or file unit
;
; CALLING SEQUENCE:
;   CMSV_RDATA, BLOCK, POINTER, SIZE, DATA, UNIT=UNIT, $
;          TEMPLATE=TEMPLATE, /TEMPORARY, PTR_INDEX=PTR_INDEX, $
;          PTR_CALLBACK=PTR_CALLBACK, PTR_OFFSETS=PTR_OFFSETS, $
;          OFFSET=OFFSET, STATUS=STATUS, ERRMSG=ERRMSG
;   
; DESCRIPTION: 
;
;   CMSV_RDATA reads the data portion of an IDL SAVE variable record.
;   An IDL variable is stored in two components: the type descriptor
;   which describes the name, type, and dimensions of the variable;
;   and the data record, which contains the raw data of the variable.
;   This procedure reads the raw data and returns it to the user.  The
;   initial type portion of the record must have already been read
;   using the CMSV_RVTYPE procedure.
;
;   CMSV_RDATA supports the following variable types:
;
;     BYTE(1),INT(2),LONG(3) - integer types
;     UINT(12),ULONG(13),LONG64(14),ULONG64(15) - integer types (IDL >5.2 only)
;     FLOAT(4),DOUBLE(5),COMPLEX(6),DCOMPLEX(9) - float types
;     STRING(7) - string type
;     STRUCT(8) - structure type
;     POINTER(10) - pointer type - SEE BELOW
;     NOT SUPPORTED - OBJ(11) - object reference type - NOT SUPPORTED
;
;   Arrays and structures containing any of the supported types are
;   supported (including structures within structures).
;
;   For scalars and arrays of numeric or string types, the caller must
;   only supply the SIZE parameter, which specifies the type and
;   dimensions of the variable to be read.  This information can be
;   obtained from the CMSV_RVTYPE routine. The data is returned in the
;   output parameter DATA.
;
;   For structure data, in addition to specifying the SIZE array, the
;   user must also supply a "template" describing the structure into
;   which the data will be read.  This template is simply a "blank"
;   form of the data structure, and is returned by CMSV_RVTYPE.
;
;   Thus, a simple way to read structure, numeric or string data is
;   the following code (with error checking removed)
;
;     CMSV_RVTYPE, block, pointer, name, size, template=template, unit=unit
;     CMSV_RDATA, block, pointer, size, data, template=template, unit=unit
;
;   [ This code assumes the record header has been read with
;   CMSV_RREC. ]
;
; ==================================================================
;   Research Systems, Inc. has issued a separate license intended
;   to resolve any potential conflict between this software and the
;   IDL End User License Agreement. The text of that license
;   can be found in the file LICENSE.RSI, included with this
;   software library.
; ==================================================================
;
; POINTER DATA
;
;   Pointer data stored in IDL SAVE files are particularly difficult
;   to manage, because the actual heap variables are stored in
;   separate records which *precede* the record of interest.  Thus, if
;   your application requires the reading of pointer data, you must
;   perform special processing in your own code in order to support
;   it.  In essence, you must maintain an inventory of heap variables
;   as they are encountered in the file.
;
;   If these procedures are not followed then pointer data will not be
;   read, and a LONG integer value appears in the pointers' places.
;   Under IDL 4, pointer data can never be read.
;
;   This is accomplished by placing some additional logic in your file
;   processing loop.  There are four separate components to this: (1)
;   loop initialization; (2) reading a HEAP_INDEX record; (3) parsing
;   a HEAP_DATA record; and (4) passing extra arguments to CMSV_RDATA.
;   The additional state information is maintained in two variables
;   named PTR_INDEX, which keeps track of the heap variable numbers,
;   and PTR_OFFSETS, which stores the file location of each variable.
;
;   (1) Loop initialization: is quite simple, use the following code:
;       ptr_index   = [0L]
;       ptr_offsets = [0L]
;       ptr_data    = [ptr_new()]
;
;   (2) Reading HEAP_INDEX, which is an array of values indicating
;       the heap variable numbers of each heap variables.  These
;       values are stored in PTR_INDEX:
;
;          CMSV_RHEAP, block, pointer, index, unit=unit
;          ptr_index   = [ptr_index, index]
;          ptr_offsets = [ptr_offsets, lonarr(n_elements(index))]
;          ptr_data    = [ptr_data, ptrarr(n_elements(index))]
;
;   (3) Parse the HEAP_DATA record.  Here were are interested in the
;       heap variable number, and the file offset.
;       
;       opointer = pointer
;       CMSV_RVTYPE, block, pointer, vindex, /heap, unit=unit
;       
;       vindex = floor(vindex(0))
;       wh = where(ptr_index EQ vindex)
;       ptr_offsets(wh(0)) = offset + opointer
;
;       Keep in mind that the file offset is OFFSET+POINTER.
;
;   (4) Pass extra parameters to CMSV_RDATA.  The user simply passes
;       these extra variables to the CMSV_RDATA procedure, which
;       automatically recognizes heap data and reads it from the
;       appropriate location.
;
;       CMSV_RVTYPE, block, pointer, name, size, unit=unit, template=tp
;       CMSV_RDATA, block, pointer, size, data, template=tp, $
;         unit=unit, ptr_offsets=ptr_offsets, $
;         ptr_index=ptr_index, ptr_data=ptr_data
;
;   If this technique is used properly, only those heap variables
;   which are needed are read.  Thus, there are never any lost or
;   dangling pointers.  Since each bit of heap data is stored in a
;   variable returned to the user, it is not necessary to
;   PTR_FREE(ptr_data); in fact, doing so would corrupt the input
;   data.
;
; BLOCK, POINTER, OFFSET
;
;   This procedure can read data from a byte array, a file unit, or
;   both.  In fact, this procedure is designed to implement "lazy"
;   reading from a file, which is to say, it normally reads from a
;   byte array of data.  However, if the requested data goes beyond
;   the end of the byte array, more data is read from the file on
;   demand.  This way the user gets the benefit of fast memory access
;   for small reads, but guaranteed file access for large reads.
;
;   The terminology is as follows: BLOCK is a byte array which
;   represents a portion of, or an entire, IDL SAVE file.  The block
;   may be a cached portion of an on-disk file, or an entire in-memory
;   SAVE file.  POINTER is the current file pointer within BLOCK
;   (i.e., the next byte to be read is BLOCK[POINTER]).  Hence, a
;   POINTER value of 0 refers to the start of the block.  OFFSET is
;   the file offset of the 0th byte of BLOCK; thus "POINT_LUN,
;   OFFSET+POINTER" should point to the same byte as BLOCK[POINTER].
;   The following diagram shows the meanings for BLOCK, POINTER and
;   OFFSET schematically:
;
;
;                 0 <-  OFFSET  -> |
;   FILE          |----------------|------*--------|--------->
;
;   BLOCK                          |------*--------|
;                                  0      ^ POINTER
;     
;
;   This procedure is part of the CMSVLIB SAVE library for IDL by
;   Craig Markwardt.  You must have the full CMSVLIB core package
;   installed in order for this procedure to function properly.
;
; INPUTS:
;
;   BLOCK - a byte array, a cache of the SAVE file.  Users will
;           usually not access this array directly.  Users are advised
;           to clear BLOCK after calling POINT_LUN.
;
;   POINTER - a long integer, a pointer to the next byte to be read
;             from BLOCK.  CMSVLIB routines will automatically
;             advance the pointer.
;
;   SIZE - an array of integers describing the type and dimensions of
;          the variable to be read, in the format returned by the
;          SIZE() routine.  This parameter is required.
;
;   DATA - upon output, the data variable.  If any heap data is read,
;          the user is ultimately responsible for freeing it.
;
;
; KEYWORDS:
;
;   UNIT - a file unit.  If a library routine reads to the end of
;          BLOCK, or if BLOCK is undefined, then this file UNIT will
;          be accessed for more data.  If undefined, then BLOCK must
;          contain the entire file in memory.
;
;   TEMPLATE - for structure data (data type 8), a "blank" structure
;              containing the fields and data values to be read in.
;              This structure is returned by CMSV_RVTYPE.
;              This keyword is mandatory for structure data.
;
;   TEMPORARY - if set, BLOCK becomes undefined upon return.
;
;   PTR_OFFSETS - array of file offsets, as described above.  Default:
;                 pointer data is converted to an integer.
;
;   PTR_INDEX - array of heap variable indices, as described above.
;               Default:  pointer data is converted to an integer.
;
;   PTR_DATA - array of pointers, as described above.  
;               Default:  pointer data is converted to an integer.
;
;   OFFSET - the file offset of byte zero of BLOCK.  Default: 0
;            (OFFSET is used by this routine)
;
;   STATUS - upon return, this keyword will contain 1 for success and
;            0 for failure.
;
;   ERRMSG - upon return with a failure, this keyword will contain the
;            error condition as a string.
;
; EXAMPLE:
;
;
; SEE ALSO:
;
;   CMRESTORE, SAVE, RESTORE, CMSVLIB
;
; MODIFICATION HISTORY:
;   Written, 2000
;   Documented, 24 Jan 2001
;   Added UNDEFINED data type for IDL >5.3, CM, 21 Apr 2001
;   Fixed bug for pointers within structures, CM, 21 Apr 2001
;   Add support for IDL 4 byte-compiled strings, CM, 22 Apr 2001
;   Make version checks with correct precision, 19 Jul 2001, CM
;   Added notification about RSI License, 13 May 2002, CM
;
; $Id: cmsv_rdata.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-
forward_function cmsv_rraw

; ---------------- Convert from network to host order ------------------
pro cmsv_rconv, data

  ;; Inspired by IDL Astronomy Library routine IEEE_TO_HOST
  common cmsv_conv_common, lendian
  if n_elements(lendian) EQ 0 then begin
      ;; Little-endian?
      lendian = (long(['01'xb,'02'xb,'03'xb,'04'xb],0,1))(0) NE '01020304'xl
  endif

  sz = size(data)
  case sz(sz(0)+1) of 
      1:  return                    ;; Byte
      2:  byteorder, data, /NTOHS   ;; Integer
      3:  byteorder, data, /NTOHL   ;; Long
      4:  byteorder, data, /XDRTOF  ;; Float
      5:  byteorder, data, /XDRTOD  ;; Double
      6:  byteorder, data, /XDRTOF  ;; Complex
      9:  byteorder, data, /XDRTOD  ;; DComplex
      12: byteorder, data, /NTOHS   ;; UInt
      13: byteorder, data, /NTOHL   ;; ULong
      14: if lendian EQ 1 then byteorder, data, /L64SWAP ;; LONG64
      15: if lendian EQ 1 then byteorder, data, /L64SWAP ;; ULONG64
      ELSE: 
  endcase

end

; ---------------- Read heap data variable ---------------------------
pro cmsv_rhdata, block, pointer, data, index, offsets, pdata, unit=unit0, $
                 offset=offset0, status=status, errmsg=errmsg

  status = 0
  errmsg = ''
  if n_elements(offset0) EQ 0 then offset = 0L $
  else                             offset = floor(offset0(0))

  if n_elements(offsets) EQ 0 OR n_elements(index) EQ 0 then begin
      errmsg = 'ERROR: CMSV_RDATA: must specify INDEX and OFFSETS data'
      return
  endif

  if n_elements(offsets) NE n_elements(index) then begin
      errmsg = 'ERROR: CMSV_RDATA: dimensions of OFFSETS and '+ $
        'INDEX do not match'
      return
  endif
  
  if n_elements(unit0) EQ 0 then begin
      errmsg = 'ERROR: CMSV_RDATA: Must specify UNIT when reading heap data'
      return
  endif
  unit = floor(unit0(0))

  sz = size(data)
  odata = temporary(data)

  ;; Create the properly sized array.  Don't worry about getting the
  ;; dimensions right since it will be reformed.
  if sz(0) GT 0 then begin
      data = ptrarr(sz(sz(0)+2))
  endif else begin
      data = ptr_new()
  endelse

  blimits = offset + [0,n_elements(block)-1]

  point_lun, -unit, opos
  null = ptr_new()
  
  for i = 0, n_elements(odata)-1 do begin
      if odata(i) EQ 0 then goto, NEXT_HEAP

      wh = (where(odata(i) EQ index, ct))(0)
      if ct EQ 0 then goto, NEXT_HEAP

      if n_elements(pdata) GT wh then begin
          if pdata(wh) NE null then begin
              data(i) = pdata(wh)
              goto, NEXT_HEAP
          endif
      endif

      point_lun, unit, offsets(wh)

      block1 = 0 & dummy = temporary(block1)
      p1 = 0L
      cmsv_rvtype, block1, p1, varname, sz1, status=status, template=tp1, $
        unit=unit, errmsg=errmsg, structure_name=stname, /heap
      if status EQ 0 then return

      if floor(varname) NE odata(i) then begin
          errmsg = 'ERROR: CMSV_RDATA: heap index mismatch'
          status = 0
          return
      endif

      val = 0 & dummy = temporary(val)
      cmsv_rdata, block1, p1, sz1, val, template=tp1, status=st1, $
        ptr_offsets=offsets, ptr_index=index, ptr_data=pdata, $
        ptr_callback='DEFAULT', unit=unit, errmsg=errmsg
      if status EQ 0 then return

      ;; Deal with case of undefined heap var
      if n_elements(val) GT 0 then data(i) = ptr_new(temporary(val)) $
      else                         data(i) = ptr_new(/allocate_heap)
      if n_elements(pdata) GT wh then pdata(wh) = data(i)
      NEXT_HEAP:
  endfor

  point_lun, unit, opos
  status = 1
  return
end

; --------------------------- Main procedure ------------------------------
pro cmsv_rdata, block, pointer, sz, data, offset=offset, unit=unit, $
                template=template1, status=status, errmsg=errmsg, $
                start=start, temporary=temp, $
                bytelong=bytelong, bcstring40=bcstring, $
                ptr_offsets=pprivate, ptr_callback=callback0, $
                ptr_index=pindex, ptr_data=pdata

  ;; VAR_DATA
  ;;   LONG - START_DATA TOKEN - value 7
  ;;   for bytes - consecutive bytes
  ;;   for (u)ints - upcast to type long 
  ;;   for (u)longs - consecutive longs
  ;;   for pointers - consecutive longs, indices into saved heap data
  ;;   for strings - consecutive STRING's
  ;;   for structs - compacted versions of above
  forward_function fix, uint, complex, dcomplex, make_array

  data = 0 & dummy = temporary(data)
  if n_elements(start) EQ 0 then start = 1
  if n_elements(pointer) EQ 0 then pointer = 0L

  tp = sz(sz(0)+1)

  if tp EQ 0 then begin     ;; UNDEFINED type
      data = 0
      dummy = temporary(data)
      return
  endif
  if (tp EQ 11) then begin  ;; OBJECT type
      status = 0
      errmsg = 'ERROR: CMSV_RDATA: cannot read object data'
      return
  endif

  if keyword_set(start) then begin
      start_token = cmsv_rraw(/long, block, pointer, status=status, $
                               unit=unit, errmsg=errmsg)
      if start_token NE 7 then begin
          errmsg = 'ERROR: CMSV_RDATA: invalid or corrupted data'
          status = 0
      endif
      if status EQ 0 then return
  endif

  nelt = sz(sz(0)+2)
  if (tp EQ 10) then tp = 3 ;; Pointer type -> LONG

  if (tp EQ 8) then begin   ;; Structure type
      if n_elements(template1) EQ 0 then begin
          errmsg = 'ERROR: CMSV_RDATA: TEMPLATE must be passed for structures'
          status = 0
          return
      endif
      data = reform(replicate(template1(0), nelt), sz(1:sz(0)), /overwrite)
      tp1 = data(0)
      tn = tag_names(data(0)) & nt = n_elements(tn)
      ssz = lonarr(12, nt)
      for j = 0L, nt-1 do begin
          ssz(0, j) = size(data(0).(j))
      endfor
      for i = 0L, nelt-1 do begin
          for j = 0L, nt-1 do begin
              cmsv_rdata, block, pointer, ssz(*,j), dataij, start=0, $
                template=(tp1.(j))(0), $
                ptr_offsets=pprivate, ptr_index=pindex, ptr_data=pdata, $
                ptr_callback=callback0, $
                unit=unit, status=status, errmsg=errmsg
              data(i).(j) = dataij
              
              if status EQ 0 then return
          endfor
      endfor
      return
  endif

  if tp EQ 7 then begin     ;; String type
      if sz(0) EQ 0 then data = '' $
      else data = reform(strarr(sz(sz(0)+2)), sz(1:sz(0)), /overwrite)
      for i = 0L, nelt-1 do begin
          len = cmsv_rraw(/long, block, pointer, status=status, $
                             unit=unit, errmsg=errmsg)
          if status EQ 0 then return
          if len GT 0 OR keyword_set(bcstring) then $
            data(i) = cmsv_rraw(/string, block, pointer, unit=unit, $
                                status=status, errmsg=errmsg)
          if status EQ 0 then return
      endfor
      if sz(0) EQ 0 then data = data(0)
      return
  endif
  
  ;; Sometimes the input data is stored as a different type
  common cmsv_datatypes, stype, sbyte, nbyte, selts
  if n_elements(stype) EQ 0 then begin
      ;;      0      1       2       3        4        5         6    7   8
      ;;          byte     int    long    float    double   complex str struct
      stype =['','BYTE', 'LONG', 'LONG', 'FLOAT', 'DOUBLE', 'FLOAT', '', '' ]
      sbyte =[0,     1,      4,      4,       4,        8,       4,   0,  0 ]
      selts =[0,     1,      1,      1,       1,        1,       2,   0,  0 ]

      ;;                  9     10   11     12       13        14
      ;;            dcomplex   ptr   obj   uint     ulong    long64   ulong64
      stype =[stype,'DOUBLE', 'LONG', '', 'ULONG', 'ULONG', 'LONG64','ULONG64']
      sbyte =[sbyte,      8,     4,   0,     4,       4,        8,        8, 0]
      selts =[selts,      2,     1,   0,     1,       1,        1,        1, 0]
  endif

  status = 0
  nb = sbyte(tp<16)
  if nb EQ 0 then begin
      errmsg = ('ERROR: CMSV_RDATA: cannot read specified type ('+ $
                strtrim(tp,2)+')')
      return
  endif
  nelt1 = nelt*selts(tp<16)     ;; Account for complex type
  nb  = nb*nelt1                ;; Number of total bytes
  nb1 = long(floor((nb+3)/4)*4) ;; Number of bytes, rounded to next long
  rt  = stype(tp<16)            ;; Read type, as opposed to output type

  ;; Error handler returns zero and error condition
  on_ioerror, READ_ERROR
  if 0 then begin
      READ_ERROR: CMSV_RDATA:
      errmsg = 'ERROR: CMSV_RDATA: a read error occurred'
      return
  end

  ;; Byte values handled specially....    argghhh!
  if tp EQ 1 AND NOT keyword_set(bytelong) then begin
      nb1 = cmsv_rraw(/long, block, pointer, unit=unit, $
                       errmsg=errmsg, status=status)
      if status EQ 0 then return
      if nb1 NE nb then begin
          errmsg = 'ERROR: CMSV_RDATA: byte count and array size do not agree'
          status = 0
          return
      endif
      ;; Round up to the nearest 4-byte boundary
      nb1 = long(floor((nb+3)/4)*4)
  endif else if tp EQ 1 AND keyword_set(bytelong) then begin

      ;; Input data will be read as long... required when decoding
      ;; values in code blocks
      nb    = sbyte(3)
      nelt1 = nelt*selts(3)
      nb    = nb*nelt1
      nb1   = nb
      rt    = stype(3)
  endif

  ;; Convert the data
  pp = pointer
  if n_elements(block) GT 0 then begin
      if pointer+nb GT n_elements(block) then begin
          dummy = cmsv_rraw(block, pointer, nb, /byte, /buffer, unit=unit, $
                            status=status, errmsg=errmsg)
          if status EQ 0 then return
      endif

      if keyword_set(temp) then $
        data = call_function(rt, (temporary(block))(pp:pp+nb-1), 0, nelt1) $
      else $
        data = call_function(rt, block(pp:pp+nb-1), 0, nelt1)
  endif else begin
      sz1 = size(call_function(rt,0))
      data = make_array(type=sz1(sz1(0)+1), nelt1, /nozero)
      readu, unit(0), data
  endelse
  pointer = pointer + nb1
  cmsv_rconv, data

  ;; Special conversions
  if (tp EQ 2)  then data = fix(temporary(data))
  if (tp EQ 12) then data = uint(temporary(data))
  if (tp EQ 6)  then data = complex(temporary(data), 0, nelt)
  if (tp EQ 9)  then data = dcomplex(temporary(data), 0, nelt)

  ;; Reset error handler
  on_ioerror, NULL

  ;; Convert to pointer type
  if (sz(sz(0)+1) EQ 10) AND double(!version.release) GE 5D $
    AND n_elements(pprivate) GT 0 AND n_elements(pindex) GT 0 then begin

      ;; The data we have read is actually just an index number into
      ;; the heap.  We must read each heap variable separately.
      if n_elements(callback0) GT 0 then callback = strtrim(callback0(0),2) $
      else                               callback = 'DEFAULT'
      if callback EQ 'DEFAULT' then      callback = 'CMSV_RHDATA'

      if callback NE '' then begin
          call_procedure, callback, block, pointer, data, $
            pindex, pprivate, pdata,$
            unit=unit, offset=offset, status=status, errmsg=errmsg
          if status EQ 0 then return
      endif
  endif
      
  ;; Reform array
  if sz(0) GT 0 then data = reform(data, sz(1:sz(0)), /overwrite) $
  else data = data(0)

  status = 1
  return
end
