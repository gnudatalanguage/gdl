; this file is part of GDL
; 30. Oct 2020 Gilles Duvert
; test ? most of all and every widget available and some functionalities.
;
function myBitmap
  return,[ $
        [000B, 000B, 000B, 000B],           $
        [000B, 000B, 000B, 000B],           $
        [000B, 000B, 000B, 000B],           $
        [128B, 255B, 001B, 000B],           $
        [224B, 255B, 007B, 000B],           $
        [240B, 000B, 015B, 000B],           $
        [056B, 000B, 028B, 000B],           $
        [024B, 000B, 024B, 000B],           $
        [028B, 000B, 056B, 000B],           $
        [012B, 000B, 048B, 000B],           $
        [012B, 000B, 048B, 000B],           $
        [012B, 000B, 048B, 000B],           $
        [012B, 000B, 048B, 000B],           $
        [012B, 000B, 240B, 031B],           $
        [012B, 000B, 240B, 031B],           $
        [012B, 000B, 112B, 029B],           $
        [028B, 000B, 184B, 026B],           $
        [024B, 000B, 088B, 029B],           $
        [056B, 000B, 188B, 026B],           $
        [240B, 000B, 095B, 029B],           $
        [224B, 255B, 175B, 026B],           $
        [128B, 255B, 085B, 029B],           $
        [000B, 176B, 170B, 026B],           $
        [000B, 112B, 085B, 029B],           $
        [000B, 176B, 170B, 026B],           $
        [000B, 112B, 085B, 029B],           $
        [000B, 176B, 170B, 026B],           $
        [000B, 240B, 255B, 031B],           $
        [000B, 240B, 255B, 031B],           $
        [000B, 000B, 000B, 000B],           $
        [000B, 000B, 000B, 000B],           $
        [000B, 000B, 000B, 000B]            $
        ]
end
pro myplot,tr,x,y,b,conn,poly,tag=tag, list=list
  if n_elements(poly) eq 0 then poly=0
  if n_elements(conn) gt 0 then doconn=1 else doconn=0
  if n_elements(b) gt 0 then dob=1 else dob=0
; colors
  n=n_elements(tr)/3

  p=randomu(seed,n,/long)

  PLOT, x, y,psym=1,/NODATA,/iso,xst=1,yst=1

  for i=0,n-1 do polyfill,[x[tr[0,i]],x[tr[1,i]],x[tr[2,i]],x[tr[0,i]]],[y[tr[0,i]],y[tr[1,i]],y[tr[2,i]],y[tr[0,i]]],color=p[i]

  if (dob) then begin plots, x[b],y[b],thick=4 & plots, x[b[0]],y[b[0]],thick=4,/cont & end

  if (doconn) then begin
     i=poly                     ;,n_elements(x)-(n_elements(r)/2)-1 do begin
     sub=conn[conn[i] : conn[i+1]-1]
     polyfill,x[sub],y[sub],color=0,/line_fill,spacing=0.2,ori=33,thick=2
  endif

  nn=n_elements(x)
  if (keyword_set(list)) then begin
     for i=0,nn-1 do begin
        print,i
        sub=conn[conn[i] : conn[i+1]-1]
        ns=n_elements(sub)
        for j=0,ns-3 do begin
           print,sub[0],sub[(j+1) mod ns],sub[(j+2) mod ns]
        endfor
     endfor
     
  endif

  if (keyword_set(tag)) then for i=0,n_elements(x)-1 do xyouts,x[i],y[i],i,ali=1,chars=2

end


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
pro base_event_base, id
  print,"event in bases base"
  help,id
end
pro base_event, id
  print,"event in top base"
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
function draw2_event,ev
  print,"draw2 event"
  return,ev
end
function test_func_button,ev
  print, "fancy button pressed! (catched in button widget itself)"
  parent=widget_info(ev.id,/parent)
  ev.handler=parent ; pass to parent
  return, ev
end
function catch_passed_event_example,ev
  print, "button "+strtrim(ev.id,2)+" was pressed! (catched in parent's event_func routine)"
;  parent=widget_info(ev.id,/parent)
;  ev.handler=parent ; pass to parent
  return, ev
end

pro draw1_event,ev
widget_id=ev.id
print,"draw1 started, will exit on key 1 release"
sav_draw_motion_events = widget_info(widget_id, /draw_motion_events)
sav_draw_button_events = widget_info(widget_id, /draw_button_events)
sav_event_pro = widget_info(widget_id,/event_pro)
sav_event_func = widget_info(widget_id,/event_func)

widget_control, widget_id, /draw_button_events
widget_control, widget_id, event_pro=''
widget_control, widget_id, event_func='draw2_event'
while 1 do begin

	ev = widget_event (widget_id)
;	print,'event in stretch_box
;	help,ev,/st

	if ev.press eq 1 then begin
          print,"keypress 1"
	endif

        if ev.release eq 1 then begin
			if sav_event_pro ne '' then widget_control, widget_id, event_pro=sav_event_pro
			if sav_event_func ne '' then widget_control, widget_id, event_func=sav_event_func
			widget_control, widget_id, draw_button_events=sav_draw_button_events, $
                           draw_motion_events=sav_draw_motion_events
                        print,"reset pro fun events & exit draw1"
                        return
	endif
	wait, .1
endwhile
end

pro list_event,event
toto=["A","list","created","with","WIDGET_LIST","YSIZE=3"]
print,toto[event.index] 
end
pro handle_Event,ev
  common mycount,count
  common pixmaps,green_bmp,red_bmp
  help,ev,/str
  if tag_names(ev, /structure_name) eq 'WIDGET_KILL_REQUEST' then begin
     acceptance=dialog_message(dialog_parent=ev.id,"I Do want to close the window", /CANCEL, /DEFAULT_NO,/QUESTION) ; +strtrim(ev.id,2))
     if acceptance eq "Yes" then begin
        widget_control,ev.id,tlb_kill_request_events=0 ; remove blocking kill
        widget_control,ev.id,/destroy
        return
     endif
     
  endif

  if tag_names(ev, /structure_name) eq 'WIDGET_TABLE_CELL_DESEL' then begin
     col=[255b,255,255]
     disj=widget_info(ev.id,/table_disj)
     if disj then begin
        desel=[ev.sel_left,ev.sel_top]
        nel=1
     endif else  begin
        desel=[ev.sel_left,ev.sel_top,ev.sel_right,ev.sel_bottom]
        nel=(ev.sel_right+1-ev.sel_left)*(ev.sel_bottom+1-ev.sel_top)
     endelse
     widget_control,ev.id,use_table_select=desel,background_color=col,set_value=replicate(-1,nel)
     
     return
  endif
  if tag_names(ev, /structure_name) eq 'WIDGET_TABLE_CELL_SEL' then begin
     if ev.sel_left eq -1 then begin
        ;widget_control,ev.id,background_color=[255,255,255]
        return
     endif
     col=byte(randomu(seed,3)*255) 
     widget_control,ev.id,background_color=col,/use_table_select
     z=widget_info(ev.id,/table_select)
     col=byte(randomu(seed,3)*255)
     nel=n_elements(z)
     if nel gt 0 then widget_control,ev.id,foreground_color=col,set_value=indgen(nel),use_table_select=z
     ;; disj=widget_info(ev.id,/table_disj)
     ;; if disj then dozero=[-1,-1] else dozero=[-1,-1,-1,-1]
     ;; widget_control,ev.id,set_table_select=dozero
     return
  endif
 
  widget_control,ev.id,get_uvalue=uv 
  widget_control,ev.top,get_uvalue=topuv
  if n_elements(uv) gt 0 then begin
     if (strlen(tag_names(ev,/structure_name)) le 1) then begin
        print,"unhandled compound widget event"
        return
     endif
     if ~ISA(uv,"STRUCT") then begin
        print,"currently unhandled event"
        return
     endif
     
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
        'test_check': begin
           check=widget_info(ev.id,/button_set)
           if (check) then begin
              print,"unsetting checkmark..."
              widget_control,ev.id,set_button=0
              widget_control,ev.id,set_value="entry 3 (unchecked)"
           endif else begin
              print,"setting checkmark..."
              widget_control,ev.id,set_button=1
              widget_control,ev.id,set_value="entry 3 (checked)"
           endelse
        end
        'clickbutton': begin
           widget_control,ev.id,get_value = val
	   if val eq 'ON' then begin
              widget_control,ev.id,set_value = 'OFF' 
           endif else begin
              widget_control,ev.id,set_value = 'ON'
           endelse
        end
        'clickpixmapbutton': begin
           widget_control,ev.id,get_uvalue = val
           if val.pos[0] eq 0 then begin
              val.pos[0]=1
              widget_control,ev.id,set_value = red_bmp, set_uvalue= val
           endif else begin
              val.pos[0]=0
               widget_control,ev.id,set_value = green_bmp, set_uvalue= val
           endelse
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
pro doMbar,mbar,fontname
  menu = widget_button(mbar,VALUE="Simple Menu",font=fontname,tooltip='TOOOLTIP!') ; 3
ex = widget_button(menu,VALUE="Exit",EVENT_PRO="exit_gui") ; 4
siz= widget_button(menu,VALUE="Resize (error)",EVENT_PRO="resize_gui") ; 5
;buttons as menu buttons
        fileID = Widget_Button(mbar, Value='Complicated Menu') ; 6
        saveID = Widget_Button(fileID, Value='submenu 1', /MENU,font=fontname)
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
        button = Widget_Button(fileID, Value='entry 3 (unchecked)', /Separator, /check ,UVALUE={vEv,'test_check',[0,0]})
        bitmap = Widget_Button(mbar, Value='bitmap menus')
        ;; this one is allowed by IDL only on WINDOWS but possible with GDL
        ;;button = Widget_Button(bitmap, Value='a bitmap', image=myBitmap(), UNAME='BUT',/menu)
        button = Widget_Button(bitmap, Value=myBitmap(), UNAME='BUT',/menu)
        button = Widget_Button(button, Value=myBitmap(), UNAME='BUT')
     end

pro test_widgets,table,help=help,nocanvas=nocanvas,notree=notree,block=block,fontname=fontname,present=present,select=select,_extra=extra
  common mycount,count
  common pixmaps,green_bmp,red_bmp

  green_bmp= bytarr(7,7,3)& green_bmp[*,*,1] = 255& & green_bmp[0,0,1] = 254
  red_bmp= bytarr(7,7,3)& red_bmp[*,*,0] = 255& & red_bmp[0,0,0] = 254
  if (n_elements(select) gt 0) then present=select
  if (n_elements(present) eq 0) then present=['TEXT','LIST','DRAW','SLIDER','BUTTON','TABLE','TREE','LABEL','DROPLIST','COMBOBOX','BASE']
count=0
title='GDL Widget Examples'
DEFSYSV, '!gdl', exists=isGDL
if ~keyword_set(fontname) then begin
   fontname=(isgdl)?"Die Nasty Bold 10":'-altsys-die nasty-bold-r-normal--0-100-94-0-p-0-ascii-0'
   title+=' (Using Fixed Font Monospace)'
endif else title+=' (Using '+fontname+')'
if ~keyword_set(block) then block=0
if keyword_set(help) then begin
print,"useage: test_widgets[,table][,/help][,/nocanvas][,/notree]"
print,"Will display some examples of currently available widgets"
print,"if table is passed as argument and is a structure, TABLE tab will show the"
print,"elements of the structure as buttons in a scrolled panel"
print,"options: /nocanvas removes the widget_draw"
print,"         /notree remove the tree widget"
print,"         fontname=""Helvetica Narrow 32"" to change a test text font."
print,"         present=[XXX,YYY] where XXX and YY are one or more of the panel types below:"
print,"         'TEXT','LIST','DRAW','SLIDER','BUTTON','TABLE','TREE','LABEL','DROPLIST','COMBOBOX','BASE' "
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
 
base = WIDGET_BASE(/col,MBAR=mbar,title=title,event_pro='base_event_nok',kill_notify='cleanup',/tlb_kill_request_events,/tlb_size_events) ; ---> PROBLEM: ,/tlb_size_events) ;,/scroll)
doMbar,mbar,fontname
;mysize=widget_info(base,string_size='012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234')

; define a tabbed base that contains everything:
label=widget_label(base,value='to best mimic IDL`s widgets, call GDL ',/align_left)
label=widget_label(base,value='with option "--widget-compat" ',/align_left)
label=widget_label(base,value='See test_widget,/help for useful hints.',/align_left)

tabbed_base = widget_tab( base, frame=12) ;, scr_xsize=400, scr_ysize=400);, multiline=6)

offy=0
; TEST: empty base to play with. use with option " present='test' "
if total(strcmp('TEST',present,/fold)) then begin
   test_base = widget_base( tabbed_base, TITLE="TEST",xsize=1500,ysize=1500,x_scroll=500,y_scroll=500,_extra=extra) & offy=0
endif

;TEXT
if total(strcmp('TEXT',present,/fold)) then begin
   text_base = widget_base( tabbed_base, TITLE="TEXT",_extra=extra) & offy=0

   label=widget_label(yoff=offy,text_base,/align_center,value='Various Sizes, no event should be issued') & offy+=20                                               ;
   text1=widget_text(yoff=offy,text_base,VALUE=["xsize=30,/scroll, (no ysize), follow base alignment","Line 2","line 3","line 4","line 5"],/scroll, xsize=30) & offy+=30     ;
   text1=widget_text(yoff=offy,text_base,VALUE=["xsize=20, no ysize, follow base alignment","Line 2","line 3","line 4","line 5"],XSIZE=20) & offy+=30              ;
   text1=widget_text(yoff=offy,text_base,VALUE=["ysize=3, fancyFont, follow base alignment","Line 2","line 3","line 4","line 5"],$
                     font=fontname, ysize=3) & offy+=30 ;
   text1=widget_text(yoff=offy,text_base,VALUE=["xsize=20, ysize=3,/scroll, follow base alignment","Line 2","line 3","line 4","line 5"],$
                     xsize=20, ysize=3,/scroll) & offy+=30 ;
   text1=widget_text(yoff=offy,text_base,VALUE=["idem but with scr_xsize=200,scr_ysize=50"," a non editable LONG line using /wrap option..."],$
                     scr_xsize=200,scr_ysize=50,/wrap) & offy+=20                                                                                 ;
   label=widget_label(yoff=offy,text_base,/align_center,value='Various Positioning, no event should be issued') & offy+=20      ;
   text3=widget_text(yoff=offy,text_base,VALUE=["Caractères Accentués ça ü û, frame=10,left"],frame=10,/align_left)  & offy+=50  ;
   text3=widget_text(yoff=offy,text_base,VALUE=["fancy center aligned, frame=10"],/align_center,frame=10,font=fontname) & offy+=50 ;
   text3=widget_text(yoff=offy,text_base,VALUE=["follows alignment, frame=10"],frame=10) & offy+=50                                ;
   
   label=widget_label(yoff=offy,text_base,/align_center,value='EVENTS') & offy+=20                                                              ;
   text1=widget_text(yoff=offy,text_base,VALUE=["a non editable LONG line using /wrap option in a restricted xsize=20,ysize=5 panel (sizes guaranteed only if an alignment has been defined), this is not finished there is still text below...","a non editable LONG line using /wrap option in a restricted xsize=20,ysize=5 panel (sizes guaranteed only if an alignment has been defined), this is not finished there is still text below...","a non editable LONG line using /wrap option in a restricted xsize=20,ysize=5 panel (sizes guaranteed only if an alignment has been defined), this is not finished there is still text below..."],$
                     xsize=20, ysize=5,/wrap,/all_events) & offy+=50 ;
   label=widget_label(yoff=offy,text_base,value='Is EDITABLE + ALL_EVENTS: input change widget content and events are generated') & offy+=20    ;
   text1=widget_text(yoff=offy,text_base,VALUE=["EDITABLE, all_events","Line 2","line 3","line 4","line 5"],/EDITABLE,/ALL_EVENTS,ysize=3) & offy+=30   ;
   label=widget_label(yoff=offy,text_base,value='Is NOT EDITABLE + ALL_EVENTS: no change but events are generated') & offy+=20                  ;
   text1=widget_text(yoff=offy,text_base,VALUE=["NOT EDITABLE, all_events","Line 2","line 3","line 4","line 5"],EDITABLE=0,/ALL_EVENTS) & offy+=30 ;
   label=widget_label(yoff=offy,text_base,value='Is only EDITABLE: input change widget content and <Return> generates an event.') & offy+=20       ;
   text1=widget_text(yoff=offy,text_base,VALUE=["EDITABLE","Line 2","line 3","line 4","line 5"],/EDITABLE,ALL_EVENTS=0) & offy+=30                 ;
endif

 if total(strcmp('LIST',present,/fold)) then begin  
;LIST
    list_base = widget_base( tabbed_base, TITLE="LISTs",_extra=extra) & offy=0                                                                                     ;

    label=widget_label(yoff=offy,list_base,value='3 lines, placement inherited') & offy+=10                                                                         ;
    list=widget_list(yoff=offy,list_base,value=["Simple rather long text of widget_list, ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST", "ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST", "ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST"],/multiple,YSIZE=3) & offy+=100 ;
    label=widget_label(yoff=offy,list_base,value='idem,fancy,framed(32 pix)') & offy+=10 ;
    list=widget_list(yoff=offy,list_base,value=["FANCY FONT long text widget_list, ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST", "ysize=3","created","with","WIDGET_LIST","widget_list, ysize=3","created","with","WIDGET_LIST"],/multiple,YSIZE=3,frame=32,font=fontname) & offy+=200 ;
    label=widget_label(yoff=offy,list_base,value='3 lines, placement inherited, xsize=12 chars (scroll)') & offy+=10                       ;
    list=widget_list(yoff=offy,list_base,value=["widget_list, ysize=3","created","with","WIDGET_LIST"],/multiple,YSIZE=3,xsize=12) & offy+=150 ;
    label=widget_label(yoff=offy,list_base,value='1 line, placement inherited, xsize=2 chars (scroll)') & offy+=10                             ;
    list=widget_list(yoff=offy,list_base,value=["widget_list, ysize=3","created","with","WIDGET_LIST"],/multiple,xsize=2) & offy+=150          ;
    label=widget_label(yoff=offy,list_base,value='no size,fancy,framed(32 pix)') & offy+=10                                                    ;
    list=widget_list(yoff=offy,list_base,value=["widget_list","(no YSIZE, frame=32)"],frame=32,font=fontname) & offy+=50                       ;
    label=widget_label(yoff=offy,list_base,value='centered,frame=1',/align_center) & offy+=10                                                  ;
    list=widget_list(yoff=offy,list_base,value=["(centered)"," frame=1"],/align_center,frame=1) & offy+=50                                     ;
    label=widget_label(yoff=offy,list_base,value='basic') & offy+=10                                                                           ;
    list=widget_list(yoff=offy,list_base,value=["default"," basic"]) & offy+=50                                                                ;
    label=widget_label(yoff=offy,list_base,value='centered,basic',/align_center) & offy+=10                                                    ;
    list=widget_list(yoff=offy,list_base,value=["(centered)","basic"],/align_center) & offy+=10                                                ;
 endif
 
  if total(strcmp('DRAW',present,/fold)) then begin     
;DRAW_BASE: Attach 256 x 256 draw widgets. Offsets 256 pix
    draw_base = widget_base( tabbed_base, TITLE="DRAW",_extra=extra) & offy=0 ;,YSIZE=1000,XSIZE=1000,Y_SCROLL=300);XPAD=25,YPAD=50,SPACE=30)

; populate
    if ~keyword_set(nocanvas) then begin 
       draw = WIDGET_DRAW(yoff=offy,draw_base, XSIZE = 1800, YSIZE = 600,x_scroll_size=256,y_scroll_size=256,frame=20,/button_events,event_pro="draw1_event")  & offy+=300                                         ;
       draw2 = WIDGET_DRAW(yoff=offy,draw_base, xoff=100, xsize=400,ysize=400,x_scroll_size=200,y_scroll_size=200,/button_events,keyboard_events=1,/motion_events)  & offy+=250 ;
       tmp=widget_label(xoff=200,yoff=offy,draw_base,value="below, has tooltip and has a popup") & offy+=10                                                                     ;
       
       draw3 = WIDGET_DRAW(xoff=200,yoff=offy,draw_base,/BUTTON_EVENTS, xsize=100,ysize=100,tooltip="WIGDET_DRAW",EVENT_PRO = 'draw_context',uname='drawToBeDeleted')  & offy+=100 ;
       contextBase = WIDGET_BASE(yoff=offy,draw3, /CONTEXT_MENU,col=2,TITLE="ZZZZZZZZZZZZZZ",UNAME = 'drawContext') & offy+=10;
       b1 = WIDGET_BUTTON(yoff=offy,contextBase, VALUE = 'Delete this draw widget', /SEPARATOR, EVENT_PRO = 'DeleteDraw') 
       b2 = WIDGET_BUTTON(contextBase, VALUE = 'just an entry') & offy+=10;
       b2 = WIDGET_BUTTON(contextBase, VALUE = 'just an entry, checked', /check) & offy+=10 ;
       widget_control,b2,/set_button
       b2 = WIDGET_BUTTON(contextBase, VALUE = 'just an entry') & offy+=10;
       b3 = WIDGET_BUTTON(contextBase, VALUE = 'a menu', /menu) & offy+=10;
       b4 = WIDGET_BUTTON(b3         , VALUE = 'an item.') & offy+=10;
       b5 = WIDGET_BUTTON(b3         , VALUE = 'Delete this draw widget (again)', /SEPARATOR, EVENT_PRO = 'DeleteDraw')
       
       tmp=widget_label(yoff=offy,draw_base,value="below, is centered, has tooltip and popup") & offy+=10                                                                                    ;
       draw4 = WIDGET_DRAW(yoff=offy,draw_base,/BUTTON_EVENTS, xsize=100,ysize=100,tooltip="Centered WIGDET_DRAW",/align_center,EVENT_PRO = 'draw_context',uname='drawToBeDeleted2')  & offy+=10 ;
    end else begin
       dummy=widget_label(yoff=offy,draw_base, value="WIDGET_DRAW not shown, (/NOCANVAS option)", font=fontname)
    endelse
 endif
 if total(strcmp('SLIDER',present,/fold)) then begin
; SLIDER_BASE: 
    slider_baseh = widget_base( tabbed_base, TITLE="H_SLIDER",_extra=extra) & offy=0                                                                                           ;

;SLIDERs
    tmp=widget_label(xoff=150,yoff=offy,slider_baseh,value="fancy, framed, horizontal, align:inherit") & offy+=10                                                              ;
    sl1=widget_slider(xoff=150,yoff=offy,slider_baseh,MIN=0,MAX=100,TITLE="FRAME=100+DRAG+HORIZONTAL+FANCY",kill_notify='slider_killed',/DRAG,FRAME=100,font=fontname) & offy+=200 ;
    tmp=widget_label(yoff=offy,slider_baseh,value="fancy, simple, horizontal, align:inherit") & offy+=10                                                                           ;
    sl3=widget_slider(yoff=offy,slider_baseh,MIN=0,MAX=1000,TITLE="SIMPLE,HORIZONTAL",font=fontname) & offy+=100                                                                   ;
    tmp=widget_label(yoff=offy,slider_baseh,value="no label, horizontal, align:inherit") & offy+=10                                                                             ;
    sl3=widget_slider(yoff=offy,slider_baseh,MIN=0,MAX=1000) & offy+=30                                                                                                            ;
    tmp=widget_label(yoff=offy,slider_baseh,value="except the following no label slider, centered",/align_center) & offy+=10                                                       ;
    sl3=widget_slider(yoff=offy,slider_baseh,MIN=0,MAX=1000,/align_center) & offy+=10                                                                                              ;
    
    
    slider_basev = widget_base( tabbed_base, TITLE="V_SLIDER",_extra=extra) & offy=0

;SLIDERs
    tmp=widget_label(yoff=offy,slider_basev,value="Vertical slider, xsize=200, ysize=300, frame=20, fancy, kill_notify") & offy+=10                                                       ;
    sl=widget_slider(yoff=offy,slider_basev,MIN=0,MAX=100,TITLE="A Quite Long Title.",kill_notify='slider_killed',ysize=300,xsize=200,FRAME=20,/VERT,font=fontname) & offy+=400 ;
    tmp=widget_label(yoff=offy,slider_basev,value="Vertical slider, fancy") & offy+=10                                                       ;
    sl3=widget_slider(yoff=offy,slider_basev,MIN=0,MAX=1000,TITLE="A title",font=fontname,/vert) & offy+=200                                                           ;
    tmp=widget_label(yoff=offy,slider_basev,value="following no label slider will be always centered",/align_center) & offy+=10                                                   ;
    sl3=widget_slider(yoff=offy,slider_basev,MIN=0,MAX=1000,/vert,/align_center) & offy+=10                                                                                    ;
    
 endif
 if total(strcmp('BUTTON',present,/fold)) then begin
; BUTTON_BASE: 
    button_base00 = widget_base( tabbed_base, TITLE="BUTTONs", COL=2, $
       SPACE=10, XPAD=10, YPAD=10) & offy=10

    button_base01 = widget_base(button_base00, TITLE="BUTTONs",/COL, event_func='catch_passed_event_example') & offy=10
    button_base02 = widget_base(button_base00, TITLE="BUTTONs",/COL) & offy=10
; BUTTONs
    tmp=widget_label(yoff=offy,button_base01,value="Simple ON/OFF Button") & offy+=10           ;
    tmp=widget_button(yoff=offy,button_base01,value="OFF",uvalue={vEv,'clickbutton',[0,0]}) & offy+=10                                                                    ;
    tmp=widget_label(yoff=offy,button_base01,value="Simple Color Button") & offy+=10           ;
    tmp=widget_button(yoff=offy,button_base01,value=green_bmp,uvalue={vEv,'clickpixmapbutton',[0,0]}) & offy+=10                                                                    ;
    tmp=widget_label(yoff=offy,button_base01,value="Framed Simple Button") & offy+=10           ;
    tmp=widget_button(yoff=offy,button_base01,value="Framed 10 px Simple Button",frame=10) & offy+=10 ;
    tmp=widget_label(yoff=offy,button_base01,value="Bitmap Simple Button") & offy+=10           ;
    tmp=widget_button(yoff=offy,button_base01,value=myBitmap()) & offy+=10 ;
    tmp=widget_label(yoff=offy,button_base01,value="Fancy Simple Button") & offy+=10           ;
    tmp=widget_button(yoff=offy,button_base01,value="Fancy Button",font=fontname, event_func='test_func_button') & offy+=10 ;
    tmp=widget_label(yoff=offy,button_base01,value="Exclusive base, framed 30") & offy+=10  ;
    radio=widget_base(yoff=offy,button_base01,/EXCLUSIVE,COL=1,frame=30) & offy+=150         ;
    rb1=widget_button(radio,VALUE="button in EXCLUSIVE base 1",uvalue={vEv,'rb1',[8,0]}, font=fontname)
    rb2=widget_button(radio,VALUE="button in EXCLUSIVE base 2",uvalue={vEv,'rb2',[9,0]})
    
    tmp=widget_label(yoff=offy,button_base01,value="Non-Exclusive base,simple look") & offy+=10 ;
    
    check=widget_base(yoff=offy,button_base01,/NONEXCLUSIVE,COL=1) & offy+=100 ;
    cb1=widget_button(check,VALUE="button in NONEXCLUSIVE base 1",uvalue={vEv,'cb1',[81,0]}, font=fontname)
    cb2=widget_button(check,VALUE="button in NONEXCLUSIVE base 2",uvalue={vEv,'cb2',[12,0]})
    
    tmp=widget_label(yoff=offy,button_base01,value='2 CW_BGROUP /COL in a framed base') & offy+=10 ;
    
    bg=widget_base(yoff=offy,button_base01,/ROW,Frame=10) & offy+=300 ;
    values = ['One', 'Two', 'Three', 'Four', 'Five','Six'] 
    
    bgroup1 = CW_BGROUP(yoff=offy,bg, values, /COLUMN, /EXCLUSIVE, $ 
                        LABEL_TOP='Exclusive', FRAME=10,SET_VALUE=3) 
    bgroup2 = CW_BGROUP(yoff=offy,bg, values, /COLUMN, /NONEXCLUSIVE, $ 
                        LABEL_TOP='Nonexclusive', FRAME=10, SET_VALUE=[1,0,1,0,1])
    
    tmp=widget_label(yoff=offy,button_base02,value="A menu button frame 10") & offy+=10 ;
    menu=widget_button(yoff=offy,button_base02,value="Menu",/menu,frame=10) & offy+=100 ;
    entry1=widget_button(menu,value="entry 1")
    entry2=widget_button(menu,value="entry 2")
    
    tmp=widget_label(yoff=offy,button_base02,value="A menu button with Bitmap, frame 30") & offy+=10              ;
    menu=widget_button(yoff=offy,button_base02,value=myBitmap(),/menu,frame=30,tooltip='A TOOOOOLTIP!!!') & offy+=100 ;
    entry1b=widget_button(menu,value=myBitmap(),/menu)
    entry1=widget_button(menu,value='submenu',/menu)
    entry2b=widget_button(entry1b,value=myBitmap(),/menu)
    entry2=widget_button(entry1,value="submenu",/menu)
    entry3b=widget_button(entry2b,value=myBitmap())
    entry3=widget_button(entry2,value='entry')
    
    tmp=widget_label(yoff=offy,button_base02,value="A fancy menu button") & offy+=10     ;
    menu=widget_button(yoff=offy,button_base02,value="Menu",/menu,font=fontname) & offy+=100 ;
    entry1=widget_button(menu,value="entry 1",font=fontname)
    entry2=widget_button(menu,value="entry 2")
    
    tmp=widget_label(yoff=offy,button_base02,value="ALIGNMENTS",/align_center,/fram) & offy+=10 ;
    base1=widget_base(yoff=offy,button_base02,/COL,/fram) & offy+=10                  ;
    tmp=widget_label(yoff=offy,base1,value="Row Base 1")
    base11=widget_base(yoff=offy,base1,/ROW,/fram)
    tmp=widget_label(base11,value="inherited")
    but=widget_button(base11,value="some button, no align",xsize=200)
    tmp=widget_label(yoff=offy,base1,value="Row Base 2") & offy+=10                        ;
    base12=widget_base(yoff=offy,base1,/ROW,/fram) & offy+=10                              ;
    tmp=widget_label(base12,value="/align_left",/align_left) & offy+=10                    ;
    but=widget_button(base12,value="some button, left-aligned",xsize=200,/align_left) & offy+=10 ;
    tmp=widget_label(yoff=offy,base1,value="Row Base 3")
    base13=widget_base(yoff=offy,base1,/ROW,/fram)
    tmp=widget_label(base13,value="/align_right") & offy+=10                                   ;
    but=widget_button(base13,value="some button, right-aligned",xsize=200,/align_right) & offy+=10 ;
    base2=widget_base(yoff=offy,button_base02,/COL,/fram) & offy+=10                               ;
    tmp=widget_label(base2,value="Column Base") & offy+=10                                       ;
    tmp=widget_label(base2,value="A Button inheriting base orientation: text centered") & offy+=10 ;
    but=widget_button(base2,value="some button, no align",xsize=200) & offy+=10                      ;
    tmp=widget_label(base2,value="A left-aligned button",/align_left) & offy+=10                   ;
    but=widget_button(base2,value="some button, left-aligned",xsize=300,/align_left) & offy+=10      ;
    tmp=widget_label(base2,value="A right-aligned button") & offy+=10                              ;
    but=widget_button(base2,value="some button, right-aligned",xsize=200,/align_right) & offy+=10    ;
endif

if total(strcmp('TABLE',present,/fold)) then begin
; TABLE
   table_base = widget_base( tabbed_base, TITLE="TABLEs",_extra=extra) & offy=0

;
   mytable1=widget_table(UVALUE={vEv,'table1',[0,0]},yoff=offy,table_base,value=dist(7),xsize=5,ysize=5,/all);,font=fontname,frame=30) & offy+=200 ;
;to be implemented! ;widget_control,mytable1,/editable,use_table_sel=[1,1,4,4]
   widget_control,mytable1,edit_cell=[1,1]
   widget_control,mytable1,background_color=[255,255,0],use_table_sel=[1,1,4,4]
   widget_control,mytable1,/table_disjoint
   nrows=n_elements(table)
   subsize=nrows < 6
   mytable2=widget_table(UVALUE={vEv,'table2',[0,0]},yoff=offy,table_base,value=table[0:subsize-1],/row_major,row_labels='',column_labels=tags,column_width=60,/resizeable_columns,y_scroll_size=40,/edit,alignment=2,frame=20) & offy+=10 ;
endif

if total(strcmp('TREE',present,/fold)) then begin
; TREE
   tree_base = widget_base( tabbed_base, TITLE="TREE",_extra=extra) & offy=0

   if ~keyword_set(notree) then begin
      racine = widget_tree(yoff=offy,tree_base,ysize=100) & offy+=90 ;
      wtRoot = widget_tree(racine, VALUE='GDL', /folder, /draggable, /drop_events,ysize=200)
      feuille_11 = WIDGET_TREE(wtRoot, VALUE='is', $
                               UVALUE='LEAF')
      branche_12 = WIDGET_TREE(wtRoot, VALUE='...', $
                               /FOLDER, /EXPANDED)
      feuille_121 = WIDGET_TREE(branche_12, VALUE='with', $
                                UVALUE='LEAF',/CHECKBOX,CHECKED=1,/NO_COPY, TOOLTIP='an unuseful tootip...')
      feuille_122 = WIDGET_TREE(branche_12, VALUE='a lot of', $
                                UVALUE='LEAF')
      feuille_13 = WIDGET_TREE(wtRoot, VALUE='widgets', $
                               UVALUE='LEAF')
      feuille_14 = WIDGET_TREE(wtRoot, VALUE='now', $
                               UVALUE='LEAF')
      racine = widget_tree(yoff=offy,tree_base,frame=50) & offy+=10 ;
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
endif




if total(strcmp('LABEL',present,/fold)) then begin
; LABEL
   label_base = widget_base( tabbed_base, TITLE="LABELs",_extra=extra) & offy=0

;
   label1=widget_label(yoff=offy,label_base,VALUE="Sunken-frame label, inherits centering",/align_center) & offy+=10                   ;
   label1=widget_label(yoff=offy,label_base,VALUE="Sunken-frame label",uvalue={vEv,'lll',[1,-1]},/sunken_frame,frame=33) & offy+=10                   ;
   label2=widget_label(yoff=offy,label_base,VALUE="Framed 33 pix Fancy label, inherits centering",/align_center) & offy+=100                  ;
   label2=widget_label(yoff=offy,label_base,VALUE="Framed Fancy label",uvalue={vEv,'lll',[1,-1]},font=fontname,frame=33) & offy+=100                  ;
   label2=widget_label(yoff=offy,label_base,VALUE="frame=1 xsize=100 of a long FANCY text herits centering",/align_center) & offy+=100 ;
   label2=widget_label(yoff=offy,xsize=100,label_base,VALUE="xSIZE=100 of a long FANCY text",uvalue={vEv,'lll',[1,-1]},font=fontname,/frame) & offy+=100 ;
   label3=widget_label(yoff=offy,label_base,VALUE="simple label, herits centering",uvalue={vEv,'lll',[1,-1]}) & offy+=10                                 ;
   label3=widget_label(yoff=offy,label_base,VALUE="simple label, centered",/align_center,uvalue={vEv,'lll',[1,-1]}) & offy+=10                          ;

   label3=widget_label(yoff=offy,label_base,VALUE='500 pixels wide, herits centering',/align_center) & offy+=10 ;
   label3=widget_label(yoff=offy,xsize=500,label_base,VALUE='I herit centering but am 300 pix long',uvalue={vEv,'lll',[1,-1]}) & offy+=10                                  ;
   label3=widget_label(yoff=offy,label_base,VALUE='same, framed 1',/align_center) & offy+=10 ;
   label3=widget_label(yoff=offy,xsize=500,label_base,VALUE='my frame herits centering but I am centered in my xsize=500',/frame,uvalue={vEv,'lll',[1,-1]}) & offy+=10                                  ;

   label3=widget_label(yoff=offy,label_base,VALUE='500 pixels wide, right-centered',/align_center) & offy+=10       ;
   label3=widget_label(yoff=offy,xsize=500,label_base,VALUE='I am right-aligned',/align_right) & offy+=10                                  ;
   label3=widget_label(yoff=offy,label_base,VALUE='same, framed 1',/align_center) & offy+=10       ;
   label3=widget_label(yoff=offy,xsize=500,label_base,VALUE='I am right-aligned in my xsize=500',/align_right,/frame) & offy+=10                                  ;

endif

if total(strcmp('DROPLIST',present,/fold)) then begin   
; DROPLIST
   droplist_base = widget_base( tabbed_base, TITLE="DROPLISTs",_extra=extra) & offy=0

;
   droplist=widget_droplist(yoff=offy,droplist_base,VALUE=["droplist","created","with","WIDGET_DROPLIST","(no YSIZE)"],title="A fancy droplist (framed)",font=fontname,frame=10) & offy+=50 ;
   droplist=widget_droplist(yoff=offy,droplist_base,VALUE=["droplist","created","with","WIDGET_DROPLIST","(no YSIZE)"],title="A fancy droplist",font=fontname) & offy+=50                   ;
   droplist=widget_droplist(yoff=offy,droplist_base,VALUE=["A","B","C"],title="A 1 char droplist") & offy+=20                                                                               ;
   droplist=widget_droplist(yoff=offy,droplist_base,VALUE=["droplist","no Title","created","with","WIDGET_DROPLIST","(no YSIZE)"]) & offy+=30                                               ;
   droplist=widget_droplist(yoff=offy,droplist_base,VALUE=["Centered droplist","no Title","created","with","WIDGET_DROPLIST","(no YSIZE)"],/align_center) & offy+=30                        ;
endif
if total(strcmp('COMBOBOX',present,/fold)) then begin
;COMBO
combobox_base = widget_base( tabbed_base, TITLE="COMBOBOXs",_extra=extra) & offy=0

;
combo=widget_combobox(xoff=100,yoff=offy,combobox_base,VALUE=["A fancy combobox (framed)","not editable","created","with","WIDGET_COMBOBOX"],frame=30,font=fontname) & offy+=100;
combo=widget_combobox(yoff=offy,combobox_base,VALUE=["A combobox","editable","created","with","WIDGET_COMBOBOX"],/editable) & offy+=30;
combo=widget_combobox(yoff=offy,combobox_base,VALUE=["A ysize=100 combobox","editable","created","with","WIDGET_COMBOBOX"],YSIZE=100) & offy+=120;
combo=widget_combobox(yoff=offy,combobox_base,VALUE=["Same, centered combobox","editable","created","with","WIDGET_COMBOBOX"],YSIZE=100,/align_center) & offy+=10;
endif
if total(strcmp('BASE',present,/fold)) then begin
 ;MISC. BASES
 bases_base0 = widget_base( tabbed_base , TITLE="BASEs",event_pro='base_event_base',_extra=extra) & offy=0

 tmp=widget_label(yoff=offy,bases_base0,value="below a tab-based base with frame=30",font=fontname) & offy+=10;
 tmp=widget_label(yoff=offy,bases_base0,value="containing a base, frame=100, with 2 buttons; ",font=fontname) & offy+=10;
 tmp=widget_label(yoff=offy,bases_base0,value="another with a label and a text; ",font=fontname) & offy+=10;
 tmp=widget_label(yoff=offy,bases_base0,value="another a /scroll ,frame=100, space=50",font=fontname) & offy+=10;
 bases_base = widget_base(yoff=offy, bases_base0,/COL,frame=30) & offy+=10;
 charpix = 8.5
 butSize = 7*charpix
 
actionBase=widget_base(bases_base,/ROW,frame=100) & offy+=10;

quitButton = widget_button(actionBase,value='WILL QUIT',uvalue={vEv,'quit',[5,0]}, font=fontnam)
downButton = widget_button(actionBase,value='A 300 px Button..', /dynamic_resize)
selectBase=widget_base(bases_base,/ROW,frame=1)
;a label that can be updated by pushing buttons
labeltoupdate = widget_label(selectBase, value='Label xsize=100(pixels),ysize=30(pixels)',xsize=100,ysize=30)
;show last button clicked value
statusLabel = widget_text(selectBase,value=['012345678901234567890123456789012','text, xsize=32(characters),ysize=10(chars)','3','4','5','6','7','8','9','10'], $
                          xsize=32,ysize=10)
                                ;pass label to top (avoid using a common)
widget_control,base,set_uvalue=[statusLabel,labeltoupdate]

;names of columns
tags = tag_names(table)
; nb of colums=nb tags
ncols=n_tags(table)
nrows=n_elements(table)         ; check...
;set max size (chars) of displayed button/label for each column
maxSize = intarr(nCols)
for icol = 0, nCols-1 do maxSize[icol] = 1 + MAX(strlen(strtrim(table.(icol),2))) 

;buttons containing header keyword values
buttons = lonarr(nCols,nRows)
;buttons containing column labels
tagButtons=lonarr(nCols)
fileButtons = lonarr(nRows)
fileNumbers = lonarr(nRows)
;base for listing contents of tables and to show selected files
;   fileBase=widget_base(bases_base,/scroll,frame=3)
;base to contain column labels and cells with table values and selected files
dummy = widget_label(bases_base,value=' ')
listentries=widget_base(bases_base,row=nRows+1,/scroll,space=10, x_scroll=300,frame=10,y_scroll=300)
widget_control,dummy,set_value='300x300 pix Scrolled Base below has ID='+string(listentries) 
;create column labels 
dummy = widget_label(listentries,value=' ', xsize=10)
dummy1 = widget_label(listentries,value=' ',xsize=10)
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


endif


;tests for /notify_realize and /kill_notify
;
widget_control,base,notify_realize='i_am_realized'
;Realize the widgets. 
WIDGET_CONTROL, /REALIZE, base 
 
;;Obtain the window index. 
if ~keyword_set(nocanvas) and total(strcmp('DRAW',present,/fold)) then begin
print,"Draw widgets:",draw,draw2
 WIDGET_CONTROL, draw, GET_VALUE = index 
  WIDGET_CONTROL, draw2, GET_VALUE = index2 

    ;;
    ;; Set the new widget to be the current graphics window 
 print,"window indexes",index,index2
 image=dist(128)
 WSET,index

  catch, error
  if error ne 0 then begin
     save,x,y,p,file="problemwithtriangulate.sav"
     message,/inf,"CONGRATULATIONS YOU FOUND (INVOLUNTARILY!) A PROBLEM WITH THE FAST TRIANGULATION ALGORITHM"
     message,"Please contribute to GDL by saving the file 'problemwithtriangulate.sav' and make an issue on github: https://github.com/gnudatalanguage/gdl , thanks in advance" 
     catch,/cancel
  endif

  n=100 & x=randomu(seed,n)& y=randomu(seed,n) &p=randomu(seed,10)*n & x[p]=x[3] &y[p]=y[22]& TRIANGULATE, x, y, tr,b,rep=r,conn=conn &myplot,tr,x,y,b,conn,1

    ;;
 WSET, index2 
 f=findgen(1000)/100.
 contour,cos(dist(100,100)/10.)
 tv,image,10,10,/data; ,/true
endif

xmanager,"handle",base,cleanup="cleanup_xmanager",no_block=~block
end
