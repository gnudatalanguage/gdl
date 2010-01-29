; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
; testing newton() and broyden() by solving a moist isentrope eq. set

; usage:
; GDL> test_multiroots

function foo, x  
  return, [x[0]^2 + x[1]^3 - 9, x[0] + x[1] - 3]
end  

function constant, x
  return, [1, 2]
end

function pseudo_constant, x
  return, 1 + dindgen(2)
end

function buggy, x
  return, undefined_var_to_cause_an_error
end

; highly-obfuscated moist isentrope equation set below
; based on a file from the Univ. of Wyoming Cloud Parcel Model:
; http://www-das.uwyo.edu/~jsnider/parcel/parcel_model_2006_1/

function moist_isentrope_init
  common multiroots_constants, sratio_start, R, mw_h2o, mw_air, Ra, $
    Rv, tk_o, cp_air_o, ew_o, p_base, tk_base, epsilon, lv_o, $
    cw_h2o_o, cp_h2o_o

  ; physical constants
  R            = 8.3144d         ; [J/K/mol] ideal gas constant 
  mw_h2o       = 0.018015d       ; [kg/mol]  molecular weight of water 
  mw_air       = 0.028964d       ; [kg/mol]  molecular weight of air
  Ra           = R / mw_air      ; [J/K/kg]  dry air specific gas constant 
  Rv           = R / mw_h2o      ; [J/K/kg]  water vapour specific gas constant
  tk_o         = 273.15d         ; [K]       reference temperature : 0C
  cp_air_o     = 1005.2d         ; [J/K/kg]  specific heat capacity for dry air at 0C and 800 mb
  cp_h2o_o     = 1869.4d         ; [J/K/kg]  specific heat capacity for water v. at 0C and 800 mb
  cw_h2o_o     = 4218.0d         ; [J/kg/K]  water heat capacity 
  ew_o         = 610.7d          ; [Pa]      saturation vapor pressure of water vapor at the triple point 
  epsilon      = mw_h2o / mw_air ; [1]       molecular weight ratio
  lv_o         = 2500800.d       ; [J/kg]    latent heat of vapourisation of water

  ; other parameters
  sratio_start = 0.95d           ; [1]       initial supersaturation ratio
  p_base       = 90000d          ; [Pa]      cloud-base pressure
  tk_base      = 10 + 273.15     ; [K]       cloud-base temperature

  ; initial guess
  x0 = dblarr(10)
  x0[0] = .001                          ; [1]
  x0[1] = 100.                          ; [Pa]
  x0[2] = .001                          ; [1]
  x0[3] = 100.                          ; [Pa]
  x0[4] = p_base                        ; [Pa]     pressure 
  x0[5] = tk_base                       ; [K]      temperature
  x0[6] = cp_air_o * alog(tk_base/tk_o) ; [J/K/kg]                
  x0[7] = p_base                        ; [Pa]
  x0[8] = cp_air_o * alog(tk_base/tk_o) ; [J/K/kg]
  x0[9] = p_base                        ; [Pa]

  return, x0
end

function m_i_cp, T, p1, p2, x
  common multiroots_constants
  return, cp_air_o * alog(T/tk_o) - Ra * alog(p1) + x * (cp_h2o_o * alog(T/tk_o) - Rv * alog(p2)) 
end

function m_i_es, T
  common multiroots_constants
  return, ew_o * exp( $
    ((lv_o + (cw_h2o_o - cp_h2o_o) * tk_o) * (1.d / tk_o - 1.d / T) - (cw_h2o_o - cp_h2o_o) * alog(T / tk_o)) / Rv $
  )
end

function m_i_lm, licz, mian1, mian2
  common multiroots_constants
  return, epsilon * licz / (mian1 - mian2)
end

function moist_isentrope, x
  common multiroots_constants
  cwmcp = cw_h2o_o - cp_h2o_o
  f = dblarr(10)
  f[0] = x[0] - m_i_lm(x[1], p_base, x[1])
  f[1] = x[1] - m_i_es(tk_base)
  f[2] = x[2] - m_i_lm(x[3], x[4], x[3])
  f[3] = x[3] - m_i_es(x[5]) * sratio_start
  f[4] = x[6] - m_i_cp(tk_base, x[7], x[1], x[0])
  f[5] = x[8] - m_i_cp(x[5],    x[9], x[3], x[2])
  f[6] = x[0] - x[2]
  f[7] = x[6] - x[8]
  f[8] = p_base - x[7] - x[1]
  f[9] = x[4] - x[9] - x[3]
  return, f
end

pro test_multiroots

  x0 = moist_isentrope_init()

  ; testing newton & broyden parameters (defaults are: tolf=1d-4 & tolx=1d-7)
  ; (note: not all IDL stopping constraints are implemented in GDL)
  out = newton(x0, 'moist_isentrope', it=3)                        ; it=2 breaks in IDL & GDL 
  out = newton(x0, 'moist_isentrope', it=4, tolf=1d-9)             ; it=3 breaks in IDL & GDL
  out = newton(x0, 'moist_isentrope', it=2, tolf=1d-1)             ; it=1 breaks in IDL & GDL
  out = newton(x0, 'moist_isentrope', it=5, tolx=1d-9, tolf=1d-10) ; it=4 breaks in IDL
  out = newton(x0, 'moist_isentrope', it=2, tolx=1d-1)             ; it=1 breaks in IDL & GDL

  out = broyden(x0, 'moist_isentrope', it=5)                       ; it=4 breaks in IDL & GDL
  out = broyden(x0, 'moist_isentrope', it=6, tolf=1d-5)            ; it=5 breaks in IDL & GDL
  out = broyden(x0, 'moist_isentrope', it=4, tolf=1d-3)            ; it=3 breaks in IDL & GDL
  out = broyden(x0, 'moist_isentrope', it=6, tolx=1d-8, tolf=1d-99); it=5 breaks in IDL & GDL 
  out = broyden(x0, 'moist_isentrope', it=4, tolx=1d-5)            ; it=3 breaks in IDL & GDL

  ; testing the /DOUBLE keyword
  if 5 ne size(newton(x0,        'moist_isentrope'         ), /type) then begin
    message, "failed 1", /conti
    exit, status=1
  endif
  if 4 ne size(newton(float(x0), 'moist_isentrope'         ), /type) then begin
    message, "failed 2", /conti
    exit, status=1
  endif
  if 5 ne size(newton(float(x0), 'moist_isentrope', /double), /type) then begin
    message, "failed 3", /conti
    exit, status=1
  endif
  if 4 ne size(newton(long(x0),  'moist_isentrope'         ), /type) then begin
    message, "failed 4", /conti
  endif

  message, "tests of newton() and broyden() passed", /continue

  ; testing GSL error handler
  message, "testing the GSL error handler, a GSL warning message should appear below...", /continue
  out = broyden([3., 0.], 'foo')

  ; testing behaviour with costant-returning user func
  out = newton([1., 2.], 'pseudo_constant')
  out = newton([3., 0.], 'constant')

end

