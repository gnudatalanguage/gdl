pro tvdelete,i0,i1,i2,i3,i4,i5,i6,i7,i8,i9
  case (n_params()) of
  0: WDELETE
  1: WDELETE,i0
  2: WDELETE,i0,i1
  3: WDELETE,i0,i1,i2
  4: WDELETE,i0,i1,i2,i3
  5: WDELETE,i0,i1,i2,i3,i4
  6: WDELETE,i0,i1,i2,i3,i4,i5
  7: WDELETE,i0,i1,i2,i3,i4,i5,i6
  8: WDELETE,i0,i1,i2,i3,i4,i5,i6,i7
  9: WDELETE,i0,i1,i2,i3,i4,i5,i6,i7,i8
  10: WDELETE,i0,i1,i2,i3,i4,i5,i6,i7,i8,i9
endcase
end  
