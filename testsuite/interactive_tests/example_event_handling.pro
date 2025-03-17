; this example shows 3 different event 'history':
; - an event_pro, exit immediately when 'treated'
; - an event_func, passed from level while reporting the level
; - an event_func as above, but changed to event_pro at some level
pro tlb_catch_final_event,ev
  print,"tlb_catch_final_event called at "+strtrim(ev.handler,2)
;  help,ev
  if size(ev,/sname) eq "WIDGET_TLB_MOVE" then widget_control,ev.handler,tlb_set_xoff=ev.x,tlb_set_yoff=ev.y else print,"not a TLB_MOVE_EVENT"
end
function show,ev
  print,"show called at "+strtrim(ev.handler,2)
;  help,ev
  return,ev
end

pro report_procedure_event,ev
 print,'report_procedure_event called at '+strtrim(ev.id,2)+" handled by "+strtrim(ev.handler,2)
end

function report_function_event,ev
  print,"report_function_event called at "+strtrim(ev.id,2)+" handled by "+strtrim(ev.handler,2)
;  help,ev
;  return,ev
  nev={WIDGET_TLB_MOVE}
  nev.handler=44; widget_info(ev.id,/parent)
  nev.id=ev.id
  nev.top=1
  nev.x=1000*randomu(seed)
  nev.y=1000*randomu(seed)
;  if ev.handler eq 5 then return,ev
  return,nev
end

function changemind,ev
  print,"changemind called at "+strtrim(ev.id,2)+" handled by "+strtrim(ev.handler,2)
  widget_control,ev.top+4,event_fun=""
  widget_control,ev.top+4,event_pro="tlb_catch_final_event"
  return,ev;report_function_event(ev)
end

pro done,ev
  widget_control,ev.top,/destroy
end

n=8 & bases=lonarr(n)
bases[0]=widget_base(/col,event_pro="tlb_catch_final_event",/tlb_move)
for i=1,n-1 do bases[i]=widget_base(bases[i-1],/col,event_func="show")
b1=widget_button(bases[n-1],value="function_event",event_func="report_function_event",tooltip="event function: will traverse the "+strtrim(n,2)+" bases up to top and make the widget bounce" )
b2=widget_button(bases[n-1],value="procedure_event",event_pro="report_procedure_event",tooltip="event procedure: will be trapped only at tlb level")
b3=widget_button(bases[n-1],value="function_to_procedure",event_fun="changemind",tooltip="event function, transformed to procedure at tlb+4" )
b4=widget_button(bases[n-1],value="exit",event_pro="done")

;foreach i,[2L,3,4,5] do  widget_control,bases[i],event_func=""     
widget_control,bases[0],/realize
z=widget_event(bases[0])
;xmanager,"report_procedure",bases[0]
end
