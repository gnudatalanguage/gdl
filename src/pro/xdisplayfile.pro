;+
;
; NAME: XDISPLAYFILE
;
; PURPOSE: show file contents using widgets
;
; CATEGORY: FILE (IO)
;
; CALLING SEQUENCE:
;
;     XDISPLAYFILE, filename, block=block, done_button=done_button,
;                   editable=editable, font=font, group=group,
;                   grow_to_screen=grow_to_screen, height=height, modal=modal,
;                   return_id=return_id, text=text, $
;                   title=title, width=width, wtext=wtext
;
; KEYWORD PARAMETERS:
;     filename       : filename
;
; OPTIONAL INPUTS:
;     block          : block events or not
;     done_button    : title of done button.
;     editable       : 1 = editable, 0 = read only.
;                      Setting this keyword also appends save/save as in File menu.    
;     font           : font to be used in widget_text.
;     group          : sets group leader.
;     grow_to_screen : not implemented
;     height         : height of widget_text
;     modal          : sets dialog modal.
;     return_id      : returns widget id of dialog.
;     text           : default text. Setting this keyward ignores filename.
;     title          : title of dialog.
;     width          : width of widget_text
;     wtext          : returns widget id of widget_text.  
;
; TODO:
;         * Currently XDISPLAYFILE does not automatically resize widget_text.
;         grow_to_screen is not implemented yet.
;         * no new lines can be added using /Editable
;         * binary files cannot be displayed ...
;
; PROCEDURE:
;         Use widgets to show file contents
;
; EXAMPLE: 
;         xdisplayfile, "", text="Example text"
;         xdisplayfile, "/var/log/gdl.log"
;
; MODIFICATION HISTORY:
;  Written by: Jeongbin Park 2015-04-30
;  AC 150506: Adding working keyword Editable, ok for basic input on
;  same line
; 
;-
; LICENCE:
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
pro XDISPLAYFILE_SAVE, filename, wtop
;
WIDGET_CONTROL, WIDGET_INFO(wtop, FIND_BY_UNAME='text'), GET_VALUE=text
OPENW, lun, filename, /GET_LUN
PRINTF, lun, text
FREE_LUN, lun
;
end
;
pro XDISPLAYFILE_EVENT, ev
;
WIDGET_CONTROL, ev.ID, GET_UVALUE=uval
case uval of
    'done': begin
        if ev.SELECT then WIDGET_CONTROL, ev.TOP, /DESTROY
    end
    'saveas': begin
        if ev.SELECT then begin
            filename = dialog_pickfile(/WRITE)
            xdisplayfile_save, filename, ev.TOP
        endif
    end
    else: begin
        if ev.SELECT then begin
            case STRMID(uval, 0, 2) of
                's_': xdisplayfile_save, STRMID(uval, 2), ev.TOP
                else:
            endcase
        endif
    end
endcase
end
;
; TODO: grow_to_screen
;
pro XDISPLAYFILE, filename, block=block, done_button=done_button, $
                  editable=editable, font=font, group=group, $
                  grow_to_screen=grow_to_screen, height=height, $
                  modal=modal, return_id=return_id, text=text, $
                  title=title, width=width, wtext=wtext, $
                  help=help, test=test, debug=debug
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'pro XDISPLAYFILE, filename, block=block, done_button=done_button, $'
    print, '                  editable=editable, font=font, group=group, $'
    print, '                  grow_to_screen=grow_to_screen, height=height, $'
    print, '                  modal=modal, return_id=return_id, text=text, $'
    print, '                  title=title, width=width, wtext=wtext, $'
    print, '                  help=help, test=test, debug=debug'
    return
endif
;
if N_PARAMS() EQ 0 then MESSAGE, 'Missing Filename !'
;
if (STRCMP(filename, "") EQ 0 AND FILE_TEST(filename) EQ 0) then begin
    res=DIALOG_MESSAGE('File <<'+filename+'>> not found, continue ? ',/question)
    if (res EQ 'No') then return
endif
if (FILE_TEST(filename,/directory) EQ 1) then begin
    res=DIALOG_MESSAGE('This is a Directory !',/error)
    if (res EQ 'No') then return
endif
;
if NOT(KEYWORD_SET(width)) then width = 80
if NOT(KEYWORD_SET(height)) then height = 24
no_block = 0
if NOT(KEYWORD_SET(block)) then no_block = 1
if NOT(KEYWORD_SET(title)) then title = 'GDL'

if KEYWORD_SET(modal) AND NOT(KEYWORD_SET(group)) then group = WIDGET_BASE()
if KEYWORD_SET(modal) OR KEYWORD_SET(group) then begin
    if KEYWORD_SET(modal) then begin
        base = WIDGET_BASE(MBAR=bar, GROUP_LEADER=group, TITLE=title, /MODAL)
    endif else begin
        base = WIDGET_BASE(MBAR=bar, GROUP_LEADER=group, TITLE=title)
    endelse
endif else begin
    base = WIDGET_BASE(MBAR=bar, TITLE=title)
endelse

menu = WIDGET_BUTTON(bar, VALUE='File', /MENU)
if KEYWORD_SET(font) then begin
    wtext = WIDGET_TEXT(base, editable=editable, XSIZE=width, UNAME='text', $
                        YSIZE=height, FONT=font, /SCROLL)
endif else begin
    wtext = WIDGET_TEXT(base, editable=editable, XSIZE=width, UNAME='text', $
                        YSIZE=height, /SCROLL)
endelse
;
if KEYWORD_SET(text) then begin
    WIDGET_CONTROL, wtext, SET_VALUE=text
    if NOT(KEYWORD_SET(done_button)) then begin
        done_button = 'Done with XDisplayFile'
    endif
endif else begin
    if FILE_TEST(filename, /READ) then begin        
        OPENR, lun, filename, /GET_LUN
        line = ''
        while not EOF(lun) do begin
            READF, lun, line
            if KEYWORD_SET(debug) then print, STRLEN(line)
            WIDGET_CONTROL, wtext, SET_VALUE=line, /APPEND
        endwhile
        FREE_LUN, lun
    endif else begin
        ;;WIDGET_CONTROL, wtext, SET_VALUE='FILE_LINES: Error opening file.   
        ;; File: ' + filename, /APPEND
        WIDGET_CONTROL, wtext, SET_VALUE='Unable to display ' + filename, /APPEND
    endelse
    if NOT(KEYWORD_SET(done_button)) then begin
        done_button = 'Done with ' + filename
    endif
endelse
;
if KEYWORD_SET(editable) then begin
    button = WIDGET_BUTTON(menu, VALUE='Save', UVALUE='s_' + filename)
    button = WIDGET_BUTTON(menu, VALUE='Save As...', UVALUE='saveas')
    button = WIDGET_BUTTON(menu, VALUE=done_button, UVALUE='done', /SEPARATOR)
endif else begin
    button = WIDGET_BUTTON(menu, VALUE=done_button, UVALUE='done')
endelse
;
WIDGET_CONTROL, base, /REALIZE
if KEYWORD_SET(modal) OR KEYWORD_SET(group) then begin
    XMANAGER, 'xdisplayfile', base, NO_BLOCK=no_block, GROUP_LEADER=group
endif else begin
    XMANAGER, 'xdisplayfile', base, NO_BLOCK=no_block
endelse
;
return_id = base
;
end
