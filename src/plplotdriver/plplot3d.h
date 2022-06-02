/* include file to insert in a driver code to define minimal functions to provide 3D projections with the 4x4 matrix in GDL_3DTRANSFORMDEVICE */
/* must be further defined internally in driver:
 *  - what is the driver function for 'LINE2D' and 'POLYLINE2D' before #include this file.
 *  -  add 'currDispatchTab=pdt;' in plD_dispatch_init_XXX
 *  -  add 'Status3D=0;' as well in  plD_dispatch_init_XXX
 *  - accept PLESC_2D and 3D in plD_esc_XXX as such:
   case PLESC_3D:
       Set3D( ptr );
       break;
   case PLESC_2D:
       UnSet3D();
       break;
 * 
 * */

#ifndef PLPLOT3D_H_
#define PLPLOT3D_H_

//if the driver needs specific writing of 3D equivalent of 2D functions (i.e., the standard conversion function does not work) then
//write LINE3D_FUNCTION and POLYLINE3D_FUNCTION in the code, and set  SPECIFIC_3D
#ifndef SPECIFIC_3D
#define LINE3D_FUNCTION plD_line_3D
#define POLYLINE3D_FUNCTION plD_polyline_3D
#endif

static PLDispatchTable *currDispatchTab;

int Status3D;

#define PLESC_2D 99
#define PLESC_3D 100

typedef struct {
  double zValue;
  double T[16];
} GDL_3DTRANSFORMDEVICE;

static GDL_3DTRANSFORMDEVICE Data3d;
//generalized for 'flat3d', using zValue, but with screen displacement of sizeX/2 sizeY/2 as this is used to bypass plplot's fixed device coordinates
//// PLPLOT device coords to physical coords (x)
//
//inline PLFLT my_plP_dcpcx(PLFLT x)
//{
//  return plsc->phyxmi + plsc->phyxlen * x;
//}
//
//// device coords to physical coords (y)
//
//inline PLFLT my_plP_dcpcy(PLFLT y)
//{
//  return plsc->phyymi + plsc->phyylen * y;
//}
// PLPLOT physical coords to device coords (x)
//

//inline PLFLT my_plP_pcdcx(PLFLT x)
//{
//  return (x - plsc->phyxmi) / (double) plsc->phyxlen;
//}
//
//// physical coords to device coords (y)
//
//inline PLFLT my_plP_pcdcy(PLFLT y)
//{
//  return (y - plsc->phyymi) / (double) plsc->phyylen;
//}

void SelfTransform3D(int *xs, int *ys) {
  if (Status3D == 1) { //enable use everywhere.
    PLFLT x = *xs, y = *ys;
    // x and Y are in raw device coordinates.
    // convert to NORM
    //  x = my_plP_pcdcx(x);
    //  y = my_plP_pcdcy(y);
    x = (x - plsc->phyxmi) / (double) plsc->phyxlen;
    y = (y - plsc->phyymi) / (double) plsc->phyylen;
    //here it is !P.T not a c/c++ transposed matrix
    PLFLT xx, yy;
    xx = x * Data3d.T[0] + y * Data3d.T[1] + Data3d.zValue * Data3d.T[2] + Data3d.T[3];
    yy = x * Data3d.T[4] + y * Data3d.T[5] + Data3d.zValue * Data3d.T[6] + Data3d.T[7];
    // convert to device again
    //  *xs = (int) (my_plP_dcpcx(xx));
    //  *ys = (int) (my_plP_dcpcy(yy));
    *xs = (int) (plsc->phyxmi + plsc->phyxlen * xx);
    *ys = (int) (plsc->phyymi + plsc->phyylen * yy);
   }
}

void Project3DToPlplotFormMatrix(PLFLT *P) { //P for P ...lplot
   if (Status3D == 1) { //enable use everywhere.
   // compute product of P with Data3d.T for the 4 elements of 'xFormMatrix' P (see plot3d.c)
    // so P' = T * P
    // We need to transpose T since T is like
    // [xFormMatrix[0] xFormMatrix[2]]
    // [xFormMatrix[1] xFormMatrix[3]]

    PLFLT a=P[0]*Data3d.T[0]+P[1]*Data3d.T[4];
    PLFLT c=P[2]*Data3d.T[0]+P[3]*Data3d.T[4];
    PLFLT b=P[0]*Data3d.T[1]+P[1]*Data3d.T[5];
    PLFLT d=P[2]*Data3d.T[1]+P[3]*Data3d.T[5];

    P[0]=a;P[1]=b;P[2]=c;P[3]=d;
   }
}

void plD_line_3D(PLStream *pls, short x1a, short y1a, short x2a, short y2a){
   if (Status3D == 1) { //enable use everywhere.
    //perform conversion on the fly
    int x1 = x1a, y1 = y1a, x2 = x2a, y2 = y2a;
    // 3D convert on normalized values
    SelfTransform3D(&x1, &y1);
    SelfTransform3D(&x2, &y2);
    x1a=x1; y1a=y1; x2a=x2; y2a=y2;
   }
  //call LINE2D genuine driver code
  LINE2D(pls, x1a, y1a, x2a, y2a);
}

void plD_polyline_3D(PLStream *pls, short *xa, short *ya, PLINT npts){
   if (Status3D == 1) { //enable use everywhere.
    //perform conversion on the fly
    for (PLINT i = 0; i < npts; ++i) {
      int x=xa[i];
      int y=ya[i];
    // 3D convert, must take into account that y is inverted.
      SelfTransform3D(&x, &y);

      xa[i] = x;
      ya[i] = y;
    }
   }
  //call POLYLINE2D genuine driver code
  POLYLINE2D(pls, xa, ya, npts);
}

static void
Set3D(void* ptr)
{
  if (currDispatchTab == NULL) return;
  Status3D=1;
  if (ptr != NULL) {
    GDL_3DTRANSFORMDEVICE* data=(GDL_3DTRANSFORMDEVICE*)ptr;
    for (int i = 0; i < 16; ++i) Data3d.T[i]=data->T[i]; 
    Data3d.zValue = data->zValue;
    currDispatchTab->pl_line = (plD_line_fp) LINE3D_FUNCTION;
    currDispatchTab->pl_polyline = (plD_polyline_fp) POLYLINE3D_FUNCTION;
  }
}
static void
UnSet3D()
{
  Status3D=0;
  if (currDispatchTab == NULL) return;
  currDispatchTab->pl_line = (plD_line_fp) LINE2D;
  currDispatchTab->pl_polyline = (plD_polyline_fp) POLYLINE2D;
}

#endif
