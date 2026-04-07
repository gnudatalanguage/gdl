;
; Orignal version from Gilles, Oct 2022
;
; -------------------------------------------
; Modifications history :
;
; AC on 2026-03-24
;  * Cosmetic changes
;  * really managing 3D images input (COLOR_CONVERT, input, output)
;  * change in RGBtoHSV to pass the test ...
; 
; -------------------------------------------
;
function hueToRgb, t1, t2, hue
if (hue le 0) then hue += 6
if (hue ge 6) then hue -= 6
if (hue lt 1) then return, (t2 - t1) * hue + t1
if (hue lt 3) then return, t2
if (hue lt 4) then return, (t2 - t1) * (4 - hue) + t1 
return, t1
end

pro hlsToRgb, hue, light, sat, r, g, b
hue = FLOAT(hue)
light=FLOAT(light)
sat=FLOAT(sat)
hue = hue / 60.
if ( light le 0.5 ) then begin
    t2 = light * (sat + 1)
endif else begin
    t2 = light + sat - (light * sat)
endelse
t1 = light * 2 - t2
r = hueToRgb(t1, t2, hue + 2) * 255
g = hueToRgb(t1, t2, hue) * 255
b = hueToRgb(t1, t2, hue - 2) * 255
r=BYTE(CEIL(r))
g=BYTE(CEIL(g))
b=BYTE(CEIL(b))
end

pro rgbToHsl, r, g, b, h, l, s
rgb = [ r / 255., g / 255., b / 255.]
min = rgb[0]
max = rgb[0]
maxcolor = 0
for i = 0,2 do begin
    if (rgb[i] le min) then min = rgb[i]
    if (rgb[i] ge max) then begin
        max = rgb[i]
        maxcolor = i
    endif
endfor

if (maxcolor eq 0) then  h = (rgb[1] - rgb[2]) / (max - min)
if (maxcolor eq 1) then  h = 2 + (rgb[2] - rgb[0]) / (max - min)
if (maxcolor eq 2) then  h = 4 + (rgb[0] - rgb[1]) / (max - min)
if (~FINITE(h)) then h = 0
h = h * 60.  
if (h lt 0) then h = h + 360.
l = (min + max) / 2.
if (min eq max) then begin
    s = 0
endif else begin
    if (l le 0.5) then begin
        s = (max - min) / (max + min)
    endif else begin
        s = (max - min) / (2. - max - min) 
    endelse
endelse
end

pro RGBtoHSV,r,g,b,h,s,v
; AC 2026
h=0. & s=0. & v=0.
;
r=BYTE(r)/255.
g=BYTE(g)/255.
b=BYTE(b)/255.
rgb=[r,g,b]
min = MIN( rgb, max=max )
v = max
delta = max-min
;
if( max ne 0 ) then begin
    s = delta / max
endif else begin
    s = 0.
    ;; h = -1
    ;; AC 2026
    h=0.
    return
endelse
;
;stop
if ( r eq max ) then h = ( g - b ) / delta else if ( g eq max ) then h = 2 + ( b - r ) / delta else h = 4 + ( r - g ) / delta
  h *= 60 ;; degrees
  if( h lt 0 ) then h += 360
end

pro HSVtoRGB,h,s,v,r,g,b
if ( s eq 0 ) then begin      ;; achromatic (grey)
    r = BYTE(v*255.)
    g = r
    b = r
    return
 endif
s=FLOAT(s)
v=FLOAT(v)
h=FLOAT(h)
h /= 60.                       ;;			// sector 0 to 5
i = floor( h )                 ;;
f = h - i                      ;;			// factorial part of h
p = v * ( 1 - s )              ;;
q = v * ( 1 - s * f )          ;;
t = v * ( 1 - s * ( 1 - f ) )  ;;

switch i OF
    0: begin
        r = v
        g = t
        b = p
        break
    end
    1: begin
        r = q
        g = v
        b = p
        break
    end
    2: begin
        r = p
        g = v
        b = t
        break
    end
    3: begin
        r = p
        g = q
        b = v
        break
    end
    4: begin
        r = t
        g = p
        b = v
        break
    end
    else : begin
        r = v
        g = p
        b = q
        break
    end
endswitch
r=BYTE(r*255.)
g=BYTE(g*255.)
b=BYTE(b*255.)
end
;  
PRO COLOR_CONVERT_SINGLE, i0, i1, i2, o0, o1, o2, $
                          hls_rgb =hls_rgb, hsv_rgb=hsv_rgb, $
                          rgb_hls=rgb_hls, rgb_hsv=rgb_hsv, $
                          rgb_ycbcr=rgb_ycbcr, rgb_yiq=rgb_yiq, $
                          rgb_ypbpr=rgb_ypbpr, rgb_yuv=rgb_yuv, $
                          ycbcr_rgb=ycbcr_rgb, yiq_rgb=yiq_rgb, $
                          ypbpr_rgb=ypbpr_rgb, yuv_rgb=yuv_rgb
;;
if (KEYWORD_SET(rgb_hls)) then begin
    r=BYTE(i0)
    g=BYTE(i1)
    b=BYTE(i2)
    rgbToHsl, r, g, b, o0, o1, o2
endif

if (KEYWORD_SET(hls_rgb)) then begin
    HlsToRgb, i0, i1, i2, o0, o1, o2
endif

if (KEYWORD_SET(rgb_hsv)) then begin
    r=BYTE(i0)
    g=BYTE(i1)
    b=BYTE(i2)
    rgbToHsv, r, g, b, o0, o1, o2
endif

if (KEYWORD_SET(hsv_rgb)) then begin
    HsvToRgb, i0, i1, i2, o0, o1, o2
endif

if (KEYWORD_SET(rgb_yiq)) then begin
    rgbtoyiq=[[0.299,     0.587,       0.114],$
              [0.596, -0.274, -0.322],$
              [0.211, -0.523, 0.312]]
    r=BYTE(i0)
    g=BYTE(i1)
    b=BYTE(i2)
    i=[r/255.,g/255.,b/255.]
    o=i#rgbtoyiq
    o0=o[0]
    o1=o[1]
    o2=o[2]
endif

if (KEYWORD_SET(yiq_rgb)) then begin
    yiqtorgb=[[1, 0.956,   0.619],$
              [ 1, -0.272,  -0.647],$
              [ 1, -1.106, 1.703]]
    i=[i0,i1,i2]
    o=BYTE(CEIL(i#yiqtorgb*255.))
    o0=o[0]
    o1=o[1]
    o2=o[2]
endif

if (KEYWORD_SET(rgb_yuv)) then begin
    R=i0/255.
    G=i1/255.
    B=i2/255.
    Y = 0.299 * R + 0.587 * G + 0.114 * B
    U = 0.492*(B-Y)
    V = 0.877*(R-Y)
    o0=Y
    o1=U
    o2=V
endif

if (KEYWORD_SET(yuv_rgb)) then begin
    R = i0 + 1.140*i2
    G = i0 - 0.395*i1 - 0.581*i2
    B = i0 + 2.032*i1
    o0=BYTE(CEIL(r*255.))
    o1=BYTE(CEIL(g*255.))
    o2=BYTE(CEIL(b*255.))
endif

if (KEYWORD_SET(rgb_ycbcr)) then begin
    R=BYTE(i0)
    G=BYTE(i1)
    B=BYTE(i2)
    i=[r,g,b]
    ;; not full range see https://web.archive.org/web/20180421030430/http://www.equasys.de/colorconversion.html
    matrix=[[0.257,0.504,0.098],[-0.148,-0.291,0.439],[0.439,-0.368,-0.071]]
    o=[16,128,128]+i#matrix
    o0=(BYTE(o[0])>16)<235
    o1=(BYTE(o[1])>16)<240
    o2=(BYTE(o[2])>16)<240
endif

if (KEYWORD_SET(ycbcr_rgb)) then begin
    i0=(BYTE(i0)>16)<235
    i1=(BYTE(i1)>16)<240
    i2=(BYTE(i2)>16)<240
    ;; not full range see https://web.archive.org/web/20180421030430/http://www.equasys.de/colorconversion.html
    matrix=[[1.164,0,1.596],[1.164,-0.392,-0.813],[1.164,2.017,0]]
    i=[i0,i1,i2]-[16,128,128]
    o=i#matrix
    o0=BYTE(o[0])
    o1=BYTE(o[1])
    o2=BYTE(o[2])
endif

if (KEYWORD_SET(rgb_ypbpr)) then begin
    R=BYTE(i0)/255.
    G=BYTE(i1)/255.
    B=BYTE(i2)/255.
    i=[r,g,b]
    ;; not full range see https://web.archive.org/web/20180421030430/http://www.equasys.de/colorconversion.html
    matrix=[[0.299,0.587,0.114],[-0.169,-0.331,0.5],[0.5,-0.419,-0.081]]
    o=i#matrix
    o0=o[0]
    o1=o[1]
    o2=o[2]
endif

if (KEYWORD_SET(ypbpr_rgb)) then begin
    ;; not full range see https://web.archive.org/web/20180421030430/http://www.equasys.de/colorconversion.html
    matrix=[[1.0,0,1.402],[1.0,-0.344,-0.714],[1.0,1.722,0]]
    i=[i0,i1,i2]
    o=i#matrix
    o0=BYTE(o[0]*255.)
    o1=BYTE(o[1]*255.)
    o2=BYTE(o[2]*255.)
endif

END

pro COLOR_CONVERT_MULTIPLE, n, i0, i1, i2, o0, o1, o2, $
                            hls_rgb=hls_rgb, hsv_rgb=hsv_rgb, rgb_hls=rgb_hls, $
                            rgb_hsv=rgb_hsv, rgb_ycbcr=rgb_ycbcr, rgb_yiq=rgb_yiq, $
                            rgb_ypbpr=rgb_ypbpr, rgb_yuv=rgb_yuv, ycbcr_rgb=ycbcr_rgb, $
                            yiq_rgb=yiq_rgb, ypbpr_rgb=ypbpr_rgb, yuv_rgb=yuv_rgb
;
if (KEYWORD_SET(rgb_hls)) then begin
    o0=FLTARR(n) & o1=o0 & o2=o0
    for i=0,n-1 do begin
        r=BYTE(i0[i])
        g=BYTE(i1[i])
        b=BYTE(i2[i])
        rgbToHsl, r, g, b, h, l, s
        o0[i]=h
        o1[i]=l
        o2[i]=s
    endfor
endif

if (KEYWORD_SET(hls_rgb)) then begin
    o0=BYTARR(n) & o1=o0 & o2=o0
    for i=0,n-1 do begin
        HlsToRgb, i0[i], i1[i], i2[i], r, g, b
        o0[i]=r
        o1[i]=g
        o2[i]=b
    endfor
endif

if (KEYWORD_SET(rgb_hsv)) then begin
    o0=FLTARR(n) & o1=o0 & o2=o0
    for i=0,n-1 do begin
        r=BYTE(i0[i])
        g=BYTE(i1[i])
        b=BYTE(i2[i])
        rgbToHsv, r, g, b, h, s, v
        o0[i]=h
        o1[i]=s
        o2[i]=v
    endfor
endif

if (KEYWORD_SET(hsv_rgb)) then begin
    o0=BYTARR(n) & o1=o0 & o2=o0
    for i=0,n-1 do begin
        HsvToRgb, i0[i], i1[i], i2[i], r, g, b
        o0[i]=r
        o1[i]=g
        o2[i]=b
    endfor
endif

if (KEYWORD_SET(rgb_yiq)) then begin
    rgbtoyiq=[[0.299,     0.587,       0.114],$
              [0.596, -0.274, -0.322],$
              [0.211, -0.523, 0.312]]
    o0=FLTARR(n) & o1=o0 & o2=o0
    for i=0,n-1 do begin
        r=BYTE(i0[i])
        g=BYTE(i1[i])
        b=BYTE(i2[i])
        in=[r/255.,g/255.,b/255.]
        o=in#rgbtoyiq
        o0[i]=o[0]
        o1[i]=o[1]
        o2[i]=o[2]
    endfor
endif

if (KEYWORD_SET(yiq_rgb)) then begin
    yiqtorgb=[[1, 0.956,   0.619],$
              [ 1, -0.272,  -0.647],$
              [ 1, -1.106, 1.703]]
    o0=BYTARR(n) & o1=o0 & o2=o0
    for i=0,n-1 do begin
        in=[i0[i],i1[i],i2[i]]
        o=BYTE(CEIL(in#yiqtorgb*255.))
        o0[i]=o[0]
        o1[i]=o[1]
        o2[i]=o[2]
    endfor
endif

if (KEYWORD_SET(rgb_yuv)) then begin
    o0=FLTARR(n) & o1=o0 & o2=o0
    for i=0,n-1 do begin
        R=i0[i]/255.
        G=i1[i]/255.
        B=i2[i]/255.
        Y = 0.299 * R + 0.587 * G + 0.114 * B
        U = 0.492*(B-Y)
        V = 0.877*(R-Y)
        o0[i]=Y
        o1[i]=U
        o2[i]=V
    endfor
endif

if (KEYWORD_SET(yuv_rgb)) then begin
    o0=BYTARR(n) & o1=o0 & o2=o0
    for i=0,n-1 do begin
        R = i0[i] + 1.140*i2[i]
        G = i0[i] - 0.395*i1[i] - 0.581*i2[i]
        B = i0[i] + 2.032*i1[i]
        o0[i]=BYTE(CEIL(r*255.))
        o1[i]=BYTE(CEIL(g*255.))
        o2[i]=BYTE(CEIL(b*255.))
    endfor
endif

if (KEYWORD_SET(rgb_ycbcr)) then begin
    ;; not full range see https://web.archive.org/web/20180421030430/http://www.equasys.de/colorconversion.html
    matrix=[[0.257,0.504,0.098],[-0.148,-0.291,0.439],[0.439,-0.368,-0.071]]
    o0=BYTARR(n) & o1=o0 & o2=o0
    for i=0,n-1 do begin
        R=BYTE(i0[i])
        G=BYTE(i1[i])
        B=BYTE(i2[i])
        in=[r,g,b]
        o=[16,128,128]+in#matrix
        o0[i]=(BYTE(o[0])>16)<235
        o1[i]=(BYTE(o[1])>16)<240
        o2[i]=(BYTE(o[2])>16)<240
    endfor
endif

if (KEYWORD_SET(ycbcr_rgb)) then begin
    ;; not full range see https://web.archive.org/web/20180421030430/http://www.equasys.de/colorconversion.html
    matrix=[[1.164,0,1.596],[1.164,-0.392,-0.813],[1.164,2.017,0]]
    o0=BYTARR(n) & o1=o0 & o2=o0
    for i=0,n-1 do begin
        a=(BYTE(i0[i])>16)<235
        b=(BYTE(i1[i])>16)<240
        c=(BYTE(i2[i])>16)<240
        in=[a,b,c]-[16,128,128]
        o=in#matrix
        o0[i]=BYTE(o[0])
        o1[i]=BYTE(o[1])
        o2[i]=BYTE(o[2])
    endfor
endif

if (KEYWORD_SET(rgb_ypbpr)) then begin
    ;; not full range see https://web.archive.org/web/20180421030430/http://www.equasys.de/colorconversion.html
    matrix=[[0.299,0.587,0.114],[-0.169,-0.331,0.5],[0.5,-0.419,-0.081]]
    o0=BYTARR(n) & o1=o0 & o2=o0
    for i=0,n-1 do begin
        R=BYTE(i0[i])
        G=BYTE(i1[i])
        B=BYTE(i2[i])
        in=[r,g,b]
        o=[16,128,128]+in#matrix
        o0[i]=(BYTE(o[0])>16)<235
        o1[i]=(BYTE(o[1])>16)<240
        o2[i]=(BYTE(o[2])>16)<240
    endfor
endif

if (KEYWORD_SET(ypbpr_rgb)) then begin
    ;; not full range see https://web.archive.org/web/20180421030430/http://www.equasys.de/colorconversion.html
    matrix=[[1.0,0,1.402],[1.0,-0.344,-0.714],[1.0,1.722,0]]
    o0=BYTARR(n) & o1=o0 & o2=o0
    for i=0,n-1 do begin
        a=(BYTE(i0[i])>16)<235
        b=(BYTE(i1[i])>16)<240
        c=(BYTE(i2[i])>16)<240
        in=[a,b,c]-[16,128,128]
        o=in#matrix
        o0[i]=BYTE(o[0])
        o1[i]=BYTE(o[1])
        o2[i]=BYTE(o[2])
    endfor
endif

END

PRO COLOR_CONVERT, a, b, c, o0, o1, o2, interleave=interleave, test=test, $
                   hls_rgb=hls_rgb, hsv_rgb=hsv_rgb, rgb_hls=rgb_hls, $
                   rgb_hsv=rgb_hsv, rgb_ycbcr=rgb_ycbcr, rgb_yiq=rgb_yiq, $
                   rgb_ypbpr=rgb_ypbpr, rgb_yuv=rgb_yuv, ycbcr_rgb=ycbcr_rgb, $
                   yiq_rgb=yiq_rgb, ypbpr_rgb=ypbpr_rgb, yuv_rgb=yuv_rgb
;
; counting keywords
;
nbkeys=0
if KEYWORD_SET(hls_rgb) then nbkeys++
if KEYWORD_set(hsv_rgb) then nbkeys++
if KEYWORD_set(rgb_hls) then nbkeys++
if KEYWORD_set(rgb_hsv) then nbkeys++
if KEYWORD_set(rgb_ycbcr) then nbkeys++
if KEYWORD_set(rgb_yiq) then nbkeys++
if KEYWORD_set(rgb_ypbpr) then nbkeys++
if KEYWORD_set(rgb_yuv) then nbkeys++
if KEYWORD_set(ycbcr_rgb) then nbkeys++
if KEYWORD_set(yiq_rgb) then nbkeys++
if KEYWORD_set(ypbpr_rgb) then nbkeys++
if KEYWORD_set(yuv_rgb) then nbkeys++
if (nbkeys EQ 0) then MESSAGE," Keyword parameters required."
if (nbkeys GT 1) then MESSAGE," one and only one color Keyword expected."
;
if (N_PARAMS() ne 2 && N_PARAMS() ne 6) then  MESSAGE,'invalid parameters for COLOR_CONVERT'
;
compact=0
if (N_PARAMS() eq 2) then begin
    s0=SIZE(a)
    if s0[1] ne [3] then MESSAGE,'invalid parameters for COLOR_CONVERT'
    compact=1
    i2=REFORM(a[2,*,*]) & i1=REFORM(a[1,*,*]) & i0=REFORM(a[0,*,*])
    ;; b is and output and will be overwritten
    b = FLTARR(SIZE(a,/dim))

endif else begin
    i2=c & i1=b & i0=a
    n=N_ELEMENTS(i0)
    if (N_ELEMENTS(i1) ne n) then MESSAGE,'vector must have '+strtrim(n,2)+' elements.'
    if (N_ELEMENTS(i2) ne n) then MESSAGE,'vector must have '+strtrim(n,2)+' elements.'
endelse
n=N_ELEMENTS(i0)

if (n eq 1) then begin
    COLOR_CONVERT_SINGLE, i0, i1, i2, o0, o1, o2, $
                          hls_rgb =hls_rgb, hsv_rgb=hsv_rgb, rgb_hls=rgb_hls, $
                          rgb_hsv=rgb_hsv, rgb_ycbcr=rgb_ycbcr, rgb_yiq=rgb_yiq, $
                          rgb_ypbpr=rgb_ypbpr, rgb_yuv=rgb_yuv, ycbcr_rgb=ycbcr_rgb, $
                          yiq_rgb=yiq_rgb, ypbpr_rgb=ypbpr_rgb, yuv_rgb=yuv_rgb
    return
endif

COLOR_CONVERT_MULTIPLE, n, i0, i1, i2, o0, o1, o2, $
                        hls_rgb=hls_rgb, hsv_rgb=hsv_rgb, rgb_hls=rgb_hls, $
                        rgb_hsv=rgb_hsv, rgb_ycbcr=rgb_ycbcr, rgb_yiq=rgb_yiq, $
                        rgb_ypbpr=rgb_ypbpr, rgb_yuv=rgb_yuv, ycbcr_rgb=ycbcr_rgb, $
                        yiq_rgb=yiq_rgb, ypbpr_rgb=ypbpr_rgb, yuv_rgb=yuv_rgb
;  if (compact) ...

if KEYWORD_SET(interleave) then MESSAGE, /informational, 'INTERLEAVE keyword not enforced (TBD)'

b[0,*,*]=o0
b[1,*,*]=o1
b[2,*,*]=o2


if KEYWORD_SET(test) then stop

end

