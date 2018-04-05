BEGIN {
  print "% auto-generated file - do not edit manually!"
  FS=","
}

$0 != "" { 
  gsub("_","-",$0)
  printf "\\gdlroutinedesc{" $1 "}{" $2 "}{" $3 "}{" 
  for (i = 4; i <= NF; i++) printf((i!=4?",":"")$i)
  printf "}\n"
}
