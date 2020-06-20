fs=findgen(40) & !prompt="" & iw=-1
iw++&window,iw,xs=512,ys=512,xp=128*iw,yp=128*iw&plot,fs,chars=iw,thi=iw
iw++&window,iw,xs=512,ys=512,xp=128*iw,yp=128*iw&plot,fs,chars=iw,thi=iw
iw++&window,iw,xs=512,ys=512,xp=128*iw,yp=128*iw&plot,fs,chars=iw,thi=iw
iw++&window,iw,xs=512,ys=512,xp=128*iw,yp=128*iw&plot,fs,chars=iw,thi=iw
iw++&window,iw,xs=512,ys=512,xp=128*iw,yp=128*iw&plot,fs,chars=iw,thi=iw
iw++&window,iw,xs=512,ys=512,xp=128*iw,yp=128*iw&plot,fs,chars=iw,thi=iw
wshow,iw,0
wshow,iw-1,0
wshow,iw-2,0
wshow,iw-2,/icon
wshow,iw-2,0    
wshow,iw-3,0
wshow,0,0,/icon
wshow,0
end
