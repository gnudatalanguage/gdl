/* include file to insert in a driver code to define minimal functions to provide 3D projections with the 4x4 matrix in GDL_3DTRANSFORMDEVICE */
/* must be further defined internally in driver:
 *  -code for 'LINE3D' and 'POLYLINE3D'
 *  - save   currDispatchTab=pdt; in plD_dispatch_init_XXX
 *  - set Status3D=0 as well in  plD_dispatch_init_XXX
 *  - accept PLESC_2D and 3D in plD_esc_ as such:
 *  case PLESC_3D:
 *      Set3D( ptr );
 *      break;
 *
 *  case PLESC_2D:
 *      UnSet3D();
 *      break;
 * */

#ifndef PLPLOT3D_H_
#define PLPLOT3D_H_

void LINE3D( PLStream *, short, short, short, short );
void POLYLINE3D( PLStream *, short *, short *, PLINT );

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

static void SelfTransform3D(int *xs, int *ys) {
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

static void
Set3D(void* ptr)
{
  Status3D=1;
  if (currDispatchTab == NULL) return;
  if (ptr != NULL) {
    GDL_3DTRANSFORMDEVICE* data=(GDL_3DTRANSFORMDEVICE*)ptr;
    for (int i = 0; i < 16; ++i) Data3d.T[i]=data->T[i]; 
    Data3d.zValue = data->zValue;
    currDispatchTab->pl_line = (plD_line_fp) LINE3D;
    currDispatchTab->pl_polyline = (plD_polyline_fp) POLYLINE3D;
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
