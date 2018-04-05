; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; part of GDL -- GNU Data Language

; procedure writing a list of GDL library routines into the 'outfile' (string filename)
; it covers both C++ and GDL-written routines, and includes a list of keywords and
; information on the number of arguments, as well as a flag if the routine is func or pro
pro routinelist, outfile
  ; get the list of GDL library routines written in C++
  lib_cpp = gdl('help, /lib')
  ; get the list of GDL library routines written in GDL
  lib_gdl = file_basename(file_search('../../src/pro/*.pro'), '.pro')
  ; allocate space for the big list
  n = n_elements(lib_cpp) - 3 + n_elements(lib_gdl) ; -3 : 'Library functions: ', 'Library procedures: ', '% At $MAIN$'
  liblist_name = strarr(n)
  liblist_isfn = bytarr(n)          ; bool, default means procedure
  liblist_args = intarr(n, /nozero) ; int in order to support -1
  liblist_kwrd = strarr(n)          ; comma-separated
  liblist_vrfy = strarr(n)          ; some diagnostics
  ; library routines written in C++
  k = 0
  for i = 1, n_elements(lib_cpp) - 2 do begin
    if strmid(lib_cpp[i], 1, 1) eq 'i' then continue ; e.g. 'Library functions: '
    liblist_vrfy[k] = lib_cpp[i]
    if strmid(lib_cpp[i], 3, 1) eq '=' then begin
      liblist_isfn[k] = 1
      lib_cpp[i] = strmid(lib_cpp[i], 4, strlen(lib_cpp[i]) - 5)
      tmp = lib_cpp[i]
      strput, tmp, ',', strpos(lib_cpp[i], '(') 
      lib_cpp[i] = temporary(tmp)
    endif
    tmp = strpos(lib_cpp[i], ',')
    liblist_name[k] = tmp eq -1 ? lib_cpp[i] : strmid(lib_cpp[i], 0, temporary(tmp))
    tmp = strpos(lib_cpp[i], '[')
    tmp = strmid(lib_cpp[i], tmp + 1, strpos(lib_cpp[i], ' ', tmp) - tmp)
    liblist_args[k] = tmp ne 'inf. ' ? fix(tmp) : -1
    tmp = (liblist_args[k] eq 0 ? strpos(lib_cpp[i], ',') + 1 : strpos(lib_cpp[i], ']') + 2)
    liblist_kwrd[k] = strmid(lib_cpp[i], tmp, strlen(lib_cpp[i]))
    k += 1
  endfor
  ; library routines written in GDL
  for i = 0, n_elements(lib_gdl) - 1 do begin
    j = i + k
    cmd = '.compile ../../src/pro/' + lib_gdl[i] + '.pro' + string(10b) + 'help, /fun & help, /pro'
    liblist_vrfy[j] = cmd
    tmp = gdl(cmd)
    isfun = -1b
    for l = 0, n_elements(tmp) - 1 do begin
      if strcmp(tmp[l], lib_gdl[i] + ' ', strlen(lib_gdl[i]) + 1, /fold_case) then break $
      else if strcmp(tmp[l], 'Compiled Functions:') then isfun = 1 $
      else if strcmp(tmp[l], 'Compiled Procedures:') then isfun = 0
    endfor
    liblist_isfn[j] = temporary(isfun)
    tmp = gdl(string('.compile ../../src/pro/', lib_gdl[i], '.pro', string(10b), $
      "info_", lib_gdl[i], " = routine_info('", lib_gdl[i], "', /param", $
      (liblist_isfn[j] ? ', /fun' : ''), ')', string(10b), $
      "print, 'liblist_args[" + string(j), "] = ', info_", lib_gdl[i], $
      ".NUM_ARGS", string(10b), "if info_", lib_gdl[i], $
      ".NUM_KW_ARGS gt 0 then print, string('liblist_kwrd[", string(j), "] = ''', ", $
      "strjoin(info_", lib_gdl[i], ".KW_ARGS, string(44b)), '''')"))
    for l = 0, n_elements(tmp) - 1 do if strcmp(tmp[l], 'liblist', 7) then begin
      ign = execute(tmp[l])
      if ign ne 1 then message, string('failed to execute: ', tmp[l])
    endif
    liblist_name[j] = STRUPCASE(lib_gdl[i])
  endfor

  srt = sort(liblist_name)
  openw, o, outfile, /get_lun
  for i = 0, n_elements(liblist_name) - 1 do begin
    s = srt[i]
    ; sorting keywords
    if liblist_kwrd[s] ne '' then begin
      tmp = strsplit(liblist_kwrd[s], ',', /extract)
      liblist_kwrd[s] = strjoin(tmp[sort(tmp)], ',')
    endif
    ; outputting everything to a file
    printf, o, string($
      liblist_name[s], ",", $
      string(strtrim(fix(liblist_isfn[s]), 2)), ",", $
      strtrim(string(liblist_args[s]), 2), (strlen(liblist_kwrd[s]) gt 0 ? "," : ""), $
      liblist_kwrd[s])
  endfor
  free_lun, o

end
