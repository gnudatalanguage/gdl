;+
; WRITEGEOTAGS
;       adds the geoTiff tags passed in the struct 'geostruct' to an existing TIFF image
;       Used in WRITE_TIFF
;       Adds tags only in the last TIFF header (last image)
;
; Usage: writegeotags,filename,geostruct
;
; Arguments:
;        filename  string   the tiff file names
;        geostruct struct   a struct (see example below) with tags as in http://geotiff.maptools.org/spec/geotiff2.4.html
;        GDL does just check the tags are known and their dimension adequate.
;        below, an example of such struct
;        g = { $
;           ModelPixelScaleTag: [25, 25, 0d], $                                                  ; double
;           ModelTiepointTag: [80s, 100s, 0s, 200, 150, 0] , $                                   ; double
;           GEOGCITATIONGEOKEY: "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" , $ ; string
;           PCSCITATIONGEOKEY: "ZZZ",$                                                           ; string
;           ProjStdParallel1GeoKey: 41.333, $                                                    ; double
;           ProjStdParallel2GeoKey: 48.666, $                                                    ; double
;           ProjCenterLongGeoKey: 120.0, $                                                       ; double
;           ProjNatOriginLatGeoKey: 45.0, $                                                      ; double
;           ProjFalseEastingGeoKey: 200000.0, $                                                  ; double
;           ProjFalseNorthingGeoKey: 1500000.0, $                                                ; double
;           GTModelTypeGeoKey: 1  , $                                                            ; (ModelTypeProjected) int
;           GTRasterTypeGeoKey: 1, $                                                             ; (RasterPixelIsArea) int
;           GeographicTypeGeoKey: 4267, $                                                        ; (GCS_NAD27) int
;           ProjectedCSTypeGeoKey: 32767, $                                                      ; (user-defined) int
;           ProjectionGeoKey: 32767, $                                                           ; (user-defined) int
;           ProjLinearUnitsGeoKey: 9001  , $                                                     ; (Linear_Meter) int
;           ProjCoordTransGeoKey: 8LL $                                                          ; (CT_LambertConfConic_2SP) int
;           }
;
; History:
;       Original: 2023-Feb-26; Gilles Duvert
;-
; LICENCE:
; Copyright (C) 2023: GD
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   

function getTagindexes,g
  common geotifftagcommon,GeoSize,GeoTagNames,GeoTagKeyId,GeoMethod,TiffType,TagTypeLen
  Geosize=48
  Geotagnames=[$
     'MODELPIXELSCALETAG',$
     'MODELTRANSFORMATIONTAG',$
     'MODELTIEPOINTTAG',$

     'GTMODELTYPEGEOKEY',$
     'GTRASTERTYPEGEOKEY',$
     'GTCITATIONGEOKEY',$

     'GEOGRAPHICTYPEGEOKEY',$
     'GEOGCITATIONGEOKEY',$
     'GEOGGEODETICDATUMGEOKEY',$
     'GEOGPRIMEMERIDIANGEOKEY',$
     'GEOGLINEARUNITSGEOKEY',$
     'GEOGLINEARUNITSIZEGEOKEY',$
     'GEOGANGULARUNITSGEOKEY',$
     'GEOGANGULARUNITSIZEGEOKEY',$
     'GEOGELLIPSOIDGEOKEY',$
     'GEOGSEMIMAJORAXISGEOKEY',$
     'GEOGSEMIMINORAXISGEOKEY',$
     'GEOGINVFLATTENINGGEOKEY',$
     'GEOGAZIMUTHUNITSGEOKEY',$
     'GEOGPRIMEMERIDIANLONGGEOKEY',$

     'PROJECTEDCSTYPEGEOKEY',$
     'PCSCITATIONGEOKEY',$
     'PROJECTIONGEOKEY',$
     'PROJCOORDTRANSGEOKEY',$
     'PROJLINEARUNITSGEOKEY',$
     'PROJLINEARUNITSIZEGEOKEY',$
     'PROJSTDPARALLEL1GEOKEY',$
     'PROJSTDPARALLEL2GEOKEY',$
     'PROJNATORIGINLONGGEOKEY',$
     'PROJNATORIGINLATGEOKEY',$
     'PROJFALSEEASTINGGEOKEY',$
     'PROJFALSENORTHINGGEOKEY',$
     'PROJFALSEORIGINLONGGEOKEY',$
     'PROJFALSEORIGINLATGEOKEY',$
     'PROJFALSEORIGINEASTINGGEOKEY',$
     'PROJFALSEORIGINNORTHINGGEOKEY',$
     'PROJCENTERLONGGEOKEY',$
     'PROJCENTERLATGEOKEY',$
     'PROJCENTEREASTINGGEOKEY',$
     'PROJCENTERNORTHINGGEOKEY',$
     'PROJSCALEATNATORIGINGEOKEY',$
     'PROJSCALEATCENTERGEOKEY',$
     'PROJAZIMUTHANGLEGEOKEY',$
     'PROJSTRAIGHTVERTPOLELONGGEOKEY',$

     'VERTICALCSTYPEGEOKEY',$
     'VERTICALCITATIONGEOKEY',$
     'VERTICALDATUMGEOKEY',$
     'VERTICALUNITSGEOKEY'$
     ]
  TiffType=[$
     12s,$
     12s,$
     12s,$
     3s,$
     3s,$
     2s,$
     3s,$
     2s,$
     3s,$
     3s,$
     3s,$
     12s,$
     3s,$
     12s,$
     3s,$
     12s,$
     12s,$
     12s,$
     3s,$
     12s,$
     3s,$
     2s,$
     3s,$
     3s,$
     3s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     12s,$
     3s,$
     2s,$
     3s,$
     3s$
  ]


  GeotagKeyID=[$
     33550us,$
     34264us,$
     33922us,$
     1024us,$
     1025us,$
     1026us,$
     2048us,$
     2049us,$
     2050us,$
     2051us,$
     2052us,$
     2053us,$
     2054us,$
     2055us,$
     2056us,$
     2057us,$
     2058us,$
     2059us,$
     2060us,$
     2061us,$
     3072us,$
     3073us,$
     3074us,$
     3075us,$
     3076us,$
     3077us,$
     3078us,$
     3079us,$
     3080us,$
     3081us,$
     3082us,$
     3083us,$
     3084us,$
     3085us,$
     3086us,$
     3087us,$
     3088us,$
     3089us,$
     3090us,$
     3091us,$
     3092us,$
     3093us,$
     3094us,$
     3095us,$
     4096us,$
     4097us,$
     4098us,$
     4099us$
  ]
  Geomethod=[1,1,1,replicate(0,45)]
  ;           byte, ascii, short, long, rational, sbyte, undefined, sshort, slong, srational, float, double
  TagTypeLen=[1,    5,     2,     4,    8,        1,     1,         2,      4,     8,         4,     8]

  GeoMinNelements=[3,16,6,replicate(1,45)]
  GeoMaxNelements=[3,16,100000000LL,replicate(1,45)]
  ntags=n_tags(g)
  tags=tag_names(g)
  tagindexes=intarr(ntags)
  for i=0,ntags-1 do begin
     w=where(Geotagnames eq tags[i],count) & if count ne 1 then Message,"Unknown GeoTag: "+tags[i]
     tagindexes[i]=w
  endfor
  ; ancillary test
  for i=0,ntags-1 do begin
   n=n_elements(g.(i))
   if  n gt GeoMaxNElements[tagindexes[i]] then  Message,"Number of values in tag "+Geotagnames[tagindexes[i]]+" invalid."
   if  n lt GeoMinNElements[tagindexes[i]] then  Message,"Number of values in tag "+Geotagnames[tagindexes[i]]+" invalid."
  endfor
  return, tagindexes
end

function writeAsimpletag,unit,geostruct,i,geoIndex
  common geotifftagcommon,GeoSize,GeoTagNames,GeoTagKeyId,GeoMethod,TiffType
   val=geostruct.(i)
   count=long(n_elements(val))
   writeu,unit,GeoTagKeyId[geoIndex],TiffType[geoIndex],count
   point_lun,-unit,pos
   writeu,unit,0L
   return, pos
end

function writeAGeoTag,unit,geostruct,i,geoIndex
  GeoDoubleParamsTag='87b0'xus
  GeoAsciiParamsTag='87b1'xus
  common geotifftagcommon,GeoSize,GeoTagNames,GeoTagKeyId,GeoMethod,TiffType,TagTypeLen
   val=geostruct.(i)
   count=n_elements(val)
   case TiffType[geoIndex] of
      12s: writeu,unit,GeoTagKeyId[geoIndex],GeoDoubleParamsTag,fix(count)
      2s: writeu,unit,GeoTagKeyId[geoIndex],GeoAsciiParamsTag,fix(strlen(val)+1) ; +1 for ending 0
      3s: writeu,unit,GeoTagKeyId[geoIndex],0s,fix(count)
      else: message,"Internal error, please report."
   endcase
   point_lun,-unit,pos
   ; will write locally all singleton tags
   if TiffType[geoIndex] ne 3s or count ne 1 then begin
      writeu,unit,0us
      return, pos
   endif
   writeu,unit,fix(val)
   return, -1
end

pro writeOffsettedValueDouble,unit,offsettable,geostruct,i,geoindex,number
  common geotifftagcommon,GeoSize,GeoTagNames,GeoTagKeyId,GeoMethod,TiffType,TagTypeLen
  if offsettable[i] le 0 then message,"Internal error, please report."
   point_lun,-unit,pos
   val=geostruct.(i) 
   count=long(n_elements(val))
   case TiffType[geoIndex] of
      12s: writeu,unit,double(val) 
      else: message,"Internal error, please report."
   endcase
   point_lun,-unit,current
   ; write start address at offsetable location and go to end 
   point_lun,unit,offsettable[i]
   writeu,unit,fix(number)
   point_lun,unit,current
end
pro writeOffsettedValueAscii,unit,offsettable,geostruct,i,geoindex,number
  common geotifftagcommon,GeoSize,GeoTagNames,GeoTagKeyId,GeoMethod,TiffType,TagTypeLen
  if offsettable[i] le 0 then message,"Internal error, please report."
   point_lun,-unit,pos
   val=geostruct.(i) 
   count=long(n_elements(val))
   case TiffType[geoIndex] of
      2s: writeu,unit,val,'7c'xb ; '|'
      else: message,"Internal error, please report."
   endcase
   point_lun,-unit,current
   ; write start address at offsetable location and go to end 
   point_lun,unit,offsettable[i]
   writeu,unit,fix(number)
   point_lun,unit,current
end

pro writeOffsettedValue,unit,offsettable,geostruct,i,geoindex
  common geotifftagcommon,GeoSize,GeoTagNames,GeoTagKeyId,GeoMethod,TiffType,TagTypeLen
  if offsettable[i] le 0 then return
   point_lun,-unit,pos
   val=geostruct.(i) 
   count=long(n_elements(val))
   case TiffType[geoIndex] of
      12s: writeu,unit,double(val)
      2s: writeu,unit,val,0b
      3s: writeu,unit,fix(val)
      else: message,"Internal error, please report."
   endcase
   point_lun,-unit,current
   ; write start address at offsetable location and go to end 
   point_lun,unit,offsettable[i]
   writeu,unit,long(pos)
   point_lun,unit,current
end

pro updategeotagsinimage,filename,g
  COMPILE_OPT idl2, HIDDEN
  common geotifftagcommon,GeoSize,GeoTagNames,GeoTagKeyId,GeoMethod,TiffType,TagTypeLen
  GeoTiffDirectory='87af'xus    ;# (value is stored after directory)
  GeoTiffDoubleParams='87b0'xus
  GeoTiffAsciiParams='87b1'xus
  Ascii=2s
  uShort=3s
  Double=12s
; check if worth doing anything
; get minimal tag informations
  tagidx=gettagindexes(g) ; will throw if problems
  ntags=n_elements(tagidx)
  offsetTable=intarr(ntags)

; find total number of new IFD0 entriss
  w=where(GeoMethod[tagidx] eq 1, count)
  nsimpletags=count
; simpletags are double, so nsimpletags must be deduced from next census:
  w=where(TiffType[tagidx] eq 12s, count)
  ndouble=count-nsimpletags & addouble=ndouble gt 0
; strings
  w=where(TiffType[tagidx] eq 2s, count)
  ntext=count & addtext=ntext gt 0
; ints
  w=where(TiffType[tagidx] eq 3s, count)
  nshort=count
  nentries=nsimpletags+1+addtext+addouble ; +1 for geo
 
; The file
  if query_tiff(filename) ne 1 then message,filename+" is not existing or is not a TIFF file!"
  openu,unit,filename,/get_lun
  signature=0s
  magic=0s
  offsetIFD=0L
  readu,unit,signature,magic,offsetIFD
  if signature ne '4949'x then message,'Cannot update big-endian TIFF files; please FIXME'

; go to first IFD0
  point_lun,unit,offsetIFD
  nexistingIFDEntries=0s
  readu,unit,nexistingIFDEntries
; go to last IFD if any
  skip_lun,unit,nexistingIFDEntries*12
; read the pointer to the next IFD (may be Zero)
  PointerToNextIFD=0L & readu,unit,PointerToNextIFD
  while PointerToNextIFD ne 0 do begin
     offsetIFD=PointerToNextIFD
     point_lun,unit,offsetIFD
     readu,unit,nexistingIFDEntries
     skip_lun,unit,nexistingIFDEntries*12
     readu,unit,PointerToNextIFD
  endwhile
; go to offsetIFD, start of the last IFD
  point_lun,unit,offsetIFD
  readu,unit,nexistingIFDEntries
; for every tag whose value is a pointer in the file, we'll add the displacement due to the insertion of the new tags
  existingTagLocation=Lonarr(nexistingIFDEntries)
; record these 'pointers' if they are meaningful:
  for i=0,nexistingIFDEntries-1 do begin
     tagid=0s & tagtype=0s & tagnum=0l & tagval=0L
     readu,unit,tagid,tagtype,tagnum,tagval
     ; Value Offset contains the Value instead of pointing to the Value if and only if the Value fits into 4 bytes.
     if tagtypelen[tagtype-1]*tagnum le 4 then existingTagLocation[i]=-1 else existingTagLocation[i]=tagval
  endfor
; we are at end of last IFD and before the zero pointer, this is where we will add our tags.
; record our position in 'goback'
  point_lun,-unit,goback
; copy everything until the end to memory, includes last pointer to the next IFD (NULL)
  endpos=(fstat(unit)).size
  temp=bytarr(endpos-goback)
  readu,unit,temp
; we will add 'nentries*12' to each of existingTagLocation if they are pointers:
  toBeAdded=nentries*12
; go back to last IFD, change number of entries:
  point_lun,unit,offsetIFD
  writeu,unit,fix(nexistingIFDEntries+nentries)
; update existing pointers if necessary  
  for i=0,nexistingIFDEntries-1 do begin
     tagid=0s & tagtype=0s & tagnum=0l & tagval=0l
     readu,unit,tagid,tagtype,tagnum
     if existingTagLocation[i] eq -1 then readu,unit,tagval else writeu,unit, long(existingTagLocation[i]+toBeAdded)
  endfor
  
; ready to write our tags:
; write normal tags
  if nsimpletags gt 0 then begin
     for i=0,ntags-1 do begin
        if GeoMethod[tagidx[i]] ne 1 then continue
        offsettable[i]=writeAsimpletag(unit,g,i,tagidx[i])
     endfor
  endif
; compute size of GeoTiffDirectory section
  nGeoEntries=1+ntext+ndouble+nshort
  writeu,unit, GeoTiffDirectory,uShort,long(nGeoEntries*4)
;memorize and GeoKey offset:
  point_lun,-unit,posGeoKey & writeu,unit,0L
  
; create GeoTiffDoubleParams
  if nDouble gt 0 then begin
     writeu,unit, GeoTiffDoubleParams,Double,long(ndouble)
;memorize and GeoDouble offset:
     point_lun,-unit,posGeoDouble & writeu,unit,0L
  endif
  
; create GeoTiffAsciiParams
  if nText gt 0 then begin
     totaltextsize=0
                                ; compute total string length
     for i=0,ntags-1 do begin
        if GeoMethod[tagidx[i]] eq 1 or TiffType[tagidx[i]] ne 2s then continue
        val=g.(i)
        textnbytes=strlen(val)+1 ; +1 for ending '|'
        totaltextsize+=textnbytes
     endfor
     totaltextsize+=1           ; for ending NULL
     addToAsciiSection=totaltextsize mod 4 & totaltextsize+=addToAsciiSection
     writeu,unit, GeoTiffAsciiParams,Ascii,long(totaltextsize)
                                ;memorize and GeoDouble offset:
     point_lun,-unit,posGeoAscii & writeu,unit,0L
  endif
; write the temp array, now displaced. It contains the pointer to the next IFD 
  writeu,unit,temp
; we should be at end of file
  
;write GeoKey header
  point_lun,-unit,current
  point_lun,unit,posGeoKey
  writeu,unit,long(current)
  point_lun,unit,current
; write all keys of GeoKeys
  writeu,unit, 1s,1s,2s,fix(nGeoEntries-1) ;Version 1 GeoTIFF GeoKey directory, the keys are Rev. 1.2
; write all GeoKey tags:
  for i=0,ntags-1 do begin
     if GeoMethod[tagidx[i]] ne 0 then continue
     offsettable[i]=writeAGeoTag(unit,g,i,tagidx[i])
  endfor
  
; GeoDouble values
  if nDouble gt 0 then begin
                                ;write GeoDouble pointer
     point_lun,-unit,current
     point_lun,unit,posGeoDouble
     writeu,unit,long(current)
     point_lun,unit,current
     number=0
     for i=0,ntags-1 do begin
        if GeoMethod[tagidx[i]] eq 1 or TiffType[tagidx[i]] ne 12s then continue
        writeOffsettedValueDouble,unit,offsettable,g,i,tagidx[i],number
        number++
     endfor
     point_lun,-unit,current
     pad=current mod 4 & if pad gt 0 then writeu,unit,replicate(0b,pad)
  endif

 ; GeoAscii values 
  if nText gt 0 then begin
                                ;write GeoAscii pointer
     point_lun,-unit,current
     point_lun,unit,posGeoAscii
     writeu,unit,long(current)
     point_lun,unit,current
     number=0
     for i=0,ntags-1 do begin
        if GeoMethod[tagidx[i]] eq 1 or TiffType[tagidx[i]] ne 2s then continue
        writeOffsettedValueAscii,unit,offsettable,g,i,tagidx[i],number
        number+=strlen(g.(i))+1
     endfor
     if addToAsciiSection gt 0 then writeu,unit,replicate(0b,addToAsciiSection) ; hope it's OK
     point_lun,-unit,current
     pad=current mod 4 & if pad gt 0 then writeu,unit,replicate(0b,pad)
  endif
; write 'offsetted' simple tags  data
  if nsimpletags gt 0 then begin
     for i=0,ntags-1 do begin
        if GeoMethod[tagidx[i]] eq 0 then continue
        writeOffsettedValue,unit,offsettable,g,i,tagidx[i]
     endfor
     point_lun,-unit,current
     pad=current mod 4 & if pad gt 0 then writeu,unit,replicate(0b,pad)
  endif
  flush,unit
  close,unit
end
