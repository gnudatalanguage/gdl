; this file is part of GDL
; using the widgets prints the generated events
; 30. Oct 2013 Marc Schellens
pro exit_gui,ev
  widget_control,ev.top,/DESTROY
end

pro handle_Event,ev
help,ev,/STRUCT
; the following crash GDL!
;  widget_control,ev.top,get_uvalue=topuval & print,topuval
;  widget_control,ev.id,get_uvalue=iduval & print,iduval
end

pro test_widgets
ev = {vEv,type:'',pos:[0,0]}
;Create a base widget. 
base = WIDGET_BASE(/COL,MBAR=mbar) 
 
menu = widget_button(mbar,VALUE="Menu")
ex = widget_button(menu,VALUE="Exit",EVENT_PRO="exit_gui")

tab = widget_tab( base)

t1 = widget_base( tab, TITLE="GDL",/COL)
t2 = widget_base( tab, TITLE="with",/COL)
t3 = widget_base( tab, TITLE="widgets",/COL)
t4 = widget_base( tab, TITLE="now",/COL)

;Attach a 256 x 256 draw widget. 
draw = WIDGET_DRAW(t1, XSIZE = 256, YSIZE = 256) 
 
radio=widget_base(t2,/EXCLUSIVE,COL=1)
rb1=widget_button(radio,VALUE="button in EXCLUSIVE base 1",uvalue={vEv,'rb1',[8,0]})
rb2=widget_button(radio,VALUE="button in EXCLUSIVE base 2",uvalue={vEv,'rb2',[9,0]})

check=widget_base(t2,/NONEXCLUSIVE,COL=1)
cb1=widget_button(check,VALUE="button in NONEXCLUSIVE base 1",uvalue={vEv,'cb1',[81,0]})
cb2=widget_button(check,VALUE="button in NONEXCLUSIVE base 2",uvalue={vEv,'cb2',[12,0]})

sl=widget_slider(t2,MIN=0,MAX=100,/DRAG,TITLE="WIDGET_SLIDER")

l=widget_label(t3,VALUE="A label created with WIDGET_LABEL",uvalue={vEv,'lll',[1,-1]})
t=widget_text(t3,VALUE=["This is a text widget",$
                        "created with","WIDGET_TEXT"],$
              /EDITABLE,/ALL_EVENTS,YSIZE=5)
dl=widget_droplist(t3,VALUE=["A","droplist","created","with","WIDGET_DROPLIST"])

bg=widget_base(t4,/ROW)
values = ['One', 'Two', 'Three', 'Four', 'Five','Six'] 

bgroup1 = CW_BGROUP(bg, values, /COLUMN, /EXCLUSIVE, $ 
  LABEL_TOP='Exclusive', /FRAME, SET_VALUE=3) 
bgroup2 = CW_BGROUP(bg, values, /COLUMN, /NONEXCLUSIVE, $ 
  LABEL_TOP='Nonexclusive', /FRAME, SET_VALUE=[1,0,1,0,1]) 

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
