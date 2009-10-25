;
; AC 18/10/2009
; under GNU/GPL 2 or later
;
pro TEST_GET_SCREEN_SIZE

print, 'basic test (call without Display name)'
taille=GET_SCREEN_SIZE(resolution=resolution)
print, 'Screen Size (in pixels) :', taille
print, 'Pixel Size (in mm) :', resolution

print, 'On current Display, using ":0" as name'
display=':0'
taille=GET_SCREEN_SIZE(display, resolution=resolution)
print, 'Screen Size (in pixels) :', taille
print, 'Pixel Size (in mm) :', resolution

end
