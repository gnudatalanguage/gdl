;+
; NAME:
;   CMSV_RRAW
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Read raw SAVE data from input block or file unit
;
; CALLING SEQUENCE:
;   DATA = CMSV_RRAW( BLOCK, POINTER, NELT, UNIT=UNIT, $
;                     STRING=STRING, LONG=LONG, BYTE=BYTE, TYPE=TYPE, $
;                     OFFSET=OFFSET, STATUS=STATUS, ERRMSG=ERRMSG )
;   
; DESCRIPTION: 
;
;   This function reads raw integer or string data from an IDL SAVE
;   file.  This is the lowest level reading function in the library,
;   intended for developers who are investigating new and existing
;   SAVE file formats.
;
;   The primary use of this function will be to read raw integer and
;   string data from the input.  By default, a single value is read as
;   a scalar; however a vector of values can be read using the NELT
;   parameter.  Special keywords are provided for the common data
;   types STRING, LONG and BYTE.  Other integer types can be read
;   using the TYPE keyword.  Users who want to read the data from an
;   IDL variable should use CMSV_RDATA, which can handle
;   multidimensional data, as well as floating point and structure
;   data.
;
;   A secondary use of this function is to buffer the data in BLOCK.
;   This will ensure that BLOCK contains enough data to convert NELT
;   integers without reading from disk.  CMSV_RRAW itself does not
;   convert any values so the return value should be ignored.
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
;   NELT - optional parameter specifying the number of values to read.
;          If unspecified, then a scalar value is read and returned.
;          If specified, then a vector of NELT values is read and
;          returned.
;
;
; KEYWORDS:
;
;   BUFFER - if set, CMSV_RRAW will ensure that BLOCK contains at
;            least NELT values without converting them.  The return
;            value should be ignored.  This keyword is ignored for
;            strings.
;
;   LONG - if set, the values are converted as LONG integers.
;
;   BYTE - if set, the values are converted as BYTEs.
;
;   STRING - if set, the values are converted as STRINGs.  Each string
;            may be of variable length.
;
;   TYPE - if none of the above keywords is set, then values of type
;          TYPE are read.  TYPE should be a string, one of 'BYTE',
;          'FIX', 'LONG', 'ULONG', 'LONG64', or 'ULONG64'.
;          If no type is specified then BYTEs are read.
;
;   UNIT - a file unit.  If a library routine reads to the end of
;          BLOCK, or if BLOCK is undefined, then this file UNIT will
;          be accessed for more data.  If undefined, then BLOCK must
;          contain the entire file in memory.
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
;   Added notification about RSI License, 13 May 2002, CM
;
; $Id: cmsv_rraw.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-

forward_function cmsv_rraw

pro cmsv_rbuf, block, pointer, nbytes, unit=unit0, errmsg=errmsg, status=status

  if n_elements(pointer) EQ 0 then pointer = 0L

  RESTART_READ:

  ;; If enough bytes are already available then there is no problem,
  ;; we simply return.  NOTE that, since we always try to read a large
  ;; chunk of data, most of the calls to this routine return right
  ;; here, and just insure that data is available.

  if pointer+nbytes LE n_elements(block) then begin
      status = 1
      return
  endif

  ;; Attempt to read from the file unit.  Make sure the unit is defined
  if n_elements(unit0) EQ 0 then begin
      errmsg = 'ERROR: CMSV_RRAW: end of block encountered ' + $
        'and UNIT is invalid)'
      status = 0
      return
  endif

  ;; Make sure the unit is open and readable.
  unit = floor(unit0(0))
  fs = fstat(unit)
  on_ioerror, READ_ERROR
  if fs.open EQ 0 OR fs.read EQ 0 then begin
      READ_ERROR:
      errmsg = string(fs.name, fs.unit, $
                      format=('("ERROR: CMSV_RRAW: ",A0," (unit ",I0,'+$
                              '") is not open and readable")') )
      status = 0
      return
  endif

  ;; Read a larger block, but not more than the existing file size
  bufsize = (pointer+nbytes-n_elements(block)) > 1024L
  bufsize = bufsize < (fs.size - fs.cur_ptr)
  if bufsize LE 0 then begin
      errmsg = 'ERROR: CMSV_RRAW: end of file encountered'
      status = 0
      return
  endif

  ;; Read the unformatted raw byte data
  buf = bytarr(bufsize)
  readu, unit, buf
  if n_elements(block) GT 0 then block = [temporary(block), temporary(buf)] $
  else block = [temporary(buf)]

  ;; Now that the block has been read, restart the read operation
  goto, RESTART_READ
end

function cmsv_rraw, block, pointer, nelt0, unit=unit, buffer=buf, $
                    string=string, byte=byte, long=long, type=type0, $
                    status=status, errmsg=errmsg, offset=offset

  status = 0
  ;; Default processing
  if n_elements(pointer) EQ 0 then pointer = 0L
  if n_elements(nelt0) EQ 0 then nelt = 1L else nelt = floor(nelt0(0))
  if nelt LE 0 then return, 0

  if keyword_set(string) then begin
      ;; STRING_DATA
      ;;   LONG - STRLEN - string length in characters
      ;;   BYTExSTRLEN - string characters, padded to next four-byte boundary
      
      ;; Default processing
      strings = 0 & dummy = temporary(strings)

      ;; Read as many strings as required
      for i = 0L, nelt-1 do begin
          st = ''
          ;; Read string length
          namelen = cmsv_rraw(/long, block, pointer, unit=unit, $
                              status=status, errmsg=errmsg)
          if status EQ 0 then return, ''
          
          ;; If length is zero, then it's an empty string, and no
          ;; string data follows
          
          if namelen GT 0 then begin
              nbytes = long(floor((namelen+3)/4)*4) ;; pad to next long
              
              cmsv_rbuf, block, pointer, nbytes, unit=unit, $
                status=status, errmsg=errmsg
              if status EQ 0 then return, ''
              st = string(block(pointer:pointer+nbytes-1))
              
              pointer = pointer + nbytes            ;; Advance pointer
          endif

          if n_elements(strings) EQ 0 then strings = st $
          else                             strings = [strings, st]
      endfor

      DONE_STRINGS:
      if n_elements(strings) EQ 0 then strings = ''
      if n_elements(nelt0) EQ 0 then strings = strings(0)

      status = 1
      return, strings
  end


  ;; INTEGER DATA 

  if keyword_set(byte) then begin
      nbytes = 1L
  endif else if keyword_set(long) then begin
      nbytes = 4L 
  endif else begin
      if n_elements(type0) EQ 0 then type = 'BYTE' $
      else                           type = strupcase(strtrim(type0(0),2))

      case type of 
          'BYTE'   : nbytes = 1L
          'FIX'    : nbytes = 2L
          'LONG'   : nbytes = 4L
          'ULONG'  : nbytes = 4L
          'LONG64' : nbytes = 8L
          'ULONG64': nbytes = 8L
          else: begin
              errmsg = 'ERROR: CMSV_RRAW: undefined integer type '+type
              status = 0
              return, 0
          end
      end
  endelse
  
  ntotbytes = nelt*nbytes
  if pointer+ntotbytes GT n_elements(block) then begin
      ;; Read the raw byte data from buffer
      cmsv_rbuf, block, pointer, ntotbytes, unit=unit, errmsg=errmsg, $
        status=status
      if status EQ 0 then return, 0
  endif

  if keyword_set(buf) then begin
      status = 1
      return, 0
  endif

  ;; Convert byte data to long values
  if n_elements(type0) EQ 0 then $
    value = long(block(pointer:pointer+ntotbytes-1),0,nelt) $
  else $
    value = call_function(type, block(pointer:pointer+ntotbytes-1),0,nelt)
  if n_elements(nelt0) EQ 0 then value = value(0)

  ;; Advance pointer
  pointer = pointer + ntotbytes
  status = 1

  common cmsv_conv_common, lendian
  if n_elements(lendian) EQ 0 then begin
      ;; Little-endian?
      lendian = (long(['01'xb,'02'xb,'03'xb,'04'xb],0,1))(0) NE '01020304'xl
  endif

  ;; Convert values to correct endianness
  case nbytes of 
      1: dummy = 1                                    ; byte
      2: byteorder, value, /NTOHS                     ; int 
      4: byteorder, value, /NTOHL                     ; long
      8: if lendian then byteorder, value, /L64SWAP   ; long64
      else:
  endcase

  return, value
end
