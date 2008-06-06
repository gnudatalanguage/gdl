; NG  27/05/2008
; test procedures for Intrinsic GDL voigt function
; If you find bugs, limitations, other interresting cases,
; please report them to Alain Coulais : alaingdl@users.sourceforge.net
;--------------------------------------------------------------------
;
;	I-Compatibility with IDL syntax 
;	II-Plot of voigt function
;	III-Comparison with Armstrong et al. VOIGT values
;	IV-Test with negative numbers 
;	V-Test with not a number and infinite values
;
;
;
;
;
;-----------------------------------------------------------------------
; 		I-Compatibility with IDL syntax
;-----------------------------------------------------------------------
pro test_voigt_behaviour


;1. "arguments are both scalars" (take a look at III-Comparison with Armstrong)

print,'------------------------------------------------------------'				     
print,'If both arguments are scalars, the function returns a scalar'
print,'------------------------------------------------------------'
print,'A = 1.705 et U = 0.04'
A=1.705
U=0.04
r=voigt(A,U)
print ,'r=voigt(A,U)=',voigt(A,U)	
help,r


;2. "arguments are both arrays"

print,'-----------------------------------------------------------------------------'		
print,'If both arguments are arrays returns array with the smallest array dimensions'
print,'-----------------------------------------------------------------------------'

;	2-1."With same dimensions"

print,'----- Same dimensions -----'
print,'A=[ 5 , 4 , 3.22 , 1.0005 ] et U=[ 1.002 , 2.003 , 5.106 , 9.0025 ]'
A=[ 5 , 4 , 3.22 , 1.0005 ]
U=[ 1.002 , 2.003 , 5.106 , 9.0025 ]
r=voigt(A,U)
print ,'r=voigt(A,U)=',voigt(A,U)	
help,r
print,''                        ;use for better display

;	2-2."With different dimensions"

print,'----- Different dimensions -----'

print,'A=[ 5 , 3.22 , 1.0005 ] et U=[ 1.002 , 2.003 , 5.106 , 9.0025 ]'
A=[ 5 , 3.22 , 1.0005 ]
U=[ 1.002 , 2.003 , 5.106 , 9.0025 ] 
r=voigt(A,U)
print ,'voigt(A,U)=',voigt(A,U)	
help,r

print,''                        ;use for better display

print,'A=[5 , 3.22 , 1.0005 ] et U=[ 1.002 , 2.003 ]'
A=[ 5, 3.22 , 1.0005 ]
U=[1.002, 2.003] 
r=voigt(A,U)
print ,'r=voigt(A,U)=',voigt(A,U)	
help,r

print,''                        ;use for better display


;3. "One argument is a scalar and the other is an array

print,'-----------------------------------------------------------------------------------'		
print,'If arguments are one array and scalar and returns array with the smallest array dim'
print,'-----------------------------------------------------------------------------------'	    

print,'A=[ 5 , 3.22 , 1.0005 ] et U=5.00'
A=[5 , 3.22 , 1.0005 ]
U=5.00 
r=voigt(A,U)
print ,'r=voigt(A,U)=',voigt(A,U)	
help,r

print,''                        ;use for better display


print,'A=4.002 et U=[ 5.00 , 2.003 , 4 , 6 ]'
U=[ 5.00 , 2.003 , 4 , 6 ]
A=4.002 
r=voigt(A,U)
print ,'r=voigt(A,U)=',voigt(A,U)	
help,r

print,''                        ;use for better display


;4. "Argument A is a double" 

print,'--------------------------------------------------------------------------------'		
print,'If argument A is a double returns double'
print,'--------------------------------------------------------------------------------'

print,'A=4.00000d et U=[ 5.00 , 2.003 , 4 , 6 ]'
A=4.00000d 
U= [ 5.00 , 2.003 , 4 , 6 ]
r=voigt(A,U)
print ,'r=voigt(A,U)=',voigt(A,U)	
help,r

print,''                        ;use for better display

print,'A=[ 1.4002 , 7.000900d , 4.000 ] et U=[ 5.00 , 2.003 , 4 , 6 ]'
A=[1.4002 , 7.000900d , 4.000 ] 
U=[ 5.00 , 2.003 , 4 , 6 ]
r=voigt(A,U)
print ,'r=voigt(A,U)=',voigt(A,U)	
help,r

print,''

print,'A=[ 1.4002 , 7.000900 , 4.000 ] et U=[ 5.00d , 2.003d , 4 , 6 ]'
A=[1.4002 , 7.000900 , 4.000 ] 
U=[ 5.00d , 2.003d , 4 , 6 ]
r=voigt(A,U)
print ,'r=voigt(A,U)=',voigt(A,U)	
help,r
end





;-----------------------------------------------------------------------
; 	 	 II-Plot of voigt function
;-----------------------------------------------------------------------
pro test_voigt_plot

x=findgen(2000)/10-100
a=[0.01, 0.1, 0.25, .5, 1., 2., 4., 8., 16., 32., 64., 128.]
nb_a=N_ELEMENTS(a)

window, 0, xsize=800, ysize=600
!p.multi=[0,3,4]

for ii=0, nb_a-1 do begin  
    plot, x, VOIGT(a[ii],x), title = 'VOIGT function with damper = '+ STRING( a[ii])
end 

end
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
;  		III-Comparison with Armstrong et al. VOIGT values
;-----------------------------------------------------------------------
pro test_voigt_ArmstrongValues

print,'-------------------------------------------------'  ;use for better display
print,'Comparison between Armstrong and GDL VOIGT values'
print,'-------------------------------------------------'
print,'. For x=5.4 et y=e-10  Armstrong=> voigt = 2.260842e-12'
print,'. For x=5.4 et y=e-10  Humlicek CPF12 1979=> voigt = 2.260845e-12'
print,'. For x=5.4 et y=e-10  Humlicek W4 1982=> voigt = 2.260842e-12'
print,'. For x=5.4 et y=e-10  Hui=> voigt = 2.667847e-8'
print,'. For x=5.4 et y=e-10  Lether and Wenston=> voigt = 2.260845e-12'
print,' Intrinsic GDL value of voigt : ',voigt(1e-10,5.4)
print,''
print,'. For x=5.5 et y=e-14  Armstrong=> voigt = 7.307387e-14'
print,'. For x=5.5 et y=e-14  Humlicek CPF12 1979=> voigt = 7.307387e-14'
print,'. For x=5.5 et y=e-14  W4 1982=> voigt = 1.966215e-16'
print,'. For x=5.5 et y=e-14  Hui=> voigt = 9.238980e-9'
print,'. For x=5.5 et y=e-14  Lether and Wenston=> voigt = 7.307386e-14'
print,' Intrinsic GDL value of voigt : ',voigt(1e-14,5.5)
print,''

end
;-----------------------------------------------------------------------


;-----------------------------------------------------------------------
;   		IV-Test with negative numbers 
;-----------------------------------------------------------------------
pro test_voigt_negative

print,'-------------------------------------------------'  ;use for better display
print,		'Show behaviour with negativ number'
print,'-------------------------------------------------'

print,''
print,'A=[ -5 , 4 , 3.22 , -1.0005 ] et U=[ -1.002 , 2.003 ]'
A=[-5,4,3.22,-1.0005]
B=[-1.002,2.003]
print , voigt(A,B)

print,''

print,'A= -1 et U= 1'
A=-1
B=1
print , voigt(A,B)

end

;-----------------------------------------------------------------------
;   		V-Test with not a number and infinite values 
;-----------------------------------------------------------------------
pro test_voigt_NaN

print,''
print,'----------there is no negative NaN in gdl-----------------'
A = [1.0005,8.33,4222d,!VALUES.D_NAN,-!VALUES.F_INFINITY ]
print ,'A = [1.0005,8.33,4222d,!VALUES.D_NAN,-!VALUES.F_INFINITY ]'

print ,'U =2.00'
U=2.00
print ,'voigt(A,U) = ',voigt(A,U)
print,''


print,'"limit value when u= (+-)!VALUES.F_INFINITY =0.000000"'
print,' voigt (2.000,!VALUES.F_INFINITY)= '
print,voigt (2.000,!VALUES.F_INFINITY)
end

;-----------------------------------------------------------------------
;   		VI-Test use to see if there is no memory leak 
;-----------------------------------------------------------------------
pro test_voigt_time, nb_loops=nb_loops
;
if (N_ELEMENTS(nb_loops) EQ 0) then nb_loops=12
;
;x=findgen(2000)/10-100
a=[0.01, 0.1, 0.25, .5, 1., 2., 4., 8., 16., 32., 64., 128.]
nb_a=N_ELEMENTS(a)
;
t0=SYSTIME(1)
for jj=0, nb_loops-1 do begin
    for ii=0, nb_a-1 do begin
        y1=VOIGT(a[ii],dist(512))
    endfor
endfor
print, 'VOIGT: ', SYSTIME(1)-t0
;

end
