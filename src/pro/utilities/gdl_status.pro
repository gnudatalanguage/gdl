;
; Alain, 31-Oct-2017
;
; To be extended ...
; Return Version number for Libs. when possible
;
; The IM/GM Magick flag need to be clarify
;
pro GDL_STATUS
;
print, 'Eigen3 ? : ', EIGEN_EXISTS()
print, 'FFTw ?   : ', FFTW_EXISTS()
print, 'GRIB ?   : ', GRIB_EXISTS()
print, 'GLPK ?   : ', GLPK_EXISTS()
print, 'GSHHG ?  : ', GSHHG_EXISTS()
print, 'HDF ?    : ', HDF_EXISTS()
print, 'HDF5 ?   : ', HDF5_EXISTS()
print, 'Magick ? : ', MAGICK_EXISTS()
print, 'NetCDf ? : ', NCDF_EXISTS()
print, 'OpenMP ? : ', OPENMP_EXISTS()
print, 'Proj4 ?  : ', PROJ4_EXISTS()
print, 'Proj4 new: ', PROJ4NEW_EXISTS()
print, 'PSlib ?  : ', PSLIB_EXISTS()
print, 'Python ? : ', PYTHON_EXISTS()
print, 'UDUNITS  : ', UDUNITS_EXISTS()
print, 'wxwidgets: ', WXWIDGETS_EXISTS()
;
end
