;+
; NAME:   DIALOG_PICKFILE
;
; PURPOSE:
;
; This function try to reproduce the IDL's DIALOG_PICKFILE
; behavior using "zenity".
;
; zenity, under GNU GPL, is available on most Linux distributions 
; and also on OSX
;
; CATEGORY:
;
;
; CALLING SEQUENCE:  resu_list=DIALOG_PICKFILE()
;
; INPUTS: none
;
; OPTIONAL INPUTS: none
;
; KEYWORD PARAMETERS:
;	DEFAULT_EXTENSION: Set this keyword to a scalar string 
; representing the default extension to be appended onto the returned file name
; or names. If the returned file name already has an extension, 
; then the value set for this keyword is not appended. The string value set
; for this keyword should not include a period (.). 
;	DIALOG_PARENT: This keywork isn't supported. 
;	DIRECTORY: Set this keyword to display only the existing
; directories in the directory specified by the PATH keyword. If a path is not
; specified, setting this keyword shows directories in the current directory.
; The Browse for Folder dialog is opened to the directory specified by the path
; definition. Individual files are displayed but can't be selected.
; The return value contains the path of the directory selected, or
; directory created and selected by the user. 
;	DISPLAY_NAME: This keyword isn't supported
;	FILE: Set this keyword to a scalar string that contains the name
; of the initial file selection. This keyword is useful for specifying a default 
; filename.
;	FILTER: Set this keyword to a string value or an array of strings
; specifying the file types to be displayed in the file list. This keyword is
; used to reduce the number of files displayed in the file list. The user can
; modify the filter unless the FIX_FILTER keyword is set. If the value contains
; a vector of strings, multiple filters are used to filter the files.
; The filter *.* is automatically added to any filter you specify.
; Needs Zenity  >= 2.23.1
;	FIX_FILTER: This keyword isn't supported
;	GET_PATH: Set this keyword to a named variable in which the path of
; the selection is returned. 
;	GROUP: Ths keyword isn't supported
;	MULTIPLE_FILES: Set this keyword to allow for multiple file selection
; in the file-selection dialog. When you set this keyword, the user can select
; multiple files using the platform-specific selection method. The currently
; selected files appear in the selection text field of the dialog. 
; With this keyword set, DIALOG_PICKFILE can return a string array that contains
; the full path name of the selected file or files. 
;	MUST_EXIST: Set this keyword to allow only files that already exist
; to be returned
;	OVERWRITE_PROMPT: If this keyword is set along with the WRITE keyword
; and the user selects a file that already exists, then a dialog will be displayed
; asking if the user wants to replace the existing file or not.
; For multiple selections, the user is prompted separately for each file.
; If the user selects "No" the file won't be returned; if the user selects "Yes"
; then the selection is allowed. This keyword has no effect unless the WRITE
; keyword is also set. 
;	PATH: Set this keyword to a string that contains the initial path from
; which to select files. Relative paths are appended to current working directory.
; If this keyword is not set, current working directory is used.
;	READ: Set this keyword to make the title of the dialog "Please Select a File|Directory for Reading". 
;	RESOURCE_NAME: This keyword isn't supported
;	TITLE: Set this keyword to a scalar string to be used for the dialog title.
; If it is not specified, the default title is "Please Select a File|Directory". 
;	WRITE: Set this keyword to make the title of the dialog "Please Select a
; File|Directory for Writing". This keyword must be set to allow OVERWRITE_PROMPT. 
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
;
;
;
; RESTRICTIONS: 
;               - need Zenity v2.23.1 or higher to use filters.
;               - interface is not exactly the same ...
;
; PROCEDURE:  straithforward
;
; EXAMPLE:
;		files=DIALOG_PICKFILE(FILTER=['*.c','*.cpp'],/MULTIPLE_FILES)
;
;
; MODIFICATION HISTORY:
;
;  Idea by Alain Coulais on June 2010, implementation by Maxime Lenoir
;  (first public version 16 June 2010).
;
;-
;
; This function try to reproduce the IDL's DIALOG_PICKFILE
; behavior using "zenity".
;
; zenity, under GNU GPL, is available in package on most Linux
; distributions (available and tested on CentOS 5.4, Mandriva 2010 and Ubuntu
; 9.04)
; Also on OSX: http://zenity.darwinports.com/
;
; Since the implementation use Zenity as File Selector, 
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
; Copyright (C) 2010, Maxime Lenoir (main author) and Alain Coulais
; (idea, contact)
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
; 
;-
function DIALOG_PICKFILE, DEFAULT_EXTENSION=default_extension, $
                          DIRECTORY=directory, DIALOG_PARENT=dialog_parent, $
                          DISPLAY_NAME=display_name, FILE=file, FILTER=filter, $
                          FIX_FILTER=fix_filter, GET_PATH=get_path, GROUP=group, $
                          MULTIPLE_FILES=multiple_files, MUST_EXIST=must_exist, $
                          OVERWRITE_PROMPT=overwrite_prompt, PATH=path, $
                          READ=read, WRITE=write, RESOURCE_NAME=resource_name, $
                          TITLE=title, $
                          ZENITY_NAME=zenity_name, ZENITY_PATH=zenity_path, $
                          HELP=help, test=test, debug=debug, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'function DIALOG_PICKFILE, DEFAULT_EXTENSION=default_extension, $'
    print, '           DIRECTORY=directory, DIALOG_PARENT=dialog_parent, $'
    print, '           DISPLAY_NAME=display_name, FILE=file, FILTER=filter, $'
    print, '           FIX_FILTER=fix_filter, GET_PATH=get_path, GROUP=group, $'
    print, '           MULTIPLE_FILES=multiple_files, MUST_EXIST=must_exist, $'
    print, '           OVERWRITE_PROMPT=overwrite_prompt, PATH=path, '
    print, '           READ=read, WRITE=write, RESOURCE_NAME=resource_name, $'
    print, '           TITLE=title, '
    print, '           ZENITY_NAME=zenity_name, ZENITY_PATH=zenity_path, $'
    print, '           HELP=help, test=test, debug=debug, verbose=verbose'
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
            MESSAGE, /continue, 'http://zenity.darwinports.com/'
        endif
        return, ''
    endif
    ;; here we have one and only one no-null path-to-zenity !
    zen=list_zenity
endelse
;
; Check default_extension
if KEYWORD_SET(default_extension) then default_extension=STRING(default_extension[0])
;
; Zenith file selection mode
cmd=zen+' --file-selection '
get_path=''
;
; Only display directories
if KEYWORD_SET(directory) then begin
    cmd+='--directory '
    type='Directory'
endif else type='File'

; Dialog_parent can't be used w/ Zenity

; Set the X Window display
; if KEYWORD_SET(display_name) then cmd+='--display="'+STRING(display_name)+'" '

; Set the initial file selection. Since zenity can't distinguish initial path from initial selected file, 
; we have to concatenate path and file
; if path is set and exist, initial path = path
; if path and file are set, then initial = path/file if exists, path otherwise (or current working directory if invalid path)
; if file is set, initial = file
; Zenity can't initialy select a non-exising file/directory
start=''
if KEYWORD_SET(path) then begin
    path=STRING(path[0])
    start+=path+path_sep()
endif

if KEYWORD_SET(file) then file=STRING(file[0])

if start ne '' then $
  if KEYWORD_SET(file) && file_test(start+file) then cmd+='--filename="'+start+file+'" ' else $
  if file_test(start) then cmd+='--filename="'+start+'" '

; Set the filters (Zenity version >= 2.23.1)
if KEYWORD_SET(filter) then begin
    SPAWN, zen+' --version', ver ; Get current Zenith version
    version=STRSPLIT(ver, '\.', /extract)
    version=UINT(version[0])*10000+UINT(version[1])*100+UINT(version[2])
    if version lt 22301 then MESSAGE, 'Zenith version need to be >= 2.23.1 to support filters', /cont else begin ; Check if zenity ver < 2.23.1
        if SIZE(filter, /dimensions) eq 0 then filter=[filter] ; Filter is as scalar STRING
        filters=''
        fsize=SIZE(filter, /n_elements)
        if fsize gt 0 then begin
            ;; Transform filter in Zenity(GTK) syntax
            for i=0, fsize-1 do filter[i]=STRJOIN(STRSPLIT(STRING(filter[i]), ';', /extract), ' ')
            if fsize gt 1 then begin ; concatenate all filters
                for i=0, fsize-2 do begin
                    filters+=STRING(filter[i])+' '
                endfor
                filters+=STRING(filter[fsize-1])
                cmd+='--file-filter="'+filters+'" ' ; Add gobal filter
            endif
            for i=0, fsize-1 do cmd+='--file-filter="'+filter[i]+'" ' ; Add individual filters
            cmd+='--file-filter="*.*" ' ; Add no-filter
        endif
    endelse
endif

; Can't perform fix_filter w/ Zenity
; Group (Dialog_parent) can't be used w/ Zenity

; Set multiple files option
if KEYWORD_SET(multiple_files) then $
  if KEYWORD_SET(directory) then message,'Selecting multiple directories is not supported.',/cont else cmd+='--multiple --separator=":" '

; Must exist can't be used w/ Zenity, but if the selected file doesn't exist, it won't be returned (see below)

; Read KW
if KEYWORD_SET(read) && ~KEYWORD_SET(title) && ~KEYWORD_SET(write) then cmd+='--title="Please Select a '+type+' for Reading" '

; Resource_name can't be used w/ Zenity

; Write KW
if KEYWORD_SET(write) && ~KEYWORD_SET(title) && ~KEYWORD_SET(read) then begin
    readtitle='"Please Select a '+type+' for Writing" '
    cmd+='--title='+readtitle
endif

; R/W KW
if KEYWORD_SET(read) && KEYWORD_SET(write) && ~KEYWORD_SET(title) then begin
    readtitle='"Please Select a '+type+' for Reading for Writing" '
    cmd+='--title='+readtitle
endif

; Set window title
if KEYWORD_SET(title) then begin
    title=STRING(title[0])
    cmd+='--title="'+title+'" '
endif else $
  if ~KEYWORD_SET(read) && ~KEYWORD_SET(write) then cmd+='--title="Please Select a '+type+'" '

; Call Zenity
SPAWN, cmd, result, exit_status=ex
if (ex ne 0) then return, ''

results=STRSPLIT(result, ':', /extract)
rsize=SIZE(results, /n_elements)
if (rsize eq 0) then return, ''

; Set the path
; If directory, get_path = the directory
; else get_path = result's directory
if KEYWORD_SET(directory) then get_path=results[0]+PATH_SEP() else get_path=FILE_DIRNAME(results[0])+PATH_SEP()

; Must exist filter
if KEYWORD_SET(must_exist) then begin
    ix=where(file_test(results), c)
    if c eq 0 then return, ''   ; No file exist
    results=results[ix]
    rsize=SIZE(results, /n_elements)
    if rsize eq 0 then return, ''
endif

; Overwrite prompt
if KEYWORD_SET(overwrite_prompt) && KEYWORD_SET(write) then begin
    ;; Zenity can't be in save and multiselection mode at the same time
    ;; That's why save mode is disable and overwrite prompt is done when Zenity returns
    MESSAGE, 'For each cancelation, associated file will be deleted from the file list at return', /cont 
    over=BYTARR(rsize)
    for i=0, rsize-1 do begin   ; for each selected files
        if file_test(results[i]) then begin ; check if it already exists
            SPAWN, 'zenity --question --title='+readtitle+'--text="'+results[i]+' already exists.\nDo you want to replace it ?"', exit_status=ex
            over[i]=~ex ; As Zenity can't pop-up question dialog, if user don't want to overwrite the file, just pop it from the list
        endif
    endfor
    ix=where(over, c)         ; indexes of files which must be deleted
    if c eq 0 then return, '' ; if every files have to be deleted, return
    results=results[ix]    ; Delete file that shouldn't be overwritten
    rsize=SIZE(results, /n_elements)
    if rsize eq 0 then return, ''
endif

; default extension (simple behaviour, unlike IDL [no filter taken into account])
if KEYWORD_SET(default_extension) then $
  for i=0, rsize-1 do if STRPOS(file_basename(results[i]), '.') eq -1 then $
  results[i]+='.'+default_extension ; no extension, then append default extension
;
if KEYWORD_SET(debug) OR KEYWORD_SET(debug) then STOP
;
return, results
end
