;
; AC 18/10/2009
;
pro TEST_GET_SCREEN_SIZE

taille=GET_SCREEN_SIZE(resolution=resolution)

print, 'Screen Size (in pixels) :', taille
print, 'Pixel Size (in mm) :', resolution

end
