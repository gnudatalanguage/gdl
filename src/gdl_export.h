#ifndef export_GDL_DEF
#define export_GDL_DEF
#ifdef MSWIN
#define GDL_STDCALL __stdcall
#define GDL_CDECL __cdecl
#else
#define GDL_STDCALL
#define GDL_CDECL
#endif

#if defined (__unix) || defined(__unix__) || defined(__APPLE__)
#define GDL_OS_HAS_TTYS
#endif
#define GDL_REGISTER

typedef DByte UCHAR ; //typedef unsigned char UCHAR;	/* Unsigned character type */
typedef DInt EXPORT_INT;
typedef DUInt EXPORT_UINT;
typedef DLong EXPORT_LONG;
typedef DLong64 EXPORT_LONG64;
typedef DULong EXPORT_ULONG;
typedef DULong64 EXPORT_ULONG64;

typedef EXPORT_ULONG EXPORT_HVID;

#define EXPORT_NUM_TYPES           16
#define EXPORT_MAX_TYPE           15

#define GDL_TYP_UNDEF	GDL_UNDEF
#define GDL_TYP_BYTE     GDL_BYTE
#define GDL_TYP_INT      GDL_INT
#define GDL_TYP_LONG     GDL_LONG
#define GDL_TYP_FLOAT    GDL_FLOAT
#define GDL_TYP_DOUBLE   GDL_DOUBLE
#define GDL_TYP_COMPLEX  GDL_COMPLEX
#define GDL_TYP_STRING   GDL_STRING
#define GDL_TYP_STRUCT   GDL_STRUCT
#define GDL_TYP_DCOMPLEX  GDL_COMPLEXDBL
#define GDL_TYP_PTR		GDL_PTR
#define GDL_TYP_OBJREF		GDL_OBJ
#define GDL_TYP_UINT		GDL_UINT
#define GDL_TYP_ULONG		GDL_ULONG
#define GDL_TYP_LONG64		GDL_LONG64
#define GDL_TYP_ULONG64		GDL_ULONG64

#define EXPORT_MEMINT	 SizeT
#define EXPORT_FILEINT SizeT
#define EXPORT_PTRINT  SizeT
#ifdef __x86_64__
#define GDL_TYP_MEMINT	 GDL_TYP_ULONG64
#define GDL_TYP_UMEMINT	 GDL_TYP_ULONG64
#else
#define GDL_TYP_MEMINT	 GDL_TYP_ULONG
#define GDL_TYP_UMEMINT	 GDL_TYP_ULONG
#endif

#define EXPORT_FILEINT_64 DULong64
#define GDL_TYP_FILEINT	  GDL_TYP_LONG64
#define GDL_TYP_B_SIMPLE            62207

#define GDL_V_CONST         1	
#define GDL_V_TEMP          2
#define GDL_V_ARR           4
#define GDL_V_FILE          8
#define GDL_V_DYNAMIC       16
#define GDL_V_STRUCT        32
#define GDL_V_NULL          64
#define GDL_V_BOOLEAN      128
#define GDL_V_NOT_SCALAR    (GDL_V_ARR | GDL_V_FILE | GDL_V_STRUCT)
#define GDL_A_FILE          1
#define GDL_A_NO_GUARD      2
#define GDL_A_FILE_PACKED   4
#define GDL_A_FILE_OFFSET   8
#define GDL_A_SHM	    16


typedef struct {
  float r,i;
} EXPORT_COMPLEX;

typedef struct {
  double r,i;
} EXPORT_DCOMPLEX;

typedef int EXPORT_STRING_SLEN_T;
#define EXPORT_STRING_MAX_SLEN 2147483647


typedef struct {		/* Define string descriptor */
  EXPORT_STRING_SLEN_T slen;	/* Length of string, 0 for null */
  short stype;			/* type of string, static or dynamic */
  char *s;			/* Addr of string */
} EXPORT_STRING;

typedef struct _idlgdl_ident {
  struct _idlgdl_ident *hash;	/* Must be the first field */
  char *name;                   /* Identifier text (NULL terminated */
  int len;			/* # of characters in id, not counting NULL
				   termination. */
} EXPORT_IDENT;

/* The following define the valid values for the init arg to basic_array */
#define GDL_ARR_INI_ZERO   0	/* Zero data area */
#define GDL_ARR_INI_NOP    1	/* Don't do anything to data area */
#define GDL_ARR_INI_INDEX  2	/* Put 1-D index into each elt. */
#define GDL_ARR_INI_TEST   3	/* Test if enough memory is available */

typedef void (* EXPORT_ARRAY_FREE_CB)(UCHAR *data);
typedef EXPORT_MEMINT EXPORT_ARRAY_DIM[MAXRANK];
typedef struct {
  EXPORT_MEMINT elt_len;		/* Length of element in char units */
  EXPORT_MEMINT arr_len;		/* Length of entire array (char) */
  EXPORT_MEMINT n_elts;		/* total # of elements */
  UCHAR *data;			/* ^ to beginning of array data */
  UCHAR n_dim;			/* # of dimensions used by array */
  UCHAR flags;			/* Array block flags */
  short file_unit;		/* # of assoc file if file var */
  EXPORT_ARRAY_DIM dim;		/* dimensions */
  EXPORT_ARRAY_FREE_CB free_cb;	/* Free callback */
  EXPORT_FILEINT offset;		/* Offset to base of data for file var */
  EXPORT_MEMINT data_guard;	/* Guard longword */
} EXPORT_ARRAY;

typedef struct {		/* Reference to a structure */
  EXPORT_ARRAY *arr;		/* ^ to array block containing data */
  struct _idlgdl_structure *sdef;	/* ^ to structure definition */
} EXPORT_SREF;

typedef union {
  char sc;
  UCHAR c;
  EXPORT_INT i;			
  EXPORT_UINT ui;		
  EXPORT_LONG l;		
  EXPORT_ULONG ul;		
  EXPORT_LONG64 l64;	
  EXPORT_ULONG64 ul64;	
  float f;
  double d;
  EXPORT_COMPLEX cmp;	
  EXPORT_DCOMPLEX dcmp;	
  EXPORT_STRING str;	
  EXPORT_ARRAY *arr;	
  EXPORT_SREF s;		
  EXPORT_HVID hvid;		
  EXPORT_MEMINT memint;	
  EXPORT_FILEINT fileint;
  EXPORT_PTRINT ptrint;	
} EXPORT_ALLTYPES;

typedef struct {
  UCHAR type;	
  UCHAR flags;	
  UCHAR flags2;
  EXPORT_ALLTYPES value;
} EXPORT_VARIABLE;
typedef EXPORT_VARIABLE *EXPORT_VPTR;

typedef void (* EXPORT_SYSRTN_PRO)(int argc, EXPORT_VPTR argv[], char *argk);
typedef EXPORT_VPTR (* EXPORT_SYSRTN_FUN)(int argc, EXPORT_VPTR argv[], char *argk);
typedef void (* EXPORT_SYSRTN_PRO)(int argc, EXPORT_VPTR argv[], char *argk);
typedef EXPORT_VPTR (* EXPORT_SYSRTN_FUN)(int argc, EXPORT_VPTR argv[], char *argk);

typedef struct _idlgdl_structure *EXPORT_StructDefPtr;
typedef struct _idlgdl_tagdef  {	/* Definition of each instance of a tag
				   in a structure: */
  EXPORT_IDENT *id;
  EXPORT_MEMINT offset;		/* Offset of this tag within struct */
  EXPORT_VARIABLE var;		/* Definition of this tag's field. */
} EXPORT_TAGDEF;

typedef struct _idlgdl_structure  {   /* Structure definition: */
  EXPORT_IDENT *id;
  UCHAR flags;
  UCHAR contains_string;
  int ntags;
  EXPORT_MEMINT length;
  EXPORT_MEMINT data_length;
  int rcount;
  void *object;
  EXPORT_ARRAY *tag_array_mem;
  EXPORT_TAGDEF tags[1];
} EXPORT_STRUCTURE;

#define EXPORT_STD_INHERIT	1

typedef struct {
  char *name;
  EXPORT_MEMINT *dims;
  void *type;
  UCHAR flags;
} EXPORT_STRUCT_TAG_DEF;

#define GDL_ENSURE_SIMPLE(v) IDL_VarEnsureSimple(v)

#define GDL_KW_ARRAY (1 << 12)
#define GDL_KW_OUT (1 << 13)
#define GDL_KW_VIN (GDL_KW_OUT | GDL_KW_ARRAY)
#define GDL_KW_ZERO (1 << 14)
#define GDL_KW_VALUE (1 << 15)
#define GDL_KW_VALUE_MASK ((1 << 12) -1)   /* Mask for value part */
#define GDL_KW_FAST_SCAN { (char *)"", 0,0,0,0,0 }

typedef struct {
  const char *keyword;
  UCHAR type;
  unsigned short mask;
  unsigned short flags;
  int *specified;
  char *value;
} GDL_KW_PAR;


#define GDL_KW_COMMON_ARR_DESC_TAGS \
  char *data;			/* Address of array to receive data. */ \
  EXPORT_MEMINT nmin;		/* Minimum # of elements allowed. */ \
  EXPORT_MEMINT nmax;		/* Maximum # of elements allowed. */
typedef struct {		/* Descriptor for array's that are returned */
  GDL_KW_COMMON_ARR_DESC_TAGS
  EXPORT_MEMINT n;			/* # present, (Returned value). */
} GDL_KW_ARR_DESC;


typedef struct {
  GDL_KW_COMMON_ARR_DESC_TAGS
  EXPORT_MEMINT *n_offset;		/* Address/Offset of variable to receive
				   # elements present, (Returned value). */
} GDL_KW_ARR_DESC_R;

typedef struct {
#ifdef GDL_OS_HAS_TTYS
  char *name;                   /* Name of terminal type */
  char is_tty;                  /* True if stdin is a terminal */
#endif
  int lines;                    /* Lines on screen */
  int columns;                  /* Width of output */
} EXPORT_TERMINFO;

#define GDL_KW_RESULT_FIRST_FIELD int _idlgdl_kw_free
#define GDL_KW_FREE if (kw._idlgdl_kw_free) EXPORT_KWFree()
typedef struct {
  EXPORT_STRING arch;
  EXPORT_STRING os;
  EXPORT_STRING os_family;
  EXPORT_STRING os_name;
  EXPORT_STRING release;
  EXPORT_STRING build_date;
  EXPORT_INT memory_bits;
  EXPORT_INT file_offset_bits;
} EXPORT_SYS_VERSION;

typedef struct {
  EXPORT_STRING name;
  EXPORT_STRING block;
  EXPORT_LONG code;
  EXPORT_LONG sys_code[2];
  EXPORT_STRING sys_code_type;
  EXPORT_STRING msg;
  EXPORT_STRING sys_msg;
  EXPORT_STRING msg_prefix;
} EXPORT_SYS_ERROR_STATE;
#define EXPORT_M_GENERIC                    -1
#define EXPORT_M_NAMED_GENERIC              -2
#define EXPORT_M_SYSERR                     -4
#define EXPORT_M_BADARRDIM                  -176
#define EXPORT_MSG_RET	    0
#define EXPORT_MSG_EXIT	    1
#define EXPORT_MSG_LONGJMP	    2
#define EXPORT_MSG_IO_LONGJMP  3
#define EXPORT_MSG_INFO	    4 
#define EXPORT_MSG_SUPPRESS	7
typedef enum {
  EXPORT_MSG_SYSCODE_NONE=0,	/* There is no system error part */
  EXPORT_MSG_SYSCODE_ERRNO=1	/* Unix style errno based error */
#ifdef MSWIN
  , EXPORT_MSG_SYSCODE_WIN=2	/* Windows system codes (aka GetLastError() */
  , EXPORT_MSG_SYSCODE_WINSOCK=3	/* MS Windows winsock error codes
				   WSAGetLastError()*/
#endif
} EXPORT_MSG_SYSCODE_T;

typedef struct {
  char *name;
  char *format;
} EXPORT_MSG_DEF;
typedef void *EXPORT_MSG_BLOCK;
typedef EXPORT_VARIABLE *(* EXPORT_SYSRTN_GENERIC)();
typedef int EXPORT_INIT_DATA_OPTIONS_T;
typedef union {
  EXPORT_SYSRTN_GENERIC generic;
  EXPORT_SYSRTN_PRO pro;
  EXPORT_SYSRTN_FUN fun;
} EXPORT_SYSRTN_UNION;	

#define EXPORT_FUN_RET EXPORT_SYSRTN_GENERIC
#define EXPORT_SYSFUN_DEF_F_OBSOLETE	1   
#define EXPORT_SYSFUN_DEF_F_KEYWORDS	2   
#define EXPORT_SYSFUN_DEF_F_METHOD		32  
#define EXPORT_SYSFUN_DEF_F_NOPROFILE	512 
#define EXPORT_SYSFUN_DEF_F_STATIC	1024 

typedef struct {
  EXPORT_FUN_RET funct_addr;
  char *name;	
  UCHAR arg_min;
  UCHAR arg_max;
  UCHAR flags;
} EXPORT_SYSFUN_DEF;

typedef struct {
  EXPORT_SYSRTN_UNION funct_addr;
  char *name;
  unsigned short arg_min;
  unsigned short arg_max;
  int flags;
  void *extra;
} EXPORT_SYSFUN_DEF2;
typedef struct {
  EXPORT_INIT_DATA_OPTIONS_T options;
  struct {
    int argc;
    char **argv;
  } clargs;
#ifdef MSWIN
  void *hwnd;
#endif
  char **applic;
} EXPORT_INIT_DATA;

typedef void (* EXPORT_EXIT_HANDLER_FUNC)(void);
#endif 
