;+
; NAME:   DIALOG_MESSAGE
;
; PURPOSE:
;
; This function try to reproduce the IDL's DIALOG_MESSAGE behavior
; using "zenity" or "wxwidgets"
;
; zenity, under GNU GPL, is available on most Linux distributions and
; and also on OSX (tested) and MSwin (not tested ?). It is better to
; have zenity version >= 2.23.1.
;
; wxwidgets version >= 2.8
;
; CATEGORY:
;
; CALLING SEQUENCE:
;
;    resu_list=DIALOG_MESSAGE( Message_Text [, TITLE=string]
;               [, /ERROR | , /INFORMATION | , /QUESTION]   <<-- the main options
;               [, /CANCEL] [, /CENTER] [, /DEFAULT_CANCEL | , /DEFAULT_NO]
;               [,DIALOG_PARENT=widget_id] [, DISPLAY_NAME=string] [, RESOURCE_NAME=string])
;
; INPUTS:
;	Message_Text: A scalar string or string array that contains the text of
; the message to be displayed. If this argument is set to an array of strings, 
; each array element is displayed as a separate line of text.
;
; OPTIONAL INPUTS: none
;
; KEYWORD PARAMETERS:
;	CANCEL: Set this keyword to add a "Cancel" button to the dialog.
;	CENTER: Set this keyword to center the dialog on the screen. 
;	DEFAULT_CANCEL: Set this keyword to make the "Cancel" button the default 
; selection for the dialog. The default selection is the button that is selected
; when the user presses the default keystroke (usually Space or Return depending
; on the platform). Setting DEFAULT_CANCEL implies that the CANCEL keyword is also
; set.
;	DEFAULT_NO: Set this keyword to make the "No" button the default selection
; for "Question" dialogs. Normally, the default is "Yes".
;	DIALOG_PARENT: Set this keyword to the widget ID of a widget over which
; the message dialog should be positioned. When displayed, the DIALOG_MESSAGE dialog
; will be positioned over the specified widget. Dialogs are often related to a
; non-dialog widget tree. The ID of the widget in that tree to which the dialog is most
; closely related should be specified.
;	DISPLAY_NAME: Set this keyword equal to a string indicating the name of the
; X Windows display on which the dialog is to appear. This keyword is ignored if the
; DIALOG_PARENT keyword is specified. This keyword is also ignored on Microsoft Windows
; platforms.
;	ERROR: Set this keyword to create an "Error" dialog. The default dialog type
; is "Warning".
;	INFORMATION: Set this keyword to create an "Information" dialog. The default
; dialog type is "Warning".
;	QUESTION: Set this keyword to create a "Question" dialog. The default dialog
; type is "Warning".
; 	RESOURCE_NAME: A string containing an X Window System resource name to be applied
; to the dialog. See RESOURCE_NAME for a complete discussion of this keyword.
;	TITLE: Set this keyword to a scalar string that contains the text of a title to
; be displayed in the dialog frame. If this keyword is not specified, the dialog has the
; dialog type as its title as shown in the table under DIALOG_MESSAGE. 
;
;	NON IDL KEYWORDS
;
;	ZENITY_NAME: Set this keyword to a scalar string used to overwrite Zenity
; executable name.
;	ZENITY_PATH: If set, Zenith will be searched in this path. If it doesn't 
; exist, then it will be searched in common unix paths. This can also be done
; by exporting the shell variable $ZENITY_PATH.
;	HELP: Display a help message and return
;	DEBUG: Display bebug messages
;	VERBOSE: Switch on verbose mode
;	
; OUTPUTS:
;
; OPTIONAL OUTPUTS:
;
; COMMON BLOCKS: none
;
; SIDE EFFECTS:
;               - due to Zenity's capacities, the function ingnores
;                 the keyword parameters: DEFAULT_CANCEL, DEFAULT_NO,
;                 DIALOG_PARENT, CENTER
;
; RESTRICTIONS: 
;               - need Zenity v2.23.1 or higher to use filters
;                 (--ok-label=TEXTE and --cancel-label=TEXTE )
;               - interface is not exactly the same ...
;
; PROCEDURE:  straithforward
;
; EXAMPLE:    resu_list=DIALOG_MESSAGE('hola')
;             resu_list=DIALOG_MESSAGE('would you like to continue ?', /question)
;
; TESTABILITY: please test using interactive TEST_DIALOG_MESSAGE in testsuite/
;              and add new test cases in this file
;
; MODIFICATION HISTORY:
;
; 23-Feb-2011: Implemented by Pedro Corona from DIALOG_PICKFILE written by Maxime Lenoir.
;  2-Mar-2011: Alain Coulais: Included the capability of manage more than one line of text
;              older zenity Version management (no custom filters before v2.23.1)
; 15-Nov-2012: AC: complete revision, some codes exported into common ZENITY_CHECK()
;              for better maintainability, better stick to orignal,
;              using --list when need (old Zenity or/and /cancel)
; 05-Apr-2015: JP: Code for Windows platform is added. 
;-
;
; This function try to reproduce the IDL's DIALOG_MESSAGE behavior using "zenity".
;
; zenity, under GNU GPL, is available in package on most Linux
; distributions (available and tested on CentOS 5.4, Mandriva 2010 and Ubuntu 9.04)
; Also on OSX: http://www.macports.org/ports.php?by=name&substr=zenity
;
; Since the implementation use Zenity as dialog messenger, 
; some functionalities are different from the original one
; but this function works well for general purpose.
;
; Make sure that you have Zenity in your PATH
; You need to have Zenity v2.23.1 or higher to use filters and /Cancel renaming.
;
; Note: this code was tested, in conjonction with TEST_DIALOG_MESSAGE
;       with Zenity 2.16, 2.24 and 2.30. Please report output of
;       "zenity --version" and error code from TEST_DIALOG_MESSAGE if problem.
;
;-
; LICENCE:
; Copyright (C) 2011, Pedro Corona and Maxime Lenoir (main author),
; 2012 Alain Coulais. 2015 JPark.
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
; 
;-
;
function DIALOG_MESSAGE_ZENITY, Message_Text, TITLE=title, CANCEL=cancel, $
                                ERROR=error, INFORMATION=information, QUESTION=question, $
                                DEFAULT_CANCEL=defaul_cancel, DEFAULT_NO=default_no, $
                                CENTER=center, DIALOG_PARENT=dialog_parent, $
                                DISPLAY_NAME=display_name, RESOURCE_NAME=resource_name, $
                                HELP=help, test=test, debug=debug, $
                                ZENITY_NAME=zenity_name, ZENITY_PATH=zenity_path
on_error, 2
;
if (N_params() NE 1) then MESSAGE, 'Incorrect number of arguments.'
;
zenity=ZENITY_CHECK( zenity_name=zenity_name,  zenity_path=zenity_path, $
                     zenity_version=zenity_version, $
                     help=help, test=test, debug=debug, verbose=verbose)
;
; improbing more than one text line
zenity_message_text=Message_Text[0]
for i=1, N_ELEMENTS(Message_Text)-1 do zenity_message_text+='\n'+Message_Text[i]
;
cmd=zenity+' --text="'+zenity_message_text+'" '
;
; Set the X Window display
if KEYWORD_SET(display_name) then cmd+='--display="'+STRING(display_name)+'" '
if KEYWORD_SET(resource_name) then cmd+='--name="'+STRING(resource_name)+'" '
;
; TITLE or INFORMATIONAL or ERROR
; default title (if nothing provided, is "Warning")
title=''
if KEYWORD_SET(error) then title='Error'
if KEYWORD_SET(question) then title='Question'
if KEYWORD_SET(error) AND KEYWORD_SET(question) then title='Information'
if KEYWORD_SET(information) then title='Information'
if KEYWORD_SET(title) then title=STRING(title[0])
if STRLEN(title) GT 0 then cmd+='--title="'+title+'" '
;
;temporal string to store the kind of zenity in.
;default option INFORMATION
;kindof='--info'
; ERROR
;if KEYWORD_SET(error) then kindof='--warning'

; QUESTION
;if KEYWORD_SET(question) && ~KEYWORD_SET(information) && ~KEYWORD_SET(error) then begin

; even with new zenity, we cannot have directly 3 buttons ...
;
if KEYWORD_SET(question) then begin
   if KEYWORD_SET(cancel) then begin
      kindof='--list --column="selection" "Yes" "Cancel" "No"'
   endif else begin
      if (zenity_version GE 22301) then begin
         kindof='--question --cancel-label="No" --ok-label="Yes"'
      endif else begin
         ;; old zenity: names of buttons cannot be changed ...
         kindof='--question'
      endelse
   endelse    
endif else begin
   if KEYWORD_SET(cancel) then begin
      if (zenity_version GE 22301) then begin
         kindof='--question --cancel-label="Cancel" --ok-label="Yes"'
      endif else begin
         kindof='--list --column="selection" "Cancel" "OK"'
      endelse
   endif else begin
      kindof='--error'
   endelse
endelse
;
if KEYWORD_SET(debug) then begin
   print, 'commande :', cmd
   print, 'option   :', kindof
endif
;
cmd+=kindof
;
; effective call to external Zenity
SPAWN, cmd, result, error, exit_status=exit_status
;
if KEYWORD_SET(debug) then begin
   print, 'Zenity result      : ', result
   print, 'Zenity error       : ', error
   print, 'Zenity exit status : ', exit_status
endif
;
reponse='Failed'

if ~KEYWORD_SET(question) AND ~KEYWORD_SET(cancel) then reponse='OK'
if ~KEYWORD_SET(question) AND KEYWORD_SET(cancel) then begin
   if (zenity_version GE 22301) then begin
      if (exit_status eq 0) then reponse='OK' else reponse='Cancel'
   endif else begin
      ;; because we needed to use the "--list"
      reponse=result
   endelse
endif
;
if KEYWORD_SET(question) then begin
   if ~KEYWORD_SET(cancel) then begin
      if (exit_status eq 0) then reponse='Yes' else reponse='No'
   endif else begin
      ;; this is directly the result from Zenity ! (result is is {Yes|Cancel|No})
      reponse=result
   endelse
endif
;
; due to strange logic maybe unexpected case remaining ?
if reponse EQ 'Failed' then begin
   MESSAGE,/continue, 'You triggered an unexpected case'
   MESSAGE,/continue, 'Please report the case (bug) to GDL team on sf.net !'
endif
;
if KEYWORD_SET(test) then STOP
;
return, reponse
;
end
;
; ------------------------------------------------------------
;
function DIALOG_MESSAGE, Message_Text, TITLE=title, CANCEL=cancel, $
                         ERROR=error, INFORMATION=information, QUESTION=question, $
                         DEFAULT_CANCEL=defaul_cancel, DEFAULT_NO=default_no, $
                         CENTER=center, DIALOG_PARENT=dialog_parent, $
                         DISPLAY_NAME=display_name, RESOURCE_NAME=resource_name, $
                         HELP=help, test=test, debug=debug, $
                         ZENITY_NAME=zenity_name, ZENITY_PATH=zenity_path, $
                         FORCE_ZENITY=force_zenity
on_error, 2
;
if KEYWORD_SET(help) then begin
    print, 'function DIALOG_MESSAGE, Message_Text, TITLE=title, CANCEL=cancel, $'
    print, '                         ERROR=error, INFORMATION=information, QUESTION=question, $'
    print, '                         DEFAULT_CANCEL=defaul_cancel, DEFAULT_NO=default_no, $'
    print, '                         CENTER=center, DIALOG_PARENT=dialog_parent, $'
    print, '                         DISPLAY_NAME=display_name, RESOURCE_NAME=resource_name, $'
    print, '                         HELP=help, test=test, debug=debug, $'
    print, '                         ZENITY_NAME=zenity_name, ZENITY_PATH=zenity_path, FORCE_ZENITY=force_zenity'
    return, -1
endif
;
if (N_params() NE 1) then MESSAGE, 'Incorrect number of arguments.'
;
wxwidget_available = WXWIDGETS_EXISTS()
;
if (wxwidget_available) and not(KEYWORD_SET(FORCE_ZENITY)) then begin
    return, DIALOG_MESSAGE_WXWIDGETS(Message_Text, TITLE=title, CANCEL=cancel, $
                                     ERROR=error, INFORMATION=information, QUESTION=question, $
                                     DEFAULT_CANCEL=defaul_cancel, DEFAULT_NO=default_no, $
                                     CENTER=center, DIALOG_PARENT=dialog_parent, $
                                     DISPLAY_NAME=display_name, RESOURCE_NAME=resource_name)
endif else begin
   return, DIALOG_MESSAGE_ZENITY(Message_Text, TITLE=title, CANCEL=cancel, $
                                 ERROR=error, INFORMATION=information, QUESTION=question, $
                                 DEFAULT_CANCEL=defaul_cancel, DEFAULT_NO=default_no, $
                                 CENTER=center, DIALOG_PARENT=dialog_parent, $
                                 DISPLAY_NAME=display_name, RESOURCE_NAME=resource_name, $
                                 HELP=help, test=test, debug=debug, $
                                 ZENITY_NAME=zenity_name, ZENITY_PATH=zenity_path)
endelse
;
end
;
