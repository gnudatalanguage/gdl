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
print, 'Eigen3 ? : ', eigen_exists()
print, 'HDF ? : ', hdf_exists()
print, 'NetCDf ? : ', ncdf_exists()
print, 'HDF5 ? : ', hdf5_exists()
print, 'GRIB ? : ', grib_exists()
print, 'GLPK ? : ', glpk_exists()
;
end
