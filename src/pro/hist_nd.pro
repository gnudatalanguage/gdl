;+
; NAME:
;       HIST_ND
;
; PURPOSE:
;
;       Perform an N-dimensional histogram, also known as the joint
;       density function of N variables, ala HIST_2D.
;
; CALLING SEQUENCE:
;       hist=HIST_ND(V,[BINSIZE,MIN=,MAX=,NBINS=,REVERSE_INDICES=])
;
; INPUTS:
;
;       V: A NxP array representing P data points in N dimensions.  
;
;       BINSIZE: The size of the bin to use. Either an N point vector
;         specifying a separate size for each dimension, or a scalar,
;         which will be used for all dimensions.  If BINSIZE is not
;         passed, NBINS must be.
;
; OPTIONAL INPUTS: 
;
;       MIN: The minimum value for the histogram.  Either a P point
;         vector specifying a separate minimum for each dimension, or
;         a scalar, which will be used for all dimensions.  If
;         omitted, the natural minimum within the dataset will be
;         used.
;
;       MAX: The maximum value for the histogram.  Either a P point
;         vector specifying a separate maximmum for each dimension, or
;         a scalar, which will be used for all dimensions. If omitted,
;         the natural maximum within the dataset will be used.
;
;       NBINS: Rather than specifying the binsize, you can pass NBINS,
;         the number of bins in each dimension, which can be a P point
;         vector, or a scalar.  If BINSIZE it also passed, NBINS will
;         be ignored, otherwise BINSIZE will then be calculated as
;         binsize=(max-min)/nbins.  Note that *unlike* RSI's version
;         of histogram as of IDL 5.4, this keyword actually works as
;         advertised, giving you NBINS bins over the range min to max.
;
; KEYWORD PARAMETERS:
;       
;       MIN,MAX,NBINS: See above
;       
;       REVERSE_INDICES: Set to a named variable to receive the
;         reverse indices, for mapping which points occurred in a
;         given bin.  Note that this is a 1-dimensional reverse index
;         vector (see HISTOGRAM).  E.g., to find the indices of points
;         which fell in a histogram bin [i,j,k], look up:
;
;             ind=[i+nx*(j+ny*k)]
;             ri[ri[ind]:ri[ind+1]-1]
;
;         See also ARRAY_INDICES for converting in the other
;         direction.
;
; OUTPUTS:
;
;       hist: The N-Dimensional histogram, an array of size
;         N1xN2xN3x...xND where the Ni's are the number of bins
;         implied by the data, and/or the optional inputs min, max and
;         binsize.
;
; OPTIONAL OUTPUTS:
;
;       The reverse indices.
;
; EXAMPLE:
;       
;       v=randomu(sd,3,100)
;       h=hist_nd(v,.25,MIN=0,MAX=1,REVERSE_INDICES=ri)
;
; SEE ALSO:
;
;       HISTOGRAM, HIST_2D
;
; MODIFICATION HISTORY:
;
;       Mon Mar 5 09:45:53 2007, J.D. Smith <jdsmith@as.arizona.edu>
;
;               Correctly trim out of range elements from the
;               histogram, when MIN/MAX are specified. Requires IDL
;               v6.1 or later.
;
;       Tue Aug 19 09:13:43 2003, J.D. Smith <jdsmith@as.arizona.edu>
;
;               Slight update to BINSIZE logic to provide consistency
;               with HIST_2D.
;
;       Fri Oct 11 10:10:01 2002, J.D. Smith <jdsmith@as.arizona.edu>
;
;               Updated to use new DIMENSION keyword to MAX/MIN.
;
;       Fri Apr 20 12:57:34 2001, JD Smith <jdsmith@astro.cornell.edu>
;
;               Slight update to NBINS logic.  More aggressive keyword
;               checking.
;
;       Wed Mar 28 19:41:10 2001, JD Smith <jdsmith@astro.cornell.edu>
;
;               Written, based on HIST_2D, and suggestions of CM.
;
;-
;##############################################################################
;
; LICENSE
;
;  Copyright (C) 2001-2003, 2004, 2007 J.D. Smith
;
;  This file is free software; you can redistribute it and/or modify
;  it under the terms of the GNU General Public License as published
;  by the Free Software Foundation; either version 2, or (at your
;  option) any later version.
;
;  This file is distributed in the hope that it will be useful, but
;  WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;  General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with this file; see the file COPYING.  If not, write to the
;  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
;  Boston, MA 02110-1301, USA.
;
;##############################################################################

function hist_nd,V,bs,MIN=mn,MAX=mx,NBINS=nbins,REVERSE_INDICES=ri
  s=size(V,/DIMENSIONS)
  if n_elements(s) ne 2 then message,'Input must be N (dimensions) x P (points)'
  if s[0] gt 8 then message, 'Only up to 8 dimensions allowed'
  
  imx=max(V,DIMENSION=2,MIN=imn)
  
  if n_elements(mx) eq 0 then mx=imx
  if n_elements(mn) eq 0 then mn=imn
  
  if s[0] gt 1 then begin 
     if n_elements(mn)    eq 1 then mn=replicate(mn,s[0])
     if n_elements(mx)    eq 1 then mx=replicate(mx,s[0])
     if n_elements(bs)    eq 1 then bs=replicate(bs,s[0])
     if n_elements(nbins) eq 1 then nbins=replicate(nbins,s[0])
  endif 
  
  if ~array_equal(mn le mx,1b) then $
     message,'Min must be less than or equal to max.'
  
  if n_elements(bs) eq 0 then begin 
     if n_elements(nbins) ne 0 then begin 
        nbins=long(nbins)       ;No fractional bins, please
        bs=float(mx-mn)/nbins   ;a correct formulation
     endif else message,'Must pass either binsize or NBINS'
  endif else nbins=long((mx-mn)/bs+1) 
  
  total_bins=product(nbins,/PRESERVE_TYPE) ;Total number of bins
  h=long((V[s[0]-1,*]-mn[s[0]-1])/bs[s[0]-1])
  ;; The scaled indices, s[n]+N[n-1]*(s[n-1]+N[n-2]*(s[n-2]+...
  for i=s[0]-2,0,-1 do h=nbins[i]*temporary(h) + long((V[i,*]-mn[i])/bs[i])
  
  out_of_range=[~array_equal(mn le imn,1b),~array_equal(mx ge imx,1b)]
  if ~array_equal(out_of_range,0b) then begin 
     in_range=1
     if out_of_range[0] then $  ;out of range low
        in_range=total(V ge rebin(mn,s,/SAMP),1,/PRESERVE_TYPE) eq s[0]
     if out_of_range[1] then $  ;out of range high
        in_range AND= total(V le rebin(mx,s,/SAMP),1,/PRESERVE_TYPE) eq s[0]
     h=(temporary(h) + 1L)*temporary(in_range) - 1L
  endif 

  ret=make_array(TYPE=3,DIMENSION=nbins,/NOZERO)
  if arg_present(ri) then $
     ret[0]=histogram(h,MIN=0L,MAX=total_bins-1L,REVERSE_INDICES=ri) $
  else $
     ret[0]=histogram(h,MIN=0L,MAX=total_bins-1L)
  return,ret
end
