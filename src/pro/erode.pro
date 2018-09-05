;
;  GDL replacement to ERODE in IDL
;  Bin Wu <bin.wu (at) edinsights.no>
;  Aug. 2018
;
;  Arguments (from IDL): Image Structure X0 Y0 Z0
;  Keywords (from IDL): 
;	GRAY (a boolean keyword)
;		Set this keyword to perform grayscale, rather than binary, erosion. Nonzero 
;		elements of the Structure parameter determine the shape of the structuring element 
;		(neighborhood). If VALUES is not present, all elements of the structuring element 
;		are 0, yielding the neighborhood minimum operator.
;	PRESERVE_TYPE (a boolean keyword)
;		Set this keyword to return the same type as the input array. This keyword only 
;		applies if the GRAY keyword is set. 
;	UINT (a boolean keyword)
;		Set this keyword to return an unsigned integer array. This keyword only applies if 
;		the GRAY keyword is set.
;	ULONG (a boolean keyword)
;		Set this keyword to return an unsigned longword integer array. This keyword only 
;		applies if the GRAY keyword is set.
;	VALUES
;		An array of the same dimensions as Structure providing the values of the 
;		structuring element. The presence of this keyword implies grayscale erosion. Each 
;		pixel of the result is the minimum of Image less the corresponding elements of 
;		VALUE. If the resulting difference is less than zero, the return value will be 
;		zero.
;  Return (from IDL): the erosion of image
;  Syntax (from IDL): 
;	Result = ERODE( Image, Structure [, X0 [, Y0 [, Z0]]] [, /GRAY [, /PRESERVE_TYPE | , /UINT 
;		 | , /ULONG]] [, VALUES=array] )
;  Ref: http://www.harrisgeospatial.com/docs/ERODE.html
;
FUNCTION ERODE, Image, Structure, X0, Y0, Z0, $
                GRAY=GRAY, PRESERVE_TYPE=PRESERVE_TYPE, $
                UINT=UINT, ULONG=ULONG, VALUES=VALUES
; Return the caller of a procedure in the event of an error.
  ON_ERROR, 2
  
; At least Image and Structure are needed.
  IF (N_PARAMS() LE 1) THEN BEGIN
     MESSAGE, 'Incorrect number of arguments.'
  ENDIF
  
; Get the Structure size.
  dimStt = SIZE(Structure)
  
; Get the image size.
  dims = SIZE(Image)

; Make sure Structure has the same dimension as Image.
  IF (dims[0] NE dimStt[0]) THEN BEGIN
     CASE dimStt[0] OF
        1: BEGIN
           CASE dims[0] OF
              2: Structure = [[Structure],[REPLICATE(0,dimStt[1],1)]]
              3: Structure = [[[Structure],[REPLICATE(0,dimStt[1],1)]],[[REPLICATE(0,dimStt[1],2)]]]
              ELSE: PRINT, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
           ENDCASE
        END
        2: BEGIN
           CASE dims[0] OF
              1: Structure = Structure[*,0]
              3: Structure = [[[Structure]],[[REPLICATE(0,dimStt[1],dimStt[2])]]]
              ELSE: PRINT, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
           ENDCASE
        END
        3: BEGIN
           CASE dims[0] OF
              1: Structure = Structure[*,0,0]
              2: Structure = Structure[*,*,0]
              ELSE: PRINT, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
           ENDCASE
        END
        ELSE: PRINT, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
     ENDCASE
     dimStt=SIZE(Structure)
  ENDIF
  
; Get the coordinate of the origin of Structure.
  CASE dimStt[0] OF
     1: IF (N_ELEMENTS(X0) EQ 0) THEN X0 = dimStt[1]/2
     2: BEGIN
        IF (N_ELEMENTS(X0) EQ 0) THEN X0 = dimStt[1]/2
        IF (N_ELEMENTS(Y0) EQ 0) THEN Y0 = dimStt[2]/2
     END
     3: BEGIN
        IF (N_ELEMENTS(X0) EQ 0) THEN X0 = dimStt[1]/2
        IF (N_ELEMENTS(Y0) EQ 0) THEN Y0 = dimStt[2]/2
        IF (N_ELEMENTS(Z0) EQ 0) THEN Z0 = dimStt[3]/2
     END
     ELSE: PRINT, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
  ENDCASE
  
; Gray: a boolean keyword.
  IF (KEYWORD_SET(GRAY) OR KEYWORD_SET(VALUES)) THEN BEGIN
; Gray image;
;
;	MESSAGE, 'A gray image is in processing.'
;
; Make sure VALUES has the same dimension as Image.
     IF KEYWORD_SET(VALUES) THEN BEGIN
        dimVal = SIZE(VALUES)
        IF (dims[0] NE dimVal[0]) THEN BEGIN
           CASE dimVal[0] OF
              1: BEGIN
                 CASE dims[0] OF
                    2: VALUES = [[VALUES],[REPLICATE(0,dimVal[1],1)]]
                    3: VALUES = [[[VALUES],[REPLICATE(0,dimVal[1],1)]],[[REPLICATE(0,dimVal[1],2)]]]
                    ELSE: PRINT, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
                 ENDCASE
              END
              2: BEGIN
                 CASE dims[0] OF
                    1: VALUES = VALUES[*,0]
                    3: VALUES = [[[VALUES]],[[REPLICATE(0,dimVal[1],dimVal[2])]]]
                    ELSE: PRINT, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
                 ENDCASE
              END
              3: BEGIN
                 CASE dims[0] OF
                    1: VALUES = VALUES[*,0,0]
                    2: VALUES = VALUES[*,*,0]
                    ELSE: PRINT, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
                 ENDCASE
              END
              ELSE: PRINT, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
           ENDCASE
           dimVal=SIZE(VALUES)
        ENDIF
        CASE dimVal[0] OF
           1: BEGIN
              IF dimVal[1] LT dimStt[1] THEN VALUES = [VALUES,REPLICATE(0,dimStt[1] - dimVal[1],1)]
              IF dimVal[1] GT dimStt[1] THEN VALUES = VALUES[0:dimStt[1]-1,*]
           END
           2: BEGIN
              IF dimVal[1] LT dimStt[1] THEN VALUES = [VALUES,REPLICATE(0,dimStt[1] - dimVal[1],dimVal[2])]
              IF dimVal[1] GT dimStt[1] THEN VALUES = VALUES[0:dimStt[1]-1,*]
              dimVal = SIZE(VALUES)
              IF dimVal[2] LT dimStt[2] THEN VALUES = [[VALUES],[REPLICATE(0,dimVal[1],dimStt[2] - dimVal[2])]]
              IF dimVal[2] GT dimStt[2] THEN VALUES = VALUES[*,0:dimStt[2]-1]
           END
           3: BEGIN
              IF dimVal[1] LT dimStt[1] THEN VALUES = [VALUES,REPLICATE(0,dimStt[1] - dimVal[1],dimVal[2],dimVal[3])]
              IF dimVal[1] GT dimStt[1] THEN VALUES = VALUES[0:dimStt[1]-1,*]
              dimVal = SIZE(VALUES)
              IF dimVal[2] LT dimStt[2] THEN VALUES = [[VALUES],[REPLICATE(0,dimVal[1],dimStt[2] - dimVal[2],dimVal[3])]]
              IF dimVal[2] GT dimStt[2] THEN VALUES = VALUES[*,0:dimStt[2]-1,*]
              dimVal = SIZE(VALUES)
              IF dimVal[3] LT dimStt[3] THEN VALUES = [[[VALUES]],[[REPLICATE(0,dimVal[1],dimVal[2],dimStt[3] - dimVal[3])]]]
              IF dimVal[3] GT dimStt[3] THEN VALUES = VALUES[*,*,0:dimStt[3]-1]
           END
           ELSE: PRINT, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
        ENDCASE
     ENDIF
;
; In case VALUES are not given, as the image is Gray,
; set VALUES to be of the same size as Structure
; with all the elements to be 0.
     IF (KEYWORD_SET(VALUES) EQ 0) THEN BEGIN
        CASE dimStt[0] OF
           1: VALUES = REPLICATE(0, dimStt[1])
           2: VALUES = REPLICATE(0, dimStt[1], dimStt[2])
           3: VALUES = REPLICATE(0, dimStt[1], dimStt[2], dimStt[3])
           ELSE: BEGIN
              MESSAGE, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
           END
        ENDCASE	
     ENDIF
;
     erodeImg = Image
;
; An auxiliary variable.
     tmpImg = erodeImg
; An auxiliary PARAMETER.
     tmpMin = 0
; An auxiliary PARAMETER.
     tmpMax = MAX(erodeImg)
; 1D-3D.
     CASE dimStt[0] OF
        1: BEGIN
           FOR I = 0, dims[1]-1 DO BEGIN
              IF ((I - X0) GE 0) AND ((I + dimStt[1]-1 - X0) LE dims[1]-1) THEN BEGIN
                 tmpImg[I] = tmpMax
                 FOR II = (I - X0),(I + dimStt[1]-1 - X0) DO BEGIN
                    IF (Structure[II - I + X0] EQ 1) THEN $
                       tmpImg[I] = MAX([0,MIN([tmpImg[I], erodeImg[II] - VALUES[II - I + X0]])])
                 ENDFOR
              ENDIF ELSE BEGIN
                 tmpImg[I] = tmpMin
              ENDELSE
           ENDFOR
        END
        2: BEGIN
           FOR I = 0, dims[1]-1 DO BEGIN
              FOR J = 0, dims[2]-1 DO BEGIN
                 IF ((I - X0) GE 0) AND ((I + dimStt[1]-1 - X0) LE dims[1]-1) AND $
                    ((J - Y0) GE 0) AND ((J + dimStt[2]-1 - Y0) LE dims[2]-1) THEN BEGIN
                    tmpImg[I,J] = tmpMax
                    FOR II = (I - X0),(I + dimStt[1]-1 - X0) DO BEGIN
                       FOR  JJ = (J - Y0),(J + dimStt[2]-1 - Y0) DO BEGIN
                          IF (Structure[II - I + X0,JJ - J + Y0] EQ 1) THEN $
                             tmpImg[I,J] =  MAX([0, MIN([tmpImg[I,J], erodeImg[II,JJ] - VALUES[II - I + X0,JJ - J + Y0]])])
                       ENDFOR
                    ENDFOR
                 ENDIF ELSE BEGIN
                    tmpImg[I,J] = tmpMin
                 ENDELSE
              ENDFOR
           ENDFOR
        END
        3: BEGIN
           FOR I = 0, dims[1]-1 DO BEGIN
              FOR J = 0, dims[2]-1 DO BEGIN
                 FOR K = 0, dims[3]-1 DO BEGIN
                    IF ((I - X0) GE 0) AND ((I + dimStt[1]-1 - X0) LE dims[1]-1) AND $
                       ((J - Y0) GE 0) AND ((J + dimStt[2]-1 - Y0) LE dims[2]-1) AND $
                       ((K - Z0) GE 0) AND ((K + dimStt[3]-1 - Z0) LE dims[3]-1) THEN BEGIN
                       tmpImg[I,J,K] = tmpMax
                       FOR II = (I - X0),(I + dimStt[1]-1 - X0) DO BEGIN
                          FOR JJ = (J - Y0),(J + dimStt[2]-1 - Y0) DO BEGIN
                             FOR KK = (K - Z0),(K + dimStt[3]-1 - Z0) DO BEGIN
                                IF (Structure[II - I + X0,JJ - J + Y0,KK - K + Z0] EQ 1) THEN $
                                   tmpImg[I,J,K] = $
                                   MAX([0, MIN([tmpImg[I,J,K], erodeImg[II,JJ,KK] - $
                                                VALUES[II - I + X0,JJ - J + Y0,KK - K + Z0]])])
                             ENDFOR
                          ENDFOR
                       ENDFOR
                    ENDIF ELSE BEGIN
                       tmpImg[I,J,K] = tmpMin
                    ENDELSE
                 ENDFOR
              ENDFOR
           ENDFOR
        END
        ELSE: BEGIN
           MESSAGE, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
        END
     ENDCASE	
     erodeImg = tmpImg
  ENDIF ELSE BEGIN
; Binary image otherwise.
     erodeImg = Image GT 0
;
; An auxiliary variable.
     tmpImg = erodeImg
;
; 1D-3D.
     CASE dimStt[0] OF
        1: BEGIN
           FOR I = 0, dims[1]-1 DO BEGIN
              IF (erodeImg[I] EQ 1) AND ((I - X0) GE 0) AND ((I + dimStt[1]-1 - X0) LE dims[1]-1) THEN BEGIN
               idtStt = 1
               FOR II = (I - X0),(I + dimStt[1]-1 - X0) DO BEGIN
                  IF (Structure[II - I + X0] EQ 1) THEN idtStt = idtStt AND erodeImg[II]
               ENDFOR
               IF idtStt NE 1 THEN BEGIN
                  tmpImg[I] = 0
               ENDIF
            ENDIF ELSE BEGIN
               tmpImg[I] = 0
            ENDELSE
         ENDFOR
        END
        2: BEGIN
           FOR I = 0, dims[1]-1 DO BEGIN
              FOR J = 0, dims[2]-1 DO BEGIN
                 IF (erodeImg[I,J] EQ 1) AND ((I - X0) GE 0) AND ((I + dimStt[1]-1 - X0) LE dims[1]-1) $
                    AND ((J - Y0) GE 0) AND ((J + dimStt[2]-1 - Y0) LE dims[2]-1) THEN BEGIN
                    idtStt = 1
                    FOR II = (I - X0),(I + dimStt[1]-1 - X0) DO BEGIN
                       FOR JJ = (J - Y0),(J + dimStt[2]-1 - Y0) DO BEGIN
                          IF (Structure[II - I + X0,JJ - J + Y0] EQ 1) THEN idtStt = (idtStt AND erodeImg[II,JJ])
                       ENDFOR
                    ENDFOR
                    IF idtStt NE 1 THEN BEGIN
                       tmpImg[I,J] = 0
                    ENDIF
                 ENDIF ELSE BEGIN
                    tmpImg[I,J] = 0
                 ENDELSE
              ENDFOR
           ENDFOR
        END
        3: BEGIN
           FOR I = 0, dims[1]-1 DO BEGIN
              FOR J = 0, dims[2]-1 DO BEGIN
                 FOR K = 0, dims[3]-1 DO BEGIN
                    IF (erodeImg[I,J,K] EQ 1) AND ((I - X0) GE 0) AND ((I + dimStt[1]-1 - X0) LE dims[1]-1) $
                       AND ((J - Y0) GE 0) AND ((J + dimStt[2]-1 - Y0) LE dims[2]-1) $
                       AND ((K - Z0) GE 0) AND ((K + dimStt[3]-1 - Z0) LE dims[3]-1) THEN BEGIN
                       idtStt = 1
                       FOR II = (I - X0),(I + dimStt[1]-1 - X0) DO BEGIN
                          FOR JJ = (J - Y0),(J + dimStt[2]-1 - Y0) DO BEGIN
                             FOR KK = (K - Z0),(K + dimStt[3]-1 - Z0) DO BEGIN
                                IF (Structure[II - I + X0,JJ - J + Y0,KK - K + Z0] $
                                    EQ 1) THEN idtStt = idtStt AND erodeImg[II,JJ,KK]
                             ENDFOR
                          ENDFOR
                       ENDFOR
                       IF idtStt NE 1 THEN BEGIN
                          tmpImg[I,J,K] = 0
                       ENDIF
                    ENDIF ELSE BEGIN
                       tmpImg[I,J,Z] = 0
                    ENDELSE
                 ENDFOR
              ENDFOR
           ENDFOR
        END
        ELSE: BEGIN
           MESSAGE, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
        END
     ENDCASE	
;
     erodeImg = erodeImg AND tmpImg
  ENDELSE

; Gray: a boolean keyword.
  IF KEYWORD_SET(GRAY) THEN BEGIN
; Gray image;
;
; Give the type of the returned array.
     IF KEYWORD_SET(PRESERVE_TYPE) THEN BEGIN 
        tp = TYPENAME(Image)
        erodeImg = CALL_FUNCTION(tp,erodeImg)
     ENDIF
     IF KEYWORD_SET(UINT) THEN erodeImg = UINT(erodeImg)
     IF KEYWORD_SET(ULONG) THEN erodeImg = ULONG(erodeImg)
  ENDIF
  
  RETURN, erodeImg
  
END
