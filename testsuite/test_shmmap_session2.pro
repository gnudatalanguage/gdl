;
; AC 2024/01/08
; This code is call by TEST_SHMMAP
;
SHMMAP,'anynamehere',/long,5
tmp2=SHMVAR('anynamehere')
print, 'session 2 before : ', tmp2
tmp2(0)=REVERSE(INDGEN(5))
print, 'session 2 after : ', tmp2
;
exit, status=0
;


