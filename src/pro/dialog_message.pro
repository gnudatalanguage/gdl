;+
; NAME:   DIALOG_MESSAGE
;
; PURPOSE:
;
; This function try to reproduce the IDL's DIALOG_MESSAGE
; behavior using "zenity".
;
; zenity, under GNU GPL, is available on most Linux distributions 
; and also on OSX
;
; CATEGORY:
;
;
; CALLING SEQUENCE:  resu_list=DIALOG_MESSAGE( Message_Text [, /CANCEL] [, /CENTER] [, /DEFAULT_CANCEL | , /DEFAULT_NO]
;                                              [,DIALOG_PARENT=widget_id] [, DISPLAY_NAME=string] [, /ERROR | ,
;                                              /INFORMATION | , /QUESTION] [, RESOURCE_NAME=string] [, TITLE=string] )
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
			
;
; OUTPUTS:
;
; OPTIONAL OUTPUTS:
;
;
;
; COMMON BLOCKS: none
;
; SIDE EFFECTS:
;               - due to Zenity's capacities, the function ingnores
;                 the keyword parameters: DEFAULT_CANCEL, DEFAULT_NO,
;                 DIALOG_PARENT, DISPLAY_NAME, RESOURCE_NAME, CENTER
;
; RESTRICTIONS: 
;               - need Zenity v2.23.1 or higher to use filters.
;               - interface is not exactly the same ...
;
; PROCEDURE:  straithforward
;
; EXAMPLE:
;		resu_list=DIALOG_MESSAGE('hola')
;
;
; MODIFICATION HISTORY:
;
;  Implemented by Pedro Corona (23 Febrary 2011) from DIALOG_PICKFILE
;  written by Maxime Lenoir.
;-
;
; This function try to reproduce the IDL's DIALOG_MESSAGE
; behavior using "zenity".
;
; zenity, under GNU GPL, is available in package on most Linux
; distributions (available and tested on CentOS 5.4, Mandriva 2010 and Ubuntu
; 9.04)
; Also on OSX: http://www.macports.org/ports.php?by=name&substr=zenity
;
; Since the implementation use Zenity as dialog messenger, 
; some functionalities are different from the original one
; but this function works well for general purpose.
;
; Make sure that you have Zenity in your PATH 
; You need to have Zenity v2.23.1 or higher to use filters. 
;
; If you try to get existing files w/ /WRITE and /OVERWRITE_PROMPT, 
; you'll be prompted when the main dialog get closed.
; Each cancelation leads to the removal of the associated file 
; from the returned file list.
;
; Default extension also applies to selected files 
; (and not only to file names typed into the dialog)
;
;-
; LICENCE:
; Copyright (C) 2011, Pedro Corona and Maxime Lenoir (main author)
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
; 
;-
function DIALOG_MESSAGE,  Message_Text, $
                          CANCEL=cancel, $
                          CENTER=center, $
                          DEFAULT_CANCEL=defaul_cancel, $
                          DEFAULT_NO=default_no, $
                          DIALOG_PARENT=dialog_parent, $
                          DISPLAY_NAME=display_name, $
                          ERROR=error, $
                          INFORMATION=information, $
                          QUESTION=question, $
                          RESOURCE_NAME=resuorce_name, $
                          TITLE=title, $
                          HELP=help, $
                          ZENITY_NAME=zenity_name, $
                          ZENITY_PATH=zenity_path

if KEYWORD_SET(help) then begin
    print, 'DIALOG_MESSAGE,  Message_Text, CANCEL=cancel, CENTER=center, $'
    print, '                 DEFAULT_CANCEL=defaul_cancel, DEFAULT_NO=default_no, $'
    print, '                 DIALOG_PARENT=dialog_parent, DISPLAY_NAME=display_name, $'
    print, '                 ERROR=error, INFORMATION=information, $'
    print, '                 QUESTION=question, RESOURCE_NAME=resuorce_name, $'
    print, '                 TITLE=title, HELP=help, ZENITY_NAME=zenity_name, $'
    print, '                 ZENITY_PATH=zenity_path'
    return, ''
endif

; name
if (N_ELEMENTS(zenity_name) EQ 0) then ZenityName='zenity' else ZenityName=zenity_name

; path
ZenityPath=''
alt_ZenityPath=GETENV('ZENITY_PATH')
if (N_ELEMENTS(zenity_path) GT 0) and (alt_ZenityPath NE '') then begin
    if ~STRCMP(zenity_path,alt_ZenityPath) then begin
        MESSAGE, /continue, 'You setup 2 different PATH to Zenity, please fix it !'
        return, ''
    endif
    ;; the two paths are the same ...
    ZenityPath=zenity_path
endif else begin
    ;; zero or one is provided ... if zero, no change to ZenityPath=''
    if (N_ELEMENTS(zenity_path) GT 0) then ZenityPath=zenity_path
    if (alt_ZenityPath NE '') then ZenityPath=alt_ZenityPath
endelse
;
if KEYWORD_SET(debug) then begin
    MESSAGE, /continue, 'Name of <<zenity>> : '+ZenityName
    MESSAGE, /continue, 'Path to <<zenity>> : '+ZenityPath
endif
;
if (ZenityPath eq '') then begin
    ;; No path provided, use shell variable PATH
    ZenityFullName=ZenityName
endif else begin
    ZenityFullName=ZenityPath+PATH_SEP()+ZenityName
endelse
;
; Check if zenity exists in current paths and which version we have
;
SPAWN, ZenityFullName+' --version', stdout, stderr, exit_status=exit_status
;
; Search with other PATHs; 
if (exit_status EQ 0) then begin
    zen=ZenityFullName
endif else begin
    if (ZenityPath EQ '') then begin ; No path provided (no ZENITY_PATH shell variable or GDL keyword)
        MESSAGE,/continue, 'No Zenity found in PATH, looking in alternate places'
    endif else begin
        MESSAGE,/continue, 'No Zenity found in the ZENITY_PATH you provide'
        MESSAGE,/continue, '($ZENITY_PATH) or keyword ZENITY_PATH=), looking in alternate places'
    endelse
    ;;
    paths=['/bin', '/usr/bin', '/usr/local/bin', '/opt/local/bin']+'/'
    list_zenity=FILE_SEARCH(paths+zenityName)
    ;;
    if N_ELEMENTS(list_zenity) GT 1 then begin
        MESSAGE, /continue, 'Multiple zenity found !'
        MESSAGE, /continue, 'Please select the good one using shell variables'
        MESSAGE, /continue, '($PATH or $ZENITY_PATH) or keyword ZENITY_PATH='
        return, ''
    endif
    if (list_zenity eq '') then begin
        MESSAGE, /continue, 'Zenity not found ! Zenity must be installed or in your PATH.'
        MESSAGE, /continue, 'Your current path is : '+GETENV('PATH')
        MESSAGE, /continue, 'You can give a path to Zenity with keyword ZENITY_PATH='
        MESSAGE, /continue, 'or using shell $ZENITY_PATH'
        if (STRLOWCASE(!version.OS) EQ 'darwin') then begin
            MESSAGE, /continue, ' '
            MESSAGE, /continue, 'How to install "zenity" on OSX ? Please have a look here:'
            MESSAGE, /continue, 'http://www.macports.org/ports.php?by=name&substr=zenity'
        endif
        return, ''
    endif
    ;; here we have one and only one no-null path-to-zenity !
    zen=list_zenity
endelse

cmd=zen+' --text="'+Message_Text+'" '

; TITLE
if KEYWORD_SET(title) then begin
	title=STRING(title[0])
	cmd+='--title="'+title+'" '
endif

;temporal string to store the kind of zenity in.
;default option INFORMATION
kindof='--info'

;the order in the next statments follows the order in IDL DIALOG_MESSAGE function.

; QUESTION
if KEYWORD_SET(question) && ~KEYWORD_SET(information) && ~KEYWORD_SET(error) then kindof='--question --cancel-label="No" --ok-label="OK"'

;CANCEL
if KEYWORD_SET(question) && KEYWORD_SET(cancel) && ~KEYWORD_SET(information) && ~KEYWORD_SET(error) then kindof='--question --cancel-label="Cancel" --ok-label="OK"'

; ERROR
if KEYWORD_SET(error) then kindof='--warning'

cmd+=kindof

; Call Zenity
SPAWN, cmd, result, errr, exit_status=ex
if errr ne '' then message, 'Zenity error: '+err
if (ex ne 0) && KEYWORD_SET(question) && ~KEYWORD_SET(cancel) then return, 'No'
if (ex ne 0) && KEYWORD_SET(question) && KEYWORD_SET(cancel) then return, 'Cancel'
if (ex eq 0) && KEYWORD_SET(question) then return, 'Yes'
if (ex eq 0) && ~KEYWORD_SET(question) then return, 'OK'
if (ex ne 0) && ~KEYWORD_SET(question) then return, 'Cancel'

end
