;+
; NAME:
;   CMSV_TEST
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Test the CMSVLIB library
;
; CALLING SEQUENCE:
;   CMSV_TEST, FILENAME, USER_VALUE=UVALUE
;   
; DESCRIPTION: 
;
;   CMSV_TEST performs a standard test of the CMSVLIB library.  It
;   reads and writes a save file using several different methods in an
;   attempt to test the different ways that the library can be used.
;   The test procedure can be used to verify that the library is
;   functioning properly.
;
;   By default, this procedure provides standard values to be written,
;   but the user can provide one of them with the USER_VALUE keyword.
;   If the user's data contains pointers, they must expect tests
;   listed under CMSVREAD and CMSVWRITE to fail.
;
;   By default the file is written in the current directory, but this
;   can be changed with the FILENAME parameter.
;
;   This procedure is part of the CMSVLIB SAVE library for IDL by
;   Craig Markwardt.  You must have the full CMSVLIB core package
;   installed in order for this procedure to function properly.
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
; INPUTS:
;
;   FILENAME - a scalar string, the output path.  
;              Default: 'CMSVTEST.SAV' in current directory
;
; KEYWORDS:
;
;   USER_VALUE - any IDL variable to be saved, in place of variable
;                "C" in the test.
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
;   Resolve all routines at start, and add VMS keyword, 14 Feb 2001, CM
;   Make version checks with correct precision, 19 Jul 2001, CM
;   Added notification about RSI License, 13 May 2002, CM
;
; $Id: cmsv_test.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-
forward_function ptr_new, ptr_valid

pro cmsv_test_set, a, b, c, d, e, set=set, reset=reset, print=print
  common cmsv_test_set_common, c0
  if keyword_set(set) then begin
      c0 = a
      return
  endif
  if keyword_set(print) then begin
      help, /struct, c0
  endif
  if keyword_set(reset) then begin
      c0 = 0 & dummy = temporary(c0)
  endif
  a = 1
  b = findgen(10)
  if n_elements(c0) GT 0 then $
    c = c0 $
  else $
    c = (5-dindgen(20))*!dpi
  d = {tag1: 1, tag2: 2, tag3: 'tag3', tag4: [4], tag5: findgen(10), $
       tag6: {tag6a: 'a', tag6b: 'DEADBEEF'XL} }
  e = dcomplexarr(2,2) + 3.1415
end

pro cmsv_test_undefine, x
  x = 0 & dummy = temporary(x)
end

pro cmsv_test_unset, a, b, c, d, e
  cmsv_test_undefine, a
  cmsv_test_undefine, b
  cmsv_test_undefine, c
  cmsv_test_undefine, d
  cmsv_test_undefine, e
end

pro cmsv_test_vcomp, x0, x, result, failed, name=name0
  result = 0

  if n_elements(name0) EQ 0 then name = '' $
  else name = strtrim(name0(0),2)+':'
  sz  = size(x)
  sz0 = size(x0)
  failed = name+'dim'
  nd = sz(0)

  if sz(0) NE sz0(0) then return
  failed = name+'typ'
  if sz(sz(0)+1) NE sz0(sz0(0)+1) then return

  if sz(0) GT 0 then begin
      for i = 1, nd do $
        if sz(i) NE sz0(i) then begin
          failed = name+'vec'+strtrim(i,2)
          return
      endif
  endif

  tp = sz(sz(0)+1)
  if (tp EQ 10) then begin
      for i = 0L, n_elements(x0)-1 do begin
          cmd = 'cmsv_test_vcomp, *(x0(i)), *(x(i)), result, reason, ' +$
            'name=name+"ptr"+strtrim(i,2)'
          dummy = execute(cmd)
          if result EQ 0 then return
          if dummy EQ 0 then begin
              reason = name+'err'
              result = 0
              return
          endif
      endfor
  endif else if (tp GE 1 AND tp LE 7) OR (tp GE 9 AND tp LE 15) then begin
      ;; Any type that is comparable
      failed = name+'val'
      if round(total(x EQ x0)) NE n_elements(x) then return
  endif else if tp EQ 8 then begin
      ;; Structures
      nt = n_tags(x)
      nt0 = n_tags(x0)
      failed = name+'ntg'
      if nt NE nt0 then return

      tn = tag_names(x)
      tn0 = tag_names(x0)
      failed = name+'tnm'
      if round(total(tn EQ tn0)) NE nt then return

      ts = tagsize(x)
      ts0 = tagsize(x0)
      failed = name+'tsz'
      if round(total(ts EQ ts0)) NE n_elements(ts) then return

      for i = 0L, nt-1 do begin
          cmsv_test_vcomp, x0.(i), x.(i), result, failed, $
            name=name+'tag:'+strtrim(i,2)
          if result EQ 0 then return
      endfor
      
  endif else begin
      ;; Other types
      failed = 'unk'
      return
  endelse
  
  failed = ''
  result = 1

  return
end


pro cmsv_test_comp, a, b, c, d, e, result, failed

  cmsv_test_set, a0, b0, c0, d0, e0

  cmsv_test_vcomp, a0, a, result, failed, name='a'
  if result EQ 0 then return

  cmsv_test_vcomp, b0, b, result, failed, name='b'
  if result EQ 0 then return

  cmsv_test_vcomp, c0, c, result, failed, name='c'
  if result EQ 0 then return

  cmsv_test_vcomp, d0, d, result, failed, name='d'
  if result EQ 0 then return

  cmsv_test_vcomp, e0, e, result, failed, name='e'
  if result EQ 0 then return

  result = 1
  failed = ''
  return

end
  
pro cmsv_test, filename0, user_value=uval
  
  rcsid = '$Id: cmsv_test.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $'

  catch, catcherr
  if catcherr EQ 0 then lib = cmsvlib(/query) else lib = 0
  catch, /cancel
  if lib EQ 0 then $
    message, 'ERROR: The CMSVLIB library must be in your IDL path.'

  ver = double(!version.release)
  if ver LT 5D then compat = 'IDL4'

  resolve_routine, 'cmrestore'
  resolve_routine, 'cmsave'
  resolve_routine, 'cmsavedir'
  resolve_routine, 'cmsv_open'
  resolve_routine, 'cmsv_ptrsum'
  resolve_routine, 'cmsv_rdata'
  resolve_routine, 'cmsv_rraw', /is_function
  resolve_routine, 'cmsv_rrec'
  resolve_routine, 'cmsv_rvtype'
  resolve_routine, 'cmsv_wdata'
  resolve_routine, 'cmsv_wraw'
  resolve_routine, 'cmsv_wrec'
  resolve_routine, 'cmsv_wvtype'
  resolve_routine, 'cmsvlib', /is_function
  resolve_routine, 'cmsvread'
  resolve_routine, 'cmsvwrite'
  resolve_routine, 'tagsize', /is_function
  resolve_routine, 'helpform', /is_function

  cmsv_test_set, /reset
  if n_elements(uval) GT 0 then begin
      cmsv_test_set, uval, /set
  endif
  if n_elements(filename0) EQ 0 then filename = 'cmsvtest.sav' $
  else                               filename = strtrim(filename0(0),2)

  print, 'CMSV_TEST - '+rcsid
  print, '---------------------------------------'
  print, systime(0)
  help, !version, /struct
  if n_elements(uval) GT 0 then begin
      print, 'Using user value for middle element:'
      cmsv_test_set, /print
  endif

  print, 'Testing SAVE and RESTORE...'
  cmsv_test_set, a, b, c, d, e
  print, '  SAVEing data...', format='(A,$)'
  catch, catcherr
  if catcherr NE 0 then begin
      catch, /cancel
      print, 'FAILED'
      return
  endif
  save, a, b, c, d, e, file=filename
  catch, /cancel
  print, 'done'

  cmsv_test_unset, a, b, c, d, e
  print, '  RESTOREing data...', format='(A,$)'
  catch, catcherr
  if catcherr NE 0 then begin
      catch, /cancel
      print, 'FAILED'
      return
  endif
  restore, filename
  catch, /cancel
  print, 'done'

  print, '  Comparing results...', format='(A,$)'
  cmsv_test_comp, a, b, c, d, e, result, FAILED
  if result EQ 0 then begin
      print, 'FAILED test '+failed
  endif else begin
      print, 'succeeded'
  endelse

  print, ''
  print, 'Testing CMSAVEDIR'
; print, '/-------------'
  cmsavedir, filename, status=status, errmsg=errmsg, n_var=nvar, $
    var_names=vnames, types=vtypes, /quiet
; print, '\-------------'
  if status EQ 0 then begin
      print, '  ** Failed to read file.  Error message follows:'
      message, errmsg, /info
  endif else begin
      if nvar NE 5 then $
        message, '  ** Incorrect number of variables'
      if vnames(0) NE 'A' OR vnames(1) NE 'B' OR vnames(2) NE 'C' $
        OR vnames(3) NE 'D' OR vnames(4) NE 'E' then $
        message, '  ** Variable names not correct'
      if total(size(a) NE vtypes(*,0)) NE 0 then $
        message, '  ** Variable "A" type incorrect'
      if total(size(b) NE vtypes(*,1)) NE 0 then $
        message, '  ** Variable "B" type incorrect'
      if total(size(c) NE vtypes(*,2)) NE 0 then $
        message, '  ** Variable "C" type incorrect'
      if total(size(d) NE vtypes(*,3)) NE 0 then $
        message, '  ** Variable "D" type incorrect'
      if total(size(e) NE vtypes(*,4)) NE 0 then $
        message, '  ** Variable "E" type incorrect'
      print, '  ** CMSAVEDIR succeeded'
  endelse

  print, ''
  print, 'Testing CMRESTORE'

  cmsv_test_unset, a, b, c, d, e
  print, '  Using IMPLICIT method...', format='(A,$)'
  cmrestore, filename, status=status, errmsg=errmsg
  if status EQ 0 then begin
      print, 'FAILED'
      message, errmsg, /info
  endif else begin
      cmsv_test_comp, a, b, c, d, e, result, failed
      if result EQ 0 then print, 'FAILED test '+failed $
      else print, 'succeeded'
  endelse

  cmsv_test_unset, a, b, c, d, e
  print, '  Using ARG method...', format='(A,$)'
  cmrestore, filename, a, b, c, d, e, status=status, errmsg=errmsg, $
    /quiet
  if status EQ 0 then begin
      print, 'FAILED'
      message, errmsg, /info
  endif else begin
      cmsv_test_comp, a, b, c, d, e, result, failed
      if result EQ 0 then print, 'FAILED test '+failed $
      else print, 'succeeded'
  endelse

  if ver GE 5D then begin
      cmsv_test_unset, a, b, c, d, e
      print, '  Using POINTER method...', format='(A,$)'
      cmrestore, filename, names=['A','B','C','D','E'], $
        status=status, errmsg=errmsg, data=data, pass_meth='POINTER', /quiet
      if status EQ 0 then begin
          print, 'FAILED'
          message, errmsg, /info
      endif else begin
          cmd = ('cmsv_test_comp, *(data(0)), *(data(1)), *(data(2)), '+ $
                 '*(data(3)), *(data(4)), result, failed')
          dummy = execute(cmd)
          if result EQ 0 OR dummy EQ 0 then print, 'FAILED test '+failed $
          else print, 'succeeded'
      endelse
      if total(ptr_valid(data)) GT 0 then $
        ptr_free, data
      
      cmsv_test_unset, a, b, c, d, e
      print, '  Using POINTER method with reversed names...', format='(A,$)'
      cmrestore, filename, names=['E','D','C','B','A'], $
        status=status, errmsg=errmsg, data=data, pass_meth='POINTER', /quiet
      if status EQ 0 then begin
          print, 'FAILED'
          message, errmsg, /info
      endif else begin
          cmd = ('cmsv_test_comp, *(data(4)), *(data(3)), *(data(2)), '+ $
                 '*(data(1)), *(data(0)), result, failed')
          dummy = execute(cmd)
          if result EQ 0 OR dummy EQ 0 then print, 'FAILED test '+failed $
          else print, 'succeeded'
      endelse
      if total(ptr_valid(data)) GT 0 then $
        ptr_free, data
  endif


  cmsv_test_unset, a, b, c, d, e
  print, '  Using STRUCT method...', format='(A,$)'
  cmrestore, filename, names=['A','B','C','D','E'], $
    status=status, errmsg=errmsg, data=data, pass_meth='STRUCT', /quiet
  if status EQ 0 then begin
      print, 'FAILED'
      message, errmsg, /info
  endif else begin
      cmsv_test_comp, data.(0), data.(1), data.(2), data.(3), data.(4), $
        result, failed
      if result EQ 0 then print, 'FAILED test '+failed $
      else print, 'succeeded'
  endelse


  print, ''
  print, 'Testing CMSAVE'
  cmsv_test_set, a, b, c, d, e
  print, '  Using ARG method...', format='(A,$)'
  cmsave, a, b, c, d, e, file=filename, status=status, errmsg=errmsg, $
    /quiet, compat=compat
  if status EQ 0 then begin
      print, 'FAILED'
      message, errmsg, /info
  endif else begin
      restore, filename
      cmsv_test_comp, a, b, c, d, e, result, failed
      if result EQ 0 then print, 'FAILED test '+failed $
      else print, 'succeeded'
  endelse

  if ver GE 5D then begin
      cmsv_test_set, a, b, c, d, e
      print, '  Using POINTER method...', format='(A,$)'
      pheap = [ptr_new(a), ptr_new(b), ptr_new(c), ptr_new(d), ptr_new(e)]
      
      cmsave, file=filename, data=pheap, names=['A','B','C','D','E'], $
        status=status, errmsg=errmsg, /nocatch
      if status EQ 0 then begin
          print, 'FAILED'
          message, errmsg, /info
      endif else begin
          restore, filename
          cmsv_test_comp, a, b, c, d, e, result, failed
          if result EQ 0 then print, 'FAILED test '+failed $
          else print, 'succeeded'
      endelse
      if total(ptr_valid(pheap)) GT 0 then $
        ptr_free, pheap
  endif

  cmsv_test_set, a, b, c, d, e
  print, '  Using UNIT method...', format='(A,$)'
  cmsave, a, b, c, d, e, file=filename, /useunit, $
    status=status, errmsg=errmsg, /quiet, compat=compat
  if status EQ 0 then begin
      print, 'FAILED'
      message, errmsg, /info
  endif else begin
      restore, filename
      cmsv_test_comp, a, b, c, d, e, result, failed
      if result EQ 0 then print, 'FAILED test '+failed $
      else print, 'succeeded'
  endelse

  cmsv_test_set, a, b, c, d, e
  print, '  Using STRUCT method...', format='(A,$)'
  pheap = {a:a, b:b, c:c, d:d, e:e}
  cmsave, file=filename, data=pheap, $
    status=status, errmsg=errmsg, /quiet, compat=compat
  if status EQ 0 then begin
      print, 'FAILED'
      message, errmsg, /info
  endif else begin
      restore, filename
      cmsv_test_comp, a, b, c, d, e, result, failed
      if result EQ 0 then print, 'FAILED test '+failed $
      else print, 'succeeded'
  endelse

  get_lun, unit

  print, ''
  print, 'Testing CMSVREAD'
  cmsv_test_set, a, b, c, d, e
  sz = size(c)
  if sz(sz(0)+1) EQ 10 then $
    print, '  WARNING: data of type POINTER was found; expect failures!!'
  print, '  Saving template file...', format='(A,$)'
  save, a, b, c, d, e, file=filename
  print, 'done'

  openr, unit, filename, error=err
  if err NE 0 then begin
      print, '  ERROR: could not open '+filename
      return
  endif
  
  print, '  Reading file with CMSVREAD...', format='(A,$)'
  cmsvread, unit, a, name='A', /quiet
  cmsvread, unit, b, name='B', /quiet
  cmsvread, unit, c, name='C', /quiet
  cmsvread, unit, d, name='D', /quiet
  cmsvread, unit, e, name='E', /quiet, status=status
  close, unit
  free_lun, unit
  if status NE 1 then print, 'FAILED' $
  else                print, 'done'

  print, '  Checking results...', format='(A,$)'
  cmsv_test_comp, a, b, c, d, e, result, failed
  if result EQ 0 then print, 'FAILED test '+failed $
  else print, 'succeeded'

  print, ''
  print, 'Testing CMSVWRITE'
  openw, unit, filename, error=err, /STREAM   ;; STREAM for VMS
  if err NE 0 then begin
      print, '  ERROR: could not open '+filename
      return
  endif
  print, '  Writing file...', format='(A,$)'
  cmsv_test_set, a, b, c, d, e
  cmsvwrite, unit, a, name='A', /quiet, compat=compat
  cmsvwrite, unit, b, name='B', /quiet, /NO_END  ;; Test this keyword
  cmsvwrite, unit, c, name='C', /quiet, /NO_END
  cmsvwrite, unit, d, name='D', /quiet
  cmsvwrite, unit, e, name='E', /quiet, status=status
  close, unit
  if status NE 1 then print, 'FAILED' $
  else                print, 'done'
  print, '  Checking results...', format='(A,$)'
  cmsv_test_unset, a, b, c, d, e
  restore, filename
  cmsv_test_comp, a, b, c, d, e, result, failed
  if result EQ 0 then print, 'FAILED test '+failed $
  else print, 'succeeded'

end
