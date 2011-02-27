$ tail *__define.pro
bottle = obj_new('beer')
print, 'bottle is a[n] ', obj_class(bottle)
spr = obj_class('beer', /superclass, count=cnt)
print, 'beer has ', strtrim(cnt,2) , ' direct superclass[es]: ', strjoin(spr, ',')
