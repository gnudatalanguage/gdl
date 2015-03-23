; IDL/GDL File
;==========================================
;
; FILE:     test_bug_635.pro
; USAGE: ---
; DESCRIPTION: ---
; BUGS: ---
;
; AUTHOR:   mchekki
; ORGANIZATION: ---
;
; VERSION: ---
; CREATED:  2015-03-18 12:36:37
; MODIFIED: 2015-03-18 14:29:38
;
;==========================================
;Create an array to contain the fortran array.
pro test_bug_635

data = FLTARR(5,3)

city={Population:0L,Latitude:0.0,Longitude:0.0,Elevation:0L}
group=replicate(city,2)

;Open the fortran-generated file. The F77_UNFORMATTED keyword is

;necessary so that IDL will know that the file contains unformatted

;data produced by a UNIX FORTRAN program.

OPENR, lun, 'test_bug_635.dat', /GET_LUN, /F77_UNFORMATTED

;Read the data in a single input operation.

READU, lun, data
READU, lun, group

;Release the logical unit number and close the fortran file.

FREE_LUN, lun

;Print the result.

PRINT,'--- DATA ---:', data
PRINT,''
PRINT,''
PRINT,'--- STRUCTURE ---'
PRINT,''
PRINT,'Population:', group[1].Population
PRINT,'Elevation: ', group[1].Elevation

end
