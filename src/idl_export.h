/* idl_export.h */


/*
 *  Copyright (c) 1992-2014, Exelis Visual Information Solutions, Inc. All
 *        rights reserved. Unauthorized reproduction is prohibited.
*/

/*
 * NOTICE OF INTENT. PLEASE READ:
 *
 *     This file is intended to supply the external definitions
 *     required to write and link code against IDL, either to extend
 *     IDL (CALL_EXTERNAL, Dynamically loadable modules, LINKIMAGE) or
 *     to use IDL to extend your own program (Callable IDL). Our goal is
 *     that including this file in your program will enable your program
 *     to access the public interfaces described in the IDL documentation,
 *     especially the External Development Guide.
 *
 *     NOT EVERYTHING IN THIS FILE IS A STABLE PUBLIC INTERFACE.
 *     For various technical reasons, there are things in this file that
 *     are considered to be private and subject to immediate
 *     change without notice. Anything in this file that is not
 *     explicitly documented elsewhere falls into this category. You should
 *     not use such interfaces, as they are not publically supported and
 *     may change or disappear without warning.
 *
 *     Examples of such interfaces are:
 *         - Types and constants that are not discussed in the documentation.
 *         - Functions that are not discussed elsewhere.
 *         - Fields of structures that are not discussed in the documentation
 *           even though the structure itself *is* documented.
 */

#ifndef export_IDL_DEF
#define export_IDL_DEF

#ifdef __cplusplus
    extern "C" {
#endif


/***** Definitions from msg_code *****/

#ifndef msg_code_IDL_DEF
#define msg_code_IDL_DEF

/* Warning: These codes can change between releases. */

#define IDL_M_GENERIC                    -1
#define IDL_M_NAMED_GENERIC              -2
#define IDL_M_SYSERR                     -4
#define IDL_M_BADARRDIM                  -176

#endif                         /* msg_code_IDL_DEF */




/***** Definitions from config_arch *****/


#ifdef sun
   #ifdef sparc
      #define SUN_SPARC
      #ifdef __sparcv9		/* Building as 64-bit application */
         #define SUN_64
      #endif
   #else
      #define SUN_INTEL
      #ifdef __x86_64 /* Predefined - see man cc */
         #define SUN_X86_64
      #else
         #define SUN_X86
      #endif
   #endif
#endif				/* sun */

#if defined(__alpha) && defined(__osf__)
#define ALPHA_OSF
#endif

#if defined(_WIN32) || defined(MSWIN)
#ifndef MSWIN
#define MSWIN
#endif
#ifdef _WIN64
#define MSWIN_64
#else
#define MSWIN_32
#endif
/*
 * Important note: WIN32 is also defined for 64-bit Windows. Use
 * IDL_MEMINT_64 or MSWIN_64 to detect 64-bit builds.
 */
#ifndef WIN32
#define WIN32
#endif
#endif

#if defined(__linux__) && !defined(linux)
#define linux
#endif

#ifdef linux
#define LINUX_X86
#ifdef __x86_64__
#define LINUX_X86_64
#else
#define LINUX_X86_32
#endif
#endif

#if defined(sgi) && (_MIPS_SZPTR == 64)
#define IRIX_64
#endif

#if defined(_AIX) && defined(__64BIT__)
#define AIX_64
#endif

/*
 * Proper ANSI C doesn't allow pre-defined cpp macros that don't start
 * with an underscore. Explicitly define the ones we use.
 */
#if defined(__hpux) && !defined(hpux)
#define hpux
#endif
#if defined(__hp9000s800) && !defined(hp9000s800)
#define hp9000s800
#endif
#if defined(hpux) && defined(__LP64__)
#define HPUX_64
#endif


/* darwin */
#ifdef __APPLE__
  #ifndef darwin
    #define darwin
  #endif
  #ifdef __ppc__
    #define darwin_ppc
  #endif
  #if defined(__i386__) || defined(__x86_64__)
    #ifdef __LP64__
      #define darwin_x86_64
    #else
      #define darwin_i386
    #endif
  #endif
#endif


/* A rose by any other name: There are multiple ways to say Unix */
#if !defined(unix) && (defined(__unix__) || defined(__unix) || defined(_AIX) || defined(darwin))
#define unix
#endif



/*
 * IDL_SIZEOF_C_LONG: The number of bytes contained in a C long
 * 	integer (i.e. sizeof(long)).
 *
 * IDL_SIZEOF_C_PTR: The number of bytes contained in a machine pointer
 * 	(i.e. sizeof(char *)).
 *
 * The C language does not specify a required size for the short, int or
 * long types. It requires that sizeof(char) is 1, and it requires
 * that the following relationship hold:
 *
 *	sizeof(char) <= sizeof(short) <= sizeof(int) <= sizeof(long).
 *
 * In the days of 16-bit computing (16-bit machine pointers), it was
 * common for short and int to be 2 bytes (16-bits), and sizeof(long)
 * to be 4 (32-bit). In the era of 32-bit computing (32-bit machine
 * pointers), it was almost always true that short remains at 2 bytes
 * while both int and long were 32-bit quantities. This is commonly
 * called ILP32, and is the model used on all 32-bit platforms ever
 * supported by IDL.
 *
 * 64-bit systems fall into two camps: Unix systems use the LP64 model,
 * in which sizeof(short) is 2, sizeof(int) is 4, and both sizeof(long)
 * and machine pointers are 8 (64-bit). Microsoft Windows uses a different
 * approach: sizeof(long) remains at 4 while machine pointers move to 8,
 * and programmers use an explicitly defined type for 64-bit integers.
 *
 * Note that although sizeof(long) generally tracks sizeof(machine pointer),
 * this relationship cannot be always be relied on, so both sizes must be
 * known independently.
 */
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






/***** Definitions from config *****/

#ifndef config_IDL_DEF
#define config_IDL_DEF

#include <stdarg.h>


#ifndef IDL_DEBUGGING
#define IDL_DEBUGGING 1
#endif



#ifdef FALSE
#undef FALSE
#endif
#define FALSE   (0)

#ifdef TRUE
#undef TRUE
#endif
#define TRUE    (1)

/*
 * The following definitions are to be used in all modules. They
 * are used by cx to generate .x files. cx_public is used for functions that
 * are used outside a module, but which are private. cx_export is used
 * for functions that outside customers can use.
*/

#define cx_public   /* C default file scope, private */
#define cx_export   /* C default file scope, visible in idl_export.h */

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


/*
 * Some platforms support tty based user interaction (Unix)
 * while others (Windows) don't. On those that don't, we
 * want to avoid compiling code that will never be called.
 * This symbol is defined on those systems that support ttys.
 */
#ifdef unix
#define IDL_OS_HAS_TTYS
#endif



#if IDL_DEBUGGING < 2
#define IDL_REGISTER register   /* Use explicit register declarations */
#else
#define IDL_REGISTER
#endif


/**** Maximum # of array dimensions ****/
#define IDL_MAX_ARRAY_DIM 8

/**** Maximum # of params allowed in a call ****/
#define IDL_MAXPARAMS 65535 /* 2^16 - 1 */

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

/*** Maximum length of an identifier ***/
#define IDL_MAXIDLEN 1000

/**** Longest allowed file path specification ****/
#ifdef MSWIN
#define IDL_MAXPATH 1024
#else
#define IDL_MAXPATH    1024     /* That's what BSD allows */
#endif



#endif              /* config_IDL_DEF */




/***** Definitions from defs *****/

#ifndef defs_IDL_DEF
#define defs_IDL_DEF


#if !defined(MSWIN) || !defined(PLTYPES)
typedef unsigned char UCHAR;	/* Unsigned character type */
#endif

/* Boolean. */ 
typedef enum {
    IDL_FALSE = 0,
    IDL_TRUE = 1
} IDLBool_t;

/*
 * IDL integer types. For historical reasons, we use UCHAR for TYP_BYTE
 * instead of defining an IDL_BYTE type.
 */
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

/* Type used for pointer and object reference variables is same as IDL_LONG */
typedef IDL_ULONG IDL_HVID;


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


#define IDL_MAX_TYPE            15
#define IDL_NUM_TYPES           16

/*
 * Various machines use different data types for representing memory
 * and file offsets and sizes. We map these to IDL types using the
 * following definitions. Doing it this way lets us easily change
 * the mapping here without having to touch all the code that uses
 * these types.
 */

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
 * The above type codes each have a bit mask value associated with
 * them. The bit mask value is computed as (2**Type_code), but the
 * following definitions can also be used. Some routines request the
 * bit mask value instead of the type code value.
 *
 * Simple types are everything except TYP_STRUCT, TYP_PTR, and TYP_OBJREF.
*/
#define IDL_TYP_B_SIMPLE            62207
#define IDL_TYP_B_ALL               65535

/* This macro turns it's argument into its bit mask equivalent.
 * The argument type_code should be one of the type codes defined
 * above.
*/ 
#define IDL_TYP_MASK(type_code)      (1 << type_code)



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



/**** Basic IDL structures: ****/

typedef struct {
  float r,i;
} IDL_COMPLEX;

typedef struct {
  double r,i;
} IDL_DCOMPLEX;

/*
 * History of IDL_STRING slen field:
 *
 * Originally, the length field of IDL_STRING was set to short because
 * that allowed IDL and VMS string descriptors to be identical and
 * interoperable, a feature that IDL exploited to simplify string handling.
 * Also, on most 32-bit machines this resulted in sizeof(IDL_STRING) being 8.
 * This was very good, because it avoided causing the sizeof(IDL_VARIABLE)
 * to be increased by the inclusion of an IDL_STRING in the values field
 * (prior to the addition of TYP_DCOMPLEX, the biggest thing in the values
 * field was 8 bytes). IDL's speed is partly gated by the size of a variable,
 * so this is an important factor. Unfortunately, that results in a
 * maximum string length of 64K, long enough for most things, but not
 * really long enough.
 *
 * In IDL 5.4, the first 64-bit version of IDL was released. I realized that
 * we could make the length field be a 32-bit int without space penalty
 * (the extra room comes from wasted "holes" in the struct due to pointers
 * being 8 bytes long). Since there is no issue with VMS (no 64-bit VMS IDL
 * was planned), I did so. 32-bit IDL stayed with the 16-bit length field
 * for backwards compatability and VMS support.
 *
 * With IDL 5.5, the decision was made to drop VMS support. This decision
 * paves the way for 32-bit IDL to also have a 32-bit length field in
 * IDL_STRING. Now, all versions of IDL have the longer string support.
 * This does not increase the size of an IDL_VARIABLE, because the biggest
 * thing in the value field is 16 bytes (DCOMPLEX), and sizeof(IDL_STRING)
 * will be 12 on most systems.
 */
typedef int IDL_STRING_SLEN_T;
#define IDL_STRING_MAX_SLEN 2147483647


typedef struct {		/* Define string descriptor */
  IDL_STRING_SLEN_T slen;	/* Length of string, 0 for null */
  short stype;			/* type of string, static or dynamic */
  char *s;			/* Addr of string */
} IDL_STRING;


/**** IDL identifiers ****/
typedef struct _idl_ident {
  struct _idl_ident *hash;	/* Must be the first field */
  char *name;                   /* Identifier text (NULL terminated */
  int len;			/* # of characters in id, not counting NULL
				   termination. */
} IDL_IDENT;


/*
 * Type of the free_cb field of IDL_ARRAY. When IDL deletes a variable and
 * the free_cb field of ARRAY non-NULL, IDL calls the function that field
 * references, passing the value of the data field as it's sole argument.
 * The primary use for this notification is to let programs know when
 * to clean up after calls to IDL_ImportArray(), which is used to create
 * arrays using memory that IDL does not allocate.
 */
typedef void (* IDL_ARRAY_FREE_CB)(UCHAR *data);

/* Type of the dim field of an IDL_ARRAY. */
typedef IDL_MEMINT IDL_ARRAY_DIM[IDL_MAX_ARRAY_DIM];

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

/*
 * IDL_SYSRTN_GENERIC is used to refer to system routines of either
 * type (function or procedure). It is conceptually equivalent to a
 * C (void *). It should never be used for calling the routine, but only
 * for initializing structures or simple pointer comparisons.
 */
typedef IDL_VARIABLE *(* IDL_SYSRTN_GENERIC)();

/* IDL_FUN_RET is an obsolete name for IDL_SYSRTN_GENERIC */
#define IDL_FUN_RET IDL_SYSRTN_GENERIC

/*
 * Union used to hold system routine pointers within IDL_SYSFUN_DEF2
 * structures.
 */
typedef union {
  IDL_SYSRTN_GENERIC generic;	/* Used for generic access to the routine
				   pointer. The Ansi C spec (K&R 2nd edition)
				   says that union initializations always
				   refer to the first field in the union.
				   Hence, this must be the first field. */
  IDL_SYSRTN_PRO pro;		/* System procedure pointer */
  IDL_SYSRTN_FUN fun;		/* System function pointer */
} IDL_SYSRTN_UNION;		/* ^ to interp. function (ret ^ to VAR) */



/*
 * This K&R typedef is pure trouble in a full 64-bit environment, and is
 * no longer used in IDL. We continue to export it for now until the EDG
 * has been purged of it and people have had a chance to modify their
 * code. Ultimately, it is destined for obsolete.h.
 */
typedef void (* IDL_PRO_PTR)();	/* ^ to interpreter procedure (ret is void) */

/* Possible values for the flags field of IDL_SYSFUN_DEF and IDL_SYSFUN_DEF2 */
#define IDL_SYSFUN_DEF_F_OBSOLETE	1   /* Routine is obsolete */
#define IDL_SYSFUN_DEF_F_KEYWORDS	2   /* Routine accepts keywords */
#define IDL_SYSFUN_DEF_F_METHOD		32   /* Routine is an object method */
#define IDL_SYSFUN_DEF_F_NOPROFILE	512  /* By default routine is not profiled */
#define IDL_SYSFUN_DEF_F_STATIC	1024   /* Routine is a static method */

/*
 * The IDL_SYSFUN_DEF2  structure defines the format of a system procedure
 * or function table entry.
 */
typedef struct {
  IDL_SYSRTN_UNION funct_addr;	/* Address of function, or procedure. */
  char *name;			/* The name of the function */
  unsigned short arg_min;	/* Minimum allowed argument count. */
  unsigned short arg_max;	/* Maximum argument count. */
  int flags;			/* IDL_SYSFUN_DEF_F_* flags */
  void *extra;			/* Caller should set this to 0 */
} IDL_SYSFUN_DEF2;

/**** IDL structures ****/
/*
 * Type of pointer to an IDL structure definition. Through trickery, we
 * make it opaque for use outside of IDL.
 */
typedef struct _idl_structure *IDL_StructDefPtr;
typedef struct _idl_tagdef  {	/* Definition of each instance of a tag
				   in a structure: */
  IDL_IDENT *id;
  IDL_MEMINT offset;		/* Offset of this tag within struct */
  IDL_VARIABLE var;		/* Definition of this tag's field. */
} IDL_TAGDEF;

typedef struct _idl_structure  {   /* Structure definition: */
  IDL_IDENT *id;		/* The name of this structure.  NULL
				   for anonymous structure. */
  UCHAR flags;			/* Structure attribute flags */
  UCHAR contains_string;	/* TRUE if structure contains strings or
                                   heap variables, and FALSE otherwise. */
  int ntags;			/* The number of tags in this structure */
  IDL_MEMINT length;		/* Length in bytes of each instance of
				   this structure. */
  IDL_MEMINT data_length;	/* Length in bytes of the data contained in
				   each instance of this structure. This is
				   (length - struct alignment-holes) */
  int rcount;			/* Reference count: only for anon structs */
  void *object;
  IDL_ARRAY *tag_array_mem;	/* If the IDL_ARRAY descriptors for the
				   tags was not allocated at the end of
				   the same memory block as the array, this
				   field contains the pointer to their memory.
				   This is used to free the memory when the
				   .reset_session executive command is
				   executed. Otherwise it is NULL. */
  IDL_TAGDEF tags[1];		/* Array of ntags tagdefs. */
} IDL_STRUCTURE;
#endif				/* defs_IDL_DEF */




/***** Definitions from message *****/

#ifndef message_IDL_DEF
#define message_IDL_DEF

/*
 * action parameter to message is composed of two parts. These two masks
 * are used to separate them out.
 */
#define IDL_MSG_ACTION_CODE	0x0000ffff
#define IDL_MSG_ACTION_ATTR	0xffff0000

/* Allowed codes for action parameter to IDL_Message() */
#define IDL_MSG_RET	    0   /* Return to caller */
#define IDL_MSG_EXIT	    1   /* Terminate process via exit(3) */
#define IDL_MSG_LONGJMP	    2   /* General error. Obey the error handling
				   established by the ON_ERROR user
				   procedure. */
#define IDL_MSG_IO_LONGJMP  3   /* I/O error. Obey the error handling
				   established by the ON_IOERROR user
				   procedure. */
#define IDL_MSG_INFO	    4   /* Informational. Like IDL_MSG_RET, but won't
				   set !ERR or !ERR_STRING. Also,
				   inhibited by !QUIET */
#define IDL_MSG_SUPPRESS	7   /* IDL_Message() returns immediately
				       without processing the message at all.
				       This code can be used to implement
				       libraries of code that can throw errors
				       or simply return status depending on
				       the input value of the action
				       parameter. */




/* Allowed attribute masks that can be OR'd into the action code */
#define IDL_MSG_ATTR_NOPRINT  0x00010000   /* Suppress the printing of
					      the error text to stderr,
					      but do everything else in
					      the normal way. */
#define IDL_MSG_ATTR_MORE  0x00020000   /* Use IDL_more() instead of printf(3S)
					   to output the message. The calling
					   routine must worry about calling
					   IDL_more_reset(). A side effect of
					   this is that the message goes to the
					   file named in IDL_more_reset(), not
					   necessarily stderr. */
#define IDL_MSG_ATTR_NOPREFIX  0x00040000   /* Don't output the normal
					       message (from
					       !ERROR_STATE.MSG_PREFIX),
					       just the message text. */
#define IDL_MSG_ATTR_QUIET     0x00080000   /* If the message would normally
					       be printed and !QUIET is
					       non-zero, the printing
					       is suppressed. Everything
					       else is updated as expected. */
#define IDL_MSG_ATTR_NOTRACE   0x00100000   /* Suppress traceback message */
#define IDL_MSG_ATTR_BELL      0x00200000   /* Ring the bell */
#define IDL_MSG_ATTR_SYS       0x00400000   /* IDL_Message() only: Include
					       system supplied error message,
					       via errno. Note that other types
					       of syscodes cannot be handled
					       via this mechanism, and that
					       it is considered to be
					       obsolete. Use
					       IDL_MessageSyscode() instead. */

/*
 * Types of system error code that can be passed to IDL_MessageSyscode()
 * and IDL_MessageSyscodeFromBlock().
 *
 * NOTE:
 *	- The message module depends on the specific values of
 *	  these constants. They cannot be arbitrarily reassigned.
 *	- The constants are only defined on platforms where that
 *	  error type is possible. This is to help catch uses of
 *	  them on incorrect platforms.
 */
typedef enum {
  IDL_MSG_SYSCODE_NONE=0,	/* There is no system error part */
  IDL_MSG_SYSCODE_ERRNO=1	/* Unix style errno based error */
#ifdef MSWIN
  , IDL_MSG_SYSCODE_WIN=2	/* Windows system codes (aka GetLastError() */
  , IDL_MSG_SYSCODE_WINSOCK=3	/* MS Windows winsock error codes
				   WSAGetLastError()*/
#endif
} IDL_MSG_SYSCODE_T;

/* The type of elements in the defs argument to IDL_MessageDefineBlock() */
typedef struct {
  char *name;
  char *format;
} IDL_MSG_DEF;

/* Type returned by IDL_MessageDefineBlock() */
typedef void *IDL_MSG_BLOCK;
#define IDL_MSG_ERR_BUF_LEN	2048   /* Intentionally huge */
#define IDL_MSG_SYSERR_BUF_LEN	512   /* Need not be so big */

typedef struct {
  int action;			/* As passed to IDL_Message(). */
  IDL_MSG_BLOCK msg_block;	/* Message block containing error */
  int code;			/* Message code within msg_block */
  int global_code;		/* Message code mapped into global
				   IDL message space */
  IDL_MSG_SYSCODE_T syscode_type;   /* System error code type */
  int syscode;			/* System error code */
  char msg[IDL_MSG_ERR_BUF_LEN];   /* Text of IDL part of message */
  char sysmsg[IDL_MSG_SYSERR_BUF_LEN];   /* Text of system part of message */
} IDL_MSG_ERRSTATE;

/*
 * Opaque type. Not for non-IDL-internal use. Set any arguments of
 * this type to NULL.
 */
typedef void *IDL_MSG_ERRSTATE_PTR;



#endif				/* message_IDL_DEF */




/***** Definitions from macros *****/

#ifndef macros_IDL_DEF
#define macros_IDL_DEF
#define IDL_VENDOR_NAME "Exelis Visual Information Solutions, Inc."
#define IDL_VENDOR_CONTACT "info@exelisvis.com"
#define IDL_PRODUCT_NAME "IDL"
#define IDL_PRODUCT_NAME_LC "idl"
#define IDL_VENDOR_NAME_CONTACT IDL_VENDOR_NAME " at " IDL_VENDOR_CONTACT

/* General math macros */
#define IDL_MIN(x,y) (((x) < (y)) ? (x) : (y))
#define IDL_MAX(x,y) (((x) > (y)) ? (x) : (y))
#define IDL_ABS(x) (((x) >= 0) ? (x) : -(x))
#define IDL_C_ABS(pZ) ((pZ)->r*(pZ)->r + (pZ)->i*(pZ)->i)


/* Return x in the range of min <= x <= max */
#define IDL_CLIP_TO_RANGE(x, min, max) \
  ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

/* Round x up modulo m. m must be a power of 2 : */
#define IDL_ROUND_UP(x,m) \
  (((x) + (m-1)) & (~(m-1)))

/**** Cast a pointer to (char *) ****/
#define IDL_CHAR(x) ((char *) x)

/**** Take the address of a variable and cast to a desired type ****/
#define IDL_CHARA(x) ((char *) &(x))
#define IDL_UCHARA(x) ((UCHAR *) &(x))
#define IDL_SHORTA(x) ((short *) &(x))
#define IDL_INTA(x) ((int *) &(x))
#define IDL_LONGA(x) ((IDL_LONG *) &(x))

/**** Get pointer to a valid string from an IDL_STRING descriptor */
#define IDL_STRING_STR(desc) ((desc)->slen ? (desc)->s : "")

/**** Compute strlen() of a static lexical C string at compile time ****/
#define IDL_STATIC_STRLEN(lexstr) (sizeof(lexstr) - 1)

/**** Initialize an IDL_STRING descriptor from a static lexical C string ****/
#define IDL_STATIC_STRING(lexstr) { IDL_STATIC_STRLEN(lexstr), 0, lexstr }

#define IDL_DELTMP(v) { if (((v)->flags) & IDL_V_TEMP) IDL_Deltmp(v); }

/**** How many elements are there in a C 1D array? ****/
#define IDL_CARRAY_ELTS(arr) (sizeof(arr)/sizeof(arr[0]))

#define IDL_EXCLUDE_UNDEF(v) { if (!(v)->type) \
	IDL_MessageVE_UNDEFVAR(v, IDL_MSG_LONGJMP); }
#define IDL_EXCLUDE_CONST(v)        { if ((v)->flags & IDL_V_CONST) \
	IDL_MessageVE_NOCONST(v, IDL_MSG_LONGJMP); }
#define IDL_EXCLUDE_EXPR(v)  { if ((v)->flags & (IDL_V_CONST | IDL_V_TEMP)) \
	IDL_MessageVE_NOEXPR(v, IDL_MSG_LONGJMP); }
#define IDL_EXCLUDE_FILE(v) { if ((v)->flags & IDL_V_FILE) \
	IDL_MessageVE_NOFILE(v, IDL_MSG_LONGJMP); }
#define IDL_EXCLUDE_STRUCT(v)       { if ((v)->flags & IDL_V_STRUCT) \
	IDL_MessageVE_NOSTRUCT(v, IDL_MSG_LONGJMP); }
#define IDL_EXCLUDE_FILE_OR_STRUCT(v) { \
	if((v)->flags & (IDL_V_FILE|IDL_V_STRUCT)) \
	  IDL_VarExclude(v, IDL_TYP_MASK(TYP_STRUCT), FALSE, FALSE, TRUE);}
#define IDL_EXCLUDE_COMPLEX(v)      { if (((v)->type == IDL_TYP_COMPLEX) \
					  || ((v)->type == IDL_TYP_DCOMPLEX)) \
	IDL_MessageVE_NOCOMPLEX(v, IDL_MSG_LONGJMP); }
#define IDL_EXCLUDE_STRING(v)       { if ((v)->type == IDL_TYP_STRING)  \
	IDL_MessageVE_NOSTRING(v, IDL_MSG_LONGJMP); }
#define IDL_EXCLUDE_SCALAR(v) { if (!((v)->flags & IDL_V_NOT_SCALAR)) \
	IDL_MessageVE_NOSCALAR(v, IDL_MSG_LONGJMP);}


/*
 * Disallow variables with 64-bit addressing requirements. A routine does this
 * if its code contains 32-bit limitations.
 */
#ifdef IDL_MEMINT_64
#define IDL_EXCLUDE_BIGVAR(v) \
  if ((v->flags & IDL_V_ARR) && IDL_MEMINT_BIG(v->value.arr->n_elts)) \
    IDL_MessageVE_NOMEMINT64(v, IDL_MSG_LONGJMP);
#define IDL_BIGVAR(v) \
  ((v->flags & IDL_V_ARR) && IDL_MEMINT_BIG(v->value.arr->n_elts))
#else
#define IDL_EXCLUDE_BIGVAR(v)
#define IDL_BIGVAR(v) IDL_FALSE
#endif


/*
 * Ensure that variables possess certain attributes.
 *
 * NOTE: It is usually better not to ENSURE_SCALAR in situations
 *	 where a 1-element array will also work. For strings,
 *	 IDL_VarGetString() is recommended. For other data types,
 *	 the IDL_*Scalar() family of functions handle this case.
 */
#define IDL_ENSURE_ARRAY(v) { if (!((v)->flags & IDL_V_ARR)) \
	IDL_MessageVE_NOTARRAY(v, IDL_MSG_LONGJMP); }
#define IDL_ENSURE_SCALAR(v) { if ((v)->flags & IDL_V_NOT_SCALAR) \
	IDL_MessageVE_NOTSCALAR(v, IDL_MSG_LONGJMP);}
#define IDL_ENSURE_STRING(v) { if ((v)->type != IDL_TYP_STRING)  \
	IDL_MessageVE_REQSTR(v, IDL_MSG_LONGJMP);}
#ifndef IDL_ENSURE_SIMPLE
#define IDL_ENSURE_SIMPLE(v) IDL_VarEnsureSimple(v)
#endif
#define IDL_ENSURE_STRUCTURE(v) { if (!((v)->flags & IDL_V_STRUCT)) \
	IDL_MessageVE_STRUC_REQ(v, IDL_MSG_LONGJMP);}
#define IDL_ENSURE_PTR(v) { if ((v)->type != IDL_TYP_PTR)  \
	IDL_MessageVE_REQPTR(v, IDL_MSG_LONGJMP);}
#define IDL_ENSURE_OBJREF(v) { if ((v)->type != IDL_TYP_OBJREF)  \
	IDL_MessageVE_REQOBJREF(v, IDL_MSG_LONGJMP);}


/* Check if var has a dynamic part. If so, delete it using IDL_Delvar  */
#define IDL_DELVAR(v) { if (((v)->flags) & IDL_V_DYNAMIC) IDL_Delvar(v); }

// BOOLEAN: Check if var is a BOOLEAN
#define IDL_BOOLEAN(v) \
  ((v)->type == IDL_TYP_BYTE && (v)->flags & IDL_V_BOOLEAN)

/* !NULL: Check if var is the null variable. */
#define IDL_NULL(v) \
        ((v)->flags & IDL_V_NULL)


#if defined(HPUX_64) || (defined(__cplusplus) && defined(MSWIN)) || defined(darwin) || defined(linux) || defined(MSWIN_64)
#define IDL_CAST_PTRINT(cast, orig_value) ((cast) ((IDL_PTRINT) (orig_value)))
#else
#define IDL_CAST_PTRINT(cast, orig_value) ((cast) (orig_value))
#endif

#endif				/* macros_IDL_DEF */




/***** Definitions from idl_pds *****/

#ifndef idl_pds_IDL_DEF
#define idl_pds_IDL_DEF





typedef struct {
  IDL_LONG hw_vector;
  IDL_LONG vector_enable;
  IDL_LONG hw_ncpu;
  IDL_LONG tpool_nthreads;
  IDL_MEMINT tpool_min_elts;
  IDL_MEMINT tpool_max_elts;
} IDL_CPU_STRUCT;




typedef struct {
  IDL_STRING name;
  IDL_STRING block;
  IDL_LONG code;
  IDL_LONG sys_code[2];
  IDL_STRING sys_code_type;
  IDL_STRING msg;
  IDL_STRING sys_msg;
  IDL_STRING msg_prefix;
} IDL_SYS_ERROR_STATE;




typedef struct {
  IDL_LONG x;
  IDL_LONG y;
  IDL_LONG button;
  IDL_LONG time;
} IDL_MOUSE_STRUCT;




typedef struct {
  IDL_STRING arch;
  IDL_STRING os;
  IDL_STRING os_family;
  IDL_STRING os_name;
  IDL_STRING release;
  IDL_STRING build_date;
  IDL_INT memory_bits;
  IDL_INT file_offset_bits;
} IDL_SYS_VERSION;

#endif				/* idl_pds_IDL_DEF */




/***** Definitions from prog_nam_ver *****/

#ifndef prog_nam_ver_IDL_VERSION
#define prog_nam_ver_IDL_VERSION

/*
 * An IDL version number consists of three fields: x.y.z, where the first
 * is the major version number, the second is the minor number, and the
 * third is the sub-release number (used primarily for bug fix releases).
 *
 * IDL_VERSION_SUB_DIRDIGIT is the sub digit to be used in forming the
 * directory name. The bridges need this to construct the directory name
 * both with and without separating "." characters, and if the sub is zero
 * it is not added to the directory name string. Note that
 * Having IDL_VERSION_SUB_DIRDIGIT allows us to have an IDL version
 * where the sub digit is incremented but we still install in the same
 * directory as the version being patched. (IDL 8.0.1 installing in IDL80).
 */
#define IDL_VERSION_MAJOR 8
#define IDL_VERSION_MINOR 4
#define IDL_VERSION_SUB	  0
#define IDL_VERSION_SUB_DIRDIGIT	  0

/*
 */

/*
 * The string supplied in !VERSION.RELEASE. Note that this is not always
 * a simple concatenation of the three values above, because we leave off
 * the sub-release field if it is a 0, and because the sub-release field
 * in the string can include letters (e.g. 3.6.1c) for extremely minor
 * releases and this is not reflected in IDL_VERSION_SUB.
 * We also need a copy of the string so that daily builds can append
 * the development build date.
 * We also need another copy without the sub-release field, for use
 * in app_user_dir, so that preferences are shared among sub-releases.
 */
#define IDL_VERSION_STRING "8.4"
#define IDL_VERSION_STRING_COPY "8.4"
#define IDL_VERSION_STRING_NOSUBMINOR "8.4"

#endif		/* prog_nam_ver_IDL_VERSION */




/***** Definitions from crearr *****/

#ifndef crearr_IDL_DEF
#define crearr_IDL_DEF

/* The following define the valid values for the init arg to basic_array */
#define IDL_ARR_INI_ZERO   0	/* Zero data area */
#define IDL_ARR_INI_NOP    1	/* Don't do anything to data area */
#define IDL_ARR_INI_INDEX  2	/* Put 1-D index into each elt. */
#define IDL_ARR_INI_TEST   3	/* Test if enough memory is available */

/* Old names for the array init arg */
#define IDL_BARR_INI_ZERO  IDL_ARR_INI_ZERO
#define IDL_BARR_INI_NOP   IDL_ARR_INI_NOP
#define IDL_BARR_INI_INDEX IDL_ARR_INI_INDEX
#define IDL_BARR_INI_TEST  IDL_ARR_INI_TEST

#endif				/* crearr_IDL_DEF */




/***** Definitions from exithand *****/

#ifndef exithand_IDL_DEF
#define exithand_IDL_DEF


/* Type of an exit handler routine */
typedef void (* IDL_EXIT_HANDLER_FUNC)(void);


#endif				/* exithand_IDL_DEF */




/***** Definitions from ez *****/

#ifndef ez_IDL_DEF
#define ez_IDL_DEF

/* These constants can be ORd together to form the value for the
   access field of IDL_EZ_ARG */
#define IDL_EZ_ACCESS_R     1	/* Arg is readable */
#define IDL_EZ_ACCESS_W     2	/* Arg is writable */
#define IDL_EZ_ACCESS_RW    3	/* Arg is readable and writable */


/* This macro turns it's argument into a bit mask suitable for
 * the allowed_dims field of IDL_EZ_ARG. The argument dim_code should be
 * 0 for scalar, 1 for 1D, 2 for 2D, etc...
 */ 
#define IDL_EZ_DIM_MASK(dim_code)   (1 << dim_code)

/* Define type mask of all numeric types: */
#define IDL_EZ_TYP_NUMERIC \
	( IDL_TYP_MASK(TYP_INT) | IDL_TYP_MASK(TYP_LONG) \
	 | IDL_TYP_MASK(TYP_FLOAT) | IDL_TYP_MASK(TYP_DOUBLE) \
	 | IDL_TYP_MASK(TYP_COMPLEX) | IDL_TYP_MASK(TYP_BYTE) \
	 | IDL_TYP_MASK(TYP_DCOMPLEX) | IDL_TYP_MASK(TYP_UINT) \
	 | IDL_TYP_MASK(TYP_ULONG) | IDL_TYP_MASK(TYP_LONG64) \
	 | IDL_TYP_MASK(TYP_ULONG64) )


/* These constants should be used instead of IDL_EZ_DIM_MASK when appropriate */
#define IDL_EZ_DIM_ARRAY    510	  /* Allow all but scalar */
#define IDL_EZ_DIM_ANY      511	  /* Allow anything */


/* These constants can be ORd together to form the value for the
   pre field of IDL_EZ_ARG. These actions are taken only if the argument
   has IDL_EZ_ACCESS_R. */
#define IDL_EZ_PRE_SQMATRIX         1	/* Arg must be a square matrix. */
#define IDL_EZ_PRE_TRANSPOSE        2	/* Transpose arg. This only happens
					   with read access. */


/* These constants can be ORd together to form the value for the
   post field of IDL_EZ_ARG. These actions are taken only if the argument
   has IDL_EZ_ACCESS_W.  If IDL_EZ_POST_WRITEBACK is not present, none of the
   other actions are considered, since that would imply wasted effort. */
#define IDL_EZ_POST_WRITEBACK       1	/* Transfer the contents of
					   uargv to the actual argument. */
#define IDL_EZ_POST_TRANSPOSE       2	/* Transpose uargv prior to writing. */


/*
 * IDL_EZ_ARG is the definition for the structure used by IDL_EzCall()
 * and IDL_EzCallCleanup() to define the plain arguments being passed
 * to a routine.
 */
typedef struct {
  short allowed_dims;		/* A bit mask that specifies the
				   allowed dimensions. Bit 0 means scalar,
				   bit 1 is 1D, etc. Use the EZ_DIM_* constants
				   defined in this file to specify this
				   value. */
  int allowed_types;		/* This is a bit mask defining the
				   allowed data types for the argument.
				   To convert the TYP_* type codes defined
				   in defs.h to the appropriate bits,
				   use the formula 2**(type_code) or use
				   the TYP_B_* bit masks defined in defs.h
				   NOTE: If you specify a value for convert,
				   its a good idea to specify IDL_TYP_B_ALL or
				   IDL_TYP_B_SIMPLE here. The type conversion
				   will catch any problems and your routine
				   will be more flexible. */
  short access;			/* Some combination of the EZ_ACCESS
				   constants defined above. */
  short convert;		/* If non-zero, the TYP_* type code to
				   which the argument will be converted.
				   A value of zero means that no conversion
				   will be applied. */
  short pre;			/* A bit mask that specifies special purpose
				   processing that should be performed on
				   the variable by IDL_EzCall(). These bits
				   are specified with the IDL_EZ_PRE_*
				   constants. This processing occurs *AFTER*
				   any type conversions specified by
				   convert. */
  short post;			/* A bit mask that specifies special purpose
				   processing that should be performed on
				   the variable by IDL_EzCallCleanup(). These
				   bits are specified with the IDL_EZ_POST_*
				   constants. */
  IDL_VPTR to_delete;		/* RESERVED TO EZ MODULE. DO NOT MAKE
				   USE OF OR CHANGE THIS FIELD. If EZ
				   allocated a temporary variable to satisfy
				   the conversion requirements given by the
				   convert field, the IDL_VPTR to that temp
				   is stashed here by IDL_EzCall for use by
				   IDL_EzCallCleanup(). */
  IDL_VPTR uargv;		/* After calling IDL_EzCall(), uargv contains
				   a pointer to the IDL_VARIABLE which is
				   the argument. */
  IDL_ALLTYPES value;		/* This is a copy of the value field
				   of the variable pointed at by uargv.
				   For scalar variables, it contains the
				   value, for arrays it points at the
				   array block. */
} IDL_EZ_ARG;

#endif				/* ez_IDL_DEF */




/***** Definitions from graphics *****/

#ifndef graphics_IDL_DEF
#define graphics_IDL_DEF

/* *** Structure defining current device and parameters: *** */
#define IDL_MAX_TICKN 60	/* Max # of axis annotations */
#define IDL_MAX_TICKUNIT_COUNT 10  /* Max # of units (or levels) for an axis */

#define IDL_COLOR_MAP_SIZE 256	 /* Size of internal color map. */

#define IDL_NUM_LINESTYLES 6	/* # of line styles */
#define IDL_X0 0		/* Subscripts of fields for rect structures */
#define IDL_Y0 1
#define IDL_X1 2
#define IDL_Y1 3
#define IDL_Z0 4
#define IDL_Z1 5


#define IDL_AX_LOG 1		/* Axis type values */
#define IDL_AX_MAP 2		/* Old style maps */
#define IDL_AX_MAP1 3		/* New style maps */

#define IDL_AX_EXACT 1		/* Axis style values: */
#define IDL_AX_EXTEND 2
#define IDL_AX_NONE 4
#define IDL_AX_NOBOX 8
#define IDL_AX_NOZERO 16

#define IDL_GR_PRECISION_SINGLE 0
#define IDL_GR_PRECISION_DOUBLE 1

#define IDL_TICKLAYOUT_STANDARD    0  /* Tick layout styles */
#define IDL_TICKLAYOUT_NOAXISLINES 1
#define IDL_TICKLAYOUT_BOXOUTLINE  2
  
typedef struct {		/* System variable for axis */
  IDL_STRING title;		/* Axis title */
  int type;			/* 0 = normal linear, 1=log. */
  int style;			/* 0 = norm, AX_EXTEND, AX_EXACT, AX_NONE,
				   AX_NOBOX. */
  int nticks;			/* # of ticks, 0=auto, -1 = none */
  float ticklen;		/* Tick length, normalized */
  float thick;			/* Axis thickness */
  double range[2];		/* Min and max of endpoints */
  double crange[2];		/* Current min & max */
  double s[2];			/* Scale factors, screen = data*s[1]+s[0] */
  float margin[2];		/* Margin size, in char units. */
  float omargin[2];		/* Outer margin, in char units */
  float window[2];		/* data WINDOW coords, normal units */
  float region[2];		/* Plot region, normal units */
  float charsize;		/* Size of annotations */
  int minor_ticks;		/* Minor ticks */
  double tickv[IDL_MAX_TICKN];	/* Position of ticks */
  IDL_STRING annot[IDL_MAX_TICKN];   /* Annotation */
  IDL_LONG gridstyle;		/* tick linestyle */
  IDL_STRING format[IDL_MAX_TICKUNIT_COUNT]; /* Axis label format/procedure */
  double tickinterval;          /* Interval between major ticks */    
  IDL_LONG ticklayout;          /* Layout style for the ticks of the axis. */
  IDL_STRING tickunits[IDL_MAX_TICKUNIT_COUNT];    
				/* Units for each axis level */

  /* After here, the elements are not accessible to the user via
   the system variables: */
  IDL_VPTR ret_values;		/* Returned tick values */
  int log_minor_ticks;		/* true if the minor tickmarks should be
			           laid out via log10() */
} IDL_AXIS;

/* Define cursor function codes: */

#define IDL_CURS_SET 1		/* Set cursor */
#define IDL_CURS_RD  2		/* Read cursor pos */
#define IDL_CURS_RD_WAIT 3	/* Read cursor with wait */
#define IDL_CURS_HIDE 4		/* Disable cursor */
#define IDL_CURS_SHOW 5		/* Display cursor */
#define IDL_CURS_RD_MOVE 6	/* Read & wait for movement or button */
#define IDL_CURS_RD_BUTTON_UP 7	  /* Wait for button up transition */
#define IDL_CURS_RD_BUTTON_DOWN 8   /* Wait for button down transition */
#define IDL_CURS_HIDE_ORIGINAL 9   /* Restore cursor to its original shape
				      (window systems) instead of blanking
				      it. */

/* Define coordinate system types: */
#define IDL_COORD_DATA 0
#define IDL_COORD_DEVICE 1
#define IDL_COORD_NORMAL 2
#define IDL_COORD_MARGIN 3
#define IDL_COORD_IDEVICE 4

#define IDL_PX 0		/* Subscripts for each point member */
#define IDL_PY 1
#define IDL_PZ 2
#define IDL_PH 3

typedef union {			/* Describe a point: */

  struct {			
    float x,y,z,h;
  } d;                          /* Single precision point,
				 * referenced by .x, .y, etc., in
				 * homogeneous coordinates. */

  struct {			
    int x,y;
  } i;                          /* Integer point,
				 * referenced by .x and .y. */

  struct {                      

    double x,y,z,h;
  } d_s;                        /* Double precision point,
				 * referenced by .x, .y, etc., in
				 * homogeneous coordinates. */

  float p[4];			/* Single precision point,
				 * referred to by [0],[1], etc. */

  double d_arr[4];              /* Double precision point,
				 * referred to by [0],[1], etc. */

  struct {
    int x, y;
    float z, h;
  } dev;                        /* Device coordinate (x,y) plus Z value
				 *  (for devices that support a Z-buffer),
				 *  and homogeneous coordinate (for 
				 *  transform of texture coordinates). */ 
} IDL_GR_PT;

typedef enum {
  IDL_GR_PT_UNKNOWN       = 0,
  IDL_GR_PT_INT_STRUCT    = 1, /* IDL_GR_PT.i     */
  IDL_GR_PT_FLOAT_STRUCT  = 2, /* IDL_GR_PT.d     */
  IDL_GR_PT_FLOAT_ARRAY   = 3, /* IDL_GR_PT.p     */
  IDL_GR_PT_DOUBLE_STRUCT = 4, /* IDL_GR_PT.d_s   */
  IDL_GR_PT_DOUBLE_ARRAY  = 5, /* IDL_GR_PT.d_arr */
  IDL_GR_PT_DEV_STRUCT    = 4  /* IDL_GR_PT.dev   */
} IDL_GR_PT_TYPE_e;

typedef struct {
  IDL_GR_PT_TYPE_e  type;   /* Indicates the field of IDL_GR_PT union to use */
  int               coord;  /* Coordinate system.  IDL_COORD_* */
  IDL_GR_PT         pt;
} IDL_GR_TYPED_PT;

typedef struct {
	IDL_GR_PT origin;
	IDL_GR_PT size;
} IDL_GR_BOX;
    
typedef struct {		/* Attributes structure for points & lines */
  IDL_ULONG color;		/* Specifys all that can go wrong w/ graphic */
  float thick;
  int linestyle;
  double *t;			/* NULL for no 3d transform, or pointer to
				   4 by 4 matrix. */
  int *clip;			/* NULL for no clipping or ^ to [2][2]
				   clipping rectangle in device coord. */
  IDL_AXIS *ax,*ay,*az;		/* Axis definitions */
  int chl;			/* For devices with multiple channels */
} IDL_ATTR_STRUCT;

typedef struct {		/* Graphic text attribute structure.
				   Passed to text routines. */
  int font;			/* 0=hdw, -1=hershey, 1=TrueType */
  int axes;			/* Text axes, 0 = xy, 1 = xz, 2 =yz,
				   3 = yx, 4 = zx, 5 = zy */
  float size;			/* Text size, 1.0 = normal */
  float orien;			/* Orientation, degrees CCW from normal */
  float align;			/* Justification, 0.0 = left, 1.0 = right,
				   0.5  = centered. */
} IDL_TEXT_STRUCT;


/* Structure that defines secondary paramenters for imaging */
typedef struct {		/* Imaging attribute structure */
  short xsize_exp;		/* Non-0 if xsize is EXPlictily set by user */
  short ysize_exp;		/* Non-0 if ysize is EXPlictily set by user */
  IDL_LONG xsize, ysize;	/* Requested size of image (dev coords) */
  int chl;			/* Channel */
  int order;			/* Image order - 0 bottom to top */
  /* Three element array giving the stride between colors of the same
     pixel, adjacent pixels of the same color, and rows of the same color.
     color_stride[0] is non-zero for true color. */
  int color_stride[3];
  int image_is_scratch;		/* True if source image is a temp */
  int b_per_pixel;		/* # of bytes/pixel */
} IDL_TV_STRUCT;


/*
 * Prototypes for functions for the IDL_DEVICE_CORE struct (below)
 * also needed for IDL_POLYFILL_ATTR.
 */
typedef void (* IDL_DEVCORE_FCN_DRAW)(IDL_GR_PT *p0, IDL_GR_PT *p1,
				      IDL_ATTR_STRUCT *a);
typedef void (* IDL_DEVCORE_FCN_RW_PIXELS)(UCHAR *data, int x0, int y0, int nx,
					   int ny, int dir,
					   IDL_TV_STRUCT *secondary);

typedef struct {
  IDLBool_t bInterior;          /* True if current ROI is interior */
  IDL_LONG iNAllocEdgeLists;    /* # of allocated edges */
  IDL_LONG iNUsedEdgeLists;     /* # of used edges */
  UCHAR **ppEdgeLists;          /* ^ to edge lists */
  IDL_MEMINT iBottomY, iTopY;   /* Overall bottom & top scanline Y */
} IDL_ROI_STATE;

typedef struct {		/* Structure defining polygon fills */
  enum {
    POLY_SOLID, POLY_PATTERN, POLY_IMAGE, POLY_GOURAUD, POLY_IMAGE3D
    } fill_type;
  IDL_ATTR_STRUCT *attr;	/* Graphics attribute structure */
  union {			/* Drawing routine to use */
    IDL_DEVCORE_FCN_DRAW draw;
    IDL_DEVCORE_FCN_RW_PIXELS rw_pixels;
  } rtn;
  union {			/* Operation dependent params: */
    struct {			/* Image fill  */
      UCHAR *data;		/* Fill data for image fill */
      int d1, d2;		/* Dimensions of fill data */
      float *im_verts;		/* Image coords of verts */
      float *im_w;		/* Screen vert W coords */
      UCHAR interp;		/* TRUE to interpolate in image space */
      UCHAR transparent;	/* Transparency threshold, 0 for none */
      UCHAR im_depth;           /* Z device only: 1 byte or 3 bytes */
    } image;
    struct {			/* Line-pattern fill: */
      float angle;		/* Fill orientation in degrees */
      int spacing;		/* Line spacing, in device units */
      float ct, st;		/* Cos / sin of rotation */
    } lines;
    int fill_style;		/* Hardware dependent fill style for
				   POLY_SOLID */
  } extra;
  struct {			/* Info used only with Z buffer device */
    union {
	float *f;
	double *d;
    }z;				/* The Z values */
    int precision;              /* Precision for the Z values. */
    int *shades;		/* Shading values at verts for POLY_GOURAUD */
  } three;

  IDL_ROI_STATE *pROIState;     /* Pointer to ROI state. */

} IDL_POLYFILL_ATTR;


/*
 * Prototypes for remaining functions in IDL_DEVICE_CORE struct (below)
 */
typedef int (* IDL_DEVCORE_FCN_TEXT)(IDL_GR_PT *p, IDL_ATTR_STRUCT *ga,
				     IDL_TEXT_STRUCT *ta, char *text);
typedef void (* IDL_DEVCORE_FCN_ERASE)(IDL_ATTR_STRUCT *a);
typedef void (* IDL_DEVCORE_FCN_CURSOR)(int funct, IDL_MOUSE_STRUCT *m);
typedef void (* IDL_DEVCORE_FCN_POLYFILL)(int *x, int *y, int n,
					  IDL_POLYFILL_ATTR *poly);
typedef void (* IDL_DEVCORE_FCN_INTER_EXIT)(void);
typedef void (* IDL_DEVCORE_FCN_FLUSH)(void);
typedef void (* IDL_DEVCORE_FCN_LOAD_COLOR)(IDL_LONG start, IDL_LONG n);
typedef void (* IDL_DEVCORE_FCN_DEV_SPECIFIC)(int argc, IDL_VPTR *argv,
					      char *argk);
typedef void (* IDL_DEVCORE_FCN_DEV_HELP)(int argc, IDL_VPTR *argv);
typedef void (* IDL_DEVCORE_FCN_LOAD_RTN)(void);
typedef void (* IDL_DEVCORE_FCN_RESET_SESSION)(void);

/*
 * IDL_DEVICE_CORE defines the core functions required by every
 * device driver. Most fields can be filled with a NULL indicating
 * the ability doesn't exist. draw and erase are exceptions to
 * this --- If you can't do that much, why bother with a driver?
*/
typedef struct {
  IDL_DEVCORE_FCN_DRAW		draw;
  IDL_DEVCORE_FCN_TEXT		text;
  IDL_DEVCORE_FCN_ERASE		erase;
  IDL_DEVCORE_FCN_CURSOR	cursor;   /* cursor inquire and set */
  IDL_DEVCORE_FCN_POLYFILL	polyfill;   /* Fill irregular polygon */
  IDL_DEVCORE_FCN_INTER_EXIT	inter_exit;   /* Return to interactive mode */
  IDL_DEVCORE_FCN_FLUSH		flush;
  IDL_DEVCORE_FCN_LOAD_COLOR	load_color;
  IDL_DEVCORE_FCN_RW_PIXELS	rw_pixels;   /* Pixel input/output */
  IDL_DEVCORE_FCN_DEV_SPECIFIC	dev_specific;   /* DEVICE procedure */
  IDL_DEVCORE_FCN_DEV_HELP	dev_help;   /* HELP,/DEVICE */
  IDL_DEVCORE_FCN_LOAD_RTN	load_rtn;   /* Call when driver is loaded */
				/* Call for .reset_session executive command
				   to close and release windows. */
  IDL_DEVCORE_FCN_RESET_SESSION	reset_session;
} IDL_DEVICE_CORE;


/*
 * IDL_DEVICE_WINDOW contains pointers to functions that accomplish
 * window system operations. If the device is a window system,
 * every field in this struct must point at a valid function,
 * they're called without checking.
 */
typedef struct {		/* Procedures & functions for image device: */
  void (* window_create)(int argc, IDL_VPTR *argv,char *argk);
  void (* window_delete)(int argc, IDL_VPTR *argv);
  void (* window_show)(int argc, IDL_VPTR *argv, char *argk);
  void (* window_set)(int argc, IDL_VPTR *argv);
  IDL_VPTR (* window_menu)(int argc, IDL_VPTR *argv, char *argk);
} IDL_DEVICE_WINDOW;

/*
 * IDL_DEVICE_DEF is the interface between a device driver and the rest
 * of the Structure defining a graphics device. Every field in this
 * structure must contain valid information --- it is used without
 * any error checking.
 */
typedef struct {		/* Device descriptor, mostly static attributes
				   and definitions: */
  IDL_STRING name;		/* Device name */
  int t_size[2];		/* Total size in device coordinates */
  int v_size[2];		/* Visible area size, device coords */
  int ch_size[2];		/* Default character sizes */
  float px_cm[2];		/* Device units / centimeter, x & y. */
  int n_colors;			/* # of possible simultaneous colors */
  int table_size;		/* # of color table elements */
  int fill_dist;		/* minimum line spacing for solid fill */
  int window;			/* Current window number */
  int unit;			/* Unit number of output file */
  int flags;			/* Advertise limitations and abilities */
  int origin[2];		/* Display XY (pan/scroll) origin */
  int zoom[2];			/* Display XY zoom factors */
  float aspect;			/* Aspect ratio, = v_size[0] / v_size[1]. */
  IDL_DEVICE_CORE core;		/* Core graphics */
  IDL_DEVICE_WINDOW winsys;	/* Window system. Only required if D_WINDOWS */
  char *reserved;		/* Set to zero. */
} IDL_DEVICE_DEF;


/* Define bits in IDL_DEVICE_DEF flags: */

#define IDL_D_SCALABLE_PIXELS 1	  /* True if pixel size is variable (e.g. PS)*/
#define IDL_D_ANGLE_TEXT (1 << 1)   /* True if device can output text at
				       angles */
#define IDL_D_THICK (1 << 2)	/* True if line thickness can be set */
#define IDL_D_IMAGE (1 << 3)	/* True if capable of imaging */
#define IDL_D_COLOR (1 << 4)	/* True if device supports color */
#define IDL_D_POLYFILL (1 << 5)	  /* True if device can do polyfills */
#define IDL_D_MONOSPACE (1<<6)	 /* True if device has only monospaced text */
#define IDL_D_READ_PIXELS (1<<7)   /* True if device can read back pixels */
#define IDL_D_WINDOWS (1<<8)	/* True if device supports windows */
#define IDL_D_WHITE_BACKGROUND (1<<9)	/* True if device background is
					   white, like PostScript. */
#define IDL_D_NO_HDW_TEXT (1<<10)   /* True if device has no hardware text */
#define IDL_D_POLYFILL_LINE (1<<11)   /* True to use device driver for line
					 style polyfills. */
#define IDL_D_HERSH_CONTROL (1<<12)   /* True if device accepts hershey style
					 control characters. */
#define IDL_D_PLOTTER (1<<13)	/* True if pen plotter */
#define IDL_D_WORDS (1<<14)	/* True if device images can be words */
#define IDL_D_KANJI (1 << 15)	/* Device has Kanji characters */
#define IDL_D_WIDGETS (1 << 16)	  /* Device supports graphical user
				     interfaces */
#define IDL_D_Z (1 << 17)	/* Device is 3d */
#define IDL_D_TRUETYPE_FONT (1 << 18) /* Device supports TrueType fonts. */

typedef struct {
  int background;		/* Background color */
  float charsize;		/* Global Character size */
  float charthick;		/* Character thickness */
  int clip[6];			/* Clipping rectangle, normalized coords */
  IDL_ULONG color;		/* Current color */
  int font;			/* Font */
  int linestyle;		/* Line style */
  int  multi[5];		/* Cnt, Cols/rows, major dir for multi plts. */
  int clip_off;			/* True if clipping is disabled */
  int noerase;			/* No erase flag */
  int nsum;			/* Number of points to sum */
  float position[4];		/* Default window */
  int psym;			/* Marker symbol */
  float region[4];		/* Default plotting region */
  IDL_STRING subtitle;		/* Plot subtitle */
  float symsize;		/* Symbol size */
  double t[16];			/* Matrix (4x4) of homogeneous transform */
  int  t3d_on;			/* True if 3d homo transform is on */
  float thick;			/* Line thickness */
  IDL_STRING title;		/* Main plot title */
  float ticklen;		/* Tick length */
  int chl;			/* Default channel */
  /* Fields below this are not visible to the IDL user */
  double sr_restore_pad;	/* Save/Restore has the user view of this
				   struct. Hence, its length calculation for
				   this struct stops with the field above this
				   one. This field ensures that the alignment
				   padding generated by the C compiler will
				   not cause RESTORE to overwrite these hidden
				   fields. */
  IDL_DEVICE_DEF *dev;		/* Current output device, not user accessible*/
} IDL_PLOT_COM;



#endif				/* graphics_IDL_DEF */




/***** Definitions from keyword *****/


#ifndef keyword_IDL_DEF
#define keyword_IDL_DEF

/* Bit values of IDL_KW_PAR flags field: */

#define IDL_KW_ARRAY (1 << 12)
/* If specified array is required, otherwise scalar required */

#define IDL_KW_OUT (1 << 13)
/* Indicates parameter is an OUTPUT parameter passed by reference.
   Expressions are excluded.  The address of the IDL_VARIABLE is stored in
   the value field.  Otherwise, no checking is performed.  Special hint:
   to find out if a IDL_KW_OUT parameter is specified, use 0 for the type,
   and IDL_KW_OUT | IDL_KW_ZERO for the flags.  The value field will either
   contain NULL or the pointer to the variable. */


#define IDL_KW_VIN (IDL_KW_OUT | IDL_KW_ARRAY)
/* Parameter is an INPUT parameter passed by reference.  The address
   of the IDL_VARIABLE or expression is stored in the value field as with
   IDL_KW_OUT.  If this flag is specified, kw_cleanup() must be called to
   properly reap temporaries that may have been allocated. */


#define IDL_KW_ZERO (1 << 14)
/* If set, zero the parameter before parsing the keywords.  I.e. if
   this bit is set, and the parameter is not specified, the value will
   always be 0. */

#define IDL_KW_VALUE (1 << 15)
/* If this bit is set and the keyword is present, and its value is
   non-zero, the low 12 bits of this field will be inclusive 'or'ed with
   the longword pointed to by IDL_KW_PAR.value.  Be sure that the type field
   contains TYP_LONG.  The largest value that may be specified is
   (2^12)-1.  Negative values are not allowed.  For example, if the
   IDL_KW_PAR struct contains:

   {"DEVICE", TYP_LONG, 1, IDL_KW_ZERO|IDL_KW_VALUE|4, 0,IDL_KW_OFFSETOF(xxx)},
   {"NORMAL", TYP_LONG, 1, IDL_KW_VALUE|3, 0, IDL_KW_OFFSETOF(xxx)},

   then xxx will contain a 3 if /NORMAL, or NORMAL = (expr) is
   present, a 4 if /DEVICE is present, 7 if both are set, and 0 if
   neither.  IDL_KW_ZERO can also be used in combination with this flag, use
   it only once for each IDL_KW_PAR.value.  */



#define IDL_KW_VALUE_MASK ((1 << 12) -1)   /* Mask for value part */

/* Use IDL_KW_FAST_SCAN as the first element of the keyword array if
   there are more than approximately 5 or 10 elements in the keyword
   array.  The IDL_KW_PAR structure defined by this macro is used to
   point to a list of elements to zero, and speeds processing of long
   keyword lists.  NEVER touch the contents of this structure.
   */
#define IDL_KW_FAST_SCAN { (char *)"", 0,0,0,0,0 }

/*
 * Macro used to compute offsets into result structure in the
 * case of IDL_KWProcessByOffset() for the specified and value
 * fields of IDL_KW_PAR.
 *
 * entry:
 *	s - Name of structure typedef
 *	m - Name of field within structure
 */
#define IDL_KW_OFFSETOF2(s, m) ((void *)(&(((s *)0)->m)))
#define IDL_KW_V_OFFSETOF2(s, m) ((char *)(&(((s *)0)->m)))
#define IDL_KW_S_OFFSETOF2(s, m) ((int *)(&(((s *)0)->m)))

/*
 * Simplified version of IDL_KW_OFFSET2 that assumes a structure
 * typedef named KW_RESULT.
 */
#define IDL_KW_OFFSETOF(m) IDL_KW_OFFSETOF2(KW_RESULT, m)
#define IDL_KW_V_OFFSETOF(m) IDL_KW_V_OFFSETOF2(KW_RESULT, m)
#define IDL_KW_S_OFFSETOF(m) IDL_KW_S_OFFSETOF2(KW_RESULT, m)


/*
 * Macro for specifying address in IDL_KW_PAR or IDL_KW_ARR_DESC
 * for use with IDL_KWProcessByAddr() or the obsolete IDL_KWGetParams().
 */
#define IDL_KW_ADDROF(x) ((char *) &(x))


/*
 * Macro for specifying address of IDL_KW_ARR_DESC or IDL_KW_ARR_DESC_R
 * in IDL_KW_PAR.
 */
#define IDL_KW_ARR_DESC_ADDROF(x) ((char *) &(x))


typedef struct {
  const char *keyword;		/* ^ to Keyword string, NULL terminated.
				   A NULL keyword string pointer value
				   terminates the keyword structure.  Strings
				   must be UPPER case and in LEXICAL order .*/
  UCHAR type;			/* Type of data required. This may be any
				   simple type or 0 in cases where the type
				   is determined by other factors
				   (KW_OUT, KW_VIN, KW_VALUE, KW_SYMCONST). */
  unsigned short mask;		/* Enable mask.  This field is AND'ed with
				   the mask field in the call to
				   GET_IDL_KW_PARAMS, and if the result is
				   non-zero the keyword is used. If it is 0,
				   the keyword is ignored.  */
  unsigned short flags;		/* Contains  flags as described above */
  int *specified;		/* (A/O) Address/Offset of int to set on
				   return if param is specified. May be null
				   if this information is not required. */
  char *value;			/* (A/O) In the case of arrays, this is a real
				   pointer to the IDL_KW_ARR_DESC
				   (IDL_KWGetParams() or
				   IDL_KWProcessByAddr()) or
				   IDL_KW_ARR_DESC_R (IDL_KWProcessByOffset())
				   structure for the data to be returned. In
				   the case of KW_SYMCONST, it is a real
				   pointer to the IDL_KW_SYMCONST_DESC
				   structure. In all other cases, it is the
				   address/offset of value to return. */
} IDL_KW_PAR;



/*
 * Descriptors for array's that are returned. We are forced to have
 * two versions of this struct because the n field of IDL_KW_ARR_DESC
 * causes it to be non-reentrant. These two structs share the initial
 * tags, meaning that a pointer to either type can be used to alter them.
 * The one to use depends on the keyword processing function you use:
 *
 *            function                    structure
 *            ----------------------------------------------
 *             IDL_KWGetParams()          IDL_KW_ARR_DESC
 *             IDL_KWProcessByAddr()      IDL_KW_ARR_DESC
 *             IDL_KWProcessByOffset()    IDL_KW_ARR_DESC_R
 *
 * Note that IDL_KWGetParams() is obsolete, and its use it not encouraged.
 */

#define IDL_KW_COMMON_ARR_DESC_TAGS \
  char *data;			/* Address of array to receive data. */ \
  IDL_MEMINT nmin;		/* Minimum # of elements allowed. */ \
  IDL_MEMINT nmax;		/* Maximum # of elements allowed. */

typedef struct {		/* Descriptor for array's that are returned */
  IDL_KW_COMMON_ARR_DESC_TAGS
  IDL_MEMINT n;			/* # present, (Returned value). */
} IDL_KW_ARR_DESC;

typedef struct {
  IDL_KW_COMMON_ARR_DESC_TAGS
  IDL_MEMINT *n_offset;		/* Address/Offset of variable to receive
				   # elements present, (Returned value). */
} IDL_KW_ARR_DESC_R;

/*
 * IDL_KWGetParams() and IDL_KWCleanup() (along with the IDL_KW_MARK
 * and IDL_KW_CLEAN flags) are obsolete and should not be used in new
 * code. However, they remain available for use by existing code.
 *
 * We recommend converting your code to use IDL_KWProcessByAddr()
 * or IDL_KWProcessByOffset() with IDL_KWFree(). This newer API is simpler
 * to understand and use, and therefore less error prone.
 */
extern int IDL_CDECL IDL_KWGetParams(int argc, IDL_VPTR *argv, char *argk,
				     IDL_KW_PAR *kw_list,
				     IDL_VPTR *plain_args, int mask);
extern void IDL_CDECL IDL_KWCleanup(int fcn);

#define IDL_KW_MARK 1		/* Mark string stack before calling get_
				   kw_params.  */
#define IDL_KW_CLEAN 2		/* Clean temps & strings created since
				   last call with KW_MARK. */

/*
 * Use of IDL_ProcessByOffset() requires the first field of the KW_RESULT
 * typedef to be an int used for deciding if IDL_KWFree() should be
 * called. These macros simplifiy coding this.
 *
 * IDL_KW_RESULT_FIRST_FIELD must be the first entry in KW_RESULT. IDL_KW_FREE
 * is then used to cleanup before exiting the routine. The structure of
 * type KW_RESULT is expected to be called kw.
 */
#define IDL_KW_RESULT_FIRST_FIELD int _idl_kw_free
#define IDL_KW_FREE if (kw._idl_kw_free) IDL_KWFree()



#endif				/* keyword_IDL_DEF */




/***** Definitions from lmgr *****/

/*
 * Define licensing request codes for the exported API
 */
#define IDL_LMGR_CLIENTSERVER     0x01
#define IDL_LMGR_DEMO             0x02
#define IDL_LMGR_EMBEDDED         0x04
#define IDL_LMGR_RUNTIME          0x08
#define IDL_LMGR_STUDENT          0x10
#define IDL_LMGR_TRIAL            0x20
#define IDL_LMGR_CALLAPPNOCHECKOUT      0x40
#define IDL_LMGR_CALLAPPLICINTERNAL		0x80
#define IDL_LMGR_VM               0x100 /* AC */

/*
 * Define licensing codes for setting licensing options in the exported API
 */
#define IDL_LMGR_SET_FORCEDEMO		0x01
#define IDL_LMGR_SET_NOCOMPILE		0x02
#define IDL_LMGR_SET_NORESTORE		0x04




/***** Definitions from os *****/

#ifndef os_IDL_DEF
#define os_IDL_DEF


/* Buffer used to hold a standard ASCII 24-character date string. */
typedef char IDL_ATIME_BUF[25];   /* Extra byte for NULL termination */

/* Structure passed to IDL_GetUserInfo() */
#define IDL_USER_INFO_MAXHOSTLEN 64
typedef struct {
  char *logname;			/* Users login name */
  char *homedir;			/* User's home directory */
  char *pid;				/* The process ID */
  char host[IDL_USER_INFO_MAXHOSTLEN];	/* The machine name */
  char wd[IDL_MAXPATH+1];		/* The current directory */
  IDL_ATIME_BUF date;			/* The current date */
} IDL_USER_INFO;

/* Possible values for type argument to IDL_GetKbrdInternal() */
typedef enum {
  IDL_GETKBRD_T_CH=0,		/* Return a single ASCII character */
  IDL_GETKBRD_T_CH_OR_ESC=1,	/* Return a single ASCII character or
				   a single ANSI escape sequence. Escape
				   sequences can only be generated by a Unix
				   tty based front end. In all other situations,
				   this request is equivalent to
				   IDL_GETKBRD_T_CH. */
  IDL_GETKBRD_T_CH_OR_NAME=2	/* Return a single ASCII character or
				   the name of a function key. */
} IDL_GETKBRD_T;

/* Type of buffer argument to IDL_GetKbrdInternal() */
typedef char IDL_GETKBRD_BUFFER[128];
#endif				/* os_IDL_DEF */




/***** Definitions from pout *****/

#ifndef pout_IDL_DEF
#define pout_IDL_DEF

/*** Mask values for flags argument to pout() ***/
#define IDL_POUT_SL         1	/* Start on a new line */
#define IDL_POUT_FL         2	/* Finish current line */
#define IDL_POUT_NOSP       4	/* Don't add leading space */
#define IDL_POUT_NOBREAK    8	/* Don't start a new line if too long */
#define IDL_POUT_LEADING    16	/* Print leading text at start of line */
#define IDL_POUT_FORCE_FL   32  /* Finish current line, even if it is empty */


/*** Structure for control argument to pout() ***/
typedef struct {
  int unit;			/* LUN of open file */
  int curcol;			/* Current output column */
  int wrap;			/* # chars at which buf should flush */
  char *leading;		/* String to output at start of each line */
  int leading_len;		/* Length of leading w/o terminating null */
  char *buf;			/* ^ to output buffer. Must be max_len chars */
  int max_len;			/* Length of buffer */
} IDL_POUT_CNTRL;

/* Macro for common case of writing a blank line to the file */
#define IDL_POUT_BLANK_LINE(pout_cntrl) \
  IDL_Pout(pout_cntrl, IDL_POUT_SL|IDL_POUT_FORCE_FL, (char *) 0, "")

#endif				/* pout_IDL_DEF */




/***** Definitions from prog_nam *****/

#ifndef prog_nam_IDL_DEF
#define prog_nam_IDL_DEF
#endif				/* prog_nam_IDL_DEF */




/***** Definitions from ptr *****/

#ifndef ptr_IDL_DEF
#define ptr_IDL_DEF
typedef struct idl_heap_variable {
  struct idl_heap_variable *hash; /* THIS MUST BE THE FIRST FIELD IN the
				     IDL_HEAP_VARIABLE struct!!! */
  IDL_HVID hash_id;		/* pointer value assigned to this heap var */
  IDL_LONG refcount;            /* Reference counting */
  int flags;			/* Bitmask of IDL_HV_F_* values */
  IDL_VARIABLE var;		/* Associated variable */
  
} IDL_HEAP_VARIABLE;
typedef IDL_HEAP_VARIABLE *IDL_HEAP_VPTR;

/* Flags for IDL_HeapVarNew() */
#define IDL_HEAPNEW_NOCOPY     1   /* Steal the dynamic part */
#define IDL_HEAPNEW_GCDISABLE  2   /* Disable garbage collection */
#define IDL_HEAPNEW_ZEROREF    4   /* Start with a refcount of 0 instead of 1 */
#endif				/* ptr_IDL_DEF */




/***** Definitions from raster *****/

#ifndef raster_IDL_DEF
#define raster_IDL_DEF


/*** Allowed values for dither_method field of IDL_RASTER_DEF struct ***/
#define IDL_DITHER_REVERSE          0 /* "Un-dither" back to bytes */
#define IDL_DITHER_THRESHOLD        1 /* Threshold dithering  */
#define IDL_DITHER_FLOYD_STEINBERG  2 /* Floyd Steinberg method */
#define IDL_DITHER_ORDERED          3 /* Ordered dither */

/* Values for flags field: */
#define IDL_DITHER_F_WHITE 0x01	/* Device has white background, dithering
				   module then sets the black bits. */
#define IDL_RASTER_1BYTEPP 0x02 /* Raster buffer is set up for 1 byte
				 * per pixel (default is 1 bit per pixel) */

/*** Convenience values for the bit_tab array of IDL_RASTER_DEF struct ***/
#define IDL_RASTER_MSB_LEFT { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 }
#define IDL_RASTER_MSB_RIGHT { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 }

typedef struct {		/* Information that characterizes a raster */
  UCHAR *fb;			/* Address of frame buffer */
  int nx, ny;			/* Size of frame buffer in pixels */
  int bytes_line;		/* # of bytes per scan line, must be whole. */
  int byte_padding;		/* Pad lines to a multiple of this amount,
				   Must be a power of 2.  */
  int dot_width;		/* The length of a dot for the linestyles.
				   Default = 1. */
  int dither_method;		/* Dithering method code. */
  int dither_threshold;		/* Threshold value for threshold dither */
  UCHAR bit_tab[8];		/* Table of set bits, bit_tab[0] is leftmost,
				   bit_tab[7] is right most. */
  int flags;			/* Raster flags, see above */
} IDL_RASTER_DEF;

#endif				/* raster_IDL_DEF */




/***** Definitions from rline *****/

#ifndef rline_IDL_DEF
#define rline_IDL_DEF

/**** Flags to OR together for options parameter to IDL_RlineRead() ****/
#define IDL_RLINE_OPT_NOSAVE        1   /* Don't save in recall buffer */
#define IDL_RLINE_OPT_NOJOURNAL     2   /* Don't journal */
#define IDL_RLINE_OPT_JOURCMT       4   /* Put a '; ' at start in journal */
#define IDL_RLINE_OPT_NOEDIT        8   /* Like (!EDIT_INPUT = 0) for one call*/

#endif				/* rline_IDL_DEF */




/***** Definitions from sfile *****/

#ifndef sfile_IDL_DEF
#define sfile_IDL_DEF

/**** Stream file flags type ****/
typedef IDL_LONG IDL_SFILE_FLAGS_T;


typedef IDL_LONG IDL_SFILE_PIPE_EXIT_STATUS;

/*
 * Time information. All fields are based on the Posix epoch
 * of 1 January 1970. Systems with a different native epoch
 * convert to this base.
 */
typedef struct {
  IDL_LONG64 access;		/* Time of last file access,  */
  IDL_LONG64 create;		/* Time of creation of file */
  IDL_LONG64 mod;		/* Time of last Modification of file. */
} IDL_SFILE_STAT_TIME;

#endif				/* sfile_IDL_DEF */




/***** Definitions from sig *****/

#ifndef sig_IDL_DEF
#define sig_IDL_DEF


#include <signal.h>

/* Dialect confusion from HP-UX */
#if defined(SIGWINDOW) && !defined(SIGWINCH)
#define SIGWINCH SIGWINDOW
#endif

/*
 * Signal sets are represented by this opaque type. The type and length
 * have been selected to be suitable for any platform.
 */
typedef struct {
#ifdef linux
  unsigned long set[_SIGSET_NWORDS];
#else
  double set[4];
#endif
} IDL_SignalSet_t;

/* The IDL definition for all signal handler functions. */
typedef void (* IDL_SignalHandler_t)(int signo);

#endif				/* sig_IDL_DEF */




/***** Definitions from structs *****/

#ifndef structs_IDL_DEF
#define structs_IDL_DEF


/* Valid bits for flags field of IDL_STRUCT_TAG_DEF */
#define IDL_STD_INHERIT	1	/* Type must be a structure. This flag
				   indicates that the structure is inherited
				   (inlined) instead of making it a
				   sub-structure as usual. */

typedef struct {		/* A tag definition for K_MakeSTruct */
  char *name;			/* Name of the tag. Must be upper case
				   and obey the rules for IDL identifiers.
				   In the case of inherited structures, this
				   can be NULL if type is set. Otherwise, it
				   is the name of the structure being inherited
				   and IDL will call a __DEFINE procedure
				   based on that name to define it. */
  IDL_MEMINT *dims;		/* NULL pointer for a scalar tag, otherwise
				   an array giving the array dimensions. The
				   first element is the number of dimensions,
				   and is followed in order by the dimensions
				   themselves. */
  void *type;			/* This may be either a pointer to another
				   structure definition, or a simple IDL
				   type code (IDL_TYP_*) cast to void
				   (e.g. (void *) IDL_TYP_BYTE). If this
				   field is NULL, it indicates that IDL
				   should search for a structure of the
				   given name and fill in the pointer to
				   its structure definition. */
  UCHAR flags;			/* Bitmask made up of IDL_STD_* bits */
} IDL_STRUCT_TAG_DEF;
#endif				/* structs_IDL_DEF */




/***** Definitions from sysnames *****/

#ifndef sysnames_IDL_DEF
#define sysnames_IDL_DEF
/*
 * Possible values for the options argument to IDL_SysRtnEnable().
 */
#define IDL_SRE_B_DISABLE	1   /* Setting this bit means disable,
				       leaving it clear means enable. */
#define IDL_SRE_B_EXCLUSIVE	2   /* Setting this bit means exclusive,
				       leaving it clear means other routines
				       are not altered. */

#define IDL_SRE_ENABLE			0   /* Enable specified routines */
				/* Enable specified routines and
				   disable all others. */
#define IDL_SRE_ENABLE_EXCLUSIVE	IDL_SRE_B_EXCLUSIVE
				/* Disable specified routines */
#define IDL_SRE_DISABLE			IDL_SRE_B_DISABLE
				/* Disable specified routines and
				   enable all others. */
#define IDL_SRE_DISABLE_EXCLUSIVE	(IDL_SRE_B_DISABLE|IDL_SRE_B_EXCLUSIVE)



#endif				/* sysnames_IDL_DEF */




/***** Definitions from sysnames_obs *****/

#ifndef sysnames_obs_IDL_DEF
#define sysnames_obs_IDL_DEF

/*
 * IDL_SYSFUN_DEF and the corresponding function IDL_AddSystemRoutines()
 * are obsolete, and are no longer used within the IDL kernel. They have
 * been replaced by IDL_SYSFUN_DEF2 and IDL_SysRtnAdd(). These old
 * interfaces and are supported here until people have had a chance to
 * modify their code to use the new interfaces. Ultimately, these
 * definitions will be moved to obsolete.h.
 */

/*
 * The IDL_SYSFUN_DEF  structure defines the format of a system procedure
 * or function table entry. Note that it has been superseded by the
 * IDL_SYSFUN_DEF2 structure, and is not intended for new code.
 */
typedef struct {		/* System function definition */
  IDL_FUN_RET funct_addr;	/* Address of function, or procedure. */
  char *name;			/* The name of the function */
  UCHAR arg_min;		/* Minimum allowed argument count. */
  UCHAR arg_max;		/* Maximum argument count.  The top
				   bit in arg_min is set to indicate that
				   the routine accepts keywords. */
  UCHAR flags;			/* IDL_SYSFUN_DEF_F_* flags */
} IDL_SYSFUN_DEF;

/*
 * Setting the top bit in the arg_min field of an IDL_SYSFUN_DEF passed to
 * IDL_AddSystemRoutine() is equivalent to the setting the
 * IDL_SYSFUNDEF_F_KEYWORDS bit in the flags field. This is strictly for
 * backwards compatibility. Direct use of the flags field is preferred.
 *
 * Also, note that IDL_SYSFUN_DEF2 and IDL_SysRtnAdd() do not support
 * this, and only look at the IDL_SYSFUNDEF_F_KEYWORDS bit.
 */
#define IDL_KW_ARGS 128		/* Bit set in argmin indicating kw's allowed */

#endif				/* sysnames_obs_IDL_DEF */




/***** Definitions from sysv *****/

#ifndef sysv_IDL_DEF
#define sysv_IDL_DEF
/*
 * These #defines allow use of older error related system variables and
 * map them to the corresponding field in !ERROR_STATE. Programmers
 * should convert their code to use the new names instead.
 */
#define IDL_SysvErrString IDL_SysvErrorState.msg
#define IDL_SysvSyserrString IDL_SysvErrorState.sys_msg
#define IDL_SysvErrorCode IDL_SysvErrorState.code
#define IDL_SysvSyserrorCodes IDL_SysvErrorState.sys_code


/*
 * Allowed values for type argument to IDL_SysvValuesGetFloat() and
 * IDL_SysvValuesGetDouble().
 */
#define IDL_SYSVVALUES_INF	0
#define IDL_SYSVVALUES_NAN	1

#endif				/* sysv_IDL_DEF */




/***** Definitions from tout *****/

#ifndef tout_IDL_DEF
#define tout_IDL_DEF

typedef void (* IDL_TOUT_OUTF)(int flags, char *buf, int n);

#define IDL_TOUT_F_STDERR   1	/* Output to stderr instead of stdout */
#define IDL_TOUT_F_NLPOST   4	/* Output a newline at end of line */

/*
 * When tout is doing "more(1)" processing, and prompts for user
 * input, these are the possible responses.
 */
#define IDL_TOUT_MORE_RSP_QUIT	0   /* Quit */
#define IDL_TOUT_MORE_RSP_PAGE	1   /* Continue for another page */
#define IDL_TOUT_MORE_RSP_LINE	2   /* Continue for a single line */
     
#endif				/* tout_IDL_DEF */




/***** Definitions from ur_main *****/

#ifndef ur_main_IDL_DEF
#define ur_main_IDL_DEF


/* Values that are OR'd together to form the options field of IDL_INIT_DATA */
#define IDL_INIT_GUI		1   /* Use the GUI interface. */
#define IDL_INIT_GUI_AUTO	(IDL_INIT_GUI|2)
				/* Try to use a GUI if possible. If that
				   fails and the OS supports ttys, use
				   the standard tty interface. Note that
				   this code includes IDL_INIT_GUI. */
#define IDL_INIT_RUNTIME	 4   /* RunTime IDL. */
#define IDL_INIT_EMBEDDED	(IDL_INIT_RUNTIME|8)
				/* Embedded IDL. Note that this code includes
				   IDL_INIT_RUNTIME. */
#define IDL_INIT_NOLICALIAS	16
				/* Our FlexLM (Unix) floating license
				   policy is to alias all IDL sessions that
				   share the same user/system/display to the
				   same license. If no_lic_alias is set,
				   this IDL session will force a unique
				   license to be checked out. In this case,
				   we allow the user to change the DISPLAY
				   environment variable. This is useful for
				   RPC servers that don't know where their
				   output will need to go before invocation.*/
#define IDL_INIT_BACKGROUND	32
				/* Identical to the combination:
				   (IDL_INIT_NOCMDLINE|IDL_INIT_NOTTYEDIT) */

#define IDL_INIT_QUIET		64   /* Suppresses the startup announcement and
					message of the day. */

#define IDL_INIT_STUDENT	128   /* IDL Student Edition */

#define IDL_INIT_DEMO		(1 << 9)   /* Force IDL into demo mode */


#define IDL_INIT_LMQUEUE	(1 << 10)
				/* For FLEXlm licensing only, cause license
				   manager to queue for a license if floating
				   licenses exist but are currently not
				   available. */

#define IDL_INIT_GENVER		(1 << 11)
				/* Request genver licensing. Unix-only */

#define IDL_INIT_NOCMDLINE	(1 << 12)
				/* IDL should assume that it has no command
				   line. In this mode, any attempt to read
				   from stdin should be treated as if EOF
				   was returned. */

#define IDL_INIT_OCX		IDL_INIT_NOCMDLINE
				/* An old name for IDL_INITNOCMDLINE */

#define IDL_INIT_VM		(1 << 13)
				/* Initialize in virtual machine mode.
				   This is add-on behavior to runtime mode
				   but unlike embedded, this flag does not
				   include the runtime behavior.  The
				   runtime flag must be set as well to
				   initiate virtual machine behavior. */


#define IDL_INIT_NOVM		(1 << 14)
				/* Prevent going into vm mode if a license
				   is not available. */


#define IDL_INIT_NOTTYEDIT	(1 << 15)
				/* Prevents IDL from using termcap/terminfo to 
				   handle command line editing, and disables
				   the use of the SIGWINCH signal to track
				   the current tty size. This is usually what
				   you want when using callable IDL in a
				   background process that isn't doing I/O
				   to the tty. In such cases, the termcap
				   initialization can cause the process
				   to block because of job control from the
				   shell with a message like "Stopped (tty
				   output) idl". Setting this option prevents
				   all tty edit functions. I/O to the tty is
				   done with a simple fgets()/printf().
				   In the case of IDL_INIT_GUI, this is
				   ignored. */



#define IDL_INIT_CLARGS		(1 << 17)
				/* The clargs field contains valid command line
				   (argc,argv) arguments */

#define IDL_INIT_HWND		(1 << 18)
				/* (MS Windows only) The hwnd field contains
				   a valid HWND to be used as the application
				   main window */

#define IDL_INIT_NODEMOWARN	(1 << 21)
								/* If set, do not display demo warning when
								   entering demo mode. */
#define IDL_INIT_APPLIC		(1 << 27)
				/* The applic field contains application
				 license strings. */

/* Definition of data type for init_data argument to IDL_Initialize() */
typedef int IDL_INIT_DATA_OPTIONS_T;

typedef struct {
  /*
   * The options field specifies initialization options.
   * Most of these option bits are boolean (on/off), but some require
   * require additional information to be specified using one
   * of the other fields in this structure, as documented in the comments
   * that accompanies the option value definitions above and the fields below.
   *
   * IDL_Initialize() always examines the value of the options field of
   * this structure. It will only examine the other fields if a value in
   * options requires it to. Otherwise, those other fields are not used and
   * may safely contain any value (i.e. can be uninitialized).
   *
   * This organization allows us to add additional initialization options
   * to newer versions of IDL without requiring source code changes to
   * older applications that do not require those new features.
   */

  IDL_INIT_DATA_OPTIONS_T options;  /* 0 or any combination of IDL_INIT_ flags */

  /* clargs requires  the IDL_INIT_CLARGS options bit to be set */
  struct {			/* Command line arguments, as passed to main() */
    int argc;			/* Number of arguments */
    char **argv;		/* The arguments. These must be readonly
				   strings that are safe to access for the
				   entire life of this process. */
  } clargs;

#ifdef MSWIN
  /* hwnd requires  the IDL_INIT_HWND options bit to be set */
  void *hwnd;			/* HWND of application main window */
#endif

  char **applic;		/* Used if (options & IDL_INIT_APPLIC) */

} IDL_INIT_DATA;




#endif				/* main_IDL_DEF */




/***** Definitions from w_main *****/

#define IDL_VAL_DEMODLG_BITMAP 37




/***** Definitions from widgets *****/

#ifndef widgets_IDL_DEF
#define widgets_IDL_DEF

typedef void (* IDL_WIDGET_STUB_SET_SIZE_FUNC)
     (IDL_ULONG id, int width, int height);

#endif				/* widgets_DEF */




/***** Definitions from zfiles *****/

#ifndef zfiles_IDL_DEF
#define zfiles_IDL_DEF
/**** Access field bits in IDL_FILE_DESC and IDL_FILE_STAT ****/
#define IDL_OPEN_R          1	/* Open file for reading */
#define IDL_OPEN_W          2	/* Open file for writing */
#define IDL_OPEN_NEW        4	/* Unix - Truncate old file contents.
                                   Other - Use a new file. */
#define IDL_OPEN_APND       8	/* File open with pointer at EOF */

/**** Flags field bits in IDL_FILE_DESC and IDL_FILE_STAT ****/
#define IDL_F_ISATTY        ((IDL_SFILE_FLAGS_T) 1)   /* Is a terminal */
				/* Is a Graphical User Interface */
#define IDL_F_ISAGUI	    ((IDL_SFILE_FLAGS_T) 2)
				/* Don't let user close */
#define IDL_F_NOCLOSE       ((IDL_SFILE_FLAGS_T) 4)
				/* Use more(1) like pager for fmt output */
#define IDL_F_MORE          ((IDL_SFILE_FLAGS_T) 8)
#define IDL_F_XDR           ((IDL_SFILE_FLAGS_T) 16)   /* Is a XDR file */
#define IDL_F_DEL_ON_CLOSE  ((IDL_SFILE_FLAGS_T) 32)   /* Delete on close */
				/* Is a SAVE/RESTORE file. */
#define IDL_F_SR            ((IDL_SFILE_FLAGS_T) 64)
				/* File has opposite byte order than
				   current system. */
#define IDL_F_SWAP_ENDIAN   ((IDL_SFILE_FLAGS_T) 128)
				/* Binary float and double are in VAX
				   F and D format. Implies that file is
				   little endian. */
#define IDL_F_VAX_FLOAT	    (((IDL_SFILE_FLAGS_T) 1) << 8)
				/* If a normal file, file is in compressed
				   gzip format. If IDL_F_SR, then file
				   contains zlib compressed data. */
#define IDL_F_COMPRESS	    (((IDL_SFILE_FLAGS_T) 1) << 9)
				/* Unformatted f77(1) I/O */
#define IDL_F_UNIX_F77      (((IDL_SFILE_FLAGS_T) 1) << 10)
				/* File is a pipe to a child process */
#define IDL_F_PIPE	    (((IDL_SFILE_FLAGS_T) 1) << 11)
#define IDL_F_UNIX_PIPE     IDL_F_PIPE
				/* Call read(2) and write(2) directly
				   and allow short transfers. */
#define IDL_F_UNIX_RAWIO    (((IDL_SFILE_FLAGS_T) 1) << 12)
#define IDL_F_UNIX_NOSTDIO  IDL_F_UNIX_RAWIO   /* Old name for rawio */
				/* It's a device/special file */
#define IDL_F_UNIX_SPECIAL  (((IDL_SFILE_FLAGS_T) 1) << 13)
				/* Uses stdio instead of native I/O */
#define IDL_F_STDIO          (((IDL_SFILE_FLAGS_T) 1) << 14)
				/* File is an internet TCP/IP socket */
#define IDL_F_SOCKET	     (((IDL_SFILE_FLAGS_T) 1) << 15)



/*
 * With IDL 5.5, IDL no longer supports VMS. These VMS-only options
 * are no longer necessary, and are defined as 0 here for the benefit of
 * existing customer code.
 */
#define IDL_F_VMS_FIXED		((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_VARIABLE  	((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_SEGMENTED 	((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_STREAM    	((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_STREAM_STRICT	((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_RMSBLK	((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_RMSBLKUDF	((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_INDEXED	((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_PRINT		((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_SUBMIT	((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_TRCLOSE	((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_CCLIST	((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_CCFORTRAN	((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_CCNONE	((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_SHARED	((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_VMS_SUPERSEDE	((IDL_SFILE_FLAGS_T) 0)

/*
 * These options are no longer necessary. With IDL 5.4, IDL no longer
 * requires changing between text and binary mode on files under
 * Microsoft Windows. They are defined as 0 here for the benefit of
 * existing customer code.
 */
#define IDL_F_DOS_NOAUTOMODE ((IDL_SFILE_FLAGS_T) 0)
#define IDL_F_DOS_BINARY     ((IDL_SFILE_FLAGS_T) 0)

/*
 * IDL 5.5 was the last version of IDL for Clasic Mac OS. Mac OS X
 * is a Unix operating system, and does not have a binary/text distinction
 * for files.
 */
#define IDL_F_MAC_BINARY     ((IDL_SFILE_FLAGS_T) 0)


  
/* Sets the IDL_F_NOCLOSE bit for file unit. */
#define IDL_FILE_NOCLOSE(unit) IDL_FileSetClose((unit), FALSE)

/* Clear the IDL_F_NOCLOSE bit for file unit. */
#define IDL_FILE_CLOSE(unit) IDL_FileSetClose((unit), TRUE)

/**** File units that map to standard units ****/
#define IDL_STDIN_UNIT      0
#define IDL_STDOUT_UNIT     -1
#define IDL_STDERR_UNIT     -2
#define IDL_NON_UNIT        -100    /* Guaranteed to be an invalid unit */

/* Valid flags to bit-OR together for IDL_FileEnsureStatus() flags argument */
#define IDL_EFS_USER        1       /* Must be user unit (1 - MAX_USER_FILES) */
#define IDL_EFS_OPEN        2       /* Unit must be open */
#define IDL_EFS_CLOSED      4       /* Unit must be closed */
#define IDL_EFS_READ        8       /* Unit must be open for input */
#define IDL_EFS_WRITE       16      /* Unit must be open for output */
#define IDL_EFS_NOTTY       32      /* Unit cannot be a tty */
#define IDL_EFS_NOGUI       64      /* Unit cannot be a tty */
#define IDL_EFS_NOPIPE      128     /* Unit cannot be a pipe */
#define IDL_EFS_NOXDR       (1 << 8)   /* Unit cannot be a XDR file */
#define IDL_EFS_ASSOC       (1 << 9)   /* Unit can be assoc'd. This implies USER,
					  OPEN, NOTTY, NOPIPE, NOXDR, NOCOMPRESS,
					  and NOSOCKET, in addition to other
					  OS specific concerns */
#define IDL_EFS_NOT_RAWIO   (1 << 10)	/* Under Unix, file wasn't opened with
					   IDL_F_UNIX_RAWIO attribute. */
#define IDL_EFS_NOT_NOSTDIO IDL_EFS_NOT_RAWIO	/* Old name for RAWIO */
#define IDL_EFS_NOCOMPRESS  (1 << 11)    /* File cannot be a GZIP file */
#define IDL_EFS_STDIO	    (1 << 12)    /* File must be IDL_F_STDIO */
#define IDL_EFS_NOSOCKET    (1 << 13)    /* Unit cannot be a socket */
#define IDL_EFS_SOCKET_LISTEN (1 << 14)	  /* Unit must be a listener socket */
     

/**** Struct for global variable term, filled by IDL_FileInit() ****/
typedef struct {
#ifdef IDL_OS_HAS_TTYS
  char *name;                   /* Name of terminal type */
  char is_tty;                  /* True if stdin is a terminal */
#endif
  int lines;                    /* Lines on screen */
  int columns;                  /* Width of output */
} IDL_TERMINFO;


/**** Struct that is filled in by IDL_FileStat() ****/
typedef struct {
  char *name;
  short access;
  IDL_SFILE_FLAGS_T flags;
  FILE *fptr;
} IDL_FILE_STAT;

#define IDL_FCU_FREELUN		2   /* If LUN has had a GET_LUN operation to
				       reserve the unit, release it. */
#endif				/* zfiles_IDL_DEF */




/***** Definitions from zstring *****/

#ifndef zstring_IDL_DEF
#define zstring_IDL_DEF


/*
 * To simplify IDL's code and promote uniformity, we standardize on
 * some basic functions for memory and string operations. This module
 * defines macros and supplies missing routines so that this standard
 * base is available on all supported platforms.
 *
 *   [] For raw memory copying and zeroing operations, IDL uses the
 *      BSD b*() functions bcmp(), bcopy(), and bzero(). If these don't
 *	exist, macros map the names to the ATT SysV mem*() equivalents.
 *
 *   [] For basic string copying and concatenation, we use strlcpy() and
 *      strlcat(). These are newer routines that are not available on most
 *      platforms (OpenBSD and Solaris at this date), but which are safer,
 *	easier to use correctly, and in many cases more efficient than
 *	strcpy(), strcat(), and especially strncpy() which is an especially
 *	bad API and which should be avoided whenever possible. We provide
 *	strlcat() and strlcpy() for platforms that lack them.
 *
 *   [] For basic string copying of known fixed lengths, we supply
 *	a function of our own invention called strbcopy(). This is
 *	a variation on strlcpy() that is much faster, but gives a coarser
 *	boolean result. It also does not require the input string
 *	to be NULL terminated.
 *
 *   [] For case insensitive string comparisons, we use strcasecmp()
 *      and strncasecmp(). We provide them on platforms that don't have
 *	them in their libraries.
 */


#include <string.h>		/* Prototypes for standard str*() functions */
#if (defined(unix) && !defined(hpux) && !defined(AIX_64))
#include <strings.h>		/* For bcopy(), bzero(), and bcmp() */
#include <wchar.h>		/* For wcscasecmp */
#endif
#include <wctype.h>
#if defined(sun)
#include <widec.h>              /* for wsncmp */
#endif



/*
 * On a platform by platform basis, make sure routines with the standard
 * names and behaviors exist uniformly. If the desired functionality exists
 * under a different name, use a macro to map to it. If the functionality
 * is missing, have IDL supply it.
 */


#if defined(hpux) || defined(MSWIN) || defined(AIX_64)
#ifndef bcopy
#define bcopy(src,dest,len)     (memcpy((dest), (src), (len)))
#endif
#ifndef bzero
#define bzero(dest,len)         (memset((dest), 0, (len)))
#endif
#ifndef bcmp
#define bcmp(b1,b2,len)         (memcmp((b1), (b2), (len)))
#endif
#endif				/* bcopy(), bzero(), and bcmp() */

/*
 * The IDL_BZERO macros encapsulate the most common idioms of bzero():
 *	IDL_BZERO - Zero scalar.
 *	IDL_BZERO_ARRAY - Zero array.
 *	IDL_BZERO_BYADDR - Zero scalar given a pointer (address) to it
 * All of these cases involve getting the address, casting it to (char *),
 * and calculating the size of the item. There is room for human error in
 * all of these stages, many of which will go undetected at compile time
 * (and even at runtime). The macros will always get these issues right, leaving
 * the programmer the sole task of choosing the right one to use.
 */
#define IDL_BZERO(zero_var) bzero((char *) &(zero_var), sizeof(zero_var))
#define IDL_BZERO_ARRAY(zero_arr) bzero((char *) (zero_arr), sizeof(zero_arr))
#define IDL_BZERO_BYADDR(zero_var_addr) \
  bzero((char *) (zero_var_addr), sizeof(*(zero_var_addr)))


/*
 * Solaris has these functions starting with Solaris 8. No one else
 * does yet. So, use our version everywhere for now.
 */
#define IDL_SUPPLIES_STRLCPY
#define IDL_SUPPLIES_STRLCAT

/*
 * strbcopy() is our own invention, so of course we supply the implemenation.
 */
#define IDL_SUPPLIES_STRBCOPY


#ifdef MSWIN			/* Different names for the same thing */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define wstrcasecmp _wcsicmp
#define wstrncasecmp _wcsnicmp

			/* They don't supply these without the underscore */
#define snprintf _snprintf
#define snwprintf _snwprintf
#define vsnprintf _vsnprintf
#else
#define snwprintf swprintf
#if defined(darwin)
#define wstrcasecmp CocoaWcsCaseCmp
#elif defined(sun)
#define wstrcasecmp wcscmp
#else
#define wstrcasecmp wcscasecmp
#endif
#endif

/*
 * strcasestr() is our own invention, so of course we supply the implemenation.
 */
#define IDL_SUPPLIES_STRCASESTR


#if defined(ALPHA_OSF)
/*
 * Thankfully, just about everyone supplies a real snprintf() now.
 */
#define IDL_SUPPLIES_SNPRINTF
#define IDL_SUPPLIES_VSNPRINTF
#endif





/*
 * If we're supplying a routine, use a macro to map the standard name
 * to our routine.
 */
#ifdef IDL_SUPPLIES_STRLCPY
#define strlcpy IDL_StrBase_strlcpy
#endif

#ifdef IDL_SUPPLIES_STRLCAT
#define strlcat IDL_StrBase_strlcat
#define strlcatW IDL_StrBase_strlcatW
#endif

#ifdef IDL_SUPPLIES_STRBCOPY
#define strbcopy IDL_StrBase_strbcopy
#define strbcopyW IDL_StrBase_strbcopyW
#endif

#ifdef IDL_SUPPLIES_STRCASECMP
#define strcasecmp IDL_StrBase_strcasecmp
#endif

#ifdef IDL_SUPPLIES_STRNCASECMP
#define strncasecmp IDL_StrBase_strncasecmp
#endif

#ifdef IDL_SUPPLIES_STRCASESTR
#define strcasestr IDL_StrBase_strcasestr
#endif

#ifdef IDL_SUPPLIES_SNPRINTF
#define snprintf IDL_StrBase_snprintf
#endif

#ifdef IDL_SUPPLIES_SNPRINTF
#define vsnprintf IDL_StrBase_vsnprintf
#endif


#endif				/* zstring_IDL_DEF */




/***** Definitions from ztimer *****/

#ifndef ztimer_IDL_DEF
#define ztimer_IDL_DEF

typedef void (* IDL_TIMER_CB)(void);
typedef IDL_TIMER_CB IDL_TIMER_CONTEXT;
typedef IDL_TIMER_CONTEXT *IDL_TIMER_CONTEXT_PTR;

#endif				/* ztimer_IDL_DEF */




/* Forward declarations for all exported routines and data */


extern void IDL_CDECL IDL_Win32MessageLoop(int fFlush);
extern int IDL_CDECL IDL_GetExitStatus();
extern int IDL_CDECL IDL_BailOut(int stop);
extern int IDL_CDECL IDL_Cleanup(int just_cleanup);
extern int IDL_CDECL IDL_Initialize(IDL_INIT_DATA *init_data);
extern int IDL_CDECL IDL_Init(IDL_INIT_DATA_OPTIONS_T options, int *argc,
        char *argv[]);
extern int IDL_CDECL IDL_Win32Init(IDL_INIT_DATA_OPTIONS_T iOpts, void
        *hinstExe, void *hwndExe, void *hAccel);
extern int IDL_CDECL IDL_Main(IDL_INIT_DATA_OPTIONS_T options, int argc,
        char *argv[]);
extern int IDL_CDECL IDL_ExecuteStr(char *cmd);
extern int IDL_CDECL IDL_Execute(int argc, char *argv[]);
extern int IDL_CDECL IDL_RuntimeExec(char *file);
extern void IDL_CDECL IDL_Runtime(IDL_INIT_DATA_OPTIONS_T options, int
        *argc, char *argv[], char *file);
extern char *IDL_OutputFormat[];
extern char *IDL_CDECL IDL_OutputFormatFunc(int type);
extern int IDL_OutputFormatLen[];
extern int IDL_CDECL IDL_OutputFormatLenFunc(int type);
extern char *IDL_OutputFormatFull[];
extern int IDL_OutputFormatFullLen[];
extern char *IDL_OutputFormatNatural[];
extern IDL_LONG IDL_TypeSize[];
extern int IDL_CDECL IDL_TypeSizeFunc(int type);
extern char *IDL_TypeName[];
extern char *IDL_CDECL IDL_TypeNameFunc(int type);
extern IDL_ALLTYPES IDL_zero;
extern IDL_VPTR IDL_CDECL IDL_nonavailable_rtn(int argc, IDL_VPTR argv[],
        char *argk);
extern IDL_TERMINFO IDL_FileTerm;
extern char *IDL_CDECL IDL_FileTermName(void);
extern int IDL_CDECL IDL_FileTermIsTty(void);
extern int IDL_CDECL IDL_FileTermLines(void);
extern int IDL_CDECL IDL_FileTermColumns(void);
extern int IDL_CDECL IDL_FileEnsureStatus(int action, int unit, int flags);
extern void IDL_CDECL IDL_FileSetMode(int unit, int binary);
extern int IDL_CDECL IDL_FileOpenUnitBasic(int unit, char *filename, int
        access_mode, IDL_SFILE_FLAGS_T flags, int msg_action, 
        IDL_MSG_ERRSTATE_PTR errstate);
extern int IDL_CDECL IDL_FileOpen(int argc, IDL_VPTR argv[], char *argk,
        int access_mode, IDL_SFILE_FLAGS_T extra_flags,  int longjmp_safe, int
        msg_attr);
extern int IDL_CDECL IDL_FileCloseUnit(int unit, int flags,
        IDL_SFILE_PIPE_EXIT_STATUS *exit_status,  int msg_action,
        IDL_MSG_ERRSTATE_PTR errstate);
extern void IDL_CDECL IDL_FileClose(int argc, IDL_VPTR argv[], char *argk);
extern void IDL_CDECL IDL_FileFlushUnit(int unit);
extern void IDL_CDECL IDL_FileGetUnit(int argc, IDL_VPTR argv[]);
extern void IDL_CDECL IDL_FileFreeUnit(int argc, IDL_VPTR argv[]);
extern int IDL_CDECL IDL_FileSetPtr(int unit, IDL_FILEINT pos, int extend,
        int msg_action);
extern int IDL_CDECL IDL_FileEOF(int unit);
extern void IDL_CDECL IDL_FileStat(int unit, IDL_FILE_STAT *stat_blk);
extern void IDL_CDECL IDL_FileSetClose(int unit, int allow);
extern IDL_VPTR IDL_CDECL IDL_FileVaxFloat(int argc, IDL_VPTR *argv, char
        *argk);
extern int IDL_CDECL IDL_SysRtnAdd(IDL_SYSFUN_DEF2 *defs, int is_function,
        int cnt);
extern IDL_MEMINT IDL_CDECL IDL_SysRtnNumEnabled(int is_function, int
        enabled);
extern void IDL_CDECL IDL_SysRtnGetEnabledNames(int is_function,
        IDL_STRING *str, int enabled);
extern void IDL_CDECL IDL_SysRtnEnable(int is_function, IDL_STRING *names,
        IDL_MEMINT n, int option,  IDL_SYSRTN_GENERIC disfcn);
extern IDL_SYSRTN_GENERIC IDL_CDECL IDL_SysRtnGetRealPtr(int is_function,
        char *name);
extern char *IDL_CDECL IDL_SysRtnGetCurrentName(void);
extern int IDL_CDECL IDL_LMGRLicenseInfo(int iFlags);
extern int IDL_CDECL IDL_LMGRSetLicenseInfo(int iFlags);
extern int IDL_CDECL IDL_LMGRLicenseCheckoutUnique(char *szFeature, char
        *szVersion);
extern int IDL_CDECL IDL_LMGRLicenseCheckout(char *szFeature, char
        *szVersion);
extern int IDL_CDECL IDL_AddSystemRoutine(IDL_SYSFUN_DEF *defs, int
        is_function, int cnt);
extern void IDL_CDECL IDL_StrDup(IDL_REGISTER IDL_STRING *str,
        IDL_REGISTER IDL_MEMINT n);
extern void IDL_CDECL IDL_StrDelete(IDL_STRING *str, IDL_MEMINT n);
extern void IDL_CDECL IDL_StrStore(IDL_STRING *s, const char *fs);
extern void IDL_CDECL IDL_StrEnsureLength(IDL_STRING *s, int n);
extern IDL_VPTR IDL_CDECL IDL_StrToSTRING(const char *s);
extern IDL_VPTR IDL_CDECL IDL_stregex(int argc, IDL_VPTR argv[], char
        *argk);
extern void IDL_CDECL IDL_VarGetData(IDL_VPTR v, IDL_MEMINT *n, char **pd,
        int ensure_simple);
extern IDL_STRING *IDL_CDECL IDL_VarGet1EltStringDesc(IDL_VPTR v, IDL_VPTR
        *tc_v, int like_print);
extern char *IDL_CDECL IDL_VarGetString(IDL_VPTR v);
extern IDL_VPTR IDL_CDECL IDL_ImportArray(int n_dim, IDL_MEMINT dim[], int
        type, UCHAR *data, IDL_ARRAY_FREE_CB free_cb,  IDL_StructDefPtr s);
extern IDL_VPTR IDL_CDECL IDL_ImportNamedArray(char *name, int n_dim,
        IDL_MEMINT dim[],  int type, UCHAR *data,  IDL_ARRAY_FREE_CB free_cb, 
        IDL_StructDefPtr s);
extern void IDL_CDECL IDL_Delvar(IDL_VPTR var);
extern IDL_VPTR IDL_CDECL IDL_VarTypeConvert(IDL_VPTR v, IDL_REGISTER int
        type);
extern void IDL_CDECL IDL_VarEnsureSimple(IDL_VPTR v);
extern char *IDL_CDECL IDL_VarMakeTempFromTemplate(IDL_VPTR template_var,
        int type, IDL_StructDefPtr sdef,  IDL_VPTR *result_addr, int zero);
extern char *IDL_DitherMethodNames[];
extern char *IDL_CDECL IDL_DitherMethodNamesFunc(int method);
extern void IDL_CDECL IDL_RasterDrawThick(IDL_GR_PT *p0, IDL_GR_PT *p1,
        IDL_ATTR_STRUCT *a,  IDL_DEVCORE_FCN_POLYFILL routine,  int dot_width);
extern void IDL_CDECL IDL_RasterPolyfill(int *x, int *y, int n,
        IDL_POLYFILL_ATTR *p, IDL_RASTER_DEF *r);
extern void IDL_CDECL IDL_RasterDraw(IDL_GR_PT *p0, IDL_GR_PT *p1,
        IDL_ATTR_STRUCT *a, IDL_RASTER_DEF *r);
extern void IDL_CDECL IDL_Raster8Image(UCHAR *data, IDL_ULONG nx,
        IDL_ULONG ny, IDL_ULONG x0, IDL_ULONG y0,  IDL_ULONG xsize, IDL_ULONG
        ysize,  IDL_TV_STRUCT *secondary,  IDL_RASTER_DEF *rs,  IDLBool_t
        bReverse);
extern void IDL_CDECL IDL_RasterImage(UCHAR *data, IDL_ULONG nx, IDL_ULONG
        ny, IDL_ULONG x0, IDL_ULONG y0,  IDL_ULONG xsize, IDL_ULONG ysize, 
        IDL_TV_STRUCT *secondary,  IDL_RASTER_DEF *rs,  IDLBool_t bReverse);
extern void IDL_CDECL IDL_Dither(UCHAR *data, int ncols, int nrows,
        IDL_RASTER_DEF *r, int x0, int y0, IDL_TV_STRUCT *secondary);
extern void IDL_CDECL IDL_BitmapLandscape(IDL_RASTER_DEF *in,
        IDL_RASTER_DEF *out, int y0);
extern IDL_VPTR IDL_CDECL IDL_transpose(int argc, IDL_VPTR *argv);
extern int IDL_CDECL IDL_KWProcessByOffset(int argc, IDL_VPTR *argv, char
        *argk, IDL_KW_PAR *kw_list, IDL_VPTR *plain_args,  int mask, void *base);
extern int IDL_CDECL IDL_KWProcessByAddr(int argc, IDL_VPTR *argv, char
        *argk, IDL_KW_PAR *kw_list, IDL_VPTR *plain_args,  int mask, int
        *free_required);
extern void IDL_CDECL IDL_KWFree(void);
extern void IDL_CDECL IDL_KWFreeAll(void);
extern IDL_VPTR IDL_CDECL IDL_CvtBytscl(int argc, IDL_VPTR *argv, char
        *argk);
extern void IDL_CDECL IDL_Freetmp(IDL_REGISTER IDL_VPTR p);
extern void IDL_CDECL IDL_Deltmp(IDL_REGISTER IDL_VPTR p);
extern IDL_VPTR IDL_CDECL IDL_Gettmp(void);
extern IDL_VPTR IDL_CDECL IDL_GettmpByte(UCHAR value);
extern IDL_VPTR IDL_CDECL IDL_GettmpInt(IDL_INT value);
extern IDL_VPTR IDL_CDECL IDL_GettmpLong(IDL_LONG value);
extern IDL_VPTR IDL_CDECL IDL_GettmpFloat(float value);
extern IDL_VPTR IDL_CDECL IDL_GettmpDouble(double value);
extern IDL_VPTR IDL_CDECL IDL_GettmpPtr(IDL_HVID value);
extern IDL_VPTR IDL_CDECL IDL_GettmpObjRef(IDL_HVID value);
extern IDL_VPTR IDL_CDECL IDL_GettmpUInt(IDL_UINT value);
extern IDL_VPTR IDL_CDECL IDL_GettmpULong(IDL_ULONG value);
extern IDL_VPTR IDL_CDECL IDL_GettmpLong64(IDL_LONG64 value);
extern IDL_VPTR IDL_CDECL IDL_GettmpULong64(IDL_ULONG64 value);
extern IDL_VPTR IDL_CDECL IDL_GettmpFILEINT(IDL_FILEINT value);
extern IDL_VPTR IDL_CDECL IDL_GettmpMEMINT(IDL_MEMINT value);
extern char *IDL_CDECL IDL_GetScratch(IDL_REGISTER IDL_VPTR *p,
        IDL_REGISTER IDL_MEMINT n_elts,  IDL_REGISTER IDL_MEMINT elt_size);
extern char *IDL_CDECL IDL_GetScratchOnThreshold(IDL_REGISTER char
        *auto_buf, IDL_REGISTER IDL_MEMINT auto_elts,  IDL_REGISTER IDL_MEMINT
        n_elts,  IDL_REGISTER IDL_MEMINT elt_size,  IDL_VPTR *tempvar);
extern void IDL_CDECL IDL_VarCopy(IDL_REGISTER IDL_VPTR src, IDL_REGISTER
        IDL_VPTR dst);
extern void IDL_CDECL IDL_StoreScalar(IDL_VPTR dest, int type,
        IDL_ALLTYPES *value);
extern void IDL_CDECL IDL_StoreScalarZero(IDL_VPTR dest, int type);
extern void IDL_CDECL IDL_unform_io(int type, int argc, IDL_VPTR *argv,
        char *argk);
extern void IDL_CDECL IDL_Print(int argc, IDL_VPTR *argv, char *argk);
extern void IDL_CDECL IDL_PrintF(int argc, IDL_VPTR *argv, char *argk);
extern IDL_VPTR IDL_CDECL IDL_String_Remove(int argc, IDL_VPTR *argv, char
        *argk);
extern IDL_VPTR IDL_CDECL IDL_Variable_Diff(int argc, IDL_VPTR *argv, char
        *argk);
extern IDL_VPTR IDL_CDECL IDL_BasicTypeConversion(int argc, IDL_VPTR
        argv[], IDL_REGISTER int type);
extern IDL_VPTR IDL_CDECL IDL_CvtByte(int argc, IDL_VPTR argv[]);
extern IDL_VPTR IDL_CDECL IDL_CvtFix(int argc, IDL_VPTR argv[]);
extern IDL_VPTR IDL_CDECL IDL_CvtLng(int argc, IDL_VPTR argv[]);
extern IDL_VPTR IDL_CDECL IDL_CvtFlt(int argc, IDL_VPTR argv[]);
extern IDL_VPTR IDL_CDECL IDL_CvtDbl(int argc, IDL_VPTR argv[]);
extern IDL_VPTR IDL_CDECL IDL_CvtUInt(int argc, IDL_VPTR argv[]);
extern IDL_VPTR IDL_CDECL IDL_CvtULng(int argc, IDL_VPTR argv[]);
extern IDL_VPTR IDL_CDECL IDL_CvtLng64(int argc, IDL_VPTR argv[]);
extern IDL_VPTR IDL_CDECL IDL_CvtULng64(int argc, IDL_VPTR argv[]);
extern IDL_VPTR IDL_CDECL IDL_CvtMEMINT(int argc, IDL_VPTR argv[]);
extern IDL_VPTR IDL_CDECL IDL_CvtFILEINT(int argc, IDL_VPTR argv[]);
extern IDL_VPTR IDL_CDECL IDL_CvtComplex(int argc, IDL_VPTR argv[], char
        *argk);
extern IDL_VPTR IDL_CDECL IDL_CvtDComplex(int argc, IDL_VPTR argv[]);
extern IDL_VPTR IDL_CDECL IDL_CvtString(int argc, IDL_VPTR argv[], char
        *argk);
extern IDL_LONG IDL_CDECL IDL_grMesh_Clip(float fPlane[4], short clipSide,
         float *pfVin, IDL_LONG iNVerts,  IDL_LONG *piCin, IDL_LONG iNConn, 
        float **pfVout, IDL_LONG *iNVout,  IDL_LONG **piCout, IDL_LONG *iNCout, 
        IDL_VPTR vpAuxInKW, IDL_VPTR vpAuxOutKW,  IDL_VPTR vpCut);
extern IDL_LONG IDL_CDECL IDL_LongScalar(IDL_REGISTER IDL_VPTR v);
extern IDL_ULONG IDL_CDECL IDL_ULongScalar(IDL_REGISTER IDL_VPTR v);
extern IDL_LONG64 IDL_CDECL IDL_Long64Scalar(IDL_REGISTER IDL_VPTR v);
extern IDL_ULONG64 IDL_CDECL IDL_ULong64Scalar(IDL_REGISTER IDL_VPTR v);
extern double IDL_CDECL IDL_DoubleScalar(IDL_REGISTER IDL_VPTR v);
extern IDL_MEMINT IDL_CDECL IDL_MEMINTScalar(IDL_REGISTER IDL_VPTR v);
extern IDL_FILEINT IDL_CDECL IDL_FILEINTScalar(IDL_REGISTER IDL_VPTR v);
extern float IDL_CDECL IDL_CastUL64_f(IDL_ULONG64 value);
extern double IDL_CDECL IDL_CastUL64_d(IDL_ULONG64 value);
extern IDL_ULONG IDL_CDECL IDL_CastFloat_UL(float value);
extern IDL_ULONG IDL_CDECL IDL_CastDouble_UL(double value);
extern int IDL_CDECL IDL_StrBase_strcasecmp(const char *str1, const char
        *str2);
extern int IDL_CDECL IDL_StrBase_strncasecmp(const char *str1, const char
        *str2, size_t nchars);
extern size_t IDL_CDECL IDL_StrBase_strlcpy(char *dst, const char *src,
        size_t siz);
extern size_t IDL_CDECL IDL_StrBase_strlcat(char *dst, const char *src,
        size_t siz);
extern size_t IDL_CDECL IDL_StrBase_strlcatW(wchar_t *dst, const wchar_t
        *src, size_t siz);
extern int IDL_CDECL IDL_StrBase_vsnprintf(char *s, size_t n, const char
        *format, va_list args);
extern int IDL_CDECL IDL_StrBase_snprintf(char *s, size_t n, const char
        *format, ...);
extern int IDL_CDECL IDL_StrBase_strbcopy(char *dst, const char *src,
        size_t siz);
extern int IDL_CDECL IDL_StrBase_strbcopyW(wchar_t *dst, const wchar_t
        *src, size_t siz);
extern void IDL_CDECL IDL_RgbToHsv(UCHAR *r, UCHAR *g, UCHAR *b,  float
        *h, float *s, float *v, int n);
extern void IDL_CDECL IDL_RgbToHls(UCHAR *r, UCHAR *g, UCHAR *b,  float
        *h, float *l, float *s, int n);
extern void IDL_CDECL IDL_HsvToRgb(float *h, float *s, float *v,  UCHAR
        *r, UCHAR *g, UCHAR *b, int n);
extern void IDL_CDECL IDL_HlsToRgb(float *h, float *l, float *s, UCHAR *r,
        UCHAR *g, UCHAR *b, int n);
extern int IDL_CDECL IDL_AddDevice( IDL_DEVICE_DEF *dev,  int msg_action);
extern char *IDL_CDECL IDL_MakeTempStruct(IDL_StructDefPtr sdef, int
        n_dim, IDL_MEMINT *dim, IDL_VPTR *var, int zero);
extern char *IDL_CDECL IDL_MakeTempStructVector(IDL_StructDefPtr sdef,
        IDL_MEMINT dim, IDL_VPTR *var, int zero);
extern IDL_StructDefPtr IDL_CDECL IDL_MakeStruct(char *name,
        IDL_STRUCT_TAG_DEF *tags);
extern IDL_MEMINT IDL_CDECL IDL_StructTagInfoByName(IDL_StructDefPtr sdef,
        char *name, int msg_action, IDL_VPTR *var);
extern IDL_MEMINT IDL_CDECL IDL_StructTagInfoByIndex(IDL_StructDefPtr
        sdef, int index, int msg_action, IDL_VPTR *var);
extern char *IDL_CDECL IDL_StructTagNameByIndex(IDL_StructDefPtr sdef, int
        index, int msg_action, char **struct_name);
extern int IDL_CDECL IDL_StructNumTags(IDL_StructDefPtr sdef);
extern int IDL_STDCALL IDL_InitOCX(void *pInit);
extern char *IDL_CDECL IDL_FilePathFromRoot(int flags, char *pathbuf, char
        *root, char *file, char *ext,  int nsubdir, char **subdir);
extern wchar_t *IDL_CDECL IDL_FilePathFromRootW(int flags, wchar_t
        *pathbuf, wchar_t *root, wchar_t *file, wchar_t *ext,  int nsubdir,
        wchar_t **subdir);
extern char *IDL_CDECL IDL_FilePathFromDist(int flags, char *pathbuf, char
        *file, char *ext, int nsubdir, char **subdir);
extern wchar_t *IDL_CDECL IDL_FilePathFromDistW(int flags, wchar_t
        *pathbuf, wchar_t *file, wchar_t *ext, int nsubdir, wchar_t **subdir);
extern char *IDL_CDECL IDL_FilePathFromDistBin(int flags, char *pathbuf,
        char *file, char *ext);
extern wchar_t *IDL_CDECL IDL_FilePathFromDistBinW(int flags, wchar_t
        *pathbuf, wchar_t *file, wchar_t *ext);
extern char *IDL_CDECL IDL_FilePathFromDistHelp(int flags, char *pathbuf,
        char *file, char *ext);
extern wchar_t *IDL_CDECL IDL_FilePathFromDistHelpW(int flags, wchar_t
        *pathbuf, wchar_t *file, wchar_t *ext);
extern void IDL_CDECL IDL_FilePathGetTmpDir(char *path);
extern void IDL_CDECL IDL_FilePathGetTmpDirW(wchar_t *path);
extern int IDL_CDECL IDL_FilePathExpand(char *path, int msg_action);
extern int IDL_CDECL IDL_FilePathExpandW(wchar_t *wcharPath, int
        msg_action);
extern IDL_VPTR IDL_CDECL IDL_FilePathSearch(int argc, IDL_VPTR *argv,
        char *argk);
extern IDL_VPTR IDL_CDECL IDL_FilePathSearch(int argc, IDL_VPTR *argv,
        char *argk);
extern void IDL_CDECL IDL_TerminalRaw(int to_from, int fnin);
extern void IDL_CDECL IDL_Pout(IDL_POUT_CNTRL *control, int flags, char
        *fmt, ...);
extern void IDL_CDECL IDL_PoutVa(IDL_POUT_CNTRL *control, int flags, char
        *fmt, va_list *args);
extern void IDL_CDECL IDL_PoutRaw(int unit, char *buf, int n);
extern void IDL_CDECL IDL_EzCall(int argc, IDL_VPTR argv[], IDL_EZ_ARG
        arg_struct[]);
extern void IDL_CDECL IDL_EzCallCleanup(int argc, IDL_VPTR argv[],
        IDL_EZ_ARG arg_struct[]);
extern void IDL_CDECL IDL_EzReplaceWithTranspose(IDL_VPTR *v, IDL_VPTR
        orig);
extern char *IDL_CDECL IDL_MakeTempArray(int type, int n_dim, IDL_MEMINT
        dim[], int init, IDL_VPTR *var);
extern char *IDL_CDECL IDL_MakeTempVector(int type, IDL_MEMINT dim, int
        init, IDL_VPTR *var);
extern void IDL_CDECL IDL_TimerSet(IDL_LONG length, IDL_TIMER_CB callback,
        int from_callback, IDL_TIMER_CONTEXT_PTR context);
extern void IDL_CDECL IDL_TimerCancel(IDL_TIMER_CONTEXT context);
extern void IDL_CDECL IDL_TimerBlock(int stop);
extern void IDL_CDECL IDL_ToutPush(IDL_TOUT_OUTF outf);
extern IDL_TOUT_OUTF IDL_CDECL IDL_ToutPop(void);
extern void IDL_CDECL IDL_CvtVAXToFloat(float *fp, IDL_MEMINT n);
extern void IDL_CDECL IDL_CvtFloatToVAX(float *fp, IDL_MEMINT n);
extern void IDL_CDECL IDL_CvtVAXToDouble(double *dp, IDL_MEMINT n);
extern void IDL_CDECL IDL_CvtDoubleToVAX(double *dp, IDL_MEMINT n);
extern void IDL_CDECL IDL_ExitRegister(IDL_EXIT_HANDLER_FUNC proc);
extern void IDL_CDECL IDL_ExitUnregister(IDL_EXIT_HANDLER_FUNC proc);
extern void IDL_CDECL IDL_WidgetIssueStubEvent(char *rec, IDL_LONG value);
extern void IDL_CDECL IDL_WidgetSetStubIds(char *rec, unsigned long t_id,
        unsigned long b_id);
extern void IDL_CDECL IDL_WidgetGetStubIds(char *rec, unsigned long *t_id,
        unsigned long *b_id);
extern void IDL_CDECL IDL_WidgetStubLock(int set);
extern void *IDL_CDECL IDL_WidgetStubGetParent(IDL_ULONG id, char
        *szDisplay);
extern char *IDL_CDECL IDL_WidgetStubLookup(IDL_ULONG id);
extern void IDL_CDECL IDL_WidgetStubSetSizeFunc(char *rec,
        IDL_WIDGET_STUB_SET_SIZE_FUNC func);
extern void IDL_CDECL IDL_Wait(int argc, IDL_VPTR argv[]);
extern void IDL_CDECL IDL_GetUserInfo(IDL_USER_INFO *user_info);
extern int IDL_CDECL IDL_GetKbrd(int should_wait);
extern void IDL_CDECL IDL_TTYReset(void);
extern IDL_PLOT_COM IDL_PlotCom;
extern UCHAR IDL_ColorMap[];
extern IDL_PLOT_COM *IDL_CDECL IDL_PlotComAddr(void);
extern UCHAR *IDL_CDECL IDL_ColorMapAddr(void);
extern void IDL_CDECL IDL_PolyfillSoftware(int *x, int *y, int n,
        IDL_POLYFILL_ATTR *s);
extern double IDL_CDECL IDL_GraphText(IDL_GR_PT *p, IDL_ATTR_STRUCT *ga,
        IDL_TEXT_STRUCT *a, char *text);
extern char *IDL_CDECL IDL_VarName(IDL_VPTR v);
extern IDL_VPTR IDL_CDECL IDL_GetVarAddr1(char *name, int ienter);
extern IDL_VPTR IDL_CDECL IDL_GetVarAddr(char *name);
extern IDL_VPTR IDL_CDECL IDL_FindNamedVariable(char *name, int ienter);
extern IDL_HEAP_VPTR IDL_CDECL IDL_HeapVarHashFind(IDL_HVID hash_id);
extern IDL_HEAP_VPTR IDL_CDECL IDL_HeapVarNew(int hvid_type, IDL_VPTR
        value, int flags, int msg_action);
extern void IDL_CDECL IDL_HeapIncrRefCount(IDL_HVID *hvid, IDL_MEMINT n);
extern char *IDL_CDECL IDL_Rline(char *s, IDL_MEMINT n, int unit, FILE
        *stream, int is_tty, char *prompt, int opt);
extern void IDL_CDECL IDL_RlineSetStdinOptions(int opt);
extern void IDL_CDECL IDL_Logit(char *s);
extern IDL_SYS_VERSION IDL_SysvVersion;
extern IDL_STRING *IDL_CDECL IDL_SysvVersionArch(void);
extern IDL_STRING *IDL_CDECL IDL_SysvVersionOS(void);
extern IDL_STRING *IDL_CDECL IDL_SysvVersionOSFamily(void);
extern IDL_STRING *IDL_CDECL IDL_SysvVersionRelease(void);
extern IDL_CPU_STRUCT IDL_SysvCpu;
extern char *IDL_ProgramName;
extern char *IDL_CDECL IDL_ProgramNameFunc(void);
extern char *IDL_ProgramNameLC;
extern char *IDL_CDECL IDL_ProgramNameLCFunc(void);
extern IDL_STRING IDL_SysvDir;
extern IDL_STRING *IDL_CDECL IDL_SysvDirFunc(void);
extern IDL_LONG IDL_SysvErrCode;
extern IDL_LONG IDL_CDECL IDL_SysvErrCodeValue(void);
extern IDL_SYS_ERROR_STATE IDL_SysvErrorState;
extern IDL_SYS_ERROR_STATE *IDL_CDECL IDL_SysvErrorStateAddr(void);
extern IDL_STRING *IDL_CDECL IDL_SysvErrStringFunc(void);
extern IDL_STRING *IDL_CDECL IDL_SysvSyserrStringFunc(void);
extern IDL_LONG IDL_CDECL IDL_SysvErrorCodeValue(void);
extern IDL_LONG *IDL_CDECL IDL_SysvSyserrorCodesAddr(void);
extern IDL_LONG IDL_SysvOrder;
extern IDL_LONG IDL_CDECL IDL_SysvOrderValue(void);
extern float IDL_CDECL IDL_SysvValuesGetFloat(int type);
extern double IDL_CDECL IDL_SysvValuesGetDouble(int type);
extern int IDL_CDECL IDL_MessageNameToCode(IDL_MSG_BLOCK block, const char
        *name);
extern IDL_MSG_BLOCK IDL_CDECL IDL_MessageDefineBlock(char *block_name,
        int n, IDL_MSG_DEF *defs);
extern void IDL_CDECL IDL_MessageErrno(int code, ...);
extern void IDL_CDECL IDL_MessageErrnoFromBlock(IDL_MSG_BLOCK block, int
        code, ...);
extern void IDL_CDECL IDL_Message(int code, int action, ...);
extern void IDL_CDECL IDL_MessageFromBlock(IDL_MSG_BLOCK block, int code,
        int action,...);
extern void IDL_CDECL IDL_MessageSyscode(int code, IDL_MSG_SYSCODE_T
        syscode_type, int syscode, int action, ...);
extern void IDL_CDECL IDL_MessageSyscodeFromBlock(IDL_MSG_BLOCK block, int
        code, IDL_MSG_SYSCODE_T syscode_type,  int syscode, int action, ...);
extern void IDL_CDECL IDL_MessageVarError(int code, IDL_VPTR var, int
        action);
extern void IDL_CDECL IDL_MessageVarErrorFromBlock(IDL_MSG_BLOCK block,
        int code, IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageResetSysvErrorState(void);
extern void IDL_CDECL IDL_MessageSJE(void *value);
extern void *IDL_CDECL IDL_MessageGJE(void);
extern void IDL_CDECL IDL_MessageVE_UNDEFVAR(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_NOTARRAY(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_NOTSCALAR(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_NOEXPR(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_NOCONST(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_NOFILE(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_NOCOMPLEX(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_NOSTRING(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_NOSTRUCT(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_REQSTR(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_NOSCALAR(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_NOMEMINT64(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_STRUC_REQ(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_REQPTR(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_MessageVE_REQOBJREF(IDL_VPTR var, int action);
extern void IDL_CDECL IDL_Message_BADARRDNUM(int action);
extern void IDL_CDECL IDL_SignalSetInit(IDL_SignalSet_t *set, int signo);
extern void IDL_CDECL IDL_SignalSetAdd(IDL_SignalSet_t *set, int signo);
extern void IDL_CDECL IDL_SignalSetDel(IDL_SignalSet_t *set, int signo);
extern int IDL_CDECL IDL_SignalSetIsMember(IDL_SignalSet_t *set, int
        signo);
extern void IDL_CDECL IDL_SignalMaskGet(IDL_SignalSet_t *set);
extern void IDL_CDECL IDL_SignalMaskSet(IDL_SignalSet_t *set,
        IDL_SignalSet_t *oset);
extern void IDL_CDECL IDL_SignalMaskBlock(IDL_SignalSet_t *set,
        IDL_SignalSet_t *oset);
extern void IDL_CDECL IDL_SignalBlock(int signo, IDL_SignalSet_t *oset);
extern void IDL_CDECL IDL_SignalSuspend(IDL_SignalSet_t *set);
extern int IDL_CDECL IDL_SignalRegister(int signo, IDL_SignalHandler_t
        func, int msg_action);
extern int IDL_CDECL IDL_SignalUnregister(int signo, IDL_SignalHandler_t
        func, int msg_action);
extern int IDL_CDECL IDL_SetValue(int id, void* pvValue);
extern void IDL_CDECL IDL_WinPostInit(void);
extern void IDL_CDECL IDL_WinCleanup(void);
extern void *IDL_CDECL IDL_MemAlloc(IDL_MEMINT n, const char *err_str, int
        msg_action);
extern void *IDL_CDECL IDL_MemRealloc(void *ptr, IDL_MEMINT n, const char
        *err_str, int action);
extern void IDL_CDECL IDL_MemFree(IDL_REGISTER void *m, const char
        *err_str, int msg_action);
extern void *IDL_CDECL IDL_MemAllocPerm(IDL_MEMINT n, const char *err_str,
        int action);


#ifdef __cplusplus
    }
#endif

#endif                               /* export_IDL_DEF */
