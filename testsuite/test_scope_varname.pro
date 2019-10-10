;
; Tomas Hillberg, 01 Sep. 2019. Under GNU GPL v2+
;
; Preliminatry test suite for function SCOPE_VARNAME
;
; TODO: There seem to be some discrepancies when routines are compiled, so that
; the order of the COMMON blocks might be different. For that reason, using
; SCOPE_VARNAME with the /COMMON keyword might output different results when
; compared to IDL. 
;

COMMON TH_COM1, gvar1, gvar2, gvar3


; testing internal calls within a procedure
pro TH_PROC1, proc1_arg, proc1_ce, test=test, key1=proc1_key, key2=proc1_unusedkey

    COMMON TH_COM1, p1gvar1, p1gvar2, p1gvar3
    p1gvar2=1
    
    expected = [ $
        'EXPECTED P1GVAR1 P1GVAR2 P1GVAR3 PROC1_ARG PROC1_CE PROC1_ERRORS PROC1_KEY PROC1_UNDEFINED PROC1_UNUSEDKEY TEST', $
        'CUMUL_ERRORS GVAR1 GVAR2 GVAR3 HELP NO_EXIT SHORT TEST TH_OBJ', $
        'PROC1_CE P1GVAR2 PROC1_UNUSEDKEY P1GVAR3 P1GVAR1 PROC1_UNDEFINED', $
        'CUMUL_ERRORS GVAR2  GVAR3 GVAR1 ', $
        '' $
    ]
    proc1_errors = 0
    
    ; List all variables in current scope
    if ( strjoin(SCOPE_VARNAME(count=p1gvar2),' ') ne expected[0] ) then begin
        ERRORS_ADD, proc1_errors, 'PROC with SCOPE_VARNAME()'
        print, 'PROC with SCOPE_VARNAME()   result: "' + strjoin(SCOPE_VARNAME(),' ') + '"'
        print, 'PROC with SCOPE_VARNAME() expected: "' + expected[0] + '"'
    endif
    if ( p1gvar2 ne 11 ) then begin
        ERRORS_ADD, proc1_errors, 'PROC with SCOPE_VARNAME() : '+string(p1gvar2)+' ne 11'
    endif
    
    ; List all variables in parent scope
    if ( strjoin(SCOPE_VARNAME(count=p1gvar2,level=-1),' ') ne expected[1] ) then begin $
        ERRORS_ADD, proc1_errors, 'PROC with SCOPE_VARNAME(level=-1)'
        print, 'PROC with SCOPE_VARNAME(level=-1)   result: "' + strjoin(SCOPE_VARNAME(level=-1),' ') + '"'
        print, 'PROC with SCOPE_VARNAME(level=-1) expected: "' + expected[1] + '"'
    endif
    if ( p1gvar2 ne 9 ) then begin
        ERRORS_ADD, proc1_errors, 'PROC with SCOPE_VARNAME(level=-1) : '+string(p1gvar2)+' ne 9'
    endif
    
    ; List some variables in current scope
    if ( strjoin(SCOPE_VARNAME( proc1_ce, proc1_arg, proc1_unusedkey, proc1_key, p1gvar1, proc1_undefined, count=p1gvar2 ),' ') ne $
            expected[2] ) then begin $
        ERRORS_ADD, proc1_errors, 'PROC with SCOPE_VARNAME({local_vars})'
        print, 'PROC with SCOPE_VARNAME( {local_vars} )   result: "'+strjoin(SCOPE_VARNAME(proc1_ce, proc1_arg, proc1_unusedkey, proc1_key, p1gvar1, proc1_undefined),' ')+'"'
        print, 'PROC with SCOPE_VARNAME( {local_vars} ) expected: "' + expected[2] + '"'
    endif
    if ( p1gvar2 ne 6 ) then begin
        ERRORS_ADD, proc1_errors, 'PROC with SCOPE_VARNAME({local_vars}) : '+string(p1gvar2)+' ne 6'
    endif
    
    ; List the corresponding name in the parent scope for a few variables in current scope
    if ( strjoin(SCOPE_VARNAME( proc1_ce, proc1_arg, proc1_unusedkey, proc1_key, p1gvar1, proc1_undefined, count=p1gvar2, level=-1 ),' ') ne $
            expected[3] ) then begin $
        ERRORS_ADD, proc1_errors, 'PROC with SCOPE_VARNAME({local_vars}, level=-1)'
        print, 'PROC with SCOPE_VARNAME( {local_vars}, level=-1 )   result: "'+strjoin(SCOPE_VARNAME(proc1_ce, proc1_arg, proc1_unusedkey, proc1_key, p1gvar1, proc1_undefined, level=-1),' ')+'"'
        print, 'PROC with SCOPE_VARNAME( {local_vars}, level=-1 ) expected: "' + expected[3] + '"'
    endif
    if ( p1gvar2 ne 6 ) then begin ; N.B. the empty strings returned are counted too
        ERRORS_ADD, proc1_errors, 'PROC with SCOPE_VARNAME({local_vars}, level=-1) : '+string(p1gvar2)+' ne 6'
    endif

    ; List unexisting name in the parent scope for a few variables (should return '')
    if ( strjoin(SCOPE_VARNAME( proc1_undefined, count=p1gvar2, level=-1 ),' ') ne  expected[4] ) then begin $
        ERRORS_ADD, proc1_errors, 'PROC with SCOPE_VARNAME(undefined, level=-1)'
        print, 'PROC with SCOPE_VARNAME( undefined, level=-1 )   result: "'+strjoin(SCOPE_VARNAME(proc1_undefined, count=p1gvar2, level=-1),' ')+'"'
        print, 'PROC with SCOPE_VARNAME( undefined, level=-1 ) expected: "' + expected[4] + '"'
    endif
    if ( p1gvar2 ne 1 ) then begin
        ERRORS_ADD, proc1_errors, 'PROC with SCOPE_VARNAME(undefined, level=-1) : '+string(p1gvar2)+' ne 1'
    endif

    BANNER_FOR_TESTSUITE, 'TH_PROC1', proc1_errors, /short
    ERRORS_CUMUL, proc1_ce, proc1_errors
    
    if KEYWORD_set(test) then STOP

end

; testing internal calls within a function
function TH_FUNC1, func1_arg, func1_ce, test=test, key1=func1_key, key2=func1_unusedkey

    COMMON TH_COM1, f1gvar1, f1gvar2, f1gvar3
    f1gvar2=2
    
    expected = [ $
        'EXPECTED F1GVAR1 F1GVAR2 F1GVAR3 FUNC1_ARG FUNC1_CE FUNC1_ERRORS FUNC1_KEY FUNC1_UNDEFINED FUNC1_UNUSEDKEY TEST', $
        'CUMUL_ERRORS GVAR1 GVAR2 GVAR3 HELP NO_EXIT SHORT TEST TH_OBJ', $
        'FUNC1_CE F1GVAR2 FUNC1_UNUSEDKEY F1GVAR3 F1GVAR1 FUNC1_UNDEFINED', $
        'CUMUL_ERRORS GVAR2  GVAR3 GVAR1 ', $
        '' $
    ]
    func1_errors = 0
    
    ; List all variables in current scope
    if ( strjoin(SCOPE_VARNAME(count=f1gvar2),' ') ne expected[0] ) then begin
        ERRORS_ADD, func1_errors, 'FUNC with SCOPE_VARNAME()'
        print, 'FUNC with SCOPE_VARNAME()   result: "' + strjoin(SCOPE_VARNAME(),' ') + '"'
        print, 'FUNC with SCOPE_VARNAME() expected: "' + expected[0] + '"'
    endif
    if ( f1gvar2 ne 11 ) then begin
        ERRORS_ADD, func1_errors, 'FUNC with SCOPE_VARNAME() : '+string(f1gvar2)+' ne 11'
    endif
    
    ; List all variables in parent scope
    if ( strjoin(SCOPE_VARNAME(count=f1gvar2, level=-1),' ') ne expected[1] ) then begin $
        ERRORS_ADD, func1_errors, 'FUNC with SCOPE_VARNAME(level=-1)'
        print, 'FUNC with SCOPE_VARNAME(level=-1)   result: "' + strjoin(SCOPE_VARNAME(level=-1),' ') + '"'
        print, 'FUNC with SCOPE_VARNAME(level=-1) expected: "' + expected[1] + '"'
    endif
    if ( f1gvar2 ne 9 ) then begin
        ERRORS_ADD, func1_errors, 'FUNC with SCOPE_VARNAME(level=-1) : '+string(f1gvar2)+' ne 9'
    endif
    
    ; List some variables in current scope
    if ( strjoin(SCOPE_VARNAME( func1_ce, func1_arg, func1_unusedkey, func1_key, f1gvar1, func1_undefined, count=f1gvar2 ),' ') ne $
            expected[2] ) then begin $
        ERRORS_ADD, func1_errors, 'FUNC with SCOPE_VARNAME({local_vars})'
        print, 'FUNC with SCOPE_VARNAME( {local_vars} )   result: "'+strjoin(SCOPE_VARNAME(func1_ce, func1_arg, func1_unusedkey, func1_key, f1gvar1, func1_undefined),' ')+'"'
        print, 'FUNC with SCOPE_VARNAME( {local_vars} ) expected: "' + expected[2] + '"'
    endif
    if ( f1gvar2 ne 6 ) then begin
        ERRORS_ADD, func1_errors, 'FUNC with SCOPE_VARNAME({local_vars}) : '+string(f1gvar2)+' ne 6'
    endif
    
    ; List the corresponding name in the parent scope for a few variables in current scope
    if ( strjoin(SCOPE_VARNAME( func1_ce, func1_arg, func1_unusedkey, func1_key, f1gvar1, func1_undefined, count=f1gvar2, level=-1 ),' ') ne $
            expected[3] ) then begin $
        ERRORS_ADD, func1_errors, 'FUNC with SCOPE_VARNAME({local_vars}, level=-1)'
        print, 'FUNC with SCOPE_VARNAME( {local_vars}, level=-1 )   result: "'+strjoin(SCOPE_VARNAME(func1_ce, func1_arg, func1_unusedkey, func1_key, f1gvar1, func1_undefined, level=-1),' ')+'"'
        print, 'FUNC with SCOPE_VARNAME( {local_vars}, level=-1 ) expected: "' + expected[3] + '"'
    endif
    if ( f1gvar2 ne 6 ) then begin ; N.B. the empty strings returned are counted too
        ERRORS_ADD, func1_errors, 'FUNC with SCOPE_VARNAME({local_vars}, level=-1) : '+string(f1gvar2)+' ne 6'
    endif

    ; List unexisting name in the parent scope for a few variables (should return '')
    if ( strjoin(SCOPE_VARNAME( func1_undefined, count=f1gvar2, level=-1 ),' ') ne  expected[4] ) then begin $
        ERRORS_ADD, func1_errors, 'FUNC with SCOPE_VARNAME(undefined, level=-1)'
        print, 'FUNC with SCOPE_VARNAME( undefined, level=-1 )   result: "'+strjoin(SCOPE_VARNAME(func1_undefined, level=-1),' ')+'"'
        print, 'FUNC with SCOPE_VARNAME( undefined, level=-1 ) expected: "' + expected[4] + '"'
    endif
    if ( f1gvar2 ne 1 ) then begin
        ERRORS_ADD, func1_errors, 'FUNC with SCOPE_VARNAME(undefined, level=-1) : '+string(f1gvar2)+' ne 1'
    endif

    BANNER_FOR_TESTSUITE, 'TH_FUNC1', func1_errors, /short
    ERRORS_CUMUL, func1_ce, func1_errors
    
    if KEYWORD_set(test) then STOP
    
    return, 1

end


pro TEST_SCOPE_VARNAME, help=help, short=short, test=test, no_exit=no_exit

    if KEYWORD_SET(help) then begin
        print, 'pro TEST_SCOPE_VARNAME, help=help, short=short, test=test, no_exit=no_exit'
        return
    endif

    COMMON TH_COM1, gvar1, gvar2, gvar3
    cumul_errors = 0
    
    ; create an object/struct just to verify that the name-resolution in SCOPE_VARNAME is OK.
    gvar1 = CREATE_STRUCT( NAME='TH_STRUCT', ['an_int'], 1 )
    th_obj = OBJ_NEW('TH_STRUCT')
    
    TH_PROC1, gvar2, cumul_errors, test=test, key1=gvar3
    gvar1 = TH_FUNC1( gvar2, cumul_errors, test=test, key1=gvar3 )

    OBJ_DESTROY, th_obj

    if ( strjoin(SCOPE_VARNAME( common='th_com1', count=gvar1, 0, 1, 2 ), ' ') ne 'GVAR1 GVAR2 GVAR3' ) then begin
        ERRORS_ADD, cumul_errors, 'SCOPE_VARNAME( common="th_com1", 0, 1, 2)'
        print, 'SCOPE_VARNAME( common="th_com1", 0, 1, 2)   result: "' + strjoin(SCOPE_VARNAME(common='th_com1', 0, 1, 2),' ') + '"'
        print, 'SCOPE_VARNAME( common="th_com1", 0, 1, 2) expected: "GVAR1 GVAR2 GVAR3"'
    endif
    if ( gvar1 ne 3 ) then begin
        ERRORS_ADD, cumul_errors, 'SCOPE_VARNAME( common="th_com1", 0, 1, 2) : '+string(gvar1)+' ne 3'
    endif

    BANNER_FOR_TESTSUITE, 'TEST_SCOPE_VARNAME', cumul_errors, short=short

    if (cumul_errors NE 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1

    if KEYWORD_SET(test) then STOP

end
