;
; AC 26 Nov. 2014
;
; just verifying the logical operators (EQ and NE only)
; with !null are OK
;
; no time to finish now ...
;
function ADD_ERRORS, message
;
print, 'Error on operation : '+message
nb_errors=nb_errors+1
;
end
;
pro TEST_NULL
;
nb_errors=0
;
test1left=(a EQ !null)
if test1left NE 1 then ADD_ERRORS, nb_errors, 'a EQ !null'
test1right=(!null EQ a)
if test1right NE 1 then ADD_ERRORS, nb_errors, '!null EQ a'

test2eq=(!null EQ !null)
;if (test2eq NE 1) then
test2ne=(!null NE !null)
;
; not defined
;test2and=(!null and !null)
;test2or=(!null or !null)
;
expected=[0,0,1,1]
b=0
test3eqleft=(b EQ !null)
test3eqright=(!null EQ b)
test3neleft=(b NE !null)
test3neright=(!null NE b)
;
if nb_errors GT 0 then EXIT, status=1 else print, 'test ok !'
;
end
