;+
; NAME: FILE_EXPAND_PATH
; 
;-
; LICENCE:
; Copyright (C) 2010, Lea Noreskal, contact Alain Coulais
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;-
;
function FILE_EXPAND_PATH , path
return , FILE_SEARCH(path, /FULLY_QUALIFY_PATH)
end
;
