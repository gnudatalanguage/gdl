; Automatic cleanup behavior verified against IDL 8.4 with
; cleanup_precedence_idl_check_20261116.pro (P1-P8, EXECUTE and CATCH).
function acp_obj::init, label, mode
  self.label = label
  self.mode = mode
  return, 1
end

pro acp_obj::cleanup
  common acp_events, events
  events = [events, 'cleanup:' + self.label]
  ; mode 0 succeeds; mode 1 fails; mode 2 catches its own failure.
  if self.mode eq 2 then begin
    catch, error
    if error ne 0 then begin
      catch, /cancel
      events = [events, self.label + ':caught']
      return
    endif
  endif
  if self.mode ne 0 then message, 'cleanup failed: ' + self.label
end

pro acp_obj::defuse
  self.mode = 0
end

function acp_obj::drop
  return, acp_drop()
end

function acp_obj::local_drop
  local = obj_new('acp_obj', 'A', 1)
  return, 1
end

pro acp_derived__define
  definition = {acp_derived, inherits acp_obj}
end

function acp_derived::parent_drop
  return, self->acp_obj::local_drop()
end

pro acp_obj__define
  definition = {acp_obj, label:'', mode:0}
end

function acp_drop
  common acp_events, events
  local = obj_new('acp_obj', 'A', 1)
  events = [events, 'drop:return']
  return, 1
end

function acp_rhs, fail
  common acp_events, events
  events = [events, 'rhs:entered']
  if fail then message, 'rhs failed'
  return, 2
end

pro acp_operation, n, value, after
  common acp_events, events
  case n of
    1: begin
      local = obj_new('acp_obj', 'A', 1)
      events = [events, 'body:primary']
      message, 'primary failure'
    end
    2: begin
      local = [obj_new('acp_obj', 'A', 1), obj_new('acp_obj', 'B', 1)]
      events = [events, 'body:primary']
      message, 'primary failure'
    end
    3: begin
      local = [obj_new('acp_obj', 'A', 1), obj_new('acp_obj', 'B', 0)]
      local = 0
    end
    4: begin
      local = [obj_new('acp_obj', 'A', 1), obj_new('acp_obj', 'B', 1)]
      local = 0
    end
    5: value = acp_drop() + acp_rhs(1)
    6: begin
      local = obj_new('acp_obj', 'A', 1)
      value = (local = 0) + acp_rhs(1)
    end
    7: value = acp_drop() + acp_rhs(0)
    8: begin
      local = [obj_new('acp_obj', 'A', 1), obj_new('acp_obj', 'B', 2)]
      local = 0
    end
  endcase
  after = 1
end

pro acp_catch_operation, n, value, after, caught, msg, code
  caught = 0
  catch, error
  if error ne 0 then begin
    catch, /cancel
    caught = 1
    msg = !error_state.msg
    code = !error_state.code
    return
  endif
  acp_operation, n, value, after
  msg = !error_state.msg
  code = !error_state.code
end

; The caller holds this snapshot through reporting and defusing, so inspecting
; survivors cannot itself release their last reference and trigger CLEANUP.
function acp_survivors, objects
  count = 0
  for i=0,n_elements(objects)-1 do begin
    if obj_isa(objects[i], 'acp_obj') then count = count + 1
  endfor
  return, count
end

pro acp_finish, objects
  for i=0,n_elements(objects)-1 do begin
    if obj_isa(objects[i], 'acp_obj') then begin
      objects[i]->defuse
      obj_destroy, objects[i]
    endif
  endfor
end

function acp_s, value
  return, strtrim(string(value), 2)
end

pro acp_test_case, n, errors, caller_catch=caller_catch
  common acp_events, events
  descriptions = ['primary error, one failing local cleanup', $
    'primary error, two failing local cleanups', $
    'drop array [A fails, B succeeds]', $
    'drop array [A fails, B fails]', $
    'function-return cleanup failure + failing RHS', $
    'assignment cleanup failure + failing RHS', $
    'function-return cleanup failure + successful RHS', $
    'drop array [A fails, B catches its own error]']
  mode = 'EXECUTE'
  if keyword_set(caller_catch) then mode = 'CATCH'
  label = 'P' + acp_s(n) + ' ' + mode
  print, 'BEGIN ' + label + ': ' + descriptions[n-1]
  events = ['']
  value = -99
  after = 0
  msg = ''
  code = 0
  message, /reset
  if keyword_set(caller_catch) then begin
    acp_catch_operation, n, value, after, caught, msg, code
    outcome = 'caught=' + acp_s(caught)
  endif else begin
    r = execute('acp_operation, n, value, after')
    msg = !error_state.msg
    code = !error_state.code
    outcome = 'r=' + acp_s(r)
  endelse
  event_text = '(none)'
  if n_elements(events) gt 1 then event_text = strjoin(events[1:*], ',')
  snapshot = obj_valid()
  survivors = acp_survivors(snapshot)
  print, label + ': ' + outcome + ' after=' + acp_s(after) + $
    ' value=' + acp_s(value) + ' live=' + acp_s(survivors) + $
    ' code=' + acp_s(code) + ' msg=' + msg + ' events=' + event_text
  expected_events = ['body:primary,cleanup:A', $
    'body:primary,cleanup:A,cleanup:B', 'cleanup:A', 'cleanup:A', $
    'drop:return,cleanup:A', 'cleanup:A', 'drop:return,cleanup:A', 'cleanup:A']
  expected_live = [0, 0, 1, 1, 0, 0, 0, 1]
  expected_label = 'A'
  if n eq 2 then expected_label = 'B'
  if keyword_set(caller_catch) then begin
    if caught ne 1 then errors_add, errors, label + ': caller did not catch error'
  endif else begin
    if r ne 0 then errors_add, errors, label + ': EXECUTE status'
  endelse
  if after ne 0 then errors_add, errors, label + ': continued after failure'
  if value ne -99 then errors_add, errors, label + ': assigned failed expression'
  if event_text ne expected_events[n-1] then $
    errors_add, errors, label + ': cleanup/evaluation order'
  if survivors ne expected_live[n-1] then errors_add, errors, label + ': survivors'
  if code ne -5 then errors_add, errors, label + ': error code'
  if msg ne 'ACP_OBJ::CLEANUP: cleanup failed: ' + expected_label then $
    errors_add, errors, label + ': error precedence'
  acp_finish, snapshot
end

pro acp_sink, value, reached
  reached = 1
end

function acp_reference_fail
  common acp_reference, reference
  local = obj_new('acp_obj', 'A', 1)
  return, reference
end

; Exercise the owned and referenced function results used by different callers.
pro acp_test_call_paths, errors
  common acp_events, events
  common acp_reference, reference
  reference = 42
  receiver = obj_new('acp_derived', 'receiver', 0)
  commands = ['value=acp_drop()', $
    "value=call_function('acp_drop')+acp_rhs(0)", $
    'acp_sink, acp_drop(), reached', $
    'value=receiver->local_drop()+acp_rhs(0)', $
    'acp_sink, receiver->local_drop(), reached', $
    'value=receiver->parent_drop()+acp_rhs(0)', $
    "value=call_method('local_drop', receiver)+acp_rhs(0)", $
    'acp_sink, acp_reference_fail(), reached', $
    'acp_reference_fail()=123', $
    "value=n_elements(obj_new('acp_obj', 'A', 1))+acp_rhs(0)"]
  foreach command, commands do begin
    value = -99
    reached = 0
    events = ['']
    status = execute(command)
    if status ne 0 then errors_add, errors, 'cleanup call status: '+command
    if value ne -99 then errors_add, errors, 'assigned result after cleanup: '+command
    if reached ne 0 then errors_add, errors, 'called procedure after cleanup: '+command
    if reference ne 42 then errors_add, errors, 'modified reference after cleanup: '+command
    if total(events eq 'rhs:entered') ne 0 then $
      errors_add, errors, 'evaluated RHS after cleanup: '+command
    if !error_state.msg ne 'ACP_OBJ::CLEANUP: cleanup failed: A' then $
      errors_add, errors, 'cleanup call error: '+command
  endforeach
  obj_destroy, receiver
end

pro test_cleanup_precedence, reference_only=reference_only
  errors = 0
  for n=1,8 do acp_test_case, n, errors
  for n=1,8 do acp_test_case, n, errors, /caller_catch
  if ~keyword_set(reference_only) then acp_test_call_paths, errors
  banner_for_testsuite, 'TEST_CLEANUP_PRECEDENCE', errors
  if errors gt 0 then exit, status=1
end
