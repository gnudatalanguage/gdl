FUNCTION tiff_read, Filename, R, G, B , _extra=extra
 case (n_params()) of
 1:RETURN, read_tiff( Filename, _extra=extra ) 
 4:RETURN, read_tiff( Filename, R, G, B, _extra=extra )
endcase
 
end
