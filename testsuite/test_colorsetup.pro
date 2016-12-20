; tests compliance of !D.n_colors, !P.color and !P.background
; when changing devices and setups.
; NOT TO BE USED with WINDOWS unless checking what devices are available,
; NOT to be added to test list: depends on cmsave.pro/cmrestore.pro which
; is not distributed with GDL. 

function do_test_colorsetup,device
  value=lonarr(9)
  set_plot,device
  if (device eq 'z' or device eq 'Z') then device,set_pixel_depth=24  
  value[0]=!D.n_colors
  value[1]=!P.color
  value[2]=!P.background
  device,decomp=0
  value[3]=!D.n_colors
  value[4]=!P.color
  value[5]=!P.background
  device,decomp=1
  value[6]=!D.n_colors
  value[7]=!P.color
  value[8]=!P.background
  return, value
end

pro test_colorsetup,build=build
  if ~n_elements(build) then build=0
  values=lonarr(4*9)
  dev=['X','PS','Z','XX']
  state=['before','dec=0','dec=1']
  param=['!D.N_COLORS','!P.COLORS','!P.BACKGROUND']

  values[0:8]=do_test_colorsetup('x')
  values[9:17]=do_test_colorsetup('ps')
  values[18:26]=do_test_colorsetup('z')
; this in case values at start of program are not the same as in the
; course of the program (difference befors command 'set_plot' and after.)
  values[27:35]=do_test_colorsetup('x')
  values=reform(values,3,3,4)
  if (build) then begin 
     save,filename='decomposed_return_values.sav',values
     return
  endif
  current=values
  restore,filename='decomposed_return_values.sav'
  w=where(current ne values, count)
  if count gt 0 then begin
    pos=array_indices(values,w)
    print,format='(A)', 'device    mode     parameter returned     correct' 
    for i=0,count-1 do print,format='(a6,x,a7,x,a13,x,i8,4x,i8)',dev[pos[2,i]],state[pos[1,i]],param[pos[0,i]],current[w[i]],values[w[i]]
 end
end
