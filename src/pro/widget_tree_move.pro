; duplicate all properties of 'nodeToCopy', add it in 'targetfolder',
function gdlWidgetTreeDuplicateNode, targetfolder, nodeToCopy, doCopyChildren, index = index, copy = copy, callback_function = callback_function, userdata = userdata

  compile_opt hidden, idl2
  ON_ERROR, 2

  folder=widget_info( nodetocopy, /tree_folder )
  expanded=widget_info( nodetocopy, /tree_expanded) 
  bitmap = widget_info( nodetocopy, /tree_bitmap )
  mask = widget_info( nodetocopy, /mask )
  draggable = widget_info( nodetocopy, /draggable )
  drag_notify = widget_info( nodetocopy, /drag_notify )
  drop_events = widget_info( nodetocopy, /drop_events )
  widget_control, nodetocopy, get_value = value
  widget_control, nodetocopy, get_uvalue = uvalue
  copiednode = widget_tree( $
               targetfolder, index = index, folder=folder, expanded=expanded,$
               bitmap=bitmap,mask=mask,draggable=draggable,drag_notify=drag_notify,$
               drop_events=drop_events,value=value,uvalue=uvalue)
  if  strlen(widget_info( nodeToCopy, /uname )) gt 0 then widget_control,nodeToCopy,set_uname=widget_info(nodeToCopy,/uname)
  if  strlen(widget_info( nodeToCopy, /event_func )) gt 0 then widget_control,nodeToCopy,event_func=widget_info(nodeToCopy,/event_func)
  if  strlen(widget_info( nodeToCopy, /event_pro )) gt 0 then widget_control,nodeToCopy,event_pro=widget_info(nodeToCopy,/event_pro)

  doCopyChildren = 1
  if ( n_elements( callback_function ) ne 0 ) then doCopyChildren = call_function(callback_function , nodeToCopy, copiedNode, userdata=userdata )
  RETURN, copiedNode
end

; reentrant function to copy a node in a tree
function gdlWidgetTreeCopy, targetfolder, nodeToCopy,_STRICT_EXTRA=extra

  compile_opt hidden, idl2
  ON_ERROR, 2

  allChildren = widget_info(nodeToCopy, /all_children )
  nChildren= widget_info( nodeToCopy, /n_children ) 
  newItem = gdlWidgetTreeDuplicateNode( targetfolder, nodeToCopy, doCopyChildren,_STRICT_EXTRA=extra)
  ; doCopyChildren is here to enable or not the children copy, as it may have been canceled by the callback function if any 
  if ( ( n_elements(doCopyChildren) gt 0 ) && (doCopyChildren ne 0) ) then begin
    for ichild = 0, nChildren-1 do newChild = gdlWidgetTreeCopy( newItem, allChildren[ichild],_STRICT_EXTRA=extra)
  endif
  RETURN, newItem
end

; widget_tree_move: companion to widget_tree, enable drop of whole
; tree hierachies onto a same or different widget_tree 
; nb: update is ignored (? wxWidgets does not 'flicker' ? TBC)
pro widget_tree_move, toBeMoved, receiverFolder, callback_func = callback_fun, copy = copy, index = index, $
                      select = select, update = update, userdata = userdata

  compile_opt idl2
  ON_ERROR, 2

  remove=~keyword_set( copy )
  ;check arguments
  if ( n_params() ne 2 ) then message, 'Incorrect number of arguments.'
  if ( n_elements( index ) eq 0 ) then index=-1
  ; if index, reverse list of toBeMoved nodes to add last first,
  ; as position where to add those nodes is fixed.  
  if index ge 0 then toBeMoved=REVERSE( toBeMoved )
  n=n_elements(toBeMoved)-1
  ; copy one after the other
  for iNode=0,n do begin
    item = gdlWidgetTreeCopy( receiverfolder, tobemoved[iNode],_strict_extra=extra )
    if ( keyword_set(select)) then widget_control, item, /set_tree_select
  endfor
  ; remove now 'old' items unless 'copy' was asked for:
  if (remove) then for iNode=n,0,-1 do widget_control, tobemoved[iNode], /destroy
end
