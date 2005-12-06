;+
; NAME:
;   CMSV_WVTYPE
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Write variable type information to output block
;
; CALLING SEQUENCE:
;   CMSV_WVTYPE, BLOCK, DATA, IDENT, $
;                HEAP=HEAP, SYSTEM=SYSTEM, UNIT=UNIT, $
;                OFFSET=OFFSET, STATUS=STATUS, ERRMSG=ERRMSG
;   
; DESCRIPTION: 
;
;   CMSV_WVTYPE writes the type portion of an IDL SAVE variable
;   record.  An IDL variable is stored in two components: the type
;   descriptor which describes the name, type, and dimensions of the
;   variable; and the data record, which contains the raw data of the
;   variable.  This procedure writes the type descriptor based on a
;   variable passed by the user.  Once the type descriptor has been
;   written, the data portion can be written using the CMSV_WDATA
;   procedure.
;
;   CMSV_WVTYPE should recognize and correctly output type descriptors
;   for all currently known IDL data types, except for object
;   references.  Type information is inferred from the DATA parameter
;   passed by the user.  
;
;   Heap data is a special case, since the data itself are stored
;   separately from the pointer in the SAVE file.  Three steps must be
;   satisfied: a HEAP_INDEX record must be written; a valid HEAP_DATA
;   record must be written containing a type descriptor (written with
;   this procedure) and the heap data; and the named pointer itself
;   must be written.
;
;
; ==================================================================
;   Research Systems, Inc. has issued a separate license intended
;   to resolve any potential conflict between this software and the
;   IDL End User License Agreement. The text of that license
;   can be found in the file LICENSE.RSI, included with this
;   software library.
; ==================================================================
;
; BLOCK, POINTER, OFFSET
;
;   This procedure writes data to a byte array only.  The intent is
;   for users to accumulate a significant amount of data in a BLOCK
;   and then write it out with a single call to WRITEU.  Users should
;   be aware that the block can be larger than the buffered data, so
;   they should use something like the following:
;
;          WRITEU, UNIT, BLOCK(0:POINTER-1)
;
;   When library routines do indeed write buffered BLOCK data to disk,
;   they will appropriately reset the BLOCK and POINTER.  Namely,
;   BLOCK will be reset to empty, and POINTER will be reset to zero.
;   OFFSET will be advanced the according number of bytes.
;
;   The terminology is as follows: BLOCK is a byte array which
;   represents a portion of, or an entire, IDL SAVE file.  The block
;   may be a cached portion of an on-disk file, or an entire in-memory
;   SAVE file.  POINTER is the current file pointer within BLOCK
;   (i.e., the next byte to be written is BLOCK[POINTER]).  Hence, a
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
;
; INPUTS:
;
;   BLOCK - a byte array, a cache of the SAVE file.  Users will
;           usually not access this array directly.  Users are advised
;           to clear BLOCK after calling POINT_LUN or writing the
;           block to disk.
;
;   POINTER - a long integer, a pointer to the next byte to be written
;             from BLOCK.  CMSVLIB routines will automatically advance
;             the pointer.
;
;   DATA - the data to be written, of any save-able data type.
;
;   IDENT - for variables, the name of the variable as a string; for
;           heap data, the heap index as an integer.
;
; KEYWORDS:
;
;   HEAP - if set, the data is treated as heap data, and IDENT must be
;          an integer heap index.
;
;   SYSTEM - if set, the data is assumed to be a system variable.
;
;   UNIT - a file unit.  Currently ignored.
;
;   OFFSET - the file offset of byte zero of BLOCK.  
;            Upon output, if the file pointer is advanced, OFFSET will
;            also be changed.
;            (OFFSET is not currently used by this routine)
;            Default: 0
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
;   Added notification about RSI License, 13 May 2002, CM
;
; $Id: cmsv_wvtype.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-
pro cmsv_warrdesc, block, pointer, sz, status=status, length=length, $
                   errmsg=errmsg
  ;; ARRAY_DESC
  ;;   LONG - START_TOKEN - value 08 - array descriptor flag
  ;;   LONG - number of bytes per element
  ;;   LONG - number of bytes per array
  ;;   LONG - number of elements per array
  ;;   LONG - number of variable dimensions
  ;;   LONG - unknown (value 0)
  ;;   LONG - unknown (checksum?)
  ;;   LONG - maximum number of stored dimensions ( = 8 )
  ;;   LONGx8 - dimensions of number
  forward_function make_array

  if n_elements(pointer) EQ 0 then pointer = 0L
  nelt = sz(sz(0)+2)
  tp = sz(sz(0)+1)

  if sz(0) EQ 0 then begin
      errmsg = 'ERROR: CMSV_WVTYPE: data must be an ARRAY type'
stop
      status = 0
      return
  endif

  if n_elements(length) EQ 0 then $
    length = n_tags({test:make_array(1,type=tp)}, /length)
  buff = lonarr(16)
  buff(0) = 8           ;; Array descriptor token
  buff(1) = length/nelt ;; Unknown value
  buff(2) = length      ;; Number of bytes
  buff(3) = nelt        ;; Number of elements
  buff(4) = sz(0)       ;; Number of dimensions
  buff(7) = 8           ;; Number of stored dimensions
  buff(8:15) = 1        ;; All eight dimensions start as 1  ...
  buff(8) = sz(1:sz(0)) ;; ... and are overwritten by the actual dimensions

  cmsv_wraw, /long, block, pointer, buff, status=status, errmsg=errmsg
end

pro cmsv_wstructdesc, block, pointer, status=status, template=template1, $
                      errmsg=errmsg, nocatch=nocatch

  ;; STRUCT_DESCR
  ;;   LONG - START_TOKEN - value 9 - struct descriptor flag
  ;;   STRING - name of struct (or 0 if anonymous)
  ;;   LONG - unknown - value 0
  ;;   LONG - N_TAGS - number of structure tags
  ;;   LONG - total "length" in bytes, but nothing meaningful
  ;;   TAGDESCxN_TAGS - TAG_TABLE - description of each tag
  ;;   STRINGxN_TAGS - TAG_NAMES - name of each tag
  ;;   ARRAY_DESCxN_ARRAYS - ARRAY_TAB - descriptor for each array
  ;;   STRUCT_DESCRxN_STRUCTS - STRUCT_TAB - descriptor for each struct

  if n_elements(pointer) EQ 0 then pointer = 0L
  cmsv_wraw, /long, block, pointer, 9L, status=status, errmsg=errmsg
  if status EQ 0 then return
  templ8 = template1(0)
  sn = tag_names(templ8, /structure_name)
  cmsv_wraw, /string, block, pointer, sn, status=status, errmsg=errmsg
  if status EQ 0 then return

  ;; Create structure descriptor header
  buff = lonarr(3)
  buff(1) = n_tags(templ8)          ;; Number of tags
  buff(2) = n_tags(templ8, /length) ;; Length of struct in bytes
  cmsv_wraw, /long, block, pointer, buff, status=status, errmsg=errmsg
  if status EQ 0 then return

  nt = buff(1)
  tn = tag_names(templ8)

  status = 0

  ;; Retrieve type and dimensions of each structure element
  forward_function tagsize
  ssz = tagsize(templ8, length=tlength, status=status)
  if status EQ 0 then begin
      errmsg = 'ERROR: CMSV_WVTYPE: cannot determine structure variable type'
      return
  endif

  ;; Create tag table - type descriptions of each tag
  ;; LONG - offset
  ;; LONG - IDL variable type
  ;; LONG - type flags '20'x: struct, '14'x: array
  offset = 0L
  tag_table = reform(lonarr(3,nt), 3, nt, /overwrite)
  for i = 0L, nt-1 do begin
      sz = ssz(*,i)
      tp = sz(sz(0)+1)
      tag_table(0,i) = offset
      tag_table(1,i) = tp
      if tp EQ 8    then tag_table(2,i) = '20'x
      if sz(0) GT 0 then tag_table(2,i) = tag_table(2,i) OR '14'x

      ilength = tlength(i) 
      if tag_table(2,i) NE 0 then ilength = floor((tlength(i)+3)/4)*4
      offset = offset + ilength
  endfor
  cmsv_wraw, /long, block, pointer, tag_table, status=status
  if status EQ 0 then return

  ;; Write tag names
  ;; STRING - name of tag
  cmsv_wraw, /string, block, pointer, tn, status=status, errmsg=errmsg

  ;; Write array descriptors, one for each array in struct
  for i = 0L, nt-1 do begin
      sz = reform(ssz(*,i))
      if (tag_table(2,i) AND '24'x) NE 0 then begin
          cmsv_warrdesc, block, pointer, sz, status=status, length=tlength(i),$
            errmsg=errmsg
          if status EQ 0 then return
      endif
  endfor

  ;; Write struct descriptors - one for each struct with this struct
  for i = 0L, nt-1 do begin
      sz = reform(ssz(*,i))

      ;; Recursively read struct descriptor
      if (tag_table(2,i) AND '20'x) NE 0 then begin
          cmsv_wstructdesc, block, pointer, status=status, $
            template=(templ8.(i))(0), errmsg=errmsg
          if status EQ 0 then return
      endif
  endfor

  status = 1
  return
end

pro cmsv_wvtype, block, pointer, data, ident, heap=heap, system=system, $
                 offset=offset, unit=unit, $
                 status=status, errmsg=errmsg

  ;; VARIABLE BLOCK
  ;;   STRING - VARNAME - name of variable
  ;;   TYPEDESC - type descriptor

  ;; HEAP VARIABLE
  ;;   LONG - HEAP_INDEX - heap index of data value
  ;;   LONG - value 0x02 or 0x12 - unknown
  ;;   TYPEDESC - type descriptor
  status = 0
  if n_elements(pointer) EQ 0 then pointer = 0L
  sz = size(data)
  if sz(sz(0)+1) EQ 0 then begin
      errmsg = 'ERROR: CMSV_WVTYPE: DATA must be set'
      return
  endif
  if keyword_set(heap) then begin
      ;; Write heap index
      if n_elements(ident) EQ 0 then ident = 0L
      cmsv_wraw, /long, block, pointer, [long(ident(0)), '02'xl], $
        status=status, errmsg=errmsg
  endif else begin
      ;; Write variable name
      if n_elements(ident) EQ 0 then ident = '<UNKNOWN>'
      cmsv_wraw, /string, block, pointer, strtrim(strupcase(ident(0)),2), $
        status=status, errmsg=errmsg
  endelse
  if status EQ 0 then return

  ;; Write out the type descriptors

  ;; SCALAR_TYPE
  ;;   LONG - variable type (IDL type code)
  ;;   LONG - value 0

  ;; ARRAY_TYPE
  ;;   LONG - variable type (IDL type code)
  ;;   LONG - ARRTYPE - values 0x14 (or 0x34 for struct)
  ;;   ARRAY_DESC - array descriptor
  ;;   STRUCT_DESC (if a structure) - structure descriptor

  vartype = sz(sz(0)+1)
  ;; IDL variable type

  status = 0
  if vartype EQ 0 then begin
      errmsg = 'CMSV_WVTYPE: ERROR: DATA must be defined'
      return
  endif
  
  if vartype EQ 11 then begin
      errmsg = 'CMSV_WVTYPE: ERROR: OBJREF data cannot be written'
      return
  endif

  cmsv_wraw, /long, block, pointer, vartype, status=status, errmsg=errmsg
  if status EQ 0 then return

  arrtype = 0L
  if sz(0) GT 0   then arrtype = arrtype OR '14'x
  if vartype EQ 8 then arrtype = arrtype OR '20'x
  ;; Array type flags
  cmsv_wraw, /long, block, pointer, arrtype, status=status, errmsg=errmsg
  if status EQ 0 then return

  ;; System variable
  if keyword_set(system) then begin
      sysflags = lonarr(2)
      sysflags(0) = vartype
      sysflags(1) = arrtype OR '02'xl

      cmsv_wraw, /long, block, pointer, sysflags, $
        status=status, errmsg=errmsg
  endif

  if arrtype EQ 0 then return  ;; Scalar type

  ;; Complex array type
  if vartype EQ 8 then length = n_tags(data(0), /length)
  cmsv_warrdesc, block, pointer, sz, status=status, length=length, $
    errmsg=errmsg
  if status AND vartype EQ 8 then begin
      cmsv_wstructdesc, block, pointer, status=status, template=data, $
        errmsg=errmsg, nocatch=nocatch
  endif

  return
end
