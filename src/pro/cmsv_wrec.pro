;+
; NAME:
;   CMSV_WREC
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Write SAVE-formatted record header to output block or file
;
; CALLING SEQUENCE:
;   CMSV_WDATA, BLOCK, POINTER, DATA, IDENT, UNIT=UNIT, $
;               INITIALIZE=INITIALIZE, NO_DATA=NO_DATA, $
;               NO_TYPE=NO_TYPE, COMPATIBILITY=COMPAT, $
;               BLOCK_TYPE=BLOCK_TYPE, BLOCK_NAME=BLOCK_NAME,
;               NEXT_BLOCK=NEXT_BLOCK, $
;               [ ... EXTRA KEYWORDS ... ]
;               OFFSET=OFFSET, STATUS=STATUS, ERRMSG=ERRMSG
;   
; DESCRIPTION: 
;
;   This procedure writes most types of IDL SAVE record, including the
;   header and contents.  The header consists of four bytes at the
;   beginning of each record which indentifies the type and size of
;   the record.  This procedure also writes the contents of certain
;   records, as noted below.
;
;   Users can specify the block type by passing the BLOCK_TYPE or
;   BLOCK_NAME keywords.  The values listed in the following table are
;   supported.  CMSV_WREC writes the contents of essentially all
;   record types as well.  Some records do not contain any contents at
;   all (those entries marked with an [empty]) and thus require no
;   further processing.
;
;    BLOCK_TYPE      BLOCK_TYPE         WRITE RECORD CONTENTS WITH...
;
;       0         = 'START_MARKER'      [empty]
;       1         = 'COMMON_BLOCK'      [this procedure]
;       2         = 'VARIABLE'          [this procedure]
;       3         = 'SYSTEM_VARIABLE'   [this procedure]
;       6         = 'END_MARKER'        [empty]
;       10        = 'TIMESTAMP'         [this procedure]
;       12        = 'COMPILED'          no published procedure
;       13        = 'IDENTIFICATION'    [this procedure]
;       14        = 'VERSION'           [this procedure]
;       15        = 'HEAP_INDEX'        [this procedure]
;       16        = 'HEAP_DATA'         [this procedure]
;       17        = 'PROMOTE64'         [empty]
;       19        = 'NOTICE'            [this procedure]
;
;   For records that contain variable data, the external procedures
;   CMSV_WVTYPE and/or CMSV_WDATA may be used, however it is not
;   recommended, since the record header must finally be re-written by
;   the user.  Users can write the entire record with this procedure.
;
;   After issuing a POINT_LUN, or after writing the BLOCK to disk, the
;   block cache in BLOCK must be reset using the /INITIALIZE keyword.
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
;   CMSV_WREC reads certain specific record types automatically based
;   on data passed in the DATA parameter.  
;
;   Records of type 'VARIABLE' (2), 'SYSTEM_VARIABLE' (3) and
;   'HEAP_DATA' (16) require both the DATA and IDENT parameters.  For
;   the first two record types, the IDENT parameter is the name of the
;   variable, as a scalar string.  Variable names should be valid IDL
;   variable names, uppercase, and have no embedded spaces.  For the
;   'HEAP_DATA' record type, the IDENT parameter is the heap index
;   value, as described below.  The DATA itself can be any supported
;   IDL variable type (as described in CMSV_WVTYPE).
;
;   For records that accept data in the form of a structure, as listed
;   below, the listed structure tag entries are optional.  If the user
;   does not provide a value, then a suitable default will be computed
;   by this procedure (listed in parentheses).
;
;   A record of type 'VERSION' (14) has the following structure:
;
;         { FORMAT_VERSION: 0L, $   ; Format version number of file (5)
;           ARCH: '', $             ; !VERSION.ARCH of creating host
;           OS: '', $               ; !VERSION.OS of creating host
;           RELEASE: '' }           ; !VERSION.RELEASE of creating host
;
;   A record of type 'TIMESTAMP' (10) has the following structure:
;
;          { SAVE_DATE: '', $  ; Date the save file was created (SYSTIME(0))
;            SAVE_USER: '', $  ; User name who  created file ('UNKNOWN')
;            SAVE_HOST: '' }   ; Host name that created file ('UNKNOWN')
;
;   Save files created by IDL version 4 do not contain a timestamp
;   record.  Under Unix this procedure will attempt to discover the
;   user and host names automatically.
;
;   A record of type 'IDENTIFICATION' (13) has the following
;   structure:
;
;          { AUTHOR: '', $  ; Author of SAVE file ('')
;            TITLE:  '', $  ; Title of SAVE file  ('')
;            IDCODE: '' }   ; Identifying code for SAVE file ('')
;
;   It appears that this record is not used in IDL version 5 or later.
;
;   A record of type 'COMMON_BLOCK' (1) defines a named common block
;   and its variables.  A common block descriptor consists of an array
;   of strings whose first element is the common block name, and whose
;   remaining elements are the common block variable names.  Thus, a
;   common block descriptor must have at least two elements.  No
;   variable data are stored with the common block definition.
;
;   When a record of type 'NOTICE' (19) defines a notice to be
;   included in the save file.  It is a structure with one field:
;   {TEXT: ''}, where TEXT is the text content of the notice.
;
;   A record of type 'HEAP_INDEX' (15) defines the heap index in a
;   SAVE file.  The heap index specifies a list of which heap
;   variables are stored in the current save file.  These indices are
;   simply numbers which identify each heap variable (i.e.,
;   "<PtrHeapVar2>" would have an index of 2).  The heap index can use
;   any numbers to identify the heap data; however it is required that
;   all index entries have corresponding heap data values.
;
; WRITING HEAP DATA
;
;   If your data contains heap data and/or pointers, then users must
;   take special care in writing their data.  Writing heap data is
;   actually more straightforward than reading it.  There are several
;   steps involved which can be summarized as followed: (1) take
;   inventory of HEAP data; (2) write HEAP_INDEX record; (3) write one
;   HEAP_DATA record for each heap variable; and (4) write any other
;   variables using the heap index.
;
;   (1) Take inventory of heap data.  Before writing any data to the
;       SAVE file, use the CMSV_PTRSUM procedure to discover all
;       pointer variables in the data set, like so:
;
;          cmsv_ptrsum, var, ptrlist
;
;       PTRLIST contains an array of any heap variables pointed to by
;       VAR (including structures or pointed-to variables).  If
;       multiple variables are to be written, then the inventory must
;       contain the union of all heap variables.
;
;   (2) Write a HEAP_INDEX record.  The heap index is an array of long
;       integers which identify the heap variables.  In principle it
;       doesn't matter which integers are used, however there must be
;       a one-to-one correspondence between the entries in the heap
;       index and the heap identifiers used in the next step.  In this
;       example a simple LINDGEN is used:
;
;           index = lindgen(n_elements(ptrlist))
;           cmsv_wrec, block, pointer, index, block_name='HEAP_INDEX', $
;             offset=offset
;
;   (3) Write one HEAP_DATA record for each heap variable.  Issue one
;       CMSV_WREC call for each entry in PTRLIST, as follows for the
;       ith heap variable:
;
;           cmsv_wrec, block, pointer, ptrlist(i), block_name='HEAP_DATA', $
;             ptr_index=index, ptr_data=ptrlist, offset=offset
;
;       Note that the PTR_INDEX and PTR_DATA keywords are required
;       because heap data may itself contain pointers.  The PTR_INDEX
;       and PTR_DATA keywords enable the CMSV_WREC procedure to write
;       appropriate descriptors when it encounters pointers.
;
;   (4) Write remaining data.  For the ith variable, use:
;
;          cmsv_wrec, block, pointer, var(i), name(i), block_name='VARIABLE',$
;             ptr_index=index, ptr_data=ptrlist, offset=offset
;
;       As above, using the PTR_INDEX and PTR_DATA keywords will allow
;       the CMSV_WREC procedure to write the appropriate data.
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
;   DATA - the record contents to be written, as describe above.
;
;   IDENT - for record types 'VARIABLE' (2) and 'SYSTEM_VARIABLE' (3),
;           the name of the variable as a scalar string.  For record
;           type 'HEAP_DATA' (16), the heap index identifier as a
;           scalar long integer.
;
; KEYWORDS:
;
;   BLOCK_NAME - a scalar string specifying the record type, as
;                described above.  The BLOCK_TYPE keyword takes
;                precedence over BLOCK_NAME.
;
;   BLOCK_TYPE - a scalar integer specifying the record type, as
;                described above.
;
;   NEXT_BLOCK - if specified, the file offset of the next record
;                location.
;                Default: the offset will be computed automatically.
;
;   INITIALIZE - if the keyword is set, then the BLOCK is emptied and
;                the POINTER is reset before any new data is written.
;
;   NO_TYPE - if set, no type descriptor or data are written for
;             variable records.
;
;   NO_DATA - if set, no data are written for variable records.
;
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
;   COMPATIBILITY - a string, which describes the format to be used in
;          the output file.  Possible values are:
;
;                  'IDL4' - format of IDL version 4;
;                  'IDL5' - format of IDL versions 5.0-5.3;
;                  'IDL6' - not supported yet, for versions 5.4-above;
;                  'RIVAL1' - same as 'IDL5', plus a directory entry is
;                            written to the file.
;           Note that files written in IDL5 format may still be
;           readable by IDL v.4.
;           Default: 'IDL5'
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
;   Added NOTICE record type, 09 Jun 2003, CM
;
; $Id: cmsv_wrec.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, 2003, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-

pro cmsv_wcomm, block, pointer, names, $
                status=status, errmsg=errmsg

  status = 0
  sz = size(names)
  if sz(sz(0)+1) NE 7 then begin
      errmsg = 'ERROR: CMSV_WCOMM: DATA must contain an array of strings'
      return
  endif
  ncommon = n_elements(names)
  if ncommon LT 2 then begin
      errmsg = 'ERROR: CMSV_WCOMM: DATA must contain at least two elements'
      return
  endif

  cmsv_wraw, /long, block, pointer, ncommon, $
    status=status, errmsg=errmsg
  if status EQ 0 then return

  cmsv_wraw, /string, block, pointer, names, $
    status=status, errmsg=errmsg

  return
end

pro cmsv_wheap, block, pointer, indices, $
                status=status, errmsg=errmsg

  ;; HEAP_INDEX
  ;;   LONG - N_HEAP - number of heap values
  ;;   LONGxN_HEAP - heap indices

  n_heap = n_elements(indices)
  if n_heap LE 0 then return

  cmsv_wraw, /long, block, pointer, [n_heap, indices], $
    status=status, errmsg=errmsg

  return
end

pro cmsv_wstamp, block, pointer, offset=offset, $
                 status=status, errmsg=errmsg, $
                 save_date=save_date, save_user=save_user, $
                 save_host=save_host, _extra=extra

  common cmsv_wstamp_state, username, hostname

  status = 1

  ;; Retrieve the username -- we need some Windows help here!
  if n_elements(save_user) EQ 0 then begin
      if n_elements(username) NE 1 then begin
          xusername = (getenv('USER'))(0)
          if xusername EQ '' then begin
              xusername = (getenv('LOGNAME'))(0)
              if xusername EQ '' then begin
                  if !version.os_family EQ 'unix' then $
                    spawn, 'whoami', xusername
                  if xusername(0) EQ '' then $
                    xusername = 'UNKNOWN'
              endif
          endif
          username = xusername(0)
      endif
      save_user = username
  endif

  ;; Retrieve the hostname -- we need some Windows help here!
  if n_elements(save_host) EQ 0 then begin
      if n_elements(hostname) NE 1 then begin
          xhostname = (getenv('HOST'))(0)
          if xhostname EQ '' then begin
              if !version.os_family EQ 'unix' then $
                spawn, 'hostname', xhostname
              if xhostname(0) EQ '' then $
                xhostname = 'UNKNOWN'
          endif
          hostname = xhostname(0)
      endif
      save_host = hostname
  endif

  if n_elements(save_date) EQ 0 then save_date = systime(0)

  ;; TIMESTAMP
  ;;   BYTEx400 - empty (?) legacy area
  ;;   STRING - save date (as a string)
  ;;   STRING - user name
  ;;   STRING - hostname

  pointer = pointer + '400'xl  ;; Advance over 400 byte blank space
  cmsv_wraw, /string, block, pointer, $
    strtrim([save_date(0), save_user(0), save_host(0)],2), $
    status=status, errmsg=errmsg

end

pro cmsv_wversion, block, pointer, status=status, errmsg=errmsg, $
                   format_version=major_release, arch=arch, os=os, $
                   release=release, compatible=compat0, _extra=extra

  if n_elements(major_release) EQ 0 then major_release = 5L
  if n_elements(arch) EQ 0 then arch = !version.arch
  if n_elements(os) EQ 0 then os = !version.os
  if n_elements(release) EQ 0 then release = !version.release

  if n_elements(compat0) GT 0 then begin
      compat = strupcase(strtrim(compat0(0),2))
      case compat of 
          'IDL4': major_release = 4L
          'IDL5': major_release = 5L
          'IDL6': major_release = 6L
          'RIVAL1': major_release = 5L
          ELSE:   begin
              errmsg = 'ERROR: CMSV_WREC: unrecognized COMPAT value'
              status = 0
              return
          end
      endcase
  endif

  ;; VERSION_STAMP
  ;;   LONG - Major version number
  ;;   STRING_DATA - Host architecture ( = !version.arch )
  ;;   STRING_DATA - Host OS ( = !version.os )
  ;;   STRING_DATA - IDL release ( = !version.release )

  cmsv_wraw, /long, block, pointer, floor(major_release(0)), $
    status=status, errmsg=errmsg
  if status EQ 0 then return
  cmsv_wraw, /string, block, pointer, $
    strtrim([arch(0), os(0), release(0)],2), $
    status=status, errmsg=errmsg

  return
end

pro cmsv_wident, block, pointer, status=status, errmsg=errmsg, $
                 author=author, title=title, idcode=idcode, $
                 _EXTRA=extra

  if n_elements(author) EQ 0 then author = ''
  if n_elements(title) EQ 0 then title = ''
  if n_elements(idcode) EQ 0 then idcode = ''

  cmsv_wraw, /string, block, pointer, $
    strtrim([author(0), title(0), idcode(0)],2), $
    status=status, errmsg=errmsg

  return
end

; --------------------------- Read Notice --------------------

pro cmsv_wnotice, block, pointer, status=status, errmsg=errmsg, $
                  text=text, _EXTRA=extra

  ;; NOTICE
  ;;   STRING - notice text

  if n_elements(text) EQ 0 then text = ''

  cmsv_wraw, /string, block, pointer, strtrim(text,2), $
    status=status, errmsg=errmsg

  return
end


pro cmsv_wrec, block, pointer, data, name, unit=unit, offset=offset0, $
               initialize=init, finish=finish, no_data=nodata, no_type=notype,$
               block_type=blocktype, block_name=blockname, next_block=np, $
               compatibility=compat, $
               status=status, errmsg=errmsg, _EXTRA=extra

  common cmsave_block_names, block_ntypenames, block_typenames
  if n_elements(block_ntypenames) EQ 0 then begin
      cmsv_rrec, /qblocknames
  endif

  if keyword_set(init) then begin
      pointer = 0L
      block = 0 & dummy = temporary(block)
  endif
  if n_elements(pointer) EQ 0 then pointer = 0L
  
  if n_elements(blocktype) EQ 0 then begin
      wh = where(strupcase(blockname(0)) EQ block_typenames, ct)
      if ct EQ 0 then begin
          errmsg = 'ERROR: CMSV_WREC: block type '+blockname(0)+' is unknown'
          status = 0
          return
      endif
      blocktype = wh(0)
  endif

  ;; Store position of file pointer at beginning of record
  p0 = pointer
  
  rechead = lonarr(4)
  rechead(0) = long(floor(blocktype(0)))
  if n_elements(offset0) EQ 0 then offset = 0L $
  else                             offset = floor(offset0(0))

  ;; Default pointer to next block assumes no data in record
  if n_elements(np) GT 0 then rechead(1) = floor(np(0)) $
  else                        rechead(1) = offset + p0 + 4*4

  cmsv_wraw, /long, block, pointer, rechead, $
    status=status, errmsg=errmsg

  if status EQ 0 OR keyword_set(finish) then return

  p1 = pointer

  blockname = block_typenames(blocktype(0) < block_ntypenames)

  vdata = (blockname EQ 'VARIABLE' OR blockname EQ 'SYSTEM_VARIABLE' OR $
           blockname EQ 'HEAP_DATA')

  if vdata AND keyword_set(notype) then return

  case blockname of
      'COMMON_BLOCK': cmsv_wcomm, block, pointer, data, $
        status=status, errmsg
      'TIMESTAMP': cmsv_wstamp, block, pointer, $
        status=status, errmsg=errmsg, _EXTRA=data
      'VERSION': cmsv_wversion, block, pointer, compatible=compat, $
        status=status, errmsg=errmsg, _EXTRA=data
      'IDENTIFICATION': cmsv_wident, block, pointer, $
        status=status, errmsg=errmsg, _EXTRA=data
      'HEAP_INDEX': cmsv_wheap, block, pointer, data, $
        status=status, errmsg=errmsg
      'VARIABLE': cmsv_wvtype, block, pointer, data, name, $
        status=status, errmsg=errmsg
      'SYSTEM_VARIABLE': cmsv_wvtype, block, pointer, data, name, $
        status=status, errmsg=errmsg, /system
      'HEAP_DATA': cmsv_wvtype, block, pointer, data, name, $
        status=status, errmsg=errmsg, /heap
      ELSE: dummy = 1
  endcase        

  if status EQ 0 then return
  
  ;; If UNIT is specified then we write out the accumulated BLOCK data
  ;; up to this point.  CMSV_WDATA will write out its own data.
  if n_elements(unit) GT 0 AND pointer GT 0 then begin
      offset1 = offset
      unit1 = floor(unit(0))

      writeu, unit1, block(0:pointer-1)

      ;; Reinitialize state
      if n_elements(block) GT 65536L then begin
          ;; Clear the block if it is too large
          block = 0 & dummy = temporary(block)
          if n_elements(block) GT 0 then block(*) = 0
      endif
      offset = offset + pointer
      pointer = 0L
  endif

  ;; Write the variable data if it exists
  if vdata AND NOT keyword_set(nodata) AND n_elements(data) GT 0 then begin
      cmsv_wdata, block, pointer, data, unit=unit, $
        status=status, errmsg=errmsg, _EXTRA=extra
      if status EQ 0 then return
      if n_elements(unit1) GT 0 then $
        point_lun, -unit1, offset
  endif

  ;; Rewrite the record header using the new header.
  if pointer NE p1 AND n_elements(np) EQ 0 then begin
      rechead(1) = offset + pointer

      if n_elements(unit1) GT 0 then begin
          ;; Be careful: we have already written the record header to
          ;; disk, so we need to seek backwards, write the new header,
          ;; and then seek forwards again.
          point_lun, -unit1, offset2
          cmsv_wraw, /long, block1, 0L, rechead, $
            status=status, errmsg=errmsg
          if status EQ 0 then return
          point_lun, unit1, offset1+p0
          writeu, unit1, block1(0:15)
          point_lun, unit1, offset2

      endif else begin
          ;; It's easy if the data has not been written yet
          cmsv_wraw, /long, block, (p0+0), rechead, $
            status=status, errmsg=errmsg
          if status EQ 0 then return
      endelse
  endif

  status = 1
  return
end
