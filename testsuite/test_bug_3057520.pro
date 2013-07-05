;
; test related to NetCDF capabilities 
;
pro TEST_BUG_3057520
;
if ~NCDF_EXISTS() then begin
   MESSAGE, /continue, 'GDL was compiled without NetCDF !'
   EXIT, status=77
endif
;
; creating a NetCDF file with few fields
;
nc = NCDF_CREATE('/dev/null', /clobber)
dt = NCDF_DIMDEF(nc, 'T', /unlimited)
dx = NCDF_DIMDEF(nc, 'X', 1024)
va = NCDF_VARDEF(nc, 'A', [dx, dt])
NCDF_CONTROL, nc, /endef
;
if EXECUTE("NCDF_VARPUT, nc, va, [1], offset=0") eq 1 then EXIT, status=1
if EXECUTE("NCDF_VARPUT, nc, va, [1], offset=[0,0,0]") ne 1 then EXIT, status=1
;
end
