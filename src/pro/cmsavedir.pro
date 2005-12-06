;+
; NAME:
;   CMSAVEDIR
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Display a directory of the variables in an IDL SAVE file.
;
; CALLING SEQUENCE:
;   CMSAVEDIR, filename [, /VERBOSE, /QUIET, ... ]
;   
; DESCRIPTION: 
;
;   CMSAVEDIR will display a listing of the variables and other
;   objects stored in an IDL SAVE file.
;
;   For command-line users the primary function of CMSAVEDIR will be
;   to find out what data is stored in a file and when it was saved.
;   For that, they simply need to type: CMSAVEDIR, 'myfile.sav'
;
;   CMSAVEDIR also offers a number of features for programmers.
;   CMSAVEDIR essentially interrogates the save file and discovers the
;   numbers, names and types of each of the variables stored in the
;   file.  Programmers can use that information to decide whether or
;   how to open a file using the other routines in the CMSVLIB.
;   Various keyword parameters are used return this information, as
;   documented below.
;
; ==================================================================
;   Research Systems, Inc. has issued a separate license intended
;   to resolve any potential conflict between this software and the
;   IDL End User License Agreement. The text of that license
;   can be found in the file LICENSE.RSI, included with this
;   software library.
; ==================================================================
;
; COMPATIBILITY:
;
;   -- File Format --
;
;   CMSAVEDIR cannot examine compressed save files, or files larger
;   than 2 gigabytes.
;
;   This procedure is part of the CMSVLIB SAVE library for IDL by
;   Craig Markwardt.  You must have the full CMSVLIB core package
;   installed in order for this procedure to function properly.
;
; INPUTS:
;
;   FILENAME - a scalar string, the name of the file to be examined.
;
; KEYWORDS:
;
;   VERBOSE - if set, print more detailed information about the input file.
;
;   QUIET - if set, then do not print any output.  Programmers can use
;           this keyword to allow CMSAVEDIR to return information
;           about the file silently.
;
;   STATUS - upon output, 1 for success, otherwise to indicate
;            failure.
;   ERRMSG - upon output, if a failure occurs, a message describing
;            the error condition.
;  
;   N_VARIABLES - upon output, the number of variables in the file.
;   VAR_NAMES - upon output, contains a string array of the names of
;               the variables stored in the file.
;   TYPES - upon output, an 11xN_VARIABLES array containing the SIZE
;           information of each variable in the file.
;
;   N_PRO - upon output, the number of procedures stored in the file.
;   PRO_NAMES - upon output, the names of the procedures stored in the
;               file, as a string array.
;
;   N_FUNCTION - upon output, the number of functions stored in the
;                 file.
;   FUNC_NAMES - upon output, the names of the functions stored in the
;                file, as a string array.
;
;   NAMED_STRUCTS - upon output, a string array listing any named
;                   structures which appear in the SAVE file.
;   NAMED_CLASSES - upon output, a string array listing any named
;                   class structures which appear in the SAVE file.
;
;   TIMESTAMP - upon output, contains the timestamp record information
;               in a structure.  The fields of the structure are:
;                    SAVE_DATE - string - date saved
;                    SAVE_USER - string - user who saved file
;                    SAVE_HOST - string - host name on which file
;                                         saved
;
;   VERSION - upon output, contains the version record information in
;             a structure.  The fields of the structure are:
;                    FORMAT_VERSION - integer - major format version
;                    ARCH    - string - saving host's !VERSION.ARCH
;                    OS      - string - saving host's !VERSION.OS
;                    RELEASE - string - saving host's !VERSION.RELEASE
;
;   NOTICE - upon output, contains any textual notice included within
;            the file.  The fields of the structure are:
;                    TEXT - string - text of the notice
;
;   FORCE - if set, will force CMSAVEDIR to open the file even if it
;           detects a potential incompatibility.
;
; EXAMPLE:
;
;   IDL> cmsavedir, 'int_str_intarr.sav'
;   ** int_str_intarr.sav
;   ** Sun Apr  9 20:28:25 2000 (craigm@beach.gsfc.nasa.gov)
;   ** IDL v5.2 (linux)
;     A               INT       =        0
;     B               STRING    = 'hello'
;     C               INT       = Array[3]
;   ** 3 variable(s), 0 heap value(s) and 0 procedure(s) in 1376 bytes
;
; SEE ALSO:
;
;   CMRESTORE, CMSAVE, SAVE, RESTORE, CMSVLIB
;
; MODIFICATION HISTORY:
;   Documented, 12 Jan 2001, CM
;   Added USAGE message, 09 Jun 2001, CM
;   Fixed bug in printing common variables, 17 Mar 2002, CM
;   Added notification about RSI License, 13 May 2002, CM
;   Added NOTICE record type, 09 Jun 2003, CM
;   
;
; $Id: cmsavedir.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2002, 2003, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-

pro cmsavedir, filename, types=vtypes, force=force, status=status, $
               var_names=vnames, pro_names=pronames, func_names=funnames, $
               n_variables=nvar, n_pro=npro, n_function=nfunc, $
               errmsg=errmsg, verbose=all, read_heap=rheap, raw=raw, $
               timestamp=stamp, version=vers, quiet=quiet, $
               named_structs=structs, named_classes=classes, $
               notice=notice

  forward_function cmsv_rraw

  status = 0
  errmsg = ''

  if n_params() EQ 0 then begin
      message, 'USAGE: CMSAVEDIR, filename, ...', /info
      return
  endif

  catch, catcherr
  if catcherr EQ 0 then lib = cmsvlib(/query) else lib = 0
  catch, /cancel
  if lib EQ 0 then $
    message, 'ERROR: The CMSVLIB library must be in your IDL path.'

  ;; Open the file
  get_lun, unit
  cmsv_open, unit, filename(0), pp, status=status, errmsg=errmsg, $
    force=force, compressed=compressed
  if status NE 1 then begin
      free_lun, unit
      message, errmsg, /info
      return
  endif
  pp0 = pp

  ;; Initial set-up
  ;; promote64 is due to a format change in IDL 5.4
  ;; stamp is the timestamp structure
  ;; vers  is the version structure
  ;; firstvar is used to know when we hit the first variable so we can
  ;;          print the header of the display
  ;; rstamp and rvers are use to know when we have read the timestamp
  ;;        and version records.

  print_raw = keyword_set(raw)
  promote64 = 0 & opromote64 = 0
  stamp = {save_date: '', save_user: '', save_host: ''}
  vers  = {arch: '', os: '', release: '4.X', format_version: 0L}
  notice = {text: ''}
  firstvar = 1 & nvar = 0L & npro = 0L & nfunc = 0L & nheap = 0L
  rstamp = 0 & rvers = 0
  fs = fstat(unit)
  filesize = fs.size

  ;; Empty the NAMED_STRUCTS and NAMED_CLASSES keywords, so they can
  ;; be filled in fresh by CMSV_RVTYPE
  structs = 0 & dummy = temporary(structs)
  classes = 0 & dummy = temporary(classes)

  ;; Simple set-up for heap data
  ptr_index   = [0L]
  ptr_offsets = [0L]

  ;; Loop through the file reading records as we go.

  while status EQ 1 do begin
      ;; Read the next block, or at least the first 2 kB of it
      ;; This is OK since all scalar types will be smaller than that
      point_lun, unit, pp
      cmsv_rrec, block, p1, bdata, unit=unit, next_block=pnext, /init, $
        block_type=bt, block_name=bn, status=status, errmsg=errmsg, $
        promote64=promote64, compressed=compressed
      if status EQ 0 then goto, DONE

      if bn EQ 'END_MARKER' then goto, DONE

      ;; After any header blocks have been read, then print info
      if NOT keyword_set(quiet) AND firstvar $
        AND bn NE 'TIMESTAMP' AND bn NE 'VERSION' then begin
          print, '** '+filename(0)
          print, stamp.save_date, stamp.save_user, stamp.save_host, $
            format='("** ",A0," (",A0,"@",A0,")")'
          print, vers.release, vers.os, $
            format='("** IDL v",A0," (",A0,")")'
          firstvar = 0
      endif
      
      ;; We can't really decode the compressed block, but we can print
      ;; some information about it
      if compressed then goto, FAILED_BLOCK

      errmsg = ''
      ;; Check for each block type of interest.
      case bn of 

          ;; Timestamp record
          'TIMESTAMP': begin
              stamp = bdata
              if NOT keyword_set(all) then goto, NEXT_BLOCK
              rstamp = 1
              if NOT keyword_set(quiet) then begin
                  print, stamp.save_date, format='("    Date: ''",A0,"''")'
                  print, stamp.save_user, format='("Username: ''",A0,"''")'
                  print, stamp.save_host, format='("Hostname: ''",A0,"''")'
              endif
          end

          ;; Version record
          'VERSION': begin
              vers = bdata
              if NOT keyword_set(all) then goto, NEXT_BLOCK

              rvers = 1
              if NOT keyword_set(quiet) then begin
                  print, vers.arch,           format='("    Arch: ''",A0,"''")'
                  print, vers.os,             format='("      OS: ''",A0,"''")'
                  print, vers.release,        format='(" Release: ''",A0,"''")'
                  print, vers.format_version, format='("File Ver: ",I0)'
              endif
          end

          'IDENTIFICATION': begin
              ident = bdata
              if NOT keyword_set(all) then goto, NEXT_BLOCK
              
              print, ident.author,        format='("  Author: ''",A0,"''")'
              print, ident.title,         format='("   Title: ''",A0,"''")'
              print, ident.idcode,        format='(" ID Code: ''",A0,"''")'
          end

          'NOTICE': begin
              notice = bdata
              if keyword_set(all) then $
                print, '   - Record type '+strtrim(bt,2)+' ('+bn+')'
              print, strlen(notice.text), $
                format='("** NOTICE record: ",I0," chars (view with /VERBOSE)")'
              if keyword_set(all) then $
                print, notice.text
          end

          ;; Variable record.  This is the meat of the routine.  Some
          ;; other routines are routed here too, when they need to
          ;; print variable information.
          'VARIABLE': begin
              sysvar = 0
              DO_VARIABLE:
              tp1 = 0
              stname = 0 & dummy = temporary(stname)
              ;; Read name and type
              cmsv_rvtype, block, p1, varname, sz1, status=st1, template=val, $
                unit=unit, system=sysvar, errmsg=errmsg, /no_create, $
                structure_name=stname, $
                named_structs=structs, named_classes=classes
              if varname EQ '' OR st1 EQ 0 then goto, FAILED_BLOCK

              ;; Save the name and type information
              if nvar EQ 0 then vnames = [varname] $
              else vnames = [vnames, varname]
              if nvar EQ 0 then vtypes = ([sz1, lonarr(11)])(0:10) $
              else vtypes = [vtypes, ([sz1, lonarr(11)])(0:10)]
              nvar = nvar + 1

              ;; Read the data if needed
              type1 = sz1(sz1(0)+1)
              if sz1(0) EQ 0 AND type1 NE 11 then begin
                  if n_elements(val) GT 0 then tp1 = val
                  cmsv_rdata, block, p1, sz1, val, template=tp1, status=st1, $
                    unit=unit, errmsg=errmsg
                  if st1 EQ 0 then goto, NEXT_BLOCK
              endif 

              ;; Print the data
              hf = helpform(varname, val, size=sz1, width=78, $
                            structure_name=stname)
              if NOT keyword_set(quiet) then begin
                  print, '  '+hf, format='(A)'
              endif
          end

          ;; A system variable is recorded just like a variable.
          'SYSTEM_VARIABLE': begin
              sysvar = 1
              goto, DO_VARIABLE
          end

          ;; A common variable is printed but not counted
          'COMMON_BLOCK': begin
              names = bdata
              if n_elements(names) LE 1 then  goto, NEXT_BLOCK
              if NOT keyword_set(quiet) then begin
                  print, '  '+names(1:*)+' ('+names(0)+')', format='(A)'
              endif
              ;; nvar = nvar + n_elements(names) ; don't count common variables
          end

          ;; A compiled routine.  Just the simple stuff is restored
          ;; here since it seems to change a lot between versions.
          'COMPILED': begin

              proname = cmsv_rraw(/string, block, p1, status=st1, unit=unit)
              if st1 then prolength = cmsv_rraw(/long, block, p1, status=st1)
              if st1 then n_vars = cmsv_rraw(/long, block, p1, status=st1)
              if st1 then n_args = cmsv_rraw(/long, block, p1, status=st1)
              if st1 then flags = cmsv_rraw(/long, block, p1, status=st1)
              if st1 EQ 0 then goto, NEXT_BLOCK
              key = '' & extra = ''
              if (flags AND '01'xb) NE 0 then begin
                  type = 'FUNCTION' 
                  if nfunc EQ 0 then funnames = proname $
                  else funnames = [funnames, proname]
                  nfunc = nfunc + 1
              endif else begin
                  type = 'PRO'
                  if npro EQ 0 then pronames = proname $
                  else pronames = [pronames, proname]
                  npro = npro + 1
              endelse
              if (flags AND '02'xb) NE 0 then key  = ', KEYWORDS'
              if (flags AND '08'xb) NE 0 then extra = ', _EXTRA'
              if NOT keyword_set(quiet) then begin
                  print, type, proname, n_args, key, extra, $
                    format='("  ",A0," ",A0,", ",I0," args",A0,A0)'
              endif
          end

          ;; A heap header is really an index to following heap
          ;; variables, so that at a later time they can be restored.
          'HEAP_INDEX': begin
              if keyword_set(all) then $
                print, '   - Record type '+strtrim(bt,2)+' ('+bn+')'
              ii = bdata
              ptr_index   = [ptr_index, ii]
              ptr_offsets = [ptr_offsets, ii*0L]
          end

          ;; The heap data is stored here, unnamed.
          'HEAP_DATA': begin
              if keyword_set(all) then $
                print, '   - Record type '+strtrim(bt,2)+' ('+bn+')'
              p2 = p1
              cmsv_rvtype, block, p1, vindex, /heap, /no_create, unit=unit, $
                status=st1, errmsg=errmsg, $
                structure_name=stname, $
                named_structs=structs, named_classes=classes
              if st1 EQ 0 then goto, NEXT_BLOCK

              ;; VINDEX will be the heap variable number.  Once we
              ;; know this we can put the file offset into
              vindex = floor(vindex(0))
              wh = where(ptr_index EQ vindex, ct)
              if ct EQ 0 then goto, NEXT_BLOCK
              nheap = nheap + 1
              ptr_offsets(wh(0)) = pp + p2
          end

          ;; What to do in the generic case
          else: begin
              FAILED_BLOCK:
              if compressed then compstr = '  (compressed)' else compstr = ''
              if keyword_set(all) OR bn EQ 'UNKNOWN' then $
                print, '   - Record type '+strtrim(bt,2)+' ('+bn+')'+compstr
              if keyword_set(all) AND bn EQ 'UNKNOWN' then print_raw = 1
          end
      endcase

      NEXT_BLOCK:
      ;; Some final extremely verbose output for every block, if
      ;; requested.
      if print_raw then begin
          point_lun, unit, pp
          cmsv_rrec, block, p1, unit=unit, /init, /full, $
            status=status, errmsg=errmsg, offset=pp, $
            promote64=opromote64, compressed=compressed
          if status EQ 1 then begin
              print, '     (binary data)'
              binaryform_long, block
          endif
      endif

      if NOT keyword_set(raw) then print_raw = 0
      if errmsg NE '' then message, errmsg, /info

      ;; Now advance to the next record.
      if pnext EQ 0 then begin
          message, 'WARNING: premature termination of '+filename(0), /info
          goto, DONE
      endif
      if pp NE pnext then begin
          pp = pnext 
      endif else begin
          status = 0
          errmsg = 'ERROR: internal inconsistency'
      endelse

      opromote64 = promote64
  endwhile
  DONE:
  if status EQ 0 then message, errmsg, /info 

  ;; Final statistics
  if nvar GT 0 then vtypes = reform(vtypes, 11, nvar, /overwrite)

  if NOT keyword_set(quiet) then $
    print, nvar, nheap, npro+nfunc, filesize, $
    format=('("** ",I0," variable(s), ",I0," heap value(s) and ",' + $
            'I0," procedure(s) in ",I0," bytes")')

  free_lun, unit
  return
end
