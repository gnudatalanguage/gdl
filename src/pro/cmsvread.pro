;+
; NAME:
;   CMSVREAD
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Read a single variable from an open SAVE file
;
; CALLING SEQUENCE:
;
;   CMSVREAD, UNIT, DATA [, NAME=NAME, /NO_DATA, VERSION=VERSION,
;                           TIMESTAMP=TIMESTAMP ]
;   
; DESCRIPTION: 
;
;   CMSVREAD reads a single IDL variable from an open IDL SAVE file.
;   The file should already have been opened as a normal file using
;   OPENR.
;
;   CMSVREAD is a simplified version of the CMSVLIB package, and as
;   such is not capable of reading heap data (pointers) or object
;   data.  Strings, structures, and all array types are supported.
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
; INPUTS:
;
;   UNIT - the open file unit.
;
;   DATA - a named variable, into which the new data is to be read.
;
; KEYWORDS:
;
;   NAME - upon output, the name of the saved variable is returned in
;          this keyword.  If a failure or end of file condition
;          occurs, name will be undefined upon return.
;
;   STRUCTURE_NAME - if the data to be read is a structure, upon
;                    output, this keyword will contain the name of the
;                    structure.  A value of '' indicates an anonymous
;                    structure.
;
;   SIZE - upon output, the SIZE type of the data is returned in this
;          keyword.
;
;   NO_DATA - if set, no data is read from the file, only the variable
;            name and type.
;
;   TIMESTAMP - after the first call to CMSVREAD on a newly opened
;               file, this keyword will contain the file timestamp
;               structure.
;
;   VERSION - after the first call to CMSVREAD on a newly opened file,
;             this keyword will contain the file version information,
;             if available.
;
;   QUIET - if set, error messages are not printed.
;           Default: an error causes errors to be printed with MESSAGE
;
;   STATUS - upon return, this keyword will contain 1 for success and
;            0 for failure.
;
;   ERRMSG - upon return with a failure, this keyword will contain the
;            error condition as a string.
;
; EXAMPLE:
;
;   Read all variables from a file, and print help on them.
;
;   openr, 50, 'test.sav'
;   name = ''
;   while n_elements(name) GT 0 do begin   ;; EOF signalled by NAME undefined
;      cmsvread, 50, data, name=name
;      help, name, data
;   end
;   close, 50
;
; SEE ALSO:
;
;   CMSVWRITE, CMRESTORE, CMSAVE, RESTORE, CMSVLIB
;
; MODIFICATION HISTORY:
;   Written and documented, 11 Jan 2001, CM
;   Added notification about RSI License, 13 May 2002, CM
;
; $Id: cmsvread.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2001, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-
pro cmsvread, unit0, data, timestamp=tstamp, version=ver, $
              name=name, size=sz, no_data=nodata, structure_name=stname, $
              promote64=promote64, $
              quiet=quiet, status=status, errmsg=errmsg

  status = 0
  catch, catcherr
  if catcherr EQ 0 then lib = cmsvlib(/query) else lib = 0
  catch, /cancel
  if lib EQ 0 then begin
      errmsg = 'ERROR: The CMSVLIB library must be in your IDL path.'
      if keyword_set(quiet) then return else message, errmsg
  endif

  name   = 0 & dummy = temporary(name)
  data   = 0 & dummy = temporary(data)
  sz     = 0 & dummy = temporary(sz)
  tp     = 0 & dummy = temporary(tp)
  stname = 0 & dummy = temporary(stname)

  if n_elements(unit0) EQ 0 then begin
      errmsg = 'ERROR: UNIT is not defined'
      if keyword_set(quiet) then return else message, errmsg
  endif
  unit = floor(unit0(0))

  stat = fstat(unit)
  if stat.read EQ 0 OR stat.open EQ 0 then begin
      errmsg = 'ERROR: UNIT is not open for reading'
      if keyword_set(quiet) then return else message, errmsg
  endif
  
  ;; Thanks to Liam Gumley to show that one can check the file pointer
  ;; to see if we are at the start.

  ;; We are at the beginning of the file, make sure this is a proper
  ;; IDL save file.
  if stat.cur_ptr EQ 0 then begin
      cmsv_open, unit, 'FILENAME', pointer, access='R', /reopen, $
        status=status, errmsg=errmsg
      if status EQ 0 then begin
          if keyword_set(quiet) then return else message, errmsg
      endif
  endif

  done = 0
  while NOT done do begin
      block = 0 & dummy = temporary(block)
      cmsv_rrec, block, p1, bdata, unit=unit, next_block=pnext, /init, $
        block_type=bt, block_name=bn, status=status, errmsg=errmsg, $
        promote64=promote64, /autopromote64
      if status EQ 0 then begin
          if keyword_set(quiet) then return else message, errmsg
      endif

      case bn of 
          'END_MARKER': begin
              done = 1
          end

          'TIMESTAMP': begin
              tstamp = bdata
          end

          'VERSION': begin
              ver = bdata
          end

          'VARIABLE': begin
              sysvar = 0
              DO_VARIABLE:
              tp = 0
              cmsv_rvtype, block, p1, name, sz, status=status, template=tp, $
                unit=unit, system=sysvar, errmsg=errmsg, $
                structure_name=stname
              if status EQ 0 OR name EQ '' then begin
                  status = 0
                  if errmsg EQ '' then $
                    errmsg = 'ERROR: could not read variable name'
                  if keyword_set(quiet) then return else message, errmsg
              endif

              if NOT keyword_set(nodata) then begin
                  tp1 = sz(sz(0)+1)
                  if tp1 EQ 0 OR tp1 EQ 10 OR tp1 EQ 11 then begin
                      status = 0
                      if tp1 EQ 0 then $
                        errmsg = 'ERROR: variable type is undefined' $
                      else if tp1 EQ 10 OR tp1 EQ 11 then $
                        errmsg = 'ERROR: CMSVREAD cannot read heap or objects'
                      if keyword_set(quiet) then return else message, errmsg
                  endif

                  cmsv_rdata, block, p1, sz, data, template=tp, unit=unit, $
                    status=status, errmsg=errmsg
                  tp = 0
                  if status EQ 0 then begin
                      errmsg = 'ERROR: could not read data'
                      if keyword_set(quiet) then return else message, errmsg
                  endif
              endif
              done = 1
          end
          
          'SYSTEM_VARIABLE': begin
              sysvar = 1
              goto, DO_VARIABLE
          end


          ELSE: dummy = 1
      endcase

      point_lun, unit, pnext
  endwhile

  status = 1
  return
end
