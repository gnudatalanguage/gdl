BEGIN {
  FS=","
}

$0 != "" { 
  gsub("_","-",$0)
  filename = "routines/" tolower($1) ($2==1?".fun.":".pro.") "tex"
  print "if `test ! -e " filename "`; then touch " filename "; fi"
}
