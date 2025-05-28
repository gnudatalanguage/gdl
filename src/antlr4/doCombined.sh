#!/bin/bash
if test $# -lt 1
 then
     echo "usage: $0 [interactive | translation_unit | interactive_compile ..."
     exit 1
fi
echo "grammar gdlCombined;" > gdlCombined.g4; for i in gdl*_include.g4; do grep -v 'grammar ' $i  >> gdlCombined.g4; done; echo "prog: $1 ;" >> gdlCombined.g4
