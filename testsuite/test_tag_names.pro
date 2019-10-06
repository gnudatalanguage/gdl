;
; Tomas Hillberg, 8 Sep. 2019. Under GNU GPL v2+
;
; Preliminatry test suite for function TAG_NAMES
;



; testing internal call within a procedure
pro TH_STRUCT::PROC1, expected, cumul_errors, test=test

    nb_errors = 0

    if( min(TAG_NAMES(self) eq expected ) ne 1 ) then ERRORS_ADD, nb_errors, 'TH_STRUCT::PROC1 : TAG_NAMES(self)'

    BANNER_FOR_TESTSUITE, 'TH_STRUCT::PROC1', nb_errors, /short
    ERRORS_CUMUL, cumul_errors, nb_errors
    
    if KEYWORD_set(test) then STOP

end

; testing internal call within a function
function TH_STRUCT::FUNC1, expected, cumul_errors, test=test
    
    nb_errors=0

    if( min(TAG_NAMES(self) eq expected ) ne 1 ) then ERRORS_ADD, nb_errors, 'TH_STRUCT::FUNC1 : TAG_NAMES(self)'

    BANNER_FOR_TESTSUITE, 'TH_STRUCT::FUNC1', nb_errors, /short
    ERRORS_CUMUL, cumul_errors, nb_errors
    
    if KEYWORD_set(test) then STOP
    
    return, 1

end


pro TEST_TAG_NAMES, help=help, verbose=verbose, short=short, $
                       debug=debug, test=test, no_exit=no_exit

    if KEYWORD_SET(help) then begin
        print, 'pro TEST_TAG_NAMES, help=help, verbose=verbose, short=short, $'
        print, '                       debug=debug, test=test, no_exit=no_exit'
        return
    endif

    cumul_errors = 0

    expected = ['FIRST','SECOND','THIRD']
    anon_struct = { first:1L,second:'a_string',third:1.234 }
    named_struct = { TH_STRUCT, first:1L,second:'a_string',third:1.234 }
    if( min(TAG_NAMES(anon_struct) eq expected ) ne 1 ) then ERRORS_ADD, cumul_errors, 'TAG_NAMES(anon_struct)'
    if( min(TAG_NAMES(anon_struct) eq expected ) ne 1 ) then ERRORS_ADD, cumul_errors, 'TAG_NAMES(named_struct)'

    my_obj = OBJ_NEW('TH_STRUCT')
    
    ; Note: the line below will fail on IDL since thy don't support tag-lookup from outside an object, for GDL it is valid though.
    ;if( min(TAG_NAMES(my_obj) eq expected ) ne 1 ) then ERRORS_ADD, cumul_errors, 'TAG_NAMES(obj_instance)'
    
    ; The below two lines are valid in IDL
    my_obj->PROC1, expected, cumul_errors, test=test
    dummy = my_obj->FUNC1( expected, cumul_errors, test=test )
    
    OBJ_DESTROY, my_obj


    BANNER_FOR_TESTSUITE, 'TEST_TAG_NAMES', cumul_errors, short=short

    if (cumul_errors NE 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1

    if KEYWORD_SET(test) then STOP

end
