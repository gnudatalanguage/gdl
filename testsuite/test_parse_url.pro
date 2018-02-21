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
pro TEST_URL, debug=debug
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
    ; IPv6 URLs examples from the RFC
    'http://user:@pass@host/path?argument?value#etc', $
    'http://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:80/index.html', $
    'http://[1080:0:0:0:8:800:200C:417A]/index.html', $
    'http://[3ffe:2a00:100:7031::1]', $
    'http://[1080::8:800:200C:417A]/foo', $
    'http://[::192.9.5.5]/ipng', $
    'http://[::FFFF:129.144.52.38]:80/index.html', $
    'http://[2010:836B:4179::836B:4179]', $
    ; testing control-character warning
    'http://www.google.com/' + string(9b) $
  ]
  expected = [ $
    '{       }', $
    '{      64.246.30.37 }', $
    '{ http   64.246.30.37   }', $
    '{ http   64.246.30.37  / }', $
    '{      64.246.30.37/ }', $
    '{    64.246.30.37 80 / }', $
    '{      php.net }', $
    '{      php.net/ }', $
    '{ http   php.net   }', $
    '{ http   php.net  / }', $
    '{      www.php.net }', $
    '{      www.php.net/ }', $
    '{ http   www.php.net   }', $
    '{ http   www.php.net  / }', $
    '{    www.php.net 80  }', $
    '{ http   www.php.net 80  }', $
    '{ http   www.php.net 80 / }', $
    '{ http   www.php.net  /index.php }', $
    '{      www.php.net/ }', $
    '{    www.php.net 80 / }', $
    '{ http   www.php.net  / }', $
    '{ http   www.php.net 80 / }', $
    '{ http   www.php.net 80 /index.php }', $
    '{ http   www.php.net 80 /foo/bar/index.php }', $
    '{ http   www.php.net 80 /this/is/a/very/deep/directory/structure/and/file.php }', $
    '{ http   www.php.net 80 /this/is/a/very/deep/directory/structure/and/file.php lots=1&of=2&parameters=3&too=4&here=5}', $
    '{ http   www.php.net 80 /this/is/a/very/deep/directory/structure/and/ }', $
    '{ http   www.php.net 80 /this/is/a/very/deep/directory/structure/and/file.php }', $
    '{ http   www.php.net 80 /this/../a/../deep/directory }', $
    '{ http   www.php.net 80 /this/../a/../deep/directory/ }', $
    '{ http   www.php.net 80 /this/is/a/very/deep/directory/../file.php }', $
    '{ http   www.php.net 80 /index.php }', $
    '{ http   www.php.net 80 /index.php }', $
    '{ http   www.php.net 80 / }', $
    '{ http   www.php.net 80 / }', $
    '{ http   www.php.net 80 / test=1}', $
    '{ http   www.php.net  / test=1&}', $
    '{ http   www.php.net 80 / &}', $
    '{ http   www.php.net 80 /index.php test=1&}', $
    '{ http   www.php.net  /index.php &}', $
    '{ http   www.php.net 80 /index.php foo&}', $
    '{ http   www.php.net  /index.php &foo}', $
    '{ http   www.php.net 80 /index.php test=1&test2=char&test3=mixesCI}', $
    '{    www.php.net 80 /index.php test=1&test2=char&test3=mixesCI}', $
    '{ http secret  www.php.net 80 /index.php test=1&test2=char&test3=mixesCI}', $
    '{ http secret  www.php.net  /index.php test=1&test2=char&test3=mixesCI}', $
    '{ http  hideout www.php.net 80 /index.php test=1&test2=char&test3=mixesCI}', $
    '{ http secret hideout www.php.net  /index.php test=1&test2=char&test3=mixesCI}', $
    '{ http secret@hideout  www.php.net 80 /index.php test=1&test2=char&test3=mixesCI}', $
    '{ http secret hid:out www.php.net 80 /index.php test=1&test2=char&test3=mixesCI}', $
    '{ nntp   news.php.net   }', $
    '{ ftp   ftp.gnu.org  /gnu/glic/glibc.tar.gz }', $
    '{ zlib     http://foo@bar }', $
    '{ zlib     filename.txt }', $
    '{ zlib     /path/to/my/file/file.txt }', $
    '{ foo foo  bar   }', $
    '{ mailto     me@mydomain.com }', $
    '{      /foo.php a=b&c=d}', $
    '{      foo.php a=b&c=d}', $
    '{ http user passwd www.example.com 8080  bar=1&boom=0}', $
    '{ file     /path/to/file }', $
    '{ file   path  /to/file }', $
    '{ file     /path/to/file }', $
    '{ http   1.2.3.4  /abc.asp a=1&b=2}', $
    '{ http   foo.com   }', $
    '{ scheme      }', $ 
    '{ foo+bar baz  bang  /bla }', $
    '{ gg     9130731 }', $
    '{ http user @pass host  /path argument?value}', $
    '{ http   [FEDC:BA98:7654:3210:FEDC:BA98:7654:3210] 80 /index.html }', $
    '{ http   [1080:0:0:0:8:800:200C:417A]  /index.html }', $
    '{ http   [3ffe:2a00:100:7031::1]   }', $
    '{ http   [1080::8:800:200C:417A]  /foo }', $
    '{ http   [::192.9.5.5]  /ipng }', $
    '{ http   [::FFFF:129.144.52.38] 80 /index.html }', $
    '{ http   [2010:836B:4179::836B:4179]   }', $
    '{ http   www.google.com  /	 }' $
  ]
;
if KEYWORD_SET(test) OR KEYWORD_SET(debug) then STOP
;
nb_errors=0
for i = 0, N_ELEMENTS(sample_urls) - 1 do begin
   s = STRING(PARSE_URL(sample_urls[i]), /print)

   ; STRING(/PRINT) returns an array if the string length exceedes the number of columns in a terminal 
   if N_ELEMENTS(s) NE 1 then begin
     ss = ''
     for ii = 0, n_elements(s) - 1 do ss += s[ii]
     s = ss
   endif

   if (s NE expected[i]) then begin
      MESSAGE, 'test failed for URL: ' + sample_urls[i], /continue
      MESSAGE, '  expected: ' + expected[i], /continue
      MESSAGE, '       got: ' + s, /continue
      ;;help, parse_url(sample_urls[i]), /stru
      ;;EXIT, status=1
      nb_errors=nb_errors+1
      CONTINUE
   endif
endfor
;
if (nb_errors GT 0) then begin
   print, 'Number of errors founded :', nb_errors
   EXIT, status=1
endif
;
if KEYWORD_SET(test) then STOP
;
end
