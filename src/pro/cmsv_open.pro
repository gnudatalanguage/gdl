;+
; NAME:
;   CMSV_OPEN
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Open IDL SAVE file for reading or writing
;
; CALLING SEQUENCE:
;   CMSV_OPEN, UNIT, FILENAME, OFFSET, $
;          ACCESS=ACCESS, /FORCE, /GET_LUN, /REOPEN, $
;          COMPATIBILITY=COMPATIBILITY, $
;          STATUS=STATUS, ERRMSG=ERRMSG
;   
; DESCRIPTION: 
;
;   CMSV_OPEN opens an IDL SAVE-formatted file for reading or writing.
;   The mode of operation is controlled by the ACCESS keyword, which
;   may be either 'R' for reading, 'W' for writing, or 'RW' for
;   read/write access.
;
;   'R': In the case of reading, the specified file is opened with
;   read-only access, and the first bytes are examined to verify that
;   it is indeed a valid IDL SAVE file.  
;
;   'W': In the case of writing, the specified file is opened with
;   write access, and the initial file signature is written.  
;
;   'RW': In the case of read-write access, the file must already
;   exist as a valid SAVE file.  Users are advised that every time
;   they switch between reading and writing operations, they must use
;   POINT_LUN to flush the file buffers.
;
;   The CMSVLIB routines do not support file sizes greater than 2 GB,
;   nor SAVE files created with the COMPRESS option.
;
;   Upon return, the file pointer is positioned at the start of the
;   first valid SAVE record.  The file offset is returned in OFFSET.
;   The user is responsible for reading or writing the remainder of
;   the file with other library routines.
;
;   The file unit is determined based on the following criteria.  This
;   behavior is similar to the OPEN family of procedures, except for
;   the REOPEN keyword.
;
;     * If REOPEN is set then it is assumed that UNIT is an
;       already-open file, and FILENAME is ignored.  
;
;     * If GET_LUN is set then a file unit is allocated with GET_LUN,
;       and upon success this unit is returned in UNIT.
;
;     * Otherwise it is asssumed that UNIT is a valid but unopened
;       file unit.  Upon successful return, UNIT is opened.
;
;   This procedure is part of the CMSVLIB SAVE library for IDL by
;   Craig Markwardt.  You must have the full CMSVLIB core package
;   installed in order for this procedure to function properly.
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
; INPUTS:
;
;   UNIT - a logical unit number (a scalar).  In the case of GET_LUN,
;          a file unit will be allocated and returned in UNIT.  In the
;          default case, or REOPEN, UNIT must be a valid file unit
;          upon input.  For REOPEN the corresponding file must be
;          seekable.
;
;   FILENAME - a scalar string specifying the filename path (ignored
;              for REOPEN).
;
;   OFFSET - upon return, the file offset of the next available SAVE
;            record.
;
;
; KEYWORDS:
;
;   ACCESS - a scalar string, case insensitive:
;               'R' - read-only access
;               'W' - write access (new file)
;               'RW' - read-write access (existing file)
;            Default: 'R' - read-only
;
;   GET_LUN - if set, the file unit is allocated using GET_LUN
;
;   FORCE - if set, then the file is opened despite a detected file
;           format inconsistency.
;
;   REOPEN - if set, then an already-opened file is manipulated.  The
;            valid file unit must be specified by UNIT, and FILENAME
;            is ignored.
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
;   Change BLOCK to STREAM to support VMS properly, 14 Feb 2001, CM
;   Added notification about RSI License, 13 May 2002, CM
;
; $Id: cmsv_open.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-
pro cmsv_open, unit, filename, offset, access=access0, force=force, $
               get_lun=get_lun, status=status, errmsg=errmsg, $
               reopen=reopen, compatibility=compat, query=query, $
               compressed=compressed

  status = 0
  offset = 0L
  errmsg = ''
  error = 0

  if keyword_set(query) then return

  if keyword_set(get_lun) then get_lun, unit
  if n_elements(access0) EQ 0 then access0 = 'R'
  access = strupcase(strtrim(access0(0),2))

  if access NE 'R' AND access NE 'W' AND access NE 'RW' then begin
      errmsg = 'ERROR: CMSV_OPEN: ACCESS must be one of "R", "RW" or "W"'
      status = 0
      return
  endif

  ;; RE-OPEN

  if keyword_set(reopen) then begin
      ;; Re-open an existing unit, but first make sure it's open
      unit = floor(unit(0))
      stat = fstat(unit)
      errmsg = 'ERROR: CMSV_OPEN: Unit '+strtrim(unit,2)+' is not open'
      if stat.open EQ 0 then begin
          REOPEN_ERROR:
          return
      endif
      if access EQ 'R' then begin
          if stat.read EQ 0 then begin
              errmsg = errmsg + ' for reading'
              goto, REOPEN_ERROR
          endif
      endif else if access EQ 'RW' then begin
          if stat.read EQ 0 OR stat.write EQ 0 then begin
              errmsg = errmsg + ' for reading and writing'
              goto, REOPEN_ERROR
          endif
      endif else begin
          if stat.read EQ 0 OR stat.write EQ 0 then begin
              errmsg = errmsg + ' for writing'
              goto, REOPEN_ERROR
          endif
      endelse

      point_lun, unit, 0L
      errmsg = ''
  endif

  ;; READ or READ-WRITE ACCESS

  if access EQ 'R' OR access EQ 'RW' then begin
      compressed = 0L

      if NOT keyword_set(reopen) then begin
          if access EQ 'R' then cmd = 'openr' else cmd = 'openu'
          call_procedure, cmd, unit(0), filename, error=error, /STREAM
          
          if error NE 0 then begin
              READ_ERROR:
              close, unit(0)
              if keyword_set(get_lun) then free_lun, unit(0)
              status = 0
              errmsg = !err_string
              return
          endif
      endif

      ;; Read 4-byte signature at start of file
      on_ioerror, READ_ERROR
      signature = bytarr(4)
      readu, unit, signature
      if string(signature(0:1)) NE 'SR' then begin
          close, unit(0)
          if keyword_set(get_lun) then free_lun, unit(0)
          errmsg = 'ERROR: CMSV_OPEN: '+strtrim(filename(0),2)+ $
            ' is not a valid SAVE file'
          return
      endif

      ;; Check for compressed files, which are not supported
      if signature(3) EQ 6 then begin
          compressed = 1
          if NOT keyword_set(force) then begin
              errmsg = 'ERROR: CMSVLIB library cannot read '+ $
                'compressed files'
              status = 0
              return
          endif
      endif
  endif else if access EQ 'W' then begin

      ;; WRITE-ONLY (CREATE) ACCESS
      
      if NOT keyword_set(reopen) then begin
          openw, unit, filename, error=error, /STREAM

          if error NE 0 then begin
              WRITE_ERROR:
              close, unit(0)
              if keyword_set(get_lun) then free_lun, unit(0)
              status = 0
              errmsg = !err_string
              return
          endif
      endif

      ;; Write a valid signature
      vbyte = '04'xb
      ;; '06'xb would indicate a compressed SAVE file, which we don't
      ;; support.

      on_ioerror, WRITE_ERROR
      ;              S       R
      signature = ['53'xb, '52'xb, '00'xb, vbyte]
      writeu, unit, signature
  endif

  status = 1
  point_lun, -unit, offset
  return
end
