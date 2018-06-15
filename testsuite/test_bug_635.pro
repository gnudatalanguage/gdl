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
; https://sourceforge.net/p/gnudatalanguage/bugs/635/
; this bug was added in the testsuite in 2015 by Gilles 
; but not put in the testsuite/Makefile.am
;
; this bug is present in GDL 0.9.4 CVS;
; should be corrected in GDL 0.9.5 CVS after 2015-06-11 (tested)
;
;==========================================
;
pro TEST_BUG_635, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_BUG_635, help=help, verbose=verbose, $'
   print, '                  no_exit=no_exit, test=test'
   return
endif
;
; data to be read back from the data file "test_bug_635.dat"
;
data=FLTARR(5,3)
expected_data=FINDGEN(5,3)
;
city={Population:0L,Latitude:0.0,Longitude:0.0,Elevation:0L}
group=REPLICATE(city,2)
;
expected_city={Population:1000000L,Latitude:45.2,Longitude:72.9,Elevation:300L}
expected_group=REPLICATE(expected_city,2)
;
;Open the fortran-generated file. The F77_UNFORMATTED keyword is
;necessary so that IDL will know that the file contains unformatted
;data produced by a UNIX FORTRAN program.
;
input_file='test_bug_635.dat'
;
if ~FILE_TEST(input_file) then begin
   MESSAGE, /CONTINUE, 'MISSING file : '+input_file
   if KEYWORD_SET(no_exit) then STOP else EXIT, status=77
endif
;
OPENR, lun, 'test_bug_635.dat', /GET_LUN, /F77_UNFORMATTED
;
;Read the data in a single input operation.
;
READU, lun, data
READU, lun, group
;
;Release the logical unit number and close the fortran generated data file.
FREE_LUN, lun
;
; if requested, print the result.
;
if KEYWORD_SET(verbose) then begin
   print,'--- read back DATA ---:'
   print, data
   print,''
   print,'--- read back STRUCTURE ---'
   print,'Population:', group[1].Population
   print,'Elevation: ', group[1].Elevation
endif
;
nb_errors=0
;
; first, verifying the "data"
;
if ARRAY_EQUAL(expected_data, data) NE 1 then begin
   ERRORS_ADD, nb_errors, 'Problem in read back DATA array'
   ;;
   ;; can we detail the problem ?
   ;;
   if ARRAY_EQUAL(SIZE(expected_data),SIZE(data)) NE 1 then begin
      BANNER_FOR_TESTSUITE, 'TEST_BUG_635', /short, verb=verbose, $
                            'size of read back DATA array wrong'
   endif else begin
      if (TOTAL(ABS(expected_data-data)) GT 0.) then begin
         BANNER_FOR_TESTSUITE, 'TEST_BUG_635', /short, verb=verbose, $
                               'content of read DATA differents than expected'
         if KEYWORD_SET(verbose) then begin 
            print,'--- EXPECTED DATA ---:'
            print, expected_data & print,''
         endif else begin
            print, 'Please rerun this test with keyword /verbose !'
         endelse
      endif
   endelse
endif
;
; second, verifying the "structure"
;
if ARRAY_EQUAL(SIZE(expected_group),SIZE(group)) NE 1 then begin
   ERRORS_ADD, nb_errors, 'Problem in read back GROUP structure'
   BANNER_FOR_TESTSUITE, 'TEST_BUG_635', /short, verb=verbose, $
                         'SIZE() of read back GROUP structure wrong'
endif else begin
   ;; debug purpose : uncomment next line do add one fake error
   ;; expected_group[1].(0)++
   ;;
   ;; global size OK, testing the content ...
   for ii=0, 1 do begin      
      for jj=0, N_TAGS(expected_group)-1 do begin
         g=group[ii].(jj)
         eg=expected_group[ii].(jj)
         txt=STRCOMPRESS(' ('+STRING(ii)+','+STRING(jj)+')')
         if (g NE eg) then begin
            ERRORS_ADD, nb_errors, 'Bad value for field'+txt
            if KEYWORD_SET(verbose) then begin
               print, 'field name : ', (TAG_NAMES(expected_group[ii]))[jj]
               print, 'expected value  :', expected_group[ii].(jj)
               print, 'read back value :', group[ii].(jj)
            endif else begin
               print, 'Please rerun this test with keyword /verbose !'
            endelse
         endif
         if ARRAY_EQUAL(SIZE(g),SIZE(eg)) NE 1 then begin
            ERRORS_ADD, nb_errors, 'Type of field'+txt
         endif
      endfor
   endfor   
endelse
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_BUG_635', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
