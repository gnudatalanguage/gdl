;; simple example file for using python's matplotlib package from GDL
;; the 

pro PY_PLOT,x,y,XLABEL=xlabel,YLABEL=ylabel,TITLE=title,GRID=grid

if N_ELEMENTS( xlabel) eq 1 then begin
    PYTHON,"matplotlib.matlab","xlabel",STRING(xlabel)
endif 
if N_ELEMENTS( ylabel) eq 1 then begin
    PYTHON,"matplotlib.matlab","ylabel",STRING(ylabel)
endif 
if N_ELEMENTS( title) eq 1 then begin
    PYTHON,"matplotlib.matlab","title",STRING(title)
endif 
if KEYWORD_SET( grid) then begin
    PYTHON,"matplotlib.matlab","grid",1
endif

PYTHON,"python_plot","py_plot",x,y

end
