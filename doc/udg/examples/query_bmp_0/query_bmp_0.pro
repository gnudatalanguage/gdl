$ wget --quiet http://wikipedia.org/favicon.ico
$ convert favicon.ico favicon.bmp
ok = query_bmp('favicon.bmp', info)
if ok then help, info, /structure else print, 'query failed!'
$ rm favicon.*
