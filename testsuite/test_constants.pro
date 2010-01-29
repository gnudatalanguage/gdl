; by Sylwester Arabas
pro test_constants

  cnsts = strarr(28)
  units = strarr(28)

  cnsts[0:5] = ['amu','atm', 'AU', 'Avogadro', 'Boltzman', 'C'  ]
  units[0:5] = ['kg', 'N/m2','m',  '1/mole',   'J/K',      'm/s']

  ;cnsts[6] = ['Catalan']
  ;units[6] = ['1']

  cnsts[7:10] = ['E', 'ElectronCharge', 'ElectronMass', 'ElectronVolt']
  units[7:10] = ['1', 'C',              'kg',           'J']

  cnsts[11:15] = ['Euler', 'Faraday', 'FineStructure', 'Gamma', 'Gas']
  units[11:15] = ['1',     'C/mole',  '1',             '1',     'J/mole/K']

  cnsts[16:20] = ['Gravity',  'Hbar', 'PerfectGasVolume', 'Pi', 'Planck']
  units[16:20] = ['N*m2/kg2', 'J*s',  'm3 / mole',        '1',  'J*s']

  cnsts[21:24] = ['ProtonMass', 'Rydberg', 'Speedlight', 'StandardGravity']
  units[21:24] = ['kg',         'm-1',     'm/s',        'm/s2']

  cnsts[25:27] = ['StandardPressure', 'StefanBoltzman', 'WaterTriple'] 
  units[25:27] = ['N/m2',             'W/K4 /m2',       'K']

  ; testing all possible calling sequences with the default units
  for i = 0, n_elements(cnsts) - 1 do if strlen(cnsts[i]) ne 0 then begin
    a = imsl_constant(cnsts[i])
    b = imsl_constant(cnsts[i], units[i])
    a = imsl_constant(cnsts[i], /double)
    b = imsl_constant(cnsts[i], units[i], /double)
    if a ne b then begin
      message, 'FAILED: ' + cnsts[i] + ' [' + units[i] + ']', /conti
      exit, status=1
    endif
  endif

  ; testing unit conversion
  ; - time
  u = 'm/day'    & if ~finite(imsl_constant('c', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'm/hour'   & if ~finite(imsl_constant('c', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'm/hr'     & if ~finite(imsl_constant('c', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'm/min'    & if ~finite(imsl_constant('c', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'm/minute' & if ~finite(imsl_constant('c', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'm/s'      & if ~finite(imsl_constant('c', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'm/sec'    & if ~finite(imsl_constant('c', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'm/second' & if ~finite(imsl_constant('c', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'm/year'   & if ~finite(imsl_constant('c', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - frequency
  u = 'm*Hz'     & if ~finite(imsl_constant('c', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - mass
  u = 'AMU'      & if ~finite(imsl_constant('electronmass', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'g'        & if ~finite(imsl_constant('electronmass', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'gram'     & if ~finite(imsl_constant('electronmass', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'lb'       & if ~finite(imsl_constant('electronmass', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'pound'    & if ~finite(imsl_constant('electronmass', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
;  u = 'ounce'    & if ~finite(imsl_constant('electronmass', u)) then begin
;    message, 'FAILED! ' + u, /conti
;    exit, status=1
;  endif
;  u = 'oz'       & if ~finite(imsl_constant('electronmass', u)) then begin
;    message, 'FAILED! ' + u, /conti
;    exit, status=1
;  endif
  u = 'slug'     & if ~finite(imsl_constant('electronmass', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
 
  ; - distance
  u = 'Angstrom' & if ~finite(imsl_constant('AU', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
;  u = 'AU'       & if ~finite(imsl_constant('AU', u)) then begin
;    message, 'FAILED! ' + u, /conti
;    exit, status=1
;  endif
  u = 'feet'     & if ~finite(imsl_constant('AU', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'foot'     & if ~finite(imsl_constant('AU', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'in'       & if ~finite(imsl_constant('AU', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'inch'     & if ~finite(imsl_constant('AU', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'm'        & if ~finite(imsl_constant('AU', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'meter'    & if ~finite(imsl_constant('AU', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'metre'    & if ~finite(imsl_constant('AU', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'micron'   & if ~finite(imsl_constant('AU', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'mile'     & if ~finite(imsl_constant('AU', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
;  u = 'mill'     & if ~finite(imsl_constant('AU', u)) then begin 
;    message, 'FAILED! ' + u, /conti
;    exit, status=1
;  endif
  u = 'parsec'   & if ~finite(imsl_constant('AU', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'yard'     & if ~finite(imsl_constant('AU', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - area
  u = 'N/acre'   & if ~finite(imsl_constant('ATM', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - volume
  u = 'l/mole'    & if ~finite(imsl_constant('PerfectGasVolume', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'liter/mole'& if ~finite(imsl_constant('PerfectGasVolume', u)) then begin 
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'litre/mole'& if ~finite(imsl_constant('PerfectGasVolume', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - force
  u = 'dyne/m^2'  & if ~finite(imsl_constant('ATM', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'N/m^2'     & if ~finite(imsl_constant('ATM', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'Newton/m^2'& if ~finite(imsl_constant('ATM', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - energy
  u = 'BTU'   & if ~finite(imsl_constant('ElectronVolt', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'Erg'   & if ~finite(imsl_constant('ElectronVolt', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'J'     & if ~finite(imsl_constant('ElectronVolt', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'Joule' & if ~finite(imsl_constant('ElectronVolt', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - work
  u = 'W*s^2'    & if ~finite(imsl_constant('Hbar', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'watt*s^2' & if ~finite(imsl_constant('Hbar', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - pressure
;  u = 'ATM'        & if ~finite(imsl_constant('ATM', u)) then begin
;    message, 'FAILED! ' + u, /conti
;    exit, status=1
;  endif
  u = 'atmosphere' & if ~finite(imsl_constant('ATM', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'bar'        & if ~finite(imsl_constant('ATM', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - temperature
  u = 'degC'       & if ~finite(imsl_constant('WaterTriple', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'Celsius'    & if ~finite(imsl_constant('WaterTriple', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'degF'       & if ~finite(imsl_constant('WaterTriple', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
;  u = 'Fahrenheit' & if ~finite(imsl_constant('WaterTriple', u)) then begin
;    message, 'FAILED! ' + u, /conti
;    exit, status=1
;  endif
  u = 'degK'       & if ~finite(imsl_constant('WaterTriple', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'Kelvin'     & if ~finite(imsl_constant('WaterTriple', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - viscosity
;  u = 'stoke/m'    & if ~finite(imsl_constant('Speedlight', u)) then begin
;    message, 'FAILED! ' + u, /conti
;    exit, status=1
;  endif
  u = 'poise/s'    & if ~finite(imsl_constant('ATM', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - charge
;  u = 'Abcoulomb'   & if ~finite(imsl_constant('electroncharge', u)) then begin 
;    message, 'FAILED! ' + u, /conti
;    exit, status=1
;  endif
  u = 'C'           & if ~finite(imsl_constant('electroncharge', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'Coulomb'     & if ~finite(imsl_constant('electroncharge', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'statcoulomb' & if ~finite(imsl_constant('electroncharge', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - current
  u = 'A*s'          & if ~finite(imsl_constant('electroncharge', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'ampere*s'     & if ~finite(imsl_constant('electroncharge', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'abampere*s'   & if ~finite(imsl_constant('electroncharge', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'statampere*s' & if ~finite(imsl_constant('electroncharge', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - voltage
  u = 'J/Abvolt' & if ~finite(imsl_constant('electroncharge', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'J/V'      & if ~finite(imsl_constant('electroncharge', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'J/volt'   & if ~finite(imsl_constant('electroncharge', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - magnetic induction
  u = 'T*C*s'     & if ~finite(imsl_constant('electronmass', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'Tesla*C*s' & if ~finite(imsl_constant('electronmass', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'Weber*A'   & if ~finite(imsl_constant('electronvolt', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'Wb*A'      & if ~finite(imsl_constant('electronvolt', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; - other units
  u = '1/mole'     & if ~finite(imsl_constant('avogadro', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'farad*V'    & if ~finite(imsl_constant('electroncharge', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'Gauss*C*s'  & if ~finite(imsl_constant('electronmass', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'Henry*C2/s' & if ~finite(imsl_constant('planck', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'Maxwell*A'  & if ~finite(imsl_constant('electronvolt', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif
  u = 'Ohm*s*A^2'  & if ~finite(imsl_constant('electronvolt', u)) then begin
    message, 'FAILED! ' + u, /conti
    exit, status=1
  endif

  ; testing prefixes
  eps = 1d-6
  au = imsl_constant('AU', /d)
  if imsl_constant('AU', 'parsec', /d) - au * 1d18 gt eps then begin
    message, 'FAILED! a', /conti
    exit, status=1
  endif
  if imsl_constant('AU', 'parsec', /d) - au * 1d15 gt eps then begin
    message, 'FAILED! f', /conti
    exit, status=1
  endif
  if imsl_constant('AU', 'pm', /d) ne au * 1d12 then begin
    message, 'FAILED! p', /conti
    exit, status=1
  endif
  if imsl_constant('AU', 'nm', /d) ne au * 1d9 then begin
    message, 'FAILED! n', /conti
    exit, status=1
  endif
;  if imsl_constant('AU', 'um', /d) ne au * 1d6 then begin
;    message, 'FAILED! u', /conti
;    exit, status=1
;  endif
  if imsl_constant('AU', 'mm', /d) ne au * 1d3 then begin
    message, 'FAILED! m', /conti
    exit, status=1
  endif
  if imsl_constant('AU', 'cm', /d) ne au * 1d2 then begin
    message, 'FAILED! c', /conti
    exit, status=1
  endif
  if imsl_constant('AU', 'dm', /d) ne au * 1d1 then begin
    message, 'FAILED! d', /conti
    exit, status=1
  endif
  if imsl_constant('AU', 'dam', /d) ne au * 1d-1 then begin
    message, 'FAILED! da', /conti
    exit, status=1
  endif
  if imsl_constant('AU', 'km', /d) ne au * 1d-3 then begin
    message, 'FAILED! k', /conti
    exit, status=1
  endif
  if imsl_constant('AU', 'Gm', /d) ne au * 1d-9 then begin
    message, 'FAILED! g', /conti
    exit, status=1
  endif
  if imsl_constant('AU', 'Tm', /d) ne au * 1d-12 then begin
    message, 'FAILED! t', /conti
    exit, status=1
  endif
 
  ; testing if PI = PI
  if imsl_constant('pi') ne !PI then begin
    message, 'FAILED! (PI != PI)', /conti
    exit, status=1
  endif
  if imsl_constant('pi', /double) ne !DPI then begin
    message, 'FAILED! (DPI != DPI)', /conti
    exit, status=1
  endif

end
