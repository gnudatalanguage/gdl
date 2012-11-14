;+
; NAME:    ZENITY_CHECK
;
; PURPOSE: return an effective name of working Zenity code and a
;          version number (important because useful keywords appeared
;          only after 2.23.1)
;
; CATEGORY:
;
; CALLING SEQUENCE:   zen_name=ZENITY_CHECK()
;
; INPUTS:          none
; OPTIONAL INPUTS: none
;
; KEYWORD PARAMETERS:
;
;
;
; OUTPUTS: a name
;
;
;
; OPTIONAL OUTPUTS: zenity_version= an u
;
;
;
; COMMON BLOCKS: none
;
; SIDE EFFECTS: none
;
; RESTRICTIONS: many !
;     1/ alternative codes may not provide equivalent keywords
;     2/ alternative codes will not have same versioning schema
;
; PROCEDURE: straitforward
;
; EXAMPLE:  zen=ZENITY_CHECK(zenity_path='/opt/local/bin/', zenity_version=zenity_version)
;
; MODIFICATION HISTORY:
;
; 14-Nov-2012, Alain Coulais : "creation": common part from
; DIALOG_PICKFILE and DIALOG_MESSAGE moved here
;
; the apparition of keywords are mentioned in NEWS file into Zenity package. 
; We used http://mcs.une.edu.au/doc/zenity-3.2.0/NEWS may be out of date ...
;
;-
; LICENCE:
; Copyright (C) 2010-2012, Maxime Lenoir (main author) and Alain Coulais (idea, contact)
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;
;-
function ZENITY_CHECK, zenity_name=zenity_name,  zenity_path=zenity_path, $
                       zenity_version=zenity_version, $
                       help=help, test=test, debug=debug, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'function ZENITY_CHECK, zenity_name=zenity_name,  zenity_path=zenity_path, $'
    print, '                       zenity_version=zenity_version, $'
    print, '                       help=help, test=test, debug=debug, verbose=verbose'
    return, -1
endif
;
; Alternative name can by provided. Maybe other equivalent codes may work
if (N_ELEMENTS(zenity_name) EQ 0) then ZenityName='zenity' else ZenityName=zenity_name
;
; path to program
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
    MESSAGE, /continue, 'Requested Name of <<zenity>> : '+ZenityName
    MESSAGE, /continue, 'Requested Path to <<zenity>> : '+ZenityPath
endif
;
if (ZenityPath eq '') then begin
    ;; No path provided, use shell variable PATH
    ZenityFullName=ZenityName
endif else begin
    ZenityFullName=ZenityPath+PATH_SEP()+ZenityName
endelse
;
; Check if zenity exists in current path and which version we have
;
SPAWN, ZenityFullName+' --version', stdout, stderr, exit_status=exit_status
;
; Search with other PATHs; 
if (exit_status EQ 0) then begin
    zen=ZenityFullName
    zen_version=stdout
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
    SPAWN, zen+' --version', stdout, stderr, exit_status=exit_status
    if (exit_status NE 0) then MESSAGE, 'not working Zenity selected ...'
    zen_version=stdout
endelse
;
; some useful options appeared only after release 2.23.1
;
version=STRSPLIT(zen_version, '.', /extract)
if N_ELEMENTS(version) EQ 1 then zenity_version=LONG(version[0])*10000
if N_ELEMENTS(version) EQ 2 then zenity_version=LONG(version[0])*10000+LONG(version[1])*100
if N_ELEMENTS(version) EQ 3 then zenity_version=LONG(version[0])*10000+LONG(version[1])*100+LONG(version[2])
if (zenity_version LT 22301) then begin
    MESSAGE, 'Zenity version need to be >= 2.23.1 to support filters', /cont
    MESSAGE, 'Since you have an old Zenity version, maybe the questions/buttons will not be adequate', /cont
endif
;
if KEYWORD_SET(debug) then begin
    MESSAGE, /continue, 'Effective Name of <<zenity>> : '+ZenityName
    MESSAGE, /continue, 'Effective Path to <<zenity>> : '+ZenityPath
    MESSAGE, /continue, 'Effective Version of <<zenity>> : '+Zen_version
endif
 
if KEYWORD_SET(test) then STOP
;
return, ZenityFullName
;
end

