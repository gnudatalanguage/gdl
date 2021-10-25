;
; Tests for QHull and its related functions
;
; Eloi R. de Linage, July 2021 
;
; APPEND_POINTS_TESTQ concatenates two set of points formated for qhull
;
function APPEND_POINTS_TEST_QHULL, a, b
    return, transpose( [ transpose(a), transpose(b) ] )
end
;
; MAKE_CUBE_QHULL generates an origin centered unit cube in N dimensions 
;
function MAKE_CUBE_QHULL, n_dim
    n_vert=2^n_dim
    cube=make_array(n_dim,n_vert)
    for i=0, n_dim-1 do begin
        for j=0, n_vert-1 do begin
            cube[i,j]=((j/(2^i)) MOD 2)-0.5
        endfor
    endfor
    return, cube
end
;
; GEN_TEST_QHULL generates custom input array
; based on a qhull test script by Evgenii Rubtsov
;
function GEN_TEST_QHULL, regular=regular, random=random, range=range, radial=radial

xarr = !null
if ~((n_elements(regular) gt 0l) xor (n_elements(random) eq 1)) then message, "undefined REGULAR/RANDOM"
if (n_elements(range) eq 0l) then message, "undefined RANGE"

size_range = size(range)
if ((size_range[0] eq 2) and (size_range[1] eq 2)) then n_xarr = size_range[2] else message, "incorrect RANGE format"
m_xarr = (keyword_set(regular))? product(regular) : random

xarr = fltarr(n_xarr,m_xarr) + !values.f_nan
if (keyword_set(regular)) then begin
    for i_xarr=0l, n_xarr-1l do begin
        xarr[i_xarr,*] = reform(rebin( $
            findgen((i_xarr gt 0l)? [lonarr(i_xarr)+1,regular[i_xarr]] : regular[i_xarr])/(regular[i_xarr]-1l) $
            * (range[1,i_xarr]-range[0,i_xarr]) + range[0,i_xarr], regular))
    endfor
endif else if (keyword_set(random)) then begin
    for i_xarr=0l, n_xarr-1l do xarr[i_xarr,*] = randomu(seed,m_xarr) * (range[1,i_xarr]-range[0,i_xarr]) + range[0,i_xarr]
endif

if (keyword_set(radial)) then begin
    tmp_xarr = xarr
    for i_xarr=0l, n_xarr-1l do begin
        tmp = xarr[0,*]
        if (n_xarr gt 1l) then begin
            if (i_xarr eq 0l) then begin
                tmp *= product(cos(xarr[i_xarr+1l:*,*]),1)
            endif else if ((i_xarr gt 0l) and (i_xarr lt n_xarr-1l)) then begin
                tmp *= (product(sin(xarr[i_xarr,*]),1) * product(cos(xarr[i_xarr:*,*]),1))
            endif else if (i_xarr eq n_xarr-1l) then begin
                tmp *= sin(xarr[i_xarr,*])
            endif
        endif
        tmp_xarr[i_xarr,*] = temporary(tmp)
    endfor
    xarr = temporary(tmp_xarr)
endif

return, xarr
end
;
; Checking QHULL connectivity validity in N-D (see QHULL IDL doc)
;
pro TEST_CONNECTIVITY_QHULL, in, cumul_errors, test=test, quiet=quiet
errors=0
qhull, in, triang, conn=conn, /DELAUNAY
start_ix=conn[0]
max_in_ix=(size(in))[2]-1
max_conn_ix=(size(conn))[2]-1
error_count=0
for i=start_ix, max_conn_ix do begin
    if (conn[i] LT 0 || conn[i] GT max_in_ix) then error_count++
end

if error_count GT 0 then ERRORS_ADD, errors, 'ONNECTIVITY invalid result (VARNAME: '+SCOPE_VARNAME(in)+')'

if ~KEYWORD_SET(quiet) then BANNER_FOR_TESTSUITE, 'TEST_CONNECTIVITY_QHULL', errors, /status
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
end
;
; Checking QHULL vdiagrams validity in 2D (see QHULL IDL doc)
;
pro TEST_VDIAGRAMS_QHULL, in, cumul_errors, test=test, quiet=quiet
errors=0
qhull, in, triang, vvert=vvert, vdiag=vdiag, vnorm=vnorm, /DELAUNAY
max_vvert_ix=(size(vvert))[2]-1
max_in_ix=(size(in))[2]-1
max_vnorm_ix=(size(vnorm))[2]-1
max_vdiag_ix=(size(vdiag))[2]-1
error_1_count=0
error_2_count=0
for i=0, max_vdiag_ix do begin
    if (( vdiag[0,i] LT 0 || vdiag[1,i] LT 0 ) || ( vdiag[0,i] GT max_in_ix || vdiag[1,i] GT max_in_ix )) then begin
        error_1_count++
    endif
    if ((vdiag[2,i] GT max_vvert_ix || vdiag[3,i] GT max_vvert_ix) || (vdiag[2,i] LT (-max_vnorm_ix-1)) || vdiag[3,i] LT (-max_vnorm_ix-1)) then begin
        error_2_count++
    endif
endfor

if error_1_count GT 0 then ERRORS_ADD, errors, 'VDIAGRAMS invalid result error 1 (VARNAME: '+SCOPE_VARNAME(in)+') out of input bounds'
if error_2_count GT 0 then ERRORS_ADD, errors, 'VDIAGRAMS invalid result error 2 (VARNAME: '+SCOPE_VARNAME(in)+') out of VNORMALS bounds'

if ~KEYWORD_SET(quiet) then BANNER_FOR_TESTSUITE, 'TEST_VDIAGRAMS_QHULL', errors, /status
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
end
;
; Checking QHULL vdiagrams validity in N-D  with N >= 3 (see QHULL IDL doc)
;
pro TEST_VDIAGRAMS_QHULL_ND, in, cumul_errors, test=test, quiet=quiet
errors=0
qhull, in, triang, vvert=vvert, vdiag=vdiag, vnorm=vnorm, /DELAUNAY

max_vvert_ix=(size(vvert))[2]-1
max_in_ix=(size(in))[2]-1
max_vnorm_ix=(size(vnorm))[2]-1
max_vdiag_ix=(size(vdiag))[2]-1

vridge_index=0
n_params=0

error_1_count=0
error_2_count=0
error_3_count=0

while vridge_index LT max_vdiag_ix do begin
    n_params=vdiag[vridge_index]
    for i=1, 2 do begin
        if (vdiag[vridge_index + i] LT 0) || (vdiag[vridge_index + i] GT max_in_ix) then begin
            error_1_count++
        endif
    endfor
    for i=3, n_params do begin
        if(vdiag[vridge_index + i] GE 0) then begin
            if (vdiag[vridge_index + i] GT max_vvert_ix) then begin
                error_2_count++
            endif
        endif else begin
            if ((-vdiag[vridge_index + i]-1) GT max_vnorm_ix) then begin
                error_3_count++
            endif
        endelse
    endfor
    vridge_index = vridge_index + n_params + 1
endwhile

if error_1_count GT 0 then ERRORS_ADD, errors, 'VDIAGRAMS invalid result error 1 (VARNAME: '+SCOPE_VARNAME(in)+') out of input bounds'
if error_2_count GT 0 then ERRORS_ADD, errors, 'VDIAGRAMS invalid result error 2 (VARNAME: '+SCOPE_VARNAME(in)+') out of VVERTICES bounds'
if error_3_count GT 0 then ERRORS_ADD, errors, 'VDIAGRAMS invalid result error 3 (VARNAME: '+SCOPE_VARNAME(in)+') out of VNORMALS bounds'

if ~KEYWORD_SET(quiet) then BANNER_FOR_TESTSUITE, 'TEST_VDIAGRAMS_QHULL', errors, /status
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
end
;
; Tests for QHULL procedure, QGRID3() function
;
pro TEST_QHULL, test=test, quiet=quiet, help=help, no_exit=no_exit

if KEYWORD_SET(help) then begin
    print, 'pro TEST_QHULL, test=test, quiet=quiet, help=help, no_exit=no_exit'
    return
endif

cumul_errors=0
error_level=1e-6
errors=0

;; --- QHULL ---

;; Generating some sets of points
; Hexagon + center + outer vertex
hexa=make_array(2,8)
angle=0.0
for i=1, 6 do begin
	hexa[0,i] = cos(angle)
	hexa[1,i] = sin(angle)
	angle += (!pi)/3
endfor
hexa[1,7]=2

; Set of random points...
in2=gen_test_qhull(random=123, range=[[0.5,1.],[[0.,1.]*!pi]], /rad) ;2D
in3=gen_test_qhull(random=321, range=rebin([-1.,1.],[2,4])) ;4D

;; --- Check vdiagrams validity (in 2-D) ---
TEST_VDIAGRAMS_QHULL, hexa, cumul_errors, /quiet
TEST_VDIAGRAMS_QHULL, in2, cumul_errors, /quiet
;; --- Check vdiagrams validity (in 4-D) ---
TEST_VDIAGRAMS_QHULL_ND, hexa, cumul_errors, /quiet

;; --- Check connectivity validity ---
TEST_CONNECTIVITY_QHULL, hexa, cumul_errors, /quiet
TEST_CONNECTIVITY_QHULL, in2, cumul_errors, /quiet
TEST_CONNECTIVITY_QHULL, in3, cumul_errors, /quiet

;; --- Convex hull of N-D hypercube containing smaller cube ---
exp_hull_size=[4,12,59,398,3725]
for ND=2, 5 do begin
    in=APPEND_POINTS_TEST_QHULL(10*MAKE_CUBE_QHULL(ND), 3.33*MAKE_CUBE_QHULL(ND))
    qhull, in, hull
    if (size(hull))[2] NE exp_hull_size[ND-2] then begin
        ERRORS_ADD, errors, 'Incorrect number of points ('+strtrim((size(hull))[2],2)+') in convex hull for '+strtrim(ND,2)+'-D hypercube ('+strtrim(exp_hull_size[ND-2],2)+' expected)'
    endif
endfor

;; --- Playing with inputs ---

; A) Separated and concatenaetd input should produce *exactly* equal results
qhull, in2, triang_a, vvert=vvert_a, vdiag=vdiag_a, vnorm=vnorm_a, conn=conn_a, /DELAUNAY
qhull, transpose(in2[0,*]), transpose(in2[1,*]), triang_b, vvert=vvert_b, vdiag=vdiag_b, vnorm=vnorm_b, conn=conn_b, /DELAUNAY

if total(abs(triang_a-triang_b)) NE 0. then ERRORS_ADD, errors, 'Results are diff. if input point set is separated, for DELAUNAY keyword'
if total(abs(vvert_a-vvert_b)) NE 0. then ERRORS_ADD, errors, 'Results are diff. if input point set is separated, for VVERTICES keyword'
if total(abs(vnorm_a-vnorm_b)) NE 0. then ERRORS_ADD, errors, 'Results are diff. if input point set is separated, for VNORMALS keyword'
if total(abs(conn_a-conn_b)) NE 0. then ERRORS_ADD, errors, 'Results are diff. if input point set is separated, for CONNECTIVITY keyword'
if total(abs(vdiag_a-vdiag_b)) NE 0. then ERRORS_ADD, errors, 'Results are diff. if input point set is separated, for VDIAGRAMS keyword'

; B) Feeding with spicy NaN and Inf. values
in4 = 100*randomu(12345,3,100)-50
in4_bad = in4 & in4_bad[150] = !values.f_nan
if execute('qhull, in4_bad, res') NE 0 then ERRORS_ADD, errors, 'Qhull should not run with a NaN in input!!!'
in4_bad = in4 & in4_bad[150] = !values.f_infinity
if execute('qhull, in4_bad, res') NE 0 then ERRORS_ADD, errors, 'Qhull should not run with an Inf in input!!!'

; C) Feeding with coordinates arrays of different size
in5_x=findgen(50)
in5_y=randomu(1,49)
in5_z=15.24 + make_array(50)
if execute('qhull, in5_x, in5_y, in5_z, res') NE 0 then ERRORS_ADD, errors, 'Qhull should not run with input arrays of different size!!!'

;; --- QGRID3, only 3D point sets ---
; Box filled with random points
in1=APPEND_POINTS_TEST_QHULL(100*MAKE_CUBE_QHULL(3), 100*randomu(12,3,100)-5)
x=transpose(in1[0,*]) & y=transpose(in1[1,*]) & z=transpose(in1[2,*])
qhull, x,y,z, triang, /DELAUNAY
f=2.1*x-3*y+!PI*z
res=QGRID3(x, y, z, f, triang, START=-4, DIMENSION=9, DELTA=1) ;box of 9*9 points contained in [-4,4]
; Since f is a purely linear function, qgrid3 interpolation should be -near- perfect
res_exp=make_array(9,9,9)
for i=0, 8 do begin
  for j=0, 8 do begin
    for k=0,8 do begin
    res_exp(i,j,k) = 2.1d*(i-4)-3d*(j-4)+!PI*(k-4)
    endfor
  endfor
endfor

if total((res-res_exp)*(res-res_exp)) GT 1E-8 then  ERRORS_ADD, errors, 'Qgrid3 numerical error 1'


; ------------------- final message ------------------
ERRORS_CUMUL, cumul_errors, errors
BANNER_FOR_TESTSUITE, 'TEST_QHULL', cumul_errors
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
if KEYWORD_SET(test) then STOP    
end
