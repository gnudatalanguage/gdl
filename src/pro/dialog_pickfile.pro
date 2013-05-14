;+
; NAME:   DIALOG_PICKFILE
;
; PURPOSE:
;
; This function try to reproduce the IDL's DIALOG_PICKFILE
; behavior using "zenity".
;
; zenity, under GNU GPL, is available on most Linux distributions 
; and also on OSX (tested) and MSwin (not tested ?) It is better to
; have zenity version >= 2.23.1.
;
; CATEGORY:
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
;	DISPLAY_NAME: X display
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
;	READ: Set this keyword to make the title of the dialog 
;             "Please Select a File|Directory for Reading". 
;	RESOURCE_NAME: X name
;	TITLE: Set this keyword to a scalar string to be used for the dialog title.
; If it is not specified, the default title is "Please Select a File|Directory". 
;	WRITE: Set this keyword to make the title of the dialog "Please Select a
; File|Directory for Writing". This keyword must be set to allow OVERWRITE_PROMPT. 
;
;	NON IDL KEYWORDS
;
;	ZENITY_NAME: Set this keyword to a scalar string used to overwrite Zenity
; executable name.
;	ZENITY_PATH: If set, Zenity will be searched in this path. If it doesn't 
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
; COMMON BLOCKS: none
;
; SIDE EFFECTS:
;
; RESTRICTIONS: 
;               - need Zenity v2.23.1 or higher to use filters.
;               - interface is not exactly the same ...
;
; PROCEDURE:  straithforward
;
; EXAMPLE:     files=DIALOG_PICKFILE(FILTER=['*.c','*.cpp'],/MULTIPLE_FILES)
;
; MODIFICATION HISTORY:
;
;  Idea by Alain Coulais on June 2010, implementation by Maxime Lenoir
;  (first public version 16 June 2010).
;
; 06-JUN-2011: - dates in ESO FITS files are using ":" as a separator,
;              in conflict with Zenity internal separator. Adding
;              a new parameter (ZENITY_SEP) and move to "|" as
;              default.
;              - if keyword DEBUG set to a number > 1 then the zenity command
;              is printed, if > 2 then we exit
;
; 14-NOV-2012: - large part of code, common with DIALOG_MESSAGE,
;                related to Zenity, moved into ZENITY_CHECK()
;
; 14-May-2013: - correcting "bug" 3612324: must start in current directory when
;              no path given. This problem appears due to change in
;              Zenity in Gnome3 (e.g. : http://www.kirsle.net/blog/kirsle/zenity-and-gnome-3)
;
;-
;
; This function try to reproduce the IDL's DIALOG_PICKFILE behavior using "zenity".
;
; zenity, under GNU GPL, is available in package on most Linux
; distributions (available and tested on CentOS 5.4, Mandriva 2010 and Ubuntu 9.04)
; Also on OSX: http://www.macports.org/ports.php?by=name&substr=zenity
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
; Copyright (C) 2010, Maxime Lenoir (main author) and Alain Coulais (idea, contact)
;
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
                          ZENITY_SEP=ZENITY_SEP, $
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
    print, '           ZENITY_SEP=ZENITY_SEP, $'
    print, '           HELP=help, test=test, debug=debug, verbose=verbose'
    return, ''
endif
;
zenity=ZENITY_CHECK( zenity_name=zenity_name,  zenity_path=zenity_path, $
                     zenity_version=zenity_version, $
                     help=help, test=test, debug=debug, verbose=verbose)
;
if (!zenity.version LT 0) then begin
   return, ''
endif
;
; Check default_extension
if KEYWORD_SET(default_extension) then default_extension=STRING(default_extension[0])
;
; Zenity file selection mode
cmd=zenity+' --file-selection '
get_path=''
;
; Only display directories
if KEYWORD_SET(directory) then begin
    cmd+='--directory '
    type='Directory'
endif else type='File'

; Dialog_parent can't be used w/ Zenity

; Set the X Window display
if KEYWORD_SET(display_name) then cmd+='--display="'+STRING(display_name)+'" '
if KEYWORD_SET(resource_name) then cmd+='--name="'+STRING(resource_name)+'" '

; Set the initial file selection. Since zenity can't distinguish initial path from initial selected file, 
; we have to concatenate path and file
; if path is set and exist, initial path = path
; if path and file are set, then initial = path/file if exists, path otherwise (or current working directory if invalid path)
; if file is set, initial = file
; Zenity can't initialy select a non-exising file/directory
;
; in gnome3, by default, Zenity (eg: 3.4.0 in Ubuntu 12.04)
; uses value storing in ~/.recently-used
; for the path. We must set it up now :(  
; (no side effects found up to now with older versions of Zenity)
;
start=''
if KEYWORD_SET(path) then begin
   path=STRING(path[0])
   start+=path+path_sep()
endif else begin
   CD, current=current
   path=current
   start+=current+path_sep()
end
;
if KEYWORD_SET(file) then file=STRING(file[0])
;
if start ne '' then begin
   if KEYWORD_SET(file) && FILE_TEST(start+file) then begin
      cmd+='--filename="'+start+file+'" ' 
   endif else begin
      if FILE_TEST(start) then cmd+='--filename="'+start+'" '
   endelse
endif
;
; Set the filters (Zenity version >= 2.23.1)
;
if KEYWORD_SET(filter) then begin
   if (zenity_version lt 22301) then begin
      MESSAGE, 'Zenity version need to be >= 2.23.1 to support filters', /cont
   endif else begin
      ;; Check if zenity ver < 2.23.1
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
         cmd+='--file-filter="*.*" '                               ; Add no-filter
      endif
   endelse
endif

; Can't perform fix_filter w/ Zenity
; Group (Dialog_parent) can't be used w/ Zenity

; Set multiple files option
;
if N_ELEMENTS(zenity_sep) EQ 0 then zenity_sep='|'
;
if KEYWORD_SET(multiple_files) then begin
   if KEYWORD_SET(directory) then begin
      MESSAGE, 'Selecting multiple directories is not supported.',/cont
   endif else begin
      cmd+='--multiple --separator="'+zenity_sep+'" '
   endelse
endif
;
; Must exist can't be used w/ Zenity, but if the selected file
; doesn't exist, it won't be returned (see below)
;
; Read KW
if KEYWORD_SET(read) && ~KEYWORD_SET(title) && ~KEYWORD_SET(write) then begin
   cmd+='--title="Please Select a '+type+' for Reading" '
endif
;
; Resource_name can't be used w/ Zenity
;
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
endif else begin
   if ~KEYWORD_SET(read) && ~KEYWORD_SET(write) then begin
      cmd+='--title="Please Select a '+type+'" '
   endif
endelse
;
if KEYWORD_SET(debug) then begin
   if debug GT 1 then MESSAGE,/continue, "Zenity command: "+cmd
   if debug GT 2 then return, ''
endif
;
; Call Zenity
SPAWN, cmd, result, exit_status=ex
if (ex ne 0) then return, ''
;
; Zenity is known to return string arrays if result is gt than some
; length ... (1024 on CentOS 5.5 x86_64) merge in one string
result=STRJOIN(TEMPORARY(result))
;
results=STRSPLIT(result, zenity_sep, /extract)
rsize=SIZE(results, /n_elements)
if (rsize eq 0) then return, ''

; Set the path
; If directory, get_path = the directory
; else get_path = result's directory
if KEYWORD_SET(directory) then begin
   get_path=results[0]+PATH_SEP()
endif else begin
   get_path=FILE_DIRNAME(results[0])+PATH_SEP()
endelse

; Must exist filter
if KEYWORD_SET(must_exist) then begin
    ix=WHERE(FILE_TEST(results), c)
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
        if FILE_TEST(results[i]) then begin ; check if it already exists
            SPAWN, zenity+' --question --title='+readtitle+'--text="'+results[i]+' already exists.\nDo you want to replace it ?"', exit_status=ex
            over[i]=~ex ; As Zenity can't pop-up question dialog, if user don't want to overwrite the file, just pop it from the list
        endif
    endfor
    ix=WHERE(over, c)         ; indexes of files which must be deleted
    if c eq 0 then return, '' ; if every files have to be deleted, return
    results=results[ix]    ; Delete file that shouldn't be overwritten
    rsize=SIZE(results, /n_elements)
    if rsize eq 0 then return, ''
endif

; default extension (simple behaviour, unlike IDL [no filter taken into account])
if KEYWORD_SET(default_extension) then begin
	w=WHERE(STRPOS(FILE_BASENAME(results),'.') eq -1)
	if w ne [-1] then results[w]+='.'+default_extension
endif

;
if KEYWORD_SET(debug) OR KEYWORD_SET(debug) then STOP
;
return, results
end
