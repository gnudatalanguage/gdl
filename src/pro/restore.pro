pro restore,     filename0

cmrestore,       filename0, $
                 p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,  p8,  p9, $
                 p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, $
                 p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, $
                 names=_nme_

for i=0,n_elements(_nme_)-1 do begin
    p = strcompress('p' + string(i), /remove_all)
    parm = 'dummy=routine_names(_nme_[i],' + p + ',store=-1)
    stat = execute(parm)
endfor

return
end
