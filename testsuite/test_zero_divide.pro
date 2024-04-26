; will exercise all divide (and mod) basic operations with 0 generating an FPE.
; should crash if one operation is not protected against integer divide by zero.
PRO TEST_ZERO_DIVIDE
; initialisations: floats at end, since some commands do not accpet floats/doubles/complex
  typecodes=[1,2,3,12,13,14,15,4,5,6,9]
  all_numeric=10
  integers_only=6
  not_complex=8
; test all divide and mod basic functions (there are dozens of specializations) do not crash on integer divide
  simple_various_types=ptrarr(11,/allo)
k=0 & foreach i,typecodes do begin & *simple_various_types[k]=fix(1,type=i) & k++ &end
   ; DIV
   for k=0,integers_only do begin
      simple_var=*simple_various_types[k]
      multiple_var=replicate(simple_var,10)
      zero_var=simple_var*0
      zero_multiple=multiple_var*0
      multiple_var2=replicate(simple_var,20)
                                ; division by zero , no dimension
      b=simple_var/0  ; DivInvSNew
      b=simple_var/zero_var  ; DivInvSNew
      b=multiple_var/0  ; DivS 
      b=multiple_var/zero_var   ; DivS 
      b=multiple_var/zero_multiple  ; Div // e1->N_Elements() <= e2->N_Elements() : res= e1->Div(e2)
      b=multiple_var2/zero_multiple  ; DivInvNew different size
      v=simple_var
      v/=0  ; DivInvSNew
      v/=zero_var  ;DivInvSNew 
      v=multiple_var
      v/=0  ; DivS 
      v/=zero_multiple  ; DivNew
      v=multiple_var2
      v/=zero_multiple  ; DivInvNew 

      b=0/simple_var  ; DivInvS
      b=zero_var/simple_var  ; DivInvS
      b=0/multiple_var  ; DivInvS
      b=zero_var/multiple_var  ; DivInvS
      b=zero_multiple/multiple_var  ; DivInv // e1->N_Elements() > e2->N_Elements() : res= e2->DivInv(e1) 
      b=zero_multiple/multiple_var2  ; DivNew

   endfor
                                ; MOD
   for k=0,integers_only do begin
      simple_var=*simple_various_types[k]
      multiple_var=replicate(simple_var,10)
      zero_var=simple_var*0
      zero_multiple=multiple_var*0
      multiple_var2=replicate(simple_var,20)
                                ; division by zero , no dimension
      b=simple_var MOD 0  ; ModInvSNew
      b=simple_var MOD zero_var  ; ModInvSNew
      b=multiple_var MOD 0  ; ModS 
      b=multiple_var MOD zero_var   ; ModS 
      b=multiple_var MOD zero_multiple  ; Mod // e1->N_Elements() <= e2->N_Elements() : res= e1->Mod(e2)
      b=multiple_var2 MOD zero_multiple  ; ModInvNew different size
      v=simple_var
      v MOD=0  ; ModInvSNew
      v MOD=zero_var  ;ModInvSNew 
      v=multiple_var
      v MOD=0  ; ModS 
      v MOD=zero_multiple  ; ModNew
      v=multiple_var2
      v MOD=zero_multiple  ; ModInvNew 

      b=0 MOD simple_var  ; ModInvS
      b=zero_var MOD simple_var  ; ModInvS
      b=0 MOD multiple_var  ; ModInvS
      b=zero_var MOD multiple_var  ; ModInvS
      b=zero_multiple MOD multiple_var  ; ModInv // e1->N_Elements() > e2->N_Elements() : res= e2->ModInv(e1) 
      b=zero_multiple MOD multiple_var2  ; ModNew
   endfor
   
end
