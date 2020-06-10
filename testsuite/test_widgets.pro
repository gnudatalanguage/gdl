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
pro base_event_ok, id
  print,"event in base Authorized"
  help,id
end
pro base_event_nok, id
  print,"FATAL: event in base Not Authorized"
  help,id
end
pro slider_killed,id
  print,"slider "+string(id)+" was killed"
end
pro button_killed,id
;  print,"button "+string(id)+" was killed"
end
pro draw_context,ev
  contextBase = WIDGET_INFO(ev.TOP, FIND_BY_UNAME = 'drawContext')
  print,contextbase,ev.top,ev.id,ev.x,ev.y
    ; Display the context menu.
    WIDGET_DISPLAYCONTEXTMENU, ev.ID, ev.X, ev.Y, contextBase
 end
pro deletedraw,ev
  tobedeleted = WIDGET_INFO(ev.TOP, FIND_BY_UNAME = 'drawToBeDeleted')
  widget_control,tobedeleted,/destroy
end

pro list_event,event
toto=["A","list","created","with","WIDGET_LIST","YSIZE=3"]
print,toto[event.index] 
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
     print,"uvalue.type=",uv.type
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
        else: begin
           print, "(other, not treated, event: ok)"
           return
        end
        
     endcase
  endif
  print, "(unhandled event: ok)"
end
pro doMbar,mbar
  menu = widget_button(mbar,VALUE="Simple Menu",font=fontname)
ex = widget_button(menu,VALUE="Exit",EVENT_PRO="exit_gui")
siz= widget_button(menu,VALUE="Resize (error)",EVENT_PRO="resize_gui")
;buttons as menu buttons
        fileID = Widget_Button(mbar, Value='Complicated Menu',font=fontname)
        saveID = Widget_Button(fileID, Value='submenu 1', /MENU)
        button = Widget_Button(saveID, Value='entry 1 (inactive)', UNAME='POSTSCRIPT',sensitive=0,font=fontname)
        button = Widget_Button(saveID, Value='entry 2', UNAME='PDF')
        raster = Widget_Button(saveID, Value='submenu 2', /MENU)
        
        button = Widget_Button(raster, Value='entry 1', UNAME='RASTER_BMP')
        button = Widget_Button(raster, Value='entry 2', UNAME='RASTER_GIF')
        button = Widget_Button(raster, Value='entry 3', UNAME='RASTER_JPEG')
        button = Widget_Button(raster, Value='entry 4', UNAME='RASTER_PNG')
        button = Widget_Button(raster, Value='entry 5', UNAME='RASTER_TIFF')
        imraster = Widget_Button(saveID, Value='submenu 3', /MENU)
        button = Widget_Button(imraster, Value='entry 1',UNAME='IMAGEMAGICK_BMP')
        button = Widget_Button(imraster, Value='entry 2',UNAME='IMAGEMAGICK_GIF')
        button = Widget_Button(imraster, Value='entry 3',UNAME='IMAGEMAGICK_JPEG')
        button = Widget_Button(imraster, Value='entry 4',UNAME='IMAGEMAGICK_PNG')
        button = Widget_Button(imraster, Value='entry 5', UNAME='IMAGEMAGICK_TIFF')
        button = Widget_Button(fileID, Value='entry 1', /Separator, UNAME='SAVECOMMANDS')
        button = Widget_Button(fileID, Value='entry 2', UNAME='RESTORECOMMANDS')
        button = Widget_Button(fileID, Value='entry 3 (separed)', /Separator, UNAME='QUIT')
end

pro test_widgets,table,help=help,nocanvas=nocanvas,notree=notree,block=block,fontname=fontname,_extra=extra
common mycount,count
count=0
title='GDL Widget Examples'
DEFSYSV, '!gdl', exists=isGDL
if ~keyword_set(fontname) then begin
   fontname=(isgdl)?"Die Nasty 18px":'-altsys-die nasty-bold-r-normal--18-0-0-0-p-0-ascii-0'
   title+=' (Using Fixed Font Monospace)'
endif else title+=' (Using '+fontname+')'
if (isGDL) then widget_control,default_font='New Century Schoolbook L 10px' else  widget_control,default_font="-urw-new century schoolbook-medium-r-normal--10-0-0-0-p-0-iso8859-1" ;-misc-liberation mono-medium-r-normal--10-0-0-0-m-0-iso8859-1"
if ~keyword_set(block) then block=0
if keyword_set(help) then begin
print,"useage: test_widgets[,table][,/help][,/nocanvas][,/notree]"
print,"Will display some examples of currently available widgets"
print,"if table is passed as argument and is a structure, TABLE tab will show the"
print,"elements of the structure as buttons in a scrolled panel"
print,"options: /nocanvas removes the widget_draw"
print,"              /notree remove the tree widget"
print,"              fontname=""Helvetica Narrow 32"" to change a test text font."

return
endif

;base for listing contents of tables and to show selected files
nrows=n_elements(table)         ; passed table
if ( n_elements(table) eq 0 or size(table,/type) ne 8 ) then begin
   tbltemplate={table2,string:'a very very very long string ',real:33.33,double:8.0080808D,another:'another',yanother:'very long string'} 
; make a long table of nrows
   nrows=50
   table=replicate(tbltemplate,nrows)
endif

ev = {vEv,type:'',pos:[0,0]}


;Create a base widget. 
base = WIDGET_BASE(/col,MBAR=mbar,title=title,event_pro='base_event_nok',kill_notify='cleanup') ;,/scroll)
doMbar,mbar
; define a tabbed base that contains everything:
label=widget_label(base,value='base contains a frame=12, tabbed base at (8,7)')
label=widget_label(base,value='Please note text sizes are OK only if font is of fixed width')

tabbed_base = widget_tab( base, xoff=8, yoff=7, frame=12) ;, scr_xsize=400, scr_ysize=400);, multiline=6)
offy=0
;TEXT
text_base = widget_base( tabbed_base, TITLE="TEXT",_extra=extra) & offy=0
label=widget_label(yoff=offy,text_base,value='0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789') & offy+=20;
text1=widget_text(yoff=offy,text_base,VALUE=["editable, all_events, no xsize, no ysize, fancy",$
                                   "created with ","WIDGET_TEXT "],$
                  /EDITABLE,/ALL_EVENTS,font=fontname) & offy+=30;
text2=widget_text(yoff=offy,text_base,VALUE=["Not editable, all_events, xsize=32, ysize=10",$
                                   "created with","WIDGET_TEXT","with YSIZE=10",'5','6','7','8','9','10'],$
                  EDITABLE=0,/ALL_EVENTS,YSIZE=10,xsize=32) & offy+=150;
text3=widget_text(yoff=offy,text_base,VALUE=["ça est éditable, frame=10: all events generated."],/EDIT,frame=10)  & offy+=50;
text3=widget_text(yoff=offy,text_base,VALUE=["fancy center aligned, not editable, noevents: no events generated, frame=10"],EDIT=0,/align_center,frame=10,font=fontname) & offy+=50 ;

;

;LIST
list_base = widget_base( tabbed_base, TITLE="LISTs",_extra=extra) & offy=0 ;
label=widget_label(yoff=offy,list_base,value='0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789') & offy+=10;
label=widget_label(yoff=offy,list_base,value='3 lines, placement inherited') & offy+=10;
list=widget_list(yoff=offy,list_base,value=["wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwidget_list, ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST", "ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST", "ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST"],/multiple,YSIZE=3) & offy+=100;
label=widget_label(yoff=offy,list_base,value='idem,fancy,framed(32 pix)') & offy+=10;
list=widget_list(yoff=offy,list_base,value=["wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwidget_list, ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST", "ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST"],/multiple,YSIZE=3,frame=32,font=fontname) & offy+=200;
label=widget_label(yoff=offy,list_base,value='3 lines, placement inherited, xsize=12 chars (scroll)') & offy+=10;
list=widget_list(yoff=offy,list_base,value=["widget_list, ysize=3","created","with","WIDGET_LIST"],/multiple,YSIZE=3,xsize=12) & offy+=150;
label=widget_label(yoff=offy,list_base,value='1 line, placement inherited, xsize=2 chars (scroll)') & offy+=10;
list=widget_list(yoff=offy,list_base,value=["widget_list, ysize=3","created","with","WIDGET_LIST"],/multiple,xsize=2) & offy+=150;
label=widget_label(yoff=offy,list_base,value='no size,fancy,framed(32 pix)') & offy+=10;
list=widget_list(yoff=offy,list_base,value=["widget_list","(no YSIZE, frame=32)"],frame=32,font=fontname) & offy+=50;
label=widget_label(yoff=offy,list_base,value='centered,frame=1',/align_center) & offy+=10;
list=widget_list(yoff=offy,list_base,value=["(centered)"," frame=1"],/align_center,frame=1) & offy+=50;
label=widget_label(yoff=offy,list_base,value='basic') & offy+=10;
list=widget_list(yoff=offy,list_base,value=["default"," basic"]) & offy+=50;
label=widget_label(yoff=offy,list_base,value='centered,basic',/align_center) & offy+=10;
list=widget_list(yoff=offy,list_base,value=["(centered)","basic"],/align_center) & offy+=10;

;DRAW_BASE: Attach 256 x 256 draw widgets. Offsets 256 pix
draw_base = widget_base( tabbed_base, TITLE="DRAW",_extra=extra) & offy=0 ;,YSIZE=1000,XSIZE=1000,Y_SCROLL=300);XPAD=25,YPAD=50,SPACE=30)
label=widget_label(yoff=offy,draw_base,value='0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789') & offy+=10;
; populate
if ~keyword_set(nocanvas) then begin 
   draw = WIDGET_DRAW(yoff=offy,draw_base, XSIZE = 1800, YSIZE = 600,x_scroll_size=256,y_scroll_size=256,frame=20)  & offy+=300;
   draw2 = WIDGET_DRAW(yoff=offy,draw_base, xoff=100, xsize=256,ysize=256,/button_events,keyboard_events=1,/motion_events)  & offy+=256;
   tmp=widget_label(xoff=200,yoff=offy,draw_base,value="below, has tooltip and has a popup") & offy+=10;
   draw3 = WIDGET_DRAW(xoff=200,yoff=offy,draw_base,/BUTTON_EVENTS, xsize=100,ysize=100,tooltip="WIGDET_DRAW",EVENT_PRO = 'draw_context',uname='drawToBeDeleted')  & offy+=100;
   contextBase = WIDGET_BASE(yoff=offy,draw3, /CONTEXT_MENU,/col,UNAME = 'drawContext') & offy+=10;
   b1 = WIDGET_BUTTON(yoff=offy,contextBase, VALUE = 'Delete this draw widget', /SEPARATOR, EVENT_PRO = 'DeleteDraw') 
   b2 = WIDGET_BUTTON(contextBase, VALUE = 'just an entry') & offy+=10;
   b3 = WIDGET_BUTTON(contextBase, VALUE = 'a menu', /menu) & offy+=10;
   b4 = WIDGET_BUTTON(b3         , VALUE = 'an item.') & offy+=10;
   b5 = WIDGET_BUTTON(b3         , VALUE = 'Delete this draw widget (again)', /SEPARATOR, EVENT_PRO = 'DeleteDraw')

   tmp=widget_label(yoff=offy,draw_base,value="below, is centered, has tooltip and popup") & offy+=10;
   draw4 = WIDGET_DRAW(yoff=offy,draw_base,/BUTTON_EVENTS, xsize=100,ysize=100,tooltip="Centered WIGDET_DRAW",/align_center,EVENT_PRO = 'draw_context',uname='drawToBeDeleted2')  & offy+=10;
end else begin
   dummy=widget_label(yoff=offy,draw_base, value="WIDGET_DRAW not shown, (/NOCANVAS option)", font=fontname)
end

; SLIDER_BASE: 
slider_baseh = widget_base( tabbed_base, TITLE="H_SLIDER",_extra=extra) & offy=0;
label=widget_label(yoff=offy,slider_baseh,value='0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789') & offy+=10;
;SLIDERs
tmp=widget_label(xoff=150,yoff=offy,slider_baseh,value="fancy, framed, horizontal, align:inherit") & offy+=10;
sl1=widget_slider(xoff=150,yoff=offy,slider_baseh,MIN=0,MAX=100,TITLE="FRAME=100+DRAG+HORIZONTAL+FANCY",kill_notify='slider_killed',/DRAG,FRAME=100,font=fontname) & offy+=200;
tmp=widget_label(yoff=offy,slider_baseh,value="fancy, simple, horizontal, align:inherit") & offy+=10;
sl3=widget_slider(yoff=offy,slider_baseh,MIN=0,MAX=1000,TITLE="SIMPLE,HORIZONTAL",font=fontname) & offy+=100;
tmp=widget_label(yoff=offy,slider_baseh,value="no label, horizontal, align:inherit") & offy+=10;
sl3=widget_slider(yoff=offy,slider_baseh,MIN=0,MAX=1000) & offy+=30;
tmp=widget_label(yoff=offy,slider_baseh,value="except the following no label slider, centered",/align_center) & offy+=10;
sl3=widget_slider(yoff=offy,slider_baseh,MIN=0,MAX=1000,/align_center) & offy+=10;


slider_basev = widget_base( tabbed_base, TITLE="V_SLIDER",_extra=extra) & offy=0
label=widget_label(yoff=offy,slider_basev,value='0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789') & offy+=10;
;SLIDERs
sl=widget_slider(yoff=offy,slider_basev,MIN=0,MAX=100,TITLE="FRAME=100+VERTICAL",kill_notify='slider_killed',ysize=300,xsize=200,FRAME=20,/VERT,font=fontname) & offy+=400;
sl3=widget_slider(yoff=offy,slider_basev,MIN=0,MAX=1000,TITLE="SIMPLE,VERTICAL",font=fontname,/vert) & offy+=200;
tmp=widget_label(yoff=offy,slider_basev,value="except the following no label slider, centered",/align_center) & offy+=10;
sl3=widget_slider(yoff=offy,slider_basev,MIN=0,MAX=1000,/vert,/align_center) & offy+=10;


; BUTTON_BASE: 
button_base = widget_base( tabbed_base, TITLE="BUTTONs",_extra=extra) & offy=10
label=widget_label(yoff=offy,button_base,value='0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789') & offy+=10;
; BUTTONs
tmp=widget_label(yoff=offy,button_base,value="Exclusive base, framed 30") & offy+=10;
radio=widget_base(yoff=offy,button_base,/EXCLUSIVE,COL=1,frame=30) & offy+=150;
rb1=widget_button(radio,VALUE="button in EXCLUSIVE base 1",uvalue={vEv,'rb1',[8,0]}, font=fontname)
rb2=widget_button(radio,VALUE="button in EXCLUSIVE base 2",uvalue={vEv,'rb2',[9,0]})

tmp=widget_label(yoff=offy,button_base,value="Non-Exclusive base,simple look") & offy+=10;

check=widget_base(yoff=offy,button_base,/NONEXCLUSIVE,COL=1) & offy+=100;
cb1=widget_button(check,VALUE="button in NONEXCLUSIVE base 1",uvalue={vEv,'cb1',[81,0]}, font=fontname)
cb2=widget_button(check,VALUE="button in NONEXCLUSIVE base 2",uvalue={vEv,'cb2',[12,0]})

tmp=widget_label(yoff=offy,button_base,value='2 CW_BGROUP /COL in a framed base') & offy+=10;

bg=widget_base(yoff=offy,button_base,/ROW,Frame=10) & offy+=300;
values = ['One', 'Two', 'Three', 'Four', 'Five','Six'] 

bgroup1 = CW_BGROUP(yoff=offy,bg, values, /COLUMN, /EXCLUSIVE, $ 
  LABEL_TOP='Exclusive', FRAME=10,SET_VALUE=3) 
bgroup2 = CW_BGROUP(yoff=offy,bg, values, /COLUMN, /NONEXCLUSIVE, $ 
  LABEL_TOP='Nonexclusive', FRAME=10, SET_VALUE=[1,0,1,0,1])

tmp=widget_label(yoff=offy,button_base,value="A menu button") & offy+=10;
menu=widget_button(yoff=offy,button_base,value="Menu",/menu) & offy+=100;
entry1=widget_button(menu,value="entry 1")
entry2=widget_button(menu,value="entry 2")

tmp=widget_label(yoff=offy,button_base,value="ALIGNMENTS",/align_center,/fram) & offy+=10;
base1=widget_base(yoff=offy,button_base,/COL,/fram,xsize=400) & offy+=10;
tmp=widget_label(yoff=offy,base1,value="Row Base 1")
base11=widget_base(yoff=offy,base1,/ROW,/fram)
tmp=widget_label(base11,value="inherited")
but=widget_button(base11,value="some text, no align",xsize=200)
tmp=widget_label(yoff=offy,base1,value="Row Base 2") & offy+=10;
base12=widget_base(yoff=offy,base1,/ROW,/fram) & offy+=10;
tmp=widget_label(base12,value="/align_left",/align_left) & offy+=10;
but=widget_button(base12,value="some text, left-aligned",xsize=200,/align_left) & offy+=10;
tmp=widget_label(yoff=offy,base1,value="Row Base 3")
base13=widget_base(yoff=offy,base1,/ROW,/fram)
tmp=widget_label(base13,value="/align_right") & offy+=10;
but=widget_button(base13,value="some text, right-aligned",xsize=200,/align_right) & offy+=10;
base2=widget_base(yoff=offy,button_base,/COL,/fram) & offy+=10;
tmp=widget_label(base2,value="Column Base") & offy+=10;
tmp=widget_label(base2,value="A Button inheriting base orientation: text centered") & offy+=10;
but=widget_button(base2,value="some text, no align",xsize=200) & offy+=10;
tmp=widget_label(base2,value="A left-aligned button",/align_left) & offy+=10;
but=widget_button(base2,value="some text, left-aligned",xsize=300,/align_left) & offy+=10;
tmp=widget_label(base2,value="A right-aligned button") & offy+=10;
but=widget_button(base2,value="some text, right-aligned",xsize=200,/align_right) & offy+=10;



; TABLE
table_base = widget_base( tabbed_base, TITLE="TABLEs",_extra=extra) & offy=0
label=widget_label(yoff=offy,table_base,value='0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789') & offy+=10;
;
mytable1=widget_table(yoff=offy,table_base,value=dist(7),xsize=5,ysize=5,/editable,/all_events,font=fontname,frame=30) & offy+=200;
;to be implemented! ;widget_control,mytable1,/editable,use_table_sel=[1,1,4,4]
widget_control,mytable1,edit_cell=[0,0]
widget_control,mytable1,background_color=[255,255,0],use_table_sel=[1,1,4,4]
mytable2=widget_table(yoff=offy,table_base,value=table[0:5],/row_major,row_labels='',column_labels=tags,column_width=100,/resizeable_columns,y_scroll_size=40,/disjoint,/all_events,alignment=2,frame=50) & offy+=10;


; TREE
tree_base = widget_base( tabbed_base, TITLE="TREE",_extra=extra) & offy=0
label=widget_label(yoff=offy,tree_base,value='0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789') & offy+=10;
if ~keyword_set(notree) then begin
   racine = widget_tree(yoff=offy,tree_base,ysize=100) & offy+=90;
   wtRoot = widget_tree(racine, VALUE='GDL', /folder, /draggable, /drop_events,ysize=200)
   feuille_11 = WIDGET_TREE(wtRoot, VALUE='is', $
                            UVALUE='LEAF')
   branche_12 = WIDGET_TREE(wtRoot, VALUE='...', $
                            /FOLDER, /EXPANDED)
   feuille_121 = WIDGET_TREE(branche_12, VALUE='with', $
                             UVALUE='LEAF')
   feuille_122 = WIDGET_TREE(branche_12, VALUE='a lot of', $
                             UVALUE='LEAF')
   feuille_13 = WIDGET_TREE(wtRoot, VALUE='widgets', $
                            UVALUE='LEAF')
   feuille_14 = WIDGET_TREE(wtRoot, VALUE='now', $
                            UVALUE='LEAF')
   racine = widget_tree(yoff=offy,tree_base,frame=50) & offy+=10;
   wtRoot = widget_tree(racine, VALUE='GDL', /folder, /draggable, /drop_events)
   feuille_11 = WIDGET_TREE(wtRoot, VALUE='is', $
                            UVALUE='LEAF')
   branche_12 = WIDGET_TREE(wtRoot, VALUE='...', $
                            /FOLDER, /EXPANDED)
   feuille_121 = WIDGET_TREE(branche_12, VALUE='with', $
                             UVALUE='LEAF')
   feuille_122 = WIDGET_TREE(branche_12, VALUE='a lot of', $
                             UVALUE='LEAF')
   feuille_13 = WIDGET_TREE(wtRoot, VALUE='widgets', $
                            UVALUE='LEAF')
   feuille_14 = WIDGET_TREE(wtRoot, VALUE='now', $
                            UVALUE='LEAF')
endif





; LABEL
label_base = widget_base( tabbed_base, TITLE="LABELs",_extra=extra) & offy=0
label=widget_label(yoff=offy,label_base,value='0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789') & offy+=10;
;
print,"LABEL"
label1=widget_label(yoff=offy,label_base,VALUE="Sunken-frame label",uvalue={vEv,'lll',[1,-1]},/sunken_frame,frame=33) & offy+=10;
label2=widget_label(yoff=offy,label_base,VALUE="Framed Fancy label",uvalue={vEv,'lll',[1,-1]},font=fontname,frame=33) & offy+=100;
label2=widget_label(yoff=offy,xsize=100,label_base,VALUE="xSIZE=100 of a long FANCY text",uvalue={vEv,'lll',[1,-1]},font=fontname,/frame) & offy+=100;
label3=widget_label(yoff=offy,label_base,VALUE="simple label",uvalue={vEv,'lll',[1,-1]}) & offy+=10;
label3=widget_label(yoff=offy,label_base,VALUE="simple label, centered",/align_center,uvalue={vEv,'lll',[1,-1]}) & offy+=10;
label3=widget_label(yoff=offy,xsize=500,label_base,VALUE="simple label, centered, xsize=500",/align_center,uvalue={vEv,'lll',[1,-1]}) & offy+=10;


; DROPLIST
droplist_base = widget_base( tabbed_base, TITLE="DROPLISTs",_extra=extra) & offy=0
label=widget_label(yoff=offy,droplist_base,value='0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789') & offy+=10;
;
droplist=widget_droplist(yoff=offy,droplist_base,VALUE=["droplist","created","with","WIDGET_DROPLIST","(no YSIZE)"],title="A fancy droplist (framed)",font=fontname,frame=10) & offy+=50;
droplist=widget_droplist(yoff=offy,droplist_base,VALUE=["droplist","created","with","WIDGET_DROPLIST","(no YSIZE)"],title="A fancy droplist",font=fontname) & offy+=50;
droplist=widget_droplist(yoff=offy,droplist_base,VALUE=["A","B","C"],title="A 1 char droplist") & offy+=20;
droplist=widget_droplist(yoff=offy,droplist_base,VALUE=["droplist","no Title","created","with","WIDGET_DROPLIST","(no YSIZE)"]) & offy+=30;
droplist=widget_droplist(yoff=offy,droplist_base,VALUE=["Centered droplist","no Title","created","with","WIDGET_DROPLIST","(no YSIZE)"],/align_center) & offy+=30;

;COMBO
combobox_base = widget_base( tabbed_base, TITLE="COMBOBOXs",_extra=extra) & offy=0
label=widget_label(yoff=offy,combobox_base,value='0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789') & offy+=10;
;
combo=widget_combobox(xoff=100,yoff=offy,combobox_base,VALUE=["A fancy combobox (framed)","not editable","created","with","WIDGET_COMBOBOX"],frame=30,font=fontname) & offy+=100;
combo=widget_combobox(yoff=offy,combobox_base,VALUE=["A combobox","editable","created","with","WIDGET_COMBOBOX"],/editable) & offy+=30;
combo=widget_combobox(yoff=offy,combobox_base,VALUE=["A ysize=100 combobox","editable","created","with","WIDGET_COMBOBOX"],YSIZE=100) & offy+=120;
combo=widget_combobox(yoff=offy,combobox_base,VALUE=["Same, centered combobox","editable","created","with","WIDGET_COMBOBOX"],YSIZE=100,/align_center) & offy+=10;


 ;MISC. BASES
 bases_base0 = widget_base( tabbed_base, TITLE="BASEs",event_pro='base_event_ok',_extra=extra) & offy=0
 label=widget_label(yoff=offy,bases_base0,value='0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789') & offy+=10;
 tmp=widget_label(yoff=offy,bases_base0,value="below a tab-based base with frame=30") & offy+=10;
 tmp=widget_label(yoff=offy,bases_base0,value="containing a base, frame=100, with 2 buttons; ") & offy+=10;
 tmp=widget_label(yoff=offy,bases_base0,value="another with a label and a text; ") & offy+=10;
 tmp=widget_label(yoff=offy,bases_base0,value="another a /scroll ,frame=100, space=50") & offy+=10;
 bases_base = widget_base(yoff=offy, bases_base0,/COL,frame=30) & offy+=10;
 charpix = 8.5
 butSize = 7*charpix
 
 actionBase=widget_base(bases_base,/ROW,frame=100) & offy+=10;
 
 quitButton = widget_button(actionBase,value='WILL QUIT',uvalue={vEv,'quit',[5,0]}, font=fontnam)
 downButton = widget_button(actionBase,value='A 300 px Button..', /dynamic_resize)
 selectBase=widget_base(bases_base,/ROW,frame=1)
 ;a label that can be updated by pushing buttons
 labeltoupdate = widget_label(selectBase, value='Label xsize=100(pixels),ysize=30(pixels)',$
                              xsize=100,ysize=30)
 ;show last button clicked value
 statusLabel = widget_text(selectBase,value=['012345678901234567890123456789012','text, xsize=32(characters),ysize=10(chars)','3','4','5','6','7','8','9','10'], $
                           xsize=32,ysize=10)
                                 ;pass label to top (avoid using a common)
 widget_control,base,set_uvalue=[statusLabel,labeltoupdate]
 
 ;names of columns
 tags = tag_names(table)
 ; nb of colums=nb tags
 ncols=n_tags(table)
 ;set max size (chars) of displayed button/label for each column
 maxSize = intarr(nCols)
 for icol = 0, nCols-1 do maxSize[icol] = 1 + MAX(strlen(strtrim(table.(icol),2))) 
 nrows=n_elements(table)         ; check...
 ;buttons containing header keyword values
 buttons = lonarr(nCols,nRows)
 ;buttons containing column labels
 tagButtons=lonarr(nCols)
 fileButtons = lonarr(nRows)
 fileNumbers = lonarr(nRows)
 ;base for listing contents of tables and to show selected files
 ;   fileBase=widget_base(bases_base,/scroll,frame=3)
 ;base to contain column labels and cells with table values and selected files
 listentries=widget_base(bases_base,row=nRows+1,/scroll,space=50);x_scroll=300,frame=10,y_scroll=300)
 ;create column labels 
 dummy = widget_label(listentries,value=' ', xsize=30)
 dummy1 = widget_label(listentries,value=' ',xsize=30)
 for icol =0, nCols-1 do tagButtons[icol]=widget_label(listentries,value=tags[icol], xsize=charpix*maxSize[icol],/align_left)
 for iRow=0,nRows-1 do begin
    fileNumbers[iRow]=widget_label(listentries, value=string(iRow), xsize=30, ysize=25)
    fileButtons[iRow]=widget_button(listentries, value="*", uvalue={vEv,'file',[iRow,-1]}, xsize=30, ysize=25)
    for icol=0,nCols-1 do buttons[icol, iRow] = widget_button(listentries, value=strtrim((table[iRow].(icol)),2), uvalue={vEv,'data',[icol,iRow]}, ysize=25,  xsize=charpix*maxSize[iCol],/align_left,kill_notify='button_killed')
 endfor
 ;;overwrite label:
 ;print,"label to update id="+string(labeltoupdate) 
 widget_control, labeltoupdate, set_value="Text2 has been replaced with widget_control"
 
 ; overwrite buttons;
    for iRow=0,nRows-1 do widget_control,fileButtons[iRow], set_value="Y"
 ;;
 
 



;tests for /notify_realize and /kill_notify
;
widget_control,base,notify_realize='i_am_realized'
;Realize the widgets. 
WIDGET_CONTROL, /REALIZE, base 
 
;Obtain the window index. 
if ~keyword_set(nocanvas) then begin
print,"Draw widgets:",draw,draw2
 WIDGET_CONTROL, draw, GET_VALUE = index 
  WIDGET_CONTROL, draw2, GET_VALUE = index2 

    ;;
    ;; Set the new widget to be the current graphics window 
 print,"window indexes",index,index2
    file='~/gdl/testsuite/Saturn.jpg'
 image=read_image(file)
 WSET,index
 plot,findgen(100)
 tv,image,10,10,/data,/true

    ;;
 WSET, index2 
 f=findgen(1000)/100.
 contour,cos(dist(100,100)/10.)

end
xmanager,"handle",base,cleanup="cleanup_xmanager",no_block=~block
end
