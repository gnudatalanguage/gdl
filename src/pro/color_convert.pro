PRO color_convert, i0, i1, i2, o0, o1, o2 , hls_rgb =hls_rgb ,  hsv_rgb=hsv_rgb , rgb_hls=rgb_hls, rgb_hsv=rgb_hsv, rgb_ycbcr=rgb_ycbcr,  rgb_yiq=rgb_yiq, rgb_ypbpr=rgb_ypbpr, rgb_yuv=rgb_yuv,  ycbcr_rgb=ycbcr_rgb, yiq_rgb=yiq_rgb, ypbpr_rgb=ypbpr_rgb,  yuv_rgb=yuv_rgb , interleave=interleave

  if (n_params() eq 6) then begin
     o0=i0
     o1=i1
     o2=i2
  endif else if (n_params() eq 2) then i1=i0 else Message,'invalid parameters for COLOR_CONVERT'

  Message,/informational,'Ignoring COLOR_CONVERT command...'
END
