;+
;
; NAME: WRITE_TIFF
;
; PURPOSE: write a tiff image from memory to a bitmap
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;    WRITE_TIFF, filename, image, append=append, bigtiff=bigtiff, bits_per_sample=bits_per_sample,$
;   red=red, green=green, blue=blue, cmyk=cmyk, compression=compression, complex=complex, dcomplex=dcomplex, description=decription,$
;   document_name=document_name,double=double, dot_range=dot_range,float=float,geotiff=geotiff,l64=l64,long=long,icc_profile=icc_profile,$
;   orientation=ori,photoshop=photoshop,planarconfig=planarconfig,signed=signed,units=units,$
;   verbose=verbose, xposition=xpos, xresol=xres, yposition=ypos, yresol=yres filename, image, /append, /bigtiff, bits_per_sample=,
;
; KEYWORD PARAMETERS: 
;     ORDER      : 1 = top-bottom, 0 = bottom-top
;     VERBOSE    : Not Used
;
; OPTIONAL INPUTS:
;        bits_per_sample: bits per sample
;        compression: compression method (not done)
;        For pseudocolor only
;        red  : the Red colormap vector (for PseudoColor images)
;        green: the Green colormap vector (for PseudoColor images)
;        blue : the Blue colormap vector (for PseudoColor images)
;
; RESTRICTIONS:
;
; PROCEDURE:
;
; EXAMPLE: 
;         Currently no example is available
;
; MODIFICATION HISTORY:
;  Written by Gilles Duvert
;-
; LICENCE:
; Copyright (C) 2023: GD
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
function WriteASimpleTag,unit,s
  writeu,unit,s.id,s.type,s.size
  point_lun,-unit,pos
  writeu,unit,s.value
  return, pos
end
pro  WritePadding,unit
     point_lun,-unit,current
     pad=current mod 4 & if pad gt 0 then writeu,unit,replicate(0b,pad)
end
pro WriteEndOfIFD,unit
   writeu,unit,0ULL
end

pro AddtoTags,tags,index,id,type,size,value
  tags[index].id=fix(id)
  tags[index].type=fix(type)
  tags[index].size=long(size)
  tags[index].value=long(value)
end

pro UpdateTagValueWithPosition,unit,offsettable,i
   point_lun,-unit,current
   ; write start address at offsetable location and go to current 
   point_lun,unit,offsettable[i]
   writeu,unit,long(current)
   point_lun,unit,current
end

function check_dimension_is_2, array, name
  on_error,2
  sz=size(array)
  if sz[0] ne 2 then Message,name+" is not a 2-dim array."
  return,sz[1:2]
end
function AddImageAndLinkIFDChain, unit, image
  on_error,2
  compile_opt HIDDEN
  signature=0s
  magic=0s
  offsetIFD=0L
  readu,unit,signature,magic,offsetIFD
  if signature ne '4949'x then message,'Cannot update big-endian TIFF files; please FIXME'
;; go to first IFD0
  point_lun,unit,offsetIFD
  nexistingIFDEntries=0s
  readu,unit,nexistingIFDEntries
;; go to last IFD if any
  skip_lun,unit,nexistingIFDEntries*12
;; read the pointer to the next IFD (may be Zero)
  PointerToNextIFD=0L & readu,unit,PointerToNextIFD
  while PointerToNextIFD ne 0 do begin
     offsetIFD=PointerToNextIFD
     point_lun,unit,offsetIFD
     readu,unit,nexistingIFDEntries
     skip_lun,unit,nexistingIFDEntries*12
     readu,unit,PointerToNextIFD
  endwhile
;; save this last IFD last position
  point_lun,-unit,lastpos
  lastpos-=4                    ; start of last 'long' value
  startofdata=(fstat(unit)).size ; data will start at end of file
  point_lun,unit,startofdata         ; go to end
  writeu,unit,image ; write image at "start of data"
  point_lun,-unit,currentpos
  point_lun,unit,lastpos
  writeu,unit,long(currentpos)      ; 32 BITS!
  point_lun,unit,currentpos         ; end of file after image written
  return, startofdata
end

pro WRITE_TIFF, filename, imageDonotTouch, append=append, bigtiff=bigtiff, bits_per_sample=bits_per_sample,$
   red=red, green=green, blue=blue, cmyk=cmyk, compression=compression, complex=complex, dcomplex=dcomplex, description=decription,$
   document_name=document_name,double=double, dot_range=dot_range,float=float,geotiff=geotiff,l64=l64,long=long,icc_profile=icc_profile,$
   orientation=orientation,photoshop=photoshop,planarconfig=planarconfig,signed=signed,units=units,$
   verbose=verbose, xposition=xpos, xresol=xres, yposition=ypos, yresol=yres

  on_error,2
  compile_opt HIDDEN

  if keyword_set(bigtiff) then Message,/informational,"BIGTIFF Keyword ignored. Fixme!"
  ;; compression UNSUPPORTED
  compression= keyword_set(compression)
  if (compression) then Mesage,/informational,"Compression not supported by GDL, writing uncompressed."
  


  tiffbyte=1us & tiffascii=2us & tiffshort=3us & tifflong=4us & tiffrational=5us & tiffsbyte=6us & tiffundef=7us &$
     stiffshort=8us & stifflong=9us & stiffrational=10us & tifffloat=11us & tiffdouble=12us
  samples=[4,1,2,2,3,3,6,4,4,6, 4, 4, 1, 1, 1, 1]

  ;;default
  PlanarConfiguration=1
  PhotometricInterpretation=1   ; GREY, Black is Zero
  
  ;;0) check sizes and image type
  ;; planarconfig and R,G,B present and dim 2: ignore imageDonotTouch, create one with R,G,B
  hasr= keyword_set(red)
  hasg= keyword_set(green)
  hasb= keyword_set(blue)
  tot= total(hasr+hasg+hasb)
  if tot ne 0 and tot ne 3 then Message,"RED, GREEN, and BLUE must appear together."
  hasplan= keyword_set(planarconfig)
  special=0
  if hasplan then begin
     ;; special planarconfig case: create a [3,n,m] byte pixel image.
     if planarconfig eq 2 then begin
        szred=check_dimension_is_2( red, 'RED')
        szgreen=check_dimension_is_2( green, 'GREEN')
        szblue=check_dimension_is_2( blue, 'BLUE')
        if (szred[0] ne szgreen[0]) or (szred[0] ne szblue[0]) then   Message,"Red, Green and Blue arrays must be of same dimensions."
        if (szred[1] ne szgreen[1]) or (szred[1] ne szblue[1]) then   Message,"Red, Green and Blue arrays must be of same dimensions."
        image=[[[red]],[[green]],[[blue]]]
        special=1
        SamplePerPixel=3
        n=szred[0]
        m=szred[1]
        PlanarConfiguration=2
        PhotometricInterpretation=2 ; RGB
        goto, image_defined
     endif
  endif

  if n_elements(imageDoNotTouch) eq 0 then Message,"Image array argument required."
  sz = size(imageDoNotTouch)
  if sz[0] eq 0 then message,"Expression must be an array in this context: "+scope_varname(imageDoNotTouch,lev=-1)
  if sz[0] le 1 or sz[0] gt 3 then message,"Image array must be [n,m] or [k,n,m] (k>3 is nonstandard)"
  SamplePerPixel=1
  n=sz[1]
  m=sz[2]
  if sz[0] eq 3 then begin
     if hasplan then begin
        case planarconfig of
           1: begin
              SamplePerPixel=sz[1]
              n=sz[2]
              m=sz[3]
           end
           2: begin
              SamplePerPixel=sz[3]
              n=sz[1]
              m=sz[2]
              PlanarConfiguration=2
           end
           else: Message,"Illegal keyword value for PLANARCONFIG."
        endcase
     endif else begin
        SamplePerPixel=sz[1]
        n=sz[2]
        m=sz[3]
     endelse
  endif
  
  ;; will use a local copy of image
  image=imageDoNotTouch
  ;; gets here from special cases above
image_defined:
  
  ;; image type options:
  dosigned  = keyword_set(signed)
  doshort   = keyword_set(short)
  dolong    = keyword_set(long)
  dol64     = keyword_set(l64)
  dofloat   = keyword_set(float)
  dodouble  = keyword_set(double)
  docomplex = keyword_set(complex)
  dodcomplex= keyword_set(dcomplex)
  dobyte = ~(doshort or dolong or dol64 or dofloat or dodouble or docomplex or dodcomplex)
  
  ;; check non-byte option
if dodcomplex then begin & image=fix(temporary(image),type=9) & bps=128 & endif else $ 
   if docomplex  then begin & image=fix(temporary(image),type=6) & bps=64 & endif else $
      if dodouble   then begin & image=fix(temporary(image),type=5) & bps=64 & endif else $
         if dofloat    then begin & image=fix(temporary(image),type=4) & bps=32 & endif else $
            if dol64      then begin & if dosigned then image=fix(temporary(image),type=15) else image=fix(temporary(image),type=14) & bps=64 & endif else $
               if dolong     then begin & if dosigned then image=fix(temporary(image),type=3) else image=fix(temporary(image),type=13) & bps=32 & endif else $
                  if doshort    then begin & if dosigned then image=fix(temporary(image),type=2) else image=fix(temporary(image),type=12) & bps=16 & endif else $
                     if dobyte     then begin & image=fix(temporary(image),type=1) & bps=8 & endif
   if dobyte and (not special) then begin
      if keyword_set(bits_per_sample) then bps=bits_per_sample
      if ~ISA(image,"Byte") then Message,"internal error, please report."
      bps=fix(bps)
      shift=8-bps
      nnew=SamplePerPixel*n*m*bps/8
      tmp=bytarr(nnew)
      K=0UL & I=0UL
      case bps of
         8: break
         4: begin
            for j=0,nnew-1 do begin
               tmp[k]=ISHFT(image[i++],4)
               tmp[k] OR= image[i++]
               k++
            endfor
            image=tmp
         end
         1: begin
            image=image gt 0
            for j=0,nnew-1 do begin
               tmp[k] =   ISHFT(image[i++],7)
               tmp[k] OR= ISHFT(image[i++],6)
               tmp[k] OR= ISHFT(image[i++],5)
               tmp[k] OR= ISHFT(image[i++],4)
               tmp[k] OR= ISHFT(image[i++],3)
               tmp[k] OR= ISHFT(image[i++],2)
               tmp[k] OR= ISHFT(image[i++],1)
               tmp[k] OR= image[i++]
               k++
            endfor
            image=tmp
         end
         else: Message,"Illegal keyword value for BITS_PER_SAMPLE."
      endcase
   endif

   ;;image OK, open file, write it
   doAppend=keyword_set(append)
   if (doAppend) then begin
      if query_tiff(filename) ne 1 then message,filename+" is not existing or is not a TIFF file!"
      openu,unit,filename,/get_lun
      StartOfData=addImageAndLinkIFDChain(unit,image) ; we are at end, after writing image
   endif else begin
      openw,unit,filename,/get_lun
      writeu,unit,'4949'xus,'002a'xus,long(8+n_bytes(image))
      StartOfData=8
      writeu,unit,image
   endelse

   ;; BitsPerSample: single or vector ?
   if SamplePerPixel eq 1 then BitsPerSample=bps else BitsPerSample=replicate(bps,SamplePerPixel)
   ;; PhotometricInterpretation
   case SamplePerPixel of
      1:   PhotometricInterpretation=1    ; Black is Zero
      3:   PhotometricInterpretation=2    ; RGB
      else:   PhotometricInterpretation=4 ;  ???
   endcase
   ;; Palette Image?
   doPalette=0
   if tot eq 3 and SamplePerPixel eq 1  then begin
      if n_elements(red) ne  n_elements(green) then Message,"RED, GREEN, and BLUE must be the same size."
      if n_elements(red) ne  n_elements(blue) then Message,"RED, GREEN, and BLUE must be the same size."
      p=2^BitsPerSample
      nn=n_elements(red)<p
      colormap=bytarr(3*p)
      colormap[0:nn-1]=red
      colormap[p:p+nn-1]=green
      colormap[2*p:-1]=blue
      PhotometricInterpretation=3
      doPalette=1
   endif

   ;;Misc. options
   ;; xposition
   hasxpos = keyword_set(xpos)
   hasypos = keyword_set(ypos)
   if n_elements(xres) eq 0 then xres=100
   if n_elements(yres) eq 0 then yres=100
   ;; dot_range: 2 elements
   dotrange= keyword_set(dot_range)
   if dotrange then begin
      if n_elements(dot_range) ne 2 then Message,"Keyword array parameter DOT_RANGE must have 2 elements."
      ;;trick to pass dot_range in a LONG
      longasdotrange=long(dot_range[0]) & longasdotrange=ISHFT(longasdotrange,16) OR long(dot_range[1])
   endif

   ;; ICC PROFILE must be byte 1
   hasicc= keyword_set(icc_profile)
   if hasicc then begin
      if size(icc_profile,/type) ne 1 then  Message,"Illegal keyword value for ICC_PROFILE: must be a byte array."
   endif
   ;; PHOTOSHOP must be byte 1
   hasphotoshop= keyword_set(photoshop)
   if hasphotoshop then begin
      if size(photoshop,/type) ne 1 then  Message,"Illegal keyword value for PHOTOSHOP: must be a byte array."
   endif
   
   ;; orientation
   if (n_elements(orientation) eq 0 ) then orientation=1 else orientation=fix(orientation[0])
   if orientation eq 0 then orientation = 4
   if orientation lt 1 or orientation gt 8 then message,"Illegal keyword value for ORIENTATION."
   ;; sampleformat
   sampleformat=samples[size(image,/type)]
   ;; document_name
   if (n_elements(document_name) eq 0 ) then document_name=filename 
   prop={id:0s, type:3s, size:1l, value:0l}
   ntags=100
   tags=replicate(prop,ntags)

   i=0 & AddtoTags,tags,i,256,tiffshort,1s,n ; ImageWidth
   i++ & AddtoTags,tags,i,257,tiffshort,1s,m ; ImageHeight

if SamplePerPixel eq 1 then begin & i++ & AddtoTags,tags,i,258,tiffshort,SamplePerPixel,BitsPerSample & end ; BitsPerSample one-liner
   if SamplePerPixel gt 1 then begin
      i++ & AddtoTags,tags,i,258,tiffshort,SamplePerPixel,0 
      BitsPerSampleIndex=i
   endif
   
   i++ & AddtoTags,tags,i,259,tiffshort,1,1                         ; Compression
   i++ & AddtoTags,tags,i,262,tiffshort,1,PhotometricInterpretation ; PhotometricInterpretation
   i++ & AddtoTags,tags,i,266,tiffshort,1,1                         ; FillOrder
   
   i++ & AddtoTags,tags,i,269,tiffascii,n_bytes(document_name)+1,0 ; DocumentName
   DocumentNameIndex=i
   imagedescription="GDL Tiff file"
   i++ & AddtoTags,tags,i,270,tiffascii,n_bytes(imagedescription)+1,0 ; ImageDescription
   ImageDescriptionIndex=i
   ;;data location
   i++ & AddtoTags,tags,i,273,tifflong,1s,StartofData    ; StripOffsets (fixed)
   i++ & AddtoTags,tags,i,274,tiffshort,1s,orientation   ; Orientation
   i++ & AddtoTags,tags,i,277,tiffshort,1,SamplePerPixel ; SamplePerPixel
   i++ & AddtoTags,tags,i,278,tifflong,1s,m              ; RowsPerStrip
   i++ & AddtoTags,tags,i,279,tifflong,1s,n_bytes(image) ; StripByteCounts
   i++ & AddtoTags,tags,i,282,tiffrational,1s,0          ; XResolution
   Xresolution=i
   i++ & AddtoTags,tags,i,283,tiffrational,1s,0 ; YResolution
   Yresolution=i
   i++ & AddtoTags,tags,i,284,tiffshort,1s,PlanarConfiguration                    ; PlanarConfiguration
if (hasxpos) then begin & i++ & AddtoTags,tags,i,286,tiffrational,1s,0 & endif    ; XPosition
   xposIndex=i
if (hasypos) then begin & i++ & AddtoTags,tags,i,287,tiffrational,1s,0 & endif ; YPosition
   yposIndex=i



   i++ & AddtoTags,tags,i,296,tiffshort,1s,2 ; ResolutionUnits
   ;;software
   software = "GDL, version "+!GDL.RELEASE
   i++ & AddtoTags,tags,i,305,tiffascii,n_bytes(software)+1,0 ; Software
   softwareIndex=i                                            ; memo
   modifydate=systime()
   i++ & AddtoTags,tags,i,306,tiffascii,n_bytes(modifydate)+1,0 ; ModifyDate
   modifydateIndex=i                                            ; memo
   if (doPalette) then begin
      i++ & AddtoTags,tags,i,320,tiffshort,3*2^BitsPerSample,0
      colormapindex=i
   endif
   
if (dotrange) then begin & i++ & AddtoTags,tags,i,336,tiffshort,2,longasdotrange & endif ; DotRange
if sampleformat ne 1 then begin
   i++ & AddtoTags,tags,i,339,tifflong,1s,sampleformat                                   ; SampleFormat
endif

   ;;photoshop
if (hasphotoshop) then begin & i++ & AddtoTags,tags,i,34377US,tiffbyte,n_elements(photoshop),0 & endif ; PHOTOSHOP
   if (hasphotoshop) then photoshopindex=i

   ;;icc_profile
if (hasicc) then begin & i++ & AddtoTags,tags,i,34675US,tiffundef,n_elements(icc_profile),0 & endif ; ICC_PROFILE
   if (hasicc) then iccIndex=i
   
   ntags=i+1
   writeu,unit,fix(ntags)
   pos=replicate(0ULL,ntags)
   for i=0,ntags-1 do pos[i]=writeAsimpletag(unit,tags[i])
   WriteEndOfIFD,unit
   if SamplePerPixel gt 1 then begin
      UpdateTagValueWithPosition,unit,pos,BitsPerSampleIndex
      writeu,unit,fix(BitsPerSample)
      WritePadding,unit
   endif
   UpdateTagValueWithPosition,unit,pos,XResolution
   writeu,unit,long(xres),1L    ; Xresolution
   UpdateTagValueWithPosition,unit,pos,YResolution
   writeu,unit,long(yres),1L    ; Yresolution
   if (hasxpos) then begin
      UpdateTagValueWithPosition,unit,pos,xposIndex
      writeu,unit,100L,long(xpos),1L ; Xposition
   endif
   if (hasypos) then begin
      UpdateTagValueWithPosition,unit,pos,yposIndex
      writeu,unit,100L,long(ypos),1L ; Xposition
   endif

   UpdateTagValueWithPosition,unit,pos,documentnameIndex
   writeu,unit,document_name,0b
   WritePadding,unit

   UpdateTagValueWithPosition,unit,pos,ImageDescriptionIndex
   writeu,unit,imagedescription,0b
   WritePadding,unit

   UpdateTagValueWithPosition,unit,pos,softwareIndex
   writeu,unit,software,0b
   WritePadding,unit

   UpdateTagValueWithPosition,unit,pos,modifydateIndex
   writeu,unit,modifydate,0b
   WritePadding,unit

   if (doPalette) then begin
      UpdateTagValueWithPosition,unit,pos,colormapindex
      writeu,unit,fix(colormap)
      WritePadding,unit
   endif
   
   if (hasphotoshop) then begin
      UpdateTagValueWithPosition,unit,pos,photoshopIndex
      writeu,unit,photoshop
      WritePadding,unit
   endif
   if (hasicc) then begin
      UpdateTagValueWithPosition,unit,pos,iccIndex
      writeu,unit,icc_profile
      WritePadding,unit
   endif

   ;; end & special geotiff treatment
   flush,unit
   close,unit
   free_lun,unit
   ;; fortunately geotiff tags have a tag number greater than all the others, including photoshop & icc
   if keyword_set(geotiff) then updategeotagsinimage,filename,geotiff
end
