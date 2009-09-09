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
    if a ne b then message, 'FAILED: ' + cnsts[i] + ' [' + units[i] + ']'
  endif

  ; testing unit conversion
  ; - time
  u = 'm/day'    & if ~finite(imsl_constant('c', u)) then message, 'FAILED! ' + u
  u = 'm/hour'   & if ~finite(imsl_constant('c', u)) then message, 'FAILED! ' + u
  u = 'm/hr'     & if ~finite(imsl_constant('c', u)) then message, 'FAILED! ' + u
  u = 'm/min'    & if ~finite(imsl_constant('c', u)) then message, 'FAILED! ' + u
  u = 'm/minute' & if ~finite(imsl_constant('c', u)) then message, 'FAILED! ' + u
  u = 'm/s'      & if ~finite(imsl_constant('c', u)) then message, 'FAILED! ' + u
  u = 'm/sec'    & if ~finite(imsl_constant('c', u)) then message, 'FAILED! ' + u
  u = 'm/second' & if ~finite(imsl_constant('c', u)) then message, 'FAILED! ' + u
  u = 'm/year'   & if ~finite(imsl_constant('c', u)) then message, 'FAILED! ' + u

  ; - frequency
  u = 'm*Hz'     & if ~finite(imsl_constant('c', u)) then message, 'FAILED! ' + u

  ; - mass
  u = 'AMU'      & if ~finite(imsl_constant('electronmass', u)) then message, 'FAILED! ' + u
  u = 'g'        & if ~finite(imsl_constant('electronmass', u)) then message, 'FAILED! ' + u
  u = 'gram'     & if ~finite(imsl_constant('electronmass', u)) then message, 'FAILED! ' + u
  u = 'lb'       & if ~finite(imsl_constant('electronmass', u)) then message, 'FAILED! ' + u
  u = 'pound'    & if ~finite(imsl_constant('electronmass', u)) then message, 'FAILED! ' + u
;  u = 'ounce'    & if ~finite(imsl_constant('electronmass', u)) then message, 'FAILED! ' + u
;  u = 'oz'       & if ~finite(imsl_constant('electronmass', u)) then message, 'FAILED! ' + u
  u = 'slug'     & if ~finite(imsl_constant('electronmass', u)) then message, 'FAILED! ' + u
 
  ; - distance
  u = 'Angstrom' & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u
;  u = 'AU'       & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u
  u = 'feet'     & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u
  u = 'foot'     & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u
  u = 'in'       & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u
  u = 'inch'     & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u
  u = 'm'        & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u 
  u = 'meter'    & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u 
  u = 'metre'    & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u
  u = 'micron'   & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u
  u = 'mile'     & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u
;  u = 'mill'     & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u
  u = 'parsec'   & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u
  u = 'yard'     & if ~finite(imsl_constant('AU', u)) then message, 'FAILED! ' + u

  ; - area
  u = 'N/acre'   & if ~finite(imsl_constant('ATM', u)) then message, 'FAILED! ' + u

  ; - volume
  u = 'l/mole'    & if ~finite(imsl_constant('PerfectGasVolume', u)) then message, 'FAILED! ' + u
  u = 'liter/mole'& if ~finite(imsl_constant('PerfectGasVolume', u)) then message, 'FAILED! ' + u
  u = 'litre/mole'& if ~finite(imsl_constant('PerfectGasVolume', u)) then message, 'FAILED! ' + u

  ; - force
  u = 'dyne/m^2'  & if ~finite(imsl_constant('ATM', u)) then message, 'FAILED! ' + u
  u = 'N/m^2'     & if ~finite(imsl_constant('ATM', u)) then message, 'FAILED! ' + u
  u = 'Newton/m^2'& if ~finite(imsl_constant('ATM', u)) then message, 'FAILED! ' + u

  ; - energy
  u = 'BTU'   & if ~finite(imsl_constant('ElectronVolt', u)) then message, 'FAILED! ' + u
  u = 'Erg'   & if ~finite(imsl_constant('ElectronVolt', u)) then message, 'FAILED! ' + u
  u = 'J'     & if ~finite(imsl_constant('ElectronVolt', u)) then message, 'FAILED! ' + u
  u = 'Joule' & if ~finite(imsl_constant('ElectronVolt', u)) then message, 'FAILED! ' + u

  ; - work
  u = 'W*s^2'    & if ~finite(imsl_constant('Hbar', u)) then message, 'FAILED! ' + u
  u = 'watt*s^2' & if ~finite(imsl_constant('Hbar', u)) then message, 'FAILED! ' + u

  ; - pressure
;  u = 'ATM'        & if ~finite(imsl_constant('ATM', u)) then message, 'FAILED! ' + u
  u = 'atmosphere' & if ~finite(imsl_constant('ATM', u)) then message, 'FAILED! ' + u
  u = 'bar'        & if ~finite(imsl_constant('ATM', u)) then message, 'FAILED! ' + u

  ; - temperature
  u = 'degC'       & if ~finite(imsl_constant('WaterTriple', u)) then message, 'FAILED! ' + u
  u = 'Celsius'    & if ~finite(imsl_constant('WaterTriple', u)) then message, 'FAILED! ' + u
  u = 'degF'       & if ~finite(imsl_constant('WaterTriple', u)) then message, 'FAILED! ' + u 
;  u = 'Fahrenheit' & if ~finite(imsl_constant('WaterTriple', u)) then message, 'FAILED! ' + u
  u = 'degK'       & if ~finite(imsl_constant('WaterTriple', u)) then message, 'FAILED! ' + u
  u = 'Kelvin'     & if ~finite(imsl_constant('WaterTriple', u)) then message, 'FAILED! ' + u

  ; - viscosity
;  u = 'stoke/m'    & if ~finite(imsl_constant('Speedlight', u)) then message, 'FAILED! ' + u
  u = 'poise/s'    & if ~finite(imsl_constant('ATM', u)) then message, 'FAILED! ' + u

  ; - charge
;  u = 'Abcoulomb'   & if ~finite(imsl_constant('electroncharge', u)) then message, 'FAILED! ' + u  
  u = 'C'           & if ~finite(imsl_constant('electroncharge', u)) then message, 'FAILED! ' + u 
  u = 'Coulomb'     & if ~finite(imsl_constant('electroncharge', u)) then message, 'FAILED! ' + u
  u = 'statcoulomb' & if ~finite(imsl_constant('electroncharge', u)) then message, 'FAILED! ' + u

  ; - current
  u = 'A*s'          & if ~finite(imsl_constant('electroncharge', u)) then message, 'FAILED! ' + u
  u = 'ampere*s'     & if ~finite(imsl_constant('electroncharge', u)) then message, 'FAILED! ' + u
  u = 'abampere*s'   & if ~finite(imsl_constant('electroncharge', u)) then message, 'FAILED! ' + u
  u = 'statampere*s' & if ~finite(imsl_constant('electroncharge', u)) then message, 'FAILED! ' + u

  ; - voltage
  u = 'J/Abvolt' & if ~finite(imsl_constant('electroncharge', u)) then message, 'FAILED! ' + u
  u = 'J/V'      & if ~finite(imsl_constant('electroncharge', u)) then message, 'FAILED! ' + u
  u = 'J/volt'   & if ~finite(imsl_constant('electroncharge', u)) then message, 'FAILED! ' + u

  ; - magnetic induction
  u = 'T*C*s'     & if ~finite(imsl_constant('electronmass', u)) then message, 'FAILED! ' + u
  u = 'Tesla*C*s' & if ~finite(imsl_constant('electronmass', u)) then message, 'FAILED! ' + u
  u = 'Weber*A'   & if ~finite(imsl_constant('electronvolt', u)) then message, 'FAILED! ' + u
  u = 'Wb*A'      & if ~finite(imsl_constant('electronvolt', u)) then message, 'FAILED! ' + u

  ; - other units
  u = '1/mole'     & if ~finite(imsl_constant('avogadro', u)) then message, 'FAILED! ' + u  
  u = 'farad*V'    & if ~finite(imsl_constant('electroncharge', u)) then message, 'FAILED! ' + u
  u = 'Gauss*C*s'  & if ~finite(imsl_constant('electronmass', u)) then message, 'FAILED! ' + u
  u = 'Henry*C2/s' & if ~finite(imsl_constant('planck', u)) then message, 'FAILED! ' + u
  u = 'Maxwell*A'  & if ~finite(imsl_constant('electronvolt', u)) then message, 'FAILED! ' + u 
  u = 'Ohm*s*A^2'  & if ~finite(imsl_constant('electronvolt', u)) then message, 'FAILED! ' + u

 ; testing prefixes
 eps = 1d-6
 au = imsl_constant('AU', /d)
 if imsl_constant('AU', 'parsec', /d) - au * 1d18 gt eps then message, 'FAILED! a'
 if imsl_constant('AU', 'parsec', /d) - au * 1d15 gt eps then message, 'FAILED! f'
 if imsl_constant('AU', 'pm', /d) ne au * 1d12 then message, 'FAILED! p'
 if imsl_constant('AU', 'nm', /d) ne au * 1d9 then message, 'FAILED! n'
; if imsl_constant('AU', 'um', /d) ne au * 1d6 then message, 'FAILED! u'
 if imsl_constant('AU', 'mm', /d) ne au * 1d3 then message, 'FAILED! m'
 if imsl_constant('AU', 'cm', /d) ne au * 1d2 then message, 'FAILED! c'
 if imsl_constant('AU', 'dm', /d) ne au * 1d1 then message, 'FAILED! d'
 if imsl_constant('AU', 'dam', /d) ne au * 1d-1 then message, 'FAILED! da'
 if imsl_constant('AU', 'km', /d) ne au * 1d-3 then message, 'FAILED! k'
 if imsl_constant('AU', 'Gm', /d) ne au * 1d-9 then message, 'FAILED! g'
 if imsl_constant('AU', 'Tm', /d) ne au * 1d-12 then message, 'FAILED! t'

 ; testing if PI = PI
 if imsl_constant('pi') ne !PI then message, 'FAILED! (PI != PI)'
 if imsl_constant('pi', /double) ne !DPI then message, 'FAILED! (DPI != DPI)'

  message, 'PASSED', /conti

end
