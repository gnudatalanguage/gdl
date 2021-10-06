;+
;
; NAME: READ_PPM
;
; PURPOSE: Reads a PPM or PGM file into memory
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE:
;       READ_PPM, filename, image [,maxval=maxval]
;
; KEYWORD PARAMETERS: 
;	MAXVAL: A variable to return the MAX value in the image.
;
; OUTPUTS: always a true color image, so data is a three dimensional array
; with pixel interleaving. 
;
; OPTIONAL OUTPUTS: 
;
; RESTRICTIONS:
;         Requires ImageMagick (tests added)
;
; PROCEDURE:
;         Use ImageMagick to read the data as requested
;
; EXAMPLE:
;         See "test_read_standard_images.pro" in testsuite/
;
; MODIFICATION HISTORY:
;	Quickly hacked from READ_PNG: James Tappin. 28/9/21.
;
;-
; LICENCE:
; Copyright (C) 2004, 2011, 2012, 2014
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
; ----------------------------- Procedure ------------------------
;
pro READ_PPM, filename, image, maxval = maxval
;

compile_opt hidden, idl2
ON_ERROR, 2

if ~MAGICK_PING(filename, 'PPM') && ~MAGICK_PING(filename, 'PGM') then  MESSAGE, "File "+filename+" is not a PGM/PPM file."
READ_ANYGRAPHICSFILEWITHMAGICK, filename, image, colortable, /order ; IDL flips, go figure
maxval = max(image)
end
