;+
; NAME:
;   CMSV_RVTYPE
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Read variable type information from input block or file unit
;
; CALLING SEQUENCE:
;   CMSV_RVTYPE, BLOCK, POINTER, NAME, SIZE, UNIT=UNIT, $
;          TEMPLATE=TEMPLATE, SUFFIX=SUFFIX, $
;          /NO_CREATE, /NO_TYPE, /HEAP, /SYSTEM, $
;          STRUCTURE_NAME=STNAME, $
;          NAMED_STRUCTS=STRUCTS, NAMED_CLASSES=CLASSES, $
;          OFFSET=OFFSET, STATUS=STATUS, ERRMSG=ERRMSG
;   
; DESCRIPTION: 
;
;   CMSV_RVTYPE reads the type portion of an IDL SAVE variable record.
;   An IDL variable is stored in two components: the type descriptor
;   which describes the name, type, and dimensions of the variable;
;   and the data record, which contains the raw data of the variable.
;   This procedure reads the type descriptor returns it to the user.
;   This procedure can also determine the name of a variable; the heap
;   index number of a heap variable; and other important information.
;   Once the type of the data has been determined, the data portion
;   can be read using the CMSV_RDATA procedure.
;
;   CMSV_RVTYPE should recognize and correctly return type descriptor
;   information about all known IDL data types, as of this writing.
;   It should be noted that CMSV_RDATA will not necessarily be capable
;   of reading all of these data types, but the description of the
;   data should still be readable.  Users can then use this
;   information to print a summary of the file contents for example.
;
;   The type information are normally returned in the SIZE parameter,
;   which gives the IDL variable type, and the dimensions of the
;   variable just as the IDL built-in function SIZE() would do.
;   However, in the case of structures, there is much more information
;   to convey.  To assist the user a blank template structure is
;   returned in the keyword parameter TEMPLATE, which they can then
;   pass on to CMSV_RDATA.
;
;   Users should be aware of structure and class name clashes.  The
;   problem arises because CMSV_RVTYPE must actually instantiate any
;   named structures or classes in the file.  If these named
;   structures clash with the definitions of the structures on the
;   user's local computer, then an error will result.  To prevent
;   this, the user can use the SUFFIX keyword.  If the SUFFIX keyword
;   contains a string, then this string is appended to any structure
;   names discovered in the save file, before instantiation.  Thus, as
;   long as the suffix is a unique string, there will be no clashes
;   with local structure definitions.  Users are advised to pick a
;   different suffix for *each* save file that they open.
;
;   CMSV_RVTYPE also provides some diagnostic information about the
;   variable.  Users can pass the NAMED_STRUCTS and NAMED_CLASSES
;   keywords in order to discover what named structures, classes and
;   superclasses are stored in the save file (this is especially
;   useful with the NO_TYPE keyword).  Since the contents of
;   NAMED_STRUCTS and NAMED_CLASSES are not destroyed, but appended to
;   instead, users are advised to clear these variables when opening
;   each new file.
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
;   NAME - upon return, the identifier of the variable.  For named
;          variables, NAME is a string.  For heap variables, NAME is
;          an integer heap index.
;
;   SIZE - upon return, an integers array describing the variable type
;          and size, in the same format as returned by the built-in
;          function SIZE.
;
; KEYWORDS:
;
;   TEMPLATE - upon return, if the variable is a structure, TEMPLATE
;              will contain a single blank template structure, which
;              can be used in calls to CMSV_RDATA.
;
;   NO_CREATE - if set, then do not create any template structures
;               (saves execution time and structure name clashes).
;
;   NO_TYPE - if set, do not read type information.  CMSV_RVTYPE
;             returns only the variable NAME or heap index.
;
;   HEAP - if set, then read the variable type assuming it is a heap
;          variable (a HEAP_DATA record).
;
;   SYSTEM - if set, then read the variable type assuming it is a
;            system variable (a SYSTEM_VARIABLE record).
;
;   SUFFIX - a scalar string, the suffix to be appended to any
;            structure names to force them to be unique.
;
;   STRUCTURE_NAME - upon return, if the variable is a named
;                    structure, STRUCTURE_NAME will contain the name
;                    of the structure as a string.
;
;   NAMED_CLASSES /
;   NAMED_STRUCTS - upon input, this keyword should contain an
;                   undefined or a string array value.
;
;                   If the variable contains any named structure/class
;                   definitions, CMSV_RVTYPE will append those names
;                   upon return, including any enclosed structures or
;                   superclasses.
;
;   UNIT - a file unit.  If a library routine reads to the end of
;          BLOCK, or if BLOCK is undefined, then this file UNIT will
;          be accessed for more data.  If undefined, then BLOCK must
;          contain the entire file in memory.
;
;   OFFSET - the file offset of byte zero of BLOCK.  Default: 0
;            (OFFSET is not used by this routine at this time)
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
; $Id: cmsv_rvtype.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-
forward_function cmsv_rraw

pro cmsv_rarrdesc, block, pointer, sz, status=status, unit=unit, errmsg=errmsg
  ;; ARRAY_DESC
  ;;   LONG - START_TOKEN - value 08 - array descriptor flag
  ;;   LONG - unknown (value 2)
  ;;   LONG - number of bytes in value
  ;;   LONG - number of elements in value
  ;;   LONG - number of variable dimensions
  ;;   LONGx2 - unknown (value 0,0)
  ;;   LONG - maximum number of stored dimensions ( = 8 )
  ;;   LONGx8 - dimensions of number
  if n_elements(pointer) EQ 0 then pointer = 0L
  buff = cmsv_rraw(/long, block, pointer, 16, unit=unit, $
                   status=status, errmsg=errmsg)
  if status EQ 0 then return
  if buff(0) NE 8 then begin
      status = 0
      errmsg = 'ERROR: CMSV_RVTYPE: invalid array type descriptor'
      return
  endif

  ndims = buff(4)
  nelt = buff(3)
  dims = buff(8:8+ndims-1)
  vartype = 0

  sz = [ndims, dims, vartype, nelt]
  return
end

pro cmsv_rstructdesc, block, pointer, template, unit=unit, $
                      status=status, errmsg=errmsg, $
                      suffix=suffix, structure_name=stname, no_create=nocreate, $
                      named_structs=structs, named_classes=classes
  ;; STRUCT_DESCR
  ;;   LONG - START_TOKEN - value 9 - struct descriptor flag
  ;;   STRING - name of struct (or 0 if anonymous)
  ;;   LONG - PREDEF - 0 if struct def'n follows, 1 if already defined
  ;;   LONG - N_TAGS - number of structure tags
  ;;   LONG - total "length" in bytes, but nothing meaningful
  ;;   TAGDESCxN_TAGS - TAG_TABLE - description of each tag
  ;;   STRINGxN_TAGS - TAG_NAMES - name of each tag
  ;;   ARRAY_DESCxN_ARRAYS - ARRAY_TAB - descriptor for each array
  ;;   STRUCT_DESCRxN_STRUCTS - STRUCT_TAB - descriptor for each struct

  template = 0 & dummy = temporary(template)

  if n_elements(pointer) EQ 0 then pointer = 0L
  start_token = cmsv_rraw(/long, block, pointer, unit=unit, $
                          status=status, errmsg=errmsg)
  ;; message, 'ERROR: CMSV_RVTYPE: structure descriptor was not found'
  if status EQ 0 OR start_token NE 9 then return
  struct_name = cmsv_rraw(/string, block, pointer, unit=unit, $
                          status=status, errmsg=errmsg)
  if status EQ 0 then return
  buff = cmsv_rraw(/long, block, pointer, 3, unit=unit, $
                   status=status, errmsg=errmsg)
  if status EQ 0 then return

  status = 0
  nt = buff(1)
  if nt LE 0 then begin
      errmsg = 'ERROR: CMSV_RVTYPE: number of structure tags was <= 0'
      return
  endif
  predefined = (buff(0) AND 1) NE 0  ;; Structure type has already been def'd
  inherits_  = (buff(0) AND 2) NE 0  ;; This is a class, inherits from a super
  is_super   = (buff(0) AND 4) NE 0  ;; This is a superclass

  ;; Structure has already been defined
  if predefined then begin
      stname = struct_name
      errmsg = ''
      status = 1
      if NOT keyword_set(nocreate) then begin
          status = execute('template = {'+stname+'}')
          if status EQ 0 then $
            errmsg = ('ERROR: CMSV_RVTYPE: could not create named structure '+$
                      struct_name)
      endif
      return
  endif

  ;; Read tag_table - type descriptions of each tag
  ;; LONG - offset
  ;; LONG - IDL variable type
  ;; LONG - type flags '20'x: struct, '14'x: array
  tag_table = cmsv_rraw(/long, block, pointer, 3*nt, unit=unit, $
                        status=status, errmsg=errmsg)
  if status EQ 0 then return
  tag_table = reform(tag_table, 3, nt, /overwrite)

  ;; Read tag names
  ;; STRING
  tag_names1 = strarr(nt)
  for i = 0L, nt-1 do begin
      tag_names1(i) = cmsv_rraw(/string, block, pointer, unit=unit, $
                                status=status, errmsg=errmsg)
      if status EQ 0 then return
  endfor

  ;; Read array descriptors - compose a list of SIZE types
  ssz = lonarr(12, nt)
  for i = 0L, nt-1 do begin
      if (tag_table(2,i) AND '24'x) NE 0 then begin
          cmsv_rarrdesc, block, pointer, sz, unit=unit, $
            status=status, errmsg=errmsg
          if status EQ 0 then return
      endif else begin
          sz = [0L, 0, 1L]
      endelse
      sz(sz(0)+1) = tag_table(1,i) ;; Insert the variable type
      ssz(0,i) = sz
  endfor

  ;; Read struct descriptors - compose the resulting struct template
  for i = 0L, nt-1 do begin
      sz = reform(ssz(*,i))

      ;; Recursively read struct descriptor
      if (tag_table(2,i) AND '20'x) NE 0 then begin
          cmsv_rstructdesc, block, pointer, tp, unit=unit, $
            status=status, errmsg=errmsg, suffix=suffix, $
            no_create=no_create, named_structs=structs, named_classes=classes
          if status EQ 0 then tp = 0L
      endif else begin
          forward_function make_array
          tp = make_array(1, type=sz(sz(0)+1))
          tp = tp(0)
      endelse

      ;; Use array descriptor to compose array if needed
      if (tag_table(2,i) AND '04'x) NE 0 then begin
          ndims = sz(0)
          nelt = sz(ndims+2)
          dims = sz(1:ndims)
          tp = replicate(tp, nelt)
          tp = reform(tp, dims, /overwrite)
      endif

      ;; Add this tag element to the structure
      if tag_names1(i) NE '' AND NOT keyword_set(nocreate) then begin
          if n_elements(ss) EQ 0 then $
            ss = create_struct(tag_names1(i), tp) $
          else $
            ss = create_struct(ss, tag_names1(i), tp)
      endif
      stname = struct_name
  endfor

  ;; Read class information
  ;; STRING - name of class
  ;; LONG - number of super classes = N_SUP
  ;; STRINGxN_SUP - names of super classes
  ;; {structdesc}xN_SUP - structures of each super class
  if inherits_ OR is_super then begin
      classname = cmsv_rraw(/string, block, pointer, unit=unit, $
                            status=status, errmsg=errmsg)
      if classname EQ '' then begin
          status = 0
          errmsg = 'ERROR: CMSV_RVTYPE: invalid class name'
          return
      endif
      if status EQ 0 then return
      n_sup = cmsv_rraw(/long, block, pointer, unit=unit, $
                        status=status, errmsg=errmsg)
      if status EQ 0 then return
      
      if n_sup GT 0 then begin
          supnames = cmsv_rraw(/string, block, pointer, n_sup, unit=unit, $
                               status=status, errmsg=errmsg)
          if status EQ 0 then return

          ;; Read sub-structures, but do not create them, we have to
          ;; do this because otherwise we lose synchronization.
          for i = 0, n_sup-1 do begin
              
              cmsv_rstructdesc, block, pointer, tp, unit=unit, $
                status=status, errmsg=errmsg, suffix=suffix, $
                no_create=no_create, named_structs=structs, named_classes=classes
          endfor
      endif

      ;; Set output variables.  First, the name of this structure
      stname = classname

      ;; Second, the name of this class is placed in the class name
      ;; inventory.
      if n_elements(classes) EQ 0 then $
        classes = [classname] $
      else $
        classes = [classes, classname]

      status = 1
      return
  endif

  ;; Make it a named structure if it has a name; otherwise it's
  ;; anonymous
  if stname NE '' AND NOT keyword_set(nocreate) then begin
      catch, catcherr
      if catcherr NE 0 then begin
          catch, /cancel
          status = 0
          errmsg = 'ERROR: CMSV_RVTYPE: conflict between memory and '+ $
            'saved named structures'
      endif else begin
          if n_elements(suffix) EQ 0 then sf = '' $
          else                            sf = strtrim(suffix(0),2)
          s1 = create_struct(ss, name=struct_name+sf)
          catch, /cancel
          
          ss = temporary(s1)
      endelse
  endif
  
  ;; Add this structure name to the inventory list
  if stname NE '' then begin
      if n_elements(structs) EQ 0 then $
        structs = [stname] $
      else $
        structs = [structs, stname]
  endif

  status = 1
  if keyword_set(nocreate) then return
  if n_elements(ss) GT 0 then begin
      template = ss
      status = 1
  endif else begin
      status = 0
      errmsg = 'ERROR: CMSV_RVTYPE: could not decode structure descriptor'
  endelse

  return
end

pro cmsv_rvtype, block, pointer, result, sz, unit=unit, offset=offset, $
                 status=status, errmsg=errmsg, $
                 template=template1, no_create=nocreate, no_type=notype, $
                 heap=heap, system=system, $
                 suffix=suffix, structure_name=stname, $
                 named_structs=structs, named_classes=classes

  ;; VARIABLE BLOCK
  ;;   STRING - VARNAME - name of variable
  ;;   TYPEDESC - type descriptor

  ;; HEAP VARIABLE
  ;;   LONG - HEAP_INDEX - heap index of data value
  ;;   LONG - value 0x02 or 0x12 - unknown
  ;;   TYPEDESC - type descriptor
  status = 0
  size = [0L, 0, 1]
  if n_elements(pointer) EQ 0 then pointer = 0L
  result = 0 & dummy = temporary(result)

  if keyword_set(heap) then begin
      ;; Read heap index
      varname = cmsv_rraw(/long, block, pointer, 2, unit=unit, $
                          status=status, errmsg=errmsg)
      varname = varname(0)
      if status EQ 0 then return
  endif else begin
      ;; Read variable name
      varname = cmsv_rraw(/string, block, pointer, unit=unit, $
                          status=status, errmsg=errmsg)
      if status EQ 0 then return
  endelse

  if n_elements(classes) GT 0 then begin
      sz1 = size(classes)
      if sz1(sz1(0)+1) NE 7 then begin
          errmsg = 'ERROR: CMSV_RVTYPE: NAMED_CLASSES must be an array '+ $
            'of strings'
          status = 0
          return
      endif
  endif

  if n_elements(structs) GT 0 then begin
      sz1 = size(structs)
      if sz1(sz1(0)+1) NE 7 then begin
          errmsg = 'ERROR: CMSV_RVTYPE: NAMED_STRUCTS must be an array '+ $
            'of strings'
          status = 0
          return
      endif
  endif

  result = varname
  status = 1
  if keyword_set(notype) then return

  ;; Read variable type information.  Here is a summary of the
  ;; formatting of the descriptors.

  ;; SCALAR_TYPE
  ;;   LONG - variable type (IDL type code)
  ;;   LONG - value 0

  ;; ARRAY_TYPE
  ;;   LONG - variable type (IDL type code)
  ;;   LONG - VARFLAGS - bitwise OR of following:
  ;;                      '02'xb - system variable
  ;;                      '04'xb - array
  ;;                      '10'xb - don't know, it's kind of random
  ;;                      '20'xb - structure
  ;;   ARRAY_DESC - array descriptor
  ;;   STRUCT_DESC (if a structure) - structure descriptor

  ;; IDL variable type
  vartype = cmsv_rraw(/long, block, pointer, unit=unit, $
                      status=status, errmsg=errmsg)
  if status EQ 0 then return
  ;; Array type flags
  varflags = cmsv_rraw(/long, block, pointer, unit=unit, $
                       status=status, errmsg=errmsg)
  if status EQ 0 then return

  ;; System variable
  if keyword_set(system) then begin
      dummy = cmsv_rraw(/long, block, pointer, 2, unit=unit, $
                        status=status, errmsg=errmsg)
      if status EQ 0 then return
      if (dummy(1) AND '02'xl) EQ 0 then begin
          errmsg = 'ERROR: CMSV_RVTYPE: system variable type mismatch'
          status = 0
          return
      endif
      if (dummy(1) AND 'ed'xl) NE (varflags AND 'ed'xl) then begin
          errmsg = 'ERROR: CMSV_RVTYPE: variable type mismatch'
          status = 0
          return
      endif
  endif

  ;; Scalar type
  if (varflags(0) AND '24'x) EQ 0 then begin     
      sz = [0L, vartype, 1]
      return
  endif

  ;; Complex array type
  struct = (varflags(0) AND '20'x) NE 0 
  cmsv_rarrdesc, block, pointer, sz, unit=unit, status=status, errmsg=errmsg
  if status EQ 0 then return
  if keyword_set(struct) then begin
      cmsv_rstructdesc, block, pointer, template1, unit=unit, $
        status=status, errmsg=errmsg, no_create=nocreate, $
        suffix=suffix, structure_name=stname, $
        named_structs=structs, named_classes=classes
  endif
  if status EQ 0 then return

  sz(sz(0)+1) = vartype

  return
end
