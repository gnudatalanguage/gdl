;+
; NAME:
;   CMRESTORE
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Restore variables from an IDL SAVE file.
;
; CALLING SEQUENCE: (various)
;   CMRESTORE, filename                           (implicit)
;   CMRESTORE, filename, var1, var2, ..., [, NAMES=names]
;   CMRESTORE, filename, DATA=pointers, NAMES=names, PASS_METHOD='POINTER'
;   CMRESTORE, filename, DATA=handles,  NAMES=names, PASS_METHOD='HANDLE'
;   CMRESTORE, filename, DATA=structure,             PASS_METHOD='STRUCT'
;   
; DESCRIPTION: 
;
;   CMRESTORE is a replacement for the built-in IDL procedure RESTORE.
;   It restores variables and data from an existing IDL SAVE file,
;   written either by SAVE or CMSAVE.  The CMSV utility library must
;   be installed in your IDL path to use CMSAVE and CMRESTORE.
;
;   The primary advantage to CMRESTORE is the ability to selectively
;   restore only certain variables from the input file (based on
;   name).  CMRESTORE provides a number of ways to pass the data
;   between routines, typically using a pointer or structure, which
;   avoids the unsafe practice of restoring variables in the caller's
;   namespace.  However, CMRESTORE can restore variables into the
;   caller's namespace, but users should be aware that this capacity
;   is somewhat limited in IDL versions 5.2 and below.
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
; COMPATIBILITY:
;
;   -- File Format --
;
;   CMRESTORE should be able to read files written by SAVE and CMSAVE
;   from IDL version 4 to version 5.4.
;
;   CMRESTORE cannot restore objects, pointers, compressed files, or
;   data sets larger than 2 gigabytes.
;   
;   Data types available in newer versions of IDL, such as pointers
;   and long integers, will not be readable in older versions of IDL
;   which do not have those data types.
;
;   -- Calling Interface --
;
;   For the most part, all capabilities of CMRESTORE are available to
;   the user.  However, it should be noted that passing variables by
;   positional parameter is not available under IDL 4, unless NAMES is
;   used to name the variables explicitly.  Also, under IDL versions
;   5.2 and previous, it is not possible for CMRESTORE to restore
;   variables into the caller's name space if they are not yet
;   defined.
;
;   This procedure is part of the CMSVLIB SAVE library for IDL by
;   Craig Markwardt.  You must have the full CMSVLIB core package
;   installed in order for this procedure to function properly.
;
; INPUTS:
;
;   FILENAME - the name of the SAVE file.
;              Default: none, this parameter must be specified.
;
;   VAR{i} - The values to be restored.  By default the save name is
;            taken from the named variables that are passed.  These
;            default names can be overridden by using the NAMES
;            keyword.
;
;            If no variables are passed as positional parameters, they
;            can still be saved using the DATA keyword.  By invoking
;            CMRESTORE without positional parameters or DATA,
;            CMRESTORE automatically will attempt to restore the
;            variables to the calling routine's name space (the
;            "implicit" technique).
;
;               NOTE: in IDL 5.2 and below, user routines are not
;               allowed to *CREATE* new variables in the caller's name
;               space.  CMRESTORE may fail if the variable in
;               undefined in the caller.  Therefore you must define it
;               before calling CMRESTORE.  The safer practice is to
;               use the VAR{i} positional parameters, or the DATA
;               keyword.
;
; KEYWORDS:
;
;   FILENAME - the name of the SAVE file.  The positional FILENAME
;              parameter takes precedence over the keyword FILENAME
;              parameter.
;
;   PASS_METHOD - a scalar string, describing the method of passing
;                 data between the caller and CMRESTORE.  The keyword
;                 can take the value 'ARGUMENT', 'POINTER', 'HANDLE'
;                 or 'STRUCT'.  A value of 'ARGUMENT' indicates that
;                 data values will be passed by command line argument,
;                 and is the default.  Other values are described
;                 below.
;
;   DATA - A list of data elements to be restored from the output
;          file.  The data elements can be one of the following,
;          depending on the value of PASS_METHOD.  The means of
;          extracting the data, and the method of naming each
;          variable, are also indicated.
;
;            * PASS_METHOD='POINTER': An array of pointers to the variables
;                    Data: pointed-to value     Name: from NAMES keyword
;            * PASS_METHOD='HANDLE':  An array of handles to the variables
;                    Data: pointed-to value     Name: from NAMES keyword
;            * PASS_METHOD='STRUCT':  A structure containing data to be saved
;                    Data: tag value            Name: tag name
;
;          Data values are restored one by one, using the appropriate
;          name.  Note that any variables passed as positional
;          parameters will cause the DATA keyword to be ignored.
;
;          CMRESTORE will allocate any pointer or handle resources.
;          The calling routine is responsible for deallocating any
;          pointer or handle resources.
;
;   NAMES - a string array, giving the names for each variable.  
;
;           If the data are passed by positional parameters, the names
;           are assigned according to the position of the parameter in
;           the procedure call.  
;
;           If the data are passed by an array of pointers or handles,
;           then the names are assigned according to the position of
;           the data in the array.  In this case there is no other way
;           to supply the variable name.  NAMES is required.
;
;           If the data are passed in a structure, then the names are
;           assigned according to the position of the data in the
;           structure.  The values specified in the names keyword
;           override the tag names.
;
;   STATUS - upon return, an integer indicating the status of the
;            operation.  A value of 1 indicates success, while 0
;            indicates failure.  A failure condition does not
;            necessarily indicate that an individual variable could
;            not be restored; use the VARSTATUS keyword to detect such
;            situations.
;
;   VARSTATUS - upon return, an integer array indicating the status of
;               the restore operation for each variable.  A value of 1
;               at position i in the array indicates success for the
;               ith variable, while a value of 0 indicates failure.
;
;   ERRMSG - upon return, a string indicating the status of the
;            operation.  The empty string indicates success, while a
;            non-empty string indicates failure and describes the
;            error condition.
;
;   QUIET - if set, then the error message is returned to the calling
;           routine.  By default an error condition causes execution
;           to stop and the message to be printed on the console.
;
;   VERBOSE - if set, then a short message is printed for each
;             variable.
;
; EXAMPLE:
;
;   CMSAVE, VAR1, VAR2, FILENAME='test.sav'
;   CMSAVE, VAR1, VAR2, FILENAME='test.sav', NAMES=['A','B']
;
;     Save the data in VAR1 and VAR2 to the file test.sav.  In the
;     first case the saved variable names will be VAR1 and VAR2.  In
;     the second case the saved variable names will be A and B.
;
;   POINTERS = [ptr_new(VAR1), ptr_new(VAR2)]
;   CMSAVE, DATA=POINTERS, NAMES=['A','B'], FILENAME='test.sav'
;
;     Save the data in VAR1 and VAR2 to the file test.sav.  The saved
;     variable names will be A and B.
;
;   STRUCTURE = {A: VAR1, B: VAR2}
;   CMSAVE, DATA=STRUCTURE, FILENAME='test.sav'
;
;     Save the data in VAR1 and VAR2 to the file test.sav.  The saved
;     variable names will be A and B.
;
; SEE ALSO:
;
;   CMSAVE, SAVE, RESTORE
;
; MODIFICATION HISTORY:
;   Written, 14 May 2000
;   Documented, 22 Sep 2000
;   Restore into caller's name space now permitted, 11 Jan 2001
;   Documented "implicit" restore a little better, w/ errors, 01 Mar 2001
;   Make version checks with correct precision, 19 Jul 2001, CM
;   Restore with no args automatically does ALL, is this right?,
;     CM, 20 Aug 2001
;   Added notification about RSI License, 13 May 2002, CM
;
; $Id: restore.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-

;; This utility function is only called under IDL 4
function arg_present, x
  return, 0
end

pro restore,     filename0, filename=filename1, $
                 p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,  p8,  p9, $
                 p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, $
                 p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, $
                 verbose=verbose, all=all, restored_objects=restobj, $
                 status=status, varstatus=colstatus, mtimes=mtimes, $
                 names=names, data=data, pass_method=method, $
                 errmsg=errmsg, quiet=quiet, nocatch=nocatch, $
                 relaxed_structure_assignment=relax
                  
  forward_function routine_names, ptrarr, ptr_new, handle_create, arg_present

  status = 0
  errmsg = ''
  colstatus = 0 & dummy = temporary(colstatus)  ;; Void out the status

  catch, catcherr
  if catcherr EQ 0 then lib = cmsvlib(/query) else lib = 0
  catch, /cancel
  if lib EQ 0 then $
    message, 'ERROR: The CMSVLIB library must be in your IDL path.'

  if NOT keyword_set(nocatch) then on_error, 2
  kall = 0                      ;; NOTE: Ignoring ALL keyword!!

  n_par = n_params()
  nnames = n_elements(names)
  ver = double(!version.release)
  if n_elements(filename0) EQ 0 AND n_elements(filename1) EQ 0 then begin
      message, 'USAGE: CMRESTORE, filename, VAR1, VAR2, ...', /info
      return
  end
  ;; Now n_par refers to the number of positional variables (ie,
  ;; filename excluded)
  n_par = n_par - 1

  ;; Default processing
  if n_elements(filename0) GT 0 then filename = strtrim(filename0(0),2) $
  else                               filename = strtrim(filename1(0),2)

  ;; Input method
  if n_elements(method) EQ 0 then begin
      if n_par GT 0 then meth = 'ARGUMENT' $
      else               meth = 'STORE'
  endif else begin
      meth = strupcase(strtrim(method(0),2))
  endelse
  ;; Trim it down so that people don't have to type too many characters
  meth = strmid(meth, 0, 3)

  ;; Error checking on input method, depending on IDL version
  if meth EQ 'ARG' AND ver LT 5D AND nnames EQ 0 then begin
      errmsg = ('ERROR: in IDL 4 you cannot pass variables by argument '+ $
                'to CMRESTORE.')
      goto, PRE_CLEANUP
  endif
  ;; Implicit restore of ALL, if no names are provided
  if nnames EQ 0 then kall = 1

  ;; Extract some information about the calling routine
  lev = routine_names(/level)
  vlev = lev - 1

  ;; Note: if the user specified "ALL" then we set NNAMES to zero,
  ;; indicating that the number of variables to be returned is unknown
  ;; at present.
  if kall then nnames = 0L

  if n_par LT 1 AND meth NE 'STO' AND arg_present(data) EQ 0 then begin
      if ver GE 5D OR meth EQ 'ARG' then begin
          errmsg = ('ERROR: A way to return data must be specified.  Pass by'+$
                    ' argument (IDL >= 5), or using the NAMES/DATA keywords.')
          goto, PRE_CLEANUP
      endif
  endif

  ;; ---------------- Establish I/O parameters -------------------
  ;; Upon exit from this block, at least the following variables must
  ;; be set: (a) nvar = number of named variables, (b) vnames = names
  ;; of variables in SAVE file to be restored.
  
  ;; Special case: if keyword ALL is set, then nvar EQ 0, and vnames
  ;; is undefined.  It will be filled in later then.
  if meth EQ 'ARG' then begin
      ;; Now processing that depends on the data passing method.  The
      ;; "argument" method is via positional arguments.

      ;; Extract variables from positional parameters
      if kall EQ 0 then nnames = n_par
      if (kall EQ 0 AND nnames EQ 0) $\
        OR (kall AND n_par LE 0) then begin
          errmsg = 'ERROR: Returnable variables must be specified.'
          goto, PRE_CLEANUP
      endif
      lev1 = strtrim(lev-1,2)
      nvar = nnames
      if nvar GT 0 then vnames = strarr(nvar)
      if NOT kall AND ver GE 5D then begin
          for i = 0L, nvar-1 do $
            dummy = execute('vnames(i) = (routine_names(p'+strtrim(i,2)+ $
                            ',arg_name='+lev1+'))(0)')
      endif
  endif else if meth EQ 'STO' then begin
      ;; This information is used to determine whether the variable
      ;; already exists in the caller.  If it doesn't, and we are in
      ;; IDL 5.2 or earlier, then we can't save it.
      cnames = strupcase(strtrim(routine_names(variables=vlev),2))

      nvar = nnames
      if nvar GT 0 then vnames = strarr(nvar)
      
  endif else begin

      ;; Instead of passing the data via positional parameters, they
      ;; can be set through the DATA keyword, but even there this can
      ;; be accomplished with several means: via pointers, via
      ;; handles, or using a structure.
      
      ;; Clear DATA in preparation for restore operation
      data = 0 & dummy = temporary(data)
      nvar = nnames
      if nvar GT 0 then vnames = strarr(nvar)

      if meth EQ 'POI' then begin               ;; POINTER TYPE
          ;; Construct an array of null pointers to start with
          if nvar GT 0 then data = ptrarr(nvar)
      endif else if meth EQ 'HAN' then begin     ;; HANDLE TYPE
          ;; Construct an array of invalid handles to start with
          if nvar GT 0 then data = lonarr(nvar)-1L
      endif else if meth EQ 'STR' then begin
          ;; Do nothing
      endif else begin
          errmsg = 'ERROR: PASS_METHOD must be one of ARGUMENT, POINTER, '+$
            'HANDLE or STRUCT'
          PRE_CLEANUP:
          status = 0
          if NOT keyword_set(quiet) then message, errmsg, /info
          return
      endelse
  endelse

  if kall EQ 0 then begin
      if nvar LE 0 then begin
          errmsg = 'ERROR: no variable names were specified'
          goto, PRE_CLEANUP
      endif else begin
          colstatus = lonarr(nvar)
      endelse
  endif

  ;; User-renamed variables.  These names will override any names
  ;; specified in positional parameters.
  if n_elements(names) GT 0 AND NOT kall then begin
      sz = size(names)
      if sz(sz(0)+1) NE 7 then begin
          errmsg = 'ERROR: NAMES must be a string array'
          goto, PRE_CLEANUP
      endif
      vnames(0) = strtrim(strupcase(names(*)),2)
  endif

  ;; Open the save file
  get_lun, unit
  cmsv_open, unit, filename, pp, access='R', status=status, errmsg=errmsg
  if status EQ 0 then goto, CLEANUP
  if keyword_set(verbose) then $
    message, 'Portable (XDR) SAVE/RESTORE file.', /info, traceback=0

  pp0 = pp  ;; Block pointer
  ivar = 0L ;; Number of variables that have been read successfully
  if n_elements(vnames) GT 0 then $
    found = lonarr(n_elements(vnames))

  ptr_index   = [0L]
  ptr_offsets = [0L]
  if ver GE 5D then ptr_data = [ptr_new()] $
  else              ptr_data = [0L]

  ;; Now begin the processing
  repeat begin
      ;; Read block from SAVE file
      bn = ''
      point_lun, unit, pp
      block = 0 & dummy = temporary(block)
      cmsv_rrec, block, pp1, bdata, unit=unit, next_block=pnext, /init, $
        block_type=bt, block_name=bn, status=status, errmsg=errmsg, offset=pp,$
        promote64=promote64
      if status EQ 0 then goto, CLEANUP
      eb = (bn EQ 'END_MARKER')

      ;; Examine each block type  -----

      errmsg = ''
      jfind = -1L
      case bn of 

          ;; Promote record header to 64-bits (compatibility)
          'PROMOTE64': if keyword_set(verbose) then begin
              message, 'File contains 64-bit offsets.', /info, traceback=0
          endif

          ;; Read time stamp record
          'TIMESTAMP': if keyword_set(verbose) then begin
              stamp = bdata
              message, 'Save file written by '+stamp.save_user+'@'+ $
                stamp.save_host+', '+stamp.save_date, /info, traceback=0
          endif
          
          ;; Read version record
          'VERSION': if keyword_set(verbose) then begin
              vers = bdata
              message, ('IDL version '+vers.release+' ('+vers.os+ $
                        ', '+vers.arch+')'), /info, traceback=0
              message, 'File format revision: '+ $
                strtrim(vers.format_version,2), /info, traceback=0
          endif

          ;; Read heap index record
          'HEAP_INDEX': begin
              ii = bdata
              ptr_index   = [ptr_index, ii]
              ptr_offsets = [ptr_offsets, ii*0L]
              if ver GE 5D then $
                ptr_data    = [ptr_data, ptrarr(n_elements(ii))]
          end

          ;; Read heap data - just store a file pointer for later
          ;;                  referral
          'HEAP_DATA': begin
              p2 = pp1
              cmsv_rvtype, block, pp1, vindex, /heap, unit=unit, $
                status=st1, errmsg=errmsg
              if st1 EQ 0 then goto, NEXT_BLOCK

              ;; VINDEX will be the heap variable number.  Once we
              ;; know this we can put the file offset into
              vindex = floor(vindex(0))
              wh = where(ptr_index EQ vindex, ct)
              if ct EQ 0 then goto, NEXT_BLOCK
              ptr_offsets(wh(0)) = pp + p2  ;; block address + offset
          end              

          ;; Read variable data, and store for return to caller
          'VARIABLE': begin
              ;; Read variable type
              cmsv_rvtype, block, pp1, vn, sz1, unit=unit, status=st1, $
                template=tpp1, errmsg=err1
              if vn EQ '' OR st1 EQ 0 then goto, NEXT_BLOCK
              if kall EQ 0 then begin
                  jfind = (where(vn EQ vnames, ct))(0)
                  if ct EQ 0 then goto, NEXT_BLOCK
                  found(jfind) = 1
              endif
              
              ;; Read variable data
              cmsv_rdata, block, pp1, sz1, val, template=tpp1, status=st1, $
                unit=unit, errmsg=errmsg, ptr_offsets=ptr_offsets, $
                ptr_index=ptr_index, ptr_data=ptr_data
              if st1 EQ 0 then goto, NEXT_BLOCK
              if sz1(0) GT 0 then arr = 1 else arr = 0
              if arr then begin
                  ;; If an array then reform to be sure dimensions are right
                  dims = sz1(1:sz1(0))
                  val = reform(val, dims, /overwrite)
              endif
              
              if kall then begin
                  ;; With ALL, we extend the vector at each variable
                  jfind = ivar & ii = strtrim(jfind,2)
                  if ivar EQ 0 then begin
                      anames = [vn]
                      colstatus = [0L]
                  endif else begin
                      anames = [anames, vn]
                      colstatus = [colstatus, 0L]
                  endelse
              endif
              
              ;; Now send the data to output, depending on the method
              case meth of 
                  'ARG': begin
                      ;; Position dependent parameter
                      if jfind GE n_par then goto, NEXT_BLOCK
                      ii = strtrim(jfind,2)
                      if arr EQ 0 then begin
                          dummy = execute('p'+ii+' = temporary(val)') 
                      endif else begin
                          dummy = execute('p'+ii+' = reform(val, dims, '+ $
                                          '/overwrite)')
                      endelse
                      if dummy EQ 0 then goto, NEXT_BLOCK
                  end
                  'STO': begin
                      ;; Store the data in caller.  Check for IDL 5.3
                      ;; compatibility.
                      jfind1 = where(vn EQ cnames, ct1)
                      if ver LT 5.3D AND jfind1(0) EQ -1 then begin
                          if keyword_set(quiet) EQ 0 then begin
                              message, 'WARNING: could not create variable '+$
                                vn+' in calling routine.', /info, traceback=0
                          endif
                          goto, NEXT_BLOCK
                      endif
                      dummy = routine_names(vn, val, store=vlev)
                  end
                  'POI': begin
                      ;; Pointer to data
                      if kall then begin
                          if ivar EQ 0 then data = ptrarr(1) $
                          else data = [data, ptr_new()]
                      endif
                      data(jfind) = ptr_new(val)
                  end
                  'HAN': begin
                      ;; Handle to data
                      if kall then begin
                          if ivar EQ 0 then data = [-1L] $
                          else data = [data, -1L]
                      endif
                      data(jfind) = handle_create(value=val, /no_copy)
                  end
                  'STR': begin
                      ;; Add data to structure
                      if n_elements(sdata) EQ 0 then $
                        sdata = create_struct(vn, val) $
                      else $
                        sdata = create_struct(sdata, vn, val)
                  end
              endcase
              
              colstatus(jfind) = 1
              if keyword_set(verbose) then $
                message, 'Restored variable: '+vn+'.', /info, traceback=0
              if kall then ivar = ivar + 1
          end


          ELSE: dummy = 1
      endcase

      NEXT_BLOCK:
      if NOT keyword_set(quiet) then begin
          if errmsg NE '' then $
            message, errmsg, /info
          if keyword_set(verbose) AND jfind GE 0 then begin
              if colstatus(jfind) EQ 1 then $
                message, 'Restored variable: '+vn+'.', /info, traceback=0 $
              else $
                message, 'Unable to restore variable: '+vn+'.', /info, $
                traceback=0
          endif
      endif
      ;; Advance to next block
      if pp NE pnext then begin
          pp = pnext 
      endif else begin
          status = 0
          errmsg = 'ERROR: internal inconsistency'
      endelse
  endrep until bn EQ 'END_MARKER' OR status EQ 0

  ;; Clean up
  free_lun, unit

  DONE_RESTORE:
  if meth EQ 'STR' then begin
      ;; Put new struct data on output
      data = 0 & dummy = temporary(data)
      if n_elements(sdata) GT 0 then data = temporary(sdata)
  endif
  if kall then begin
      names = 0 & dummy = temporary(names)
      if n_elements(anames) GT 0 then names = anames
  endif

  if n_elements(found) GT 0 then begin
      wh = where(found EQ 0, ct)
      fmt = '(A,'+strtrim(ct,2)+'(A,:," "),".")'
      if ct GT 0 then begin
          errmsg = string("WARNING: the following variables were not found: ",$
                          vnames(wh), format=fmt)+'.'
          if NOT keyword_set(quiet) OR keyword_set(verbose) then $
            message, errmsg, /info, traceback=0
      endif
  endif

  if total(colstatus) GT 0 then begin
      status = 1
  endif else begin
      status = 0
      errmsg = 'ERROR: No variables were restored.'
      if meth EQ 'STO' AND ver LT 5.3D then begin
          errmsg = errmsg + '  NOTE: In IDL 5.2 and earlier the variable '+$
            'must first be defined by the caller in order to use the '+$
            '"implicit" restore technique.'
          message, errmsg, /info, traceback=0
      endif
  endelse

  return

  ;; 
  ;; Error handling routine, outside the normal call flow
  CLEANUP:
  catch, /cancel
  if n_elements(unit) GT 0 then free_lun, unit
  status = 0
  if errmsg NE '' AND NOT keyword_set(quiet) then message, errmsg, /info

  return
end

