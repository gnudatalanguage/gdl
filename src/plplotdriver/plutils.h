
void gdl_plP_movphy(PLINT x, PLINT y); 
void gdl_plP_draphy(PLINT x, PLINT y);
void gdl_pllclp(PLINT *x, PLINT *y, PLINT npts);
void gdl_plP_pllclp(PLINT *x, PLINT *y, PLINT npts,
  PLINT xmin, PLINT xmax, PLINT ymin, PLINT ymax,
  void ( *draw)(short *, short *, PLINT));
void gdl_plP_draphy_poly(PLINT *x, PLINT *y, PLINT n);
void gdl_plxtik(PLINT x, PLINT y, PLINT below, PLINT above);
void gdl_plytik(PLINT x, PLINT y, PLINT left, PLINT right);
static PLUNICODE unicode_buffer_static[1024];

void gdl_plP_text(PLINT base, PLFLT just, PLFLT *xform, PLINT x, PLINT y,
  PLINT refx, PLINT refy, PLCHAR_VECTOR string) ;
void gdl_plstr(PLINT base, PLFLT *xform, PLINT refx, PLINT refy, PLCHAR_VECTOR string);
void gdl_plchar(signed char *vxygrid, PLFLT *xform, PLINT base, PLINT oline, PLINT uline,
  PLINT refx, PLINT refy, PLFLT scale, PLFLT xpmm, PLFLT ypmm,
  PLFLT *p_xorg, PLFLT *p_yorg, PLFLT *p_width);
PLFLT gdl_plstrl(PLCHAR_VECTOR string); 
void gdl_c_plmtex(PLCHAR_VECTOR side, PLFLT disp, PLFLT pos, PLFLT just,
  PLCHAR_VECTOR text);
void gdl_grid_box(PLCHAR_VECTOR xopt, PLFLT xtick1, PLINT nxsub1,
  PLCHAR_VECTOR yopt, PLFLT ytick1, PLINT nysub1);
static PLCHAR_VECTOR gdl_plgesc_string(void);

void gdl_plP_default_label_log(PLINT PL_UNUSED(axis), PLFLT value, char *string, PLINT len, void * PL_UNUSED(data));

void gdl_plP_default_label_log_fixed(PLINT PL_UNUSED(axis), PLFLT value, char *string, PLINT len, void * PL_UNUSED(data));

void gdl_plP_default_label(PLINT PL_UNUSED(axis), PLFLT value, char *string, PLINT len, void *data);
static void gdl_plform(PLINT axis, PLFLT value, PLINT scale, PLINT prec, char *string, PLINT len, PLBOOL ll, PLBOOL lf, PLBOOL lo);
static void gdl_label_box(PLCHAR_VECTOR xopt, PLFLT xtick1, PLCHAR_VECTOR yopt, PLFLT ytick1);

void gdl_c_plaxes(PLFLT x0, PLFLT y0,
  PLCHAR_VECTOR xopt, PLFLT xtick, PLINT nxsub,
  PLCHAR_VECTOR yopt, PLFLT ytick, PLINT nysub);

