pro tick,tl,la,units,cs
;  tickformat=['(%"%6.6I")','(f6.2)','label_date']
  tickformat= ['(%"%6.6I!Cline2")','(f6.2)','label_date']
  toto=label_date(date_format="%M!C%D!C(%W)")
  !Y.TICKFORMAT=tickformat & !Y.TITLE="AAAAAAAAAAAAAA" &!X.TICKFORMAT=tickformat  & !X.TITLE="AAAAAAAAAAAAAA" &!P.POSITION = [0.4, 0.4, 0.9, 0.9]
  a=dist(10)
  device,set_character_size=cs
  plot,a,a,yticklen=tl,yticklay=la,ytickunits=units,xticklen=tl,xticklay=la,xtickunits=units,title='TITLE-TITLE-TITLE-TITLE',subtitle='SUBTITLE-SUBTITLE'
  !Y.TICKFORMAT="" & !Y.TITLE="" &!X.TICKFORMAT=""  & !X.TITLE="" &!P.POSITION = 0
end
pro tickv,tl,la,units,cs
;  tickformat=['(%"%6.6I")','(f6.2)','label_date']
  tickformat= ['(%"%6.6I!Cline2")','(f6.2)','label_date']
  toto=label_date(date_format="%Y%M!C%H%I!C(%W)")
  !Y.TICKFORMAT=tickformat & !Y.TITLE="AAAAAAAAAAAAAA" &!X.TICKFORMAT=tickformat  & !X.TITLE="AAAAAAAAAAAAAA" &!P.POSITION = [0.4, 0.4, 0.9, 0.9]
  n=10
  a=dist(n)
  defsysv,"!GDL",exists=isgdl
  if isgdl then z=randomu(33,10,/RAN1) else  z=randomu(33,10)
  device,set_character_size=cs
  plot,a,a,yticklen=tl,yticklay=la,ytickunits=units,xticklen=tl,xticklay=la,xtickunits=units,xtickv=z*n*n,xticks=8,ytickv=z*n,yticks=8
  !Y.TICKFORMAT="" & !Y.TITLE="" &!X.TICKFORMAT=""  & !X.TITLE="" &!P.POSITION = 0
end
pro axistick,tl,la,units,cs
  tickformat=['(%"%6.6I")','(f6.2)','label_date']
  toto=label_date(date_format="%Y %M %S (%W)")
  !Y.TICKFORMAT=tickformat & !Y.TITLE="AAAAAAAAAAAAAA" &!X.TICKFORMAT=tickformat  & !X.TITLE="AAAAAAAAAAAAAA" &!P.POSITION = [0.3, 0.3, 0.7, 0.7]
  device,set_character_size=cs
  plot,findgen(10),findgen(10),/nodata,xsty=4,ysty=4,title='TITLE-TITLE-TITLE-TITLE',subtitle='SUBTITLE-SUBTITLE'
  axis,0,0,xax=0,yticklen=tl,yticklay=la,ytickunits=units,xticklen=tl,xticklay=la,xtickunits=units
  axis,0,0,yax=0,yticklen=tl,yticklay=la,ytickunits=units,xticklen=tl,xticklay=la,xtickunits=units
  axis,0,10,xax=1,yticklen=tl,yticklay=la,ytickunits=units,xticklen=tl,xticklay=la,xtickunits=units
  axis,10,0,yax=1,yticklen=tl,yticklay=la,ytickunits=units,xticklen=tl,xticklay=la,xtickunits=units
  !Y.TICKFORMAT="" & !Y.TITLE="" &!X.TICKFORMAT=""  & !X.TITLE="" &!P.POSITION = 0
end
pro axistickv,tl,la,units,cs
  tickformat=['(%"%6.6I")','(f6.2)','label_date']
  toto=label_date(date_format="%Y %M %S (%W)")
  !Y.TICKFORMAT=tickformat & !Y.TITLE="AAAAAAAAAAAAAA" &!X.TICKFORMAT=tickformat  & !X.TITLE="AAAAAAAAAAAAAA" ;&!P.POSITION = [0.4, 0.4, 0.9, 0.9]
  n=10
  a=dist(n)
  defsysv,"!GDL",exists=isgdl
  if isgdl then z=randomu(33,10,/RAN1) else  z=randomu(33,10)
  device,set_character_size=cs
  plot,findgen(10),dist(10),/nodata,xsty=4,ysty=4,title='TITLE-TITLE-TITLE-TITLE',subtitle='SUBTITLE-SUBTITLE'
  axis,0.5,0.5,/norm,xax=0,yticklen=tl,yticklay=la,ytickunits=units,xticklen=tl,xticklay=la,xtickunits=units,xtickv=z*n*n,xticks=8,ytickv=z*n,yticks=8
  axis,0.5,0.5,/norm,yax=0,yticklen=tl,yticklay=la,ytickunits=units,xticklen=tl,xticklay=la,xtickunits=units,xtickv=z*n*n,xticks=8,ytickv=z*n,yticks=8
  !Y.TICKFORMAT="" & !Y.TITLE="" &!X.TICKFORMAT=""  & !X.TITLE="" &!P.POSITION = 0
end

pro test_box_axis, no_exit=no_exit, test=test

  nb_errors=0
  set_plot,'z'
  device, set_resolution=[1920,1080]
  axistick,0.1,2,[".","."],[8,20]
  a=tvrd()
  if (fix(total(a),type=3) ne 4822305) then nb_errors++
  axistick,-0.1,2,[".",".","."],[8,20]
  a=tvrd()
  if (fix(total(a),type=3) ne 7938150) then nb_errors++
  tick,-0.1,2,[".",".","."],[8,15]
  a=tvrd()
  if (fix(total(a),type=3) ne 5124990) then nb_errors++
  tick,0.1,2,[".",".","."],[8,15]
  a=tvrd()
  if (fix(total(a),type=3) ne 4535940) then nb_errors++
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, "TEST_LABEL_DATE", nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;  
end
