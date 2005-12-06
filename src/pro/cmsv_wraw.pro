;+
; NAME:
;   CMSV_WRAW
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Write raw SAVE data to output block
;
; CALLING SEQUENCE:
;   CMSV_WRAW, BLOCK, POINTER, DATA, UNIT=UNIT, $
;              STRING=STRING, LONG=LONG, BYTE=BYTE, TYPE=TYPE, $
;              OFFSET=OFFSET, STATUS=STATUS, ERRMSG=ERRMSG
;   
; DESCRIPTION: 
;
;   This procedure writes raw integer or string data to an IDL SAVE
;   block in memory.  This is the lowest level writing function in the
;   library, intended for developers who are investigating new and
;   existing SAVE file formats.
;
;   The data to be written is specified by the DATA parameter.  The
;   data must be of type BYTE, LONG or STRING, and the type is
;   determined automatically from the data itself.  [ The mnemonic
;   STRING LONG and BYTE keywords are accepted for programming clarity
;   but ignored. ]
;
;   This procedure accepts but currently ignores the UNIT keyword.  It
;   is the caller's responsibility to write the BLOCK data to disk
;   when appropriate.
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
;   DATA - the data to be written.  Must of type STRING, BYTE or LONG.
;
;
; KEYWORDS:
;
;   LONG - ignored (to be used for clarity)
;   BYTE - ignored (to be used for clarity)
;   STRING - ignored (to be used for clarity)
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
; $Id: cmsv_wraw.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-
pro cmsv_enlarge, block, pointer, nbytes, status=status, errmsg=errmsg

  status = 1

  diff = pointer+nbytes - n_elements(block)
  if diff GT 0 then begin
      if n_elements(block) GT 0 then $
        block = [temporary(block), bytarr(diff + 1024L)] $
      else $
        block = bytarr(diff + 1024L)
  endif

  return
end

pro cmsv_wraw, block, pointer, value0, unit=unit, replen=replen, $
               byte=byte, long=long, string=string, $
               status=status, errmsg=errmsg

  if n_elements(pointer) EQ 0 then pointer = 0L
  sz = size(value0)
  tp = sz(sz(0)+1)
  
  if tp EQ 7 then begin   ;; STRING DATA
      ;; STRING_DATA
      ;;   LONG - STRLEN - string length in characters
      ;;   BYTExSTRLEN - string characters, padded to next four-byte boundary

      if n_elements(pointer) EQ 0 then pointer = 0L

      ;; Compute number of bytes required to store this data
      replen1 = keyword_set(replen)
      len = strlen(value0)
      llen = n_elements(value0)*4*(replen1 + 1)

      stride = 4 + 4*replen1
      ntotbytes = long( total(floor((len+3)/4)*4L) + llen )
      if pointer+ntotbytes GT n_elements(block) then $
        cmsv_enlarge, block, pointer, ntotbytes

      for i = 0L, n_elements(value0)-1 do begin
          name = value0(i)

          ;; Add string length
          hlen = len(i)
          byteorder, hlen, /HTONL
          block(pointer) = byte(hlen, 0, 4)
          if replen1 then block(pointer+4) = byte(hlen, 0, 4)
          pointer = pointer + stride

          if len(i) GT 0 then begin
              ;; Add string contents
              nbytes = floor((len(i)+3)/4)*4L  ;; Round up to next 4-bytes
              block(pointer) = byte(name)
              pointer = pointer + nbytes
          endif
      endfor

      status = 1
      return
  endif

  status = 0

  nelt = n_elements(value0)
  sz = size(value0)
  tp = sz(sz(0)+1)
  value = value0

  if tp EQ 1 then begin
      nbytes = 1L    ;; BYTE DATA
  endif else if tp EQ 3 then begin
      nbytes = 4L    ;; LONG DATA
  endif else begin
      status = 0
      errmsg = 'ERROR: CMSV_WRAW: data must byte BYTE, LONG or STRING'
      return
  endelse
      
  ntotbytes = nbytes * nelt

  if pointer+ntotbytes GT n_elements(block) then $
    cmsv_enlarge, block, pointer, ntotbytes

  common cmsv_conv_common, lendian
  if n_elements(lendian) EQ 0 then begin
      ;; Little-endian?
      lendian = (long(['01'xb,'02'xb,'03'xb,'04'xb],0,1))(0) NE '01020304'xl
  endif

  ;; Convert values to net order from host endianness
  case nbytes of 
;     1: dummy = 1                                    ; byte
;     2: byteorder, value, /HTONS                     ; int 
      4: byteorder, value, /HTONL                     ; long
;     8: if lendian then byteorder, value, /L64SWAP   ; long64
      else:
  endcase

  block(pointer) = byte(temporary(value), 0, ntotbytes)

  pointer = pointer + ntotbytes
  status = 1
  return
end
