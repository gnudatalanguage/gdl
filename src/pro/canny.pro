;+
;B=Canny(A,sigma,tlow,thigh) is an implementation of the Canny
;edge detector.
;
;PARAMETERS:
;A is a 2D image array of numbers, typically of bytes.
;
;sigma is the standard deviation of a gaussian smoothing filter.
;      Typical values are in the range 0.6 to 2.5. The choice
;      depends on the noisiness of the image.
;
;tlow and thigh are the hysteresis threshold values. They must
;      be in the range 0<tlow<thigh<1. Typical values are
;      tlow in 0.2-0.5 and thigh in 0.6-0.9.
;
;KEYWORD PARAMETERS:
;The following arrays are not of primary importance, but may be
;of interest in probing the behavior of the algorithm when trying
;to adjust the primary parameter values.
;
;SMOOTHED_IMAGE provides the image after it has been smoothed by
;       the gaussian blur filter.
;
;GRAD_X provides the horizontal gradient image as an array the
;        same size as A.
;
;GRAD_Y provides the vertical gradient image as an array the
;        same size as A.
;
;GRAD_M provides the gradient magnitude
;
;GRAD_A provides the gradient angle
;
;NMS    provides the array that is produced by suppressing the
;       non-maximum peaks. This is an intermediate array that
;       is used in the edge-finding process. It is of interest
;       only if you are curious about this inner map.
;
;HISTORY
;Written February 2001 by Harvey Rhody (rhody@cis.rit.edu). Use
;was made of a program written in C by Mike Heath at University
;of South Florida (heath@csee.usf.edu). He, in turn, had made
;use of some pieces of code that had been written at Michigan
;State University.
;-
;****************************************************************************
;****************************************************************************

PRO params,NOEDGE,POSSIBLE_EDGE,ISEDGE
NOEDGE=0
POSSIBLE_EDGE=128
ISEDGE=255
END
;****************************************************************************

PRO follow_edges, edge, mag, pos, lowval, cols
; PROCEDURE: follow_edges
; PURPOSE: This procedure is a recursive routine that traces edgs along
; all possible edges whose magnitude values remain above some specifyable
; a lower threshhold.

params,NOEDGE,POSSIBLE_EDGE,ISEDGE
x = [1,1,0,-1,-1,-1,0,1]
y = [0,1,1,1,0,-1,-1,-1]
   FOR i=0,7 DO BEGIN
      tpos = pos - y[i]*cols + x[i];

      IF (edge[tpos] EQ POSSIBLE_EDGE) AND (mag[tpos] GT lowval) $
         THEN BEGIN
         	edge[tpos]=ISEDGE;
         	follow_edges,edge, mag, tpos, lowval, cols
         ENDIF
   ENDFOR
END; End of procedure follow_edges
;****************************************************************************

PRO apply_hysteresis, mag,nms,rows,cols,tlow,thigh,edge
; PURPOSE: This routine finds edges that are above some high threshhold or
; are connected to a high pixel by a path of pixels greater than a low
; threshold.

   ; Initialize the edge map to possible edges everywhere the non-maximal
   ; suppression suggested there could be an edge except for the border. At
   ; the border we say there can not be an edge because it makes the
   ; follow_edges algorithm more efficient to not worry about tracking an
   ; edge off the side of the image.

params,NOEDGE,POSSIBLE_EDGE,ISEDGE

edge=Replicate(NOEDGE,cols,rows)
pos=Where(nms EQ POSSIBLE_EDGE)

edge[pos]=POSSIBLE_EDGE
edge[0,*]=NOEDGE
edge[cols-1,*]=NOEDGE
edge[*,0]=NOEDGE
edge[*,rows-1]=NOEDGE

   ; Compute the histogram of the magnitude image where there are
   ; possible edges. . Then use the histogram to compute hysteresis thresholds.

pos=Where(edge EQ POSSIBLE_EDGE)

hist=Histogram(mag[pos],OMIN=minimum_mag,OMAX=maximum_mag)
numedges=N_Elements(pos)
highcount = ROUND(numedges * thigh)

   ; Compute the high threshold value as the (100 * thigh) percentage point
   ; in the magnitude of the gradient histogram of all the pixels that passes
   ; non-maximal suppression. Then calculate the low threshold as a fraction
   ; of the computed high threshold value. John Canny said in his paper
   ; "A Computational Approach to Edge Detection" that "The ratio of the
   ; high to low threshold in the implementation is in the range two or three
   ; to one." That means that in terms of this implementation, we should
   ; choose tlow ~= 0.5 or 0.33333.

   r = 1L;
   numedges = hist[0];
   WHILE r LT maximum_mag-1 AND numedges LT highcount DO BEGIN
   		r=r+1
        numedges=numedges + hist[r];
   ENDWHILE

   highthreshold = r;
   lowthreshold = Round(highthreshold * tlow)


   ; This loop looks for pixels above the highthreshold to locate edges and
   ; then calls follow_edges to continue the edge.

   pos=0L
   FOR r=0,rows-1 DO BEGIN
      FOR c=0,cols-1 DO BEGIN
         IF (edge[pos] EQ POSSIBLE_EDGE) AND (mag[pos] GE highthreshold) THEN BEGIN
            edge[pos] = ISEDGE;
            follow_edges,edge, mag, pos, lowthreshold, cols
         ENDIF
         pos=pos+1
      ENDFOR
   ENDFOR


   ; Set all the remaining possible edges to non-edges.

   pos=Where(edge NE ISEDGE)
   edge[pos]=NOEDGE

   END; End of Apply_Hysteresis procedure

;****************************************************************************
PRO Non_max_supp,mag,gradx,grady,nrows,ncols,result
; PURPOSE: This routine applies non-maximal suppression to the magnitude of
; the gradient image.

params,NOEDGE,POSSIBLE_EDGE,ISEDGE
;Initialize the result image
result=FltArr(ncols,nrows)
FOR rowcount=1L,nrows-2 DO BEGIN

FOR colcount=1L,ncols-2 DO BEGIN
	p=rowcount*ncols+colcount
	m00=Float(mag[p])

;        print,'m00',m00
;        print, m00 EQ 0 

	IF m00 EQ 0 THEN $
		result[p]=NOEDGE ELSE BEGIN
			gx=gradx[p]
			xperp=-gx/m00
			gy=grady[p]
			yperp=gy/m00
		ENDELSE

	IF m00 GT 0 THEN BEGIN
	;One of the cases below should be computed. Construct
	;a selection key with the three tests as bit values.
	test=(gx GE 0)*4B + (gy GE 0)*2B + (Abs(gx) GE Abs(gy))

;        print,'test',test

	CASE test OF
	7:	  BEGIN
		  ; 111
		  ; Left point
		  	z1=mag[p-1]
		  	z2=mag[p-ncols-1]
		  	mag1=(m00-z1)*xperp +(z2-z1)*yperp
		  ;Right Point
		  	z1=mag[p+1]
		  	z2=mag[p+ncols+1]
		  	mag2=(m00-z1)*xperp + (z2-z1)*yperp
		  END
   6:	  BEGIN
         ; 110
         ; Left point
         z1 = mag[p-ncols]
         z2=mag[p-ncols-1]
         mag1 = (z1 - z2)*xperp + (z1 - m00)*yperp;

                        ; Right point
                        z1 = mag[p+ncols]
                        z2 = mag[p+ncols + 1]

                        mag2 = (z1 - z2)*xperp + (z1 - m00)*yperp;
			END
   5:     BEGIN
                        ; 101
                        ; Left point
                        z1 = mag[p-1]
                        z2 = mag[p+ ncols - 1]

                        mag1 = (m00 - z1)*xperp + (z1 - z2)*yperp;

                        ; Right point
                        z1 = mag[p+ 1]
                        z2 = mag[p- ncols + 1]

                        mag2 = (m00 - z1)*xperp + (z1 - z2)*yperp;

			END

 4:			BEGIN
                        ; 100
                        ; Left point
                        z1 = mag[p+ ncols]
                        z2 = mag[p+ ncols - 1]

                        mag1 = (z1 - z2)*xperp + (m00 - z1)*yperp;

                        ; Right point
                        z1 = mag[p- ncols]
                        z2 = mag[p- ncols + 1]

                        mag2 = (z1 - z2)*xperp  + (m00 - z1)*yperp;
        END
 3:		BEGIN
                        ; 011
                        ; Left point
                        z1 = mag[p+ 1]
                        z2 = mag[p- ncols + 1]

                        mag1 = (z1 - m00)*xperp + (z2 - z1)*yperp;

                        ; Right point
                        z1 = mag[p- 1]
                        z2 = mag[p+ ncols - 1]

                        mag2 = (z1 - m00)*xperp + (z2 - z1)*yperp;
         END
 2:		BEGIN
                        ; 010
                        ; Left point
                        z1 = mag[p- ncols]
                        z2 = mag[p- ncols + 1]

                        mag1 = (z2 - z1)*xperp + (z1 - m00)*yperp;

                        ; Right point
                        z1 = mag[p+ ncols]
                        z2 = mag[p+ ncols - 1]

                        mag2 = (z2 - z1)*xperp + (z1 - m00)*yperp;
        END
1:		BEGIN
                        ; 001
                        ; Left point
                        z1 = mag[p+ 1]
                        z2 = mag[p+ ncols + 1]

                        mag1 = (z1 - m00)*xperp + (z1 - z2)*yperp;

                        ; Right point
                        z1 = mag[p- 1]
                        z2 = mag[p- ncols - 1]

                        mag2 = (z1 - m00)*xperp + (z1 - z2)*yperp;
       END
0:		BEGIN
                        ; 000
                        ; Left point
                        z1 = mag[p+ ncols]
                        z2 = mag[p+ ncols + 1]

                        mag1 = (z2 - z1)*xperp + (m00 - z1)*yperp;

                        ; Right point
                        z1 = mag[p- ncols]
                        z2 = mag[p- ncols - 1]

                        mag2 = (z2 - z1)*xperp + (m00 - z1)*yperp;
END
ENDCASE


            ; Now determine if the current point is a maximum point

           IF ((mag1 GT 0.0) OR (mag2 GT 0.0)) THEN $
                result[p] = NOEDGE ELSE IF (mag2 EQ 0.0) THEN $
                    result[p] = NOEDGE ELSE BEGIN
                    result[p] = POSSIBLE_EDGE;
            ENDELSE
ENDIF
ENDFOR
ENDFOR

END
;===============================================================================
PRO make_gaussian_kernel,sigma,kernel,windowsize
;PURPOSE: To make a 1D convolution kernel with a gaussian profile to
;be used in smoothing the image edges.
windowsize = 1 + 2 * ceil(2.5 * sigma);
center =windowsize/2
x=FindGen(windowsize)-center
kernel=Exp(-(x/sigma)^2/2.0)/sigma/Sqrt(2*!pi)
kernel=kernel/Total(kernel)
END ;Make_Gaussian_kernel
;===============================================================================
PRO gaussian_smooth,image, rows, cols, sigma, P
;PURPOSE: To smooth the image with a gaussian kernel. Smoothing
;is done in both horizontal and vertical directions. The
;array is first padded with zeros so that convolution is done
;to the edges and then the result is trimmed back to the image
;size.

print,'image',min(image),max(image)

BOOSTBLURFACTOR=90.0

;Make a 1D Gaussian kernel
make_gaussian_kernel,sigma,kernel,windowsize

print,'kernel',min(kernel),max(kernel)

center=windowsize/2
;Zero-pad the array
P=FltArr(cols+2*windowsize-2,rows+2*windowsize-2)

P[windowsize-1:cols+windowsize-2,windowsize-1:rows+windowsize-2]=image

print,'P',min(P),max(P)

help,P,kernel

;Convolve horizontally
P=Convol(P,kernel)

print,'P',min(P),max(P)

;Convolve vertically
P=Convol(P,Transpose(kernel))

print,'P',min(P),max(P)

;Trim the result
P=P[windowsize-1:cols+windowsize-2,windowsize-1:rows+windowsize-2]

;Scale the result.
P=P/(Total(Abs(kernel))^2)*BOOSTBLURFACTOR

print,min(P),max(P)

END ;gaussian_smooth
;===============================================================================
PRO derivative_x_y,smoothedim, rows, cols, delta_x, delta_y
;PURPOSE: To compute the gradients in the horizontal
;and vertical directions. The array is first padded
;with zeros so that convolution is done to the edges
;and then the result is trimmed back to the image size.

;print,smoothedim,rows,cols

hx=[[-1,0,1],[-1,0,1],[-1,0,1]]
hy=[[1,1,1],[0,0,0],[-1,-1,-1]]
;Pad the array
P=FltArr(cols+4,rows+4)
P[2:cols+1,2:rows+1]=smoothedim
;Convolve with gradient masks and then trim to size.
delta_x=Convol(P,Float(hx))
delta_x=delta_x[2:cols+1,2:rows+1]
delta_y=Convol(P,Float(hy))
delta_y=delta_y[2:cols+1,2:rows+1]

print,min(delta_x)
print,max(delta_x)

print,min(delta_y)
print,max(delta_y)

END
;===============================================================================

PRO radian_direction,delta_x, delta_y, dir_radians, xdirtag, ydirtag

;DIRECTION TAGS:
; Purpose: To compute a direction of the gradient image from component dx and
; dy images. Because not all derriviatives are computed in the same way, this
; code allows for dx or dy to have been calculated in different ways.
;
; FOR X:  xdirtag = -1  for  [-1 0  1]
;         xdirtag =  1  for  [ 1 0 -1]
;
; FOR Y:  ydirtag = -1  for  [-1 0  1]'
;         ydirtag =  1  for  [ 1 0 -1]'
;
; The resulting angle is in radians measured counterclockwise from the
; xdirection. The angle points "up the gradient".dir_radians=Atan(dy,dx)
; If the direction tags are not given then they are assumed to be [1,1].

IF N_Elements(xdirtag) EQ 0 THEN xdirtag=1
IF N_Elements(ydirtag) EQ 0 THEN ydirtag=1

dir_radians=Atan(delta_y*ydirtag,delta_x*xdirtag)

END

;=====================================================================================


FUNCTION canny,$
	image,$						;Input image array (required)
    sigma,$						;Smoothing filter spread (required)
    tlow,$						;Low threshold (required)
    thigh,$						;High threshold (required)
	SMOOTHED_IMAGE=smoothedim,$ ;Smoothed image
	GRAD_X=delta_x,$            ;X gradient
	GRAD_Y=delta_y,$            ;Y gradient
	GRAD_M=magnitude,$          ;Gradient magnitudes
	GRAD_A=dir_radians,$		;Gradient angles
	NMS=nms                     ;Internal possible edge map


imsize=Size(image)
IF imsize[0] NE 2 THEN Message,'Image must be 2D,/Error'
rows=imsize[2] & cols=imsize[1]

;Perform Gaussian smoothing
gaussian_smooth,image, rows, cols, sigma, smoothedim

;Compute X and Y gradients
derivative_x_y,smoothedim, rows, cols, delta_x, delta_y

;Compute magnitude of gradient
magnitude=Sqrt(Float(delta_x)^2 + Float(delta_y)^2)


;Compute direction of gradient
radian_direction,delta_x, delta_y, dir_radians, -1, -1

;Perform non-maximal suppression
Non_max_supp,magnitude, delta_x, delta_y, rows, cols, nms

;Apply hysteresis to mark edge pixels
Apply_Hysteresis,magnitude, nms, rows, cols, tlow, thigh, edge

Return,BytScl(edge)

END
