;
; Tomas Hillberg, 28 Aug. 2019. Under GNU GPL v2+
;
; Preliminatry test suite for function ROUTINE_FILEPATH
;


; testing internal call within a procedure
pro TH_PRO1234, ref, cumul_errors, test=test

    name = ROUTINE_NAME()
    nb_errors = 0
    
    if ( ROUTINE_FILEPATH() NE ref ) then ERRORS_ADD, nb_errors, 'PROC with ROUTINE_FILEPATH()'
    if ( ROUTINE_FILEPATH(/EITHER) NE ref ) then ERRORS_ADD, nb_errors, 'PROC with ROUTINE_FILEPATH(/EITHER)'
    if ( ROUTINE_FILEPATH(/IS_FUNC) NE '' ) then ERRORS_ADD, nb_errors, 'PROC with ROUTINE_FILEPATH(/IS_FUNC)'

    BANNER_FOR_TESTSUITE, name, nb_errors, /short
    ERRORS_CUMUL, cumul_errors, nb_errors
    
    if KEYWORD_set(test) then STOP

end

; testing internal call within a function
function TH_FUNC1234, ref, cumul_errors, test=test

    name = ROUTINE_NAME()
    nb_errors=0

    if ( ROUTINE_FILEPATH(/IS_FUNC) NE ref ) then ERRORS_ADD, nb_errors, 'FUNC with ROUTINE_FILEPATH(/IS_FUNC)'
    if ( ROUTINE_FILEPATH(/EITHER) NE ref ) then ERRORS_ADD, nb_errors, 'FUNC with ROUTINE_FILEPATH(/EITHER)'
    if ( ROUTINE_FILEPATH() NE '' ) then ERRORS_ADD, nb_errors, 'FUNC with ROUTINE_FILEPATH()'

    BANNER_FOR_TESTSUITE, name, nb_errors, /short
    ERRORS_CUMUL, cumul_errors, nb_errors
    
    if KEYWORD_set(test) then STOP
    
    return, 1

end


; testing internal call within a procedure
pro TH_STRUCT::PROC1, ref, cumul_errors, test=test

    name = ROUTINE_NAME()
    nb_errors = 0

    if ( ROUTINE_FILEPATH() NE ref ) then ERRORS_ADD, nb_errors, 'STRUCT::PROC with ROUTINE_FILEPATH()'
    if ( ROUTINE_FILEPATH(/EITHER) NE ref ) then ERRORS_ADD, nb_errors, 'STRUCT::PROC with ROUTINE_FILEPATH(/EITHER)'
    if ( ROUTINE_FILEPATH(/IS_FUNC) NE '' ) then ERRORS_ADD, nb_errors, 'STRUCT::PROC with ROUTINE_FILEPATH(/IS_FUNC)'

    BANNER_FOR_TESTSUITE, name, nb_errors, /short
    ERRORS_CUMUL, cumul_errors, nb_errors
    
    if KEYWORD_set(test) then STOP

end

; testing internal call within a function
function TH_STRUCT::FUNC1, ref, cumul_errors, test=test
    
    name = ROUTINE_NAME()
    nb_errors=0

    if ( ROUTINE_FILEPATH(/IS_FUNC) NE ref ) then ERRORS_ADD, nb_errors, 'STRUCT::FUNC with ROUTINE_FILEPATH(/IS_FUNC)'
    if ( ROUTINE_FILEPATH(/EITHER) NE ref ) then ERRORS_ADD, nb_errors, 'STRUCT::FUNC with ROUTINE_FILEPATH(/EITHER)'
    if ( ROUTINE_FILEPATH() NE '' ) then ERRORS_ADD, nb_errors, 'STRUCT::FUNC with ROUTINE_FILEPATH()'

    BANNER_FOR_TESTSUITE, name, nb_errors, /short
    ERRORS_CUMUL, cumul_errors, nb_errors
    
    if KEYWORD_set(test) then STOP
    
    return, 1

end


pro TEST_ROUTINE_FILEPATH, help=help, verbose=verbose, short=short, $
                       debug=debug, test=test, no_exit=no_exit

    if KEYWORD_SET(help) then begin
        print, 'pro TEST_ROUTINE_FILEPATH, help=help, verbose=verbose, short=short, $'
        print, '                       debug=debug, test=test, no_exit=no_exit'
        return
    endif

    name = ROUTINE_NAME()
    cumul_errors = 0

    ref = ROUTINE_DIR() + 'test_routine_filepath.pro'
    
    ; simple check that the ref makes sense, i.e. we should be able to read "this" script
    if ( FILE_TEST( ref, /READ ) ne 1 ) then ERRORS_ADD, nb_errors, 'FILE_TEST(ref)'

    TH_PRO1234, ref, cumul_errors, test=test
    dummy = TH_FUNC1234( ref, cumul_errors, test=test )
    
    ; testing external calls for a procedure
    if ( ROUTINE_FILEPATH('TH_PRO1234') NE ref ) then ERRORS_ADD, nb_errors, 'ROUTINE_FILEPATH(proc)'
    if ( ROUTINE_FILEPATH('TH_PRO1234',/EITHER) NE ref ) then ERRORS_ADD, nb_errors, 'ROUTINE_FILEPATH(proc,/EITHER)'
    if ( ROUTINE_FILEPATH('TH_PRO1234',/IS_FUNC) NE '' ) then ERRORS_ADD, nb_errors, 'ROUTINE_FILEPATH(proc,/IS_FUNC)'

    ; testing external calls for a function
    if ( ROUTINE_FILEPATH('TH_FUNC1234',/IS_FUNC) NE ref ) then ERRORS_ADD, nb_errors, 'ROUTINE_FILEPATH(func,/IS_FUNC)'
    if ( ROUTINE_FILEPATH('TH_FUNC1234',/EITHER) NE ref ) then ERRORS_ADD, nb_errors, 'ROUTINE_FILEPATH(func,/EITHER)'
    if ( ROUTINE_FILEPATH('TH_FUNC1234') NE '' ) then ERRORS_ADD, nb_errors, 'ROUTINE_FILEPATH(func)'

    dummy = CREATE_STRUCT( NAME='TH_STRUCT', ['an_int'], 1 )
    th_obj = OBJ_NEW('TH_STRUCT')
    th_obj->PROC1, ref, cumul_errors, test=test
    dummy = th_obj->FUNC1( ref, cumul_errors, test=test )
    ; testing external calls for a procedure
    if ( ROUTINE_FILEPATH('TH_STRUCT::PROC1') NE ref ) then ERRORS_ADD, nb_errors, 'ROUTINE_FILEPATH(struct::proc)'
    if ( ROUTINE_FILEPATH('TH_STRUCT::PROC1',/EITHER) NE ref ) then ERRORS_ADD, nb_errors, 'ROUTINE_FILEPATH(struct::proc,/EITHER)'
    if ( ROUTINE_FILEPATH('TH_STRUCT::PROC1',/IS_FUNC) NE '' ) then ERRORS_ADD, nb_errors, 'ROUTINE_FILEPATH(struct::proc,/IS_FUNC)'
    ; testing external calls for a function
    if ( ROUTINE_FILEPATH('TH_STRUCT::FUNC1',/IS_FUNC) NE ref ) then ERRORS_ADD, nb_errors, 'ROUTINE_FILEPATH(struct::func,/IS_FUNC)'
    if ( ROUTINE_FILEPATH('TH_STRUCT::FUNC1',/EITHER) NE ref ) then ERRORS_ADD, nb_errors, 'ROUTINE_FILEPATH(struct::func,/EITHER)'
    if ( ROUTINE_FILEPATH('TH_STRUCT::FUNC1') NE '' ) then ERRORS_ADD, nb_errors, 'ROUTINE_FILEPATH(struct::func)'
    OBJ_DESTROY, th_obj

    BANNER_FOR_TESTSUITE, name, cumul_errors, short=short

    if (cumul_errors NE 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1

    if KEYWORD_SET(test) then STOP

end
