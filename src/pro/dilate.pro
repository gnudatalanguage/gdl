;
;  GDL replacement to DILATE in IDL
;  Bin Wu <bin.wu (at) edinsights.no>
;  Aug. 2018
;
;  Arguments (from IDL): Image Structure X0 Y0 Z0
;  Keywords (from IDL):
;     BACKGROUND
;        Set this keyword to the pixel value that is to be considered the background when
;        dilation is being performed in constrained mode. The default value is 0.
;     CONSTRAINED (a boolean keyword)
;        If this keyword is set and grayscale dilation has been selected, the dilation
;        algorithm will operate in constrained mode. In this mode, a pixel is set to the
;        value determined by normal grayscale dilation rules in the output image only if
;        the current value destination pixel value matches the BACKGROUND pixel value. Once
;        a pixel in the output image has been set to a value other than the BACKGROUND
;        value, it cannot change.
;     GRAY (a boolean keyword)
;        Set this keyword to perform grayscale, rather than binary, dilation. The nonzero
;        elements of the Structure parameter determine the shape of the structuring element
;        (neighborhood). If VALUES is not present, all elements of the structuring element
;        are 0, yielding the neighborhood maximum operator.
;     PRESERVE_TYPE (a boolean keyword)
;        Set this keyword to return the same type as the input array. This keyword only
;        applies if the GRAY keyword is set.
;     UINT (a boolean keyword)
;        Set this keyword to return an unsigned integer array. This keyword only applies if
;        the GRAY keyword is set.
;     ULONG (a boolean keyword)
;        Set this keyword to return an unsigned longword integer array. This keyword only
;        applies if the GRAY keyword is set.
;     VALUES
;        An array with the same dimensions as Structure providing the values of the
;        structuring element. The presence of this parameter implies grayscale dilation.
;        Each pixel of the result is the maximum of the sum of the corresponding elements
;        of VALUE and the Image pixel value. If the resulting sum is greater than 255, the
;        return value is 255.
;  Return (from IDL): the dilation of image
;  Syntax (from IDL):
;    Result = DILATE( Image, Structure [, X0 [, Y0 [, Z0]]] [, /CONSTRAINED [,
;         BACKGROUND=value]] [, /GRAY [, /PRESERVE_TYPE | , /UINT | , /ULONG]] [,
;         VALUES=array] )
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

     dilateImg = Image
;
; Two situations: CONSTRAINED and UNCONSTRAINED
     IF KEYWORD_SET(CONSTRAINED) THEN BEGIN
; An auxiliary variable.
        tmpImg = dilateImg
;
; CONSTRAINED situation.
; 1D-3D.
        CASE dimStt[0] OF
           1: BEGIN
                   tmpS = REVERSE(Structure)
                   tmpV = REVERSE(VALUES)
                   tmpValue = INTARR(dims[1])
                   tmpValue[X0] = $
                     dilateImg[X0:dims[1]-dimStt[1]+X0]
              FOR I = 0, dims[1]-1 DO BEGIN
                  IF dilateImg[I] EQ BACKGROUND THEN BEGIN
                     tmpM1 = (I-dimStt[1]+1+X0 LT 0) ? 0:I-dimStt[1]+1+X0
                     tmpM2 = (I+X0 GT dims[1]-1) ? dims[1]-1:I+X0
                     tmpSM1 = (I-dimStt[1]+1+X0 LT 0) ? -I+dimStt[1]-1-X0:0
                     tmpSM2 = (I+X0 GT dims[1]-1) ? dimStt[1]-1-I-X0+dims[1]-1:dimStt[1]-1
                     tmpImg[I] = MIN([255,MAX($
                         (tmpValue[tmpM1:tmpM2] + $
                         tmpV[tmpSM1:tmpSM2]) * $
                         tmpS[tmpSM1:tmpSM2])])
                ENDIF
              ENDFOR
           END
           2: BEGIN
                   tmpS = REVERSE(REVERSE(Structure),2)
                   tmpV = REVERSE(REVERSE(VALUES),2)
                   tmpValue = INTARR(dims[1],dims[2])
                   tmpValue[X0,Y0] = $
                     adilateImg[X0:dims[1]-dimStt[1]+X0,Y0:dims[2]-dimStt[2]+Y0]
              FOR I = 0, dims[1]-1 DO BEGIN
                 FOR J = 0, dims[2]-1 DO BEGIN
                   IF dilateImg[I,J] EQ BACKGROUND THEN BEGIN
                     tmpM1 = (I-dimStt[1]+1+X0 LT 0) ? 0:I-dimStt[1]+1+X0
                     tmpN1 = (J-dimStt[2]+1+Y0 LT 0) ? 0:J-dimStt[2]+1+Y0
                     tmpM2 = (I+X0 GT dims[1]-1) ? dims[1]-1:I+X0
                     tmpN2 = (J+Y0 GT dims[2]-1) ? dims[2]-1:J+Y0
                     tmpSM1 = (I-dimStt[1]+1+X0 LT 0) ? -I+dimStt[1]-1-X0:0
                     tmpSN1 = (J-dimStt[2]+1+Y0 LT 0) ? -J+dimStt[2]-1-Y0:0
                     tmpSM2 = (I+X0 GT dims[1]-1) ? dimStt[1]-1-I-X0+dims[1]-1:dimStt[1]-1
                     tmpSN2 = (J+Y0 GT dims[2]-1) ? dimStt[2]-1-J-Y0+dims[2]-1:dimStt[2]-1
                     tmpImg[I,J] = MIN([255,MAX($
                         (tmpValue[tmpM1:tmpM2,tmpN1:tmpN2] + $
                         tmpV[tmpSM1:tmpSM2,tmpSN1:tmpSN2]) * $
                         tmpS[tmpSM1:tmpSM2,tmpSN1:tmpSN2])])
                   ENDIF
                 ENDFOR
              ENDFOR
           END
           3: BEGIN
                   tmpS = REVERSE(REVERSE(REVERSE(Structure),2),3)
                   tmpV = REVERSE(REVERSE(REVERSE(VALUES),2),3)
                   tmpValue = INTARR(dims[1],dims[2],dims[3])
                   tmpValue[X0,Y0,Z0] = $
                     dilateImg[X0:dims[1]-dimStt[1]+X0,Y0:dims[2]-dimStt[2]+Y0,Z0:dims[3]-dimStt[3]+Z0]
              FOR I = 0, dims[1]-1 DO BEGIN
                 FOR J = 0, dims[2]-1 DO BEGIN
                    FOR K = 0, dims[3]-1 DO BEGIN
                      IF dilateImg[I,J,K] EQ BACKGROUND THEN BEGIN
                        tmpM1 = (I-dimStt[1]+1+X0 LT 0) ? 0:I-dimStt[1]+1+X0
                        tmpN1 = (J-dimStt[2]+1+Y0 LT 0) ? 0:J-dimStt[2]+1+Y0
                        tmpL1 = (K-dimStt[3]+1+Z0 LT 0) ? 0:K-dimStt[3]+1+Z0
                        tmpM2 = (I+X0 GT dims[1]-1) ? dims[1]-1:I+X0
                        tmpN2 = (J+Y0 GT dims[2]-1) ? dims[2]-1:J+Y0
                        tmpL2 = (K+Z0 GT dims[3]-1) ? dims[3]-1:K+Z0
                        tmpSM1 = (I-dimStt[1]+1+X0 LT 0) ? -I+dimStt[1]-1-X0:0
                        tmpSN1 = (J-dimStt[2]+1+Y0 LT 0) ? -J+dimStt[2]-1-Y0:0
                        tmpSL1 = (K-dimStt[3]+1+Z0 LT 0) ? -K+dimStt[3]-1-Z0:0
                        tmpSM2 = (I+X0 GT dims[1]-1) ? dimStt[1]-1-I-X0+dims[1]-1:dimStt[1]-1
                        tmpSN2 = (J+Y0 GT dims[2]-1) ? dimStt[2]-1-J-Y0+dims[2]-1:dimStt[2]-1
                        tmpSL2 = (K+Z0 GT dims[3]-1) ? dimStt[3]-1-K-Z0+dims[3]-1:dimStt[3]-1
                        tmpImg[I,J,K] = MIN([255,MAX($
                            (tmpValue[tmpM1:tmpM2,tmpN1:tmpN2,tmpL1:tmpL2] + $
                            tmpV[tmpSM1:tmpSM2,tmpSN1:tmpSN2,tmpSL1:tmpSL2]) * $
                            tmpS[tmpSM1:tmpSM2,tmpSN1:tmpSN2,tmpSL1:tmpSL2])])
                      ENDIF
                    ENDFOR
                 ENDFOR
              ENDFOR
           END
           ELSE: BEGIN
              MESSAGE, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
           END
        ENDCASE
        dilateImg = tmpImg
;
     ENDIF ELSE BEGIN
; An auxiliary variable.
        tmpImg = dilateImg
;
; UNCONSTRAINED situation.
; 1D-3D.
        CASE dimStt[0] OF
           1: BEGIN
                   tmpS = REVERSE(Structure)
                   tmpV = REVERSE(VALUES)
                   tmpValue = INTARR(dims[1])
                   tmpValue[X0] = $
                     dilateImg[X0:dims[1]-dimStt[1]+X0]
              FOR I = 0, dims[1]-1 DO BEGIN
                     tmpM1 = (I-dimStt[1]+1+X0 LT 0) ? 0:I-dimStt[1]+1+X0
                     tmpM2 = (I+X0 GT dims[1]-1) ? dims[1]-1:I+X0
                     tmpSM1 = (I-dimStt[1]+1+X0 LT 0) ? -I+dimStt[1]-1-X0:0
                     tmpSM2 = (I+X0 GT dims[1]-1) ? dimStt[1]-1-I-X0+dims[1]-1:dimStt[1]-1
                     tmpImg[I] = MIN([255,MAX($
                         (tmpValue[tmpM1:tmpM2] + $
                         tmpV[tmpSM1:tmpSM2]) * $
                         tmpS[tmpSM1:tmpSM2])])
              ENDFOR
           END
           2: BEGIN
                   tmpS = REVERSE(REVERSE(Structure),2)
                   tmpV = REVERSE(REVERSE(VALUES),2)
                   tmpValue = INTARR(dims[1],dims[2])
                   tmpValue[X0,Y0] = $
                     dilateImg[X0:dims[1]-dimStt[1]+X0,Y0:dims[2]-dimStt[2]+Y0]
              FOR I = 0, dims[1]-1 DO BEGIN
                 FOR J = 0, dims[2]-1 DO BEGIN
                     tmpM1 = (I-dimStt[1]+1+X0 LT 0) ? 0:I-dimStt[1]+1+X0
                     tmpN1 = (J-dimStt[2]+1+Y0 LT 0) ? 0:J-dimStt[2]+1+Y0
                     tmpM2 = (I+X0 GT dims[1]-1) ? dims[1]-1:I+X0
                     tmpN2 = (J+Y0 GT dims[2]-1) ? dims[2]-1:J+Y0
                     tmpSM1 = (I-dimStt[1]+1+X0 LT 0) ? -I+dimStt[1]-1-X0:0
                     tmpSN1 = (J-dimStt[2]+1+Y0 LT 0) ? -J+dimStt[2]-1-Y0:0
                     tmpSM2 = (I+X0 GT dims[1]-1) ? dimStt[1]-1-I-X0+dims[1]-1:dimStt[1]-1
                     tmpSN2 = (J+Y0 GT dims[2]-1) ? dimStt[2]-1-J-Y0+dims[2]-1:dimStt[2]-1
                     tmpImg[I,J] = MIN([255,MAX($
                         (tmpValue[tmpM1:tmpM2,tmpN1:tmpN2] + $
                         tmpV[tmpSM1:tmpSM2,tmpSN1:tmpSN2]) * $
                         tmpS[tmpSM1:tmpSM2,tmpSN1:tmpSN2])])
                 ENDFOR
              ENDFOR
           END
           3: BEGIN
                   tmpS = REVERSE(REVERSE(REVERSE(Structure),2),3)
                   tmpV = REVERSE(REVERSE(REVERSE(VALUES),2),3)
                   tmpValue = INTARR(dims[1],dims[2],dims[3])
                   tmpValue[X0,Y0,Z0] = $
                     dilateImg[X0:dims[1]-dimStt[1]+X0,Y0:dims[2]-dimStt[2]+Y0,Z0:dims[3]-dimStt[3]+Z0]
              FOR I = 0, dims[1]-1 DO BEGIN
                 FOR J = 0, dims[2]-1 DO BEGIN
                    FOR K = 0, dims[3]-1 DO BEGIN
                     tmpM1 = (I-dimStt[1]+1+X0 LT 0) ? 0:I-dimStt[1]+1+X0
                     tmpN1 = (J-dimStt[2]+1+Y0 LT 0) ? 0:J-dimStt[2]+1+Y0
                     tmpL1 = (K-dimStt[3]+1+Z0 LT 0) ? 0:K-dimStt[3]+1+Z0
                     tmpM2 = (I+X0 GT dims[1]-1) ? dims[1]-1:I+X0
                     tmpN2 = (J+Y0 GT dims[2]-1) ? dims[2]-1:J+Y0
                     tmpL2 = (K+Z0 GT dims[3]-1) ? dims[3]-1:K+Z0
                     tmpSM1 = (I-dimStt[1]+1+X0 LT 0) ? -I+dimStt[1]-1-X0:0
                     tmpSN1 = (J-dimStt[2]+1+Y0 LT 0) ? -J+dimStt[2]-1-Y0:0
                     tmpSL1 = (K-dimStt[3]+1+Z0 LT 0) ? -K+dimStt[3]-1-Z0:0
                     tmpSM2 = (I+X0 GT dims[1]-1) ? dimStt[1]-1-I-X0+dims[1]-1:dimStt[1]-1
                     tmpSN2 = (J+Y0 GT dims[2]-1) ? dimStt[2]-1-J-Y0+dims[2]-1:dimStt[2]-1
                     tmpSL2 = (K+Z0 GT dims[3]-1) ? dimStt[3]-1-K-Z0+dims[3]-1:dimStt[3]-1
                     tmpImg[I,J,K] = MIN([255,MAX($
                         (tmpValue[tmpM1:tmpM2,tmpN1:tmpN2,tmpL1:tmpL2] + $
                         tmpV[tmpSM1:tmpSM2,tmpSN1:tmpSN2,tmpSL1:tmpSL2]) * $
                         tmpS[tmpSM1:tmpSM2,tmpSN1:tmpSN2,tmpSL1:tmpSL2])])
                    ENDFOR
                 ENDFOR
              ENDFOR
           END
           ELSE: BEGIN
              MESSAGE, 'The input is an invalid image, considering only 1D, 2D, and 3D at the moment.'
           END
        ENDCASE
        dilateImg = tmpImg
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
            FOR I = X0, dims[1]-dimStt[1]+X0 DO BEGIN
                   IF dilateImg[I] EQ 1 THEN BEGIN
                       tmpImg[I-X0:I+dimStt[1]-1-X0] = $
                       tmpImg[I-X0:I+dimStt[1]-1-X0] OR $
                       Structure
                   ENDIF
               ENDFOR
        END
        2: BEGIN
           FOR I = X0, dims[1]-dimStt[1]+X0 DO BEGIN
               FOR J = Y0, dims[2]-dimStt[2]+Y0 DO BEGIN
                   IF dilateImg[I,J] EQ 1 THEN BEGIN
                       tmpImg[I-X0:I+dimStt[1]-1-X0,J-Y0:J+dimStt[2]-1-Y0] = $
                       tmpImg[I-X0:I+dimStt[1]-1-X0,J-Y0:J+dimStt[2]-1-Y0] OR $
                       Structure
                   ENDIF
               ENDFOR
           ENDFOR
        END
        3: BEGIN
           FOR I = X0, dims[1]-dimStt[1]+X0 DO BEGIN
               FOR J = Y0, dims[2]-dimStt[2]+Y0 DO BEGIN
                   FOR K = Z0, dims[3]-dimStt[3]+Z0 DO BEGIN
                   IF dilateImg[I,J] EQ 1 THEN BEGIN
                       tmpImg[I-X0:I+dimStt[1]-1-X0,J-Y0:J+dimStt[2]-1-Y0,K-Z0:K+dimStt[3]-1-Z0] = $
                       tmpImg[I-X0:I+dimStt[1]-1-X0,J-Y0:J+dimStt[2]-1-Y0,K-Z0:K+dimStt[3]-1-Z0] OR $
                       Structure
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
     dilateImg = tmpImg
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

