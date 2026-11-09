;
; Checks that the HDF4 SD readers reject a dataset whose rank is
; larger than the maximum number of dimensions GDL supports (8),
; with a GDL error, and that a normal dataset still reads fine.
;
; The test file is generated in the temporary directory.
;
; -----------------------------------------------
;
pro TEST_HDF_SD_RANK, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_HDF_SD_RANK, help=help, test=test, no_exit=no_exit'
   return
endif
;
if ~HDF_EXISTS() then begin
   MESSAGE, /continue, "GDL was compiled without HDF4 support."
   EXIT, status=77
endif
;
errors=0
file=FILEPATH('test_hdf_sd_rank.hdf', /TMP)
;
; a valid rank-1 dataset and a rank-9 dataset (one more than supported)
data=INDGEN(5)
sd=HDF_SD_START(file, /CREATE)
sds=HDF_SD_CREATE(sd, 'rank1', [5], /SHORT)
HDF_SD_ADDDATA, sds, data
HDF_SD_ENDACCESS, sds
sds=HDF_SD_CREATE(sd, 'rank9', REPLICATE(1L,9), /SHORT)
HDF_SD_ENDACCESS, sds
HDF_SD_END, sd
;
sd=HDF_SD_START(file, /RDWR)
;
sds=HDF_SD_SELECT(sd, HDF_SD_NAMETOINDEX(sd, 'rank1'))
HDF_SD_GETINFO, sds, NDIMS=ndims, DIMS=dims
if (ndims NE 1) then ERRORS_ADD, errors, 'rank1: bad NDIMS'
if ~ARRAY_EQUAL(dims, [5]) then ERRORS_ADD, errors, 'rank1: bad DIMS'
HDF_SD_GETDATA, sds, back
if ~ARRAY_EQUAL(back, data) then ERRORS_ADD, errors, 'rank1: bad data'
HDF_SD_ENDACCESS, sds
;
sds=HDF_SD_SELECT(sd, HDF_SD_NAMETOINDEX(sd, 'rank9'))
if EXECUTE('HDF_SD_GETINFO, sds, NDIMS=ndims') then $
   ERRORS_ADD, errors, 'rank9: HDF_SD_GETINFO did not fail'
if EXECUTE('HDF_SD_GETDATA, sds, back') then $
   ERRORS_ADD, errors, 'rank9: HDF_SD_GETDATA did not fail'
if EXECUTE('HDF_SD_ADDDATA, sds, data') then $
   ERRORS_ADD, errors, 'rank9: HDF_SD_ADDDATA did not fail'
if EXECUTE('dim=HDF_SD_DIMGETID(sds, 0)') then $
   ERRORS_ADD, errors, 'rank9: HDF_SD_DIMGETID did not fail'
HDF_SD_ENDACCESS, sds
;
HDF_SD_END, sd
FILE_DELETE, file, /quiet
;
BANNER_FOR_TESTSUITE, 'TEST_HDF_SD_RANK', errors
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
