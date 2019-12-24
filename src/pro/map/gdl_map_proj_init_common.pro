; the purpose of all this is to translate IDL-like commands to define
; a specific projection to the osgeo/proj4 equivalent commandline,
; e.g., translate
; proj=map_proj_init(127,CENTER_LON=28.1336,FALSE_EASTING=-70,FALSE_NORTHING=12,SPHERE_RADIUS=1)
; into
; "+proj=hammer +R=1 +lon_0=28.1336 +x_0=-70 +y_0=12"
; passing this expression to proj4 will set up all the projections
; parameters and thus the coordinates transformation.
; This is not finished however. One has to define ancillary values
; such as the limits in lat,lon  and u,v of the projection and the
; pipeline values for the clip, split etc applied to the eventual
; contours and lines that will be plotted using this projection.
; the common below defines all the conversion parameters. 

pro mirror_dictionary,a,b
  keys=a.keys()
  values=a.values()
  b=hash(values,keys)
end

pro gdl_map_proj_init_common
compile_opt idl2, hidden
; set commonflag telling common needs to be initialized or
; not. Otherwise, bang, recursive call.
doInitMapProjectionCommon=1b;
; link to our internal private common
@gdlcommon_mapprojections_common
; find either system or (eventually) current file. impotant to keep
; this as is for easy use of program updating the file projection definition.
if file_test("projDefinitions.sav") then restore,"projDefinitions.sav" else begin
proj4definitionFile=FILEPATH("projDefinitions.sav", ROOT_DIR=!GDL_MAPS_DIR)
restore,proj4definitionFile
endelse

; create compressed uppercase namestring for easy comparison:
compressed_ids1=strupcase(strcompress(proj.fullname, /REMOVE_ALL))
compressed_ids2=strupcase(strcompress(proj.othername, /REMOVE_ALL))
; name of ellipsoids in proj lingua.
ellipsoid_idl=['Clarke 1866','Clarke 1880','Bessel','International 1967','International 1909','WGS 72',$
'Everest','WGS 66','GRS 1980','Airy','Modified Everest','Modified Airy','Walbeck','Southeast Asia','Australian National',$
'Krassovsky','Hough','Mercury 1960','Modified Mercury 1968','Sphere','Clarke IGN','Helmert 1906','Modified Fischer 1960',$
'South American 1969','WGS 84']
ellipsoid_proj=['clrk66','clrk80','bessel','new_intl','intl','WGS72','evrst30','WGS66','GRS80','airy','evrst48','mod_airy','walbeck','SEasia','aust_SA',$
'kras','hough','fschr60','fschr68','sphere','clrk80','helmert','fschr60m','aust_SA','WGS84']

; the options that need to be translated
;; IS_ZONES=  +n=4 +m=  zone num see https://modis-land.gsfc.nasa.gov/MODLAND_grid.html
;; IS_JUSTIFY=i++              ;see above
;; SOM_INCLINATION=i++         ; unknown with proj4
;; SOM_LONGITUDE=i++           ;
;; SOM_PERIOD=i++              ;
;; SOM_RATIO=i++               ;
;; SOM_FLAG=i++                ;
;; ROTATION=i++                ;done elsewhere?

; the readable options
dictionary=hash($
"CENTER_LONGITUDE","+lon_0=",$
"CENTER_LATITUDE","+lat_0=",$
"CENTER_AZIMUTH","+azi=",$              ; check.
"TRUE_SCALE_LATITUDE","+lat_ts=",$
"STANDARD_PARALLEL","+lat_1=",$
"STANDARD_PAR1","+lat_1=",$
"STANDARD_PAR2","+lat_2=",$
"HEIGHT","+h=",$
"SAT_TILT","+tilt=",$ 
"ZONE","+zone=",$
"FALSE_EASTING","+x_0=",$
"FALSE_NORTHING","+y_0=",$
"HOM_LONGITUDE1","+lon_1=",$
"HOM_LONGITUDE2","+lon_2=",$
"HOM_AZIM_ANGLE","+alpha=",$
"HOM_LATITUDE1","+lat_1=",$
"HOM_LATITUDE2","+lat_2=",$
"HOM_AZIM_LONGITUDE","+lonc=",$
"MERCATOR_SCALE","+k0=",$
"SOM_LANDSAT_NUMBER","+lsat=",$
"SOM_LANDSAT_PATH","+path=",$
"OEA_SHAPEM","+m=",$
"OEA_SHAPEN","+n=",$
"OEA_ANGLE","+theta=",$ ; and all the proj4 equivalent keywords:
"LAT_1","+lat_1=",$
"LON_1","+lon_1=",$
"LAT_2","+lat_2=",$
"LON_2","+lon_2=",$
"LAT_3","+lat_3=",$
"LON_3","+lon_3=",$
"LAT_TS","+lat_ts=",$
"LON_0","+lon_0=",$
"_N","+n=",$
"_M","+m=",$
"SWEEP","+sweep=",$
"K_0","+k_0=",$
"W","+W=",$
"THETA","+theta=",$
"ALPHA","+alpha=",$
"GAMMA","+gamma=",$
"LONC","+lonc=",$
"LSAT","+lsat=",$
"PATH","+path=",$
"PLAT_0","+plat_0=",$  ; only for proj=sch !
"PLON_0","+plon_0=",$  ; id
"PHDG_0","+phdg=")     ; ibid

mirror_dictionary,dictionary,yranoitcid

idl_ids=["NONE",$
"STEREOGRAPHIC",$
"ORTHOGRAPHIC",$
"LAMBERTCONIC",$
"LAMBERTAZIMUTHAL",$
"GNOMIC",$
"AZIMUTHALEQUIDISTANT",$
"SATELLITE",$
"CYLINDRICAL",$
"MERCATOR",$
"MOLLWEIDE",$
"SINUSOIDAL",$
"AITOFF",$
"HAMMERAITOFF",$
"ALBERSEQUALAREACONIC",$
"TRANSVERSEMERCATOR",$
"MILLERCYLINDRICAL",$
"ROBINSON",$
"LAMBERTELLIPSOIDCONIC",$
"GOODESHOMOLOSINE",$
"GEOGRAPHIC",$
"GCTP_UTM",$
"GCTP_STATEPLANE",$
"GCTP_ALBERSEQUALAREA",$
"GCTP_LAMBERTCONFORMALCONIC",$
"GCTP_MERCATOR",$
"GCTP_POLARSTEREOGRAPHIC",$
"GCTP_POLYCONIC",$
"GCTP_EQUIDISTANTCONIC",$
"GCTP_TRANSVERSEMERCATOR",$
"GCTP_STEREOGRAPHIC",$
"GCTP_LAMBERTAZIMUTHA",$
"GCTP_AZIMUTHAL",$
"GCTP_GNOMONIC",$
"GCTP_ORTHOGRAPHIC",$
"GCTP_NEARSIDEPERSPECTIVE",$
"GCTP_SINUSOIDAL",$
"GCTP_EQUIRECTANGULAR",$
"GCTP_MILLERCYLINDRICAL",$
"GCTP_VANDERGRINTEN",$
"GCTP_HOTINEOBLIQUEMERCATOR",$
"GCTP_ROBINSON",$
"GCTP_SPACEOBLIQUEMERCATOR",$
"GCTP_ALASKACONFORMAL",$
"GCTP_INTERRUPTEDGOODE",$
"GCTP_MOLLWEIDE",$
"GCTP_INTERRUPTEDMOLLWEIDE",$
"GCTP_HAMMER",$
"GCTP_WAGNERIV",$
"GCTP_WAGNERVII",$
"GCTP_OBLATEDEQUALAREA",$
"GCTP_INTEGERIZEDSINUSOIDAL",$
"GCTP_CYLINDRICALEQUALAREA"]
idl_equiv=['null','stere','ortho','lcc','laea',$
'gnom',$
'aeqd',$
'tpers',$
'eqc',$
'merc',$
'moll',$
'sinu',$
'aitoff',$
'hammer',$
'aea',$
'tmerc',$
'mill',$
'robin',$
'lcc',$
'igh',$
'eqc',$
'utm +ellps=WGS84',$ 
'gnom',$ ;stateplane
'aea',$
'lcc',$
'merc',$
'merc',$ ;'ups +ellps=WGS84',$ ; ups, elliptical usage
'poly',$
'eqdc',$
'tmerc',$
'stere',$
'laea',$
'aeqd',$
'gnom',$
'ortho',$
'nsper',$
'sinu',$
'eqc',$
'mill',$
'vandg',$
'omerc',$
'robin',$
'lsat',$
'alsk',$
'igh',$
'moll',$
'moll',$
'hammer',$
'wag4',$
'wag7',$
'oea',$
'sinu',$
'cea']

end
