;
; Joel Gales, March 2008
;
; Basic examples for widgets.
; You must have compiled GDL WITH WXWIDGETS library !
;
PRO widget2_event, ev  
WIDGET_CONTROL, ev.TOP, GET_UVALUE=textwid  
WIDGET_CONTROL, ev.ID, GET_UVALUE=uval  
CASE uval OF  
   'ONE' : WIDGET_CONTROL, textwid, SET_VALUE='Button 1 Pressed'  
   'TWO' : WIDGET_CONTROL, textwid, SET_VALUE='Button 2 Pressed'  
   'DONE': WIDGET_CONTROL, ev.TOP, /DESTROY  
ENDCASE  
END  
  
PRO widget2
print, 'Do you have compiles GDL with WXWIDGETS support ?'
;
base = WIDGET_BASE(/COLUMN)  
button1 = WIDGET_BUTTON(base, VALUE='One', UVALUE='ONE')  
button2 = WIDGET_BUTTON(base, VALUE='Two', UVALUE='TWO')  
text = WIDGET_TEXT(base, XSIZE=20)  
button3 = WIDGET_BUTTON(base, value='Done', UVALUE='DONE')  
WIDGET_CONTROL, base, SET_UVALUE=text  
WIDGET_CONTROL, base, /REALIZE  
XMANAGER, 'widget2', base  
END  

