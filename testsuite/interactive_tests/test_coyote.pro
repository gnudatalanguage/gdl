; ------------------------------------
;
function DoWeBreak, tictac=tictac
;
; when tictac set, we just wait a given time then continue
; (no interative question, no break)
;
if KEYWORD_SET(tictac) then begin
   WAIT, tictac
   return, 0
endif
;
msg='Press Q to quit, any key or Enter to continue '
;
key=''
read, prompt=msg, key
;
if (STRUPCASE(key) EQ 'Q') then return, 1 else return, 0
;
end
;

PRO Basic_Line_Plot
   data = cgDemoData(1)
   time = cgScaleVector(Findgen(N_Elements(data)), 0, 6)
   xtitle = 'Time'
   ytitle = 'Signal Strength'
   title = 'Basic Line Plot'
   position = [0.125, 0.125, 0.9, 0.925]
   cgDisplay, 600, 500, Title='Basic Line Plot'
   cgPlot, time, data, Color='red', PSym=-16, SymColor='olive', $
      SymSize=1.5, Title=title, XTitle=xtitle, YTitle=ytitle, $
      Position=position
END 

PRO Basic_Line_plot_PS
  data = cgDemoData(1)
  time = cgScaleVector(Findgen(N_Elements(data)), 0, 6)
  cgPlot, time, data, $
          Color='red', $
          PSym=-16, $
          SymColor='olive', $
          SymSize=1.5, $
          Title='Basic Line Plot', $
          XTitle='Time', $
          YTitle='Signal Strength', $
          Position=[0.125, 0.125, 0.9, 0.925], $
          Output='basic_line_plot.ps'
END

PRO Plot_With_Legend
  data_1 = cgDemoData(17)
  data_2 = cgDemoData(17)
  maxData = Max(data_1) > Max(data_2)
  minData = Min(data_1) > Min(data_2)
  dataRange = maxData - minData
  yrange = [MinData, maxData + (dataRange*0.25)]
  cgPlot, data_1, PSym=-15, Color='red7', YRange=yrange, YStyle=1, $
          XTitle='Time', YTitle='Signal'
  cgPlot, data_2, PSym=-16, Color='blu7', /Overplot
  items = ['Experiment 1', 'Experiment 2']
  psyms = [-15, -16]
  colors = ['red7', 'blu7']
  yloc = (!Y.CRange[1] - !Y.CRange[0]) * 0.95 + !Y.CRange[0]
  xloc = (!X.CRange[1] - !X.CRange[0]) * 0.05 + !X.CRange[0]
  cgLegend, Title=items, PSym=psyms, Lines=lines, Color=colors,$
            Location=[xloc,yloc], /Box, /Data
       
END 

PRO Additional_Axes_Plot
    data_1 = cgScaleVector(cgDemodata(17), 0.0, 1.0)
    data_2 = cgScaleVector(cgDemodata(17), 0.0, 1000.0)
    data_3 = (Findgen(101)+1) / 5
    thick = (!D.Name EQ 'PS') ? 6 : 2
    cgDisplay, 600, 450
    cgPlot, data_1, YStyle=4, Position=[0.15, 0.15, 0.7, 0.820], /NoData
    cgAxis, YAxis=0.0, /Save, Color='red7', YTitle='Data 1'
    cgOPlot, data_1, Color='red7', Thick=thick
    cgAxis, YAxis=1.0, /Save, Color='grn7', YTitle='Data 2', YRange=[0,1000]
    cgOPlot, data_2, Color='grn7', LineStyle=2, Thick=thick
    cgAxis, 0.85, 0.15, /Normal, YAxis=1.0, /Save, Color='blu7', YTitle='Data 3', YRange=[0.1,100], /YLog
    cgOPlot, data_3, Color='blu7', LineStyle=1, Thick=thick
END        

PRO Colored_Line_Plot
   data = cgDemoData(17)
   time = cgScaleVector(Findgen(N_Elements(data)), 0, 6)
   thick = (!D.Name EQ 'PS') ? 6 : 3
   cgDisplay
   !P.Multi=[0,1,2]
   timeColors = Byte(Round(cgScaleVector(Findgen(N_Elements(data)), 0, 255)))
   colors = cgScaleVector(Findgen(N_Elements(data)), Min(data), Max(data))
   elevColors = Value_Locate(colors, data)
   elevColors = Byte(Round(cgScaleVector(elevColors, 0, 255)))
   cgLoadCT, 34
   cgPlot, time, data, /NoData, XTitle='Time', YTitle='Elevation', Label='Colored in Time'
   FOR j=0,N_Elements(data)-2 DO cgPlotS, [time[j], time[j+1]], [data[j], data[j+1]], $
       Color=timeColors[j], Thick=thick
   FOR j=0,N_Elements(data)-1,2 DO cgPlotS, time[j], data[j], PSym=2, $
       SymSize=1.5, Color=timeColors[j]
   cgPlot, time, data, /NoData, XTitle='Time', YTitle='Elevation', Label='Colored in Elevation'
   FOR j=0,N_Elements(data)-2 DO cgPlotS, [time[j], time[j+1]], [data[j], data[j+1]], $
       Color=elevColors[j], Thick=thick
   FOR j=0,N_Elements(data)-1,2 DO cgPlotS, time[j], data[j], PSym=2, $
       SymSize=1.5, Color=elevColors[j]
   !P.Multi = 0
END 

PRO Error_Estimate_Plot
   data = cgDemoData(1)
   time = cgScaleVector(Findgen(N_Elements(data)), 0, 6)
   high_error = (data + cgScaleVector(RandomU(seed, N_Elements(data)), 3, 7)) < 35
   low_error = (data - cgScaleVector(RandomU(seed, N_Elements(data)), 2, 6)) > (-5)
   xtitle = 'Time'
   ytitle = 'Signal Strength'
   title = 'Error Estimate Plot'
   position = [0.125, 0.125, 0.9, 0.925]
   cgDisplay, 600, 500, Title='Error Estimate Plot'
   cgPlot, time, data, Title=title, XTitle=xtitle, YTitle=ytitle, $
      Position=position, /NoData, YRange=[-5, 35], YStyle=1
   cgColorFill, [time, Reverse(time), time[0]], $
       [high_error, Reverse(low_error), high_error[0]], $
       Color='sky blue'
   cgPlotS, time, data, Color='red', PSym=-16, SymColor='olive', $
      SymSize=1.0, Thick=2
END 

PRO Errorbar_Plot
   data = Congrid(cgDemoData(1), 15)
   seed = -5L
   time = cgScaleVector(Findgen(N_Elements(data)), 1, 9)
   high_yerror = RandomU(seed, N_Elements(data)) * 5 > 0.5
   low_yerror = RandomU(seed, N_Elements(data)) * 4 > 0.25
   high_xerror = RandomU(seed, N_Elements(data)) * 0.75 > 0.1
   low_xerror = RandomU(seed, N_Elements(data))  * 0.75 > 0.1
   xtitle = 'Time'
   ytitle = 'Signal Strength'
   title = 'Error Bar Plot'
   position = [0.125, 0.125, 0.9, 0.925]
   thick = (!D.Name EQ 'PS') ? 3 : 1
   cgDisplay, 600, 500, Title='Errorbar Plot'
   cgPlot, time, data, Color='red5', PSym=-16, SymColor='olive', $
       SymSize=1.0, Thick=thick, Title=title, XTitle=xtitle, YTitle=ytitle, $
       Position=position, YRange=[-5, 35], XRange=[0,10], YStyle=1, $
       ERR_XLow=low_xerror, ERR_XHigh=high_xerror, $
       ERR_YLow=low_yerror, ERR_YHigh=high_yerror, ERR_Color='blu5'
END 

PRO Plot_Within_Plot
   y = cgDemoData(1)
   x = Findgen(N_Elements(y))
   zoomIn = [50,65]
   cgDisplay
   maxData = Max(y)
   cgPlot, x, y, YRange=[0,maxData*1.5], Position=[0.125, 0.125, 0.9, 0.9], $
       XTitle='Time', YTitle='Signal'
   saveX = !X
   saveY = !Y
   saveP = !P
   charsize = cgDefCharsize()*0.75
   pos = [0.5, 0.65, 0.85, 0.825]
   cgColorFill, Position=pos, Color='yellow'
   cgPlot, x[zoomIn[0]:zoomIn[1]], y[zoomIn[0]:zoomIn[1]], $
       Position=pos, /NoErase, XStyle=1, Color='Red', Charsize=charsize, $
       PSym=-2, Symcolor='dodger blue', YNoZero=1, Label='Critical Region'
   !X = saveX
   !Y = saveY
   !P = saveP
   miny = Min(y[zoomIn[0]:zoomIn[1]], Max=maxy)
   coordNormal = Convert_Coord([ x[zoomIn[0]], x[zoomIn[1]] ], [ miny, maxy ], /Data, /To_Normal)
   cgPlotS, [coordNormal[0,0], coordNormal[0,1], coordNormal[0,1], coordNormal[0,0], coordNormal[0,0]], $
       [coordNormal[1,0], coordNormal[1,0], coordNormal[1,1], coordNormal[1,1], coordNormal[1,0]], $
       Thick=(!D.Name EQ 'PS') ? 4 : 2, Color='RED5', /Normal
   cgPlotS, [coordNormal[0,0], 0.50], [coordNormal[1,1], 0.65], LineStyle=2, Color='blu4', /Normal
   cgPlotS, [coordNormal[0,1], 0.85], [coordNormal[1,1], 0.65], LineStyle=2, Color='blu4', /Normal

END 

PRO MultiAnnotate_Plot
   data = cgScaleVector(cgDemoData(2), 0, 80)
   positions = cgLayout([2,2], OYMargin=[10,12], YGap=4, OXMargin=[6,4])
   cgLoadCT, 4, /Brewer, /Reverse, NColors=10, Bottom=1
   p = positions[*,0]
   cgContour, data, Position=p, NLevels=10, $
       C_Colors=Indgen(10)+1, /Fill, /Outline, Title='Contour Plot', $
       Charsize=cgDefCharsize()*0.65
   yspace = 4.0*(!D.Y_CH_SIZE)/!D.Y_SIZE
   p = [p[0], p[3]+yspace, p[2], p[3]+yspace+0.02]
   cgColorbar, NColors=10, Bottom=1, Range=[Min(data),Max(data)], Position=p, $
       Charsize=cgDefCharsize()*0.65, /Discrete
   p = positions[*,1]
   cgLoadCT, 29, /Brewer, RGB_Table=palette
   cgImage, data, Position=p, Palette=palette, /Scale, $
       /Axes, /NoErase, Title='Image Plot', Charsize=cgDefCharsize()*0.65
   p = [p[0], p[3]+yspace, p[2], p[3]+yspace+0.02]
   cgColorbar, Range=[Min(data),Max(data)], Position=p, $
       Charsize=cgDefCharsize()*0.65, Palette=palette
   cgLoadCT, 33, RGB_Table=rainbowPalette
   yspace = 5.0*(!D.Y_CH_SIZE)/!D.Y_SIZE
   p = positions[*,2]
   cgSurf, data, Position=p, ROTX=60, ZCharsize=cgDefCharsize()*1.25, $
       /NoErase, /Elevation, /Shaded, Palette=rainbowPalette, Title='Elevation Colors', $
       TSize=0.85, TLocation=[0.20, 0.425]
   p = [0.1, p[1]-yspace-0.02, 0.425, p[1]-yspace]
   cgColorbar, Palette=rainbowPalette, Range=[Min(data),Max(data)], Position=p, $
      Charsize=cgDefCharsize()*0.65
   cgLoadCT, 4, RGB_Table=blugrn_palette
   p = positions[*,3]
   cgSurf, data, Position=p, ROTX=60, ZCharsize=cgDefCharsize()*1.25, $
      /NoErase, /Shaded, Palette=blugrn_palette, Title='Wire Elevation Colors', $
       TSize=0.85, TLocation=[0.70, 0.425]
   cgSurf, data, Position=positions[*,3], ROTX=60, ZCharsize=cgDefCharsize()*1.25, $
      /NoErase, /Elevation, Palette=blugrn_palette
       
   p = [0.585, p[1]-yspace-0.02, 0.915, p[1]-yspace]
   cgColorbar, Palette=blugrn_palette, Range=[Min(data),Max(data)], Position=p, $
      Charsize=cgDefCharsize()*0.65
   
   cgText, 0.5, 0.965, /Normal, 'Multiple Annotated Plots', $
       Charsize=cgDefCharsize()*1.25, Alignment=0.5
END 

PRO Height_Filled_Area_Plot
   x = Findgen(101)
   y = 4 * Sin(x * !DtoR) / Exp( (x-15) / 25.)
   cgDisplay, /Free, Title='Fill Area Under a Curve'
   cgPlot, x, y, /NoData
   low = 10
   high = 45
   lowY  = 4 * Sin(low * !DtoR) / Exp( (low-15) / 25.)
   highY = 4 * Sin(high * !DtoR) / Exp( (high-15) / 25.)
   indices = Value_Locate(x, [low, high])
   lowIndex = indices[0]
   highIndex = indices[1]
   IF x(lowIndex) LT low THEN lowIndex = lowIndex + 1
   IF x(highIndex) GT high THEN highIndex = highIndex - 1
   cgLoadCT, 4, /Brewer, Clip=[50, 230]
   min_y = Min(y[lowIndex:highIndex], Max=max_y)
   colors = BytScl(y, MIN=min_y, MAX=max_y)
   FOR j=lowIndex,highIndex-1 DO BEGIN
       xpoly = [x[j],         x[j], x[j+1],       x[j+1],         x[j]]
       ypoly = [!Y.CRange[0], y[j], y[j+1], !Y.CRange[0], !Y.CRange[0]]
       cgColorFill, xpoly, ypoly, Color=colors[j]
   ENDFOR
   xpoly = [         low,  low, x[lowIndex:highIndex],  high,         high]
   ypoly = [!Y.CRange[0], lowY, y[lowIndex:highIndex], highY, !Y.CRange[0]]
   thick = (!D.Name EQ 'PS') ? 5 : 2
   cgPolygon, xpoly, ypoly, color='charcoal', Thick=2
   cgPlot, x, y, /NoData, /NoErase, XTitle='X Axis', YTitle='Y Axis'
   cgOPlot, x, y, Color='navy', Thick=thick
END 

PRO StreamFunction_Plot
    nlevels = 13
    x = (0.1 * Findgen(100) - 4.95) # (Fltarr(100)+1)
    y = (Fltarr(100)+1) # (0.1 * Findgen(100) - 4.95)
    r = SQRT(x^2 + y^2)
    psi = (r*r - 1/(r*r)) * Sin(2*ATan(y,x)) + 10 * ALog(r) / !PI
    cgDisplay, 700, 800
    levels = [Min(psi), cgScaleVector(Findgen(nlevels), -6, 6)]
    cgLoadct, 2, NColors=nlevels, Bottom=1, /Brewer
    TVLCT, cgColor('charcoal', /Triple), 0
    TVLCT, cgColor('gray', /Triple), nlevels
    cgContour, psi, x, y, Levels=levels, /Fill, /Outline, Label=3, $
        C_Colors=Indgen(nlevels+1), Aspect=1.0, $
        Position=[0.125, 0.125, 0.9, 0.8], XTitle='X', YTitle='Y'
    theta = 2 * !PI * Findgen(100)/100
    thick = (!D.Name EQ 'PS') ? 4 : 2
    cgPlotS,  Cos(theta), Sin(theta), Color='red', Thick=thick
    cgText, 2, 1.75, 'Unit Circle', Color='red', CharSize=cgDefCharsize()*1.25
    cgArrow, 0.7100, 0.5700, 0.5771, 0.5175, /Normal, Color='red', /Solid, Thick=thick
    cgText, -3, -4, Alignment=0.5, 'U$\subO$=1, $\gamma$=20', Charsize=cgDefCharsize()*1.25
    cgColorbar, NColors=nlevels-2, Bottom=1, OOB_Low=0B, OOB_High=Byte(nlevels), $
        Divisions=10, /Discrete, Range=[-6, 6], Position=[0.2, 0.875, 0.8, 0.905], $
        Title='$\phi$ = U$\subO$(r$\up2$-1/r$\up2$)sin2$\theta$+($\gamma$/2$\pi$)log(r)', $
        TLocation='top', TCharSize=cgDefCharsize()*1.25, Format='(i0)'
END 

PRO Basic_Contour_Plot
   data = cgDemoData(26)
   minValue = Floor(Min(data))
   nLevels = 10
   xtitle = 'X Axis'
   ytitle = 'Y Axis'
   position = [0.125, 0.125, 0.9, 0.9]
   title = 'Basic Contour Plot'
   cgDisplay, 600, 500, Title='Basic Contour Plot'
   contourLevels = cgConLevels(data, NLevels=10, MinValue=minValue)
   cgContour, data,Levels=contourLevels, Color='purple', $
      Position=position, XTitle=xtitle, YTitle=ytitle, Title=title
END 

PRO Histo_Plot
   data = randomU(seed, 1000)
   cgDisplay
   cgHistoplot, data, /Frequency, /OProbability, ProbColor='red', $
      ProbThick=thick, PolyColor='dodger blue', /FillPolygon, DataColor='navy'
END 

PRO Scatter_Plot_3D
    seed = 1L
    x = RandomU(seed, 32)
    y = RandomU(seed, 32)
    z = Exp(-3 * ((x - 0.5)^2 + (y - 0.5)^2))
    cgLoadCT, 33
    zcolors = BytScl(z)
    cgSurf, DIST(5), /NODATA, /SAVE, XRANGE=[0,1], $
       YRANGE=[0,1], ZRANGE=[0, 1], XSTYLE=1, $
       YSTYLE=1, ZSTYLE=1, CHARSIZE=2.0, $
       POSITION=[0.1, 0.1, 0.95, 0.95, 0.1, 0.95], $
       XTICKLEN=1, YTICKLEN=1, XGRIDSTYLE=1, YGRIDSTYLE=1
    cgAXIS, XAXIS=1, /T3D, CHARSIZE=2.0
    cgAXIS, YAXIS=1, /T3D, CHARSIZE=2.0
    phi = Findgen(32) * (!PI * 2 / 32.)
    phi = [ phi, phi(0) ]
    cgPlotS, x, y, z, PSYM=16, COLOR=zcolors, SYMSIZE=2.5, /T3D
    FOR j=0,31 DO cgPlotS, [x(j), x(j)], [y(j), y(j)], [0, z(j)], $
       COLOR=zcolors(j), /T3D
              
END

PRO test_coyote
  print,"  basic_line_plot"
  basic_line_plot
  if DoWeBreak(tictac=tictac) then goto, go_to_end
  print,"  plot_with_legend"
  plot_with_legend
  if DoWeBreak(tictac=tictac) then goto, go_to_end
  print,"  additional_axes_plot"
  additional_axes_plot
  if DoWeBreak(tictac=tictac) then goto, go_to_end
  print,"  colored_line_plot"
  colored_line_plot
  if DoWeBreak(tictac=tictac) then goto, go_to_end
  print,"  Error_Estimate_Plot"
  Error_Estimate_Plot
  if DoWeBreak(tictac=tictac) then goto, go_to_end
  print,"  Errorbar_Plot"
  Errorbar_Plot
  if DoWeBreak(tictac=tictac) then goto, go_to_end
  print,"  Plot_Within_Plot"
  Plot_Within_Plot
  if DoWeBreak(tictac=tictac) then goto, go_to_end
  print,"  MultiAnnotate_Plot"
  MultiAnnotate_Plot
  if DoWeBreak(tictac=tictac) then goto, go_to_end
  print,"  Height_Filled_Area_Plot"
  Height_Filled_Area_Plot
  if DoWeBreak(tictac=tictac) then goto, go_to_end
  print,"  StreamFunction_Plot"
  StreamFunction_Plot
  if DoWeBreak(tictac=tictac) then goto, go_to_end
  print,"  Basic_Contour_Plot"
  Basic_Contour_Plot
  if DoWeBreak(tictac=tictac) then goto, go_to_end
  print,"  Histo_Plot"
  Histo_Plot
  if DoWeBreak(tictac=tictac) then goto, go_to_end
  print,"  Scatter_Plot_3D"
  Scatter_Plot_3D
  if DoWeBreak(tictac=tictac) then goto, go_to_end
  print,"  basic_line_plot_ps"
  basic_line_plot_ps
  file_delete, 'basic_line_plot.ps'
;
go_to_end:
;
END
