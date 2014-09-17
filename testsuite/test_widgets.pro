; this file is part of GDL
; using the widgets prints the generated events
; 30. Oct 2013 Marc Schellens
; expanded by GD. Thanks to MIDI's GuiSelect procedure for ideas.
pro exit_gui,ev
  widget_control,ev.top,/DESTROY
end

pro handle_Event,ev
  widget_control,ev.id,get_uvalue=uval 
  widget_control,ev.top,get_uvalue=topuv
  if n_elements(uval) gt 0 then begin
     case uval.type of
        'file': begin
           widget_control,ev.id,get_value=value
           case value of
              "Y": widget_control,ev.id,set_value="N"
              "N": widget_control,ev.id,set_value="Y"
              else: return
           endcase
        end
        'data': begin
           widget_control,ev.id,get_value=value
           widget_control,topuv,set_value=value
           end
        'quit':  widget_control,ev.top,/DESTROY
        else: return
     endcase
  endif
end

pro test_widgets,table,help=help
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
base = WIDGET_BASE(/COL,MBAR=mbar) 
 
menu = widget_button(mbar,VALUE="Menu")
ex = widget_button(menu,VALUE="Exit",EVENT_PRO="exit_gui")
siz= widget_button(menu,VALUE="Resize",EVENT_PRO="resize_gui")

tab = widget_tab( base)
; note inversion tab5<->tab3
tab1 = widget_base( tab, TITLE="GDL",/COL)
tab2 = widget_base( tab, TITLE="with",/COL)
tab5 = widget_base( tab, TITLE="scrolling",/COL)
tab4 = widget_base( tab, TITLE="widgets",/COL)
tab3 = widget_base( tab, TITLE="now!",/COL)

;Attach a 256 x 256 draw widget. 
draw = WIDGET_DRAW(tab1, XSIZE = 256, YSIZE = 256) 
 
radio=widget_base(tab2,/EXCLUSIVE,COL=1)
rb1=widget_button(radio,VALUE="button in EXCLUSIVE base 1",uvalue={vEv,'rb1',[8,0]})
rb2=widget_button(radio,VALUE="button in EXCLUSIVE base 2",uvalue={vEv,'rb2',[9,0]})

check=widget_base(tab2,/NONEXCLUSIVE,COL=1)
cb1=widget_button(check,VALUE="button in NONEXCLUSIVE base 1",uvalue={vEv,'cb1',[81,0]})
cb2=widget_button(check,VALUE="button in NONEXCLUSIVE base 2",uvalue={vEv,'cb2',[12,0]})

sl=widget_slider(tab2,MIN=0,MAX=100,/DRAG,TITLE="WIDGET_SLIDER")

label1=widget_label(tab3,VALUE="To be replaced",uvalue={vEv,'lll',[1,-1]})
label2=widget_label(tab3,VALUE="Idem...",uvalue={vEv,'lll',[1,-1]})
text1=widget_text(tab3,VALUE=["This is an editable text widget",$
                        "created with","WIDGET_TEXT"],$
              /EDITABLE,/ALL_EVENTS,YSIZE=5)
text2=widget_text(tab3,VALUE=["Not editable text",$
                        "created with","WIDGET_TEXT"],$
              EDITABLE=0,/ALL_EVENTS,YSIZE=5)

droplist=widget_droplist(tab3,VALUE=["A","droplist","created","with","WIDGET_DROPLIST"])

bg=widget_base(tab4,/ROW)
values = ['One', 'Two', 'Three', 'Four', 'Five','Six'] 

bgroup1 = CW_BGROUP(bg, values, /COLUMN, /EXCLUSIVE, $ 
  LABEL_TOP='Exclusive', /FRAME, SET_VALUE=3) 
bgroup2 = CW_BGROUP(bg, values, /COLUMN, /NONEXCLUSIVE, $ 
  LABEL_TOP='Nonexclusive', /FRAME, SET_VALUE=[1,0,1,0,1]) 

; panel 5
actionBase=widget_base(tab5,/ROW)
   quitButton = widget_button(actionBase,value='QUIT', $
      xsize=butSize,uvalue={vEv,'quit',[5,0]})
;buttons to scroll files within widget show
   upButton = widget_button(actionBase,value='UP', $
      xsize=butSize,uvalue={vEv,'up',[6,0]})
   downButton = widget_button(actionBase,value='DOWN', $
      xsize=butSize,uvalue={vEv,'down',[7,0]})
selectBase=widget_base(tab5,/ROW)
;total number of rows and number of selected rows
   totalrows = widget_label(selectBase, value='------',$
      xsize=80,ysize=25,/align_left)
   selectedrows = widget_label(selectBase, value='------',$
      xsize=80,ysize=25,/align_left)
;show last button clicked value
   statusLabel = widget_text(selectBase,value='', $
      xsize=30)
   ;pass label to top (avoid using a common)
   widget_control,base,set_uvalue=statusLabel
;base for listing contents of tables and to show selected files
;(below)
nrows=n_elements(table)
if ( n_elements(table) eq 0 or size(table,/type) ne 9 ) then begin
tbltemplate={table2,string:'a very very very long string ',real:0.0,double:0.0D,another:'another very very very long string',yanother:'yet another very very very long string'} 
; make a long table of nrows
nrows=53
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

   charpix = 8.5
   butSize = 7*charpix
;base for listing contents of tables and to show selected files
;   fileBase=widget_base(tab5,/row, y_scroll_size=600,x_scroll_size=1000,/scroll)
;base to contain column labels and cells with table values and selected files
   listentries=widget_base(tab5,row=nRows+1,space=0, y_scroll_size=600,x_scroll_size=1000,/scroll)
;create column labels 
   dummy = widget_label(listentries,value='', xsize=30)
   dummy1 = widget_label(listentries,value='',xsize=30)
   for icol =0, nCols-1 do tagButtons(icol)=widget_label(listentries,value=tags(icol), xsize=charpix*maxSize[icol],/align_left)
   for iRow=0,nRows-1 do begin
         fileNumbers(iRow)=widget_label(listentries, value=string(iRow), xsize=30, ysize=25)
         fileButtons(iRow)=widget_button(listentries, value="*", uvalue={vEv,'file',[iRow,-1]}, xsize=30, ysize=25)
      for icol=0,nCols-1 do buttons(icol, iRow) = widget_button(listentries, value=strtrim((table[iRow].(icol)),2), uvalue={vEv,'data',[icol,iRow]}, ysize=25,  xsize=charpix*maxSize[iCol],/align_left)
   endfor
; 
;;overwrite label: 
;  widget_control, label2, set_value="Text2 has been replaced with widget_control"
;  widget_control, label1, set_value="Text1 has been replaced with widget_control"
;; overwrite text:
;  widget_control, text1, set_value="Text has been replaced with widget_control"
;  widget_control, text2, set_value="Text has been replaced with widget_control"
; overwrite buttons;
   for iRow=0,nRows-1 do widget_control,fileButtons(iRow) , set_value="Y"
;;
;Realize the widgets. 
WIDGET_CONTROL, /REALIZE, base 
 
;Obtain the window index. 
WIDGET_CONTROL, draw, GET_VALUE = index 
 
;Set the new widget to be the current graphics window 
print,"window index",index
WSET, index 

f=findgen(1000)/100.
contour,cos(dist(100,100)/10.)

xmanager,/NO_BLOCK,"handle",base
end
