; this file is part of GDL
; using the widgets prints the generated events
; 30. Oct 2013 Marc Schellens
; expanded by GD. Thanks to MIDI's GuiSelect procedure for ideas.
;
pro exit_gui,ev
  widget_control,ev.top,/DESTROY
end
pro cleanup_xmanager, id
  print,"Cleaning up called by xmanager..."
  widget_control,id,/DESTROY
end
pro cleanup, id
  print,"Cleaning up called by base widget..."
  widget_control,id,/DESTROY
end
pro cleanup_control, id
  print,"Cleaning up called by widget_control..."
  widget_control,id,/DESTROY
end
pro i_am_realized, id
  print,"Widget "+string(id)+" is realized now."
end
pro event_in_base, id
  print,"event in base"
  help,id
  print,"event in "+string(id.id)
end
pro slider_killed,id
  print,"slider "+string(id)+" was killed"
end
pro button_killed,id
;  print,"button "+string(id)+" was killed"
end


pro handle_Event,ev
common mycount,count
help,ev,/str
  widget_control,ev.id,get_uvalue=uv 
  widget_control,ev.top,get_uvalue=topuv
  if n_elements(uv) gt 0 then begin
     if (strlen(tag_names(ev,/structure_name)) le 1) then begin
        print,"unhandled compound widget event"
        return
     end
;     print,"uvalue.type=",uv.type
     case uv.type of
        'file': begin
           widget_control,ev.id,get_value=value ;& print,value
           case value of
              "Y": widget_control,ev.id,set_value="N"
              "N": widget_control,ev.id,set_value="Y"
              else: return
           endcase
        end
        'data': begin
           widget_control,ev.id,get_value=value
           widget_control,topuv[0],set_value=value
           widget_control,topuv[1],set_value=value
           end
        'quit':  widget_control,ev.top,/DESTROY
        else: return
     endcase
  endif
end

pro test_widgets, table, nocanvas=nocanvas, notree=notree, $
                  block=block,fontname=fontname, help=help
common mycount, count
count=0
if ~keyword_set(fontname) then fontname="Fixed 32"
if ~keyword_set(block) then block=0
if keyword_set(help) then begin
    print,"useage: test_widgets[,table][,/help][,/nocanvas][,/notree]"
    print,"Will display some examples of currently available widgets"
    print,"if table is passed as argument and is a structure, tab 3 will show the"
    print,"elements of the structure as buttons in a scrolled panel"
    print,"options: /nocanvas removes the widget_draw"
    print,"         /notree remove the tree widget"
    print,"         fontname=""Helvetica Narrow 32"" to change a test text font."
return
endif
;
ev = {vEv,type:'',pos:[0,0]}
;Create a base widget. 
base = WIDGET_BASE(MBAR=mbar,title="gdl widget examples", $
                   event_pro='event_in_base',kill_notify='cleanup')
;
menu = WIDGET_BUTTON(mbar,VALUE="Menu")
ex = WIDGET_BUTTON(menu,VALUE="Exit",EVENT_PRO="exit_gui")
siz= WIDGET_BUTTON(menu,VALUE="Resize (error)",EVENT_PRO="resize_gui")
;buttons as menu buttons
        fileID = Widget_Button(mbar, Value='Complicated Menu')
        saveID = Widget_Button(fileID, Value='submenu 1', /MENU)
        button = Widget_Button(saveID, Value='entry 1', UNAME='POSTSCRIPT')
        button = Widget_Button(saveID, Value='entry 2', UNAME='PDF')
        raster = Widget_Button(saveID, Value='submenu 2', /MENU)
        
        button = Widget_Button(raster, Value='BMP', UNAME='RASTER_BMP')
        button = Widget_Button(raster, Value='GIF', UNAME='RASTER_GIF')
        button = Widget_Button(raster, Value='JPEG', UNAME='RASTER_JPEG')
        button = Widget_Button(raster, Value='PNG', UNAME='RASTER_PNG')
        button = Widget_Button(raster, Value='TIFF', UNAME='RASTER_TIFF')
        imraster = Widget_Button(saveID, Value='submenu 3', /MENU)
        button = Widget_Button(imraster, Value='BMP', UNAME='IMAGEMAGICK_BMP')
        button = Widget_Button(imraster, Value='GIF', UNAME='IMAGEMAGICK_GIF')
        button = Widget_Button(imraster, Value='JPEG', UNAME='IMAGEMAGICK_JPEG')
        button = Widget_Button(imraster, Value='PNG', UNAME='IMAGEMAGICK_PNG')
        button = Widget_Button(imraster, Value='TIFF', UNAME='IMAGEMAGICK_TIFF')
        button = Widget_Button(fileID, Value='entry 3', /Separator, UNAME='SAVECOMMANDS')
        button = Widget_Button(fileID, Value='entry 4', UNAME='RESTORECOMMANDS')
        button = Widget_Button(fileID, Value='entry 5', /Separator, UNAME='QUIT')


tab = widget_tab( base)
; note inversion tab5<->tab5
tab1 = widget_base( tab, TITLE="GDL",/COL)
tab2 = widget_base( tab, TITLE="is",/COL)
tab3 = widget_base( tab, TITLE="with",/COL)
tab4 = widget_base( tab, TITLE="lots",/COL)
tab5 = widget_base( tab, TITLE="of",/COL)
tab6 = widget_base( tab, TITLE="widgets",/COL)
tab7 = widget_base( tab, TITLE="now!",/COL)

;TAB1: Attach 256 x 256 draw widgets. 
if ~keyword_set(nocanvas) then begin 
draw = WIDGET_DRAW(tab1, XSIZE = 1800, YSIZE = 600,x_scroll_size=256,y_scroll_size=256,frame=20) 
draw2 = WIDGET_DRAW(tab1, xoff=100, yoff=100, xsize=256,ysize=256,/button_events,keyboard_events=1,/motion_events) 
end else begin
dummy=widget_label(tab1, value="WIDGET_DRAW not shown, (/NOCANVAS option)", font=fontname)
end
; TAB2: 
radio=widget_base(tab2,/EXCLUSIVE,COL=1)
rb1=widget_button(radio,VALUE="button in EXCLUSIVE base 1",uvalue={vEv,'rb1',[8,0]})
rb2=widget_button(radio,VALUE="button in EXCLUSIVE base 2",uvalue={vEv,'rb2',[9,0]})

check=widget_base(tab2,/NONEXCLUSIVE,COL=1)
cb1=widget_button(check,VALUE="button in NONEXCLUSIVE base 1",uvalue={vEv,'cb1',[81,0]})
cb2=widget_button(check,VALUE="button in NONEXCLUSIVE base 2",uvalue={vEv,'cb2',[12,0]})

sl1=widget_slider(tab2,MIN=0,MAX=100,TITLE="WIDGET_SLIDER-DRAG+FRAME",kill_notify='slider_killed',/DRAG,FRAME=3)
sl=widget_slider(tab2,MIN=0,MAX=100,TITLE="WIDGET_SLIDER-NORMAL-NOVAL",kill_notify='slider_killed',/SUPPRESS);


;tab3

   charpix = 8.5
   butSize = 7*charpix

actionBase=widget_base(tab3,/ROW)
   quitButton = widget_button(actionBase,value='WILL QUIT',uvalue={vEv,'quit',[5,0]})
   downButton = widget_button(actionBase,value='A 300 px Button..', /dynamic_resize)
selectBase=widget_base(tab3,/ROW)
;a label that can be updated by pushing buttons
   labeltoupdate = widget_label(selectBase, value='Label xsize=100(pixels),ysize=30(pixels)',$
      xsize=100,ysize=30,/align_left)
;show last button clicked value
   statusLabel = widget_text(selectBase,value='text, xsize=32(characters),ysize=10(chars)', $
      xsize=32,ysize=10)
   ;pass label to top (avoid using a common)
   widget_control,base,set_uvalue=[statusLabel,labeltoupdate]
;base for listing contents of tables and to show selected files
nrows=n_elements(table) ; passed table
if ( n_elements(table) eq 0 or size(table,/type) ne 8 ) then begin
tbltemplate={table2,string:'a very very very long string ',real:33.33,double:8.0080808D,another:'another',yanother:'very long string'} 
; make a long table of nrows
nrows=50
table=replicate(tbltemplate,nrows)
endif

;names of columns
tags = tag_names(table)
; nb of colums=nb tags
ncols=n_tags(table)
;set max size (chars) of displayed button/label for each column
maxSize = intarr(nCols)
for icol = 0, nCols-1 do maxSize[icol] = 1 + MAX(strlen(strtrim(table.(icol),2))) 
nrows=n_elements(table); check...
;buttons containing header keyword values
   buttons = lonarr(nCols,nRows)
;buttons containing column labels
   tagButtons=lonarr(nCols)
   fileButtons = lonarr(nRows)
   fileNumbers = lonarr(nRows)
;base for listing contents of tables and to show selected files
   fileBase=widget_base(tab3,/scroll,/frame,/row)
;base to contain column labels and cells with table values and selected files
   listentries=widget_base(filebase,row=nRows+1)
;create column labels 
   dummy = widget_label(listentries,value=' ', xsize=30)
   dummy1 = widget_label(listentries,value=' ',xsize=30)
   for icol =0, nCols-1 do tagButtons(icol)=widget_label(listentries,value=tags(icol), xsize=charpix*maxSize[icol],/align_left)
   for iRow=0,nRows-1 do begin
         fileNumbers(iRow)=widget_label(listentries, value=string(iRow), xsize=30, ysize=25)
         fileButtons(iRow)=widget_button(listentries, value="*", uvalue={vEv,'file',[iRow,-1]}, xsize=30, ysize=25)
      for icol=0,nCols-1 do buttons(icol, iRow) = widget_button(listentries, value=strtrim((table[iRow].(icol)),2), uvalue={vEv,'data',[icol,iRow]}, ysize=25,  xsize=charpix*maxSize[iCol],/align_left,kill_notify='button_killed')
   endfor
;;overwrite label:
;print,"label to update id="+string(labeltoupdate) 
  widget_control, labeltoupdate, set_value="Text2 has been replaced with widget_control"



; tab4
bg=widget_base(tab4,/ROW)
values = ['One', 'Two', 'Three', 'Four', 'Five','Six'] 

    CATCH,cw_bgroup_error
    if cw_bgroup_error eq 0 then bgroup1 = CW_BGROUP(bg, values, /COLUMN, /EXCLUSIVE, $ 
            LABEL_TOP='Exclusive', /FRAME, SET_VALUE=3) else $
        message,/contin," can't make a CW_Bgroup widget"
        CATCH,/CANCEL
;bgroup1 = CW_BGROUP(bg, values, /COLUMN, /EXCLUSIVE, $ 
;  LABEL_TOP='Exclusive', /FRAME, SET_VALUE=3) 
;bgroup2 = CW_BGROUP(bg, values, /COLUMN, /NONEXCLUSIVE, $ 
;  LABEL_TOP='Nonexclusive', /FRAME, SET_VALUE=[1,0,1,0,1]) 

    CATCH,cw_bgroup_error
    if cw_bgroup_error eq 0 then bgroup2 = CW_BGROUP(bg, values, /COLUMN, /NONEXCLUSIVE, $ 
  LABEL_TOP='Nonexclusive', /FRAME, SET_VALUE=[1,0,1,0,1])  else $
        message,/contin," can't make a CW_Bgroup widget"
        CATCH,/CANCEL


; tab5 
label1=widget_label(tab5,VALUE="Text on the left (/align_left)",/align_left,uvalue={vEv,'lll',[1,-1]},/sunken_frame)
label2=widget_label(tab5,VALUE="FONT: "+fontname,/align_left,uvalue={vEv,'lll',[1,-1]},font=fontname)
label3=widget_label(tab5,VALUE="...and on the right (/align_right)",/align_right,uvalue={vEv,'lll',[1,-1]})
text1=widget_text(tab5,VALUE=["This is a loooooooooooooooooooooong editable text widget",$
                        "created with","WIDGET_TEXT","without YSIZE keyword"],$
              /EDITABLE,/ALL_EVENTS)
text2=widget_text(tab5,VALUE=["Not editable text",$
                        "created with","WIDGET_TEXT","with YSIZE=10"],$
              EDITABLE=0,/ALL_EVENTS,XSIZE=32,YSIZE=10)
text3=widget_text(tab5,VALUE=["right align text"],/EDIT,/align_right);
list=widget_list(tab5,value=["A","list","created","with","WIDGET_LIST","(no YSIZE)"])
list=widget_list(tab5,value=["An aligned","list","created","with","WIDGET_LIST","(no YSIZE)"],/align_right)
droplist=widget_droplist(tab5,VALUE=["A","droplist","created","with","WIDGET_DROPLIST","(no YSIZE)"],title="A droplist")
droplist=widget_droplist(tab5,VALUE=["ib","pt"],title="A droplist")
droplist=widget_droplist(tab5,VALUE=["An aligned","droplist","created","with","WIDGET_DROPLIST","(no YSIZE)"],/align_right)
combo=widget_combobox(tab5,VALUE=["A not-aligned","not editable","combobox","created","with","WIDGET_COMBOBOX","(no YSIZE)"])
combo=widget_combobox(tab5,VALUE=["A right-aligned","combobox","created","with","WIDGET_COMBOBOX","(no YSIZE)"],/editable,/align_right)


; tab6: a widget_table
mytable1=widget_table(tab6,value=dist(7),xsize=5,ysize=5,/editable,/all_events)
;to be implemented!
;widget_control,mytable1,/editable,use_table_sel=[1,1,4,4]
widget_control,mytable1,edit_cell=[0,0]
widget_control,mytable1,background_color=[255,255,0],use_table_sel=[1,1,4,4]

ntable = min([5,nrows-1])
mytable2=widget_table(tab6,value=table[0:ntable],$
    /row_major,row_labels='',column_labels=tags,column_width=50,/resizeable_columns,y_scroll_size=40,/disjoint,/all_events)
;
;tab7: a tree table
if ~keyword_set(notree) then begin
  racine = widget_tree(tab7)
  wtRoot = widget_tree(racine, VALUE='GDL', /folder, /draggable, /drop_events)
  feuille_11 = WIDGET_TREE(wtRoot, VALUE='is', $
    UVALUE={vEv,'LEAF',[0,0]})
  branche_12 = WIDGET_TREE(wtRoot, VALUE='...', $
    /FOLDER, /EXPANDED)
  feuille_121 = WIDGET_TREE(branche_12, VALUE='with', $
    UVALUE={vEv,'LEAF',[0,0]})

  feuille_122 = WIDGET_TREE(branche_12, VALUE='a lot of', $
    UVALUE={vEv,'LEAF',[0,0]})

  feuille_13 = WIDGET_TREE(wtRoot, VALUE='widgets', $
    UVALUE={vEv,'LEAF',[0,0]})

  feuille_14 = WIDGET_TREE(wtRoot, VALUE='now', $
    UVALUE={vEv,'LEAF',[0,0]})

end
; overwrite buttons;
   for iRow=0,nRows-1 do widget_control,fileButtons(iRow) , set_value="Y"
;;

;tests for /notify_realize and /kill_notify
;
widget_control,base,notify_realize='i_am_realized'
;Realize the widgets. 
WIDGET_CONTROL, /REALIZE, base 
 
if ~keyword_set(nocanvas) then begin
;Obtain the window index. 
    index =0  & index2 = 1
    print,"Draw widgets:",draw,draw2
    WIDGET_CONTROL, draw, GET_VALUE = index 
    WIDGET_CONTROL, draw2, GET_VALUE = index2 
; 
;  Accomodate the "NO_WIDGET_DRAW" crowd:
;
; 0 <= index,index2 < 32

    if (!d.name eq "WIN") or (!d.name eq "X") $
            and (index lt 32) and (index2 lt 32) then begin
        device,window_state=wcheck
        if wcheck[index] eq 0  then window,index
        if wcheck[index2] eq 0  then window,index2
         endif
    ;;
    ;; Set the new widget to be the current graphics window 
    print,"window indexes",index,index2
    ; for window index 32 or greater we cannot call WINDOW to create but
    ; it may be "closed and unavailable" when WX is not the plot device.

    CATCH,wset_error
    if wset_error eq 0 then WSET,index else $
        message,/contin," can't plot to, or image on, the draw widget"
        CATCH,/CANCEL

    PLOT, FINDGEN(100)
    ;; we try to TV an image, skipped if not found ...
    ;; Read "Saturn.jpg" and return content in "image"
    status=GET_IMAGE_FOR_TESTSUITE(image)
    if status EQ 0 then begin
        print, 'Test Image not found, skipped ...'
    endif else begin
        TV, image,10,10, /data, /true    
    endelse
    ;;
    if wset_error ne 0 then wait,2
    
    CATCH,wset_error
    if wset_error eq 0 then WSET,index2 else $
        message,/contin," can't plot to, or image on, the draw widget"
        CATCH,/CANCEL

    f=FINDGEN(1000)/100.
    CONTOUR, COS(DIST(100,100)/10.)
end
;
xmanager,"handle",base,cleanup="cleanup_xmanager",no_block=~block
;
end
