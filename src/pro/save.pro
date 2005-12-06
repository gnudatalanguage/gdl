;+
; NAME:
;   CMSAVE
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Save IDL variables to a file.  (IDL v5 and greater)
;
; CALLING SEQUENCE: (various)
;   CMSAVE, var1, var2, ..., FILENAME=filename [, NAMES=names]
;   CMSAVE, DATA=pointers, NAMES=names, FILENAME=filename
;   CMSAVE, DATA=handles, NAMES=names,  FILENAME=filename
;   CMSAVE, DATA=structure,             FILENAME=filename
;   
; DESCRIPTION: 
;
;   CMSAVE is a replacement for the built-in IDL procedure SAVE, with
;   more flexibility and increased capabilities.
;
;   CMSAVE produces output files that are compatible with the standard
;   save format of IDL versions 4 and 5 (and perhaps greater).
;   Variables may be restored by using either the built-in procedure
;   RESTORE, or CMRESTORE.  The CMSV utility library must be installed
;   in your IDL path to use CMSAVE and CMRESTORE.
;
;   The primary advantages to CMSAVE are the ability to append
;   additional variables to an existing SAVE file, and several
;   flexible ways to pass the data and variable names to be saved.
;   CMSAVE also attempts to run on all IDL versions.
;
;   To append variables to an existing file, simply specify the APPEND
;   keyword, and the filename of an existing writable SAVE file.
;   Variables will be appended to the end of the file.  It is possible
;   to append a variable with the same name as an existing variable in
;   a file.  Both data values are stored in the file.  However the
;   results upon restore are undefined.  There is also a limitation
;   that only one series of heap values -- pointed-to data -- may be
;   saved in a file.
;
;   By the normal convention of the built-in SAVE command, both the
;   data and variable names to be saved are passed as parameters on
;   the command line.  Each parameter must be a named variable; both
;   the name and value are saved.
;
;   This convention may be used in invoking CMSAVE as well.  However,
;   in addition to passing the data by positional parameter, the user
;   can pass the data using the DATA keyword.  If the DATA keyword is
;   used, then an array of pointers or handles may be passed, or a
;   structure of values may be passed.  (see below)  If both are
;   passed, then the positional parameters take precedence.
;
;   It is also possible to explicitly rename the saved variables: the
;   saved name does not need to be the name of the named variable.
;   Use the NAMES keyword to override the default name.  By default
;   the name is drawn from any named variables, or from the structure
;   tag names if the DATA keyword is used with a structure.  The NAMES
;   keyword takes precedence over these values.  NOTE: Values passed
;   by pointer or handle are not named by default, and so will not be
;   saved unless the NAMES keyword is used.
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
;   Files written by CMSAVE should be readable with all known versions
;   of IDL at the time of this writing (version 4 - version 5.4).  It
;   is expected that this compatibility will persist.
;
;   CMSAVE cannot write objects, compressed files, or data sets larger
;   than 2 gigabytes.
;
;   Data types available in newer versions of IDL, such as pointers
;   and long integers, will not be readable in older versions of IDL
;   which do not have those data types.
;
;   -- Calling Interface --
;
;   For the most part, all capabilities of CMSAVE are available to the
;   user.  However, it should be noted that passing variables by
;   positional parameter is not available under IDL 4, unless NAMES is
;   used to name the variables explicitly.
;
;   This procedure is part of the CMSVLIB SAVE library for IDL by
;   Craig Markwardt.  You must have the full CMSVLIB core package
;   installed in order for this procedure to function properly.
;
; INPUTS:
;
;   VAR{i} - The variables to be saved.  By default the save name is
;            taken from the named variables that are passed.  These
;            default names can be overridden by using the NAMES
;            keyword.
;
;            Variables can also be specified by passing an array of
;            handles or pointers in the DATA keyword.
;
;            If no variables are passed as positional parameters or
;            using DATA, then CMSAVE will assume that *all* variables
;            at the caller's level should be saved (this is similar
;            behavior to the built-in SAVE routine).  Note that system
;            variables cannot be saved with CMSAVE.
;
; KEYWORDS:
;
;   ALL - for compatibility with the built-in SAVE routine.  Ignored.
;         Note that CMSAVE cannot save system variables.
;
;   FILENAME - the name of the output file.
;              Default: 'cmsave.sav'
;
;   DATA - A list of data elements to be saved to the output file.
;          The data elements can be one of the following.  The means
;          of extracting the data, and the method of naming each
;          variable, are also indicated.
;
;            * An array of pointers to the variables
;                    Data: pointed-to value     Name: from NAMES keyword
;            * An array of handles to the variables
;                    Data: pointed-to value     Name: from NAMES keyword
;            * A 1-element structure containing data to be saved.
;                    Data: tag value            Name: tag name
;
;          Data values are saved one by one, using the appropriate
;          name.  Note that any variables passed as positional
;          parameters will cause the DATA keyword to be ignored.
;
;          The calling routine is responsible for deallocating any
;          pointer or handle resources.
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
;   NAMES - a string array, giving the names for each variable.  
;
;           If the data are passed by positional parameters, the names
;           are assigned according to the position of the parameter in
;           the procedure call.  This can be especially useful to
;           rename local variables, and to give names to expressions.
;
;           If the data are passed by an array of pointers or handles,
;           then the names are assigned according to the position of
;           the data in the array.  In this case there is no other way
;           to supply the variable name.  NAMES is required.
;
;           If the data are passed in a structure, then the names are
;           assigned according to the position of the data in the
;           structure.  The NAMES keyword values override the tag
;           names.
;
;   APPEND - if set, then the specified variables are appended to an
;            existing file.  
;
;            Repeated variables will not cause an error, however they
;            may not be restored properly using the built-in RESTORE
;            procedure.  It may also not be permitted to append
;            variables that contain heap pointers, to a save file
;            which already contains heap data.
;
;   STATUS - upon return, an integer indicating the status of the
;            operation.  A value of 1 indicates success, while 0
;            indicates failure.  A failure condition does not
;            necessarily indicate that an individual variable could
;            not be written; use the VARSTATUS keyword to detect such
;            situations.
;
;   VARSTATUS - upon return, an integer array indicating the status of
;               the save operation for each variable.  A value of 1 at
;               position i in the array indicates success for the ith
;               variable, while a value of 0 indicates failure.
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
;   XDR - for compatibility with the built-in SAVE routine.  Ignored.
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
;     variable names will be A and B.  Data are passed by pointer.
;
;   STRUCTURE = {A: VAR1, B: VAR2}
;   CMSAVE, DATA=STRUCTURE, FILENAME='test.sav'
;
;     Save the data in VAR1 and VAR2 to the file test.sav.  The saved
;     variable names will be A and B.  Data are passed by structure.
;
; SEE ALSO:
;
;   CMRESTORE, SAVE, RESTORE, CMSVLIB
;
; MODIFICATION HISTORY:
;   Written, 14 May 2000
;   Documented, 22 Sep 2000
;   Made "more" compatible with SAVE; additional documentation, 11 Jan
;     2001, CM
;   Make version checks with correct precision, 19 Jul 2001, CM
;   Added notification about RSI License, 13 May 2002, CM
;
; $Id: save.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-
function cmsave_handle_value, handle, no_copy=nocop  ;; Utility function
  handle_value, handle, value, no_copy=keyword_set(nocop)
  return, value
end

;; Undocumented
;; MTIMES
;; PASS_METHOD
;; NOCATCH

pro save,    p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,  p8,  p9, $
             p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, $
             p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, $
             filename=filename0, verbose=verbose, xdr=xdr, $
             compatible=compat0, append=append, all=all, $
             status=status, varstatus=colstatus, mtimes=mtimes, $
             names=names, data=data, pass_method=method, $
             errmsg=errmsg, quiet=quiet, nocatch=nocatch, useunit=useunit

  forward_function routine_names, ptr_valid, handle_info

  status = 0
  errmsg = ''
  colstatus = 0 & dummy = temporary(colstatus)  ;; Void out the status

  catch, catcherr
  if catcherr EQ 0 then lib = cmsvlib(/query) else lib = 0
  catch, /cancel
  if lib EQ 0 then $
    message, 'ERROR: The CMSVLIB library must be in your IDL path.'

  if NOT keyword_set(nocatch) then on_error, 2

  n_par = n_params()
  ver   = double(!version.release)
  kall  = 0                        ;; NOTE: Ignoring ALL keywod!!
  if n_elements(filename0) EQ 0 then filename0 = 'cmsave.sav'
  filename = strtrim(filename0(0),2)

  if n_elements(compat0) EQ 0 then compat0 = 'IDL5'
  compat = strupcase(strtrim(compat0(0),2))
  lev = routine_names(/level)
  szd = size(data)
  ndata = n_elements(data)
  nnames = n_elements(names)
  
  ;; By default, if no arguments are specified (and no data) then, then
  ;; all of the data in the caller is saved.
  if n_par EQ 0 AND ndata EQ 0 then kall = 1
  if n_par EQ 0 AND kall EQ 0 AND nnames EQ 0 AND ndata EQ 0 then begin
      NO_VARS:
      status = 1
      errmsg = 'WARNING: no variables were specified; '+filename+' not created'
      if NOT keyword_set(quiet) then message, errmsg, /info
      return
  endif

  if kall then begin
      ;; Extract variables from calling procedure
      vlev = lev - 1
      vnames = routine_names(variables=vlev)
      fnames = 'routine_names("'+vnames+'",fetch=vlev)'
      nvar = n_elements(vnames)
      atype = 'FETCH'

  endif else if n_par EQ 0 AND nnames GT 0 then begin
      ;; Extract named variables from the calling procedure
      vlev = lev - 1
      vnames = strtrim(names,2)
      fnames = 'routine_names("'+vnames+'",fetch=vlev)'
      nvar = nnames
      atype = 'FETCH'
      
  endif else if n_par GT 0 then begin
      ;; Extract variables from positional parameters
      vlev = lev
      lev1 = strtrim(lev-1,2)
      nvar = n_par
      fnames = 'p'+strtrim(indgen(nvar),2)
      vnames = strarr(nvar)
      if ver GE 5D then $
        for i = 0L, nvar-1 do $
        dummy = execute('vnames(i) = (routine_names('+fnames(i)+ $
                        ',arg_name='+lev1+'))(0)')
      atype = 'FETCH'

  endif else begin
      ;; Extract variables from DATA
      vlev = lev
      tp = szd(szd(0)+1)
      if tp EQ 10 then begin               ;; POINTER TYPE
          nvar = ndata
          fnames = strarr(nvar) & vnames = fnames
          wh = where(ptr_valid(data) EQ 1, ct)
          if ct GT 0 then fnames(wh) = '*(data['+strtrim(wh,2)+'])'
          atype = 'POINTER'
      endif else if tp EQ 3 then begin     ;; HANDLE TYPE
          nvar = ndata
          fnames = strarr(nvar) & vnames = fnames
          wh = where(handle_info(data) EQ 1, ct)
          if ct GT 0 then $
            fnames(wh) = 'cmsave_handle_value(data('+strtrim(wh,2)+'))'
          atype = 'HANDLE'
      endif else if tp EQ 8 then begin
          nvar = n_tags(data(0))
          stsize = tagsize(data(0), n_tags=nvar, tag_names=vnames)
          fnames = 'data(0).'+vnames
          atype = 'STRUCT'
      endif else begin
          errmsg = 'ERROR: keyword DATA must be a pointer array or structure'
          status = 0
          return
      endelse
  endelse

  if nvar LE 0 then goto, NO_VARS
  colstatus = lonarr(nvar)

  ;; User-renamed variables
  if n_elements(names) GT 0 then begin
      sz = size(names)
      if sz(sz(0)+1) NE 7 then begin
          errmsg = 'ERROR: NAMES must be a string array'
          status = 0
          return
      endif
      vnames(0) = strtrim(strupcase(names(*)),2)
  endif

  ;; Check for variables with missing names.  By default don't do
  ;; anything, unless all the variables do not have names, or if we
  ;; are using IDL 4, which cannot accept parameters from the command
  ;; line.
  wh = where(vnames EQ '', ct)
  if ct EQ nvar then begin
      errmsg = ('ERROR: no variables have names (are you using IDL 4 or' + $
                ' forget the NAMES keyword?)')
      status = 0
      return
  endif
  if ver LT 5D AND ct GT 0 then begin
      if keyword_set(verbose) then $
        message, ('WARNING: in IDL 4 you cannot pass variables by argument '+ $
                  'to CMSAVE.'), /info, traceback=0
  endif

  ;; Collect a summary of pointers
  if ver GE 5D then begin
      cmsv_ptrsum, null, /null
      pheap = null
      pind  = 0L
      
      catcherr = 0
      catch, catcherr
      if catcherr NE 0 then goto, DONE_PTR
      for i = 0L, nvar-1 do begin
          pheap1 = null
          if fnames(i) EQ '' OR $
            execute('cmsv_ptrsum, '+fnames(i)+', pheap1') EQ 0 then $
            goto, DONE_PTR
          if n_elements(pheap1) GT 1 then begin
              pheap = [pheap, pheap1]
              pheap = pheap(uniq(pheap, sort(pheap)))
          endif
          DONE_PTR:
      endfor
      catch, /cancel
  endif else pheap = 0L

  nheap = n_elements(pheap) - 1
  if nheap GT 0 AND compat EQ 'IDL4' then begin
      errmsg = 'ERROR: cannot store pointer data in IDL4 version file'
      status = 0
      return
  endif

  ;; Open output file
  if keyword_set(append) then access='RW' else access='W'
  cmsv_open, unit, filename, off0, access=access, /get_lun, status=status, $
    errmsg=errmsg, compat=compat
  if status EQ 0 then begin
      if keyword_set(quiet) EQ 0 then message, errmsg, traceback=0
      return
  endif
  pp = 0L

  has_heap = 0
  nauxvars = 0L
  if keyword_set(append) then begin
      ;; Scan through input file until we reach the end-of-file block
      bn = ''
      while status EQ 1 AND bn NE 'END_MARKER' do begin
          point_lun, unit, off0
          block = 0 & dummy = temporary(block)
          cmsv_rrec, block, pp, unit=unit, next_block=pnext, /init, $
            block_type=bt, block_name=bn, status=status, errmsg=errmsg, $
            promote64=promote64
          if status EQ 0 then goto, CLEANUP
          if bn EQ 'HEAP_INDEX' then has_heap = 1
          if bn EQ 'VARIABLE' AND status EQ 1 then begin
              cmsv_rvtype, block, pp, vn, sz1, unit=unit, status=st1, $
                template=tp1, errmsg=err1
              if strmid(vn,0,12) EQ '_CMSAVE_DIR_' then $
                nauxvars = nauxvars + 1
          endif
          if status EQ 1 AND bn NE 'END_MARKER' then off0 = pnext
      endwhile

      if bn NE 'END_MARKER' then goto, CLEANUP
      point_lun, unit, off0
      
  endif else begin
      ;; Create the file and append the start record blocks
      if keyword_set(verbose) then $
        message, 'Portable (XDR) SAVE/RESTORE file.', /info

      ;; both timestamp and version information
      cmsv_wrec, block, pp, block_name='TIMESTAMP', offset=off0, $
        status=status, errmsg=errmsg
      if (status NE 0) AND (compat NE 'IDL4') then $
        cmsv_wrec, block, pp, block_name='VERSION', offset=off0, $
        status=status, errmsg=errmsg, compat=compat
      if (status EQ 0) OR (pp EQ 0) then goto, CLEANUP

      writeu, unit, block(0:pp-1)
      off0 = off0 + pp
  endelse

  if nheap GT 0 AND keyword_set(append) AND has_heap AND $
    strmid(compat,0,3) EQ 'IDL' then begin
      errmsg = 'ERROR: cannot APPEND data containing POINTER type'
      goto, CLEANUP
  endif

  pp = 0L
  block = 0 & dummy = temporary(block)

  if nheap GT 0 then begin
      pind = lindgen(nheap) + 1
      pheap = pheap(1:nheap)
      cmsv_wrec, block, pp, pind, block_name='HEAP_INDEX', offset=off0, $
        status=status, errmsg=errmsg, unit=unit1
      if status NE 1 then goto, CLEANUP
  endif 
  if pp GT 0 then writeu, unit, block(0:pp-1)
  off0 = off0 + pp

  if keyword_set(useunit) then $
    unit1 = unit

  pp = 0L
  block = 0 & dummy = temporary(block)
  init = 1 & err1 = '' & saved = 0
  erri = strarr(nvar+nheap)

  catcherr = 0 & caught = 0
  if NOT keyword_set(nocatch) then catch, catcherr
  if catcherr NE 0 then begin
      caught = caught + 1
      goto, DONE_VAR
  endif

  for i = 0L, nheap+nvar-1 do begin
      saved = 0
      nelem = 0
      j = i - nheap

      if j LT 0 then begin
          ;; Extract heap value
          bn = 'HEAP_DATA'
          vname = long(pind(i))
          dummy = execute('sz = size(*pheap(i))')
          if sz(sz(0)+1) EQ 0 then goto, DONE_VAR
          dummy = execute('var = *pheap(i)')

      endif else begin
          ;; Extract variable value
          bn = 'VARIABLE'
          fname = fnames(j)
          vname = vnames(j)
          
          sz = 0 & var = 0 & dummy = temporary(var)
          if fname EQ '' then begin
              NO_NAME:
              err1 = 'WARNING: un-named expression(s) not written'
              goto, DONE_VAR
          endif

          if execute('sz = size('+fname+')') EQ 0 then goto, DONE_VAR
          if sz(sz(0)+1) EQ 0 OR sz(sz(0)+2) EQ 0 then goto, DONE_VAR
          if execute('var = '+fname) EQ 0         then goto, DONE_VAR

          ;; For data taken from the structure then we may need to
          ;; pull out a more accurate array type.
          if atype EQ 'STRUCT' then sz = stsize(*,j)
          sz = sz(0:sz(0)+2)
          
          nelem = n_elements(var)
          if nelem EQ 0  then goto, DONE_VAR
          if vname EQ '' then goto, DONE_VAR
      endelse

      ;; Stupid dimensions can be reformed just by assignment.  This
      ;; will catch and reform the variable.
      if (sz(0) GT 0) AND (n_elements(size(var)) NE n_elements(sz)) then $
        var = reform([var], sz(1:sz(0)), /overwrite)
      sz = size(var)
 
      ;; Write data to a block in memory
      off1 = pp
      cmsv_wrec, block, pp, var, vname, block_name=bn, offset=off0, $
        ptr_index=pind, ptr_data=pheap, init=init, unit=unit1, $
        status=status, errmsg=errmsg
      init = 0
      if status EQ 0 then goto, CLEANUP
      saved = 1

      DONE_VAR:
      ;; A tricky business, this error catching...
      if caught GT 1 then begin
          errmsg = 'ERROR: an internal error occurred'
          goto, CLEANUP
      endif
      if caught GT 0 then begin
          err1 = errmsg
      endif

      ;; If we have accumulated enough data, write to file
      if (pp GT 32768L) OR ((i EQ nheap+nvar-1) AND pp GT 0) then begin
          writeu, unit, block(0:pp-1)
          block = 0 & dummy = temporary(block)
          init = 1
          off0 = off0 + pp
          pp = 0L
      endif
      if keyword_set(useunit) AND pp EQ 0 then $
        point_lun, -unit, off0

      ;; Print some diagnostic messages
      if j GE 0 then begin
          colstatus(j) = saved
          if keyword_set(verbose) AND saved then $
            erri(i) = 'Saved variable: '+vname+'.'
          if saved EQ 0 then begin
              if err1 NE '' then $
                erri(i) = err1 $
              else if nelem EQ 0 then $
                erri(i) = 'Undefined item not saved: '+vnames(j)+'.' $
              else begin
                  erri(i) = ('Expression must be named variable: <'+$
                             helpform('', var, /short)+'>')
                  if ver LT 5D then $
                    erri(i) = erri(i) + ' - use NAMES keyword under IDL4'
              endelse
          endif
      endif else begin
          if keyword_set(verbose) AND saved then $
            erri(i) = ('Saved heap value: <'+ $
                       helpform('', var, /short)+'>')
      endelse

      caught = 0
  endfor
  catch, /cancel
  wh = where(erri NE '', ct)
  if ct GT 0 then begin
      if NOT keyword_set(quiet) then for i = 0L, ct-1 do $
        message, erri(wh(i)), /info
      fmt = '('+strtrim(ct,2)+'(A,:))'
      if ct GT 1 then erri(wh(1:*)) = erri(wh(1:*)) + ';'
      errmsg = string(erri(wh), format=fmt)
  endif

  if pp GT 0 then writeu, unit, (temporary(block))(0:pp-1)
  off0 = off0 + pp

  pp = 0L & off1 = 0L
  ;; Write a set of auxiliary directory information
  whgood = where(colstatus NE 0, ngoodvars)
  if ngoodvars GT 0 AND strmid(compat,0,5) EQ 'RIVAL' then begin
      ;; Construct a unique name for the auxiliary directory
      nauxvars = nauxvars + 1
      fname = filename
      i = (rstrpos(fname,'/') > rstrpos(fname,'\') > rstrpos(fname,':') > $
           rstrpos(fname,';') > 0L)
      fname = byte(strmid(fname, i, strlen(fname)))
      i = strpos(fname, '.')
      if i GT 0 then fname = strupcase(strmid(fname, 0, i))
      bb = byte(fname)
      wh = where(bb LT (byte('A'))(0) OR bb GT (byte('Z'))(0), ct)
      if ct GT 0 then bb(wh) = 32b
      fname = strcompress(string(bb), /remove_all)
      if strlen(fname) EQ 0 then fname = 'FILE'
      
      ;; Whew, we got a name!  Now create the data for the directory
      auxvname = string(fname,nauxvars, $
                        format='("_CMSAVE_DIR_",A0,"_",I2.2)')
      auxvdata = replicate({variable:'', mtime:systime(1)}, ngoodvars)
      auxvdata.variable = vnames(whgood)
      if n_elements(mtimes) GT 0 then $
        auxvdata.mtime(0) = double(mtimes(whgood(0:n_elements(mtimes)-1)))
      auxsz = size(auxvdata)

      ;; Write out directory data
      cmsv_wrec, block, pp, auxvdata, auxvname, block_name='VARIABLE', $
        offset=off0, status=status, errmsg=errmsg, unit=unit1
  endif
  ;; Write end marker
  cmsv_wrec, block, pp, block_name='END_MARKER', status=status, offset=off0
  if status EQ 1 AND pp GT 0 then writeu, unit, block(0:pp-1)
  free_lun, unit
  return

  ;; 
  ;; Error handling routine, outside the normal call flow
  CLEANUP:
  catch, /cancel
  free_lun, unit
  status = 0
  if errmsg NE '' AND NOT keyword_set(quiet) then message, errmsg, /info

  return
end
