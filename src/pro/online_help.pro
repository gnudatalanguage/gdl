;+
;
; NAME:   ONLINE_HELP
;
; PURPOSE: accessing the documentation, the general one or for a given
;          procedure of function (intrinsic or not)
;
; CATEGORY: documentation
;
; CALLING SEQUENCE:  ( ? or ONLINE_HELP ) or ( ?fft or ONLINE_HELP, 'fft')
;
; INPUTS: no mandatory ones
;
; OPTIONAL INPUTS: name of a procedure, function or code
;
; KEYWORD PARAMETERS: 
;   original ones: book=, context=, full_path=, title=
;   extensions : nopdf=nopdf, nohtml=nohtml, nokey=nokey, browser=browser, $
;                 path2pdf=path2pdf, path2key=path2key, link2html=link2htlm, $
;                 test=test, debug=debug, help=help, verbose=verbose 
;
; OUTPUTS: none
;
; OPTIONAL OUTPUTS: none
;
; COMMON BLOCKS: none
;
; SIDE EFFECTS: may or not succeed to start a WEB browser.
;
; RESTRICTIONS:
;
; 1/ except if a copy of the "GDL.pdf" is locally available
; and in the !path, an internet connection is mandatory ...
;
; 2/ the result is very sensitive to the version of the WEB browser
; and which plugings (and pluging versions) are available.
;
; PROCEDURE: straitforward
;
; EXAMPLE:  ONLINE_HELP, 'fft', browser='midori'
;
; MODIFICATION HISTORY:
; -- 01-March-2013: creation by Alain Coulais, 
; -- 18-April-2013: managing the book= keyword, with/out /full_path
; 
; LICENCE: This code is under GNU GPL v2 or later.
;
;
; Very preliminary concept. the goal is to link to internal pages of 
; the PDF file "gdl.pdf" (eventually downloaded if not found)
; (currently at: http://gnudatalanguage.sourceforge.net/gdl.pdf)
; and also starting online HTML doc.
;
; The PDF file is currently at: http://gnudatalanguage.sourceforge.net/gdl.pdf
; Following Adobe Documention, direct links to page, chapter shall be possible 
; http://partners.adobe.com/public/developer/en/acrobat/PDFOpenParameters.pdf
; We use now only the search option ralated to Acroread viewer.
; Up to now, no equivalent functions inside alternative PDF
; readers (evince, xpdf) but is is supposed to be OK with "pdf.js"
; pluging in Firefox https://github.com/mozilla/pdf.js/issues/1875
;
; We have to consider to have local HTML version of the documentation.
;
; It seems that recent Firefox browsers can be call passing --new-tab
; / --new-win
;
;-
pro ONLINE_HELP, name, book=book, context=context, full_path=full_path, title=title, $
                 nopdf=nopdf, nohtml=nohtml, nokey=nokey, browser=browser, $
                 path2pdf=path2pdf, path2key=path2key, link2html=link2htlm, $
                 test=test, debug=debug, help=help, verbose=verbose
;
ON_ERROR, 2
;
if ~KEYWORD_SET(test) then ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'pro ONLINE_HELP, name, book=book, context=context, full_path=full_path, title=title, $'
    print, '                 nopdf=nopdf, nohtml=nohtml, nokey=nokey, browser=browser, $'
    print, '                 path2pdf=path2pdf, path2key=path2key, link2html=link2htlm, $'
    print, '                 test=test, debug=debug, help=help, verbose=verbose'
    print, ''
    return
endif
;
if N_PARAMS() EQ 0 then name=''
;
if N_PARAMS() EQ 1 then name=STRCOMPRESS(name,/remove_all)
;
; do we have access to X11 ??
; (we may consider using Lynx (tested succesfully) but is it really
; useful ?)
;
status=EXECUTE('xy=GET_SCREEN_SIZE()')
if (status EQ 0) then begin
   MESSAGE, 'Since we are unable to connect to X Windows display, no ONLINE HELP'
endif
;
if KEYWORD_SET(context) then begin
    MESSAGE, /continue, 'This option (only MSwin) is not available'
endif
;
; setting a default browser if not provided
; this code was tested with konqueror, midori and firefox
;
if ~KEYWORD_SET(browser) then begin
   ;; classical default !
   browser='firefox'
   ;;
   ;; on some GNU/Linux systems, a BROWSER is defined ...
   default_browser=GETENV('BROWSER')
   if (STRLEN(default_browser) GT 0) then browser=default_browser
   ;;
   ;; on OSX, it seems to be better to use "open" but this is not
   ;; working over ssh -X connection ... (suggestion welcome !)
   ;;
   if (!version.os EQ 'darwin') then browser='open'
endif
;
; we check if the default or selected brower is in the path
;
SPAWN, 'which '+browser, ok, error
;
if (STRLEN(ok) EQ 0) then begin
    MESSAGE, /continue, 'WEB Browser not found : '+browser
    MESSAGE, 'Please provide the name (+path) to the browser you want to use'
endif
;
space=' '
background=' &'
;
if KEYWORD_SET(book) then begin
    ;;
    ;; when the document is a PDF file, we try to use a PDF viewer
    ;;
    idx_suffixe=STRPOS(book, '.', /reverse_search)
    ;; when the suffixe is not found we keed "browser"
    if (idx_suffixe GE 0) then begin
        suffixe=STRMID(book, idx_suffixe+1)
        if STRLOWCASE(suffixe) EQ 'pdf' then begin
            ;; don't hesitate to complete this list
            list_of_pdf_viewers=['xpdf','evince','acroread']
            ;; if we don't locate a PDF viewer, we will use the default (web) Brower
            for ii=0, N_ELEMENTS(list_of_pdf_viewers)-1 do begin
                SPAWN, 'which '+list_of_pdf_viewers[ii], ok, error
                if (STRLEN(ok) NE 0) then begin
                    browser=list_of_pdf_viewers[ii]
                    break
                endif
            endfor
        endif
    endif
    if ~KEYWORD_SET(full_path) then begin
        ;;message, /continue, 'ToDo : managing !HELP_PATH'
        DEFSYSV, '!HELP_PATH', exist=exist
        if exist then begin
            book=FILE_WHICH(!HELP_PATH,book)
            if (STRLEN(book) EQ 0) then MESSAGE, 'no file found, please check !HELP_PATH and book name'
        endif else begin
            ;; falling back to standard multi-tab help
            MESSAGE, /continue, '!HELP_PATH not set !'
        endelse
    endif
    ;; we have to check whether the file exist or not !!
    if FILE_TEST(book) then begin
        command=browser+space+book+background
        goto, execute_command
    endif
endif
;
; link to IDL exelis in-line documentation
;
link1=''
if ~KEYWORD_SET(nohtml) then begin
    if ~KEYWORD_SET(link2html) then link2html='http://www.exelisvis.com/docs/'
    suffixe='.html'
    ;;
    if STRLEN(name) GT 0 then begin
        link1=space+link2html+STRUPCASE(name)+suffixe
    endif else begin
        link1=space+link2html
    endelse
endif
;
; link to PDF 
; if not found in the !PATH, this file is downloaded the first time
;
link2=''
if ~KEYWORD_SET(nopdf) then begin
    path2pdf='http://gnudatalanguage.sourceforge.net/'
    local_pdf=FILE_WHICH(!path, 'gdl.pdf',/include_current_dir)
    ;;
    ;; if no "gdl.pdf" in the !Path, trying to download it
    if STRLEN(local_pdf) EQ 0 then begin
        script=''
        SPAWN, 'which wget', res
        if STRLEN(res) GT 0 then begin
            script='wget '
        endif else begin
            SPAWN, 'which curl', res
            if STRLEN(res) GT 0 then script='curl -O '
        endelse
        if (STRLEN(script) GT 0) then begin
            SPAWN, script+path2pdf+'gdl.pdf', ok, pb
        endif
        local_pdf=FILE_WHICH(!path, 'gdl.pdf',/include_current_dir)
    endif
    ;;
    if (STRLEN(local_pdf) GT 0) then begin
        if STRLEN(name) GT 0 then begin
            ;; activating the search capability inside PDF, 
            ;; worked on Acroread pluging
            ;; should worked withing 
            link2='file://'+FILE_EXPAND_PATH(local_pdf)+'#search="'+name+'"'
        endif else begin
            link2='file://'+FILE_EXPAND_PATH(local_pdf)
        endelse
    endif else begin
        MESSAGE, /continue, 'GDL pdf documentaion not found :('
    endelse
endif
;
link3=''
if ~KEYWORD_SET(nokey) then begin
    path2key='http://aramis.obspm.fr/~coulais/IDL_et_GDL/'
    ;;
    if (STRLEN(name) GT 0) then begin
       ;; is it a .PRO file ??
       pro_file=FILE_WHICH(name+'.pro')
       if STRLEN(pro_file) GT 0 then begin
          link3='file://'+pro_file+space
          link3=link3+path2key+'Matrice_IDLvsGDL.html#'+STRUPCASE(STRMID(name,0,1))
       endif else begin
          link3=path2key+'known_keywords.html#GDL_'+STRUPCASE(name)
       endelse
    endif else begin
       link3=path2key+'Matrice_IDLvsGDL.html'
    endelse
endif
;
; line by line the command used by browser
;
if keyword_set(verbose) then begin
    MESSAGE, /continue, 'link2html= : '+link2html
    MESSAGE, /continue, 'path2pdf = : '+path2pdf
    MESSAGE, /continue, 'path2key = : '+path2key
endif
;
command=browser+space+link1+space+link2+space+link3+background
;
execute_command:
;
if KEYWORD_SET(debug) then begin
    print, command
    STOP
endif
SPAWN, command
;
if KEYWORD_SET(test) then stop
;
end
