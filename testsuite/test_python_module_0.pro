; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
;
; assumes the following sequence of events:
; - ./configure
; - make 
; - ./configure --enable-python_module
; - make
; - make check

pro TEST_PYTHON_MODULE_0, no_exit=no_exit, $
                          help=help, test=test, verbose=verbose
;
if (STRlowCase(!version.os_family) EQ 'windows') then begin
   MESSAGE, /continue, 'Not ready for MSwin, please contribute (!fixme!)'
   EXIT, status=77
endif
;
suffix=''
if STRlowCase(!version.os) eq 'darwin' then suffix='dylib'
if STRlowCase(!version.os) eq 'linux' then suffix='so'
if STRLEN(suffix) EQ 0 then begin
   MESSAGE, /continue, 'Unknow OS, please contribute (!fixme!)'
   EXIT, status=77
endif
;
image = '../src/.libs/libgdl.' +suffix
;
if ~FILE_TEST(image) then begin
   MESSAGE, /continue, image + ' not found'
   EXIT, status=77
endif
;
if ~FILE_TEST('GDL.so') then SPAWN, 'ln -s ' + image + ' GDL.so'
;
end
