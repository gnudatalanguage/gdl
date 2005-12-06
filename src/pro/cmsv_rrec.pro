;+
; NAME:
;   CMSV_RREC
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Read SAVE-formatted record header from input block or file unit
;
; CALLING SEQUENCE:
;   CMSV_RREC, BLOCK, POINTER, DATA, UNIT=UNIT, $
;        BLOCK_TYPE=BLOCK_TYPE, BLOCK_NAME=BLOCK_NAME, NEXT_BLOCK=NEXT_BLOCK, $
;        INITIALIZE=INITIALIZE, FULL=FULL, PROMOTE64=PROMOTE64, $
;        OFFSET=OFFSET, STATUS=STATUS, ERRMSG=ERRMSG
;   
; DESCRIPTION: 
;
;   This procedure reads the header of an IDL SAVE record.  The header
;   consists of four bytes at the beginning of each record which
;   indentifies the type and size of the record.  This procedure also
;   additionally reads many full records as noted below.
;
;   Users can determine the block type by examining the values
;   returned in the BLOCK_TYPE and BLOCK_NAME keywords.  The following
;   values are supported.  Some record types contain additional data.
;   CMSV_RREC reads some of these record types automatically (those
;   entries below marked with [this procedure]).  Other records do not
;   contain any additional data at all (those entries marked with an
;   [empty]), and thus require no further processing.
;
;    BLOCK_TYPE      BLOCK_TYPE         READ RECORD DATA WITH...
;
;       0         = 'START_MARKER'      [empty]
;       1         = 'COMMON_BLOCK'      [this procedure]
;       2         = 'VARIABLE'          CMSV_RVTYPE / CMSV_RDATA
;       3         = 'SYSTEM_VARIABLE'   CMSV_RVTYPE / CMSV_RDATA
;       6         = 'END_MARKER'        [empty]
;       10        = 'TIMESTAMP'         [this procedure]
;       12        = 'COMPILED'          no published procedure
;       13        = 'IDENTIFICATION'    [this procedure]
;       14        = 'VERSION'           [this procedure]
;       15        = 'HEAP_INDEX'        [this procedure]
;       16        = 'HEAP_DATA'         CMSV_RVTYPE
;       17        = 'PROMOTE64'         [empty]
;       19        = 'NOTICE'            [this procedure]
;
;   For records that contain variable data, the external procedures
;   CMSV_RVTYPE and CMSV_RDATA must be used, as noted above.
;   Otherwise CMSV_RREC will read and convert the appropriate data
;   automatically and return it in the DATA positional parameter.
;
;   The offset of the next record is returned in the NEXT_BLOCK
;   keyword.  For file input, the command, "POINT_LUN, UNIT,
;   NEXT_BLOCK" will position the file pointer to the next block.
;
;   Users should be aware that the SAVE files produced by IDL version
;   5.4 appear to have a different header format.  The new header size
;   is five bytes, and is incompatible with the older format.  In
;   order to activate the longer header size, the PROMOTE64 keyword
;   must be set.
;
;   By default the entire record is not read from the file at once.
;   Users that wish to operate on the entire record immediately should
;   set the FULL keyword.
;
;   After issuing a POINT_LUN the block cache in BLOCK must be reset
;   using the /INITIALIZE keyword.
;
; ==================================================================
;   Research Systems, Inc. has issued a separate license intended
;   to resolve any potential conflict between this software and the
;   IDL End User License Agreement. The text of that license
;   can be found in the file LICENSE.RSI, included with this
;   software library.
; ==================================================================
;
; SPECIFIC RECORD TYPES
;
;   CMSV_RREC reads certain specific record types automatically and
;   returns the data in the positional parameter data.  Users should
;   pass a named variable in this parameter to retrieve the return
;   value.
;
;   When a record of type 'VERSION' (14) is encountered, it is read,
;   and returned as a structure. The returned data are of the form:
;
;         { FORMAT_VERSION: 0L, $   ; Format version number of file
;           ARCH: '', $             ; !VERSION.ARCH of creating host
;           OS: '', $               ; !VERSION.OS of creating host
;           RELEASE: '' }           ; !VERSION.RELEASE of creating host
;
;   When a record of type 'TIMESTAMP' (10) is encountered, it is read,
;   and returned as a structure. The returned data are of the form:
;
;          { SAVE_DATE: '', $  ; Date the save file was created
;            SAVE_USER: '', $  ; User name who  created file
;            SAVE_HOST: '' }   ; Host name that created file
;
;   Save files created by IDL version 4 do not contain a timestamp
;   record.
;
;   When a record of type 'IDENTIFICATION' (13) is encountered, it is
;   read, and returned as a structure.  The returned data are of the
;   form:
;
;          { AUTHOR: '', $  ; Author of SAVE file
;            TITLE:  '', $  ; Title of SAVE file
;            IDCODE: '' }   ; Identifying code for SAVE file
;
;   It appears that this record is not used in IDL version 5 or later.
;
;   When a record of type 'COMMON_BLOCK' (1) is encountered, it is
;   read and returned.  A common block descriptor consists of an array
;   of strings whose first element is the common block name, and whose
;   remaining elements are the common block variable names.  No
;   variable data are stored with the common block definition.
;
;   When a record of type 'HEAP_INDEX' (15) is encountered, it is read
;   and returned in DATA.  The heap index specifies a list of which
;   heap variables are stored in the current save file.  These indices
;   are simply numbers which identify each heap variable (i.e.,
;   "<PtrHeapVar2>" would have an index of 2).  Users should note that
;   the heap index will not necessarily be sequentially increasing,
;   and may have gaps.
;
;   When a record of type 'NOTICE' (19) is encountered, it is read and
;   returned in DATA.  It is a structure with one field: {TEXT: ''},
;   where TEXT is the text content of the notice.
;
;   Users should consult CMSV_RDATA for instructions on how to read
;   heap data.
;   
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
;
; KEYWORDS:
;
;   UNIT - a file unit.  If a library routine reads to the end of
;          BLOCK, or if BLOCK is undefined, then this file UNIT will
;          be accessed for more data.  If undefined, then BLOCK must
;          contain the entire file in memory.
;
;   OFFSET - the file offset of byte zero of BLOCK.  Default: 0
;            (OFFSET is used by this routine)
;
;   BLOCK_TYPE - upon return, the numeric record type, as described
;                above.
;
;   BLOCK_NAME - upon return, a scalar string specifying the record
;                type, as specified above.
;
;   NEXT_BLOCK - upon return, file offset of the next record in the
;                file.
;
;   INITIALIZE - if set, then BLOCK and POINTER are initialized to a
;                pristine state.  All data in these two variables is
;                lost before reading the next record.
;
;   FULL - if set, then the entire record will be read into BLOCK.
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
;   Fix typo for RHEAP call, CM, 21 Apr 2001
;   Added notification about RSI License, 13 May 2002, CM
;   Added NOTICE record type, 09 Jun 2003, CM
;
; $Id: cmsv_rrec.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, 2003, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-

; ----------------- Read common block descriptor ----------------------

pro cmsv_rcomm, block, pointer, names, unit=unit, offset=offset, $
                status=status, errmsg=errmsg

  status = 0
  if n_elements(pointer) EQ 0 then pointer = 0L
  names = 0 & dummy = temporary(names)
  
  ncommon = cmsv_rraw(/long, block, pointer, unit=unit, $
                      status=status, errmsg=errmsg)
  if status EQ 0 then return
  if ncommon LE 0 then begin
      status = 0
      errmsg = 'ERROR: CMSV_RREC: invalid common record block'
      return
  endif

  names = cmsv_rraw(/string, block, pointer, ncommon+1, unit=unit, $
                    status=status, errmsg=errmsg)
  return
end


; ----------------- Read heap index ---------------------------------

pro cmsv_rheap, block, pointer, index, unit=unit, offset=offset, $
                status=status, errmsg=errmsg

  ;; HEAP_INDEX
  ;;   LONG - N_HEAP - number of heap values
  ;;   LONGxN_HEAP - heap indices

  if n_elements(pointer) EQ 0 then pointer = 0L
  index = 0 & dummy = temporary(index)

  n_heap = cmsv_rraw(/long, block, pointer, unit=unit, $
                     status=status, errmsg=errmsg)
  if status then $
    index = cmsv_rraw(/long, block, pointer, n_heap, unit=unit, $
                      status=status, errmsg=errmsg)

  return
end

; ------------------------ Read time stamp record -----------------------

pro cmsv_rstamp, block, pointer, tstamp, unit=unit, offset=offset, $
                 status=status, errmsg=errmsg

  ;; TIMESTAMP
  ;;   BYTEx400 - empty (?) legacy area
  ;;   STRING - save date (as a string)
  ;;   STRING - user name
  ;;   STRING - hostname
  status = 0
  if n_elements(pointer) EQ 0 then pointer = 0L
  pointer = pointer + '400'xl
  tstamp = 0 & dummy = temporary(tstamp)

  strings = cmsv_rraw(/string, block, pointer, 3, unit=unit, $
                      status=status, errmsg=errmsg)
  if status EQ 0 then return

  tstamp = {save_date: strings(0), save_user: strings(1), save_host:strings(2)}
  return
end


; ---------------------- Read Version Info ---------------------------

pro cmsv_rversion, block, pointer, vers, unit=unit, offset=offset, $
                   status=status, errmsg=errmsg
  ;; VERSION_STAMP
  ;;   LONG - Major version number
  ;;   STRING_DATA - Host architecture ( = !version.arch )
  ;;   STRING_DATA - Host OS ( = !version.os )
  ;;   STRING_DATA - IDL release ( = !version.release )

  if n_elements(pointer) EQ 0 then pointer = 0L
  major_release = 5
  vers = 0 & dummy = temporary(vers)

  arch = '' & os = '' & release = ''
  major_release = cmsv_rraw(/long, block, pointer, unit=unit, $
                            status=status, errmsg=errmsg)
  if status EQ 0 then return

  strings = cmsv_rraw(/string, block, pointer, 3L, status=status, $
                      unit=unit, errmsg=errmsg)
  if status EQ 0 then return
  status = 1
  
  vers = {format_version: major_release, arch: strings(0), $
          os: strings(1), release: strings(2)}
  return
end

; --------------------------- Read Identification --------------------

pro cmsv_rident, block, pointer, ident, unit=unit, offset=offset, $
                 status=status, errmsg=errmsg

  ;; IDENT
  ;;   STRING - author
  ;;   STRING - title
  ;;   STRING - idcode

  strings = cmsv_rraw(/string, block, pointer, 3, unit=unit, $
                      status=status, errmsg=errmsg)
  if status EQ 0 then return

  ident = {author: strings(0), title: strings(1), idcode:strings(2)}
  return
end

; --------------------------- Read Notice --------------------

pro cmsv_rnotice, block, pointer, notice, unit=unit, offset=offset, $
                  status=status, errmsg=errmsg

  ;; NOTICE
  ;;   STRING - notice text

  string = cmsv_rraw(/string, block, pointer, 1, unit=unit, $
                      status=status, errmsg=errmsg)
  if status EQ 0 then return

  notice = {text: string}
  return
end


; ---------------------------- Main Read Routine ---------------------

pro cmsv_rrec, block, pointer, data, unit=unit, offset=offset, $
               status=status, errmsg=errmsg, compressed=compressed, $
               block_type=blocktype, block_name=blockname, next_block=np, $
               initialize=init, full=full, promote64=prom, $
               qblocknames=qblock, autopromote64=autoprom

  common cmsave_block_names, block_ntypenames, block_typenames
  if n_elements(block_ntypenames) EQ 0 then begin
      block_ntypenames    = 20
      block_typenames     = strarr(block_ntypenames+1)+'UNKNOWN'
      block_typenames(0)  = 'START_MARKER'
      block_typenames(1)  = 'COMMON_BLOCK'
      block_typenames(2)  = 'VARIABLE'
      block_typenames(3)  = 'SYSTEM_VARIABLE'
      block_typenames(6)  = 'END_MARKER'
      block_typenames(10) = 'TIMESTAMP'
      block_typenames(12) = 'COMPILED'
      block_typenames(13) = 'IDENTIFICATION'
      block_typenames(14) = 'VERSION'
      block_typenames(15) = 'HEAP_INDEX'
      block_typenames(16) = 'HEAP_DATA'
      block_typenames(17) = 'PROMOTE64'
      block_typenames(19) = 'NOTICE'
  endif

  if keyword_set(qblock) then begin
      data = block_typenames
      return
  end

  status = 0
  errmsg = ''

  if n_elements(pointer) EQ 0 then pointer = 0L
  pointer = floor(pointer(0))
  if keyword_set(init) then begin
      block = 0 & dummy = temporary(block)
      pointer = 0L
  endif

  pointer0 = pointer
  nlongs = 4L
  rechead = cmsv_rraw(/long, block, pointer, nlongs, unit=unit(0), $
                      status=status, errmsg=errmsg)
  if status EQ 0 then return
  
  blocktype = rechead(0)
  blockname = block_typenames(blocktype < block_ntypenames)

  np = rechead(1)
  doprom = 0
  if keyword_set(prom) then $
    doprom = 1 $
  else if (keyword_set(autoprom) AND $
           (rechead(1) EQ 0 AND rechead(2) NE 0)) then $
    doprom = 1

  if keyword_set(doprom) then begin
      ;; If file offset is to be promoted to 64-bit then we compute it
      ;; here.  The additional logic is to preserve 32-bit offsets in
      ;; most cases, and promote only if absolutely needed.  Signal an
      ;; error in earlier versions of IDL that don't support 64-bit
      ;; numbers.
      np = rechead(2) + rechead(1)
      if rechead(1) NE 0 AND rechead(2) NE 0 then begin
          if double(!version.release) LT 5.2D then begin
              errmsg = ('ERROR: CMSV_RREC: file contains a 64-bit file '+ $
                        'offset which is unstorable by this version of IDL')
              status = 0
              return
          endif
          np = cmsv_rraw(/long, block, pointer-3L*4L, type='ULONG64', $
                         status=status, errmsg=errmsg)
          if status EQ 0 then return
      endif

      ;; A 64-bit header has an extra long in it.  Read that now.
      ;; It should be zero.
      val0 = cmsv_rraw(/long, block, pointer, status=status, errmsg=errmsg)
      if status EQ 0 then return
      if val0 NE 0 then begin
          errmsg = 'ERROR: CMSV_RREC: inconsistent 64-bit header'
          status = 0
          return
      endif

  endif

  ;; Activate 64-bit promotion.  Blocks after this current one will
  ;; have 64-bit file offsets rather than 32-bit ones.
  if blocktype EQ 17 then prom = 1

  if keyword_set(full) then begin
      if n_elements(offset) EQ 0 then offset = 0L
      nbytes = np - (pointer0 + floor(offset(0)))
      if nbytes GT 0 then begin
          dummy = cmsv_rraw(block, 0L, nbytes, /byte, /buffer, unit=unit, $
                            status=status, errmsg=errmsg)
          if status EQ 0 then return
          block = block(0:nbytes-1)
      endif
  endif

  ;; Can't read compressed data for the moment
  if keyword_set(compressed) then begin
      status = 1
      return
  endif

  case blockname of 
      'COMMON_BLOCK': cmsv_rcomm, block, pointer, data, unit=unit, $
        status=status, errmsg=errmsg
      'HEAP_INDEX': cmsv_rheap, block, pointer, data, unit=unit, $
        status=status, errmsg=errmsg
      'VERSION': cmsv_rversion, block, pointer, data, unit=unit, $
        status=status, errmsg=errmsg
      'TIMESTAMP': cmsv_rstamp, block, pointer, data, unit=unit, $
        status=status, errmsg=errmsg
      'IDENTIFICATION': cmsv_rident, block, pointer, data, unit=unit, $
        status=status, errmsg=errmsg
      'NOTICE': cmsv_rnotice, block, pointer, data, unit=unit, $
        status=status, errmsg=errmsg
      ELSE: status = 1
  end

  return
end
