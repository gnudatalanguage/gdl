;+
; NAME:
;   CMSVLIB
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Initialize the CMSVLIB save library
;
; CALLING SEQUENCE:
;   VALUE = CMSVLIB(/QUERY)
;   
; DESCRIPTION: 
;
;   This function initializes the CMSVLIB library to read, write and
;   interrogate IDL save files.  Use the QUERY keyword to determine
;   whether the full CMSVLIB library is present.
;
;   The procedures in the library are:
;
;    High-level
;      CMSAVE - save variables to a save file
;      CMRESTORE - restore variables from a save file
;      CMSAVEDIR - list contents of a save file 
;      CMSVLIB (function) - this file
;
;    Mid-level  
;      CMSV_OPEN - open a save file for reading or writing
;      CMSVREAD - read non-pointer data from file
;      CMSVWRITE - write non-pointer data to file
;
;    Low-level
;      CMSV_RREC - read record from save file
;      CMSV_RVTYPE - read variable type information from file
;      CMSV_RDATA - read variable data from file
;      CMSV_WREC - write record to save file
;      CMSV_WVTYPE - write variable type information to file
;      CMSV_WDATA - write variable data to file
;
;    Utility
;      CMSV_RRAW (function) - read raw integer or string data from file
;      CMSV_WRAW - write raw integer or string data to file
;      CMSV_PTRSUM - create a heap data inventory
;      CMSV_TEST - test the library
;      TAGSIZE (function) - determine the types of all tags in a structure
;      HELPFORM (function) - create HELP-like string describing a variable
;
;   Users should consult 
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
;   None
;
; KEYWORDS:
;
;   QUERY - if set, determine whether the CMSVLIB library is
;           installed.  Function returns 1 upon success, 0 upon
;           failure.
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
; $Id: cmsvlib.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-

function cmsvlib, version=version, query=query

  forward_function cmsv_rraw

  catch, catcherr
  if catcherr EQ 0 then cmsv_open, /query
  catch, /cancel
  if catcherr NE 0 then $
    message, 'ERROR: The complete CMSVLIB library must be in your IDL path.'

  version = '1.0'
  return, 1
end
