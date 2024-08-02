
pro   IDL_IDLBridge::Abort
  gmem_abort,self.handle
end
pro    IDL_IDLBridge::Cleanup
  gmem_send,self.handle,"exit",nowait=1
end

pro    IDL_IDLBridge::Execute, what, nowait=nowait
  gmem_send,self.handle,what,nowait=nowait
end

pro    IDL_IDLBridge::GetProperty, userdata=userdata,ouput=output,ops=ops,callback=callback, var=var
  if n_elements(var) gt 0 then var=0
  if n_elements(output) gt 0 then output=self.output
  if n_elements(ops) gt 0 then ops=self.ops
  if n_elements(callback) gt 0 then callback=self.callback
  if n_elements(userdata) gt 0 then userdata=*(self.userdata)
end

function IDL_IDLBridge::GetVar,varname
  return, gmem_getvar(self.handle,varname)
end


pro IDL_IDLBridge::OnCallback, Status, Error
  if self.callback ne "" then call_procedure, self.callback, status, error, self, *(self.userdata)
end

pro    IDL_IDLBridge::SetProperty, userdata=userdata,ops=ops,callback=callback
  if arg_present(ops) then self.ops=ops
  if arg_present(callback) then self.callback=callback
  if arg_present(userdata) then self.userdata=ptr_new(userdata)
end

pro    IDL_IDLBridge::SetVar, Name, Value
  gmem_setvar,self.handle,name,value
end

function    IDL_IDLBridge::Status,error=error
  status=gmem_status(self.handle, err=err)
  if n_elements(error) gt 0 then error=err
  print,gmem_receive(self.handle);
  return, status
end

function   IDL_IDLBridge::Init, userdata=userdata,ouput=output,ops=ops,callback=callback
  self.handle=gmem_fork(self)
  if arg_present(output) then self.output=output
  if arg_present(ops) then self.ops=ops
  if arg_present(callback) then self.callback=callback
  if arg_present(userdata) then self.userdata=ptr_new(userdata)
  return, 1
end

