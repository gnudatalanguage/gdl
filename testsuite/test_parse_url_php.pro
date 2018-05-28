; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
; testing PARSE_URL routines (and GDLnetURL objects in future...)

; based on 
; - the test script for the PHP parse_url() function
;   by The PHP Group (PHP license v. 3.01)
;   (http://svn.php.net/viewvc/php/php-src/trunk/ext/standard/tests/strings/url_t.phpt)
;   PHP is free software available at http://www.php.net/software/
; - examples of IPv6 addresses from the RFC2732 (Format for Literal IPv6 Addresses in URL's)
;   (http://www.faqs.org/rfcs/rfc2732.html)
;
; ---------------------------------
; 
; Modifications history :
;
; - 2018-Feb-20 : AC. This code is in total disagrement with IDL
;   results ! We should fit with IDL !
;
; ---------------------------------
;
; IPv6 is not working in IDL ...
;
pro TEST_PARSE_URL_PHP_IPv6, cumul_errors, verbose=verbose, test=test
;
nb_errors=0
;                                
; IPv6 URLs examples from the RFC
;
samples_urls=[$
    'http://user:@pass@host/path?argument?value#etc', $
    'http://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:80/index.html', $
    'http://[1080:0:0:0:8:800:200C:417A]/index.html', $
    'http://[3ffe:2a00:100:7031::1]', $
    'http://[1080::8:800:200C:417A]/foo', $
    'http://[::192.9.5.5]/ipng', $
    'http://[::FFFF:129.144.52.38]:80/index.html', $
    'http://[2010:836B:4179::836B:4179]']
expected=[$
    '{ http user @pass host  path argument?value}', $
    '{ http   [FEDC:BA98:7654:3210:FEDC:BA98:7654:3210] 80 index.html }', $
    '{ http   [1080:0:0:0:8:800:200C:417A]  index.html }', $
    '{ http   [3ffe:2a00:100:7031::1]   }', $
    '{ http   [1080::8:800:200C:417A]  foo }', $
    '{ http   [::192.9.5.5]  ipng }', $
    '{ http   [::FFFF:129.144.52.38] 80 index.html }', $
    '{ http   [2010:836B:4179::836B:4179]   }']

;
for ii=0, N_elements(samples_urls)-1 do begin
   res=STRING(PARSE_URL_PHP(samples_urls[ii]),/print)
   if res NE expected[ii] then begin
      ERRORS_ADD, nb_errors, expected[ii]
      print, 'Case : ', ii
      print, 'expected  : ', expected[ii]
      print, 'processed : ', res
   endif
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_PARSE_URL_PHP_IPv6', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ---------------------------------
;
; basic tests on Port value ...
;
pro TEST_PARSE_URL_PHP_PORT, cumul_errors, verbose=verbose, test=test
;
nb_errors=0
;
samples_urls=['http://someserver.com/path/to/firstfile.dat', $
              'http://someserver.com:/path/to/firstfile.dat', $
              'http://someserver.com:1/path/to/firstfile.dat', $
              'http://someserver.com:1234/path/to/firstfile.dat', $
              'http://someserver.com:&&/path/to/firstfile.dat']
expected=['{ http   someserver.com 80 path/to/firstfile.dat }', $
          '{ http   someserver.com  path/to/firstfile.dat }', $
          '{ http   someserver.com 1 path/to/firstfile.dat }', $
          '{ http   someserver.com 1234 path/to/firstfile.dat }', $
          '{ http   someserver.com && path/to/firstfile.dat }']
;
for ii=0, N_elements(samples_urls)-1 do begin
   res=STRING(PARSE_URL_PHP(samples_urls[ii]),/print)
   if res NE expected[ii] then begin
      ERRORS_ADD, nb_errors, expected[ii]
      print, 'Case : ', ii
      print, 'expected  : ', expected[ii]
      print, 'processed : ', res
   endif
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_PARSE_URL_PORT', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ---------------------------------
;
pro TEST_PARSE_URL_PHP_GENERAL, cumul_errors, verbose=verbose, test=test
;
nb_errors=0
;
; TODO: testing of parse-errors:
; GDL> print, parse_url('a://:000000')
; % PARSE_URL: port cannot be longer then 5 characters
; GDL> print, parse_url('a://:00000')
; % PARSE_URL: invalid host
;
sample_urls = [ $
    ;; examples from PHP test script
    '', $             ; IDL 6.4 fails (syntax error)
    '64.246.30.37', $
    'http://64.246.30.37', $
    'http://64.246.30.37/', $
    '64.246.30.37/', $
    '64.246.30.37:80/', $
    'php.net', $
    'php.net/', $
    'http://php.net', $
    'http://php.net/', $
    'www.php.net', $
    'www.php.net/', $
    'http://www.php.net', $
    'http://www.php.net/', $
    'www.php.net:80', $
    'http://www.php.net:80', $
    'http://www.php.net:80/', $
    'http://www.php.net/index.php', $
    'www.php.net/?', $
    'www.php.net:80/?', $
    'http://www.php.net/?', $
    'http://www.php.net:80/?', $
    'http://www.php.net:80/index.php', $
    'http://www.php.net:80/foo/bar/index.php', $
    'http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php', $
    'http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php?lots=1&of=2&parameters=3&too=4&here=5', $
    'http://www.php.net:80/this/is/a/very/deep/directory/structure/and/', $
    'http://www.php.net:80/this/is/a/very/deep/directory/structure/and/file.php', $
    'http://www.php.net:80/this/../a/../deep/directory', $
    'http://www.php.net:80/this/../a/../deep/directory/', $
    'http://www.php.net:80/this/is/a/very/deep/directory/../file.php', $
    'http://www.php.net:80/index.php', $
    'http://www.php.net:80/index.php?', $
    'http://www.php.net:80/#foo', $
    'http://www.php.net:80/?#', $
    'http://www.php.net:80/?test=1', $
    'http://www.php.net/?test=1&', $
    'http://www.php.net:80/?&', $
    'http://www.php.net:80/index.php?test=1&', $
    'http://www.php.net/index.php?&', $
    'http://www.php.net:80/index.php?foo&', $
    'http://www.php.net/index.php?&foo', $
    'http://www.php.net:80/index.php?test=1&test2=char&test3=mixesCI', $
    'www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123', $
    'http://secret@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123', $
    'http://secret:@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123', $
    'http://:hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123', $
    'http://secret:hideout@www.php.net/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123', $
    'http://secret@hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123', $
    'http://secret:hid:out@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123', $
    'nntp://news.php.net', $
    'ftp://ftp.gnu.org/gnu/glic/glibc.tar.gz', $
    'zlib:http://foo@bar', $
    'zlib:filename.txt', $
    'zlib:/path/to/my/file/file.txt', $
    'foo://foo@bar', $
    'mailto:me@mydomain.com', $
    '/foo.php?a=b&c=d', $
    'foo.php?a=b&c=d', $
    'http://user:passwd@www.example.com:8080?bar=1&boom=0', $
    'file:///path/to/file', $
    'file://path/to/file', $
    'file:/path/to/file', $
    'http://1.2.3.4:/abc.asp?a=1&b=2', $
    'http://foo.com#bar', $
    'scheme:', $
    'foo+bar://baz@bang/bla', $
    'gg:9130731', $
    ; testing control-character warning
    'http://www.google.com/' + string(9b) $
  ]
  expected = [ $
    '{       }', $
    '{      64.246.30.37 }', $
    '{ http   64.246.30.37   }', $
    '{ http   64.246.30.37   }', $
    '{      64.246.30.37/ }', $
    '{    64.246.30.37 80  }', $
    '{      php.net }', $
    '{      php.net/ }', $
    '{ http   php.net   }', $
    '{ http   php.net   }', $
    '{      www.php.net }', $
    '{      www.php.net/ }', $
    '{ http   www.php.net   }', $
    '{ http   www.php.net   }', $
    '{    www.php.net 80  }', $
    '{ http   www.php.net 80  }', $
    '{ http   www.php.net 80  }', $
    '{ http   www.php.net  index.php }', $
    '{      www.php.net/ }', $
    '{    www.php.net 80  }', $
    '{ http   www.php.net   }', $
    '{ http   www.php.net 80  }', $
    '{ http   www.php.net 80 index.php }', $
    '{ http   www.php.net 80 foo/bar/index.php }', $
    '{ http   www.php.net 80 this/is/a/very/deep/directory/structure/and/file.php }', $
    '{ http   www.php.net 80 this/is/a/very/deep/directory/structure/and/file.php lots=1&of=2&parameters=3&too=4&here=5}', $
    '{ http   www.php.net 80 this/is/a/very/deep/directory/structure/and/ }', $
    '{ http   www.php.net 80 this/is/a/very/deep/directory/structure/and/file.php }', $
    '{ http   www.php.net 80 this/../a/../deep/directory }', $
    '{ http   www.php.net 80 this/../a/../deep/directory/ }', $
    '{ http   www.php.net 80 this/is/a/very/deep/directory/../file.php }', $
    '{ http   www.php.net 80 index.php }', $
    '{ http   www.php.net 80 index.php }', $
    '{ http   www.php.net 80  }', $
    '{ http   www.php.net 80  }', $
    '{ http   www.php.net 80  test=1}', $
    '{ http   www.php.net   test=1&}', $
    '{ http   www.php.net 80  &}', $
    '{ http   www.php.net 80 index.php test=1&}', $
    '{ http   www.php.net  index.php &}', $
    '{ http   www.php.net 80 index.php foo&}', $
    '{ http   www.php.net  index.php &foo}', $
    '{ http   www.php.net 80 index.php test=1&test2=char&test3=mixesCI}', $
    '{    www.php.net 80 index.php test=1&test2=char&test3=mixesCI}', $
    '{ http secret  www.php.net 80 index.php test=1&test2=char&test3=mixesCI}', $
    '{ http secret  www.php.net  index.php test=1&test2=char&test3=mixesCI}', $
    '{ http  hideout www.php.net 80 index.php test=1&test2=char&test3=mixesCI}', $
    '{ http secret hideout www.php.net  index.php test=1&test2=char&test3=mixesCI}', $
    '{ http secret@hideout  www.php.net 80 index.php test=1&test2=char&test3=mixesCI}', $
    '{ http secret hid:out www.php.net 80 index.php test=1&test2=char&test3=mixesCI}', $
    '{ nntp   news.php.net   }', $
    '{ ftp   ftp.gnu.org  /gnu/glic/glibc.tar.gz }', $
    '{ zlib     http://foo@bar }', $
    '{ zlib     filename.txt }', $
    '{ zlib     path/to/my/file/file.txt }', $
    '{ foo foo  bar   }', $
    '{ mailto     me@mydomain.com }', $
    '{      foo.php a=b&c=d}', $
    '{      foo.php a=b&c=d}', $
    '{ http user passwd www.example.com 8080  bar=1&boom=0}', $
    '{ file     path/to/file }', $
    '{ file   path  /to/file }', $
    '{ file     path/to/file }', $
    '{ http   1.2.3.4  abc.asp a=1&b=2}', $
    '{ http   foo.com   }', $
    '{ scheme      }', $ 
    '{ foo+bar baz  bang  bla }', $
    '{ gg     9130731 }', $
    '{ http   www.google.com  	 }' $
  ]
;
;if KEYWORD_SET(test) OR KEYWORD_SET(debug) then STOP
;
nb_errors=0
nb_sample_urls=N_ELEMENTS(sample_urls)
;
if KEYWORD_SET(debug) then STOP
;
for i = 0, nb_sample_urls-1 do begin
   ;;
   s = STRING(PARSE_URL_PHP(sample_urls[i]), /print)
   ;;
   ;; STRING(/PRINT) returns an array if the string length exceedes the number of columns in a terminal 
   if N_ELEMENTS(s) NE 1 then begin
     ss = ''
     for ii = 0, n_elements(s) - 1 do ss += s[ii]
     s = ss
   endif
   ;;
   if (s NE expected[i]) then begin
      MESSAGE, 'test failed for URL: ' + sample_urls[i], /continue
      MESSAGE, '  expected: ' + expected[i], /continue
      MESSAGE, '       got: ' + s, /continue
      ;;help, parse_url_php(sample_urls[i]), /stru
      ;;EXIT, status=1
      nb_errors=nb_errors+1
     ;; CONTINUE
   endif
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_PARSE_URL_PHP_GENERAL', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ------------------------------------------------
;
pro TEST_PARSE_URL_PHP, help=help, test=test, no_exit=no_exit
;
cumul_errors=0
;
TEST_PARSE_URL_PHP_IPv6, cumul_errors, test=test
TEST_PARSE_URL_PHP_PORT, cumul_errors, test=test
TEST_PARSE_URL_PHP_GENERAL, cumul_errors, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_PARSE_URL_PHP', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

