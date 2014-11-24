; this file is part of GDL
; using the widgets prints the generated events
; 30. Oct 2013 Marc Schellens
; expanded by GD. Thanks to MIDI's GuiSelect procedure for ideas.
pro exit_gui,ev
  widget_control,ev.top,/DESTROY
end
pro cleanup, id
  widget_control,id,/DESTROY
end
pro toto, id
  print,"event in",id
end

pro handle_Event,ev
common mycount,count
help,ev,/str
  widget_control,ev.id,get_uvalue=uv 
  widget_control,ev.top,get_uvalue=topuv
  if n_elements(uv) gt 0 then begin
     print,"uvalue.type=",uv.type
     case uv.type of
        'file': begin
           widget_control,ev.id,get_value=value & print,value
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

pro test_widgets,table,help=help,nocanvas=nocanvas,block=block
common mycount,count
count=0
if ~keyword_set(block) then block=0
if keyword_set(help) then begin
print,"useage: test_widgets[,table][,/help]"
print,"Will display some examples of currently available widgets"
print,"if table is passed as argument and is a structure, tab 3 will show the"
print,"elements of the structure as buttons in a scrolled panel"
print,"(plus several test buttons), instead of the default one."
return
endif

ev = {vEv,type:'',pos:[0,0]}
;Create a base widget. 
base = WIDGET_BASE(COL=4,MBAR=mbar,title="gdl widget examples",event_pro=toto);,kill_notify="cleanup") 
 
menu = widget_button(mbar,VALUE="Menu")
ex = widget_button(menu,VALUE="Exit",EVENT_PRO="exit_gui")
siz= widget_button(menu,VALUE="Resize (error)",EVENT_PRO="resize_gui")
butt = widget_button(mbar,VALUE="ARealButton")
;buttons as menu buttons in menubar without /MENU: will crash gdl
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
tab2 = widget_base( tab, TITLE="with",/COL)
tab3 = widget_base( tab, TITLE="scrolling",/COL)
tab4 = widget_base( tab, TITLE="widgets",/COL)
tab5 = widget_base( tab, TITLE="now!",/COL)
;tab6 = widget_base( tab, TITLE="...",/COL)
 
;TAB1: Attach 256 x 256 draw widgets. 
if ~keyword_set(nocanvas) then draw = WIDGET_DRAW(tab1, XSIZE = 1800, YSIZE = 600,x_scroll_size=256,y_scroll_size=256) 
if ~keyword_set(nocanvas) then draw2 = WIDGET_DRAW(tab1, xoff=100, yoff=100, xsize=256,ysize=256,/button_events,keyboard_events=1) 


; TAB2: 
radio=widget_base(tab2,/EXCLUSIVE,COL=1)
rb1=widget_button(radio,VALUE="button in EXCLUSIVE base 1",uvalue={vEv,'rb1',[8,0]})
rb2=widget_button(radio,VALUE="button in EXCLUSIVE base 2",uvalue={vEv,'rb2',[9,0]})

check=widget_base(tab2,/NONEXCLUSIVE,COL=1)
cb1=widget_button(check,VALUE="button in NONEXCLUSIVE base 1",uvalue={vEv,'cb1',[81,0]})
cb2=widget_button(check,VALUE="button in NONEXCLUSIVE base 2",uvalue={vEv,'cb2',[12,0]})

sl=widget_slider(tab2,MIN=0,MAX=100,/DRAG,TITLE="WIDGET_SLIDER")


;tab3

   charpix = 8.5
   butSize = 7*charpix

actionBase=widget_base(tab3,/ROW)
   quitButton = widget_button(actionBase,value='WILL QUIT',uvalue={vEv,'quit',[5,0]})
   downButton = widget_button(actionBase,value='A 300 px Button..', xsize=300,uvalue={vEv,'down',[7,0]})
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
   fileBase=widget_base(tab3,y_scroll_size=250,x_scroll_size=300,/scroll,/row)
;base to contain column labels and cells with table values and selected files
   listentries=widget_base(filebase,row=nRows+1)
;create column labels 
   dummy = widget_label(listentries,value=' ', xsize=30)
   dummy1 = widget_label(listentries,value=' ',xsize=30)
   for icol =0, nCols-1 do tagButtons(icol)=widget_label(listentries,value=tags(icol), xsize=charpix*maxSize[icol],/align_left)
   for iRow=0,nRows-1 do begin
         fileNumbers(iRow)=widget_label(listentries, value=string(iRow), xsize=30, ysize=25)
         fileButtons(iRow)=widget_button(listentries, value="*", uvalue={vEv,'file',[iRow,-1]}, xsize=30, ysize=25)
      for icol=0,nCols-1 do buttons(icol, iRow) = widget_button(listentries, value=strtrim((table[iRow].(icol)),2), uvalue={vEv,'data',[icol,iRow]}, ysize=25,  xsize=charpix*maxSize[iCol],/align_left)
   endfor
;;overwrite label:
print,"label to update id="+string(labeltoupdate) 
;  widget_control, labeltoupdate, set_value="Text2 has been replaced with widget_control"



; tab4
bg=widget_base(tab4,/ROW)
values = ['One', 'Two', 'Three', 'Four', 'Five','Six'] 

bgroup1 = CW_BGROUP(bg, values, /COLUMN, /EXCLUSIVE, $ 
  LABEL_TOP='Exclusive', /FRAME, SET_VALUE=3) 
bgroup2 = CW_BGROUP(bg, values, /COLUMN, /NONEXCLUSIVE, $ 
  LABEL_TOP='Nonexclusive', /FRAME, SET_VALUE=[1,0,1,0,1]) 

; tab5 
label1=widget_label(tab5,VALUE="Text on the left (/align_left)",/align_left,uvalue={vEv,'lll',[1,-1]})
label2=widget_label(tab5,VALUE="...and on the right (/align_right)",/align_right,uvalue={vEv,'lll',[1,-1]})
text1=widget_text(tab5,VALUE=["This is a loooooooooooooooooooooong editable text widget",$
                        "created with","WIDGET_TEXT","without YSIZE keyword"],$
              /EDITABLE,/ALL_EVENTS)
text2=widget_text(tab5,VALUE=["Not editable text",$
                        "created with","WIDGET_TEXT","with YSIZE=10"],$
              EDITABLE=0,/ALL_EVENTS,XSIZE=32,YSIZE=10)
;
list=widget_list(tab5,value=["A","list","created","with","WIDGET_LIST","(no YSIZE)"])
droplist=widget_droplist(tab5,VALUE=["A","droplist","created","with","WIDGET_DROPLIST","(no YSIZE)"])
combo=widget_combobox(tab5,VALUE=["A","combobox","created","with","WIDGET_COMBOBOX","(no YSIZE)"])


; tab6: same, as widget_table: does not work AT ALL!
;mytable=widget_table(tab6,value=table,/row_major,row_labels='',column_labels=tags,column_width=50,/resizeable_columns,y_scroll_size=40)
; 
; overwrite buttons;
   for iRow=0,nRows-1 do widget_control,fileButtons(iRow) , set_value="Y"
;;


;Realize the widgets. 
WIDGET_CONTROL, /REALIZE, base 
 
;Obtain the window index. 
if ~keyword_set(nocanvas) then begin
print,"Draw widgets:",draw,draw2
 WIDGET_CONTROL, draw, GET_VALUE = index 
  WIDGET_CONTROL, draw2, GET_VALUE = index2 

;Set the new widget to be the current graphics window 
 print,"window indexes",index,index2
 file=FILE_SEARCH('*','Saturn.jpg')
 image=read_image(file)
 WSET,index
 plot,findgen(100)
 tv,image,10,10,/data,/true

 WSET, index2 
 f=findgen(1000)/100.
 contour,cos(dist(100,100)/10.)

end
xmanager,"handle",base,cleanup="cleanup",no_block=~block
end
