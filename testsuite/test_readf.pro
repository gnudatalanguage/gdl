;
; Maxime Lenoir (contact also: Alain Coulais)
; Distributed version 2010/06/14
; Under GNU GPL V2 or later
;
; Purpose: Check READF procedure with different end-of-line characters (CR, LF, CRLF)
; @2010/06/14 : issue with lines ended with CR character
;
; --------------------------------------------
;
; due to changes by Alain, no more useful
;
; pro TESTREADF_ERR, type
; ;
; MESSAGE, /continue, '% TEST_READF: failed with '+type+' char'
; SPAWN, 'rm -f testreadf.txt'
; EXIT, status=1
; ;
; end
;
pro MINIREREADF, filename
;
if N_PARAMS() EQ 0 then begin
    MESSAGE, /continue, 'You mus provide a Filename !'
    return
endif
;
resu=FILE_INFO(filename)
if (resu.exists NE 1) then begin
    MESSAGE, /continue, 'File : '+filename+' not available'
    return
endif
;
line=''
;
OPENR, lun, /get_lun, filename
WHILE ~EOF(lun) do begin
    READF, lun, line
    print, line
endwhile
;
end
;
; --------------------------------------------
;
pro TESTREADF, verbose=verbose, type=type, no_erase=no_erase, errors=errors
;
if KEYWORD_SET(verbose) then begin
    print, '====='
    print, 'test'+type+':'
endif
case type of
    'CR': begin
        char='\r'
        suffixe='CR.txt'
    end
    'LF': begin
        char='\n'
        suffixe='LF.txt'
    end
    'CRLF': begin
        char='\r\n'
        suffixe='CRLF.txt'
    end
    else: begin
        MESSAGE, /continue, 'type={CR, LF, CRLF}'
        EXIT, status=1
    end
endcase
;
DEFSYSV, '!gdl', exists=is_it_gdl
if (is_it_gdl EQ 1) then soft='GDL' else soft='IDL'
;
filename='TestReadF_'+soft+'_'+suffixe
;
; generating the ASCII file
;
SPAWN, 'echo -e "testl1'+char+'testl2" > '+filename
;
; reading back the generated ASCII file
;
OPENR, fd, filename, /get_lun
str=''
;
; reading first line
;
READF, fd, str
if (str NE 'testl1') then begin
    MESSAGE, /continue, '% TEST_READF: failed with '+type+' char'
    errors=errors+1
endif
if KEYWORD_SET(verbose) then print, str
if EOF(fd) then begin
    MESSAGE, /continue, '% TEST_READF: failed with '+type+' char'
    errors=errors+1
endif else begin
    ;;
    ;; reading second line
    ;;
    READF, fd, str
    if (str NE 'testl2') then begin
        MESSAGE, /continue, '% TEST_READF: failed with '+type+' char'
        errors=errors+1
    endif
    if KEYWORD_SET(verbose) then print, str
endelse
;
CLOSE, fd
FREE_LUN, fd
;
if NOT(KEYWORD_SET(no_erase)) then SPAWN, 'rm -f '+filename
;
end
;
; --------------------------------------------
;
pro TEST_READF, verbose=verbose, no_erase=no_erase, help=help
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_READF, verbose=verbose, no_erase=no_erase, help=help'
    return
endif
;
if KEYWORD_SET(verbose) then begin
    print, 'Should print (for each pro):'
    print, 'testl1'
    print, 'testl2'
endif
;
errors=0
;
TESTREADF, verbose=verbose, no_erase=no_erase, errors=errors, type='LF'
TESTREADF, verbose=verbose, no_erase=no_erase, errors=errors, type='CRLF'
TESTREADF, verbose=verbose, no_erase=no_erase, errors=errors, type='CR'
;
if (errors GT 0) then EXIT, status=1
;
end
