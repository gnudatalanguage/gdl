;+
; NAME:
;   CMSV_WDATA
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Write SAVE-formatted data variable record to output block or file
;
; CALLING SEQUENCE:
;   CMSV_WDATA, BLOCK, POINTER, DATA, UNIT=UNIT, TEMPORARY=TEMPORARY, $
;               PTR_INDEX=PTR_INDEX, PTR_DATA=PTR_DATA, $
;               OFFSET=OFFSET, STATUS=STATUS, ERRMSG=ERRMSG
;   
; DESCRIPTION: 
;
;   CMSV_WDATA writes the data portion of an IDL SAVE variable record.
;   An IDL variable is stored in two components: the type descriptor
;   which describes the name, type, and dimensions of the variable;
;   and the data record, which contains the raw data of the variable.
;   This procedure writes the raw data to the output.  The initial
;   type descriptor portion of the record must have already been
;   writtenusing the CMSV_WVTYPE procedure.
;
;   Under normal circumstances a user will write variable or heap data
;   using the CMSV_WREC procedure.
;
;   CMSV_WDATA supports the following variable types:
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
;   The caller must specify in the DATA parameter, the data to be
;   written to output.  The variable passed as DATA must have the same
;   type and dimensions as passed to CMSV_WVTYPE.
;
;   Unlike most of the other output routines, this procedure is able
;   to send its output to a file rather than to the BLOCK buffer.  If
;   the UNIT keyword is specified then output is sent to that file
;   UNIT, after any pending BLOCK data is first sent.  Users should
;   note that after such operations, the BLOCK POINTER and OFFSET
;   parameters may be modified (ie reset to new values).
;
;   See CMSV_WREC for instructions on how to write heap data.
;
;   [ This code assumes the record header and type descriptor have
;   been written with CMSV_WREC and CMSV_WVTYPE. ]
;
; ==================================================================
;   Research Systems, Inc. has issued a separate license intended
;   to resolve any potential conflict between this software and the
;   IDL End User License Agreement. The text of that license
;   can be found in the file LICENSE.RSI, included with this
;   software library.
; ==================================================================
;
;
; BLOCK, POINTER, OFFSET
;
;   This procedure writes data to a byte array or a file.  If the UNIT
;   keyword is specified then file is sent to the specified unit
;   number rather than to the buffer BLOCK.  However, the intent is
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
;
; INPUTS:
;
;   BLOCK - a byte array, a cache of the SAVE file.  Users will
;           usually not access this array directly.  Users are advised
;           to clear BLOCK after calling POINT_LUN or writing the
;           block to disk.
;
;   POINTER - a long integer, a pointer to the next byte to be read
;             from BLOCK.  CMSVLIB routines will automatically
;             advance the pointer.
;
;   DATA - the data to be written, of any save-able data type.
;
; KEYWORDS:
;
;   TEMPORARY - if set, then the input DATA are discarded after being
;               written, as a memory economy provision.
;
;   PTR_INDEX - a heap index array for the data being written, if any
;               heap data records have been written.
;               Default: no pointers are written
;
;   PTR_DATA - an array of pointers, pointing to the heap values being
;              written.
;              Default: no pointers are written
;
;   UNIT - a file unit.  If specified then data are directed to the
;          file unit rather than to the buffer BLOCK.
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
; $Id: cmsv_wdata.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-

; ---------------- Convert from host to network order ------------------
pro cmsv_wconv, data

  ;; Inspired by IDL Astronomy Library routine HOST_TO_IEEE
  common cmsv_conv_common, lendian
  if n_elements(lendian) EQ 0 then begin
      ;; Little-endian?
      lendian = (long(['01'xb,'02'xb,'03'xb,'04'xb],0,1))(0) NE '01020304'xl
  endif

  sz = size(data)
  case sz(sz(0)+1) of 
      1:  return                    ;; Byte
      2:  byteorder, data, /HTONS   ;; Integer
      3:  byteorder, data, /HTONL   ;; Long
      4:  byteorder, data, /FTOXDR  ;; Float
      5:  byteorder, data, /DTOXDR  ;; Double
      6:  byteorder, data, /FTOXDR  ;; Complex
      9:  byteorder, data, /DTOXDR  ;; DComplex
      12: byteorder, data, /HTONS   ;; UInt
      13: byteorder, data, /HTONL   ;; ULong
      14: if lendian EQ 1 then byteorder, data, /L64SWAP ;; LONG64
      15: if lendian EQ 1 then byteorder, data, /L64SWAP ;; ULONG64
      ELSE: 
  endcase

end

pro cmsv_wdata, block, pointer, value, unit=unit, temporary=temp, $
                ptr_index=pi, ptr_data=pd, start=start, $
                status=status, errmsg=errmsg

  ;; VAR_DATA
  ;;   LONG - START_DATA TOKEN - value 7
  ;;   for bytes - consecutive bytes
  ;;   for (u)ints - upcast to type long 
  ;;   for (u)longs - consecutive longs
  ;;   for pointers - consecutive longs, indices into saved heap data
  ;;   for strings - consecutive STRING's
  ;;   for structs - compacted versions of above
  forward_function byte, long, ulong, float, double, ptr_new

  data = 0
  if n_elements(start) EQ 0 then start = 1
  if n_elements(pointer) EQ 0 then pointer = 0L

  if keyword_set(start) then begin
      cmsv_wraw, /long, block, pointer, 7L, unit=unit, $
        status=status, errmsg=errmsg
      if status EQ 0 then return
  endif
  sz = size(value)
  tp = sz(sz(0)+1)
  if (tp EQ 11) then begin
      status = 0
      errmsg = 'ERROR: CMSV_WDATA: cannot write object data'
      return
  endif
  nelt = sz(sz(0)+2)

  if (tp EQ 8) then begin   ;; Structure type
      tn = tag_names(value(0)) & nt = n_elements(tn)

      for i = 0L, nelt-1 do begin
          for j = 0L, nt-1 do begin
              cmsv_wdata, block, pointer, value(i).(j), start=0, $
                status=status, errmsg=errmsg
              if status EQ 0 then return
          endfor

          ;; Occasionally flush the data to disk
          if keyword_set(start) AND n_elements(unit) GT 0 then begin
              if (i EQ nelt-1) OR (pointer GT 65536L) then begin
                  writeu, unit(0), block(0:pointer-1)
                  pointer = 0L
                  block = 0 & dummy = temporary(block)
              endif
          endif
      endfor

      return
  endif

  if tp EQ 7 then begin     ;; String type
      cmsv_wraw, /string, block, pointer, value, /replen, $
        status=status, errmsg=errmsg
      if status EQ 0 then return

      if (n_elements(unit) GT 0 AND keyword_set(start) $
          AND pointer GT 0) then begin
          writeu, unit(0), block(0:pointer-1)
          pointer = 0L
          block = 0 & dummy = temporary(block)
      endif
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
      errmsg = 'ERROR: CMSV_WDATA: cannot write specified type ('+strtrim(tp,2)+')'
      return
  endif
  nelt1 = nelt*selts(tp<16)     ;; Account for complex type
  nb = nb*nelt1                 ;; Number of total bytes

  if keyword_set(temp) then data = temporary(value) $
  else                      data = value
  if sz(0) GT 0 then data = reform(data, /overwrite)
  
  ;; Convert from pointer type to LONG
  psz = size(pd)
  if (tp EQ 10) then begin
      odata = temporary(data)
      null = ptr_new()
      ;; Initialize as null pointers
      if sz(0) GT 0 then data = lonarr(nelt) else data = 0L

      ;; Fill in the pointers if we know about it
      if (n_elements(pi) GT 0) AND (n_elements(pi) EQ n_elements(pd)) $
        AND (psz(psz(0)+1) EQ 10) then begin
          for i = 0L, nelt-1 do if odata(i) NE null then begin
              wh = where(odata(i) EQ pd, ct)
              if ct GT 0 then begin
                  data(i) = abs(pi(wh(0)))
                  pi(wh(0)) = -pi(wh(0))
              endif
          endif
      endif
      odata = 0
  endif

  ;; Error handler returns zero and error condition
  on_ioerror, WRITE_ERROR
  if 0 then begin
      WRITE_ERROR:
      errmsg = 'ERROR: CMSV_WDATA: a write error occurred'
      return
  end

  ;; Special conversions
  if (tp EQ 2)  then data = long(temporary(data))
  if (tp EQ 12) then data = ulong(temporary(data))
  if (tp EQ 6)  then data = float(temporary(data), 0, nelt1)
  if (tp EQ 9)  then data = double(temporary(data), 0, nelt1)
  cmsv_wconv, data

  if n_elements(unit) GT 0 then begin
      ;; Write data to file directly
      if n_elements(pointer) GT 0 then begin
          writeu, unit(0), block(0:pointer-1)
          pointer = 0L
          block = 0 & dummy = temporary(block)
      endif
      writeu, unit(0), data
  endif else begin
      ;; Write data to byte buffer

      cmsv_wraw, block, pointer, byte(temporary(data), 0, nb), /byte, $
        status=status, errmsg=errmsg
  endelse

  on_ioerror, NULL

  status = 1
  return
end
