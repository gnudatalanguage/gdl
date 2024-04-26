; LICENCE:
; Copyright (C) 2022: SJT
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   

;+
; TEST_VELOVECT
;	Simple test for VELOVECT. Just runs a selection of cases and
;	describes what should be seen.
;
; Usage:
;	test_velovect
;
; Note:
;	I think that there are issues with the PS device, but
;	it's not clear to me how much is velovect and how much
;	is the general horridness of PS coordinate systems,
;-

pro test_velovect

  vx =  findgen(20, 20)-100

  vy = transpose(vx)/2.

  print, "Simple case X & Y components equal."
  velovect, vx, vx
  print, "All vectors should be at 45 degrees, pointing to lower left"
  print, "for the bottom quarter and top right for the rest."

  a = ''
  read, a, prompt = 'Press Enter to continue...'

  print, "Simple case X & Y components not equal."

  velovect, vx, vy
  print, "Should show an X-type stationary point about (5,5)"

  a = ''
  read, a, prompt = 'Press Enter to continue...'
  
  print, "User defined range."

  velovect, vx, vy, xrange = [-1., 20.], yrange = [-1., 20.], $
            xsty = 1, ysty = 1
  print, "Axes should now run -1 to +20 in each direction."

  a = ''
  read, a, prompt = 'Press Enter to continue...'

  print, "User defined X & Y"

  x = findgen(20)+5.
  y = [findgen(10), findgen(10)+15]
  velovect, vx, vy, x, y

  print, "X should now run from 5 to 24, Y from 0 to 24 with a gap in the middle."

  a = ''
  read, a, prompt = 'Press Enter to continue...'

  print, "With MISSING set."
  velovect, vx, vy, missing = 150.

  print, "Points at the top and also at the bottom right, are omitted."

  a = ''
  read, a, prompt = 'Press Enter to continue...'

  print, "With MISSING and /DOTS set."
  velovect, vx, vy, missing = 150., /dots

  print, "Points at the top and also at the bottom right, are now dots."

  a = ''
  read, a, prompt = 'Press Enter to continue...'

  print, "Subwindows."

  !p.multi = [0, 2, 1]
  velovect, vx, vx, symsize = 4.
  velovect, vx, vx, length = 2.
  !p.multi = 0

  print, "Should have two plots. Should have vectors at 45 degrees."
  print, "Left should have big heads, right: long arrows."

  a = ''
  read, a, prompt = 'Press Enter to continue...'

  print, "Isotropic coordinates"
  velovect, vx, vx, /iso

  print, "Arrows at 45 degress should now be aligned on the diagonals."

  if (!d.flags and 256) ne 0 then begin
                                ; Don't do the window tests if
                                ; the device doesn't support it.
     a = ''
     read, a, prompt = 'Press Enter to continue...'

     print, "Tall window."
     window, xs = 600, ys = 1000
     velovect, vx, vx

     print, "Vector angles should still be 45 degrees."

     a = ''
     read, a, prompt = 'Press Enter to continue...'

     print, "Wide window."
     window, xs = 1200, ys = 600
     velovect, vx, vx

     print, "Vector angles should still be 45 degrees."
  endif

end
