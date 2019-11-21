;
; Alain, 31-Oct-2017
;
; To be extended ...
; Return Version number for Libs. when possible
;
; The IM/GM Magick flag need to be clarify
;
; ----------------------------------------------------
; Modifications history :
;
; 2019-Feb-15 : AC. revision 
; -- we can display only the non active options
; -- we cross check with the internal list ...
;
; 2019-Nov-18 : AC. 
; -- updating the list of EXISTS_ functs.
;
; ---------------------------------------------------
;
pro IPRINT, ii, txt, status, only_off=only_off
;
if KEYWORD_SET(only_off) then begin
   if status EQ 1 then begin
      ii++
      return
   endif
endif
print, STRING(ii, ' ',format='(i2, A1)'), txt, status
ii++
end
;
; ---------------------------------------------------
;
pro GDL_STATUS, test=test, verbose=verbose, only_off=only_off
;
FORWARD_FUNCTION DSFMT_EXISTS, EIGEN_EXISTS, FFTW_EXISTS, GEOTIFF_EXISTS, $
   GLPK_EXISTS, GRIB_EXISTS, GSHHG_EXISTS, HDF5_EXISTS, HDF_EXISTS, $
   MAGICK_EXISTS, NCDF4_EXISTS, NCDF_EXISTS, OPENMP_EXISTS, PNGLIB_EXISTS, $
   PROJ4NEW_EXISTS, PROJ4_EXISTS, PSLIB_EXISTS, PYTHON_EXISTS, $
   TIFF_EXISTS, UDUNITS_EXISTS, WXWIDGETS_EXISTS, X11_EXISTS
;
ON_ERROR, 2
;
if GDL_IDL_FL(/uppercase) NE 'GDL' then $
   MESSAGE, 'This code can be run only under GDL !'
;
; counting ...
i=1
;
IPRINT, i, 'DSFMT ?  : ', DSFMT_EXISTS(), only_off=only_off
IPRINT, i, 'Eigen3 ? : ', EIGEN_EXISTS(), only_off=only_off
IPRINT, i, 'Expat ?  : ', EXPAT_EXISTS(), only_off=only_off
IPRINT, i, 'FFTw ?   : ', FFTW_EXISTS(), only_off=only_off
IPRINT, i, 'GEOTIFF ?: ', GEOTIFF_EXISTS(), only_off=only_off
IPRINT, i, 'GLPK ?   : ', GLPK_EXISTS(), only_off=only_off
IPRINT, i, 'GRIB ?   : ', GRIB_EXISTS(), only_off=only_off
IPRINT, i, 'HDF ?    : ', HDF_EXISTS(), only_off=only_off
IPRINT, i, 'HDF5 ?   : ', HDF5_EXISTS(), only_off=only_off
IPRINT, i, 'Magick ? : ', MAGICK_EXISTS(), only_off=only_off
IPRINT, i, 'NetCDF ? : ', NCDF_EXISTS(), only_off=only_off
IPRINT, i, 'NetCDF4 ?: ', NCDF4_EXISTS(), only_off=only_off
IPRINT, i, 'OpenMP ? : ', OPENMP_EXISTS(), only_off=only_off
IPRINT, i, 'PNGLIB ? : ', PNGLIB_EXISTS(), only_off=only_off
IPRINT, i, 'Proj4 ?  : ', PROJ4_EXISTS(), only_off=only_off
IPRINT, i, 'Proj4 new: ', PROJ4NEW_EXISTS(), only_off=only_off
IPRINT, i, 'PSlib ?  : ', PSLIB_EXISTS(), only_off=only_off
IPRINT, i, 'Python ? : ', PYTHON_EXISTS(), only_off=only_off
IPRINT, i, 'ShapeLib : ', SHAPELIB_EXISTS(), only_off=only_off
IPRINT, i, 'TIFF ?   : ', TIFF_EXISTS(), only_off=only_off
IPRINT, i, 'UDUNITS  : ', UDUNITS_EXISTS(), only_off=only_off
IPRINT, i, 'WxWidgets: ', WXWIDGETS_EXISTS(), only_off=only_off
IPRINT, i, 'X11      : ', X11_EXISTS(), only_off=only_off
;
; How many _EXISTS() founctions should we have ?
;
HELP, /lib, out=out
ii_list=STRPOS(out, '_EXISTS')
list=out[WHERE(ii_list GE 0)]
;
i--
if (i EQ N_ELEMENTS(list)) then begin
   print, 'All _EXISTS() functions have been detected !'
endif else begin
   print, 'Some _EXISTS() functions are missing !'
   print, format='(A18, i3)', 'Expected      : ', N_ELEMENTS(list)
   print, format='(A18, i3)', 'Really tested : ', i
   if KEYWORD_SET(verbose) then begin
      print, 'Expected list : ', list
   endif
endelse
;
if KEYWORD_SET(test) then STOP
;
end
