/* @(#)xdr.h	2.2 88/07/29 4.0 RPCSRC */
/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 * 
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 * 
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 * 
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 * 
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 * 
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */
/*      @(#)xdr.h 1.19 87/04/22 SMI      */

/*
 * xdr.h, External Data Representation Serialization Routines.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

#ifndef __XDR_HEADER__
#define __XDR_HEADER__

/*
 * XDR provides a conventional way for converting between C data
 * types and an external bit-string representation.  Library supplied
 * routines provide for the conversion on built-in C data types.  These
 * routines and utility routines defined here are used to help implement
 * a type encode/decode routine for each user-defined type.
 *
 * Each data type provides a single procedure which takes two arguments:
 *
 *	bool_t
 *	xdrproc(xdrs, argresp)
 *		XDR *xdrs;
 *		<type> *argresp;
 *
 * xdrs is an instance of a XDR handle, to which or from which the data
 * type is to be converted.  argresp is a pointer to the structure to be
 * converted.  The XDR handle contains an operation field which indicates
 * which of the operations (ENCODE, DECODE * or FREE) is to be performed.
 *
 * XDR_DECODE may allocate space if the pointer argresp is null.  This
 * data can be freed with the XDR_FREE operation.
 *
 * We write only one procedure per data type to make it easy
 * to keep the encode and decode procedures for a data type consistent.
 * In many cases the same code performs all operations on a user defined type,
 * because all the hard work is done in the component type routines.
 * decode as a series of calls on the nested data types.
 */

/*
 * Xdr operations.  XDR_ENCODE causes the type to be encoded into the
 * stream.  XDR_DECODE causes the type to be extracted from the stream.
 * XDR_FREE can be used to release the space allocated by an XDR_DECODE
 * request.
 */
enum xdr_op {
	XDR_ENCODE=0,
	XDR_DECODE=1,
	XDR_FREE=2
};

/*
 * This is the number of bytes per unit of external data.
 */
#define BYTES_PER_XDR_UNIT	(4)
#define RNDUP(x)  ((((x) + BYTES_PER_XDR_UNIT - 1) / BYTES_PER_XDR_UNIT) \
		    * BYTES_PER_XDR_UNIT)

/*
 * A xdrproc_t exists for each data type which is to be encoded or decoded.
 *
 * The second argument to the xdrproc_t is a pointer to an opaque pointer.
 * The opaque pointer generally points to a structure of the data type
 * to be decoded.  If this pointer is 0, then the type routines should
 * allocate dynamic storage of the appropriate size and return it.
 * bool_t	(*xdrproc_t)(XDR *, caddr_t *);
 */
typedef	bool_t (*xdrproc_t)();

/*
 * The XDR handle.
 * Contains operation which is being applied to the stream,
 * an operations vector for the paticular implementation (e.g. see xdr_mem.c),
 * and two private fields for the use of the particular impelementation.
 */
typedef struct  __rpc_xdr {
	enum xdr_op	x_op;		/* operation; fast additional param */
	struct xdr_ops {
               /* get a long from underlying stream */
                bool_t  (*x_getlong)(struct __rpc_xdr *, long *);
                /* put a long to " */
                bool_t  (*x_putlong)(struct __rpc_xdr *, const long *);
                /* get some bytes from " */
                bool_t  (*x_getbytes)(struct __rpc_xdr *, char *, u_int);
                /* put some bytes to " */
                bool_t  (*x_putbytes)(struct __rpc_xdr *, const char *, u_int);
                /* returns bytes off from beginning */
                u_int   (*x_getpostn)(struct __rpc_xdr *);
                /* lets you reposition the stream */
                bool_t  (*x_setpostn)(struct __rpc_xdr *, u_int);
                /* buf quick ptr to buffered data */
                int32_t *(*x_inline)(struct __rpc_xdr *, u_int);
                /* free privates of this xdr_stream */
                void    (*x_destroy)(struct __rpc_xdr *);
                bool_t  (*x_control)(struct __rpc_xdr *, int, void *);
	} *x_ops;
	caddr_t 	x_public;	/* users' data */
	caddr_t		x_private;	/* pointer to private data */
	caddr_t 	x_base;		/* private used for position info */
	int		x_handy;	/* extra private word */
} XDR;

/*
 * Operations defined on a XDR handle
 *
 * XDR		*xdrs;
 * long		*longp;
 * caddr_t	 addr;
 * u_int	 len;
 * u_int	 pos;
 */
#define XDR_GETLONG(xdrs, longp)			\
	(*(xdrs)->x_ops->x_getlong)(xdrs, longp)
#define xdr_getlong(xdrs, longp)			\
	(*(xdrs)->x_ops->x_getlong)(xdrs, longp)

#define XDR_PUTLONG(xdrs, longp)			\
	(*(xdrs)->x_ops->x_putlong)(xdrs, longp)
#define xdr_putlong(xdrs, longp)			\
	(*(xdrs)->x_ops->x_putlong)(xdrs, longp)

#define XDR_GETBYTES(xdrs, addr, len)			\
	(*(xdrs)->x_ops->x_getbytes)(xdrs, addr, len)
#define xdr_getbytes(xdrs, addr, len)			\
	(*(xdrs)->x_ops->x_getbytes)(xdrs, addr, len)

#define XDR_PUTBYTES(xdrs, addr, len)			\
	(*(xdrs)->x_ops->x_putbytes)(xdrs, addr, len)
#define xdr_putbytes(xdrs, addr, len)			\
	(*(xdrs)->x_ops->x_putbytes)(xdrs, addr, len)

#define XDR_GETPOS(xdrs)				\
	(*(xdrs)->x_ops->x_getpostn)(xdrs)
#define xdr_getpos(xdrs)				\
	(*(xdrs)->x_ops->x_getpostn)(xdrs)

#define XDR_SETPOS(xdrs, pos)				\
	(*(xdrs)->x_ops->x_setpostn)(xdrs, pos)
#define xdr_setpos(xdrs, pos)				\
	(*(xdrs)->x_ops->x_setpostn)(xdrs, pos)

#define	XDR_INLINE(xdrs, len)				\
	(void *)((*(xdrs)->x_ops->x_inline)(xdrs, len))
#define	xdr_inline(xdrs, len)				\
	(*(xdrs)->x_ops->x_inline)(xdrs, len)

#define	XDR_DESTROY(xdrs)				\
	if ((xdrs)->x_ops->x_destroy) 			\
		(*(xdrs)->x_ops->x_destroy)(xdrs)
#define	xdr_destroy(xdrs)				\
	if ((xdrs)->x_ops->x_destroy) 			\
		(*(xdrs)->x_ops->x_destroy)(xdrs)

/*
 * Support struct for discriminated unions.
 * You create an array of xdrdiscrim structures, terminated with
 * a entry with a null procedure pointer.  The xdr_union routine gets
 * the discriminant value and then searches the array of structures
 * for a matching value.  If a match is found the associated xdr routine
 * is called to handle that part of the union.  If there is
 * no match, then a default routine may be called.
 * If there is no match and no default routine it is an error.
 */
#define NULL_xdrproc_t ((xdrproc_t)0)
struct xdr_discrim {
	int	value;
	xdrproc_t proc;
};

/*
 * In-line routines for fast encode/decode of primitive data types.
 * Caveat emptor: these use single memory cycles to get the
 * data from the underlying buffer, and will fail to operate
 * properly if the data is not aligned.  The standard way to use these
 * is to say:
 *	if ((buf = XDR_INLINE(xdrs, count)) == NULL)
 *		return (FALSE);
 *	<<< macro calls >>>
 * where ``count'' is the number of bytes of data occupied
 * by the primitive data types.
 *
 * N.B. and frozen for all time: each data type here uses 4 bytes
 * of external representation.
 */
#define IXDR_GET_LONG(buf)		((long)ntohl((u_long)*(buf)++))
#define IXDR_PUT_LONG(buf, v)		(*(buf)++ = (long)htonl((u_long)v))

#define IXDR_GET_BOOL(buf)		((bool_t)IXDR_GET_LONG(buf))
#define IXDR_GET_ENUM(buf, t)		((t)IXDR_GET_LONG(buf))
#define IXDR_GET_U_LONG(buf)		((u_long)IXDR_GET_LONG(buf))
#define IXDR_GET_SHORT(buf)		((short)IXDR_GET_LONG(buf))
#define IXDR_GET_U_SHORT(buf)		((u_short)IXDR_GET_LONG(buf))

#define IXDR_PUT_BOOL(buf, v)		IXDR_PUT_LONG((buf), ((long)(v)))
#define IXDR_PUT_ENUM(buf, v)		IXDR_PUT_LONG((buf), ((long)(v)))
#define IXDR_PUT_U_LONG(buf, v)		IXDR_PUT_LONG((buf), ((long)(v)))
#define IXDR_PUT_SHORT(buf, v)		IXDR_PUT_LONG((buf), ((long)(v)))
#define IXDR_PUT_U_SHORT(buf, v)	IXDR_PUT_LONG((buf), ((long)(v)))

/*
 * These are the "generic" xdr routines.
 */
#ifdef __cplusplus
extern "C" {
#endif
extern bool_t   xdr_void(void);
extern bool_t   xdr_int(XDR *, int *);
extern bool_t   xdr_u_int(XDR *, u_int *);
extern bool_t   xdr_long(XDR *, long *);
extern bool_t   xdr_u_long(XDR *, u_long *);
extern bool_t   xdr_short(XDR *, short *);
extern bool_t   xdr_u_short(XDR *, u_short *);
extern bool_t   xdr_bool(XDR *, bool_t *);
extern bool_t   xdr_enum(XDR *, enum_t *);
extern bool_t   xdr_array(XDR *, char **, u_int *, u_int, u_int, xdrproc_t);
extern bool_t   xdr_bytes(XDR *, char **, u_int *, u_int);
extern bool_t   xdr_opaque(XDR *, char *, u_int);
extern bool_t   xdr_string(XDR *, char **, u_int);
extern bool_t   xdr_union(XDR *, enum_t *, char *, const struct xdr_discrim *, xdrproc_t);
extern bool_t   xdr_char(XDR *, char *);
extern bool_t   xdr_u_char(XDR *, u_char *);
extern bool_t   xdr_vector(XDR *, char *, u_int, u_int, xdrproc_t);
extern bool_t   xdr_float(XDR *, float *);
extern bool_t   xdr_double(XDR *, double *);
extern bool_t   xdr_reference(XDR *, char **, u_int, xdrproc_t);
extern bool_t   xdr_pointer(XDR *, char **, u_int, xdrproc_t);
extern bool_t   xdr_wrapstring(XDR *, char **);
extern bool_t   xdr_uint64_t (XDR *xdrs, uint64_t *uip);
extern bool_t   xdr_int64_t (XDR *xdrs, int64_t *uip);
#ifdef __cplusplus
}
#endif

/*
 * Common opaque bytes objects used by many rpc protocols;
 * declared here due to commonality.
 */
#define MAX_NETOBJ_SZ 1024 
struct netobj {
	u_int	n_len;
	char	*n_bytes;
};
typedef struct netobj netobj;
extern bool_t   xdr_netobj();

/*
 * These are the public routines for the various implementations of
 * xdr streams.
 */
#ifdef __cplusplus
extern "C" {
#endif
/* XDR using memory buffers */
extern void   xdrmem_create(XDR *, char *, u_int, enum xdr_op);

/* XDR using stdio library */
extern void   xdrstdio_create(XDR *, FILE *, enum xdr_op);

/* XDR pseudo records for tcp */
extern void   xdrrec_create(XDR *, u_int, u_int, void *,
                            int (*)(void *, void *, int),
                            int (*)(void *, void *, int));

/* make end of xdr record */
extern bool_t xdrrec_endofrecord(XDR *, int);

/* move to beginning of next record */
extern bool_t xdrrec_skiprecord(XDR *);

/* true if no more input */
extern bool_t xdrrec_eof(XDR *);
#ifdef __cplusplus
}
#endif

/* RWMJ */
extern void xdr_free (xdrproc_t, char *);

#define xdr_u_quad_t xdr_uint64_t
#define xdr_quad_t xdr_int64_t

#define IXDR_PUT_INT32 IXDR_PUT_LONG
#define IXDR_GET_INT32 IXDR_GET_LONG

#define XDR_PUTINT32(xdrs, int32p)                      \
        (*(xdrs)->x_ops->x_putlong)(xdrs, int32p)
#define XDR_GETINT32(xdrs, int32p)                      \
        (*(xdrs)->x_ops->x_getlong)(xdrs, int32p)

#endif /* __XDR_HEADER__ */
