;
;  GDL replacement to DILATE in IDL
;  Bin Wu <bin.wu (at) edinsights.no>
;  Aug. 2018
;
;  Arguments (from IDL): Image Structure X0 Y0 Z0
;  Keywords (from IDL): 
;  	BACKGROUND 
;		Set this keyword to the pixel value that is to be considered the background when 
;		dilation is being performed in constrained mode. The default value is 0.
; 	CONSTRAINED (a boolean keyword)
;		If this keyword is set and grayscale dilation has been selected, the dilation 
;		algorithm will operate in constrained mode. In this mode, a pixel is set to the 
;		value determined by normal grayscale dilation rules in the output image only if 
;		the current value destination pixel value matches the BACKGROUND pixel value. Once 
;		a pixel in the output image has been set to a value other than the BACKGROUND 
;		value, it cannot change.
;	GRAY (a boolean keyword)
;		Set this keyword to perform grayscale, rather than binary, dilation. The nonzero 
;		elements of the Structure parameter determine the shape of the structuring element 
;		(neighborhood). If VALUES is not present, all elements of the structuring element 
;		are 0, yielding the neighborhood maximum operator.
; 	PRESERVE_TYPE (a boolean keyword)
;		Set this keyword to return the same type as the input array. This keyword only 
;		applies if the GRAY keyword is set.
;	UINT (a boolean keyword)
;		Set this keyword to return an unsigned integer array. This keyword only applies if 
;		the GRAY keyword is set.
; 	ULONG (a boolean keyword)
;		Set this keyword to return an unsigned longword integer array. This keyword only 
;		applies if the GRAY keyword is set.
;	VALUES
;		An array with the same dimensions as Structure providing the values of the 
;		structuring element. The presence of this parameter implies grayscale dilation. 
;		Each pixel of the result is the maximum of the sum of the corresponding elements 
;		of VALUE and the Image pixel value. If the resulting sum is greater than 255, the 
;		return value is 255.
;  Return (from IDL): the dilation of image
;  Syntax (from IDL): 
;	Result = DILATE( Image, Structure [, X0 [, Y0 [, Z0]]] [, /CONSTRAINED [, 
;		 BACKGROUND=value]] [, /GRAY [, /PRESERVE_TYPE | , /UINT | , /ULONG]] [, 
;		 VALUES=array] )
;  Ref: http://www.harrisgeospatial.com/docs/DILATE.html
;
FUNCTION DILATE, Image, Structure, X0, Y0, Z0, $
                 CONSTRAINED=CONSTRAINED, BACKGROUND=BACKGROUND, $ 
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
  
; Set default value to BACKGROUND.
  IF (KEYWORD_SET(BACKGROUND) EQ 0) THEN BACKGROUND = 0
  
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
     dilateImg = TRANSPOSE(Image)
;
; Two situations: CONSTRAINED and UNCONSTRAINED
     IF KEYWORD_SET(CONSTRAINED) THEN BEGIN
; An auxiliary variable.
        tmpImg = dilateImg
; An auxiliary PARAMETER.
        tmpMin = MIN(dilateImg)
;  
; CONSTRAINED situation.
; 1D-3D.
        CASE dimStt[0] OF
           1: BEGIN
              FOR I = 0, dims[1]-1 DO BEGIN
                 IF dilateImg[I] EQ BACKGROUND THEN BEGIN
                    IF ((I - X0) GE 0) AND ((I + dimStt[1]-1 - X0) LE dims[1]-1) THEN BEGIN
                       tmpImg[I] = tmpMin
                       FOR II = (I - dimStt[1]+1 + X0),(I + X0) DO BEGIN
                          IF (Structure[I - II + X0] EQ 1) THEN BEGIN
                             IF (II LT 0) OR (II GT dims[1]-1) THEN BEGIN
                                tmpII = tmpMin
                             ENDIF ELSE BEGIN
                                tmpII = dilateImg[II]
                             ENDELSE
                             tmpImg[I] = MIN([255,MAX([tmpImg[I], tmpII + VALUES[I - II + X0]])])
                          ENDIF
                       ENDFOR
                    ENDIF ELSE BEGIN
                       tmpImg[I] = tmpMin
                    ENDELSE
                 ENDIF
              ENDFOR
           END
           2: BEGIN
              FOR I = 0, dims[1]-1 DO BEGIN
                 FOR J = 0, dims[2]-1 DO BEGIN
                    IF dilateImg[I,J] EQ BACKGROUND THEN BEGIN
                       IF ((I - X0) GE 0) AND ((I + dimStt[1]-1 - X0) LE dims[1]-1) AND $
                          ((J - Y0) GE 0) AND ((J + dimStt[2]-1 - Y0) LE dims[2]-1) THEN BEGIN
                          tmpImg[I,J] = tmpMin
                          FOR II = (I - dimStt[1]+1 + X0),(I + X0) DO BEGIN
                             FOR  JJ = (J - dimStt[2]+1 + Y0),(J + Y0) DO BEGIN
                                IF (Structure[I - II + X0,J - JJ + Y0] EQ 1) THEN BEGIN
                                   IF  (II LT 0) OR (II GT dims[1]-1) OR (JJ LT 0) OR (JJ GT dims[2]-1) THEN BEGIN
                                      tmpIIJJ = tmpMin
                                   ENDIF ELSE BEGIN
                                      tmpIIJJ = dilateImg[II,JJ]
                                   ENDELSE
                                   tmpImg[I,J] =  MIN([255, MAX([tmpImg[I,J], tmpIIJJ + VALUES[I - II + X0,J - JJ + Y0]])])
                                ENDIF
                             ENDFOR
                          ENDFOR
                       ENDIF ELSE BEGIN
                          tmpImg[I,J] = tmpMin
                       ENDELSE
                    ENDIF
                 ENDFOR
              ENDFOR
           END
           3: BEGIN
              FOR I = 0, dims[1]-1 DO BEGIN
                 FOR J = 0, dims[2]-1 DO BEGIN
                    FOR K = 0, dims[3]-1 DO BEGIN
                       IF dilateImg[I,J,K] EQ BACKGROUND THEN BEGIN
                          IF ((I - X0) GE 0) AND ((I + dimStt[1]-1 - X0) LE dims[1]-1) AND $
                             ((J - Y0) GE 0) AND ((J + dimStt[2]-1 - Y0) LE dims[2]-1) AND $
                             ((K - Z0) GE 0) AND ((K + dimStt[3]-1 - Z0) LE dims[3]-1) THEN BEGIN
                             tmpImg[I,J,K] = tmpMin
                             FOR II = (I - dimStt[1]+1 + X0),(I + X0) DO BEGIN
                                FOR JJ = (J - dimStt[2]+1 + Y0),(J + Y0) DO BEGIN
                                   FOR KK = (K - dimStt[3]+1 + Z0),(K + Z0) DO BEGIN
                                      IF (Structure[I - II + X0,J - JJ + Y0,K - KK + Z0] EQ 1) THEN BEGIN
                                         IF  (II LT 0) OR (II GT dims[1]-1) OR (JJ LT 0) OR (JJ GT dims[2]-1) OR (KK LT 0) OR (KK GT dims[3]-1) THEN BEGIN
                                            tmpIIJJKK = tmpMin
                                         ENDIF ELSE BEGIN
                                            tmpIIJJKK = dilateImg[II,JJ,KK]
                                         ENDELSE
                                         tmpImg[I,J,K] = $
                                            MIN([255, MAX([tmpImg[I,J,K], tmpIIJJKK + $
                                                           VALUES[I - II + X0,J - JJ + Y0,K - KK + Z0]])])
                                      ENDIF
                                   ENDFOR
                                ENDFOR
                             ENDFOR
                          ENDIF ELSE BEGIN
                             tmpImg[I,J,K] = tmpMin
                          ENDELSE
                       ENDIF
                    ENDFOR
                 ENDFOR
              ENDFOR
           END
           ELSE: BEGIN
              MESSAGE, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
           END
        ENDCASE	
        dilateImg = TRANSPOSE(tmpImg)      
;          
     ENDIF ELSE BEGIN
; An auxiliary variable.
        tmpImg = dilateImg
; An auxiliary PARAMETER.
        tmpMin = MIN(dilateImg)
;      
; UNCONSTRAINED situation.
; 1D-3D.
        CASE dimStt[0] OF
           1: BEGIN
              FOR I = 0, dims[1]-1 DO BEGIN
                 IF ((I - X0) GE 0) AND ((I + dimStt[1]-1 - X0) LE dims[1]-1) THEN BEGIN
                    tmpImg[I] = tmpMin
                    FOR II = (I - dimStt[1]+1 + X0),(I + X0) DO BEGIN
                       IF (Structure[I - II + X0] EQ 1) THEN BEGIN
                          IF (II LT 0) OR (II GT dims[1]-1) THEN BEGIN
                             tmpII = tmpMin
                          ENDIF ELSE BEGIN
                             tmpII = dilateImg[II]
                          ENDELSE
                          tmpImg[I] = MIN([255,MAX([tmpImg[I], tmpII + VALUES[I - II + X0]])])
                       ENDIF
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
                       tmpImg[I,J] = tmpMin
                       FOR II = (I - dimStt[1]+1 + X0),(I + X0) DO BEGIN
                          FOR  JJ = (J - dimStt[2]+1 + Y0),(J + Y0) DO BEGIN
                             IF (Structure[I - II + X0,J - JJ + Y0] EQ 1) THEN BEGIN
                                IF  (II LT 0) OR (II GT dims[1]-1) OR (JJ LT 0) OR (JJ GT dims[2]-1) THEN BEGIN
                                   tmpIIJJ = tmpMin
                                ENDIF ELSE BEGIN
                                   tmpIIJJ = dilateImg[II,JJ]
                                ENDELSE
                                tmpImg[I,J] =  MIN([255, MAX([tmpImg[I,J], tmpIIJJ + VALUES[I - II + X0,J - JJ + Y0]])])
                             ENDIF
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
                          tmpImg[I,J,K] = tmpMin
                          FOR II = (I - dimStt[1]+1 + X0),(I + X0) DO BEGIN
                             FOR JJ = (J - dimStt[2]+1 + Y0),(J + Y0) DO BEGIN
                                FOR KK = (K - dimStt[3]+1 + Z0),(K + Z0) DO BEGIN
                                   IF (Structure[I - II + X0,J - JJ + Y0,K - KK + Z0] EQ 1) THEN BEGIN
                                      IF  (II LT 0) OR (II GT dims[1]-1) OR (JJ LT 0) OR (JJ GT dims[2]-1) OR (KK LT 0) OR (KK GT dims[3]-1) THEN BEGIN
                                         tmpIIJJKK = tmpMin
                                      ENDIF ELSE BEGIN
                                         tmpIIJJKK = dilateImg[II,JJ,KK]
                                      ENDELSE
                                      tmpImg[I,J,K] = $
                                         MIN([255, MAX([tmpImg[I,J,K], tmpIIJJKK + $
                                                        VALUES[I - II + X0,J - JJ + Y0,K - KK + Z0]])])
                                   ENDIF
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
        dilateImg = TRANSPOSE(tmpImg)
     ENDELSE
;
  ENDIF ELSE BEGIN
; Binary image otherwise.
     dilateImg = Image GT 0
;
; An auxiliary variable.
     tmpImg = dilateImg - dilateImg
;
; 1D-3D.
     CASE dimStt[0] OF
        1: BEGIN
           FOR I = 0, dims[1]-1 DO BEGIN
              IF (dilateImg[I] EQ 1) AND ((I - X0) GE 0) AND ((I + dimStt[1]-1 - X0) LE dims[1]-1) THEN BEGIN
                 FOR II = (I - X0),(I + dimStt[1]-1 - X0) DO BEGIN
                    IF (Structure[II - I + X0] EQ 1) THEN tmpImg[II] = 1
                 ENDFOR
              ENDIF
           ENDFOR
           FOR I = 0, dims[1]-1 DO BEGIN
              IF (dilateImg[I] EQ 1) AND (((I - X0) LT 0) OR ((I + dimStt[1]-1 - X0) GT dims[1]-1)) THEN dilateImg[I] = 0
           ENDFOR
        END
        2: BEGIN
           FOR I = 0, dims[1]-1 DO BEGIN
              FOR J = 0, dims[2]-1 DO BEGIN
                 IF (dilateImg[I,J] EQ 1) AND ((I - X0) GE 0) AND ((I + dimStt[1]-1 - X0) LE dims[1]-1) $
                    AND ((J - Y0) GE 0) AND ((J + dimStt[2]-1 - Y0) LE dims[2]-1) THEN BEGIN
                    FOR II = (I - X0),(I + dimStt[1]-1 - X0) DO BEGIN
                       FOR JJ = (J - Y0),(J + dimStt[2]-1 - Y0) DO BEGIN
                          IF (Structure[II - I + X0,JJ - J + Y0] EQ 1) THEN tmpImg[II,JJ] = 1
                       ENDFOR
                    ENDFOR
                 ENDIF
              ENDFOR
           ENDFOR
           FOR I = 0, dims[1]-1 DO BEGIN
              FOR J = 0, dims[2]-1 DO BEGIN
                 IF (dilateImg[I,J] EQ 1) AND (((I - X0) LT 0) OR ((I + dimStt[1]-1 - X0) GT dims[1]-1) $
                                               OR ((J - Y0) LT 0) OR ((J + dimStt[2]-1 - Y0) GT dims[2]-1)) THEN dilateImg[I,J] = 0
              ENDFOR
           ENDFOR
        END
        3: BEGIN
           FOR I = 0, dims[1]-1 DO BEGIN
              FOR J = 0, dims[2]-1 DO BEGIN
                 FOR K = 0, dims[3]-1 DO BEGIN
                    IF (dilateImg[I,J,K] EQ 1) AND ((I - X0) GE 0) AND ((I + dimStt[1]-1 - X0) LE dims[1]-1) $
                       AND ((J - Y0) GE 0) AND ((J + dimStt[2]-1 - Y0) LE dims[2]-1) $
                       AND ((K - Z0) GE 0) AND ((K + dimStt[3]-1 - Z0) LE dims[3]-1) THEN BEGIN
                       FOR II = (I - X0),(I + dimStt[1]-1 - X0) DO BEGIN
                          FOR JJ = (J - Y0),(J + dimStt[2]-1 - Y0) DO BEGIN
                             FOR KK = (K - Z0),(K + dimStt[3]-1 - Z0) DO BEGIN
                                IF (Structure[II - I + X0,JJ - J + Y0,KK - K + Z0] $
                                    EQ 1) THEN tmpImg[II,JJ,KK] = 1
                             ENDFOR
                          ENDFOR
                       ENDFOR
                    ENDIF
                 ENDFOR
              ENDFOR
           ENDFOR
           FOR I = 0, dims[1]-1 DO BEGIN
              FOR J = 0, dims[2]-1 DO BEGIN
                 FOR K = 0, dims[3]-1 DO BEGIN
                    IF (dilateImg[I,J,K] EQ 1) AND (((I - X0) LT 0) OR ((I + dimStt[1]-1 - X0) GT dims[1]-1) $
                                                    OR ((J - Y0) LT 0) OR ((J + dimStt[2]-1 - Y0) GT dims[2]-1) $
                                                    OR ((K - Z0) LT 0) OR ((K + dimStt[3]-1 - Z0) GT dims[3]-1)) THEN dilateImg[II,JJ,KK] = 0
                 ENDFOR
              ENDFOR
           ENDFOR
        END
        ELSE: BEGIN
           MESSAGE, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
        END
     ENDCASE	
;
     dilateImg = dilateImg OR tmpImg
  ENDELSE
  
; Gray: a boolean keyword.
  IF KEYWORD_SET(GRAY) THEN BEGIN
; Gray image;
;
; Give the type of the returned array.
     IF KEYWORD_SET(PRESERVE_TYPE) THEN BEGIN 
        tp = TYPENAME(Image)
        dilateImg = CALL_FUNCTION(tp,dilateImg)
     ENDIF
     IF KEYWORD_SET(UINT) THEN dilateImg = UINT(dilateImg)
     IF KEYWORD_SET(ULONG) THEN dilateImg = ULONG(dilateImg)
  ENDIF
  
  RETURN, dilateImg
  
END
