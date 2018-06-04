;By Sylvain Flinois
;
;The tests bellow are made to test the PARSE_URL function 
;
; The PARSE_URL function is based on IDL PARSE_URL
; You can find the IDL PARSE_URL function documentation at :
; http://www.harrisgeospatial.com/docs/PARSE_URL.html
;

pro TEST_PARSE_URL_BASIC, cumul_errors, verbose=verbose, test=test

nb_errors=0

;Basics tests

samples_urls=[$
    '',$
    '://',$
    '://@',$
    '://:@',$
    '://:',$
    ':///',$
    '://:/',$
    ':///:',$
    ':///?',$
    '://:@:/?',$
    'www.google.com',$
    'https:/www.google.com',$
    'https://www.google.com',$
    'https://user@www.google.com',$
    'https://user:password@www.google.com',$
    'https://www.google.com:8088',$
    'https://www.google.com/path',$ 
    'https://www.google.com/path?query=myquery',$
    'foo+bar://baz@bang/bla',$
    'http://www.example.com:port/path?query',$
    'http://www.example.com:port?query/path',$
    'http://www.example.com/path:port?query',$
    'http://www.example.com/path?query:port',$
    'http://www.example.com?query:port/path',$
    'http://www.example.com?query/path:port',$
	'http://example:port/path',$
	'http://example:port?query',$
	'http://example/path:port',$
	'http://example/path?query',$
	'http://example?query:port',$
	'http://example?query/path',$
    'http://www.google.com/' + string(9b)$
]

expected=[$
    '{     80  }',$
    '{     80  }',$
    '{     80  }',$
    '{     80  }',$
    '{       }',$
    '{     80  }',$
    '{       }',$
    '{    /  : }',$
    '{     80  }',$
    '{       }',$
    '{     80  }',$
    '{     80  }',$
    '{ https   www.google.com 80  }',$
    '{ https user  www.google.com 80  }',$
    '{ https user password www.google.com 80  }',$
    '{ https   www.google.com 8088  }',$
    '{ https   www.google.com 80 path }',$
    '{ https   www.google.com 80 path query=myquery}',$
    '{ foo+bar baz  bang 80 bla }',$
    '{ http   www.example.com port path query}',$
    '{ http   www.example.com port?query  query/path}',$
    '{ http   www.example.com/path  path:port query}',$
    '{ http   www.example.com/path?query  path?query:port }',$
    '{ http   www.example.com?query port path }',$
    '{ http   www.example.com?query/path  path:port }',$
    '{ http   example port path }',$
    '{ http   example port?query  query}',$
    '{ http   example/path  path:port }',$
    '{ http   example 80 path query}',$
    '{ http   example?query port  }',$
    '{ http   example?query 80 path }',$
    '{ http   www.google.com 80 	 }'$
]

for ii=0, N_elements(samples_urls)-1 do begin
    res=STRING(PARSE_URL(samples_urls[ii]),/print)
   if res NE expected[ii] then begin
      ERRORS_ADD, nb_errors, expected[ii]
      print, 'Case : ', samples_urls[ii]
      print, 'expected  : ', expected[ii]
      print, 'processed : ', res
   endif
endfor

BANNER_FOR_TESTSUITE, 'TEST_PARSE_URL_BASIC', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP

end

pro TEST_PARSE_URL, help=help, test=test, no_exit=no_exit
;
cumul_errors=0
;
TEST_PARSE_URL_BASIC, cumul_errors, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_PARSE_URL', cumul_errors
;
no_exit=1
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
