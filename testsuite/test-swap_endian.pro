; Script : test-swap_endian

 ; we need a way to know whether IDL or GDL is running...
 isGDL = !version.build_date eq 'Jul 07 2005'
 filename = (isGDL?'gdl':'idl'+strtrim(!version.release,1))+$
            '-test-swap_endian.txt'
 journal, filename
 
 print, ' 1--' & print, swap_endian(1b)
 print, ' 2--' & print, swap_endian(1)
 print, ' 3--' & print, swap_endian(1l)
 print, ' 4--' & print, swap_endian(1.)
 print, ' 5--' & print, swap_endian(1.d0)
 print, ' 6--' & print, swap_endian(complex(1, 1))
 print, ' 7--' & print, swap_endian("")
 print, ' 8--' & print, swap_endian(dcomplex(1, 1))
 print, ' 9--' & print, swap_endian(1us)
 print, '10--' & print, swap_endian(1ul)
 print, '11--' & print, swap_endian(1ll)
 print, '12--' & print, swap_endian(1ull)

 journal ; swap_if... keywords not implemented in idl 5.4
 
 print, '13--' & print, swap_endian(1b, /swap_if_big_endian)
 print, '14--' & print, swap_endian(1, /swap_if_big_endian)
 print, '15--' & print, swap_endian(1l, /swap_if_big_endian)
 print, '16--' & print, swap_endian(1., /swap_if_big_endian)
 print, '17--' & print, swap_endian(1.d0, /swap_if_big_endian)
 print, '18--' & print, swap_endian(complex(1, 1), /swap_if_big_endian)
 print, '19--' & print, swap_endian("", /swap_if_big_endian)
 print, '20--' & print, swap_endian(dcomplex(1, 1), /swap_if_big_endian)
 print, '21--' & print, swap_endian(1us, /swap_if_big_endian)
 print, '22--' & print, swap_endian(1ul, /swap_if_big_endian)
 print, '23--' & print, swap_endian(1ll, /swap_if_big_endian)
 print, '24--' & print, swap_endian(1ull, /swap_if_big_endian)

 print, '25--' & print, swap_endian(1b, /swap_if_little_endian)
 print, '26--' & print, swap_endian(1, /swap_if_little_endian)
 print, '27--' & print, swap_endian(1l, /swap_if_little_endian)
 print, '28--' & print, swap_endian(1., /swap_if_little_endian)
 print, '29--' & print, swap_endian(1.d0, /swap_if_little_endian)
 print, '30--' & print, swap_endian(complex(1, 1), /swap_if_little_endian)
 print, '31--' & print, swap_endian("", /swap_if_little_endian)
 print, '32--' & print, swap_endian(dcomplex(1, 1), /swap_if_little_endian)
 print, '33--' & print, swap_endian(1us, /swap_if_little_endian)
 print, '34--' & print, swap_endian(1ul, /swap_if_little_endian)
 print, '35--' & print, swap_endian(1ll, /swap_if_little_endian)
 print, '36--' & print, swap_endian(1ull, /swap_if_little_endian)

end
