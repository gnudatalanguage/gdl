;
; This code is under GNU GPL v2 or later.
;
; Initial version from Eric Hivon, August 2011
; Revision by Alain Coulais, October 2011
; (special thanks to Sylwester for the work on X/YOFFSET for PS outputs)
;
; Prerequisites:
; - The HealPix library must be in the GDL_PATH/IDL_PATH
; - You must also have AstronLib around ...
;
; Status on October 18, 2011:
; Except the missing DOC_LIBRARY, this code is working,
; numerical values are OK,
; figures in various formats are OK
;
; Please report any troubles or discrepancies between IDL and GDL
; outputs. We have users using the GDL version. Extentions welcome.
;
pro TEST_HEALPIX, test=test
;
; do we have access to the Astron lib ?
; (in fact, they have a local copy of some AstroLib routines
; in the HealPix directories ...)
;
if (EXECUTE('ASTROLIB') EQ 0) then begin
    MESSAGE, /CONTINUE, "Missing Astron. Lib. in your GDL_PATH or IDL_PATH"
    EXIT, status=77
endif
;
; do we have access to the HealPix lib ?
;
if (EXECUTE('INIT_HEALPIX') EQ 0) then begin
    MESSAGE, /CONTINUE, "Missing HealPix lib. in your GDL_PATH or IDL_PATH"
    EXIT, status=77
endif
;
; IDL or GDL ?
;
DEFSYSV, '!GDL', exist=is_it_gdl
if (is_it_gdl EQ 1) then prefixe='GDL_' else prefixe='IDL_'
;
; basic tests
;
nside = 128
TEST_TK, nside, random=0.5
;
; --- only on screen now ---
; plotting c2l
;
FITS2CL, clw1, /wmap1, /show
;
; --- now maps, on screen and in PNG or PS files ---
;
; display a CMB map
;
MOLLVIEW, !healpix.path.test+'map.fits', title='CMB', png=prefixe+'cmbmap.png'
;
preview=1
map=FINDGEN(12)
INIT_HEALPIX
map[2]=!healpix.bad_value
;
MOLLVIEW, map, /grat,/igra,coord=['g','c'],rot=[10,20,30], png=prefixe+'grat.png'
ORTHVIEW, map, /shade, png=prefixe+'shade.png',preview=preview
MOLLVIEW, map, transp=3, png=prefixe+'map.png',preview=preview
;
noise = RANDOMN(seed,12,3)
MOLLVIEW, noise, /true, png=prefixe+'noise_g.png',preview=preview
MOLLVIEW, noise, ps=prefixe+'noise_g.ps', preview=preview
;
; can we display the in line documentation ?
;
GNOMVIEW, /help
;
if KEYWORD_SET(test) then STOP
;
end

