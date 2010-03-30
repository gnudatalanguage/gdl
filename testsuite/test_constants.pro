;
; Created by Sylwester Arabas
; Modifications by AC on 30 March 2010
;
pro TEST_CONSTANTS, test=test
;
ON_ERROR, 2
;
is_imsl_ready=EXECUTE("a=IMSL_CONSTANT('c','m/s')") 
;
if (is_imsl_ready EQ 0) then begin
    MESSAGE, 'CHECK on TEST_CONSTANTS cannot be done without UDUNITS', /conti
    EXIT, status=0
endif
;
cnsts = STRARR(28)
units = STRARR(28)
;
cnsts[0:5] = ['amu','atm', 'AU', 'Avogadro', 'Boltzman', 'C'  ]
units[0:5] = ['kg', 'N/m2','m',  '1/mole',   'J/K',      'm/s']
;;
;;cnsts[6] = ['Catalan']
;;units[6] = ['1']
;;
cnsts[7:10] = ['E', 'ElectronCharge', 'ElectronMass', 'ElectronVolt']
units[7:10] = ['1', 'C',              'kg',           'J']
;
cnsts[11:15] = ['Euler', 'Faraday', 'FineStructure', 'Gamma', 'Gas']
units[11:15] = ['1',     'C/mole',  '1',             '1',     'J/mole/K']
;
cnsts[16:20] = ['Gravity',  'Hbar', 'PerfectGasVolume', 'Pi', 'Planck']
units[16:20] = ['N*m2/kg2', 'J*s',  'm3 / mole',        '1',  'J*s']
;
cnsts[21:24] = ['ProtonMass', 'Rydberg', 'Speedlight', 'StandardGravity']
units[21:24] = ['kg',         'm-1',     'm/s',        'm/s2']
;
cnsts[25:27] = ['StandardPressure', 'StefanBoltzman', 'WaterTriple'] 
units[25:27] = ['N/m2',             'W/K4 /m2',       'K']
;;
;; testing all possible calling sequences with the default units
;;
for i = 0, n_elements(cnsts) - 1 do if strlen(cnsts[i]) NE 0 then begin
    a = IMSL_CONSTANT(cnsts[i])
    b = IMSL_CONSTANT(cnsts[i], units[i])
    a = IMSL_CONSTANT(cnsts[i], /double)
    b = IMSL_CONSTANT(cnsts[i], units[i], /double)
    if a NE b then begin
        MESSAGE, 'FAILED: ' + cnsts[i] + ' [' + units[i] + ']', /conti
        EXIT, status=1
    endif
endif
;;
;; testing unit conversion
;;
;; - time
;;
u = 'm/day'    & if ~FINITE(IMSL_CONSTANT('c', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'm/hour'   & if ~FINITE(IMSL_CONSTANT('c', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'm/hr'     & if ~FINITE(IMSL_CONSTANT('c', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'm/min'    & if ~FINITE(IMSL_CONSTANT('c', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'm/minute' & if ~FINITE(IMSL_CONSTANT('c', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'm/s'      & if ~FINITE(IMSL_CONSTANT('c', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'm/sec'    & if ~FINITE(IMSL_CONSTANT('c', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'm/second' & if ~FINITE(IMSL_CONSTANT('c', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'm/year'   & if ~FINITE(IMSL_CONSTANT('c', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - frequency
;;
u = 'm*Hz'     & if ~FINITE(IMSL_CONSTANT('c', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - mass
;;
u = 'AMU'      & if ~FINITE(IMSL_CONSTANT('electronmass', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'g'        & if ~FINITE(IMSL_CONSTANT('electronmass', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'gram'     & if ~FINITE(IMSL_CONSTANT('electronmass', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'lb'       & if ~FINITE(IMSL_CONSTANT('electronmass', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'pound'    & if ~FINITE(IMSL_CONSTANT('electronmass', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;  u = 'ounce'    & if ~FINITE(IMSL_CONSTANT('electronmass', u)) then begin
;    MESSAGE, 'FAILED! ' + u, /conti
;    EXIT, status=1
;  endif
;  u = 'oz'       & if ~FINITE(IMSL_CONSTANT('electronmass', u)) then begin
;    MESSAGE, 'FAILED! ' + u, /conti
;    EXIT, status=1
;  endif
u = 'slug'     & if ~FINITE(IMSL_CONSTANT('electronmass', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - distance
;;
u = 'Angstrom' & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;  u = 'AU'       & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin
;    MESSAGE, 'FAILED! ' + u, /conti
;    EXIT, status=1
;  endif
u = 'feet'     & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'foot'     & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'in'       & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'inch'     & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'm'        & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'meter'    & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'metre'    & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'micron'   & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'mile'     & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;  u = 'mill'     & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin 
;    MESSAGE, 'FAILED! ' + u, /conti
;    EXIT, status=1
;  endif
u = 'parsec'   & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'yard'     & if ~FINITE(IMSL_CONSTANT('AU', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - area
;;
u = 'N/acre'   & if ~FINITE(IMSL_CONSTANT('ATM', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - volume
;;
u = 'l/mole'    & if ~FINITE(IMSL_CONSTANT('PerfectGasVolume', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'liter/mole'& if ~FINITE(IMSL_CONSTANT('PerfectGasVolume', u)) then begin 
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'litre/mole'& if ~FINITE(IMSL_CONSTANT('PerfectGasVolume', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - force
;;
u = 'dyne/m^2'  & if ~FINITE(IMSL_CONSTANT('ATM', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'N/m^2'     & if ~FINITE(IMSL_CONSTANT('ATM', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'Newton/m^2'& if ~FINITE(IMSL_CONSTANT('ATM', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - energy
;;
u = 'BTU'   & if ~FINITE(IMSL_CONSTANT('ElectronVolt', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'Erg'   & if ~FINITE(IMSL_CONSTANT('ElectronVolt', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'J'     & if ~FINITE(IMSL_CONSTANT('ElectronVolt', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'Joule' & if ~FINITE(IMSL_CONSTANT('ElectronVolt', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - work
;;
u = 'W*s^2'    & if ~FINITE(IMSL_CONSTANT('Hbar', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'watt*s^2' & if ~FINITE(IMSL_CONSTANT('Hbar', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - pressure
;;
;  u = 'ATM'        & if ~FINITE(IMSL_CONSTANT('ATM', u)) then begin
;    MESSAGE, 'FAILED! ' + u, /conti
;    EXIT, status=1
;  endif
u = 'atmosphere' & if ~FINITE(IMSL_CONSTANT('ATM', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'bar'        & if ~FINITE(IMSL_CONSTANT('ATM', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - temperature
;;
u = 'degC'       & if ~FINITE(IMSL_CONSTANT('WaterTriple', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'Celsius'    & if ~FINITE(IMSL_CONSTANT('WaterTriple', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'degF'       & if ~FINITE(IMSL_CONSTANT('WaterTriple', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;  u = 'Fahrenheit' & if ~FINITE(IMSL_CONSTANT('WaterTriple', u)) then begin
;    MESSAGE, 'FAILED! ' + u, /conti
;    EXIT, status=1
;  endif
u = 'degK'       & if ~FINITE(IMSL_CONSTANT('WaterTriple', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'Kelvin'     & if ~FINITE(IMSL_CONSTANT('WaterTriple', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - viscosity
;;
;  u = 'stoke/m'    & if ~FINITE(IMSL_CONSTANT('Speedlight', u)) then begin
;    MESSAGE, 'FAILED! ' + u, /conti
;    EXIT, status=1
;  endif
u = 'poise/s'    & if ~FINITE(IMSL_CONSTANT('ATM', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - charge
;;
;  u = 'Abcoulomb'   & if ~FINITE(IMSL_CONSTANT('electroncharge', u)) then begin 
;    MESSAGE, 'FAILED! ' + u, /conti
;    EXIT, status=1
;  endif
u = 'C'           & if ~FINITE(IMSL_CONSTANT('electroncharge', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'Coulomb'     & if ~FINITE(IMSL_CONSTANT('electroncharge', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'statcoulomb' & if ~FINITE(IMSL_CONSTANT('electroncharge', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif

                                ; - current
u = 'A*s'          & if ~FINITE(IMSL_CONSTANT('electroncharge', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'ampere*s'     & if ~FINITE(IMSL_CONSTANT('electroncharge', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'abampere*s'   & if ~FINITE(IMSL_CONSTANT('electroncharge', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'statampere*s' & if ~FINITE(IMSL_CONSTANT('electroncharge', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - voltage
;;
u = 'J/Abvolt' & if ~FINITE(IMSL_CONSTANT('electroncharge', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'J/V'      & if ~FINITE(IMSL_CONSTANT('electroncharge', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'J/volt'   & if ~FINITE(IMSL_CONSTANT('electroncharge', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - magnetic induction
;;
u = 'T*C*s'     & if ~FINITE(IMSL_CONSTANT('electronmass', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'Tesla*C*s' & if ~FINITE(IMSL_CONSTANT('electronmass', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'Weber*A'   & if ~FINITE(IMSL_CONSTANT('electronvolt', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'Wb*A'      & if ~FINITE(IMSL_CONSTANT('electronvolt', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; - other units
;;
u = '1/mole'     & if ~FINITE(IMSL_CONSTANT('avogadro', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'farad*V'    & if ~FINITE(IMSL_CONSTANT('electroncharge', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'Gauss*C*s'  & if ~FINITE(IMSL_CONSTANT('electronmass', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'Henry*C2/s' & if ~FINITE(IMSL_CONSTANT('planck', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'Maxwell*A'  & if ~FINITE(IMSL_CONSTANT('electronvolt', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
u = 'Ohm*s*A^2'  & if ~FINITE(IMSL_CONSTANT('electronvolt', u)) then begin
    MESSAGE, 'FAILED! ' + u, /conti
    EXIT, status=1
endif
;;
;; testing prefixes
;;
eps = 1d-6
au = IMSL_CONSTANT('AU', /d)
if IMSL_CONSTANT('AU', 'parsec', /d) - au * 1d18 GT eps then begin
    MESSAGE, 'FAILED! a', /conti
    EXIT, status=1
endif
if IMSL_CONSTANT('AU', 'parsec', /d) - au * 1d15 GT eps then begin
    MESSAGE, 'FAILED! f', /conti
    EXIT, status=1
endif
if IMSL_CONSTANT('AU', 'pm', /d) NE au * 1d12 then begin
    MESSAGE, 'FAILED! p', /conti
    EXIT, status=1
endif
if IMSL_CONSTANT('AU', 'nm', /d) NE au * 1d9 then begin
    MESSAGE, 'FAILED! n', /conti
    EXIT, status=1
endif
;  if IMSL_CONSTANT('AU', 'um', /d) NE au * 1d6 then begin
;    MESSAGE, 'FAILED! u', /conti
;    EXIT, status=1
;  endif
if IMSL_CONSTANT('AU', 'mm', /d) NE au * 1d3 then begin
    MESSAGE, 'FAILED! m', /conti
    EXIT, status=1
endif
if IMSL_CONSTANT('AU', 'cm', /d) NE au * 1d2 then begin
    MESSAGE, 'FAILED! c', /conti
    EXIT, status=1
endif
if IMSL_CONSTANT('AU', 'dm', /d) NE au * 1d1 then begin
    MESSAGE, 'FAILED! d', /conti
    EXIT, status=1
endif
if IMSL_CONSTANT('AU', 'dam', /d) NE au * 1d-1 then begin
    MESSAGE, 'FAILED! da', /conti
    EXIT, status=1
endif
if IMSL_CONSTANT('AU', 'km', /d) NE au * 1d-3 then begin
    MESSAGE, 'FAILED! k', /conti
    EXIT, status=1
endif
if IMSL_CONSTANT('AU', 'Gm', /d) NE au * 1d-9 then begin
    MESSAGE, 'FAILED! g', /conti
    EXIT, status=1
endif
if IMSL_CONSTANT('AU', 'Tm', /d) NE au * 1d-12 then begin
    MESSAGE, 'FAILED! t', /conti
    EXIT, status=1
endif
;;
;; testing if PI = PI
;;
if IMSL_CONSTANT('pi') NE !PI then begin
    MESSAGE, 'FAILED! (PI != PI)', /conti
    EXIT, status=1
endif
if IMSL_CONSTANT('pi', /double) NE !DPI then begin
    MESSAGE, 'FAILED! (DPI != DPI)', /conti
    EXIT, status=1
endif
;
if KEYWORD_SET(test) then STOP
;
end
