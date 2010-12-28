;
; This code is under GNU/GPL v2 or later
; This code has been released in GDL with written permission.
; 07 July 2008 Alain Coulais
;
; The initial author of the Nose-Hoover code is:
; Sylwester Arabas <slayoo __AT__ igf.fuw.edu.pl>
; A PDF description of the Nose-Hoover system implemented here
; can be downloaded : http://slayoo.itstudio.pl/files/nosehoover/nosehoover.pdf
;
; This code has been significantly modified by Alain C.from the original version
; with the goal to provide a simple framework for testing IDL and GDL
; performances and stability of RK4 versions written in IDL/GDL syntax and in C++.
;
; See TEST_RK4 "hub" to have a list of all capabilities.
; Calling a demo sequence: NOSE_HOOVER OR  NOSE_HOOVER, /demo
;
pro NOSE_HOOVER, demo=demo, dt=dt, t_stop=t_stop, display=display, $
                 method=method, out=out, timetest=timetest, $
                 invQ1ext=invQ1ext, invQ2ext=invQ2ext, $
                 help=help, test=test, verbose=verbose
;
; compilation options
compile_opt idl2, strictarr 
;
if KEYWORD_SET(help) then begin
   print, 'pro NOSE_HOOVER, demo=demo, dt=dt, t_stop=t_stop, display=display, $'
   print, '                 method=method, out=out, timetest=timetest, $'
   print, '                 invQ1ext=invQ1ext, invQ2ext=invQ2ext, $'
   print, '                 help=help, test=test, verbose=verbose'
   return
endif
;
if KEYWORD_SET(timetest) then time0=SYSTIME(1)
;
if N_ELEMENTS(demo) EQ 0 then begin
   demo=1
   if (N_ELEMENTS(display) EQ 0) then display=1
   print, 'NB: in order to desactivate the DEMO mode, you must switch it OFF explicitely (NOSE_HOOVER, demo=0)'
endif   
;
; memory allocation / declarations
dim = 4
y = DBLARR(dim)
ytmp = DBLARR(dim)
F1 = DBLARR(dim)
F2 = DBLARR(dim)
F3 = DBLARR(dim)
F4 = DBLARR(dim)

; common block
common constants, invQ1, invQ2, m, N, kT, K
common indices, q_ind, p_ind, zeta1_ind, zeta2_ind 

; eq sys variable index numbering
q_ind = 0
p_ind = 1
zeta1_ind = 2
zeta2_ind = 3

;while (plot lt 4) do begin
; initial conditions and simplifications (as used by Hoover in the 1985 paper)
y[q_ind] = 1.
y[p_ind] = 1.
y[zeta1_ind] = 0.
y[zeta2_ind] = 0.               ; (non-Hoover value)
m = 1.
kT = 1.
N = 1.
K = 1.
;
; some default values ... for fast tests ...
;
if KEYWORD_SET(demo) then begin
   if (N_ELEMENTS(dt) EQ 0) then dt=0.005D
   if (N_ELEMENTS(t_stop) EQ 0) then t_stop=100.
   if (N_ELEMENTS(invQ1ext) EQ 0) then invQ1=2. else invQ1=invQ1ext
   if (N_ELEMENTS(invQ2ext) EQ 0) then invQ2=2. else invQ2=invQ2ext
endif else begin
   ;; computation parameters
   ;;print, "please supply (missing) computation parameters..."
   ;;
   if N_ELEMENTS(dt) EQ 0 then begin
      read, dt, prompt="  integration step in t_new (Hoover values: 0.01, 0.001) [1]: " 
   endif
   if N_ELEMENTS(t_stop) EQ 0 then begin
      read, t_stop, prompt="  integration limit in t_new (several thousands give good phase-space coverage) [1]: " 
   endif
   if N_ELEMENTS(invQ1ext) EQ 0 then begin
      read, invQ1, prompt="  reciprocal of the thermal inertia parameter Q for 1st thermostat (Hoover values: 1, 10) [1]: "
   endif else begin
      invQ1=invQ1ext
   endelse
   if N_ELEMENTS(invQ2ext) EQ 0 then begin
      read, invQ2, prompt="  reciprocal of the thermal inertia parameter Q for 2nd thermostat (0 - no chain) [1]: "
   endif else begin
      invQ2=invQ2ext
   endelse
endelse
;
; computing total number of elements
itot = FIX(t_stop/dt, type=3)
;
plot_q = DBLARR(itot) 
plot_p = DBLARR(itot) 
;
if KEYWORD_SET(verbose) then begin
   print, 'Parameter  "dt" : ', dt
   print, 'Parameter  "t_stop" : ', t_stop
   print, 'Parameter  "inv1" : ', invQ1
   print, 'Parameter  "inv2" : ', invQ2
   print, 'Total points number "itot" : ', itot
endif

t = 0.D
i = 0L

if N_ELEMENTS(method) EQ 0 then method=0

if (method EQ 0) then begin
   ;; using internal RK4 method
   while (t lt t_stop && i lt itot) do begin
      dxdy=EQSYS_FUN(t,y)
      y_new=RK4(y, dxdy, t, dt, 'EQSYS_FUN',/double)
      plot_q[i] = y_new[q_ind]
      plot_p[i] = y_new[p_ind]
      y=y_new
      t=t+dt
      i=i+1
   endwhile
endif
;
if (method EQ 1) then begin
   ;; using by hand external RK4 method
   ;; integration (Runge-Kutta Method, Numerical Recipes)
   while (t lt t_stop && i lt itot) do begin
      dt1= 0.5D * dt
      EQSYS_PRO, t, y, F1
      ytmp = y + dt1 * F1        
      EQSYS_PRO, t + dt1, ytmp, F2
      ytmp = y + dt1*F2
      EQSYS_PRO, t + dt1, ytmp, F3
      ytmp = y + dt*F3
      EQSYS_PRO, t + dt, ytmp, F4
      y  = y + (dt / 6.) * (F1 + F2 + F2 + F3 + F3 + F4)
      t  = t + dt
      ;; remembering calculated values
      plot_q[i] = y[q_ind]
      plot_p[i] = y[p_ind]
      i = i + 1        
   endwhile
endif
;
if (method EQ 2) then begin
   ;; using by hand "very" external RK4 method (with CALL_PROCEDURE)
   ;; integration (Runge-Kutta Method, Numerical Recipes)
   while (t lt t_stop && i lt itot) do begin        
      CALL_PROCEDURE, 'EQSYS_PRO', t, y, F1
      y_new=RK4_PRO(y, F1, t, dt, 'EQSYS_PRO', /double)     
      y  = y_new
      t  = t + dt
      ;; remembering calculated values
      plot_q[i] = y[q_ind]
      plot_p[i] = y[p_ind]
      i = i + 1        
   endwhile
endif
;
out=TRANSPOSE([[plot_p], [plot_q]])
;
if KEYWORD_SET(display) then begin
   ;;
   new_pmulti=0
   if (!p.multi[1] EQ 0) then begin
      new_pmulti=1
      !P.MULTI = [0, 2, 0]
   endif
   ;;
   ;; energy plot alloc
   bins = 25
   plot_x = DBLARR(bins)                       
   ;; plotting traj.
   plot, plot_q, plot_p, xtitle="q [m]", xrange=[-4., 4.], $
         ytitle="p [kg m s-1]", yrange=[-4., 4.], /ynozero, $
         psym=3, charsize=1.2
   ;;
   ;; plotting p histo.
   qmax = MAX(plot_q, min=qmin)
   q_binsize=(qmax-qmin)/(bins-1)
   pmax = MAX(plot_p, min=pmin)
   p_binsize=(pmax-pmin)/(bins-1)
   ;;
   plot_y = HISTOGRAM(plot_p, binsize=p_binsize, locations=plot_x)
   plot_y = plot_y/( itot*p_binsize)
   PLOT, plot_x, plot_y, psym=10, charsize=1.2, $
         xtitle="q [m] / p [kg m s-1]", xrange=[-4.,4.], $
         ytitle="prob. dens. [m-1]/[kg-1 m-1 s]", yrange=[0., .75]
   ;;
   ;; plotting q histo.
   plot_y = HISTOGRAM(plot_q, binsize=q_binsize, locations=plot_x)
   plot_y = plot_y/( itot*q_binsize)
   OPLOT, plot_x, plot_y, psym=10
   ;;
   ;;  plotting theoretical curve (gaussian with sigma=(2*K/kT)^(-1/2))
   plot_x = (DINDGEN(bins*10) - bins*5) * 8. / 10. / bins
   plot_y = exp(-.5 * K * plot_x^2 / kT)
   plot_y /= sqrt(2*!PI) / sqrt(2*K/kT)
   OPLOT, plot_x, plot_y
   ;;
   if (new_pmulti EQ 1) then !p.multi=0
endif
;
if KEYWORD_SET(timetest) then begin
   time1=SYSTIME(1)-time0
   if (method EQ 0) then print, 'Time for Internal method (c++)  : ', time1
   if (method EQ 1) then print, 'Time for External 1 method (GDL): ', time1
   if (method EQ 2) then print, 'Time for External 2 method (GDL): ', time1
   timetest=time1
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------------
; equation system definitions
; arguments: 
; - x (floating point scalar with current value of variable used for integration), 
; - y (floating point vector of size of number of eq. with current values)
; - F (floating point vector of size of number of eq. ready for output values - dericatives)
pro EQSYS_PRO, x, y, F
; compilation options
compile_opt idl2, strictarr
;
common constants, invQ1, invQ2, m, N, kT, K
common indices, p_ind, q_ind, zeta1_ind, zeta2_ind 
;
; Nose-Hoover equations 
; derivatives are stated with respect to t_new
;
;   q - length dimension of the phase-space
F[q_ind] = y[p_ind] / m
;
;   p - momentum dimension of the phase-space
;   y[q_ind] -> 1-D harmonic oscillator potential derivative with respect to length
F[p_ind] = - K * y[q_ind] - y[zeta1_ind] * y[p_ind]
;
;   zeta1 - thermodynamic friction coefficient 
F[zeta1_ind] = invQ1 * (y[p_ind] * y[p_ind] / m - N * kT) - y[zeta1_ind] * y[zeta2_ind]
;
;   zeta2 - thermod. friction coefficient of the second (chained) thermostat
F[zeta2_ind] = invQ2 * (y[zeta1_ind] * y[zeta1_ind] / invQ1 - kT) 
;
end
;
; -------------------------------------------------
;
function EQSYS_FUN, x, y
; compilation options
compile_opt idl2, strictarr

common constants, invQ1, invQ2, m, N, kT, K
common indices, p_ind, q_ind, zeta1_ind, zeta2_ind 

F=DBLARR(N_ELEMENTS(y))

F[q_ind] = y[p_ind] / m

;;   p - momentum dimension of the phase-space
;;   y[q_ind] -> 1-D harmonic oscillator potential derivative with respect to length
F[p_ind] = - K * y[q_ind] - y[zeta1_ind] * y[p_ind] 

;;   zeta1 - thermodynamic friction coefficient 
F[zeta1_ind] = invQ1 * (y[p_ind] * y[p_ind] / m - N * kT) - y[zeta1_ind] * y[zeta2_ind]

;;   zeta2 - thermod. friction coefficient of the second (chained) thermostat
F[zeta2_ind] = invQ2 * (y[zeta1_ind] * y[zeta1_ind] / invQ1 - kT) 

return, F
end
;
; -------------------------------------------------
; Run NOSE-HOOVER with differents parameters ...
; By default, will reproduce the figures published by S. Arabas.
;
pro EXPLORE_NOSE_HOOVER, t_stop=t_stop, tab_q1=tab_q1, tab_q2=tab_q2, $
                         postscript=postscript, $
                         test=test, help=help, quiet=quiet
;
name="EXPLORE_NOSE_HOOVER"
mess={name: name, info: "Info in "+name+": ", warning: "Warning in "+name+": "}
;
if KEYWORD_SET(help) then begin
    print, 'pro EXPLORE_NOSE_HOOVER, t_stop=t_stop, tab_q1=tab_q1, tab_q2=tab_q2, $'
    print, '                         postscript=postscript, $'
    print, '                         test=test, help=help, quiet=quiet'
    return
endif
;
nb_tab_q1=N_ELEMENTS(tab_q1)
nb_tab_q2=N_ELEMENTS(tab_q2)

if (nb_tab_q1 EQ 0) OR (nb_tab_q2 EQ 0) then begin
    nb_cases=4
    tab_q1=[1.,10.,1.,10.]    
    tab_q2=[0., 0.,1.,10.]
endif else begin
    nb_cases=MAX([nb_tab_q1,nb_tab_q2])
    if (nb_tab_q1 LT nb_cases) then begin
        tab_q1=REPLICATE(tab_q1[0],nb_cases)
        print, mess.warning+'Only first value used'
    endif
    if (nb_tab_q2 LT nb_cases) then begin
        tab_q2=REPLICATE(tab_q2[0],nb_cases)
        print, mess.warning+'Only first value used'
    endif
endelse
;
if NOT(KEYWORD_SET(quiet)) then begin
    print, mess.info+'Number of cases: ', nb_cases
    for ii=0, nb_cases-1 do begin
        print, mess.info+'q1, q2 values: ', tab_q1[ii], tab_q2[ii]
    endfor
endif
;
if (N_ELEMENTS(t_stop) EQ 0) then t_stop=100.
;
!P.MULTI = [0, 2, nb_cases]
;
if KEYWORD_SET(postscript) then begin
   set_plot, 'PS'
   device, /landscape, filename = "plot.eps"
endif
;
for ii=0, nb_cases-1 do begin
    NOSE_HOOVER, t_stop=t_stop, invQ1=tab_q1[ii], invQ2=tab_q2[ii], $
      /display, /demo
endfor
;
if KEYWORD_SET(postscript) then begin
   device, /close
   set_plot, 'X'
endif
;
!P.MULTI = 0
;
if KEYWORD_SET(test) then stop
;
end
;
; --------------------------------------------------------
; Testing the independance of RK4 to delta_time parameter !
; stables:
; NOSE_HOOVER_STABILITY_CHECK, meth=0, invq1=.4
; NOSE_HOOVER_STABILITY_CHECK, meth=0, invq1=2, invq2=0.0002
; NOSE_HOOVER_STABILITY_CHECK, meth=0, invq1=2, invq2=3.7
; instables:
; NOSE_HOOVER_STABILITY_CHECK, meth=0, invq1=4, invq2=3.7
;
pro NOSE_HOOVER_STABILITY_CHECK, method=method, dt_max=dt_max, t_stop=t_stop, $
                                 invQ1=invQ1, invQ2=invQ2, timetest=timetest, $
                                 verbose=verbose, test=test, help=help
;
if KEYWORD_SET(help) then begin
    print, 'pro NOSE_HOOVER_STABILITY_CHECK, method=method, dt_max=dt_max, t_stop=t_stop, $'
    print, '                                 invQ1=invQ1, invQ2=invQ2, timetest=timetest, $'
    print, '                                 verbose=verbose, test=test, help=help'
    return
endif
;
if N_ELEMENTS(invQ1) EQ 0 then invQ1=0.1
if N_ELEMENTS(invQ2) EQ 0 then invQ2=2.0
;
if (N_ELEMENTS(method) EQ 0) then method=0
if (N_ELEMENTS(t_stop) EQ 0) then t_stop=100.
if (N_ELEMENTS(dt_max) EQ 0) then dt_max=0.005
;
NOSE_HOOVER, demo=0, method=method, verbose=verbose, invQ1=invQ1, invQ2=invQ2, $
  out=out0, dt=dt_max, t_stop=t_stop, timetest=timetest
NOSE_HOOVER, demo=0, method=method, verbose=verbose, invQ1=invQ1, invQ2=invQ2, $
  out=out1, dt=dt_max/2., t_stop=t_stop, timetest=timetest
NOSE_HOOVER, demo=0, method=method, verbose=verbose, invQ1=invQ1, invQ2=invQ2, $
  out=out2, dt=dt_max/4., t_stop=t_stop, timetest=timetest
;
DEVICE, /decomposed
WINDOW, 0
;
PLOT, out0[0,*], out0[1,*]
OPLOT, out1[0,*], out1[1,*], color='ff'x
OPLOT, out2[0,*], out2[1,*], color='ff00'x
;
if KEYWORD_SET(test) then stop
;
end
; --------------------------------------------------------
;
pro COMPARE_RK4_METHODS, nodisplay=nodisplay, test=test, help=help
;
if KEYWORD_SET(help) then begin
    print, 'pro COMPARE_RK4_METHODS, nodisplay=nodisplay, test=test, help=help'
    txt= ' (Fast check of (quasi)-equal results'
    txt=txt+' via different implementations of RK4)'
    print, txt
    return
endif
;
; Classical internal (c++ coded) RK4 method
;
NOSE_HOOVER, /demo, method=0, out=out0, /timetest
;
; Classical external (IDL/GDL syntax coded) RK4 method
;
NOSE_HOOVER, /demo, method=1, out=out1, /timetest
;
; External (IDL/GDL syntax coded) RK4 method via RK4_pro
;
NOSE_HOOVER, /demo, method=2, out=out2, /timetest
;
; Displaying results
;
if NOT(KEYWORD_SET(nodisplay)) then begin
    DEVICE, /decomposed
    WINDOW, 0
    PLOT, out0[0,*]
    OPLOT, out1[0,*], col='ff'x, linestyle=2
    OPLOT, out2[0,*], col='ff00'x, linestyle=5
    WINDOW, 1
    PLOT, out0[1,*]
    OPLOT, out1[1,*], col='ff'x, linestyle=2
    OPLOT, out2[1,*], col='ff00'x, linestyle=5
    WINDOW, 2
    PLOT, out0[1,*]-out1[1,*], title='Difference between implementations ...'
    OPLOT, out2[1,*]-out1[1,*], col='ff00'x
    ;;
    enter=''
    read, 'Press [enter] to finish ...', enter
    WDELETE, 0, 1, 2
endif
;
if KEYWORD_SET(test) then stop
;
end
; ------------------------------------------------------------
; This function is a very simple implementation ok RK4 in GDL syntax
; allowing using Procedural form of derivative.
;
function RK4_PRO, Y, Dydx, X, H, ProcedureName, double=double
;
if N_PARAMS() LT 5 then begin
    print, 'Fatal in RK4_PRO: missing input parameters'
    return, -1
endif
;
if KEYWORD_SET(double) then begin
    t=x
    dt=h
endif else begin
    t=DOUBLE(x)
    dt=DOUBLE(h)
endelse
;
dt1= 0.5 * dt
;
F1= dydx
F2=F1
F3=F1
F4=F1
;
ytmp = y + dt1 * F1
;
CALL_PROCEDURE, ProcedureName, t + dt1, ytmp, F2
ytmp = y + dt1*F2

CALL_PROCEDURE, ProcedureName, t + dt1, ytmp, F3
ytmp = y + dt*F3

CALL_PROCEDURE, ProcedureName, t + dt, ytmp, F4
;
return, y + (dt / 6.) * (F1 + F2 + F2 + F3 + F3 + F4)
;
end
; ------------------------------------------------------------
; General description
;
pro TEST_RK4, help=help

  if keyword_set(help) then begin
    print, ' This test suite is based on RK4 tests on Nose-Hoover equations'
    print, ' We use with permission a code initially written by S. Arabas.'
    print, ' '
    print, 'The equation it-self is coded in "Functionnal" and in "Procedural" ways'
    print, '(EQSYS_FUN and EQSYS_PRO). A general wrapper (RK4_PRO) is provided for'
    print, 'calling RK4 when the code of the derivative is in Procedural form.'
    print, ''
    print, ' You can call:'
    print, '  -- NOSE_HOOVER : just basic test (3 methods inside)'
    print, '  -- NOSE_HOOVER_STABILITY_CHECK : is a couple of parameters stable with Dt ?'
    print, '  -- EXPLORE_NOSE_HOOVER: reproducing the 4 examples in Arabas'' demo'
    print, '                          or giving other inputs parameters'
    print, '  -- COMPARE_RK4_METHODS: does differents ways to code and call RK4'
    print, '                          give the same outputs ?'
    print, ' '
    print, 'Calling sequences:'
    print, ' '
    NOSE_HOOVER, /help
    NOSE_HOOVER_STABILITY_CHECK, /help
    COMPARE_RK4_METHODS, /help
    EXPLORE_NOSE_HOOVER, /help
  endif

  ; doing it as a port of "make check":
  compare_rk4_methods, /nodisplay

end

