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
; 2020-Mar-30 : AC. 
; -- I clearly prefer keyword /missing ! (more nmemonic)
;
; 2021-Aug-03 : AC.
; -- it was not a good idea to use GDL_IDL_FL() here, removing it.
;
; ---------------------------------------------------
;
pro IPRINT, ii, txt, status, skip
;
if skip then begin
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
pro GDL_STATUS, missing=missing, only_off=only_off, $
                test=test, verbose=verbose, help=help
;
FORWARD_FUNCTION DSFMT_EXISTS, EIGEN_EXISTS, EXPAT_EXISTS, $
   FFTW_EXISTS, GEOTIFF_EXISTS, $
   GLPK_EXISTS, GRIB_EXISTS, GSHHG_EXISTS, HDF5_EXISTS, HDF_EXISTS, $
   MAGICK_EXISTS, MPI_EXISTS, NCDF4_EXISTS, NCDF_EXISTS, $
   OPENMP_EXISTS, PNGLIB_EXISTS, $
   PROJ_EXISTS, PYTHON_EXISTS, SHAPELIB_EXISTS, $
   TIFF_EXISTS, UDUNITS_EXISTS, WXWIDGETS_EXISTS, X11_EXISTS
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, 'pro GDL_STATUS, missing=missing, only_off=only_off, $'
   print, '                test=test, verbose=verbose help=help'
   return
endif
;
DEFSYSV, '!gdl', exist=exist
if ~exist then MESSAGE, 'This code can be run only under GDL !'
;
; counting ...
i=1
;
skip=0
if KEYWORD_SET(missing) OR KEYWORD_SET(only_off) then skip=1 
;
IPRINT, i, 'DSFMT ?  : ', DSFMT_EXISTS(), skip
IPRINT, i, 'Eigen3 ? : ', EIGEN_EXISTS(), skip
IPRINT, i, 'Expat ?  : ', EXPAT_EXISTS(), skip
IPRINT, i, 'FFTw ?   : ', FFTW_EXISTS(), skip
IPRINT, i, 'GLPK ?   : ', GLPK_EXISTS(), skip
IPRINT, i, 'GRIB ?   : ', GRIB_EXISTS(), skip
IPRINT, i, 'HDF ?    : ', HDF_EXISTS(), skip
IPRINT, i, 'HDF5 ?   : ', HDF5_EXISTS(), skip
IPRINT, i, 'Magick ? : ', MAGICK_EXISTS(), skip
IPRINT, i, 'MPI ?    : ', MPI_EXISTS(), skip
IPRINT, i, 'NetCDF ? : ', NCDF_EXISTS(), skip
IPRINT, i, 'NetCDF4 ?: ', NCDF4_EXISTS(), skip
IPRINT, i, 'OpenMP ? : ', OPENMP_EXISTS(), skip
IPRINT, i, 'PNGLIB ? : ', PNGLIB_EXISTS(), skip
IPRINT, i, 'PROJ ?   : ', PROJ_EXISTS(), skip
IPRINT, i, 'Python ? : ', PYTHON_EXISTS(), skip
IPRINT, i, 'ShapeLib : ', SHAPELIB_EXISTS(), skip
IPRINT, i, 'TIFF ?   : ', TIFF_EXISTS(), skip
IPRINT, i, 'GEOTIFF ?: ', GEOTIFF_EXISTS(), skip
IPRINT, i, 'UDUNITS  : ', UDUNITS_EXISTS(), skip
IPRINT, i, 'WxWidgets: ', WXWIDGETS_EXISTS(), skip
IPRINT, i, 'X11      : ', X11_EXISTS(), skip
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

