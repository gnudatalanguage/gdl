#ifndef export_GDL_DEF
#define export_GDL_DEF

#ifdef __cplusplus
    extern "C" {
#endif
/*
 * Microsoft Windows has two calling conventions, __stdcall and __cdecl. We
 * refer to them using the following macros so that they can be easily
 * nulled on non-MS platforms.
 */
#ifdef MSWIN
#define IDL_STDCALL __stdcall
#define IDL_CDECL __cdecl
#else
#define IDL_STDCALL
#define IDL_CDECL
#endif

#define IDL_REGISTER register   /* Use explicit register declarations */

/*
 * Define IDL_VARIABLE type values - Note that IDL_TYP_UNDEF is always 0 by
 * definition. It is correct to use the value 0 in place of IDL_TYP_UNDEF.
 * It is not correct to assume the value assigned to any other
 * type - the preprocessor definitions below must be used.
 */

#define IDL_TYP_UNDEF		0
#define IDL_TYP_BYTE            1
#define IDL_TYP_INT             2
#define IDL_TYP_LONG            3
#define IDL_TYP_FLOAT           4
#define IDL_TYP_DOUBLE          5
#define IDL_TYP_COMPLEX         6
#define IDL_TYP_STRING          7
#define IDL_TYP_STRUCT          8
#define IDL_TYP_DCOMPLEX        9
#define IDL_TYP_PTR		10
#define IDL_TYP_OBJREF		11
#define IDL_TYP_UINT		12
#define IDL_TYP_ULONG		13
#define IDL_TYP_LONG64		14
#define IDL_TYP_ULONG64		15

#if defined(ALPHA_OSF) || defined(SUN_64) || defined(LINUX_X86_64) || defined(HPUX_64) || defined(IRIX_64) || defined(AIX_64) || defined(darwin_x86_64) || defined(SUN_X86_64)
#define IDL_SIZEOF_C_LONG	8
#else
#define IDL_SIZEOF_C_LONG	4
#endif
#if (IDL_SIZEOF_C_LONG == 8) || defined(MSWIN_64)
#define IDL_SIZEOF_C_PTR	8
#else
#define IDL_SIZEOF_C_PTR	4
#endif

typedef unsigned char UCHAR;	/* Unsigned character type */
typedef short IDL_INT;
typedef unsigned short IDL_UINT;
#if IDL_SIZEOF_C_LONG == 8
typedef int IDL_LONG;
typedef unsigned int IDL_ULONG;
#elif IDL_SIZEOF_C_LONG == 4
typedef long IDL_LONG;
typedef unsigned long IDL_ULONG;
#else
#error "IDL_LONG not defined --- unexpected value of IDL_SIZEOF_C_LONG"
#endif
#ifdef MSWIN
typedef __int64 IDL_LONG64;
typedef unsigned __int64 IDL_ULONG64;
#else
typedef long long IDL_LONG64;
typedef unsigned long long IDL_ULONG64;
#endif
/***** IDL_VARIABLE flag values ********/

#define IDL_V_CONST         1	/* A variable that does not have a name,
				   and which is treated as a static
				   non-assignable expression by the interpreter.
				   The most common example is a lexical
				   constant. Different from V_TEMP in that
				   V_CONST variables are not part of the
				   temporary variable pool, and because IDL
				   can alter the value of a V_TEMP variable
				   under some circumstances. */
#define IDL_V_TEMP          2	/* An unnamed variable from the IDL temporary
				   variable pool, used to hold the results
				   of expressions, and often returned as the
				   result of IDL system functions */
#define IDL_V_ARR           4	/* Variable has an array block, accessed via
				   the value.arr field of IDL_VARIABLE, and
				   the data is kept there. If V_ARR is not
				   set, the variable is scalar, and the value
				   is kept directly within the value union. */
#define IDL_V_FILE          8	/* An ASSOC variable. Note that V_ARR is
				   not set for ASSOC variables, but they
				   still have array blocks. */
#define IDL_V_DYNAMIC       16	 /* Variable contains pointers to other
				    data that must be cleaned up when the
				    variable is destroyed. This happens
				    with scalar strings, or arrays of any
				    type. */
#define IDL_V_STRUCT        32	 /* Variable is a structure. V_ARR is always
				    set when V_STRUCT is set (there are no
				    scalar structures) */
#define IDL_V_NULL          64	 /* Variable is !NULL */
#define IDL_V_BOOLEAN      128	 /* Variable is special Boolean byte type */

#define IDL_V_NOT_SCALAR    (IDL_V_ARR | IDL_V_FILE | IDL_V_STRUCT)

/**** IDL_ARRAY flag values ****/
#define IDL_A_FILE          1	/* Array is a FILE variable (ASSOC) */
#define IDL_A_NO_GUARD      2	/* Indicates no data guards for array */
#define IDL_A_FILE_PACKED   4	/* If array is a FILE variable and the data
				   type is IDL_TYP_STRUCT, then I/O to
				   this struct should assume packed data
				   layout compatible with WRITEU instead of
				   being a direct mapping onto the struct
				   including its alignment holes. */
#define IDL_A_FILE_OFFSET   8	/* Only set with IDL_A_FILE. Indicates that
				   variable has a non-zero offset to the base
				   of the data for the file variable, as
				   contained in the IDL_ARRAY offset field.
				   IDL versions prior to IDL 5.5 did not
				   properly SAVE and RESTORE the offset of
				   a file variable. Introducing this bit in
				   IDL 5.5 makes it possible for us to
				   generate files that older IDLs will be
				   able to read as long as that file does
				   not contain any file variables with
				   non-zero file offsets. If not for this
				   minor compatability win, this bit would
				   serve no significant purpose. */
#define IDL_A_SHM	    16	 /* This array is a shared memory segment */

typedef struct {
  float r,i;
} IDL_COMPLEX;

typedef struct {
  double r,i;
} IDL_DCOMPLEX;

typedef int IDL_STRING_SLEN_T;
#define IDL_STRING_MAX_SLEN 2147483647


typedef struct {		/* Define string descriptor */
  IDL_STRING_SLEN_T slen;	/* Length of string, 0 for null */
  short stype;			/* type of string, static or dynamic */
  char *s;			/* Addr of string */
} IDL_STRING;

/* Type used for pointer and object reference variables is same as IDL_LONG */
typedef IDL_ULONG IDL_HVID;
/*
 * Memory is currently limited to 2^31 on most platforms. If using 64-bit
 * addressing on systems that can do it, we define IDL_MEMINT_64 for the
 * benefit of code that needs to know.
 *
 * MEMINT must always be a signed type.
 */
#if IDL_SIZEOF_C_PTR == 8
#define IDL_MEMINT_64
#define IDL_TYP_MEMINT	 IDL_TYP_LONG64
#define IDL_TYP_UMEMINT	 IDL_TYP_ULONG64
#define IDL_MEMINT	 IDL_LONG64
#define IDL_UMEMINT	 IDL_ULONG64
#elif IDL_SIZEOF_C_PTR == 4
#define IDL_TYP_MEMINT	 IDL_TYP_LONG
#define IDL_TYP_UMEMINT	 IDL_TYP_ULONG
#define IDL_MEMINT	 IDL_LONG
#define IDL_UMEMINT	 IDL_ULONG
#else
#error "IDL_MEMINT not defined --- unexpected value of IDL_SIZEOF_C_PTR "
#endif

#if defined(sun) || defined(ALPHA_OSF) || defined(sgi) || defined(hpux) || defined(MSWIN) || defined(linux) || defined(_AIX) || defined(darwin)
				/* Files can have 64-bit sizes */
#define IDL_FILEINT_64
#define IDL_TYP_FILEINT	  IDL_TYP_LONG64
#define IDL_FILEINT	  IDL_LONG64
#else				/* Stick with 2^31 sized files */
#define IDL_TYP_FILEINT	  IDL_TYP_LONG
#define IDL_FILEINT	  IDL_LONG
#endif
/*
 * IDL_PTRINT is an integer type of the same size as a machine pointer.
 * It can hold a pointer without truncation or excess range.
 * IDL_TYP_PTRINT is the IDL typecode for the IDL type that maps to
 * an IDL_PTRINT.
 */
#if IDL_SIZEOF_C_PTR == 8       /* 64-bit systems */
#define IDL_TYP_PTRINT IDL_TYP_LONG64
#if IDL_SIZEOF_C_LONG == 8      /* LP64 */
typedef long IDL_PTRINT;
#elif defined(MSWIN)
typedef __int64 IDL_PTRINT;
#else
#error "IDL_PTRINT not defined --- unexpected value of IDL_SIZEOF_C_LONG"
#endif
#elif IDL_SIZEOF_C_PTR == 4 /* 32-bit systems --- all ILP32 */
typedef long IDL_PTRINT;
#define IDL_TYP_PTRINT IDL_TYP_LONG
#else
#error "IDL_PTRINT not defined --- unexpected value of IDL_SIZEOF_C_PTR"
#endif

/**** Maximum # of array dimensions ****/
#define IDL_MAX_ARRAY_DIM 8

/* Type of the dim field of an IDL_ARRAY. */
typedef IDL_MEMINT IDL_ARRAY_DIM[IDL_MAX_ARRAY_DIM];
/*
 * Type of the free_cb field of IDL_ARRAY. When IDL deletes a variable and
 * the free_cb field of ARRAY non-NULL, IDL calls the function that field
 * references, passing the value of the data field as it's sole argument.
 * The primary use for this notification is to let programs know when
 * to clean up after calls to IDL_ImportArray(), which is used to create
 * arrays using memory that IDL does not allocate.
 */
typedef void (* IDL_ARRAY_FREE_CB)(UCHAR *data);

typedef struct {		/* Its important that this block
				   be an integer number of longwords
				   in length to ensure that array
				   data is longword aligned.  */
  IDL_MEMINT elt_len;		/* Length of element in char units */
  IDL_MEMINT arr_len;		/* Length of entire array (char) */
  IDL_MEMINT n_elts;		/* total # of elements */
  UCHAR *data;			/* ^ to beginning of array data */
  UCHAR n_dim;			/* # of dimensions used by array */
  UCHAR flags;			/* Array block flags */
  short file_unit;		/* # of assoc file if file var */
  IDL_ARRAY_DIM dim;		/* dimensions */
  IDL_ARRAY_FREE_CB free_cb;	/* Free callback */
  IDL_FILEINT offset;		/* Offset to base of data for file var */
  IDL_MEMINT data_guard;	/* Guard longword */
} IDL_ARRAY;


typedef struct {		/* Reference to a structure */
  IDL_ARRAY *arr;		/* ^ to array block containing data */
  struct _idl_structure *sdef;	/* ^ to structure definition */
} IDL_SREF;

/* IDL_ALLTYPES can be used to represent all IDL_VARIABLE types */
typedef union {
  char sc;			/* A standard char, where "standard" is defined
				   by the compiler. This isn't an IDL data
				   type, but having this field is sometimes
				   useful for internal code */
  UCHAR c;			/* Byte value */
  IDL_INT i;			/* 16-bit integer */
  IDL_UINT ui;			/* 16-bit unsigned integer */
  IDL_LONG l;			/* 32-bit integer */
  IDL_ULONG ul;			/* 32-bit unsigned integer */
  IDL_LONG64 l64;		/* 64-bit integer */
  IDL_ULONG64 ul64;		/* 64-bit unsigned integer */
  float f;			/* 32-bit floating point value */
  double d;			/* 64-bit floating point value */
  IDL_COMPLEX cmp;		/* Complex value */
  IDL_DCOMPLEX dcmp;		/* Double complex value */
  IDL_STRING str;		/* String descriptor */
  IDL_ARRAY *arr;		/* ^ to array descriptor */
  IDL_SREF s;			/* Descriptor of structure */
  IDL_HVID hvid;		/* Heap variable identifier */

  /* The following are mappings to basic types that vary between platforms */
  IDL_MEMINT memint;		/* Memory size or offset */
  IDL_FILEINT fileint;		/* File size or offset */
  IDL_PTRINT ptrint;		/* A pointer size integer */
} IDL_ALLTYPES;

typedef struct {		/* IDL_VARIABLE definition */
  UCHAR type;			/* Type byte */
  UCHAR flags;			/* Flags byte */
  UCHAR flags2;
  IDL_ALLTYPES value;
} IDL_VARIABLE;
typedef IDL_VARIABLE *IDL_VPTR;

/* Signatures for IDL system routines */
typedef void (* IDL_SYSRTN_PRO)(int argc, IDL_VPTR argv[], char *argk);
typedef IDL_VPTR (* IDL_SYSRTN_FUN)(int argc, IDL_VPTR argv[], char *argk);


/* Forward declarations for all exported routines and data */
//
//
//extern void IDL_CDECL IDL_Win32MessageLoop(int fFlush);
//extern int IDL_CDECL IDL_GetExitStatus();
//extern int IDL_CDECL IDL_BailOut(int stop);
//extern int IDL_CDECL IDL_Cleanup(int just_cleanup);
//extern int IDL_CDECL IDL_Initialize(IDL_INIT_DATA *init_data);
//extern int IDL_CDECL IDL_Init(IDL_INIT_DATA_OPTIONS_T options, int *argc,
//        char *argv[]);
//extern int IDL_CDECL IDL_Win32Init(IDL_INIT_DATA_OPTIONS_T iOpts, void
//        *hinstExe, void *hwndExe, void *hAccel);
//extern int IDL_CDECL IDL_Main(IDL_INIT_DATA_OPTIONS_T options, int argc,
//        char *argv[]);
//extern int IDL_CDECL IDL_ExecuteStr(char *cmd);
//extern int IDL_CDECL IDL_Execute(int argc, char *argv[]);
//extern int IDL_CDECL IDL_RuntimeExec(char *file);
//extern void IDL_CDECL IDL_Runtime(IDL_INIT_DATA_OPTIONS_T options, int
//        *argc, char *argv[], char *file);
//extern char *IDL_OutputFormat[];
//extern char *IDL_CDECL IDL_OutputFormatFunc(int type);
//extern int IDL_OutputFormatLen[];
//extern int IDL_CDECL IDL_OutputFormatLenFunc(int type);
//extern char *IDL_OutputFormatFull[];
//extern int IDL_OutputFormatFullLen[];
//extern char *IDL_OutputFormatNatural[];
//extern IDL_LONG IDL_TypeSize[];
//extern int IDL_CDECL IDL_TypeSizeFunc(int type);
//extern char *IDL_TypeName[];
//extern char *IDL_CDECL IDL_TypeNameFunc(int type);
//extern IDL_ALLTYPES IDL_zero;
//extern IDL_VPTR IDL_CDECL IDL_nonavailable_rtn(int argc, IDL_VPTR argv[],
//        char *argk);
//extern IDL_TERMINFO IDL_FileTerm;
//extern char *IDL_CDECL IDL_FileTermName(void);
//extern int IDL_CDECL IDL_FileTermIsTty(void);
//extern int IDL_CDECL IDL_FileTermLines(void);
//extern int IDL_CDECL IDL_FileTermColumns(void);
//extern int IDL_CDECL IDL_FileEnsureStatus(int action, int unit, int flags);
//extern void IDL_CDECL IDL_FileSetMode(int unit, int binary);
//extern int IDL_CDECL IDL_FileOpenUnitBasic(int unit, char *filename, int
//        access_mode, IDL_SFILE_FLAGS_T flags, int msg_action, 
//        IDL_MSG_ERRSTATE_PTR errstate);
//extern int IDL_CDECL IDL_FileOpen(int argc, IDL_VPTR argv[], char *argk,
//        int access_mode, IDL_SFILE_FLAGS_T extra_flags,  int longjmp_safe, int
//        msg_attr);
//extern int IDL_CDECL IDL_FileCloseUnit(int unit, int flags,
//        IDL_SFILE_PIPE_EXIT_STATUS *exit_status,  int msg_action,
//        IDL_MSG_ERRSTATE_PTR errstate);
//extern void IDL_CDECL IDL_FileClose(int argc, IDL_VPTR argv[], char *argk);
//extern void IDL_CDECL IDL_FileFlushUnit(int unit);
//extern void IDL_CDECL IDL_FileGetUnit(int argc, IDL_VPTR argv[]);
//extern void IDL_CDECL IDL_FileFreeUnit(int argc, IDL_VPTR argv[]);
//extern int IDL_CDECL IDL_FileSetPtr(int unit, IDL_FILEINT pos, int extend,
//        int msg_action);
//extern int IDL_CDECL IDL_FileEOF(int unit);
//extern void IDL_CDECL IDL_FileStat(int unit, IDL_FILE_STAT *stat_blk);
//extern void IDL_CDECL IDL_FileSetClose(int unit, int allow);
//extern IDL_VPTR IDL_CDECL IDL_FileVaxFloat(int argc, IDL_VPTR *argv, char
//        *argk);
//extern int IDL_CDECL IDL_SysRtnAdd(IDL_SYSFUN_DEF2 *defs, int is_function,
//        int cnt);
//extern IDL_MEMINT IDL_CDECL IDL_SysRtnNumEnabled(int is_function, int
//        enabled);
//extern void IDL_CDECL IDL_SysRtnGetEnabledNames(int is_function,
//        IDL_STRING *str, int enabled);
//extern void IDL_CDECL IDL_SysRtnEnable(int is_function, IDL_STRING *names,
//        IDL_MEMINT n, int option,  IDL_SYSRTN_GENERIC disfcn);
//extern IDL_SYSRTN_GENERIC IDL_CDECL IDL_SysRtnGetRealPtr(int is_function,
//        char *name);
//extern char *IDL_CDECL IDL_SysRtnGetCurrentName(void);
//extern int IDL_CDECL IDL_LMGRLicenseInfo(int iFlags);
//extern int IDL_CDECL IDL_LMGRSetLicenseInfo(int iFlags);
//extern int IDL_CDECL IDL_LMGRLicenseCheckoutUnique(char *szFeature, char
//        *szVersion);
//extern int IDL_CDECL IDL_LMGRLicenseCheckout(char *szFeature, char
//        *szVersion);
//extern int IDL_CDECL IDL_AddSystemRoutine(IDL_SYSFUN_DEF *defs, int
//        is_function, int cnt);
//extern void IDL_CDECL IDL_StrDup(IDL_REGISTER IDL_STRING *str,
//        IDL_REGISTER IDL_MEMINT n);
//extern void IDL_CDECL IDL_StrDelete(IDL_STRING *str, IDL_MEMINT n);
//extern void IDL_CDECL IDL_StrStore(IDL_STRING *s, const char *fs);
//extern void IDL_CDECL IDL_StrEnsureLength(IDL_STRING *s, int n);
IDL_VPTR IDL_CDECL IDL_StrToSTRING(const char *s);
//extern IDL_VPTR IDL_CDECL IDL_stregex(int argc, IDL_VPTR argv[], char
//        *argk);
//extern void IDL_CDECL IDL_VarGetData(IDL_VPTR v, IDL_MEMINT *n, char **pd,
//        int ensure_simple);
//extern IDL_STRING *IDL_CDECL IDL_VarGet1EltStringDesc(IDL_VPTR v, IDL_VPTR
//        *tc_v, int like_print);
//extern char *IDL_CDECL IDL_VarGetString(IDL_VPTR v);
//extern IDL_VPTR IDL_CDECL IDL_ImportArray(int n_dim, IDL_MEMINT dim[], int
//        type, UCHAR *data, IDL_ARRAY_FREE_CB free_cb,  IDL_StructDefPtr s);
//extern IDL_VPTR IDL_CDECL IDL_ImportNamedArray(char *name, int n_dim,
//        IDL_MEMINT dim[],  int type, UCHAR *data,  IDL_ARRAY_FREE_CB free_cb, 
//        IDL_StructDefPtr s);
//extern void IDL_CDECL IDL_Delvar(IDL_VPTR var);
//extern IDL_VPTR IDL_CDECL IDL_VarTypeConvert(IDL_VPTR v, IDL_REGISTER int
//        type);
//extern void IDL_CDECL IDL_VarEnsureSimple(IDL_VPTR v);
//extern char *IDL_CDECL IDL_VarMakeTempFromTemplate(IDL_VPTR template_var,
//        int type, IDL_StructDefPtr sdef,  IDL_VPTR *result_addr, int zero);
//extern char *IDL_DitherMethodNames[];
//extern char *IDL_CDECL IDL_DitherMethodNamesFunc(int method);
//extern void IDL_CDECL IDL_RasterDrawThick(IDL_GR_PT *p0, IDL_GR_PT *p1,
//        IDL_ATTR_STRUCT *a,  IDL_DEVCORE_FCN_POLYFILL routine,  int dot_width);
//extern void IDL_CDECL IDL_RasterPolyfill(int *x, int *y, int n,
//        IDL_POLYFILL_ATTR *p, IDL_RASTER_DEF *r);
//extern void IDL_CDECL IDL_RasterDraw(IDL_GR_PT *p0, IDL_GR_PT *p1,
//        IDL_ATTR_STRUCT *a, IDL_RASTER_DEF *r);
//extern void IDL_CDECL IDL_Raster8Image(UCHAR *data, IDL_ULONG nx,
//        IDL_ULONG ny, IDL_ULONG x0, IDL_ULONG y0,  IDL_ULONG xsize, IDL_ULONG
//        ysize,  IDL_TV_STRUCT *secondary,  IDL_RASTER_DEF *rs,  IDLBool_t
//        bReverse);
//extern void IDL_CDECL IDL_RasterImage(UCHAR *data, IDL_ULONG nx, IDL_ULONG
//        ny, IDL_ULONG x0, IDL_ULONG y0,  IDL_ULONG xsize, IDL_ULONG ysize, 
//        IDL_TV_STRUCT *secondary,  IDL_RASTER_DEF *rs,  IDLBool_t bReverse);
//extern void IDL_CDECL IDL_Dither(UCHAR *data, int ncols, int nrows,
//        IDL_RASTER_DEF *r, int x0, int y0, IDL_TV_STRUCT *secondary);
//extern void IDL_CDECL IDL_BitmapLandscape(IDL_RASTER_DEF *in,
//        IDL_RASTER_DEF *out, int y0);
//extern IDL_VPTR IDL_CDECL IDL_transpose(int argc, IDL_VPTR *argv);
//extern int IDL_CDECL IDL_KWProcessByOffset(int argc, IDL_VPTR *argv, char
//        *argk, IDL_KW_PAR *kw_list, IDL_VPTR *plain_args,  int mask, void *base);
//extern int IDL_CDECL IDL_KWProcessByAddr(int argc, IDL_VPTR *argv, char
//        *argk, IDL_KW_PAR *kw_list, IDL_VPTR *plain_args,  int mask, int
//        *free_required);
//extern void IDL_CDECL IDL_KWFree(void);
//extern void IDL_CDECL IDL_KWFreeAll(void);
//extern IDL_VPTR IDL_CDECL IDL_CvtBytscl(int argc, IDL_VPTR *argv, char
//        *argk);
//extern void IDL_CDECL IDL_Freetmp(IDL_REGISTER IDL_VPTR p);
//extern void IDL_CDECL IDL_Deltmp(IDL_REGISTER IDL_VPTR p);
//extern IDL_VPTR IDL_CDECL IDL_Gettmp(void);
//extern IDL_VPTR IDL_CDECL IDL_GettmpByte(UCHAR value);
//extern IDL_VPTR IDL_CDECL IDL_GettmpInt(IDL_INT value);
//extern IDL_VPTR IDL_CDECL IDL_GettmpLong(IDL_LONG value);
//extern IDL_VPTR IDL_CDECL IDL_GettmpFloat(float value);
//extern IDL_VPTR IDL_CDECL IDL_GettmpDouble(double value);
//extern IDL_VPTR IDL_CDECL IDL_GettmpPtr(IDL_HVID value);
//extern IDL_VPTR IDL_CDECL IDL_GettmpObjRef(IDL_HVID value);
//extern IDL_VPTR IDL_CDECL IDL_GettmpUInt(IDL_UINT value);
//extern IDL_VPTR IDL_CDECL IDL_GettmpULong(IDL_ULONG value);
//extern IDL_VPTR IDL_CDECL IDL_GettmpLong64(IDL_LONG64 value);
//extern IDL_VPTR IDL_CDECL IDL_GettmpULong64(IDL_ULONG64 value);
//extern IDL_VPTR IDL_CDECL IDL_GettmpFILEINT(IDL_FILEINT value);
//extern IDL_VPTR IDL_CDECL IDL_GettmpMEMINT(IDL_MEMINT value);
//extern char *IDL_CDECL IDL_GetScratch(IDL_REGISTER IDL_VPTR *p,
//        IDL_REGISTER IDL_MEMINT n_elts,  IDL_REGISTER IDL_MEMINT elt_size);
//extern char *IDL_CDECL IDL_GetScratchOnThreshold(IDL_REGISTER char
//        *auto_buf, IDL_REGISTER IDL_MEMINT auto_elts,  IDL_REGISTER IDL_MEMINT
//        n_elts,  IDL_REGISTER IDL_MEMINT elt_size,  IDL_VPTR *tempvar);
//extern void IDL_CDECL IDL_VarCopy(IDL_REGISTER IDL_VPTR src, IDL_REGISTER
//        IDL_VPTR dst);
//extern void IDL_CDECL IDL_StoreScalar(IDL_VPTR dest, int type,
//        IDL_ALLTYPES *value);
//extern void IDL_CDECL IDL_StoreScalarZero(IDL_VPTR dest, int type);
//extern void IDL_CDECL IDL_unform_io(int type, int argc, IDL_VPTR *argv,
//        char *argk);
//extern void IDL_CDECL IDL_Print(int argc, IDL_VPTR *argv, char *argk);
//extern void IDL_CDECL IDL_PrintF(int argc, IDL_VPTR *argv, char *argk);
//extern IDL_VPTR IDL_CDECL IDL_String_Remove(int argc, IDL_VPTR *argv, char
//        *argk);
//extern IDL_VPTR IDL_CDECL IDL_Variable_Diff(int argc, IDL_VPTR *argv, char
//        *argk);
//extern IDL_VPTR IDL_CDECL IDL_BasicTypeConversion(int argc, IDL_VPTR
//        argv[], IDL_REGISTER int type);
//extern IDL_VPTR IDL_CDECL IDL_CvtByte(int argc, IDL_VPTR argv[]);
//extern IDL_VPTR IDL_CDECL IDL_CvtFix(int argc, IDL_VPTR argv[]);
//IDL_VPTR IDL_CDECL IDL_CvtLng(int argc, IDL_VPTR argv[]);
//extern IDL_VPTR IDL_CDECL IDL_CvtFlt(int argc, IDL_VPTR argv[]);
//extern IDL_VPTR IDL_CDECL IDL_CvtDbl(int argc, IDL_VPTR argv[]);
//extern IDL_VPTR IDL_CDECL IDL_CvtUInt(int argc, IDL_VPTR argv[]);
//extern IDL_VPTR IDL_CDECL IDL_CvtULng(int argc, IDL_VPTR argv[]);
//extern IDL_VPTR IDL_CDECL IDL_CvtLng64(int argc, IDL_VPTR argv[]);
//extern IDL_VPTR IDL_CDECL IDL_CvtULng64(int argc, IDL_VPTR argv[]);
//extern IDL_VPTR IDL_CDECL IDL_CvtMEMINT(int argc, IDL_VPTR argv[]);
//extern IDL_VPTR IDL_CDECL IDL_CvtFILEINT(int argc, IDL_VPTR argv[]);
//extern IDL_VPTR IDL_CDECL IDL_CvtComplex(int argc, IDL_VPTR argv[], char
//        *argk);
//extern IDL_VPTR IDL_CDECL IDL_CvtDComplex(int argc, IDL_VPTR argv[]);
//extern IDL_VPTR IDL_CDECL IDL_CvtString(int argc, IDL_VPTR argv[], char
//        *argk);
//extern IDL_LONG IDL_CDECL IDL_grMesh_Clip(float fPlane[4], short clipSide,
//         float *pfVin, IDL_LONG iNVerts,  IDL_LONG *piCin, IDL_LONG iNConn, 
//        float **pfVout, IDL_LONG *iNVout,  IDL_LONG **piCout, IDL_LONG *iNCout, 
//        IDL_VPTR vpAuxInKW, IDL_VPTR vpAuxOutKW,  IDL_VPTR vpCut);
//extern IDL_LONG IDL_CDECL IDL_LongScalar(IDL_REGISTER IDL_VPTR v);
//extern IDL_ULONG IDL_CDECL IDL_ULongScalar(IDL_REGISTER IDL_VPTR v);
//extern IDL_LONG64 IDL_CDECL IDL_Long64Scalar(IDL_REGISTER IDL_VPTR v);
//extern IDL_ULONG64 IDL_CDECL IDL_ULong64Scalar(IDL_REGISTER IDL_VPTR v);
//extern double IDL_CDECL IDL_DoubleScalar(IDL_REGISTER IDL_VPTR v);
//extern IDL_MEMINT IDL_CDECL IDL_MEMINTScalar(IDL_REGISTER IDL_VPTR v);
//extern IDL_FILEINT IDL_CDECL IDL_FILEINTScalar(IDL_REGISTER IDL_VPTR v);
//extern float IDL_CDECL IDL_CastUL64_f(IDL_ULONG64 value);
//extern double IDL_CDECL IDL_CastUL64_d(IDL_ULONG64 value);
//extern IDL_ULONG IDL_CDECL IDL_CastFloat_UL(float value);
//extern IDL_ULONG IDL_CDECL IDL_CastDouble_UL(double value);
//extern int IDL_CDECL IDL_StrBase_strcasecmp(const char *str1, const char
//        *str2);
//extern int IDL_CDECL IDL_StrBase_strncasecmp(const char *str1, const char
//        *str2, size_t nchars);
//extern size_t IDL_CDECL IDL_StrBase_strlcpy(char *dst, const char *src,
//        size_t siz);
//extern size_t IDL_CDECL IDL_StrBase_strlcat(char *dst, const char *src,
//        size_t siz);
//extern size_t IDL_CDECL IDL_StrBase_strlcatW(wchar_t *dst, const wchar_t
//        *src, size_t siz);
//extern int IDL_CDECL IDL_StrBase_vsnprintf(char *s, size_t n, const char
//        *format, va_list args);
//extern int IDL_CDECL IDL_StrBase_snprintf(char *s, size_t n, const char
//        *format, ...);
//extern int IDL_CDECL IDL_StrBase_strbcopy(char *dst, const char *src,
//        size_t siz);
//extern int IDL_CDECL IDL_StrBase_strbcopyW(wchar_t *dst, const wchar_t
//        *src, size_t siz);
//extern void IDL_CDECL IDL_RgbToHsv(UCHAR *r, UCHAR *g, UCHAR *b,  float
//        *h, float *s, float *v, int n);
//extern void IDL_CDECL IDL_RgbToHls(UCHAR *r, UCHAR *g, UCHAR *b,  float
//        *h, float *l, float *s, int n);
//extern void IDL_CDECL IDL_HsvToRgb(float *h, float *s, float *v,  UCHAR
//        *r, UCHAR *g, UCHAR *b, int n);
//extern void IDL_CDECL IDL_HlsToRgb(float *h, float *l, float *s, UCHAR *r,
//        UCHAR *g, UCHAR *b, int n);
//extern int IDL_CDECL IDL_AddDevice( IDL_DEVICE_DEF *dev,  int msg_action);
//extern char *IDL_CDECL IDL_MakeTempStruct(IDL_StructDefPtr sdef, int
//        n_dim, IDL_MEMINT *dim, IDL_VPTR *var, int zero);
//extern char *IDL_CDECL IDL_MakeTempStructVector(IDL_StructDefPtr sdef,
//        IDL_MEMINT dim, IDL_VPTR *var, int zero);
//extern IDL_StructDefPtr IDL_CDECL IDL_MakeStruct(char *name,
//        IDL_STRUCT_TAG_DEF *tags);
//extern IDL_MEMINT IDL_CDECL IDL_StructTagInfoByName(IDL_StructDefPtr sdef,
//        char *name, int msg_action, IDL_VPTR *var);
//extern IDL_MEMINT IDL_CDECL IDL_StructTagInfoByIndex(IDL_StructDefPtr
//        sdef, int index, int msg_action, IDL_VPTR *var);
//extern char *IDL_CDECL IDL_StructTagNameByIndex(IDL_StructDefPtr sdef, int
//        index, int msg_action, char **struct_name);
//extern int IDL_CDECL IDL_StructNumTags(IDL_StructDefPtr sdef);
//extern int IDL_STDCALL IDL_InitOCX(void *pInit);
//extern char *IDL_CDECL IDL_FilePathFromRoot(int flags, char *pathbuf, char
//        *root, char *file, char *ext,  int nsubdir, char **subdir);
//extern wchar_t *IDL_CDECL IDL_FilePathFromRootW(int flags, wchar_t
//        *pathbuf, wchar_t *root, wchar_t *file, wchar_t *ext,  int nsubdir,
//        wchar_t **subdir);
//extern char *IDL_CDECL IDL_FilePathFromDist(int flags, char *pathbuf, char
//        *file, char *ext, int nsubdir, char **subdir);
//extern wchar_t *IDL_CDECL IDL_FilePathFromDistW(int flags, wchar_t
//        *pathbuf, wchar_t *file, wchar_t *ext, int nsubdir, wchar_t **subdir);
//extern char *IDL_CDECL IDL_FilePathFromDistBin(int flags, char *pathbuf,
//        char *file, char *ext);
//extern wchar_t *IDL_CDECL IDL_FilePathFromDistBinW(int flags, wchar_t
//        *pathbuf, wchar_t *file, wchar_t *ext);
//extern char *IDL_CDECL IDL_FilePathFromDistHelp(int flags, char *pathbuf,
//        char *file, char *ext);
//extern wchar_t *IDL_CDECL IDL_FilePathFromDistHelpW(int flags, wchar_t
//        *pathbuf, wchar_t *file, wchar_t *ext);
//extern void IDL_CDECL IDL_FilePathGetTmpDir(char *path);
//extern void IDL_CDECL IDL_FilePathGetTmpDirW(wchar_t *path);
//extern int IDL_CDECL IDL_FilePathExpand(char *path, int msg_action);
//extern int IDL_CDECL IDL_FilePathExpandW(wchar_t *wcharPath, int
//        msg_action);
//extern IDL_VPTR IDL_CDECL IDL_FilePathSearch(int argc, IDL_VPTR *argv,
//        char *argk);
//extern IDL_VPTR IDL_CDECL IDL_FilePathSearch(int argc, IDL_VPTR *argv,
//        char *argk);
//extern void IDL_CDECL IDL_TerminalRaw(int to_from, int fnin);
//extern void IDL_CDECL IDL_Pout(IDL_POUT_CNTRL *control, int flags, char
//        *fmt, ...);
//extern void IDL_CDECL IDL_PoutVa(IDL_POUT_CNTRL *control, int flags, char
//        *fmt, va_list *args);
//extern void IDL_CDECL IDL_PoutRaw(int unit, char *buf, int n);
//extern void IDL_CDECL IDL_EzCall(int argc, IDL_VPTR argv[], IDL_EZ_ARG
//        arg_struct[]);
//extern void IDL_CDECL IDL_EzCallCleanup(int argc, IDL_VPTR argv[],
//        IDL_EZ_ARG arg_struct[]);
//extern void IDL_CDECL IDL_EzReplaceWithTranspose(IDL_VPTR *v, IDL_VPTR
//        orig);
//extern char *IDL_CDECL IDL_MakeTempArray(int type, int n_dim, IDL_MEMINT
//        dim[], int init, IDL_VPTR *var);
//extern char *IDL_CDECL IDL_MakeTempVector(int type, IDL_MEMINT dim, int
//        init, IDL_VPTR *var);
//extern void IDL_CDECL IDL_TimerSet(IDL_LONG length, IDL_TIMER_CB callback,
//        int from_callback, IDL_TIMER_CONTEXT_PTR context);
//extern void IDL_CDECL IDL_TimerCancel(IDL_TIMER_CONTEXT context);
//extern void IDL_CDECL IDL_TimerBlock(int stop);
//extern void IDL_CDECL IDL_ToutPush(IDL_TOUT_OUTF outf);
//extern IDL_TOUT_OUTF IDL_CDECL IDL_ToutPop(void);
//extern void IDL_CDECL IDL_CvtVAXToFloat(float *fp, IDL_MEMINT n);
//extern void IDL_CDECL IDL_CvtFloatToVAX(float *fp, IDL_MEMINT n);
//extern void IDL_CDECL IDL_CvtVAXToDouble(double *dp, IDL_MEMINT n);
//extern void IDL_CDECL IDL_CvtDoubleToVAX(double *dp, IDL_MEMINT n);
//extern void IDL_CDECL IDL_ExitRegister(IDL_EXIT_HANDLER_FUNC proc);
//extern void IDL_CDECL IDL_ExitUnregister(IDL_EXIT_HANDLER_FUNC proc);
//extern void IDL_CDECL IDL_WidgetIssueStubEvent(char *rec, IDL_LONG value);
//extern void IDL_CDECL IDL_WidgetSetStubIds(char *rec, unsigned long t_id,
//        unsigned long b_id);
//extern void IDL_CDECL IDL_WidgetGetStubIds(char *rec, unsigned long *t_id,
//        unsigned long *b_id);
//extern void IDL_CDECL IDL_WidgetStubLock(int set);
//extern void *IDL_CDECL IDL_WidgetStubGetParent(IDL_ULONG id, char
//        *szDisplay);
//extern char *IDL_CDECL IDL_WidgetStubLookup(IDL_ULONG id);
//extern void IDL_CDECL IDL_WidgetStubSetSizeFunc(char *rec,
//        IDL_WIDGET_STUB_SET_SIZE_FUNC func);
//extern void IDL_CDECL IDL_Wait(int argc, IDL_VPTR argv[]);
//extern void IDL_CDECL IDL_GetUserInfo(IDL_USER_INFO *user_info);
//extern int IDL_CDECL IDL_GetKbrd(int should_wait);
//extern void IDL_CDECL IDL_TTYReset(void);
//extern IDL_PLOT_COM IDL_PlotCom;
//extern UCHAR IDL_ColorMap[];
//extern IDL_PLOT_COM *IDL_CDECL IDL_PlotComAddr(void);
//extern UCHAR *IDL_CDECL IDL_ColorMapAddr(void);
//extern void IDL_CDECL IDL_PolyfillSoftware(int *x, int *y, int n,
//        IDL_POLYFILL_ATTR *s);
//extern double IDL_CDECL IDL_GraphText(IDL_GR_PT *p, IDL_ATTR_STRUCT *ga,
//        IDL_TEXT_STRUCT *a, char *text);
//extern char *IDL_CDECL IDL_VarName(IDL_VPTR v);
//extern IDL_VPTR IDL_CDECL IDL_GetVarAddr1(char *name, int ienter);
//extern IDL_VPTR IDL_CDECL IDL_GetVarAddr(char *name);
//extern IDL_VPTR IDL_CDECL IDL_FindNamedVariable(char *name, int ienter);
//extern IDL_HEAP_VPTR IDL_CDECL IDL_HeapVarHashFind(IDL_HVID hash_id);
//extern IDL_HEAP_VPTR IDL_CDECL IDL_HeapVarNew(int hvid_type, IDL_VPTR
//        value, int flags, int msg_action);
//extxtern IDL_STRING *IDL_CDECL IDL_SysvVersionArch(void);
//extern IDL_STRING *IDL_CDECL IDL_SysvVersionOS(void);
//extern IDL_STRING *IDL_CDECL IDL_SysvVersionOSFamily(void);
//extern IDL_STRING *IDL_CDECL IDL_SysvVersionRelease(void);
//extern IDL_CPU_STRUCT IDL_SysvCpu;
//extern char *IDL_ProgramName;
//extern char *IDL_CDECL IDL_ProgramNameFunc(void);
//extern char *IDL_ProgramNameLC;
//extern char *IDL_CDECL IDL_ProgramNameLCFunc(void);
//extern IDL_STRING IDL_SysvDir;
//extern IDL_STRING *IDL_CDECL IDL_SysvDirFunc(void);
//extern IDL_LONG IDL_SysvErrCode;
//extern IDL_LONG IDL_CDECL IDL_SysvErrCodeValue(void);
//extern IDL_SYS_ERROR_STATE IDL_SysvErrorState;
//extern IDL_SYS_ERROR_STATE *IDL_CDECL IDL_SysvErrorStateAddr(void);
//extern IDL_STRING *IDL_CDECL IDL_SysvErrStringFunc(void);
//extern IDL_STRING *IDL_CDECL IDL_SysvSyserrStringFunc(void);
//extern IDL_LONG IDL_CDECL IDL_SysvErrorCodeValue(void);
//extern IDL_LONG *IDL_CDECL IDL_SysvSyserrorCodesAddr(void);
//extern IDL_LONG IDL_SysvOrder;
//extern IDL_LONG IDL_CDECL IDL_SysvOrderValue(void);
//extern float IDL_CDECL IDL_SysvValuesGetFloat(int type);
//extern double IDL_CDECL IDL_SysvValuesGetDouble(int type);
//extern int IDL_CDECL IDL_MessageNameToCode(IDL_MSG_BLOCK block, const char
//        *name);
//extern IDL_MSG_BLOCK IDL_CDECL IDL_MessageDefineBlock(char *block_name,
//        int n, IDL_MSG_DEF *defs);
//extern void IDL_CDECL IDL_MessageErrno(int code, ...);
//extern void IDL_CDECL IDL_MessageErrnoFromBlock(IDL_MSG_BLOCK block, int
//        code, ...);
//extern void IDL_CDECL IDL_Message(int code, int action, ...);
//extern void IDL_CDECL IDL_MessageFromBlock(IDL_MSG_BLOCK block, int code,
//        int action,...);
//extern void IDL_CDECL IDL_MessageSyscode(int code, IDL_MSG_SYSCODE_T
//        syscode_type, int syscode, int action, ...);
//extern void IDL_CDECL IDL_MessageSyscodeFromBlock(IDL_MSG_BLOCK block, int
//        code, IDL_MSG_SYSCODE_T syscode_type,  int syscode, int action, ...);
//extern void IDL_CDECL IDL_MessageVarError(int code, IDL_VPTR var, int
//        action);
//extern void IDL_CDECL IDL_MessageVarErrorFromBlock(IDL_MSG_BLOCK block,
//        int code, IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageResetSysvErrorState(void);
//extern void IDL_CDECL IDL_MessageSJE(void *value);
//extern void *IDL_CDECL IDL_MessageGJE(void);
//extern void IDL_CDECL IDL_MessageVE_UNDEFVAR(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOTARRAY(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOTSCALAR(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOEXPR(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOCONST(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOFILE(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOCOMPLEX(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOSTRING(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOSTRUCT(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_REQSTR(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOSCALAR(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOMEMINT64(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_STRUC_REQ(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_REQPTR(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_REQOBJREF(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_Message_BADARRDNUM(int action);
//extern void IDL_CDECL IDL_SignalSetInit(IDL_SignalSet_t *set, int signo);
//extern void IDL_CDECL IDL_SignalSetAdd(IDL_SignalSet_t *set, int signo);
//extern void IDL_CDECL IDL_SignalSetDel(IDL_SignalSet_t *set, int signo);
//extern int IDL_CDECL IDL_SignalSetIsMember(IDL_SignalSet_t *set, int
//        signo);
//extern void IDL_CDECL IDL_SignalMaskGet(IDL_SignalSet_t *set);
//extern void IDL_CDECL IDL_SignalMaskSet(IDL_SignalSet_t *set,
//        IDL_SignalSet_t *oset);
//extern void IDL_CDECL IDL_SignalMaskBlock(IDL_SignalSet_t *set,
//        IDL_SignalSet_t *oset);
//extern void IDL_CDECL IDL_SignalBlock(int signo, IDL_SignalSet_t *oset);
//extern void IDL_CDECL IDL_SignalSuspend(IDL_SignalSet_t *set);
//extern int IDL_CDECL IDL_SignalRegister(int signo, IDL_SignalHandler_t
//        func, int msg_action);
//extern int IDL_CDECL IDL_SignalUnregister(int signo, IDL_SignalHandler_t
//        func, int msg_action);
//extern int IDL_CDECL IDL_SetValue(int id, void* pvValue);
//extern void IDL_CDECL IDL_WinPostInit(void);
//extern void IDL_CDECL IDL_WinCleanup(void);
//extern void *IDL_CDECL IDL_MemAlloc(IDL_MEMINT n, const char *err_str, int
//        msg_action);
//extern void *IDL_CDECL IDL_MemRealloc(void *ptr, IDL_MEMINT n, const char
//        *err_str, int action);
//extern void IDL_CDECL IDL_MemFree(IDL_REGISTER void *m, const char
//        *err_str, int msg_action);
//extern void *IDL_CDECL IDL_MemAllocPerm(IDL_MEMINT n, const char *err_str,
//        int action);ern void IDL_CDECL IDL_HeapIncrRefCount(IDL_HVID *hvid, IDL_MEMINT n);
//extern char *IDL_CDECL IDL_Rline(char *s, IDL_MEMINT n, int unit, FILE
//        *stream, int is_tty, char *prompt, int opt);
//extern void IDL_CDECL IDL_RlineSetStdinOptions(int opt);
//extern void IDL_CDECL IDL_Logit(char *s);
//extern IDL_SYS_VERSION IDL_SysvVersion;
//extern IDL_STRING *IDL_CDECL IDL_SysvVersionArch(void);
//extern IDL_STRING *IDL_CDECL IDL_SysvVersionOS(void);
//extern IDL_STRING *IDL_CDECL IDL_SysvVersionOSFamily(void);
//extern IDL_STRING *IDL_CDECL IDL_SysvVersionRelease(void);
//extern IDL_CPU_STRUCT IDL_SysvCpu;
//extern char *IDL_ProgramName;
//extern char *IDL_CDECL IDL_ProgramNameFunc(void);
//extern char *IDL_ProgramNameLC;
//extern char *IDL_CDECL IDL_ProgramNameLCFunc(void);
//extern IDL_STRING IDL_SysvDir;
//extern IDL_STRING *IDL_CDECL IDL_SysvDirFunc(void);
//extern IDL_LONG IDL_SysvErrCode;
//extern IDL_LONG IDL_CDECL IDL_SysvErrCodeValue(void);
//extern IDL_SYS_ERROR_STATE IDL_SysvErrorState;
//extern IDL_SYS_ERROR_STATE *IDL_CDECL IDL_SysvErrorStateAddr(void);
//extern IDL_STRING *IDL_CDECL IDL_SysvErrStringFunc(void);
//extern IDL_STRING *IDL_CDECL IDL_SysvSyserrStringFunc(void);
//extern IDL_LONG IDL_CDECL IDL_SysvErrorCodeValue(void);
//extern IDL_LONG *IDL_CDECL IDL_SysvSyserrorCodesAddr(void);
//extern IDL_LONG IDL_SysvOrder;
//extern IDL_LONG IDL_CDECL IDL_SysvOrderValue(void);
//extern float IDL_CDECL IDL_SysvValuesGetFloat(int type);
//extern double IDL_CDECL IDL_SysvValuesGetDouble(int type);
//extern int IDL_CDECL IDL_MessageNameToCode(IDL_MSG_BLOCK block, const char
//        *name);
//extern IDL_MSG_BLOCK IDL_CDECL IDL_MessageDefineBlock(char *block_name,
//        int n, IDL_MSG_DEF *defs);
//extern void IDL_CDECL IDL_MessageErrno(int code, ...);
//extern void IDL_CDECL IDL_MessageErrnoFromBlock(IDL_MSG_BLOCK block, int
//        code, ...);
//extern void IDL_CDECL IDL_Message(int code, int action, ...);
//extern void IDL_CDECL IDL_MessageFromBlock(IDL_MSG_BLOCK block, int code,
//        int action,...);
//extern void IDL_CDECL IDL_MessageSyscode(int code, IDL_MSG_SYSCODE_T
//        syscode_type, int syscode, int action, ...);
//extern void IDL_CDECL IDL_MessageSyscodeFromBlock(IDL_MSG_BLOCK block, int
//        code, IDL_MSG_SYSCODE_T syscode_type,  int syscode, int action, ...);
//extern void IDL_CDECL IDL_MessageVarError(int code, IDL_VPTR var, int
//        action);
//extern void IDL_CDECL IDL_MessageVarErrorFromBlock(IDL_MSG_BLOCK block,
//        int code, IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageResetSysvErrorState(void);
//extern void IDL_CDECL IDL_MessageSJE(void *value);
//extern void *IDL_CDECL IDL_MessageGJE(void);
//extern void IDL_CDECL IDL_MessageVE_UNDEFVAR(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOTARRAY(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOTSCALAR(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOEXPR(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOCONST(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOFILE(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOCOMPLEX(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOSTRING(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOSTRUCT(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_REQSTR(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOSCALAR(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_NOMEMINT64(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_STRUC_REQ(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_REQPTR(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_MessageVE_REQOBJREF(IDL_VPTR var, int action);
//extern void IDL_CDECL IDL_Message_BADARRDNUM(int action);
//extern void IDL_CDECL IDL_SignalSetInit(IDL_SignalSet_t *set, int signo);
//extern void IDL_CDECL IDL_SignalSetAdd(IDL_SignalSet_t *set, int signo);
//extern void IDL_CDECL IDL_SignalSetDel(IDL_SignalSet_t *set, int signo);
//extern int IDL_CDECL IDL_SignalSetIsMember(IDL_SignalSet_t *set, int
//        signo);
//extern void IDL_CDECL IDL_SignalMaskGet(IDL_SignalSet_t *set);
//extern void IDL_CDECL IDL_SignalMaskSet(IDL_SignalSet_t *set,
//        IDL_SignalSet_t *oset);
//extern void IDL_CDECL IDL_SignalMaskBlock(IDL_SignalSet_t *set,
//        IDL_SignalSet_t *oset);
//extern void IDL_CDECL IDL_SignalBlock(int signo, IDL_SignalSet_t *oset);
//extern void IDL_CDECL IDL_SignalSuspend(IDL_SignalSet_t *set);
//extern int IDL_CDECL IDL_SignalRegister(int signo, IDL_SignalHandler_t
//        func, int msg_action);
//extern int IDL_CDECL IDL_SignalUnregister(int signo, IDL_SignalHandler_t
//        func, int msg_action);
//extern int IDL_CDECL IDL_SetValue(int id, void* pvValue);
//extern void IDL_CDECL IDL_WinPostInit(void);
//extern void IDL_CDECL IDL_WinCleanup(void);
//extern void *IDL_CDECL IDL_MemAlloc(IDL_MEMINT n, const char *err_str, int
//        msg_action);
//extern void *IDL_CDECL IDL_MemRealloc(void *ptr, IDL_MEMINT n, const char
//        *err_str, int action);
//extern void IDL_CDECL IDL_MemFree(IDL_REGISTER void *m, const char
//        *err_str, int msg_action);
//extern void *IDL_CDECL IDL_MemAllocPerm(IDL_MEMINT n, const char *err_str,
//        int action);
//

#ifdef __cplusplus
    }
#endif

#endif 
