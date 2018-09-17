;
;  GDL replacement to ERODE in IDL
;  Bin Wu <bin.wu (at) edinsights.no>
;  Aug. 2018
;
;  Arguments (from IDL): Image Structure X0 Y0 Z0
;  Keywords (from IDL):
;    GRAY (a boolean keyword)
;        Set this keyword to perform grayscale, rather than binary, erosion. Nonzero
;        elements of the Structure parameter determine the shape of the structuring element
;        (neighborhood). If VALUES is not present, all elements of the structuring element
;        are 0, yielding the neighborhood minimum operator.
;    PRESERVE_TYPE (a boolean keyword)
;        Set this keyword to return the same type as the input array. This keyword only
;        applies if the GRAY keyword is set.
;    UINT (a boolean keyword)
;        Set this keyword to return an unsigned integer array. This keyword only applies if
;        the GRAY keyword is set.
;    ULONG (a boolean keyword)
;        Set this keyword to return an unsigned longword integer array. This keyword only
;        applies if the GRAY keyword is set.
;    VALUES
;        An array of the same dimensions as Structure providing the values of the
;        structuring element. The presence of this keyword implies grayscale erosion. Each
;        pixel of the result is the minimum of Image less the corresponding elements of
;        VALUE. If the resulting difference is less than zero, the return value will be
;        zero.
;  Return (from IDL): the erosion of image
;  Syntax (from IDL):
;    Result = ERODE( Image, Structure [, X0 [, Y0 [, Z0]]] [, /GRAY [, /PRESERVE_TYPE | , /UINT
;         | , /ULONG]] [, VALUES=array] )
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
     tmpImg = erodeImg - erodeImg
; 1D-3D.
     CASE dimStt[0] OF
        1: BEGIN
              FOR I = X0, dims[1]-dimStt[1]+X0 DO BEGIN
                     tmpImg[I] = MAX([0,MIN((erodeImg[I-X0:I+dimStt[1]-1-X0] - VALUES) * Structure)])
              ENDFOR
           END
        2: BEGIN
              FOR I = X0, dims[1]-dimStt[1]+X0 DO BEGIN
                 FOR J = Y0, dims[2]-dimStt[2]+Y0 DO BEGIN
                     tmpImg[I,J] = MAX([0,MIN((erodeImg[I-X0:I+dimStt[1]-1-X0,J-Y0:J+dimStt[2]-1-Y0] - VALUES) * Structure)])
                 ENDFOR
              ENDFOR
           END
        3: BEGIN
              FOR I = X0, dims[1]-dimStt[1]+X0 DO BEGIN
                 FOR J = Y0, dims[2]-dimStt[2]+Y0 DO BEGIN
                    FOR K = Z0, dims[3]-dimStt[3]+Z0 DO BEGIN
                       tmpImg[I,J,K] = MAX([0,MIN((erodeImg[I-X0:I+dimStt[1]-1-X0,J-Y0:J+dimStt[2]-1-Y0,K-Z0:K+dimStt[3]-1-Z0] - VALUES) * Structure)])
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
     tmpImg = erodeImg - erodeImg
     tmpS = TOTAL(Structure)
;
; 1D-3D.
     CASE dimStt[0] OF
        1: BEGIN
           FOR I = X0, dims[1]-dimStt[1]+X0 DO BEGIN
                   IF erodeImg[I] EQ 1 THEN BEGIN
                       tmpImg[I] = $
                       TOTAL(erodeImg[I-X0:I+dimStt[1]-1-X0] * $
                       Structure) EQ tmpS
                   ENDIF
           ENDFOR
        END
        2: BEGIN
           FOR I = X0, dims[1]-dimStt[1]+X0 DO BEGIN
               FOR J = Y0, dims[2]-dimStt[2]+Y0 DO BEGIN
                   IF erodeImg[I,J] EQ 1 THEN BEGIN
                       tmpImg[I,J] = $
                       TOTAL(erodeImg[I-X0:I+dimStt[1]-1-X0,J-Y0:J+dimStt[2]-1-Y0] * $
                       Structure) EQ tmpS
                   ENDIF
               ENDFOR
           ENDFOR
        END
        3: BEGIN
           FOR I = X0, dims[1]-dimStt[1]+X0 DO BEGIN
               FOR J = Y0, dims[2]-dimStt[2]+Y0 DO BEGIN
                  FOR K = Z0, dims[3]-dimStt[3]+Z0 DO BEGIN
                     IF erodeImg[I,J,K] EQ 1 THEN BEGIN
                       tmpImg[I,J,K] = $
                       TOTAL(erodeImg[I-X0:I+dimStt[1]-1-X0,J-Y0:J+dimStt[2]-1-Y0,K-Z0:K+dimStt[3]-1-Z0] * $
                       Structure) EQ tmpS
                     ENDIF
                  ENDFOR
               ENDFOR
           ENDFOR
        END
        ELSE: BEGIN
           MESSAGE, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
        END
     ENDCASE
;
     erodeImg = tmpImg
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

