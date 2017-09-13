/* tripack.f -- translated by f2c (version 20100827).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#ifdef __cplusplus
extern "C" {
#endif
//#include "f2c.h"
typedef DDouble doublereal;
typedef DLong integer;
typedef DLong logical;

#define TRUE_ (1)
#define FALSE_ (0)

namespace tripack {

/* Common Block Declarations */

static struct {
    doublereal y;
} stcom_;

#define stcom_1 stcom_

static struct {
    doublereal swtol;
} swpcom_;

#define swpcom_1 swpcom_

/*      ALGORITHM 751, COLLECTED ALGORITHMS FROM ACM. */
/*      THIS WORK PUBLISHED IN TRANSACTIONS ON MATHEMATICAL SOFTWARE, */
/*      VOL. 22, NO. 1, March, 1996, P.  1--8. */
/*      ####### With remark from renka (to appear) 4/dec/1998 */

/*      modifications for R: */
/*        REAL -> DOUBLE PRECISION  albrecht.gebhardt@uni-klu.ac.at */

/* Subroutine */ int addcst_(integer *ncc, integer *lcc, integer *n, 
	doublereal *x, doublereal *y, integer *lwk, integer *iwk, integer *
	list, integer *lptr, integer *lend, integer *ier)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    integer i__, k, n1, n2, kn, lp, lw, lpb, lpf, lpl, lwd2;
    extern /* Subroutine */ int edge_(integer *, integer *, doublereal *, 
	    doublereal *, integer *, integer *, integer *, integer *, integer 
	    *, integer *);
    integer kbak, kfor, ilast, ifrst, lccip1;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   11/12/94 */

/*   This subroutine provides for creation of a constrained */
/* Delaunay triangulation which, in some sense, covers an */
/* arbitrary connected region R rather than the convex hull */
/* of the nodes.  This is achieved simply by forcing the */
/* presence of certain adjacencies (triangulation arcs) cor- */
/* responding to constraint curves.  The union of triangles */
/* coincides with the convex hull of the nodes, but triangles */
/* in R can be distinguished from those outside of R.  The */
/* only modification required to generalize the definition of */
/* the Delaunay triangulation is replacement of property 5 */
/* (refer to TRMESH) by the following: */

/*  5')  If a node is contained in the interior of the cir- */
/*       cumcircle of a triangle, then every interior point */
/*       of the triangle is separated from the node by a */
/*       constraint arc. */

/*   In order to be explicit, we make the following defini- */
/* tions.  A constraint region is the open interior of a */
/* simple closed positively oriented polygonal curve defined */
/* by an ordered sequence of three or more distinct nodes */
/* (constraint nodes) P(1),P(2),...,P(K), such that P(I) is */
/* adjacent to P(I+1) for I = 1,...,K with P(K+1) = P(1). */
/* Thus, the constraint region is on the left (and may have */
/* nonfinite area) as the sequence of constraint nodes is */
/* traversed in the specified order.  The constraint regions */
/* must not contain nodes and must not overlap.  The region */
/* R is the convex hull of the nodes with constraint regions */
/* excluded. */

/*   Note that the terms boundary node and boundary arc are */
/* reserved for nodes and arcs on the boundary of the convex */
/* hull of the nodes. */

/*   The algorithm is as follows:  given a triangulation */
/* which includes one or more sets of constraint nodes, the */
/* corresponding adjacencies (constraint arcs) are forced to */
/* be present (Subroutine EDGE).  Any additional new arcs */
/* required are chosen to be locally optimal (satisfy the */
/* modified circumcircle property). */


/* On input: */

/*       NCC = Number of constraint curves (constraint re- */
/*             gions).  NCC .GE. 0. */

/*       LCC = Array of length NCC (or dummy array of length */
/*             1 if NCC = 0) containing the index (for X, Y, */
/*             and LEND) of the first node of constraint I in */
/*             LCC(I) for I = 1 to NCC.  Thus, constraint I */
/*             contains K = LCC(I+1) - LCC(I) nodes, K .GE. */
/*             3, stored in (X,Y) locations LCC(I), ..., */
/*             LCC(I+1)-1, where LCC(NCC+1) = N+1. */

/*       N = Number of nodes in the triangulation, including */
/*           constraint nodes.  N .GE. 3. */

/*       X,Y = Arrays of length N containing the coordinates */
/*             of the nodes with non-constraint nodes in the */
/*             first LCC(1)-1 locations, followed by NCC se- */
/*             quences of constraint nodes.  Only one of */
/*             these sequences may be specified in clockwise */
/*             order to represent an exterior constraint */
/*             curve (a constraint region with nonfinite */
/*             area). */

/* The above parameters are not altered by this routine. */

/*       LWK = Length of IWK.  This must be at least 2*NI */
/*             where NI is the maximum number of arcs which */
/*             intersect a constraint arc to be added.  NI */
/*             is bounded by N-3. */

/*       IWK = Integer work array of length LWK (used by */
/*             Subroutine EDGE to add constraint arcs). */

/*       LIST,LPTR,LEND = Data structure defining the trian- */
/*                        gulation.  Refer to Subroutine */
/*                        TRMESH. */

/* On output: */

/*       LWK = Required length of IWK unless IER = 1 or IER = */
/*             3.  In the case of IER = 1, LWK is not altered */
/*             from its input value. */

/*       IWK = Array containing the endpoint indexes of the */
/*             new arcs which were swapped in by the last */
/*             call to Subroutine EDGE. */

/*       LIST,LPTR,LEND = Triangulation data structure with */
/*                        all constraint arcs present unless */
/*                        IER .NE. 0.  These arrays are not */
/*                        altered if IER = 1. */

/*       IER = Error indicator: */
/*             IER = 0 if no errors were encountered. */
/*             IER = 1 if NCC, N, or an LCC entry is outside */
/*                     its valid range, or LWK .LT. 0 on */
/*                     input. */
/*             IER = 2 if more space is required in IWK. */
/*             IER = 3 if the triangulation data structure is */
/*                     invalid, or failure (in EDGE or OPTIM) */
/*                     was caused by collinear nodes on the */
/*                     convex hull boundary.  An error mes- */
/*                     sage is written to logical unit 6 in */
/*                     this case. */
/*             IER = 4 if intersecting constraint arcs were */
/*                     encountered. */
/*             IER = 5 if a constraint region contains a */
/*                     node. */

/* Modules required by ADDCST:  EDGE, LEFT, LSTPTR, OPTIM, */
/*                                SWAP, SWPTST */

/* Intrinsic functions called by ADDCST:  ABS, MAX */

/* *********************************************************** */

    /* Parameter adjustments */
    --lcc;
    --lend;
    --y;
    --x;
    --iwk;
    --list;
    --lptr;

    /* Function Body */
    lwd2 = *lwk / 2;

/* Test for errors in input parameters. */

    *ier = 1;
    if (*ncc < 0 || *lwk < 0) {
	return 0;
    }
    if (*ncc == 0) {
	if (*n < 3) {
	    return 0;
	}
	*lwk = 0;
	goto L9;
    } else {
	lccip1 = *n + 1;
	for (i__ = *ncc; i__ >= 1; --i__) {
	    if (lccip1 - lcc[i__] < 3) {
		return 0;
	    }
	    lccip1 = lcc[i__];
/* L1: */
	}
	if (lccip1 < 1) {
	    return 0;
	}
    }

/* Force the presence of constraint arcs.  The outer loop is */
/*   on constraints in reverse order.  IFRST and ILAST are */
/*   the first and last nodes of constraint I. */

    *lwk = 0;
    ifrst = *n + 1;
    for (i__ = *ncc; i__ >= 1; --i__) {
	ilast = ifrst - 1;
	ifrst = lcc[i__];

/*   Inner loop on constraint arcs N1-N2 in constraint I. */

	n1 = ilast;
	i__1 = ilast;
	for (n2 = ifrst; n2 <= i__1; ++n2) {
	    lw = lwd2;
	    edge_(&n1, &n2, &x[1], &y[1], &lw, &iwk[1], &list[1], &lptr[1], &
		    lend[1], ier);
/* Computing MAX */
	    i__2 = *lwk, i__3 = lw << 1;
	    *lwk = max(i__2,i__3);
	    if (*ier == 4) {
		*ier = 3;
	    }
	    if (*ier != 0) {
		return 0;
	    }
	    n1 = n2;
/* L2: */
	}
/* L3: */
    }

/* Test for errors.  The outer loop is on constraint I with */
/*   first and last nodes IFRST and ILAST, and the inner loop */
/*   is on constraint nodes K with (KBAK,K,KFOR) a subse- */
/*   quence of constraint I. */

    *ier = 4;
    ifrst = *n + 1;
    for (i__ = *ncc; i__ >= 1; --i__) {
	ilast = ifrst - 1;
	ifrst = lcc[i__];
	kbak = ilast;
	i__1 = ilast;
	for (k = ifrst; k <= i__1; ++k) {
	    kfor = k + 1;
	    if (k == ilast) {
		kfor = ifrst;
	    }

/*   Find the LIST pointers LPF and LPB of KFOR and KBAK as */
/*     neighbors of K. */

	    lpf = 0;
	    lpb = 0;
	    lpl = lend[k];
	    lp = lpl;

L4:
	    lp = lptr[lp];
	    kn = (i__2 = list[lp], abs(i__2));
	    if (kn == kfor) {
		lpf = lp;
	    }
	    if (kn == kbak) {
		lpb = lp;
	    }
	    if (lp != lpl) {
		goto L4;
	    }

/*   A pair of intersecting constraint arcs was encountered */
/*     if and only if a constraint arc is missing (introduc- */
/*     tion of the second caused the first to be swapped out). */

	    if (lpf == 0 || lpb == 0) {
		return 0;
	    }

/*   Loop on neighbors KN of node K which follow KFOR and */
/*     precede KBAK.  The constraint region contains no nodes */
/*     if and only if all such nodes KN are in constraint I. */

	    lp = lpf;
L5:
	    lp = lptr[lp];
	    if (lp == lpb) {
		goto L6;
	    }
	    kn = (i__2 = list[lp], abs(i__2));
	    if (kn < ifrst || kn > ilast) {
		goto L10;
	    }
	    goto L5;

/*   Bottom of loop. */

L6:
	    kbak = k;
/* L7: */
	}
/* L8: */
    }

/* No errors encountered. */

L9:
    *ier = 0;
    return 0;

/* A constraint region contains a node. */

L10:
    *ier = 5;
    return 0;
} /* addcst_ */

/* Subroutine */ int addnod_(integer *k, doublereal *xk, doublereal *yk, 
	integer *ist, integer *ncc, integer *lcc, integer *n, doublereal *x, 
	doublereal *y, integer *list, integer *lptr, integer *lend, integer *
	lnew, integer *ier)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, l, i1, i2, i3, kk, lp, in1, io1, io2, nm1, ibk, lpf, lpo1;
    extern /* Subroutine */ int swap_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *);
    extern logical crtri_(integer *, integer *, integer *, integer *, integer 
	    *);
    integer lccip1;
    extern /* Subroutine */ int bdyadd_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *), intadd_(integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *);
    extern integer indxcc_(integer *, integer *, integer *, integer *, 
	    integer *);
    extern /* Subroutine */ int trfind_(integer *, doublereal *, doublereal *,
	     integer *, doublereal *, doublereal *, integer *, integer *, 
	    integer *, integer *, integer *, integer *);
    extern integer lstptr_(integer *, integer *, integer *, integer *);
    extern logical swptst_(integer *, integer *, integer *, integer *, 
	    doublereal *, doublereal *);


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   06/27/98 */

/*   Given a triangulation of N nodes in the plane created by */
/* Subroutine TRMESH or TRMSHR, this subroutine updates the */
/* data structure with the addition of a new node in position */
/* K.  If node K is inserted into X and Y (K .LE. N) rather */
/* than appended (K = N+1), then a corresponding insertion */
/* must be performed in any additional arrays associated */
/* with the nodes.  For example, an array of data values Z */
/* must be shifted down to open up position K for the new */
/* value:  set Z(I+1) to Z(I) for I = N,N-1,...,K.  For */
/* optimal efficiency, new nodes should be appended whenever */
/* possible.  Insertion is necessary, however, to add a non- */
/* constraint node when constraints are present (refer to */
/* Subroutine ADDCST). */

/*   Note that a constraint node cannot be added by this */
/* routine.  In order to insert a constraint node, it is */
/* necessary to add the node with no constraints present */
/* (call this routine with NCC = 0), update LCC by increment- */
/* ing the appropriate entries, and then create (or restore) */
/* the constraints by a call to ADDCST. */

/*   The algorithm consists of the following steps:  node K */
/* is located relative to the triangulation (TRFIND), its */
/* index is added to the data structure (INTADD or BDYADD), */
/* and a sequence of swaps (SWPTST and SWAP) are applied to */
/* the arcs opposite K so that all arcs incident on node K */
/* and opposite node K (excluding constraint arcs) are local- */
/* ly optimal (satisfy the circumcircle test).  Thus, if a */
/* (constrained) Delaunay triangulation is input, a (con- */
/* strained) Delaunay triangulation will result.  All indexes */
/* are incremented as necessary for an insertion. */


/* On input: */

/*       K = Nodal index (index for X, Y, and LEND) of the */
/*           new node to be added.  1 .LE. K .LE. LCC(1). */
/*           (K .LE. N+1 if NCC=0). */

/*       XK,YK = Cartesian coordinates of the new node (to be */
/*               stored in X(K) and Y(K)).  The node must not */
/*               lie in a constraint region. */

/*       IST = Index of a node at which TRFIND begins the */
/*             search.  Search time depends on the proximity */
/*             of this node to node K.  1 .LE. IST .LE. N. */

/*       NCC = Number of constraint curves.  NCC .GE. 0. */

/* The above parameters are not altered by this routine. */

/*       LCC = List of constraint curve starting indexes (or */
/*             dummy array of length 1 if NCC = 0).  Refer to */
/*             Subroutine ADDCST. */

/*       N = Number of nodes in the triangulation before K is */
/*           added.  N .GE. 3.  Note that N will be incre- */
/*           mented following the addition of node K. */

/*       X,Y = Arrays of length at least N+1 containing the */
/*             Cartesian coordinates of the nodes in the */
/*             first N positions with non-constraint nodes */
/*             in the first LCC(1)-1 locations if NCC > 0. */

/*       LIST,LPTR,LEND,LNEW = Data structure associated with */
/*                             the triangulation of nodes 1 */
/*                             to N.  The arrays must have */
/*                             sufficient length for N+1 */
/*                             nodes.  Refer to TRMESH. */

/* On output: */

/*       LCC = List of constraint curve starting indexes in- */
/*             cremented by 1 to reflect the insertion of K */
/*             unless NCC = 0 or (IER .NE. 0 and IER .NE. */
/*             -4). */

/*       N = Number of nodes in the triangulation including K */
/*           unless IER .NE. 0 and IER .NE. -4.  Note that */
/*           all comments refer to the input value of N. */

/*       X,Y = Arrays updated with the insertion of XK and YK */
/*             in the K-th positions (node I+1 was node I be- */
/*             fore the insertion for I = K to N if K .LE. N) */
/*             unless IER .NE. 0 and IER .NE. -4. */

/*       LIST,LPTR,LEND,LNEW = Data structure updated with */
/*                             the addition of node K unless */
/*                             IER .NE. 0 and IER .NE. -4. */

/*       IER = Error indicator: */
/*             IER =  0 if no errors were encountered. */
/*             IER = -1 if K, IST, NCC, N, or an LCC entry is */
/*                      outside its valid range on input. */
/*             IER = -2 if all nodes (including K) are col- */
/*                      linear. */
/*             IER =  L if nodes L and K coincide for some L. */
/*             IER = -3 if K lies in a constraint region. */
/*             IER = -4 if an error flag is returned by SWAP */
/*                      implying that the triangulation */
/*                      (geometry) was bad on input. */

/*             The errors conditions are tested in the order */
/*             specified. */

/* Modules required by ADDNOD:  BDYADD, CRTRI, INDXCC, */
/*                                INSERT, INTADD, JRAND, */
/*                                LEFT, LSTPTR, SWAP, */
/*                                SWPTST, TRFIND */

/* Intrinsic function called by ADDNOD:  ABS */

/* *********************************************************** */

    /* Parameter adjustments */
    --lend;
    --lptr;
    --list;
    --y;
    --x;
    --lcc;

    /* Function Body */
    kk = *k;

/* Test for an invalid input parameter. */

    if (kk < 1 || *ist < 1 || *ist > *n || *ncc < 0 || *n < 3) {
	goto L7;
    }
    lccip1 = *n + 1;
    for (i__ = *ncc; i__ >= 1; --i__) {
	if (lccip1 - lcc[i__] < 3) {
	    goto L7;
	}
	lccip1 = lcc[i__];
/* L1: */
    }
    if (kk > lccip1) {
	goto L7;
    }

/* Find a triangle (I1,I2,I3) containing K or the rightmost */
/*   (I1) and leftmost (I2) visible boundary nodes as viewed */
/*   from node K. */

    trfind_(ist, xk, yk, n, &x[1], &y[1], &list[1], &lptr[1], &lend[1], &i1, &
	    i2, &i3);

/* Test for collinear nodes, duplicate nodes, and K lying in */
/*   a constraint region. */

    if (i1 == 0) {
	goto L8;
    }
    if (i3 != 0) {
	l = i1;
	if (*xk == x[l] && *yk == y[l]) {
	    goto L9;
	}
	l = i2;
	if (*xk == x[l] && *yk == y[l]) {
	    goto L9;
	}
	l = i3;
	if (*xk == x[l] && *yk == y[l]) {
	    goto L9;
	}
	if (*ncc > 0 && crtri_(ncc, &lcc[1], &i1, &i2, &i3)) {
	    goto L10;
	}
    } else {

/*   K is outside the convex hull of the nodes and lies in a */
/*     constraint region iff an exterior constraint curve is */
/*     present. */

	if (*ncc > 0 && indxcc_(ncc, &lcc[1], n, &list[1], &lend[1]) != 0) {
	    goto L10;
	}
    }

/* No errors encountered. */

    *ier = 0;
    nm1 = *n;
    ++(*n);
    if (kk < *n) {

/* Open a slot for K in X, Y, and LEND, and increment all */
/*   nodal indexes which are greater than or equal to K. */
/*   Note that LIST, LPTR, and LNEW are not yet updated with */
/*   either the neighbors of K or the edges terminating on K. */

	i__1 = kk;
	for (ibk = nm1; ibk >= i__1; --ibk) {
	    x[ibk + 1] = x[ibk];
	    y[ibk + 1] = y[ibk];
	    lend[ibk + 1] = lend[ibk];
/* L2: */
	}
	i__1 = *ncc;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ++lcc[i__];
/* L3: */
	}
	l = *lnew - 1;
	i__1 = l;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (list[i__] >= kk) {
		++list[i__];
	    }
	    if (list[i__] <= -kk) {
		--list[i__];
	    }
/* L4: */
	}
	if (i1 >= kk) {
	    ++i1;
	}
	if (i2 >= kk) {
	    ++i2;
	}
	if (i3 >= kk) {
	    ++i3;
	}
    }

/* Insert K into X and Y, and update LIST, LPTR, LEND, and */
/*   LNEW with the arcs containing node K. */

    x[kk] = *xk;
    y[kk] = *yk;
    if (i3 == 0) {
	bdyadd_(&kk, &i1, &i2, &list[1], &lptr[1], &lend[1], lnew);
    } else {
	intadd_(&kk, &i1, &i2, &i3, &list[1], &lptr[1], &lend[1], lnew);
    }

/* Initialize variables for optimization of the triangula- */
/*   tion. */

    lp = lend[kk];
    lpf = lptr[lp];
    io2 = list[lpf];
    lpo1 = lptr[lpf];
    io1 = (i__1 = list[lpo1], abs(i__1));

/* Begin loop:  find the node opposite K. */

L5:
    lp = lstptr_(&lend[io1], &io2, &list[1], &lptr[1]);
    if (list[lp] < 0) {
	goto L6;
    }
    lp = lptr[lp];
    in1 = (i__1 = list[lp], abs(i__1));
    if (crtri_(ncc, &lcc[1], &io1, &io2, &in1)) {
	goto L6;
    }

/* Swap test:  if a swap occurs, two new arcs are */
/*             opposite K and must be tested. */

    if (! swptst_(&in1, &kk, &io1, &io2, &x[1], &y[1])) {
	goto L6;
    }
    swap_(&in1, &kk, &io1, &io2, &list[1], &lptr[1], &lend[1], &lpo1);
    if (lpo1 == 0) {
	goto L11;
    }
    io1 = in1;
    goto L5;

/* No swap occurred.  Test for termination and reset */
/*   IO2 and IO1. */

L6:
    if (lpo1 == lpf || list[lpo1] < 0) {
	return 0;
    }
    io2 = io1;
    lpo1 = lptr[lpo1];
    io1 = (i__1 = list[lpo1], abs(i__1));
    goto L5;

/* A parameter is outside its valid range on input. */

L7:
    *ier = -1;
    return 0;

/* All nodes are collinear. */

L8:
    *ier = -2;
    return 0;

/* Nodes L and K coincide. */

L9:
    *ier = l;
    return 0;

/* Node K lies in a constraint region. */

L10:
    *ier = -3;
    return 0;

/* Zero pointer returned by SWAP. */

L11:
    *ier = -4;
    return 0;
} /* addnod_ */

doublereal areap_(doublereal *x, doublereal *y, integer *nb, integer *nodes)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val;

    /* Local variables */
    doublereal a;
    integer i__, nd1, nd2, nnb;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   09/21/90 */

/*   Given a sequence of NB points in the plane, this func- */
/* tion computes the signed area bounded by the closed poly- */
/* gonal curve which passes through the points in the */
/* specified order.  Each simple closed curve is positively */
/* oriented (bounds positive area) if and only if the points */
/* are specified in counterclockwise order.  The last point */
/* of the curve is taken to be the first point specified, and */
/* this point should therefore not be specified twice. */

/*   The area of a triangulation may be computed by calling */
/* AREAP with values of NB and NODES determined by Subroutine */
/* BNODES. */


/* On input: */

/*       X,Y = Arrays of length N containing the Cartesian */
/*             coordinates of a set of points in the plane */
/*             for some N .GE. NB. */

/*       NB = Length of NODES. */

/*       NODES = Array of length NB containing the ordered */
/*               sequence of nodal indexes (in the range */
/*               1 to N) which define the polygonal curve. */

/* Input parameters are not altered by this function. */

/* On output: */

/*       AREAP = Signed area bounded by the polygonal curve, */
/*              or zero if NB < 3. */

/* Modules required by AREAP:  None */

/* *********************************************************** */


/* Local parameters: */

/* A =       Partial sum of signed (and doubled) trapezoid */
/*             areas */
/* I =       DO-loop and NODES index */
/* ND1,ND2 = Elements of NODES */
/* NNB =     Local copy of NB */

    /* Parameter adjustments */
    --x;
    --y;
    --nodes;

    /* Function Body */
    nnb = *nb;
    a = 0.;
    if (nnb < 3) {
	goto L2;
    }
    nd2 = nodes[nnb];

/* Loop on line segments NODES(I-1) -> NODES(I), where */
/*   NODES(0) = NODES(NB), adding twice the signed trapezoid */
/*   areas (integrals of the linear interpolants) to A. */

    i__1 = nnb;
    for (i__ = 1; i__ <= i__1; ++i__) {
	nd1 = nd2;
	nd2 = nodes[i__];
	a += (x[nd2] - x[nd1]) * (y[nd1] + y[nd2]);
/* L1: */
    }

/* A contains twice the negative signed area of the region. */

L2:
    ret_val = -a / 2.;
    return ret_val;
} /* areap_ */

/* Subroutine */ int bdyadd_(integer *kk, integer *i1, integer *i2, integer *
	list, integer *lptr, integer *lend, integer *lnew)
{
    integer k, n1, n2, lp, lsav, nsav, next;
    extern /* Subroutine */ int insert_(integer *, integer *, integer *, 
	    integer *, integer *);


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   02/22/91 */

/*   This subroutine adds a boundary node to a triangulation */
/* of a set of points in the plane.  The data structure is */
/* updated with the insertion of node KK, but no optimization */
/* is performed. */


/* On input: */

/*       KK = Index of a node to be connected to the sequence */
/*            of all visible boundary nodes.  KK .GE. 1 and */
/*            KK must not be equal to I1 or I2. */

/*       I1 = First (rightmost as viewed from KK) boundary */
/*            node in the triangulation which is visible from */
/*            node KK (the line segment KK-I1 intersects no */
/*            arcs. */

/*       I2 = Last (leftmost) boundary node which is visible */
/*            from node KK.  I1 and I2 may be determined by */
/*            Subroutine TRFIND. */

/* The above parameters are not altered by this routine. */

/*       LIST,LPTR,LEND,LNEW = Triangulation data structure */
/*                             created by TRMESH or TRMSHR. */
/*                             Nodes I1 and I2 must be in- */
/*                             cluded in the triangulation. */

/* On output: */

/*       LIST,LPTR,LEND,LNEW = Data structure updated with */
/*                             the addition of node KK.  Node */
/*                             KK is connected to I1, I2, and */
/*                             all boundary nodes in between. */

/* Module required by BDYADD:  INSERT */

/* *********************************************************** */

    /* Parameter adjustments */
    --lend;
    --lptr;
    --list;

    /* Function Body */
    k = *kk;
    n1 = *i1;
    n2 = *i2;

/* Add K as the last neighbor of N1. */

    lp = lend[n1];
    lsav = lptr[lp];
    lptr[lp] = *lnew;
    list[*lnew] = -k;
    lptr[*lnew] = lsav;
    lend[n1] = *lnew;
    ++(*lnew);
    next = -list[lp];
    list[lp] = next;
    nsav = next;

/* Loop on the remaining boundary nodes between N1 and N2, */
/*   adding K as the first neighbor. */

L1:
    lp = lend[next];
    insert_(&k, &lp, &list[1], &lptr[1], lnew);
    if (next == n2) {
	goto L2;
    }
    next = -list[lp];
    list[lp] = next;
    goto L1;

/* Add the boundary nodes between N1 and N2 as neighbors */
/*   of node K. */

L2:
    lsav = *lnew;
    list[*lnew] = n1;
    lptr[*lnew] = *lnew + 1;
    ++(*lnew);
    next = nsav;

L3:
    if (next == n2) {
	goto L4;
    }
    list[*lnew] = next;
    lptr[*lnew] = *lnew + 1;
    ++(*lnew);
    lp = lend[next];
    next = list[lp];
    goto L3;

L4:
    list[*lnew] = -n2;
    lptr[*lnew] = lsav;
    lend[k] = *lnew;
    ++(*lnew);
    return 0;
} /* bdyadd_ */

/* Subroutine */ int bnodes_(integer *n, integer *list, integer *lptr, 
	integer *lend, integer *nodes, integer *nb, integer *na, integer *nt)
{
    integer k, n0, lp, nst;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   09/01/88 */

/*   Given a triangulation of N points in the plane, this */
/* subroutine returns an array containing the indexes, in */
/* counterclockwise order, of the nodes on the boundary of */
/* the convex hull of the set of points. */


/* On input: */

/*       N = Number of nodes in the triangulation.  N .GE. 3. */

/*       LIST,LPTR,LEND = Data structure defining the trian- */
/*                        gulation.  Refer to Subroutine */
/*                        TRMESH. */

/* The above parameters are not altered by this routine. */

/*       NODES = Integer array of length at least NB */
/*               (NB .LE. N). */

/* On output: */

/*       NODES = Ordered sequence of boundary node indexes */
/*               in the range 1 to N. */

/*       NB = Number of boundary nodes. */

/*       NA,NT = Number of arcs and triangles, respectively, */
/*               in the triangulation. */

/* Modules required by BNODES:  None */

/* *********************************************************** */


/* Set NST to the first boundary node encountered. */

    /* Parameter adjustments */
    --lend;
    --list;
    --lptr;
    --nodes;

    /* Function Body */
    nst = 1;
L1:
    lp = lend[nst];
    if (list[lp] < 0) {
	goto L2;
    }
    ++nst;
    goto L1;

/* Initialization. */

L2:
    nodes[1] = nst;
    k = 1;
    n0 = nst;

/* Traverse the boundary in counterclockwise order. */

L3:
    lp = lend[n0];
    lp = lptr[lp];
    n0 = list[lp];
    if (n0 == nst) {
	goto L4;
    }
    ++k;
    nodes[k] = n0;
    goto L3;

/* Termination. */

L4:
    *nb = k;
    *nt = (*n << 1) - *nb - 2;
    *na = *nt + *n - 1;
    return 0;
} /* bnodes_ */

/* Subroutine */ int circum_(doublereal *x1, doublereal *y1, doublereal *x2, 
	doublereal *y2, doublereal *x3, doublereal *y3, logical *ratio, 
	doublereal *xc, doublereal *yc, doublereal *cr, doublereal *sa, 
	doublereal *ar)
{
    /* System generated locals */
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    integer i__;
    doublereal u[3], v[3], ds[3], fx, fy;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   12/10/96 */

/*   Given three vertices defining a triangle, this subrou- */
/* tine returns the circumcenter, circumradius, signed */
/* triangle area, and, optionally, the aspect ratio of the */
/* triangle. */


/* On input: */

/*       X1,...,Y3 = Cartesian coordinates of the vertices. */

/*       RATIO = Logical variable with value TRUE if and only */
/*               if the aspect ratio is to be computed. */

/* Input parameters are not altered by this routine. */

/* On output: */

/*       XC,YC = Cartesian coordinates of the circumcenter */
/*               (center of the circle defined by the three */
/*               points) unless SA = 0, in which XC and YC */
/*               are not altered. */

/*       CR = Circumradius (radius of the circle defined by */
/*            the three points) unless SA = 0 (infinite */
/*            radius), in which case CR is not altered. */

/*       SA = Signed triangle area with positive value if */
/*            and only if the vertices are specified in */
/*            counterclockwise order:  (X3,Y3) is strictly */
/*            to the left of the directed line from (X1,Y1) */
/*            toward (X2,Y2). */

/*       AR = Aspect ratio r/CR, where r is the radius of the */
/*            inscribed circle, unless RATIO = FALSE, in */
/*            which case AR is not altered.  AR is in the */
/*            range 0 to .5, with value 0 iff SA = 0 and */
/*            value .5 iff the vertices define an equilateral */
/*            triangle. */

/* Modules required by CIRCUM:  None */

/* Intrinsic functions called by CIRCUM:  ABS, SQRT */

/* *********************************************************** */


/* Set U(K) and V(K) to the x and y components, respectively, */
/*   of the directed edge opposite vertex K. */

    u[0] = *x3 - *x2;
    u[1] = *x1 - *x3;
    u[2] = *x2 - *x1;
    v[0] = *y3 - *y2;
    v[1] = *y1 - *y3;
    v[2] = *y2 - *y1;

/* Set SA to the signed triangle area. */

    *sa = (u[0] * v[1] - u[1] * v[0]) / 2.;
    if (*sa == 0.) {
	if (*ratio) {
	    *ar = 0.;
	}
	return 0;
    }

/* Set DS(K) to the squared distance from the origin to */
/*   vertex K. */

    ds[0] = *x1 * *x1 + *y1 * *y1;
    ds[1] = *x2 * *x2 + *y2 * *y2;
    ds[2] = *x3 * *x3 + *y3 * *y3;

/* Compute factors of XC and YC. */

    fx = 0.;
    fy = 0.;
    for (i__ = 1; i__ <= 3; ++i__) {
	fx -= ds[i__ - 1] * v[i__ - 1];
	fy += ds[i__ - 1] * u[i__ - 1];
/* L1: */
    }
    *xc = fx / (*sa * 4.);
    *yc = fy / (*sa * 4.);
/* Computing 2nd power */
    d__1 = *xc - *x1;
/* Computing 2nd power */
    d__2 = *yc - *y1;
    *cr = sqrt(d__1 * d__1 + d__2 * d__2);
    if (! (*ratio)) {
	return 0;
    }

/* Compute the squared edge lengths and aspect ratio. */

    for (i__ = 1; i__ <= 3; ++i__) {
	ds[i__ - 1] = u[i__ - 1] * u[i__ - 1] + v[i__ - 1] * v[i__ - 1];
/* L2: */
    }
    *ar = abs(*sa) * 2. / ((sqrt(ds[0]) + sqrt(ds[1]) + sqrt(ds[2])) * *cr);
    return 0;
} /* circum_ */

logical crtri_(integer *ncc, integer *lcc, integer *i1, integer *i2, integer *
	i3)
{
    /* System generated locals */
    integer i__1;
    logical ret_val;

    /* Local variables */
    integer i__, imin, imax;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   08/14/91 */

/*   This function returns TRUE if and only if triangle (I1, */
/* I2,I3) lies in a constraint region. */


/* On input: */

/*       NCC,LCC = Constraint data structure.  Refer to Sub- */
/*                 routine ADDCST. */

/*       I1,I2,I3 = Nodal indexes of the counterclockwise- */
/*                  ordered vertices of a triangle. */

/* Input parameters are altered by this function. */

/*       CRTRI = TRUE iff (I1,I2,I3) is a constraint region */
/*               triangle. */

/* Note that input parameters are not tested for validity. */

/* Modules required by CRTRI:  None */

/* Intrinsic functions called by CRTRI:  MAX, MIN */

/* *********************************************************** */

    /* Parameter adjustments */
    --lcc;

    /* Function Body */
/* Computing MAX */
    i__1 = max(*i1,*i2);
    imax = max(i__1,*i3);

/*   Find the index I of the constraint containing IMAX. */

    i__ = *ncc + 1;
L1:
    --i__;
    if (i__ <= 0) {
	goto L2;
    }
    if (imax < lcc[i__]) {
	goto L1;
    }
/* Computing MIN */
    i__1 = min(*i1,*i2);
    imin = min(i__1,*i3);

/* P lies in a constraint region iff I1, I2, and I3 are nodes */
/*   of the same constraint (IMIN >= LCC(I)), and (IMIN,IMAX) */
/*   is (I1,I3), (I2,I1), or (I3,I2). */

    ret_val = imin >= lcc[i__] && (imin == *i1 && imax == *i3 || imin == *i2 
	    && imax == *i1 || imin == *i3 && imax == *i2);
    return ret_val;

/* NCC .LE. 0 or all vertices are non-constraint nodes. */

L2:
    ret_val = FALSE_;
    return ret_val;
} /* crtri_ */

/* Subroutine */ int delarc_(integer *n, integer *io1, integer *io2, integer *
	list, integer *lptr, integer *lend, integer *lnew, integer *ier)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer n1, n2, n3, lp, lph, lpl;
    extern /* Subroutine */ int delnb_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *);
    extern integer lstptr_(integer *, integer *, integer *, integer *);


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   11/12/94 */

/*   This subroutine deletes a boundary arc from a triangula- */
/* tion.  It may be used to remove a null triangle from the */
/* convex hull boundary.  Note, however, that if the union of */
/* triangles is rendered nonconvex, Subroutines DELNOD, EDGE, */
/* and TRFIND may fail.  Thus, Subroutines ADDCST, ADDNOD, */
/* DELNOD, EDGE, and NEARND should not be called following */
/* an arc deletion. */


/* On input: */

/*       N = Number of nodes in the triangulation.  N .GE. 4. */

/*       IO1,IO2 = Indexes (in the range 1 to N) of a pair of */
/*                 adjacent boundary nodes defining the arc */
/*                 to be removed. */

/* The above parameters are not altered by this routine. */

/*       LIST,LPTR,LEND,LNEW = Triangulation data structure */
/*                             created by TRMESH or TRMSHR. */

/* On output: */

/*       LIST,LPTR,LEND,LNEW = Data structure updated with */
/*                             the removal of arc IO1-IO2 */
/*                             unless IER > 0. */

/*       IER = Error indicator: */
/*             IER = 0 if no errors were encountered. */
/*             IER = 1 if N, IO1, or IO2 is outside its valid */
/*                     range, or IO1 = IO2. */
/*             IER = 2 if IO1-IO2 is not a boundary arc. */
/*             IER = 3 if the node opposite IO1-IO2 is al- */
/*                     ready a boundary node, and thus IO1 */
/*                     or IO2 has only two neighbors or a */
/*                     deletion would result in two triangu- */
/*                     lations sharing a single node. */
/*             IER = 4 if one of the nodes is a neighbor of */
/*                     the other, but not vice versa, imply- */
/*                     ing an invalid triangulation data */
/*                     structure. */

/* Modules required by DELARC:  DELNB, LSTPTR */

/* Intrinsic function called by DELARC:  ABS */

/* *********************************************************** */

    /* Parameter adjustments */
    --lend;
    --list;
    --lptr;

    /* Function Body */
    n1 = *io1;
    n2 = *io2;

/* Test for errors, and set N1->N2 to the directed boundary */
/*   edge associated with IO1-IO2:  (N1,N2,N3) is a triangle */
/*   for some N3. */

    if (*n < 4 || n1 < 1 || n1 > *n || n2 < 1 || n2 > *n || n1 == n2) {
	*ier = 1;
	return 0;
    }

    lpl = lend[n2];
    if (-list[lpl] != n1) {
	n1 = n2;
	n2 = *io1;
	lpl = lend[n2];
	if (-list[lpl] != n1) {
	    *ier = 2;
	    return 0;
	}
    }

/* Set N3 to the node opposite N1->N2 (the second neighbor */
/*   of N1), and test for error 3 (N3 already a boundary */
/*   node). */

    lpl = lend[n1];
    lp = lptr[lpl];
    lp = lptr[lp];
    n3 = (i__1 = list[lp], abs(i__1));
    lpl = lend[n3];
    if (list[lpl] <= 0) {
	*ier = 3;
	return 0;
    }

/* Delete N2 as a neighbor of N1, making N3 the first */
/*   neighbor, and test for error 4 (N2 not a neighbor */
/*   of N1).  Note that previously computed pointers may */
/*   no longer be valid following the call to DELNB. */

    delnb_(&n1, &n2, n, &list[1], &lptr[1], &lend[1], lnew, &lph);
    if (lph < 0) {
	*ier = 4;
	return 0;
    }

/* Delete N1 as a neighbor of N2, making N3 the new last */
/*   neighbor. */

    delnb_(&n2, &n1, n, &list[1], &lptr[1], &lend[1], lnew, &lph);

/* Make N3 a boundary node with first neighbor N2 and last */
/*   neighbor N1. */

    lp = lstptr_(&lend[n3], &n1, &list[1], &lptr[1]);
    lend[n3] = lp;
    list[lp] = -n1;

/* No errors encountered. */

    *ier = 0;
    return 0;
} /* delarc_ */

/* Subroutine */ int delnb_(integer *n0, integer *nb, integer *n, integer *
	list, integer *lptr, integer *lend, integer *lnew, integer *lph)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, lp, nn, lpb, lpl, lpp, lnw;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   07/30/98 */

/*   This subroutine deletes a neighbor NB from the adjacency */
/* list of node N0 (but N0 is not deleted from the adjacency */
/* list of NB) and, if NB is a boundary node, makes N0 a */
/* boundary node.  For pointer (LIST index) LPH to NB as a */
/* neighbor of N0, the empty LIST,LPTR location LPH is filled */
/* in with the values at LNEW-1, pointer LNEW-1 (in LPTR and */
/* possibly in LEND) is changed to LPH, and LNEW is decremen- */
/* ted.  This requires a search of LEND and LPTR entailing an */
/* expected operation count of O(N). */


/* On input: */

/*       N0,NB = Indexes, in the range 1 to N, of a pair of */
/*               nodes such that NB is a neighbor of N0. */
/*               (N0 need not be a neighbor of NB.) */

/*       N = Number of nodes in the triangulation.  N .GE. 3. */

/* The above parameters are not altered by this routine. */

/*       LIST,LPTR,LEND,LNEW = Data structure defining the */
/*                             triangulation. */

/* On output: */

/*       LIST,LPTR,LEND,LNEW = Data structure updated with */
/*                             the removal of NB from the ad- */
/*                             jacency list of N0 unless */
/*                             LPH < 0. */

/*       LPH = List pointer to the hole (NB as a neighbor of */
/*             N0) filled in by the values at LNEW-1 or error */
/*             indicator: */
/*             LPH > 0 if no errors were encountered. */
/*             LPH = -1 if N0, NB, or N is outside its valid */
/*                      range. */
/*             LPH = -2 if NB is not a neighbor of N0. */

/* Modules required by DELNB:  None */

/* Intrinsic function called by DELNB:  ABS */

/* *********************************************************** */


/* Local parameters: */

/* I =   DO-loop index */
/* LNW = LNEW-1 (output value of LNEW) */
/* LP =  LIST pointer of the last neighbor of NB */
/* LPB = Pointer to NB as a neighbor of N0 */
/* LPL = Pointer to the last neighbor of N0 */
/* LPP = Pointer to the neighbor of N0 that precedes NB */
/* NN =  Local copy of N */

    /* Parameter adjustments */
    --lend;
    --list;
    --lptr;

    /* Function Body */
    nn = *n;

/* Test for error 1. */

    if (*n0 < 1 || *n0 > nn || *nb < 1 || *nb > nn || nn < 3) {
	*lph = -1;
	return 0;
    }

/*   Find pointers to neighbors of N0: */

/*     LPL points to the last neighbor, */
/*     LPP points to the neighbor NP preceding NB, and */
/*     LPB points to NB. */

    lpl = lend[*n0];
    lpp = lpl;
    lpb = lptr[lpp];
L1:
    if (list[lpb] == *nb) {
	goto L2;
    }
    lpp = lpb;
    lpb = lptr[lpp];
    if (lpb != lpl) {
	goto L1;
    }

/*   Test for error 2 (NB not found). */

    if ((i__1 = list[lpb], abs(i__1)) != *nb) {
	*lph = -2;
	return 0;
    }

/*   NB is the last neighbor of N0.  Make NP the new last */
/*     neighbor and, if NB is a boundary node, then make N0 */
/*     a boundary node. */

    lend[*n0] = lpp;
    lp = lend[*nb];
    if (list[lp] < 0) {
	list[lpp] = -list[lpp];
    }
    goto L3;

/*   NB is not the last neighbor of N0.  If NB is a boundary */
/*     node and N0 is not, then make N0 a boundary node with */
/*     last neighbor NP. */

L2:
    lp = lend[*nb];
    if (list[lp] < 0 && list[lpl] > 0) {
	lend[*n0] = lpp;
	list[lpp] = -list[lpp];
    }

/*   Update LPTR so that the neighbor following NB now fol- */
/*     lows NP, and fill in the hole at location LPB. */

L3:
    lptr[lpp] = lptr[lpb];
    lnw = *lnew - 1;
    list[lpb] = list[lnw];
    lptr[lpb] = lptr[lnw];
    for (i__ = nn; i__ >= 1; --i__) {
	if (lend[i__] == lnw) {
	    lend[i__] = lpb;
	    goto L5;
	}
/* L4: */
    }

L5:
    i__1 = lnw - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (lptr[i__] == lnw) {
	    lptr[i__] = lpb;
	}
/* L6: */
    }

/* No errors encountered. */

    *lnew = lnw;
    *lph = lpb;
    return 0;
} /* delnb_ */

/* Subroutine */ int delnod_(integer *k, integer *ncc, integer *lcc, integer *
	n, doublereal *x, doublereal *y, integer *list, integer *lptr, 
	integer *lend, integer *lnew, integer *lwk, integer *iwk, integer *
	ier)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, j, n1, n2;
    doublereal x1, x2, y1, y2;
    integer nl, lp, nn, nr;
    doublereal xl, yl, xr, yr;
    integer nnb, lp21, lpf, lph, lpl, lpn, iwl, nit, lnw, lpl2;
    extern logical left_(doublereal *, doublereal *, doublereal *, doublereal 
	    *, doublereal *, doublereal *);
    logical bdry;
    integer ierr, lwkl;
    extern /* Subroutine */ int swap_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), delnb_(
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *);
    extern integer nbcnt_(integer *, integer *);
    extern /* Subroutine */ int optim_(doublereal *, doublereal *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *);
    integer nfrst, lccip1;
    extern integer lstptr_(integer *, integer *, integer *, integer *);


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   06/28/98 */

/*   This subroutine deletes node K (along with all arcs */
/* incident on node K) from a triangulation of N nodes in the */
/* plane, and inserts arcs as necessary to produce a triangu- */
/* lation of the remaining N-1 nodes.  If a Delaunay triangu- */
/* lation is input, a Delaunay triangulation will result, and */
/* thus, DELNOD reverses the effect of a call to Subroutine */
/* ADDNOD. */

/*   Note that a constraint node cannot be deleted by this */
/* routine.  In order to delete a constraint node, it is */
/* necessary to call this routine with NCC = 0, decrement the */
/* appropriate LCC entries (LCC(I) such that LCC(I) > K), and */
/* then create (or restore) the constraints by a call to Sub- */
/* routine ADDCST. */


/* On input: */

/*       K = Index (for X and Y) of the node to be deleted. */
/*           1 .LE. K .LT. LCC(1).  (K .LE. N if NCC=0). */

/*       NCC = Number of constraint curves.  NCC .GE. 0. */

/* The above parameters are not altered by this routine. */

/*       LCC = List of constraint curve starting indexes (or */
/*             dummy array of length 1 if NCC = 0).  Refer to */
/*             Subroutine ADDCST. */

/*       N = Number of nodes in the triangulation on input. */
/*           N .GE. 4.  Note that N will be decremented */
/*           following the deletion. */

/*       X,Y = Arrays of length N containing the coordinates */
/*             of the nodes with non-constraint nodes in the */
/*             first LCC(1)-1 locations if NCC > 0. */

/*       LIST,LPTR,LEND,LNEW = Data structure defining the */
/*                             triangulation.  Refer to Sub- */
/*                             routine TRMESH. */

/*       LWK = Number of columns reserved for IWK.  LWK must */
/*             be at least NNB-3, where NNB is the number of */
/*             neighbors of node K, including an extra */
/*             pseudo-node if K is a boundary node. */

/*       IWK = Integer work array dimensioned 2 by LWK (or */
/*             array of length .GE. 2*LWK). */

/* On output: */

/*       LCC = List of constraint curve starting indexes de- */
/*             cremented by 1 to reflect the deletion of K */
/*             unless NCC = 0 or 1 .LE. IER .LE. 4. */

/*       N = New number of nodes (input value minus one) un- */
/*           less 1 .LE. IER .LE. 4. */

/*       X,Y = Updated arrays of length N-1 containing nodal */
/*             coordinates (with elements K+1,...,N shifted */
/*             up a position and thus overwriting element K) */
/*             unless 1 .LE. IER .LE. 4.  (N here denotes the */
/*             input value.) */

/*       LIST,LPTR,LEND,LNEW = Updated triangulation data */
/*                             structure reflecting the dele- */
/*                             tion unless IER .NE. 0.  Note */
/*                             that the data structure may */
/*                             have been altered if IER .GE. */
/*                             3. */

/*       LWK = Number of IWK columns required unless IER = 1 */
/*             or IER = 3. */

/*       IWK = Indexes of the endpoints of the new arcs added */
/*             unless LWK = 0 or 1 .LE. IER .LE. 4.  (Arcs */
/*             are associated with columns, or pairs of */
/*             adjacent elements if IWK is declared as a */
/*             singly-subscripted array.) */

/*       IER = Error indicator: */
/*             IER = 0 if no errors were encountered. */
/*             IER = 1 if K, NCC, N, or an LCC entry is out- */
/*                     side its valid range or LWK < 0 on */
/*                     input. */
/*             IER = 2 if more space is required in IWK. */
/*                     Refer to LWK. */
/*             IER = 3 if the triangulation data structure is */
/*                     invalid on input. */
/*             IER = 4 if K is an interior node with 4 or */
/*                     more neighbors, and the number of */
/*                     neighbors could not be reduced to 3 */
/*                     by swaps.  This could be caused by */
/*                     floating point errors with collinear */
/*                     nodes or by an invalid data structure. */
/*             IER = 5 if an error flag was returned by */
/*                     OPTIM.  An error message is written */
/*                     to the standard output unit in this */
/*                     event. */

/*   Note that the deletion may result in all remaining nodes */
/* being collinear.  This situation is not flagged. */

/* Modules required by DELNOD:  DELNB, LEFT, LSTPTR, NBCNT, */
/*                                OPTIM, SWAP, SWPTST */

/* Intrinsic function called by DELNOD:  ABS */

/* *********************************************************** */


/* Set N1 to K and NNB to the number of neighbors of N1 (plus */
/*   one if N1 is a boundary node), and test for errors.  LPF */
/*   and LPL are LIST indexes of the first and last neighbors */
/*   of N1, IWL is the number of IWK columns containing arcs, */
/*   and BDRY is TRUE iff N1 is a boundary node. */

    /* Parameter adjustments */
    iwk -= 3;
    --lend;
    --lptr;
    --list;
    --y;
    --x;
    --lcc;

    /* Function Body */
    n1 = *k;
    nn = *n;
    if (*ncc < 0 || n1 < 1 || nn < 4 || *lwk < 0) {
	goto L21;
    }
    lccip1 = nn + 1;
    for (i__ = *ncc; i__ >= 1; --i__) {
	if (lccip1 - lcc[i__] < 3) {
	    goto L21;
	}
	lccip1 = lcc[i__];
/* L1: */
    }
    if (n1 >= lccip1) {
	goto L21;
    }
    lpl = lend[n1];
    lpf = lptr[lpl];
    nnb = nbcnt_(&lpl, &lptr[1]);
    bdry = list[lpl] < 0;
    if (bdry) {
	++nnb;
    }
    if (nnb < 3) {
	goto L23;
    }
    lwkl = *lwk;
    *lwk = nnb - 3;
    if (lwkl < *lwk) {
	goto L22;
    }
    iwl = 0;
    if (nnb == 3) {
	goto L5;
    }

/* Initialize for loop on arcs N1-N2 for neighbors N2 of N1, */
/*   beginning with the second neighbor.  NR and NL are the */
/*   neighbors preceding and following N2, respectively, and */
/*   LP indexes NL.  The loop is exited when all possible */
/*   swaps have been applied to arcs incident on N1.  If N1 */
/*   is interior, the number of neighbors will be reduced */
/*   to 3. */

    x1 = x[n1];
    y1 = y[n1];
    nfrst = list[lpf];
    nr = nfrst;
    xr = x[nr];
    yr = y[nr];
    lp = lptr[lpf];
    n2 = list[lp];
    x2 = x[n2];
    y2 = y[n2];
    lp = lptr[lp];

/* Top of loop:  set NL to the neighbor following N2. */

L2:
    nl = (i__1 = list[lp], abs(i__1));
    if (nl == nfrst && bdry) {
	goto L5;
    }
    xl = x[nl];
    yl = y[nl];

/*   Test for a convex quadrilateral.  To avoid an incorrect */
/*     test caused by collinearity, use the fact that if N1 */
/*     is a boundary node, then N1 LEFT NR->NL and if N2 is */
/*     a boundary node, then N2 LEFT NL->NR. */

    lpl2 = lend[n2];
    if ((bdry || left_(&xr, &yr, &xl, &yl, &x1, &y1)) && (list[lpl2] < 0 || 
	    left_(&xl, &yl, &xr, &yr, &x2, &y2))) {
	goto L3;
    }

/*   Nonconvex quadrilateral -- no swap is possible. */

    nr = n2;
    xr = x2;
    yr = y2;
    goto L4;

/*   The quadrilateral defined by adjacent triangles */
/*     (N1,N2,NL) and (N2,N1,NR) is convex.  Swap in */
/*     NL-NR and store it in IWK.  Indexes larger than N1 */
/*     must be decremented since N1 will be deleted from */
/*     X and Y. */

L3:
    swap_(&nl, &nr, &n1, &n2, &list[1], &lptr[1], &lend[1], &lp21);
    ++iwl;
    if (nl <= n1) {
	iwk[(iwl << 1) + 1] = nl;
    } else {
	iwk[(iwl << 1) + 1] = nl - 1;
    }
    if (nr <= n1) {
	iwk[(iwl << 1) + 2] = nr;
    } else {
	iwk[(iwl << 1) + 2] = nr - 1;
    }

/*   Recompute the LIST indexes LPL,LP and decrement NNB. */

    lpl = lend[n1];
    --nnb;
    if (nnb == 3) {
	goto L5;
    }
    lp = lstptr_(&lpl, &nl, &list[1], &lptr[1]);
    if (nr == nfrst) {
	goto L4;
    }

/*   NR is not the first neighbor of N1. */
/*     Back up and test N1-NR for a swap again:  Set N2 to */
/*     NR and NR to the previous neighbor of N1 -- the */
/*     neighbor of NR which follows N1.  LP21 points to NL */
/*     as a neighbor of NR. */

    n2 = nr;
    x2 = xr;
    y2 = yr;
    lp21 = lptr[lp21];
    lp21 = lptr[lp21];
    nr = (i__1 = list[lp21], abs(i__1));
    xr = x[nr];
    yr = y[nr];
    goto L2;

/*   Bottom of loop -- test for invalid termination. */

L4:
    if (n2 == nfrst) {
	goto L24;
    }
    n2 = nl;
    x2 = xl;
    y2 = yl;
    lp = lptr[lp];
    goto L2;

/* Delete N1 from the adjacency list of N2 for all neighbors */
/*   N2 of N1.  LPL points to the last neighbor of N1. */
/*   LNEW is stored in local variable LNW. */

L5:
    lp = lpl;
    lnw = *lnew;

/* Loop on neighbors N2 of N1, beginning with the first. */

L6:
    lp = lptr[lp];
    n2 = (i__1 = list[lp], abs(i__1));
    delnb_(&n2, &n1, n, &list[1], &lptr[1], &lend[1], &lnw, &lph);
    if (lph < 0) {
	goto L23;
    }

/*   LP and LPL may require alteration. */

    if (lpl == lnw) {
	lpl = lph;
    }
    if (lp == lnw) {
	lp = lph;
    }
    if (lp != lpl) {
	goto L6;
    }

/* Delete N1 from X, Y, and LEND, and remove its adjacency */
/*   list from LIST and LPTR.  LIST entries (nodal indexes) */
/*   which are larger than N1 must be decremented. */

    --nn;
    if (n1 > nn) {
	goto L9;
    }
    i__1 = nn;
    for (i__ = n1; i__ <= i__1; ++i__) {
	x[i__] = x[i__ + 1];
	y[i__] = y[i__ + 1];
	lend[i__] = lend[i__ + 1];
/* L7: */
    }

    i__1 = lnw - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (list[i__] > n1) {
	    --list[i__];
	}
	if (list[i__] < -n1) {
	    ++list[i__];
	}
/* L8: */
    }

/*   For LPN = first to last neighbors of N1, delete the */
/*     preceding neighbor (indexed by LP). */

/*   Each empty LIST,LPTR location LP is filled in with the */
/*     values at LNW-1, and LNW is decremented.  All pointers */
/*     (including those in LPTR and LEND) with value LNW-1 */
/*     must be changed to LP. */

/*  LPL points to the last neighbor of N1. */

L9:
    if (bdry) {
	--nnb;
    }
    lpn = lpl;
    i__1 = nnb;
    for (j = 1; j <= i__1; ++j) {
	--lnw;
	lp = lpn;
	lpn = lptr[lp];
	list[lp] = list[lnw];
	lptr[lp] = lptr[lnw];
	if (lptr[lpn] == lnw) {
	    lptr[lpn] = lp;
	}
	if (lpn == lnw) {
	    lpn = lp;
	}
	for (i__ = nn; i__ >= 1; --i__) {
	    if (lend[i__] == lnw) {
		lend[i__] = lp;
		goto L11;
	    }
/* L10: */
	}

L11:
	for (i__ = lnw - 1; i__ >= 1; --i__) {
	    if (lptr[i__] == lnw) {
		lptr[i__] = lp;
	    }
/* L12: */
	}
/* L13: */
    }

/* Decrement LCC entries. */

    i__1 = *ncc;
    for (i__ = 1; i__ <= i__1; ++i__) {
	--lcc[i__];
/* L14: */
    }

/* Update N and LNEW, and optimize the patch of triangles */
/*   containing K (on input) by applying swaps to the arcs */
/*   in IWK. */

    *n = nn;
    *lnew = lnw;
    if (iwl > 0) {
	nit = iwl << 2;
	optim_(&x[1], &y[1], &iwl, &list[1], &lptr[1], &lend[1], &nit, &iwk[3]
		, &ierr);
	if (ierr != 0) {
	    goto L25;
	}
    }

/* Successful termination. */

    *ier = 0;
    return 0;

/* Invalid input parameter. */

L21:
    *ier = 1;
    return 0;

/* Insufficient space reserved for IWK. */

L22:
    *ier = 2;
    return 0;

/* Invalid triangulation data structure.  NNB < 3 on input or */
/*   N2 is a neighbor of N1 but N1 is not a neighbor of N2. */

L23:
    *ier = 3;
    return 0;

/* K is an interior node with 4 or more neighbors, but the */
/*   number of neighbors could not be reduced. */

L24:
    *ier = 4;
    return 0;

/* Error flag returned by OPTIM. */

L25:
    *ier = 5;
/*     WRITE (*,100) NIT, IERR */
    return 0;
/* L100: */
} /* delnod_ */

/* Subroutine */ int edge_(integer *in1, integer *in2, doublereal *x, 
	doublereal *y, integer *lwk, integer *iwk, integer *list, integer *
	lptr, integer *lend, integer *ier)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, n0, n1, n2;
    doublereal x0, y0, x1, y1, x2, y2;
    integer nl, lp;
    doublereal dx, dy;
    integer nr, lp21, iwc, iwf, lft, lpl, iwl, nit;
    extern logical left_(doublereal *, doublereal *, doublereal *, doublereal 
	    *, doublereal *, doublereal *);
    integer ierr;
    extern /* Subroutine */ int swap_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *);
    integer next, iwcp1, n1lst, iwend;
    extern /* Subroutine */ int optim_(doublereal *, doublereal *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *);
    integer n1frst;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   06/23/98 */

/*   Given a triangulation of N nodes and a pair of nodal */
/* indexes IN1 and IN2, this routine swaps arcs as necessary */
/* to force IN1 and IN2 to be adjacent.  Only arcs which */
/* intersect IN1-IN2 are swapped out.  If a Delaunay triangu- */
/* lation is input, the resulting triangulation is as close */
/* as possible to a Delaunay triangulation in the sense that */
/* all arcs other than IN1-IN2 are locally optimal. */

/*   A sequence of calls to EDGE may be used to force the */
/* presence of a set of edges defining the boundary of a non- */
/* convex and/or multiply connected region (refer to Subrou- */
/* tine ADDCST), or to introduce barriers into the triangula- */
/* tion.  Note that Subroutine GETNP will not necessarily */
/* return closest nodes if the triangulation has been con- */
/* strained by a call to EDGE.  However, this is appropriate */
/* in some applications, such as triangle-based interpolation */
/* on a nonconvex domain. */


/* On input: */

/*       IN1,IN2 = Indexes (of X and Y) in the range 1 to N */
/*                 defining a pair of nodes to be connected */
/*                 by an arc. */

/*       X,Y = Arrays of length N containing the Cartesian */
/*             coordinates of the nodes. */

/* The above parameters are not altered by this routine. */

/*       LWK = Number of columns reserved for IWK.  This must */
/*             be at least NI -- the number of arcs which */
/*             intersect IN1-IN2.  (NI is bounded by N-3.) */

/*       IWK = Integer work array of length at least 2*LWK. */

/*       LIST,LPTR,LEND = Data structure defining the trian- */
/*                        gulation.  Refer to Subroutine */
/*                        TRMESH. */

/* On output: */

/*       LWK = Number of arcs which intersect IN1-IN2 (but */
/*             not more than the input value of LWK) unless */
/*             IER = 1 or IER = 3.  LWK = 0 if and only if */
/*             IN1 and IN2 were adjacent (or LWK=0) on input. */

/*       IWK = Array containing the indexes of the endpoints */
/*             of the new arcs other than IN1-IN2 unless IER */
/*             .GT. 0 or LWK = 0.  New arcs to the left of */
/*             IN2-IN1 are stored in the first K-1 columns */
/*             (left portion of IWK), column K contains */
/*             zeros, and new arcs to the right of IN2-IN1 */
/*             occupy columns K+1,...,LWK.  (K can be deter- */
/*             mined by searching IWK for the zeros.) */

/*       LIST,LPTR,LEND = Data structure updated if necessary */
/*                        to reflect the presence of an arc */
/*                        connecting IN1 and IN2 unless IER */
/*                        .NE. 0.  The data structure has */
/*                        been altered if IER = 4. */

/*       IER = Error indicator: */
/*             IER = 0 if no errors were encountered. */
/*             IER = 1 if IN1 .LT. 1, IN2 .LT. 1, IN1 = IN2, */
/*                     or LWK .LT. 0 on input. */
/*             IER = 2 if more space is required in IWK. */
/*             IER = 3 if IN1 and IN2 could not be connected */
/*                     due to either an invalid data struc- */
/*                     ture or collinear nodes (and floating */
/*                     point error). */
/*             IER = 4 if an error flag was returned by */
/*                     OPTIM. */

/*   An error message is written to the standard output unit */
/* in the case of IER = 3 or IER = 4. */

/* Modules required by EDGE:  LEFT, LSTPTR, OPTIM, SWAP, */
/*                              SWPTST */

/* Intrinsic function called by EDGE:  ABS */

/* *********************************************************** */


/* Local parameters: */

/* DX,DY =   Components of arc N1-N2 */
/* I =       DO-loop index and column index for IWK */
/* IERR =    Error flag returned by Subroutine OPTIM */
/* IWC =     IWK index between IWF and IWL -- NL->NR is */
/*             stored in IWK(1,IWC)->IWK(2,IWC) */
/* IWCP1 =   IWC + 1 */
/* IWEND =   Input or output value of LWK */
/* IWF =     IWK (column) index of the first (leftmost) arc */
/*             which intersects IN1->IN2 */
/* IWL =     IWK (column) index of the last (rightmost) are */
/*             which intersects IN1->IN2 */
/* LFT =     Flag used to determine if a swap results in the */
/*             new arc intersecting IN1-IN2 -- LFT = 0 iff */
/*             N0 = IN1, LFT = -1 implies N0 LEFT IN1->IN2, */
/*             and LFT = 1 implies N0 LEFT IN2->IN1 */
/* LP21 =    Unused parameter returned by SWAP */
/* LP =      List pointer (index) for LIST and LPTR */
/* LPL =     Pointer to the last neighbor of IN1 or NL */
/* N0 =      Neighbor of N1 or node opposite NR->NL */
/* N1,N2 =   Local copies of IN1 and IN2 */
/* N1FRST =  First neighbor of IN1 */
/* N1LST =   (Signed) last neighbor of IN1 */
/* NEXT =    Node opposite NL->NR */
/* NIT =     Flag or number of iterations employed by OPTIM */
/* NL,NR =   Endpoints of an arc which intersects IN1-IN2 */
/*             with NL LEFT IN1->IN2 */
/* X0,Y0 =   Coordinates of N0 */
/* X1,Y1 =   Coordinates of IN1 */
/* X2,Y2 =   Coordinates of IN2 */


/* Store IN1, IN2, and LWK in local variables and test for */
/*   errors. */

    /* Parameter adjustments */
    --lend;
    --lptr;
    --list;
    iwk -= 3;
    --y;
    --x;

    /* Function Body */
    n1 = *in1;
    n2 = *in2;
    iwend = *lwk;
    if (n1 < 1 || n2 < 1 || n1 == n2 || iwend < 0) {
	goto L31;
    }

/* Test for N2 as a neighbor of N1.  LPL points to the last */
/*   neighbor of N1. */

    lpl = lend[n1];
    n0 = (i__1 = list[lpl], abs(i__1));
    lp = lpl;
L1:
    if (n0 == n2) {
	goto L30;
    }
    lp = lptr[lp];
    n0 = list[lp];
    if (lp != lpl) {
	goto L1;
    }

/* Initialize parameters. */

    iwl = 0;
    nit = 0;

/* Store the coordinates of N1 and N2. */

L2:
    x1 = x[n1];
    y1 = y[n1];
    x2 = x[n2];
    y2 = y[n2];

/* Set NR and NL to adjacent neighbors of N1 such that */
/*   NR LEFT N2->N1 and NL LEFT N1->N2, */
/*   (NR Forward N1->N2 or NL Forward N1->N2), and */
/*   (NR Forward N2->N1 or NL Forward N2->N1). */

/*   Initialization:  Set N1FRST and N1LST to the first and */
/*     (signed) last neighbors of N1, respectively, and */
/*     initialize NL to N1FRST. */

    lpl = lend[n1];
    n1lst = list[lpl];
    lp = lptr[lpl];
    n1frst = list[lp];
    nl = n1frst;
    if (n1lst < 0) {
	goto L4;
    }

/*   N1 is an interior node.  Set NL to the first candidate */
/*     for NR (NL LEFT N2->N1). */

L3:
    if (left_(&x2, &y2, &x1, &y1, &x[nl], &y[nl])) {
	goto L4;
    }
    lp = lptr[lp];
    nl = list[lp];
    if (nl != n1frst) {
	goto L3;
    }

/*   All neighbors of N1 are strictly left of N1->N2. */

    goto L5;

/*   NL = LIST(LP) LEFT N2->N1.  Set NR to NL and NL to the */
/*     following neighbor of N1. */

L4:
    nr = nl;
    lp = lptr[lp];
    nl = (i__1 = list[lp], abs(i__1));
    if (left_(&x1, &y1, &x2, &y2, &x[nl], &y[nl])) {

/*   NL LEFT N1->N2 and NR LEFT N2->N1.  The Forward tests */
/*     are employed to avoid an error associated with */
/*     collinear nodes. */

	dx = x2 - x1;
	dy = y2 - y1;
	if ((dx * (x[nl] - x1) + dy * (y[nl] - y1) >= 0. || dx * (x[nr] - x1) 
		+ dy * (y[nr] - y1) >= 0.) && (dx * (x[nl] - x2) + dy * (y[nl]
		 - y2) <= 0. || dx * (x[nr] - x2) + dy * (y[nr] - y2) <= 0.)) 
		{
	    goto L6;
	}

/*   NL-NR does not intersect N1-N2.  However, there is */
/*     another candidate for the first arc if NL lies on */
/*     the line N1-N2. */

	if (! left_(&x2, &y2, &x1, &y1, &x[nl], &y[nl])) {
	    goto L5;
	}
    }

/*   Bottom of loop. */

    if (nl != n1frst) {
	goto L4;
    }

/* Either the triangulation is invalid or N1-N2 lies on the */
/*   convex hull boundary and an edge NR->NL (opposite N1 and */
/*   intersecting N1-N2) was not found due to floating point */
/*   error.  Try interchanging N1 and N2 -- NIT > 0 iff this */
/*   has already been done. */

L5:
    if (nit > 0) {
	goto L33;
    }
    nit = 1;
    n1 = n2;
    n2 = *in1;
    goto L2;

/* Store the ordered sequence of intersecting edges NL->NR in */
/*   IWK(1,IWL)->IWK(2,IWL). */

L6:
    ++iwl;
    if (iwl > iwend) {
	goto L32;
    }
    iwk[(iwl << 1) + 1] = nl;
    iwk[(iwl << 1) + 2] = nr;

/*   Set NEXT to the neighbor of NL which follows NR. */

    lpl = lend[nl];
    lp = lptr[lpl];

/*   Find NR as a neighbor of NL.  The search begins with */
/*     the first neighbor. */

L7:
    if (list[lp] == nr) {
	goto L8;
    }
    lp = lptr[lp];
    if (lp != lpl) {
	goto L7;
    }

/*   NR must be the last neighbor, and NL->NR cannot be a */
/*     boundary edge. */

    if (list[lp] != nr) {
	goto L33;
    }

/*   Set NEXT to the neighbor following NR, and test for */
/*     termination of the store loop. */

L8:
    lp = lptr[lp];
    next = (i__1 = list[lp], abs(i__1));
    if (next == n2) {
	goto L9;
    }

/*   Set NL or NR to NEXT. */

    if (left_(&x1, &y1, &x2, &y2, &x[next], &y[next])) {
	nl = next;
    } else {
	nr = next;
    }
    goto L6;

/* IWL is the number of arcs which intersect N1-N2. */
/*   Store LWK. */

L9:
    *lwk = iwl;
    iwend = iwl;

/* Initialize for edge swapping loop -- all possible swaps */
/*   are applied (even if the new arc again intersects */
/*   N1-N2), arcs to the left of N1->N2 are stored in the */
/*   left portion of IWK, and arcs to the right are stored in */
/*   the right portion.  IWF and IWL index the first and last */
/*   intersecting arcs. */

    iwf = 1;

/* Top of loop -- set N0 to N1 and NL->NR to the first edge. */
/*   IWC points to the arc currently being processed.  LFT */
/*   .LE. 0 iff N0 LEFT N1->N2. */

L10:
    lft = 0;
    n0 = n1;
    x0 = x1;
    y0 = y1;
    nl = iwk[(iwf << 1) + 1];
    nr = iwk[(iwf << 1) + 2];
    iwc = iwf;

/*   Set NEXT to the node opposite NL->NR unless IWC is the */
/*     last arc. */

L11:
    if (iwc == iwl) {
	goto L21;
    }
    iwcp1 = iwc + 1;
    next = iwk[(iwcp1 << 1) + 1];
    if (next != nl) {
	goto L16;
    }
    next = iwk[(iwcp1 << 1) + 2];

/*   NEXT RIGHT N1->N2 and IWC .LT. IWL.  Test for a possible */
/*     swap. */

    if (! left_(&x0, &y0, &x[nr], &y[nr], &x[next], &y[next])) {
	goto L14;
    }
    if (lft >= 0) {
	goto L12;
    }
    if (! left_(&x[nl], &y[nl], &x0, &y0, &x[next], &y[next])) {
	goto L14;
    }

/*   Replace NL->NR with N0->NEXT. */

    swap_(&next, &n0, &nl, &nr, &list[1], &lptr[1], &lend[1], &lp21);
    iwk[(iwc << 1) + 1] = n0;
    iwk[(iwc << 1) + 2] = next;
    goto L15;

/*   Swap NL-NR for N0-NEXT, shift columns IWC+1,...,IWL to */
/*     the left, and store N0-NEXT in the right portion of */
/*     IWK. */

L12:
    swap_(&next, &n0, &nl, &nr, &list[1], &lptr[1], &lend[1], &lp21);
    i__1 = iwl;
    for (i__ = iwcp1; i__ <= i__1; ++i__) {
	iwk[(i__ - 1 << 1) + 1] = iwk[(i__ << 1) + 1];
	iwk[(i__ - 1 << 1) + 2] = iwk[(i__ << 1) + 2];
/* L13: */
    }
    iwk[(iwl << 1) + 1] = n0;
    iwk[(iwl << 1) + 2] = next;
    --iwl;
    nr = next;
    goto L11;

/*   A swap is not possible.  Set N0 to NR. */

L14:
    n0 = nr;
    x0 = x[n0];
    y0 = y[n0];
    lft = 1;

/*   Advance to the next arc. */

L15:
    nr = next;
    ++iwc;
    goto L11;

/*   NEXT LEFT N1->N2, NEXT .NE. N2, and IWC .LT. IWL. */
/*     Test for a possible swap. */

L16:
    if (! left_(&x[nl], &y[nl], &x0, &y0, &x[next], &y[next])) {
	goto L19;
    }
    if (lft <= 0) {
	goto L17;
    }
    if (! left_(&x0, &y0, &x[nr], &y[nr], &x[next], &y[next])) {
	goto L19;
    }

/*   Replace NL->NR with NEXT->N0. */

    swap_(&next, &n0, &nl, &nr, &list[1], &lptr[1], &lend[1], &lp21);
    iwk[(iwc << 1) + 1] = next;
    iwk[(iwc << 1) + 2] = n0;
    goto L20;

/*   Swap NL-NR for N0-NEXT, shift columns IWF,...,IWC-1 to */
/*     the right, and store N0-NEXT in the left portion of */
/*     IWK. */

L17:
    swap_(&next, &n0, &nl, &nr, &list[1], &lptr[1], &lend[1], &lp21);
    i__1 = iwf;
    for (i__ = iwc - 1; i__ >= i__1; --i__) {
	iwk[(i__ + 1 << 1) + 1] = iwk[(i__ << 1) + 1];
	iwk[(i__ + 1 << 1) + 2] = iwk[(i__ << 1) + 2];
/* L18: */
    }
    iwk[(iwf << 1) + 1] = n0;
    iwk[(iwf << 1) + 2] = next;
    ++iwf;
    goto L20;

/*   A swap is not possible.  Set N0 to NL. */

L19:
    n0 = nl;
    x0 = x[n0];
    y0 = y[n0];
    lft = -1;

/*   Advance to the next arc. */

L20:
    nl = next;
    ++iwc;
    goto L11;

/*   N2 is opposite NL->NR (IWC = IWL). */

L21:
    if (n0 == n1) {
	goto L24;
    }
    if (lft < 0) {
	goto L22;
    }

/*   N0 RIGHT N1->N2.  Test for a possible swap. */

    if (! left_(&x0, &y0, &x[nr], &y[nr], &x2, &y2)) {
	goto L10;
    }

/*   Swap NL-NR for N0-N2 and store N0-N2 in the right */
/*     portion of IWK. */

    swap_(&n2, &n0, &nl, &nr, &list[1], &lptr[1], &lend[1], &lp21);
    iwk[(iwl << 1) + 1] = n0;
    iwk[(iwl << 1) + 2] = n2;
    --iwl;
    goto L10;

/*   N0 LEFT N1->N2.  Test for a possible swap. */

L22:
    if (! left_(&x[nl], &y[nl], &x0, &y0, &x2, &y2)) {
	goto L10;
    }

/*   Swap NL-NR for N0-N2, shift columns IWF,...,IWL-1 to the */
/*     right, and store N0-N2 in the left portion of IWK. */

    swap_(&n2, &n0, &nl, &nr, &list[1], &lptr[1], &lend[1], &lp21);
    i__ = iwl;
L23:
    iwk[(i__ << 1) + 1] = iwk[(i__ - 1 << 1) + 1];
    iwk[(i__ << 1) + 2] = iwk[(i__ - 1 << 1) + 2];
    --i__;
    if (i__ > iwf) {
	goto L23;
    }
    iwk[(iwf << 1) + 1] = n0;
    iwk[(iwf << 1) + 2] = n2;
    ++iwf;
    goto L10;

/* IWF = IWC = IWL.  Swap out the last arc for N1-N2 and */
/*   store zeros in IWK. */

L24:
    swap_(&n2, &n1, &nl, &nr, &list[1], &lptr[1], &lend[1], &lp21);
    iwk[(iwc << 1) + 1] = 0;
    iwk[(iwc << 1) + 2] = 0;

/* Optimization procedure -- */

    if (iwc > 1) {

/*   Optimize the set of new arcs to the left of IN1->IN2. */

	nit = (iwc - 1) * 3;
	i__1 = iwc - 1;
	optim_(&x[1], &y[1], &i__1, &list[1], &lptr[1], &lend[1], &nit, &iwk[
		3], &ierr);
	if (ierr != 0) {
	    goto L34;
	}
    }
    if (iwc < iwend) {

/*   Optimize the set of new arcs to the right of IN1->IN2. */

	nit = (iwend - iwc) * 3;
	i__1 = iwend - iwc;
	optim_(&x[1], &y[1], &i__1, &list[1], &lptr[1], &lend[1], &nit, &iwk[(
		iwc + 1 << 1) + 1], &ierr);
	if (ierr != 0) {
	    goto L34;
	}
    }

/* Successful termination. */

    *ier = 0;
    return 0;

/* IN1 and IN2 were adjacent on input. */

L30:
    *ier = 0;
    return 0;

/* Invalid input parameter. */

L31:
    *ier = 1;
    return 0;

/* Insufficient space reserved for IWK. */

L32:
    *ier = 2;
    return 0;

/* Invalid triangulation data structure or collinear nodes */
/*   on convex hull boundary. */

L33:
    *ier = 3;
/*     WRITE (*,130) IN1, IN2 */
/* L130: */
    return 0;

/* Error flag returned by OPTIM. */

L34:
    *ier = 4;
/*     WRITE (*,140) NIT, IERR */
/* L140: */
    return 0;
} /* edge_ */

/* Subroutine */ int getnp_(integer *ncc, integer *lcc, integer *n, 
	doublereal *x, doublereal *y, integer *list, integer *lptr, integer *
	lend, integer *l, integer *npts, doublereal *ds, integer *ier)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    integer i__, j, k, n1;
    doublereal x1, y1, dc, dl;
    integer nc, nj, nk, nl;
    doublereal xc;
    integer lp, nn;
    doublereal yc, xj, xk, yj, yk;
    integer nf1, nf2, km1, lm1;
    logical ncf, njf;
    integer lpk;
    logical vis, isw;
    integer lcc1;
    logical lft1, lft2, lft12;
    integer lpcl, lpkl;
    logical skip;
    integer nkbak, ilast, nkfor, ifrst;
    logical sksav;
    extern logical intsec_(doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *);


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   11/12/94 */

/*   Given a triangulation of N nodes and an array NPTS con- */
/* taining the indexes of L-1 nodes ordered by distance from */
/* NPTS(1), this subroutine sets NPTS(L) to the index of the */
/* next node in the sequence -- the node, other than NPTS(1), */
/* ...,NPTS(L-1), which is closest to NPTS(1).  Thus, the */
/* ordered sequence of K closest nodes to N1 (including N1) */
/* may be determined by K-1 calls to GETNP with NPTS(1) = N1 */
/* and L = 2,3,...,K for K .GE. 2.  Note that NPTS must in- */
/* clude constraint nodes as well as non-constraint nodes. */
/* Thus, a sequence of K1 closest non-constraint nodes to N1 */
/* must be obtained as a subset of the closest K2 nodes to N1 */
/* for some K2 .GE. K1. */

/*   The terms closest and distance have special definitions */
/* when constraint nodes are present in the triangulation. */
/* Nodes N1 and N2 are said to be visible from each other if */
/* and only if the line segment N1-N2 intersects no con- */
/* straint arc (except possibly itself) and is not an interi- */
/* or constraint arc (arc whose interior lies in a constraint */
/* region).  A path from N1 to N2 is an ordered sequence of */
/* nodes, with N1 first and N2 last, such that adjacent path */
/* elements are visible from each other.  The path length is */
/* the sum of the Euclidean distances between adjacent path */
/* nodes.  Finally, the distance from N1 to N2 is defined to */
/* be the length of the shortest path from N1 to N2. */

/*   The algorithm uses the property of a Delaunay triangula- */
/* tion that the K-th closest node to N1 is a neighbor of one */
/* of the K-1 closest nodes to N1.  With the definition of */
/* distance used here, this property holds when constraints */
/* are present as long as non-constraint arcs are locally */
/* optimal. */


/* On input: */

/*       NCC = Number of constraints.  NCC .GE. 0. */

/*       LCC = List of constraint curve starting indexes (or */
/*             dummy array of length 1 if NCC = 0).  Refer to */
/*             Subroutine ADDCST. */

/*       N = Number of nodes in the triangulation.  N .GE. 3. */

/*       X,Y = Arrays of length N containing the coordinates */
/*             of the nodes with non-constraint nodes in the */
/*             first LCC(1)-1 locations if NCC > 0. */

/*       LIST,LPTR,LEND = Triangulation data structure.  Re- */
/*                        fer to Subroutine TRMESH. */

/*       L = Number of nodes in the sequence on output.  2 */
/*           .LE. L .LE. N. */

/*       NPTS = Array of length .GE. L containing the indexes */
/*              of the L-1 closest nodes to NPTS(1) in the */
/*              first L-1 locations. */

/*       DS = Array of length .GE. L containing the distance */
/*            (defined above) between NPTS(1) and NPTS(I) in */
/*            the I-th position for I = 1,...,L-1.  Thus, */
/*            DS(1) = 0. */

/* Input parameters other than NPTS(L) and DS(L) are not */
/*   altered by this routine. */

/* On output: */

/*       NPTS = Array updated with the index of the L-th */
/*              closest node to NPTS(1) in position L unless */
/*              IER .NE. 0. */

/*       DS = Array updated with the distance between NPTS(1) */
/*            and NPTS(L) in position L unless IER .NE. 0. */

/*       IER = Error indicator: */
/*             IER =  0 if no errors were encountered. */
/*             IER = -1 if NCC, N, L, or an LCC entry is */
/*                      outside its valid range on input. */
/*             IER =  K if NPTS(K) is not a valid index in */
/*                      the range 1 to N. */

/* Module required by GETNP:  INTSEC */

/* Intrinsic functions called by GETNP:  ABS, MIN, SQRT */

/* *********************************************************** */


/* Store parameters in local variables and test for errors. */
/*   LCC1 indexes the first constraint node. */

    /* Parameter adjustments */
    --lcc;
    --lend;
    --y;
    --x;
    --list;
    --lptr;
    --ds;
    --npts;

    /* Function Body */
    *ier = -1;
    nn = *n;
    lcc1 = nn + 1;
    lm1 = *l - 1;
    if (*ncc < 0 || lm1 < 1 || lm1 >= nn) {
	return 0;
    }
    if (*ncc == 0) {
	if (nn < 3) {
	    return 0;
	}
    } else {
	for (i__ = *ncc; i__ >= 1; --i__) {
	    if (lcc1 - lcc[i__] < 3) {
		return 0;
	    }
	    lcc1 = lcc[i__];
/* L1: */
	}
	if (lcc1 < 1) {
	    return 0;
	}
    }

/* Test for an invalid index in NPTS. */

    i__1 = lm1;
    for (k = 1; k <= i__1; ++k) {
	nk = npts[k];
	if (nk < 1 || nk > nn) {
	    *ier = k;
	    return 0;
	}
/* L2: */
    }

/* Store N1 = NPTS(1) and mark the elements of NPTS. */

    n1 = npts[1];
    x1 = x[n1];
    y1 = y[n1];
    i__1 = lm1;
    for (k = 1; k <= i__1; ++k) {
	nk = npts[k];
	lend[nk] = -lend[nk];
/* L3: */
    }

/* Candidates NC for NL = NPTS(L) are the unmarked visible */
/*   neighbors of nodes NK in NPTS.  ISW is an initialization */
/*   switch set to .TRUE. when NL and its distance DL from N1 */
/*   have been initialized with the first candidate encount- */
/*   ered. */

    isw = FALSE_;
    dl = 0.;

/* Loop on marked nodes NK = NPTS(K).  LPKL indexes the last */
/*   neighbor of NK in LIST. */

    i__1 = lm1;
    for (k = 1; k <= i__1; ++k) {
	km1 = k - 1;
	nk = npts[k];
	xk = x[nk];
	yk = y[nk];
	lpkl = -lend[nk];
	nkfor = 0;
	nkbak = 0;
	vis = TRUE_;
	if (nk >= lcc1) {

/*   NK is a constraint node.  Set NKFOR and NKBAK to the */
/*     constraint nodes which follow and precede NK.  IFRST */
/*     and ILAST are set to the first and last nodes in the */
/*     constraint containing NK. */

	    ifrst = nn + 1;
	    for (i__ = *ncc; i__ >= 1; --i__) {
		ilast = ifrst - 1;
		ifrst = lcc[i__];
		if (nk >= ifrst) {
		    goto L5;
		}
/* L4: */
	    }

L5:
	    if (nk < ilast) {
		nkfor = nk + 1;
	    } else {
		nkfor = ifrst;
	    }
	    if (nk > ifrst) {
		nkbak = nk - 1;
	    } else {
		nkbak = ilast;
	    }

/*   Initialize VIS to TRUE iff NKFOR precedes NKBAK in the */
/*     adjacency list for NK -- the first neighbor is visi- */
/*     ble and is not NKBAK. */

	    lpk = lpkl;
L6:
	    lpk = lptr[lpk];
	    nc = (i__2 = list[lpk], abs(i__2));
	    if (nc != nkfor && nc != nkbak) {
		goto L6;
	    }
	    vis = nc == nkfor;
	}

/* Loop on neighbors NC of NK, bypassing marked and nonvis- */
/*   ible neighbors. */

	lpk = lpkl;
L7:
	lpk = lptr[lpk];
	nc = (i__2 = list[lpk], abs(i__2));
	if (nc == nkbak) {
	    vis = TRUE_;
	}

/*   VIS = .FALSE. iff NK-NC is an interior constraint arc */
/*     (NK is a constraint node and NC lies strictly between */
/*     NKFOR and NKBAK). */

	if (! vis) {
	    goto L15;
	}
	if (nc == nkfor) {
	    vis = FALSE_;
	}
	if (lend[nc] < 0) {
	    goto L15;
	}

/* Initialize distance DC between N1 and NC to Euclidean */
/*   distance. */

	xc = x[nc];
	yc = y[nc];
	dc = sqrt((xc - x1) * (xc - x1) + (yc - y1) * (yc - y1));
	if (isw && dc >= dl) {
	    goto L15;
	}
	if (k == 1) {
	    goto L14;
	}

/* K .GE. 2.  Store the pointer LPCL to the last neighbor */
/*   of NC. */

	lpcl = lend[nc];

/* Set DC to the length of the shortest path from N1 to NC */
/*   which has not previously been encountered and which is */
/*   a viable candidate for the shortest path from N1 to NL. */
/*   This is Euclidean distance iff NC is visible from N1. */
/*   Since the shortest path from N1 to NL contains only ele- */
/*   ments of NPTS which are constraint nodes (in addition to */
/*   N1 and NL), only these need be considered for the path */
/*   from N1 to NC.  Thus, for distance function D(A,B) and */
/*   J = 1,...,K, DC = min(D(N1,NJ) + D(NJ,NC)) over con- */
/*   straint nodes NJ = NPTS(J) which are visible from NC. */

	i__2 = km1;
	for (j = 1; j <= i__2; ++j) {
	    nj = npts[j];
	    if (j > 1 && nj < lcc1) {
		goto L13;
	    }

/* If NC is a visible neighbor of NJ, a path from N1 to NC */
/*   containing NJ has already been considered.  Thus, NJ may */
/*   be bypassed if it is adjacent to NC. */

	    lp = lpcl;
L8:
	    lp = lptr[lp];
	    if (nj == (i__3 = list[lp], abs(i__3))) {
		goto L12;
	    }
	    if (lp != lpcl) {
		goto L8;
	    }

/* NJ is a constraint node (unless J=1) not adjacent to NC, */
/*   and is visible from NC iff NJ-NC is not intersected by */
/*   a constraint arc.  Loop on constraints I in reverse */
/*   order -- */

	    xj = x[nj];
	    yj = y[nj];
	    ifrst = nn + 1;
	    for (i__ = *ncc; i__ >= 1; --i__) {
		ilast = ifrst - 1;
		ifrst = lcc[i__];
		nf1 = ilast;
		ncf = nf1 == nc;
		njf = nf1 == nj;
		skip = ncf || njf;

/* Loop on boundary constraint arcs NF1-NF2 which contain */
/*   neither NC nor NJ.  NCF and NJF are TRUE iff NC (or NJ) */
/*   has been encountered in the constraint, and SKIP = */
/*   .TRUE. iff NF1 = NC or NF1 = NJ. */

		i__3 = ilast;
		for (nf2 = ifrst; nf2 <= i__3; ++nf2) {
		    if (nf2 == nc) {
			ncf = TRUE_;
		    }
		    if (nf2 == nj) {
			njf = TRUE_;
		    }
		    sksav = skip;
		    skip = nf2 == nc || nf2 == nj;

/*   The last constraint arc in the constraint need not be */
/*     tested if none of the arcs have been skipped. */

		    if (sksav || skip || nf2 == ilast && ! ncf && ! njf) {
			goto L9;
		    }
		    if (intsec_(&x[nf1], &y[nf1], &x[nf2], &y[nf2], &xc, &yc, 
			    &xj, &yj)) {
			goto L12;
		    }
L9:
		    nf1 = nf2;
/* L10: */
		}
		if (! ncf || ! njf) {
		    goto L11;
		}

/* NC and NJ are constraint nodes in the same constraint. */
/*   NC-NJ is intersected by an interior constraint arc iff */
/*   1)  NC LEFT NF2->NF1 and (NJ LEFT NF1->NC and NJ LEFT */
/*         NC->NF2) or */
/*   2)  NC .NOT. LEFT NF2->NF1 and (NJ LEFT NF1->NC or */
/*         NJ LEFT NC->NF2), */
/*   where NF1, NC, NF2 are consecutive constraint nodes. */

		if (nc != ifrst) {
		    nf1 = nc - 1;
		} else {
		    nf1 = ilast;
		}
		if (nc != ilast) {
		    nf2 = nc + 1;
		} else {
		    nf2 = ifrst;
		}
		lft1 = (xc - x[nf1]) * (yj - y[nf1]) >= (xj - x[nf1]) * (yc - 
			y[nf1]);
		lft2 = (x[nf2] - xc) * (yj - yc) >= (xj - xc) * (y[nf2] - yc);
		lft12 = (x[nf1] - x[nf2]) * (yc - y[nf2]) >= (xc - x[nf2]) * (
			y[nf1] - y[nf2]);
		if (lft1 && lft2 || ! lft12 && (lft1 || lft2)) {
		    goto L12;
		}
L11:
		;
	    }

/* NJ is visible from NC.  Exit the loop with DC = Euclidean */
/*   distance if J = 1. */

	    if (j == 1) {
		goto L14;
	    }
/* Computing MIN */
	    d__1 = dc, d__2 = ds[j] + sqrt((xc - xj) * (xc - xj) + (yc - yj) *
		     (yc - yj));
	    dc = min(d__1,d__2);
	    goto L13;

/* NJ is not visible from NC or is adjacent to NC.  Initial- */
/*   ize DC with D(N1,NK) + D(NK,NC) if J = 1. */

L12:
	    if (j == 1) {
		dc = ds[k] + sqrt((xc - xk) * (xc - xk) + (yc - yk) * (yc - 
			yk));
	    }
L13:
	    ;
	}

/* Compare DC with DL. */

	if (isw && dc >= dl) {
	    goto L15;
	}

/* The first (or a closer) candidate for NL has been */
/*   encountered. */

L14:
	nl = nc;
	dl = dc;
	isw = TRUE_;
L15:
	if (lpk != lpkl) {
	    goto L7;
	}
/* L16: */
    }

/* Unmark the elements of NPTS and store NL and DL. */

    i__1 = lm1;
    for (k = 1; k <= i__1; ++k) {
	nk = npts[k];
	lend[nk] = -lend[nk];
/* L17: */
    }
    npts[*l] = nl;
    ds[*l] = dl;
    *ier = 0;
    return 0;
} /* getnp_ */

integer indxcc_(integer *ncc, integer *lcc, integer *n, integer *list, 
	integer *lend)
{
    /* System generated locals */
    integer ret_val;

    /* Local variables */
    integer i__, n0, lp, nst, nxt, ilast, ifrst;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   08/25/91 */

/*   Given a constrained Delaunay triangulation, this func- */
/* tion returns the index, if any, of an exterior constraint */
/* curve (an unbounded constraint region).  An exterior con- */
/* straint curve is assumed to be present if and only if the */
/* clockwise-ordered sequence of boundary nodes is a subse- */
/* quence of a constraint node sequence.  The triangulation */
/* adjacencies corresponding to constraint edges may or may */
/* not have been forced by a call to ADDCST, and the con- */
/* straint region may or may not be valid (contain no nodes). */


/* On input: */

/*       NCC = Number of constraints.  NCC .GE. 0. */

/*       LCC = List of constraint curve starting indexes (or */
/*             dummy array of length 1 if NCC = 0).  Refer to */
/*             Subroutine ADDCST. */

/*       N = Number of nodes in the triangulation.  N .GE. 3. */

/*       LIST,LEND = Data structure defining the triangula- */
/*                   tion.  Refer to Subroutine TRMESH. */

/*   Input parameters are not altered by this function.  Note */
/* that the parameters are not tested for validity. */

/* On output: */

/*       INDXCC = Index of the exterior constraint curve, if */
/*                present, or 0 otherwise. */

/* Modules required by INDXCC:  None */

/* *********************************************************** */

    /* Parameter adjustments */
    --lcc;
    --lend;
    --list;

    /* Function Body */
    ret_val = 0;
    if (*ncc < 1) {
	return ret_val;
    }

/* Set N0 to the boundary node with smallest index. */

    n0 = 0;
L1:
    ++n0;
    lp = lend[n0];
    if (list[lp] > 0) {
	goto L1;
    }

/* Search in reverse order for the constraint I, if any, that */
/*   contains N0.  IFRST and ILAST index the first and last */
/*   nodes in constraint I. */

    i__ = *ncc;
    ilast = *n;
L2:
    ifrst = lcc[i__];
    if (n0 >= ifrst) {
	goto L3;
    }
    if (i__ == 1) {
	return ret_val;
    }
    --i__;
    ilast = ifrst - 1;
    goto L2;

/* N0 is in constraint I which indexes an exterior constraint */
/*   curve iff the clockwise-ordered sequence of boundary */
/*   node indexes beginning with N0 is increasing and bounded */
/*   above by ILAST. */

L3:
    nst = n0;

L4:
    nxt = -list[lp];
    if (nxt == nst) {
	goto L5;
    }
    if (nxt <= n0 || nxt > ilast) {
	return ret_val;
    }
    n0 = nxt;
    lp = lend[n0];
    goto L4;

/* Constraint I contains the boundary node sequence as a */
/*   subset. */

L5:
    ret_val = i__;
    return ret_val;
} /* indxcc_ */

/* Subroutine */ int insert_(integer *k, integer *lp, integer *list, integer *
	lptr, integer *lnew)
{
    integer lsav;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   09/01/88 */

/*   This subroutine inserts K as a neighbor of N1 following */
/* N2, where LP is the LIST pointer of N2 as a neighbor of */
/* N1.  Note that, if N2 is the last neighbor of N1, K will */
/* become the first neighbor (even if N1 is a boundary node). */


/* On input: */

/*       K = Index of the node to be inserted. */

/*       LP = LIST pointer of N2 as a neighbor of N1. */

/* The above parameters are not altered by this routine. */

/*       LIST,LPTR,LNEW = Data structure defining the trian- */
/*                        gulation.  Refer to Subroutine */
/*                        TRMESH. */

/* On output: */

/*       LIST,LPTR,LNEW = Data structure updated with the */
/*                        addition of node K. */

/* Modules required by INSERT:  None */

/* *********************************************************** */


    /* Parameter adjustments */
    --lptr;
    --list;

    /* Function Body */
    lsav = lptr[*lp];
    lptr[*lp] = *lnew;
    list[*lnew] = *k;
    lptr[*lnew] = lsav;
    ++(*lnew);
    return 0;
} /* insert_ */

/* Subroutine */ int intadd_(integer *kk, integer *i1, integer *i2, integer *
	i3, integer *list, integer *lptr, integer *lend, integer *lnew)
{
    integer k, n1, n2, n3, lp;
    extern /* Subroutine */ int insert_(integer *, integer *, integer *, 
	    integer *, integer *);
    extern integer lstptr_(integer *, integer *, integer *, integer *);


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   02/22/91 */

/*   This subroutine adds an interior node to a triangulation */
/* of a set of points in the plane.  The data structure is */
/* updated with the insertion of node KK into the triangle */
/* whose vertices are I1, I2, and I3.  No optimization of the */
/* triangulation is performed. */


/* On input: */

/*       KK = Index of the node to be inserted.  KK .GE. 1 */
/*            and KK must not be equal to I1, I2, or I3. */

/*       I1,I2,I3 = Indexes of the counterclockwise-ordered */
/*                  sequence of vertices of a triangle which */
/*                  contains node KK. */

/* The above parameters are not altered by this routine. */

/*       LIST,LPTR,LEND,LNEW = Data structure defining the */
/*                             triangulation.  Refer to Sub- */
/*                             routine TRMESH.  Triangle */
/*                             (I1,I2,I3) must be included */
/*                             in the triangulation. */

/* On output: */

/*       LIST,LPTR,LEND,LNEW = Data structure updated with */
/*                             the addition of node KK.  KK */
/*                             will be connected to nodes I1, */
/*                             I2, and I3. */

/* Modules required by INTADD:  INSERT, LSTPTR */

/* *********************************************************** */

    /* Parameter adjustments */
    --lend;
    --lptr;
    --list;

    /* Function Body */
    k = *kk;

/* Initialization. */

    n1 = *i1;
    n2 = *i2;
    n3 = *i3;

/* Add K as a neighbor of I1, I2, and I3. */

    lp = lstptr_(&lend[n1], &n2, &list[1], &lptr[1]);
    insert_(&k, &lp, &list[1], &lptr[1], lnew);
    lp = lstptr_(&lend[n2], &n3, &list[1], &lptr[1]);
    insert_(&k, &lp, &list[1], &lptr[1], lnew);
    lp = lstptr_(&lend[n3], &n1, &list[1], &lptr[1]);
    insert_(&k, &lp, &list[1], &lptr[1], lnew);

/* Add I1, I2, and I3 as neighbors of K. */

    list[*lnew] = n1;
    list[*lnew + 1] = n2;
    list[*lnew + 2] = n3;
    lptr[*lnew] = *lnew + 1;
    lptr[*lnew + 1] = *lnew + 2;
    lptr[*lnew + 2] = *lnew;
    lend[k] = *lnew + 2;
    *lnew += 3;
    return 0;
} /* intadd_ */

logical intsec_(doublereal *x1, doublereal *y1, doublereal *x2, doublereal *
	y2, doublereal *x3, doublereal *y3, doublereal *x4, doublereal *y4)
{
    /* System generated locals */
    logical ret_val;

    /* Local variables */
    doublereal a, b, d__, dx12, dx31, dy12, dy31, dx34, dy34;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   09/01/88 */

/*   Given a pair of line segments P1-P2 and P3-P4, this */
/* function returns the value .TRUE. if and only if P1-P2 */
/* shares one or more points with P3-P4.  The line segments */
/* include their endpoints, and the four points need not be */
/* distinct.  Thus, either line segment may consist of a */
/* single point, and the segments may meet in a V (which is */
/* treated as an intersection).  Note that an incorrect */
/* decision may result from floating point error if the four */
/* endpoints are nearly collinear. */


/* On input: */

/*       X1,Y1 = Coordinates of P1. */

/*       X2,Y2 = Coordinates of P2. */

/*       X3,Y3 = Coordinates of P3. */

/*       X4,Y4 = Coordinates of P4. */

/* Input parameters are not altered by this function. */

/* On output: */

/*       INTSEC = Logical value defined above. */

/* Modules required by INTSEC:  None */

/* *********************************************************** */


/* Test for overlap between the smallest rectangles that */
/*   contain the line segments and have sides parallel to */
/*   the axes. */

    if (*x1 < *x3 && *x1 < *x4 && *x2 < *x3 && *x2 < *x4 || *x1 > *x3 && *x1 
	    > *x4 && *x2 > *x3 && *x2 > *x4 || *y1 < *y3 && *y1 < *y4 && *y2 <
	     *y3 && *y2 < *y4 || *y1 > *y3 && *y1 > *y4 && *y2 > *y3 && *y2 > 
	    *y4) {
	ret_val = FALSE_;
	return ret_val;
    }

/* Compute A = P4-P3 X P1-P3, B = P2-P1 X P1-P3, and */
/*   D = P2-P1 X P4-P3 (Z components). */

    dx12 = *x2 - *x1;
    dy12 = *y2 - *y1;
    dx34 = *x4 - *x3;
    dy34 = *y4 - *y3;
    dx31 = *x1 - *x3;
    dy31 = *y1 - *y3;
    a = dx34 * dy31 - dx31 * dy34;
    b = dx12 * dy31 - dx31 * dy12;
    d__ = dx12 * dy34 - dx34 * dy12;
    if (d__ == 0.) {
	goto L1;
    }

/* D .NE. 0 and the point of intersection of the lines de- */
/*   fined by the line segments is P = P1 + (A/D)*(P2-P1) = */
/*   P3 + (B/D)*(P4-P3). */

    ret_val = a / d__ >= 0. && a / d__ <= 1. && b / d__ >= 0. && b / d__ <= 
	    1.;
    return ret_val;

/* D .EQ. 0 and thus either the line segments are parallel, */
/*   or one (or both) of them is a single point. */

L1:
    ret_val = a == 0. && b == 0.;
    return ret_val;
} /* intsec_ */

integer jrand_(integer *n, integer *ix, integer *iy, integer *iz)
{
    /* System generated locals */
    integer ret_val;

    /* Local variables */
    doublereal u, x;


/* *********************************************************** */

/*                                              From STRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   07/28/98 */

/*   This function returns a uniformly distributed pseudo- */
/* random integer in the range 1 to N. */


/* On input: */

/*       N = Maximum value to be returned. */

/* N is not altered by this function. */

/*       IX,IY,IZ = Integer seeds initialized to values in */
/*                  the range 1 to 30,000 before the first */
/*                  call to JRAND, and not altered between */
/*                  subsequent calls (unless a sequence of */
/*                  random numbers is to be repeated by */
/*                  reinitializing the seeds). */

/* On output: */

/*       IX,IY,IZ = Updated integer seeds. */

/*       JRAND = Random integer in the range 1 to N. */

/* Reference:  B. A. Wichmann and I. D. Hill, "An Efficient */
/*             and Portable Pseudo-random Number Generator", */
/*             Applied Statistics, Vol. 31, No. 2, 1982, */
/*             pp. 188-190. */

/* Modules required by JRAND:  None */

/* Intrinsic functions called by JRAND:  INT, MOD, DBLE */

/* *********************************************************** */


/* Local parameters: */

/* U = Pseudo-random number uniformly distributed in the */
/*     interval (0,1). */
/* X = Pseudo-random number in the range 0 to 3 whose frac- */
/*       tional part is U. */

    *ix = *ix * 171 % 30269;
    *iy = *iy * 172 % 30307;
    *iz = *iz * 170 % 30323;
    x = (doublereal) (*ix) / 30269. + (doublereal) (*iy) / 30307. + (
	    doublereal) (*iz) / 30323.;
    u = x - (integer) x;
    ret_val = (integer) ((doublereal) (*n) * u + 1.);
    return ret_val;
} /* jrand_ */

logical left_(doublereal *x1, doublereal *y1, doublereal *x2, doublereal *y2, 
	doublereal *x0, doublereal *y0)
{
    /* System generated locals */
    logical ret_val;

    /* Local variables */
    doublereal dx1, dy1, dx2, dy2;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   09/01/88 */

/*   This function determines whether node N0 is to the left */
/* or to the right of the line through N1-N2 as viewed by an */
/* observer at N1 facing N2. */


/* On input: */

/*       X1,Y1 = Coordinates of N1. */

/*       X2,Y2 = Coordinates of N2. */

/*       X0,Y0 = Coordinates of N0. */

/* Input parameters are not altered by this function. */

/* On output: */

/*       LEFT = .TRUE. if and only if (X0,Y0) is on or to the */
/*              left of the directed line N1->N2. */

/* Modules required by LEFT:  None */

/* *********************************************************** */


/* Local parameters: */

/* DX1,DY1 = X,Y components of the vector N1->N2 */
/* DX2,DY2 = X,Y components of the vector N1->N0 */

    dx1 = *x2 - *x1;
    dy1 = *y2 - *y1;
    dx2 = *x0 - *x1;
    dy2 = *y0 - *y1;

/* If the sign of the vector cross product of N1->N2 and */
/*   N1->N0 is positive, then sin(A) > 0, where A is the */
/*   angle between the vectors, and thus A is in the range */
/*   (0,180) degrees. */

    ret_val = dx1 * dy2 >= dx2 * dy1;
    return ret_val;
} /* left_ */

integer lstptr_(integer *lpl, integer *nb, integer *list, integer *lptr)
{
    /* System generated locals */
    integer ret_val;

    /* Local variables */
    integer nd, lp;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   09/01/88 */

/*   This function returns the index (LIST pointer) of NB in */
/* the adjacency list for N0, where LPL = LEND(N0). */


/* On input: */

/*       LPL = LEND(N0) */

/*       NB = Index of the node whose pointer is to be re- */
/*            turned.  NB must be connected to N0. */

/*       LIST,LPTR = Data structure defining the triangula- */
/*                   tion.  Refer to Subroutine TRMESH. */

/* Input parameters are not altered by this function. */

/* On output: */

/*       LSTPTR = Pointer such that LIST(LSTPTR) = NB or */
/*                LIST(LSTPTR) = -NB, unless NB is not a */
/*                neighbor of N0, in which case LSTPTR = LPL. */

/* Modules required by LSTPTR:  None */

/* *********************************************************** */


    /* Parameter adjustments */
    --lptr;
    --list;

    /* Function Body */
    lp = lptr[*lpl];
L1:
    nd = list[lp];
    if (nd == *nb) {
	goto L2;
    }
    lp = lptr[lp];
    if (lp != *lpl) {
	goto L1;
    }

L2:
    ret_val = lp;
    return ret_val;
} /* lstptr_ */

integer nbcnt_(integer *lpl, integer *lptr)
{
    /* System generated locals */
    integer ret_val;

    /* Local variables */
    integer k, lp;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   09/01/88 */

/*   This function returns the number of neighbors of a node */
/* N0 in a triangulation created by Subroutine TRMESH (or */
/* TRMSHR). */


/* On input: */

/*       LPL = LIST pointer to the last neighbor of N0 -- */
/*             LPL = LEND(N0). */

/*       LPTR = Array of pointers associated with LIST. */

/* Input parameters are not altered by this function. */

/* On output: */

/*       NBCNT = Number of neighbors of N0. */

/* Modules required by NBCNT:  None */

/* *********************************************************** */


    /* Parameter adjustments */
    --lptr;

    /* Function Body */
    lp = *lpl;
    k = 1;

L1:
    lp = lptr[lp];
    if (lp == *lpl) {
	goto L2;
    }
    ++k;
    goto L1;

L2:
    ret_val = k;
    return ret_val;
} /* nbcnt_ */

integer nearnd_(doublereal *xp, doublereal *yp, integer *ist, integer *n, 
	doublereal *x, doublereal *y, integer *list, integer *lptr, integer *
	lend, doublereal *dsq)
{
    /* System generated locals */
    integer ret_val, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    integer l, i1, i2, i3, n1, n2, n3, lp, nr;
    doublereal ds1;
    integer lp1, lp2;
    doublereal dx11, dx12, dx21, dx22, dy11, dy12, dy21, dy22;
    integer lpl;
    doublereal dsr;
    integer nst;
    doublereal cos1, cos2, sin1, sin2;
    integer listp[25], lptrp[25];
    extern /* Subroutine */ int trfind_(integer *, doublereal *, doublereal *,
	     integer *, doublereal *, doublereal *, integer *, integer *, 
	    integer *, integer *, integer *, integer *);
    extern integer lstptr_(integer *, integer *, integer *, integer *);


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   06/27/98 */

/*   Given a point P in the plane and a Delaunay triangula- */
/* tion created by Subroutine TRMESH or TRMSHR, this function */
/* returns the index of the nearest triangulation node to P. */

/*   The algorithm consists of implicitly adding P to the */
/* triangulation, finding the nearest neighbor to P, and */
/* implicitly deleting P from the triangulation.  Thus, it */
/* is based on the fact that, if P is a node in a Delaunay */
/* triangulation, the nearest node to P is a neighbor of P. */


/* On input: */

/*       XP,YP = Cartesian coordinates of the point P to be */
/*               located relative to the triangulation. */

/*       IST = Index of a node at which TRFIND begins the */
/*             search.  Search time depends on the proximity */
/*             of this node to P. */

/*       N = Number of nodes in the triangulation.  N .GE. 3. */

/*       X,Y = Arrays of length N containing the Cartesian */
/*             coordinates of the nodes. */

/*       LIST,LPTR,LEND = Data structure defining the trian- */
/*                        gulation.  Refer to TRMESH. */

/* Input parameters are not altered by this function. */

/* On output: */

/*       NEARND = Nodal index of the nearest node to P, or 0 */
/*                if N < 3 or the triangulation data struc- */
/*                ture is invalid. */

/*       DSQ = Squared distance between P and NEARND unless */
/*             NEARND = 0. */

/*       Note that the number of candidates for NEARND */
/*       (neighbors of P) is limited to LMAX defined in */
/*       the PARAMETER statement below. */

/* Modules required by NEARND:  JRAND, LEFT, LSTPTR, TRFIND */

/* Intrinsic function called by NEARND:  ABS */

/* *********************************************************** */


/* Store local parameters and test for N invalid. */

    /* Parameter adjustments */
    --lend;
    --y;
    --x;
    --list;
    --lptr;

    /* Function Body */
    if (*n < 3) {
	goto L7;
    }
    nst = *ist;
    if (nst < 1 || nst > *n) {
	nst = 1;
    }

/* Find a triangle (I1,I2,I3) containing P, or the rightmost */
/*   (I1) and leftmost (I2) visible boundary nodes as viewed */
/*   from P. */

    trfind_(&nst, xp, yp, n, &x[1], &y[1], &list[1], &lptr[1], &lend[1], &i1, 
	    &i2, &i3);

/* Test for collinear nodes. */

    if (i1 == 0) {
	goto L7;
    }

/* Store the linked list of 'neighbors' of P in LISTP and */
/*   LPTRP.  I1 is the first neighbor, and 0 is stored as */
/*   the last neighbor if P is not contained in a triangle. */
/*   L is the length of LISTP and LPTRP, and is limited to */
/*   LMAX. */

    if (i3 != 0) {
	listp[0] = i1;
	lptrp[0] = 2;
	listp[1] = i2;
	lptrp[1] = 3;
	listp[2] = i3;
	lptrp[2] = 1;
	l = 3;
    } else {
	n1 = i1;
	l = 1;
	lp1 = 2;
	listp[l - 1] = n1;
	lptrp[l - 1] = lp1;

/*   Loop on the ordered sequence of visible boundary nodes */
/*     N1 from I1 to I2. */

L1:
	lpl = lend[n1];
	n1 = -list[lpl];
	l = lp1;
	lp1 = l + 1;
	listp[l - 1] = n1;
	lptrp[l - 1] = lp1;
	if (n1 != i2 && lp1 < 25) {
	    goto L1;
	}
	l = lp1;
	listp[l - 1] = 0;
	lptrp[l - 1] = 1;
    }

/* Initialize variables for a loop on arcs N1-N2 opposite P */
/*   in which new 'neighbors' are 'swapped' in.  N1 follows */
/*   N2 as a neighbor of P, and LP1 and LP2 are the LISTP */
/*   indexes of N1 and N2. */

    lp2 = 1;
    n2 = i1;
    lp1 = lptrp[0];
    n1 = listp[lp1 - 1];

/* Begin loop:  find the node N3 opposite N1->N2. */

L2:
    lp = lstptr_(&lend[n1], &n2, &list[1], &lptr[1]);
    if (list[lp] < 0) {
	goto L4;
    }
    lp = lptr[lp];
    n3 = (i__1 = list[lp], abs(i__1));

/* Swap test:  Exit the loop if L = LMAX. */

    if (l == 25) {
	goto L5;
    }
    dx11 = x[n1] - x[n3];
    dx12 = x[n2] - x[n3];
    dx22 = x[n2] - *xp;
    dx21 = x[n1] - *xp;

    dy11 = y[n1] - y[n3];
    dy12 = y[n2] - y[n3];
    dy22 = y[n2] - *yp;
    dy21 = y[n1] - *yp;

    cos1 = dx11 * dx12 + dy11 * dy12;
    cos2 = dx22 * dx21 + dy22 * dy21;
    if (cos1 >= 0. && cos2 >= 0.) {
	goto L4;
    }
    if (cos1 < 0. && cos2 < 0.) {
	goto L3;
    }

    sin1 = dx11 * dy12 - dx12 * dy11;
    sin2 = dx22 * dy21 - dx21 * dy22;
    if (sin1 * cos2 + cos1 * sin2 >= 0.) {
	goto L4;
    }

/* Swap:  Insert N3 following N2 in the adjacency list for P. */
/*        The two new arcs opposite P must be tested. */

L3:
    ++l;
    lptrp[lp2 - 1] = l;
    listp[l - 1] = n3;
    lptrp[l - 1] = lp1;
    lp1 = l;
    n1 = n3;
    goto L2;

/* No swap:  Advance to the next arc and test for termination */
/*           on N1 = I1 (LP1 = 1) or N1 followed by 0. */

L4:
    if (lp1 == 1) {
	goto L5;
    }
    lp2 = lp1;
    n2 = n1;
    lp1 = lptrp[lp1 - 1];
    n1 = listp[lp1 - 1];
    if (n1 == 0) {
	goto L5;
    }
    goto L2;

/* Set NR and DSR to the index of the nearest node to P and */
/*   its squared distance from P, respectively. */

L5:
    nr = i1;
/* Computing 2nd power */
    d__1 = x[nr] - *xp;
/* Computing 2nd power */
    d__2 = y[nr] - *yp;
    dsr = d__1 * d__1 + d__2 * d__2;
    i__1 = l;
    for (lp = 2; lp <= i__1; ++lp) {
	n1 = listp[lp - 1];
	if (n1 == 0) {
	    goto L6;
	}
/* Computing 2nd power */
	d__1 = x[n1] - *xp;
/* Computing 2nd power */
	d__2 = y[n1] - *yp;
	ds1 = d__1 * d__1 + d__2 * d__2;
	if (ds1 < dsr) {
	    nr = n1;
	    dsr = ds1;
	}
L6:
	;
    }
    *dsq = dsr;
    ret_val = nr;
    return ret_val;

/* Invalid input. */

L7:
    ret_val = 0;
    return ret_val;
} /* nearnd_ */

/* Subroutine */ int optim_(doublereal *x, doublereal *y, integer *na, 
	integer *list, integer *lptr, integer *lend, integer *nit, integer *
	iwk, integer *ier)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer i__, n1, n2, lp, io1, io2, nna, lp21, lpl, lpp;
    logical swp;
    integer iter;
    extern /* Subroutine */ int swap_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *);
    integer maxit;
    extern logical swptst_(integer *, integer *, integer *, integer *, 
	    doublereal *, doublereal *);


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   06/27/98 */

/*   Given a set of NA triangulation arcs, this subroutine */
/* optimizes the portion of the triangulation consisting of */
/* the quadrilaterals (pairs of adjacent triangles) which */
/* have the arcs as diagonals by applying the circumcircle */
/* test and appropriate swaps to the arcs. */

/*   An iteration consists of applying the swap test and */
/* swaps to all NA arcs in the order in which they are */
/* stored.  The iteration is repeated until no swap occurs */
/* or NIT iterations have been performed.  The bound on the */
/* number of iterations may be necessary to prevent an */
/* infinite loop caused by cycling (reversing the effect of a */
/* previous swap) due to floating point inaccuracy when four */
/* or more nodes are nearly cocircular. */


/* On input: */

/*       X,Y = Arrays containing the nodal coordinates. */

/*       NA = Number of arcs in the set.  NA .GE. 0. */

/* The above parameters are not altered by this routine. */

/*       LIST,LPTR,LEND = Data structure defining the trian- */
/*                        gulation.  Refer to Subroutine */
/*                        TRMESH. */

/*       NIT = Maximum number of iterations to be performed. */
/*             A reasonable value is 3*NA.  NIT .GE. 1. */

/*       IWK = Integer array dimensioned 2 by NA containing */
/*             the nodal indexes of the arc endpoints (pairs */
/*             of endpoints are stored in columns). */

/* On output: */

/*       LIST,LPTR,LEND = Updated triangulation data struc- */
/*                        ture reflecting the swaps. */

/*       NIT = Number of iterations performed. */

/*       IWK = Endpoint indexes of the new set of arcs */
/*             reflecting the swaps. */

/*       IER = Error indicator: */
/*             IER = 0 if no errors were encountered. */
/*             IER = 1 if a swap occurred on the last of */
/*                     MAXIT iterations, where MAXIT is the */
/*                     value of NIT on input.  The new set */
/*                     of arcs in not necessarily optimal */
/*                     in this case. */
/*             IER = 2 if NA < 0 or NIT < 1 on input. */
/*             IER = 3 if IWK(2,I) is not a neighbor of */
/*                     IWK(1,I) for some I in the range 1 */
/*                     to NA.  A swap may have occurred in */
/*                     this case. */
/*             IER = 4 if a zero pointer was returned by */
/*                     Subroutine SWAP. */

/* Modules required by OPTIM:  LSTPTR, SWAP, SWPTST */

/* Intrinsic function called by OPTIM:  ABS */

/* *********************************************************** */


/* Local parameters: */

/* I =       Column index for IWK */
/* IO1,IO2 = Nodal indexes of the endpoints of an arc in IWK */
/* ITER =    Iteration count */
/* LP =      LIST pointer */
/* LP21 =    Parameter returned by SWAP (not used) */
/* LPL =     Pointer to the last neighbor of IO1 */
/* LPP =     Pointer to the node preceding IO2 as a neighbor */
/*             of IO1 */
/* MAXIT =   Input value of NIT */
/* N1,N2 =   Nodes opposite IO1->IO2 and IO2->IO1, */
/*             respectively */
/* NNA =     Local copy of NA */
/* SWP =     Flag set to TRUE iff a swap occurs in the */
/*             optimization loop */

    /* Parameter adjustments */
    --x;
    --y;
    iwk -= 3;
    --list;
    --lptr;
    --lend;

    /* Function Body */
    nna = *na;
    maxit = *nit;
    if (nna < 0 || maxit < 1) {
	goto L7;
    }

/* Initialize iteration count ITER and test for NA = 0. */

    iter = 0;
    if (nna == 0) {
	goto L5;
    }

/* Top of loop -- */
/*   SWP = TRUE iff a swap occurred in the current iteration. */

L1:
    if (iter == maxit) {
	goto L6;
    }
    ++iter;
    swp = FALSE_;

/*   Inner loop on arcs IO1-IO2 -- */

    i__1 = nna;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io1 = iwk[(i__ << 1) + 1];
	io2 = iwk[(i__ << 1) + 2];

/*   Set N1 and N2 to the nodes opposite IO1->IO2 and */
/*     IO2->IO1, respectively.  Determine the following: */

/*     LPL = pointer to the last neighbor of IO1, */
/*     LP = pointer to IO2 as a neighbor of IO1, and */
/*     LPP = pointer to the node N2 preceding IO2. */

	lpl = lend[io1];
	lpp = lpl;
	lp = lptr[lpp];
L2:
	if (list[lp] == io2) {
	    goto L3;
	}
	lpp = lp;
	lp = lptr[lpp];
	if (lp != lpl) {
	    goto L2;
	}

/*   IO2 should be the last neighbor of IO1.  Test for no */
/*     arc and bypass the swap test if IO1 is a boundary */
/*     node. */

	if ((i__2 = list[lp], abs(i__2)) != io2) {
	    goto L8;
	}
	if (list[lp] < 0) {
	    goto L4;
	}

/*   Store N1 and N2, or bypass the swap test if IO1 is a */
/*     boundary node and IO2 is its first neighbor. */

L3:
	n2 = list[lpp];
	if (n2 < 0) {
	    goto L4;
	}
	lp = lptr[lp];
	n1 = (i__2 = list[lp], abs(i__2));

/*   Test IO1-IO2 for a swap, and update IWK if necessary. */

	if (! swptst_(&n1, &n2, &io1, &io2, &x[1], &y[1])) {
	    goto L4;
	}
	swap_(&n1, &n2, &io1, &io2, &list[1], &lptr[1], &lend[1], &lp21);
	if (lp21 == 0) {
	    goto L9;
	}
	swp = TRUE_;
	iwk[(i__ << 1) + 1] = n1;
	iwk[(i__ << 1) + 2] = n2;
L4:
	;
    }
    if (swp) {
	goto L1;
    }

/* Successful termination. */

L5:
    *nit = iter;
    *ier = 0;
    return 0;

/* MAXIT iterations performed without convergence. */

L6:
    *nit = maxit;
    *ier = 1;
    return 0;

/* Invalid input parameter. */

L7:
    *nit = 0;
    *ier = 2;
    return 0;

/* IO2 is not a neighbor of IO1. */

L8:
    *nit = iter;
    *ier = 3;
    return 0;

/* Zero pointer returned by SWAP. */

L9:
    *nit = iter;
    *ier = 4;
    return 0;
} /* optim_ */

doublereal store_(doublereal *x)
{
    /* System generated locals */
    doublereal ret_val;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   03/18/90 */

/*   This function forces its argument X to be stored in a */
/* memory location, thus providing a means of determining */
/* floating point number characteristics (such as the machine */
/* precision) when it is necessary to avoid computation in */
/* high precision registers. */


/* On input: */

/*       X = Value to be stored. */

/* X is not altered by this function. */

/* On output: */

/*       STORE = Value of X after it has been stored and */
/*               possibly truncated or rounded to the single */
/*               precision word length. */

/* Modules required by STORE:  None */

/* *********************************************************** */


    stcom_1.y = *x;
    ret_val = stcom_1.y;
    return ret_val;
} /* store_ */

/* Subroutine */ int swap_(integer *in1, integer *in2, integer *io1, integer *
	io2, integer *list, integer *lptr, integer *lend, integer *lp21)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer lp, lph, lpsav;
    extern integer lstptr_(integer *, integer *, integer *, integer *);


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   06/22/98 */

/*   Given a triangulation of a set of points on the unit */
/* sphere, this subroutine replaces a diagonal arc in a */
/* strictly convex quadrilateral (defined by a pair of adja- */
/* cent triangles) with the other diagonal.  Equivalently, a */
/* pair of adjacent triangles is replaced by another pair */
/* having the same union. */


/* On input: */

/*       IN1,IN2,IO1,IO2 = Nodal indexes of the vertices of */
/*                         the quadrilateral.  IO1-IO2 is re- */
/*                         placed by IN1-IN2.  (IO1,IO2,IN1) */
/*                         and (IO2,IO1,IN2) must be trian- */
/*                         gles on input. */

/* The above parameters are not altered by this routine. */

/*       LIST,LPTR,LEND = Data structure defining the trian- */
/*                        gulation.  Refer to Subroutine */
/*                        TRMESH. */

/* On output: */

/*       LIST,LPTR,LEND = Data structure updated with the */
/*                        swap -- triangles (IO1,IO2,IN1) and */
/*                        (IO2,IO1,IN2) are replaced by */
/*                        (IN1,IN2,IO2) and (IN2,IN1,IO1) */
/*                        unless LP21 = 0. */

/*       LP21 = Index of IN1 as a neighbor of IN2 after the */
/*              swap is performed unless IN1 and IN2 are */
/*              adjacent on input, in which case LP21 = 0. */

/* Module required by SWAP:  LSTPTR */

/* Intrinsic function called by SWAP:  ABS */

/* *********************************************************** */


/* Local parameters: */

/* LP,LPH,LPSAV = LIST pointers */


/* Test for IN1 and IN2 adjacent. */

    /* Parameter adjustments */
    --lend;
    --lptr;
    --list;

    /* Function Body */
    lp = lstptr_(&lend[*in1], in2, &list[1], &lptr[1]);
    if ((i__1 = list[lp], abs(i__1)) == *in2) {
	*lp21 = 0;
	return 0;
    }

/* Delete IO2 as a neighbor of IO1. */

    lp = lstptr_(&lend[*io1], in2, &list[1], &lptr[1]);
    lph = lptr[lp];
    lptr[lp] = lptr[lph];

/* If IO2 is the last neighbor of IO1, make IN2 the */
/*   last neighbor. */

    if (lend[*io1] == lph) {
	lend[*io1] = lp;
    }

/* Insert IN2 as a neighbor of IN1 following IO1 */
/*   using the hole created above. */

    lp = lstptr_(&lend[*in1], io1, &list[1], &lptr[1]);
    lpsav = lptr[lp];
    lptr[lp] = lph;
    list[lph] = *in2;
    lptr[lph] = lpsav;

/* Delete IO1 as a neighbor of IO2. */

    lp = lstptr_(&lend[*io2], in1, &list[1], &lptr[1]);
    lph = lptr[lp];
    lptr[lp] = lptr[lph];

/* If IO1 is the last neighbor of IO2, make IN1 the */
/*   last neighbor. */

    if (lend[*io2] == lph) {
	lend[*io2] = lp;
    }

/* Insert IN1 as a neighbor of IN2 following IO2. */

    lp = lstptr_(&lend[*in2], io2, &list[1], &lptr[1]);
    lpsav = lptr[lp];
    lptr[lp] = lph;
    list[lph] = *in1;
    lptr[lph] = lpsav;
    *lp21 = lph;
    return 0;
} /* swap_ */

logical swptst_(integer *in1, integer *in2, integer *io1, integer *io2, 
	doublereal *x, doublereal *y)
{
    /* System generated locals */
    logical ret_val;

    /* Local variables */
    doublereal dx11, dx12, dx22, dx21, dy11, dy12, dy22, dy21, cos1, cos2, 
	    sin1, sin2, sin12;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   09/01/88 */

/*   This function applies the circumcircle test to a quadri- */
/* lateral defined by a pair of adjacent triangles.  The */
/* diagonal arc (shared triangle side) should be swapped for */
/* the other diagonl if and only if the fourth vertex is */
/* strictly interior to the circumcircle of one of the */
/* triangles (the decision is independent of the choice of */
/* triangle).  Equivalently, the diagonal is chosen to maxi- */
/* mize the smallest of the six interior angles over the two */
/* pairs of possible triangles (the decision is for no swap */
/* if the quadrilateral is not strictly convex). */

/*   When the four vertices are nearly cocircular (the */
/* neutral case), the preferred decision is no swap -- in */
/* order to avoid unnecessary swaps and, more important, to */
/* avoid cycling in Subroutine OPTIM which is called by */
/* DELNOD and EDGE.  Thus, a tolerance SWTOL (stored in */
/* SWPCOM by TRMESH or TRMSHR) is used to define 'nearness' */
/* to the neutral case. */


/* On input: */

/*       IN1,IN2,IO1,IO2 = Nodal indexes of the vertices of */
/*                         the quadrilateral.  IO1-IO2 is the */
/*                         triangulation arc (shared triangle */
/*                         side) to be replaced by IN1-IN2 if */
/*                         the decision is to swap.  The */
/*                         triples (IO1,IO2,IN1) and (IO2, */
/*                         IO1,IN2) must define triangles (be */
/*                         in counterclockwise order) on in- */
/*                         put. */

/*       X,Y = Arrays containing the nodal coordinates. */

/* Input parameters are not altered by this routine. */

/* On output: */

/*       SWPTST = .TRUE. if and only if the arc connecting */
/*                IO1 and IO2 is to be replaced. */

/* Modules required by SWPTST:  None */

/* *********************************************************** */


/* Tolerance stored by TRMESH or TRMSHR. */


/* Local parameters: */

/* DX11,DY11 = X,Y components of the vector IN1->IO1 */
/* DX12,DY12 = X,Y components of the vector IN1->IO2 */
/* DX22,DY22 = X,Y components of the vector IN2->IO2 */
/* DX21,DY21 = X,Y components of the vector IN2->IO1 */
/* SIN1 =      Cross product of the vectors IN1->IO1 and */
/*               IN1->IO2 -- proportional to sin(T1), where */
/*               T1 is the angle at IN1 formed by the vectors */
/* COS1 =      Inner product of the vectors IN1->IO1 and */
/*               IN1->IO2 -- proportional to cos(T1) */
/* SIN2 =      Cross product of the vectors IN2->IO2 and */
/*               IN2->IO1 -- proportional to sin(T2), where */
/*               T2 is the angle at IN2 formed by the vectors */
/* COS2 =      Inner product of the vectors IN2->IO2 and */
/*               IN2->IO1 -- proportional to cos(T2) */
/* SIN12 =     SIN1*COS2 + COS1*SIN2 -- proportional to */
/*               sin(T1+T2) */


/* Compute the vectors containing the angles T1 and T2. */

    /* Parameter adjustments */
    --y;
    --x;

    /* Function Body */
    dx11 = x[*io1] - x[*in1];
    dx12 = x[*io2] - x[*in1];
    dx22 = x[*io2] - x[*in2];
    dx21 = x[*io1] - x[*in2];

    dy11 = y[*io1] - y[*in1];
    dy12 = y[*io2] - y[*in1];
    dy22 = y[*io2] - y[*in2];
    dy21 = y[*io1] - y[*in2];

/* Compute inner products. */

    cos1 = dx11 * dx12 + dy11 * dy12;
    cos2 = dx22 * dx21 + dy22 * dy21;

/* The diagonals should be swapped iff (T1+T2) > 180 */
/*   degrees.  The following two tests ensure numerical */
/*   stability:  the decision must be FALSE when both */
/*   angles are close to 0, and TRUE when both angles */
/*   are close to 180 degrees. */

    if (cos1 >= 0. && cos2 >= 0.) {
	goto L2;
    }
    if (cos1 < 0. && cos2 < 0.) {
	goto L1;
    }

/* Compute vector cross products (Z-components). */

    sin1 = dx11 * dy12 - dx12 * dy11;
    sin2 = dx22 * dy21 - dx21 * dy22;
    sin12 = sin1 * cos2 + cos1 * sin2;
    if (sin12 >= -swpcom_1.swtol) {
	goto L2;
    }

/* Swap. */

L1:
    ret_val = TRUE_;
    return ret_val;

/* No swap. */

L2:
    ret_val = FALSE_;
    return ret_val;
} /* swptst_ */

/* Subroutine */ int trfind_(integer *nst, doublereal *px, doublereal *py, 
	integer *n, doublereal *x, doublereal *y, integer *list, integer *
	lptr, integer *lend, integer *i1, integer *i2, integer *i3)
{
    /* Initialized data */

    static integer ix = 1;
    static integer iy = 2;
    static integer iz = 3;

    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Local variables */
    doublereal b1, b2;
    integer n0, n1, n2, n3, n4, nb, nf, nl, lp, np;
    doublereal xp, yp;
    integer n1s, n2s, npp;
    extern logical left_(doublereal *, doublereal *, doublereal *, doublereal 
	    *, doublereal *, doublereal *);
    extern integer jrand_(integer *, integer *, integer *, integer *);
    extern doublereal store_(doublereal *);
    extern integer lstptr_(integer *, integer *, integer *, integer *);


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   07/28/98 */

/*   This subroutine locates a point P relative to a triangu- */
/* lation created by Subroutine TRMESH or TRMSHR.  If P is */
/* contained in a triangle, the three vertex indexes are */
/* returned.  Otherwise, the indexes of the rightmost and */
/* leftmost visible boundary nodes are returned. */


/* On input: */

/*       NST = Index of a node at which TRFIND begins the */
/*             search.  Search time depends on the proximity */
/*             of this node to P. */

/*       PX,PY = X and y coordinates of the point P to be */
/*               located. */

/*       N = Number of nodes in the triangulation.  N .GE. 3. */

/*       X,Y = Arrays of length N containing the coordinates */
/*             of the nodes in the triangulation. */

/*       LIST,LPTR,LEND = Data structure defining the trian- */
/*                        gulation.  Refer to Subroutine */
/*                        TRMESH. */

/* Input parameters are not altered by this routine. */

/* On output: */

/*       I1,I2,I3 = Nodal indexes, in counterclockwise order, */
/*                  of the vertices of a triangle containing */
/*                  P if P is contained in a triangle.  If P */
/*                  is not in the convex hull of the nodes, */
/*                  I1 indexes the rightmost visible boundary */
/*                  node, I2 indexes the leftmost visible */
/*                  boundary node, and I3 = 0.  Rightmost and */
/*                  leftmost are defined from the perspective */
/*                  of P, and a pair of points are visible */
/*                  from each other if and only if the line */
/*                  segment joining them intersects no trian- */
/*                  gulation arc.  If P and all of the nodes */
/*                  lie on a common line, then I1 = I2 = I3 = */
/*                  0 on output. */

/* Modules required by TRFIND:  JRAND, LEFT, LSTPTR, STORE */

/* Intrinsic function called by TRFIND:  ABS */

/* *********************************************************** */


    /* Parameter adjustments */
    --lend;
    --y;
    --x;
    --list;
    --lptr;

    /* Function Body */

/* Local parameters: */

/* B1,B2 =    Unnormalized barycentric coordinates of P with */
/*              respect to (N1,N2,N3) */
/* IX,IY,IZ = Integer seeds for JRAND */
/* LP =       LIST pointer */
/* N0,N1,N2 = Nodes in counterclockwise order defining a */
/*              cone (with vertex N0) containing P */
/* N1S,N2S =  Saved values of N1 and N2 */
/* N3,N4 =    Nodes opposite N1->N2 and N2->N1, respectively */
/* NB =       Index of a boundary node -- first neighbor of */
/*              NF or last neighbor of NL in the boundary */
/*              traversal loops */
/* NF,NL =    First and last neighbors of N0, or first */
/*              (rightmost) and last (leftmost) nodes */
/*              visible from P when P is exterior to the */
/*              triangulation */
/* NP,NPP =   Indexes of boundary nodes used in the boundary */
/*              traversal loops */
/* XA,XB,XC = Dummy arguments for FRWRD */
/* YA,YB,YC = Dummy arguments for FRWRD */
/* XP,YP =    Local variables containing the components of P */

/* Statement function: */

/* FRWRD = TRUE iff C is forward of A->B */
/*              iff <A->B,A->C> .GE. 0. */


/* Initialize variables. */

    xp = *px;
    yp = *py;
    n0 = *nst;
    if (n0 < 1 || n0 > *n) {
	n0 = jrand_(n, &ix, &iy, &iz);
    }

/* Set NF and NL to the first and last neighbors of N0, and */
/*   initialize N1 = NF. */

L1:
    lp = lend[n0];
    nl = list[lp];
    lp = lptr[lp];
    nf = list[lp];
    n1 = nf;

/* Find a pair of adjacent neighbors N1,N2 of N0 that define */
/*   a wedge containing P:  P LEFT N0->N1 and P RIGHT N0->N2. */

    if (nl > 0) {
	goto L2;
    }

/*   N0 is a boundary node.  Test for P exterior. */

    nl = -nl;
    if (! left_(&x[n0], &y[n0], &x[nf], &y[nf], &xp, &yp)) {
	nl = n0;
	goto L9;
    }
    if (! left_(&x[nl], &y[nl], &x[n0], &y[n0], &xp, &yp)) {
	nb = nf;
	nf = n0;
	np = nl;
	npp = n0;
	goto L11;
    }
    goto L3;

/*   N0 is an interior node.  Find N1. */

L2:
    if (left_(&x[n0], &y[n0], &x[n1], &y[n1], &xp, &yp)) {
	goto L3;
    }
    lp = lptr[lp];
    n1 = list[lp];
    if (n1 == nl) {
	goto L6;
    }
    goto L2;

/*   P is to the left of edge N0->N1.  Initialize N2 to the */
/*     next neighbor of N0. */

L3:
    lp = lptr[lp];
    n2 = (i__1 = list[lp], abs(i__1));
    if (! left_(&x[n0], &y[n0], &x[n2], &y[n2], &xp, &yp)) {
	goto L7;
    }
    n1 = n2;
    if (n1 != nl) {
	goto L3;
    }
    if (! left_(&x[n0], &y[n0], &x[nf], &y[nf], &xp, &yp)) {
	goto L6;
    }
    if (xp == x[n0] && yp == y[n0]) {
	goto L5;
    }

/*   P is left of or on edges N0->NB for all neighbors NB */
/*     of N0. */
/*   All points are collinear iff P is left of NB->N0 for */
/*     all neighbors NB of N0.  Search the neighbors of N0. */
/*     NOTE -- N1 = NL and LP points to NL. */

L4:
    if (! left_(&x[n1], &y[n1], &x[n0], &y[n0], &xp, &yp)) {
	goto L5;
    }
    lp = lptr[lp];
    n1 = (i__1 = list[lp], abs(i__1));
    if (n1 == nl) {
	goto L17;
    }
    goto L4;

/*   P is to the right of N1->N0, or P=N0.  Set N0 to N1 and */
/*     start over. */

L5:
    n0 = n1;
    goto L1;

/*   P is between edges N0->N1 and N0->NF. */

L6:
    n2 = nf;

/* P is contained in the wedge defined by line segments */
/*   N0->N1 and N0->N2, where N1 is adjacent to N2.  Set */
/*   N3 to the node opposite N1->N2, and save N1 and N2 to */
/*   test for cycling. */

L7:
    n3 = n0;
    n1s = n1;
    n2s = n2;

/* Top of edge hopping loop.  Test for termination. */

L8:
    if (left_(&x[n1], &y[n1], &x[n2], &y[n2], &xp, &yp)) {

/*   P LEFT N1->N2 and hence P is in (N1,N2,N3) unless an */
/*     error resulted from floating point inaccuracy and */
/*     collinearity.  Compute the unnormalized barycentric */
/*     coordinates of P with respect to (N1,N2,N3). */

	b1 = (x[n3] - x[n2]) * (yp - y[n2]) - (xp - x[n2]) * (y[n3] - y[n2]);
	b2 = (x[n1] - x[n3]) * (yp - y[n3]) - (xp - x[n3]) * (y[n1] - y[n3]);
	d__1 = b1 + 1.;
	d__2 = b2 + 1.;
	if (store_(&d__1) >= 1. && store_(&d__2) >= 1.) {
	    goto L16;
	}

/*   Restart with N0 randomly selected. */

	n0 = jrand_(n, &ix, &iy, &iz);
	goto L1;
    }

/*   Set N4 to the neighbor of N2 which follows N1 (node */
/*     opposite N2->N1) unless N1->N2 is a boundary edge. */

    lp = lstptr_(&lend[n2], &n1, &list[1], &lptr[1]);
    if (list[lp] < 0) {
	nf = n2;
	nl = n1;
	goto L9;
    }
    lp = lptr[lp];
    n4 = (i__1 = list[lp], abs(i__1));

/*   Select the new edge N1->N2 which intersects the line */
/*     segment N0-P, and set N3 to the node opposite N1->N2. */

    if (left_(&x[n0], &y[n0], &x[n4], &y[n4], &xp, &yp)) {
	n3 = n1;
	n1 = n4;
	n2s = n2;
	if (n1 != n1s && n1 != n0) {
	    goto L8;
	}
    } else {
	n3 = n2;
	n2 = n4;
	n1s = n1;
	if (n2 != n2s && n2 != n0) {
	    goto L8;
	}
    }

/*   The starting node N0 or edge N1-N2 was encountered */
/*     again, implying a cycle (infinite loop).  Restart */
/*     with N0 randomly selected. */

    n0 = jrand_(n, &ix, &iy, &iz);
    goto L1;

/* Boundary traversal loops.  NL->NF is a boundary edge and */
/*   P RIGHT NL->NF.  Save NL and NF. */
L9:
    np = nl;
    npp = nf;

/* Find the first (rightmost) visible boundary node NF.  NB */
/*   is set to the first neighbor of NF, and NP is the last */
/*   neighbor. */

L10:
    lp = lend[nf];
    lp = lptr[lp];
    nb = list[lp];
    if (! left_(&x[nf], &y[nf], &x[nb], &y[nb], &xp, &yp)) {
	goto L12;
    }

/*   P LEFT NF->NB and thus NB is not visible unless an error */
/*     resulted from floating point inaccuracy and collinear- */
/*     ity of the 4 points NP, NF, NB, and P. */

L11:
    if ((x[np] - x[nf]) * (xp - x[nf]) + (y[np] - y[nf]) * (yp - y[nf]) >= 0. 
	    || (x[np] - x[nf]) * (x[nb] - x[nf]) + (y[np] - y[nf]) * (y[nb] - 
	    y[nf]) >= 0.) {
	*i1 = nf;
	goto L13;
    }

/*   Bottom of loop. */

L12:
    np = nf;
    nf = nb;
    goto L10;

/* Find the last (leftmost) visible boundary node NL.  NB */
/*   is set to the last neighbor of NL, and NPP is the first */
/*   neighbor. */

L13:
    lp = lend[nl];
    nb = -list[lp];
    if (! left_(&x[nb], &y[nb], &x[nl], &y[nl], &xp, &yp)) {
	goto L14;
    }

/*   P LEFT NB->NL and thus NB is not visible unless an error */
/*     resulted from floating point inaccuracy and collinear- */
/*     ity of the 4 points P, NB, NL, and NPP. */

    if ((x[npp] - x[nl]) * (xp - x[nl]) + (y[npp] - y[nl]) * (yp - y[nl]) >= 
	    0. || (x[npp] - x[nl]) * (x[nb] - x[nl]) + (y[npp] - y[nl]) * (y[
	    nb] - y[nl]) >= 0.) {
	goto L15;
    }

/*   Bottom of loop. */

L14:
    npp = nl;
    nl = nb;
    goto L13;

/* NL is the leftmost visible boundary node. */

L15:
    *i2 = nl;
    *i3 = 0;
    return 0;

/* P is in the triangle (N1,N2,N3). */

L16:
    *i1 = n1;
    *i2 = n2;
    *i3 = n3;
    return 0;

/* All points are collinear. */

L17:
    *i1 = 0;
    *i2 = 0;
    *i3 = 0;
    return 0;
} /* trfind_ */

/* Subroutine */ int trlist_(integer *ncc, integer *lcc, integer *n, integer *
	list, integer *lptr, integer *lend, integer *nrow, integer *nt, 
	integer *ltri, integer *lct, integer *ier)
{
    /* System generated locals */
    integer ltri_dim1, ltri_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, l, i1, i2, i3, n1, n2, n3, ka, kn, lp, nn, kt, nm2, lp2, 
	    lpl, isv, lcc1, n1st;
    logical arcs;
    integer lpln1;
    logical pass2;
    integer jlast;
    logical cstri;


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   03/22/97 */

/*   This subroutine converts a triangulation data structure */
/* from the linked list created by Subroutine TRMESH or */
/* TRMSHR to a triangle list. */

/* On input: */

/*       NCC = Number of constraints.  NCC .GE. 0. */

/*       LCC = List of constraint curve starting indexes (or */
/*             dummy array of length 1 if NCC = 0).  Refer to */
/*             Subroutine ADDCST. */

/*       N = Number of nodes in the triangulation.  N .GE. 3. */

/*       LIST,LPTR,LEND = Linked list data structure defin- */
/*                        ing the triangulation.  Refer to */
/*                        Subroutine TRMESH. */

/*       NROW = Number of rows (entries per triangle) re- */
/*              served for the triangle list LTRI.  The value */
/*              must be 6 if only the vertex indexes and */
/*              neighboring triangle indexes are to be */
/*              stored, or 9 if arc indexes are also to be */
/*              assigned and stored.  Refer to LTRI. */

/* The above parameters are not altered by this routine. */

/*       LTRI = Integer array of length at least NROW*NT, */
/*              where NT is at most 2N-5.  (A sufficient */
/*              length is 12N if NROW=6 or 18N if NROW=9.) */

/*       LCT = Integer array of length NCC or dummy array of */
/*             length 1 if NCC = 0. */

/* On output: */

/*       NT = Number of triangles in the triangulation unless */
/*            IER .NE. 0, in which case NT = 0.  NT = 2N - NB */
/*            - 2, where NB is the number of boundary nodes. */

/*       LTRI = NROW by NT array whose J-th column contains */
/*              the vertex nodal indexes (first three rows), */
/*              neighboring triangle indexes (second three */
/*              rows), and, if NROW = 9, arc indexes (last */
/*              three rows) associated with triangle J for */
/*              J = 1,...,NT.  The vertices are ordered */
/*              counterclockwise with the first vertex taken */
/*              to be the one with smallest index.  Thus, */
/*              LTRI(2,J) and LTRI(3,J) are larger than */
/*              LTRI(1,J) and index adjacent neighbors of */
/*              node LTRI(1,J).  For I = 1,2,3, LTRI(I+3,J) */
/*              and LTRI(I+6,J) index the triangle and arc, */
/*              respectively, which are opposite (not shared */
/*              by) node LTRI(I,J), with LTRI(I+3,J) = 0 if */
/*              LTRI(I+6,J) indexes a boundary arc.  Vertex */
/*              indexes range from 1 to N, triangle indexes */
/*              from 0 to NT, and, if included, arc indexes */
/*              from 1 to NA = NT+N-1.  The triangles are or- */
/*              dered on first (smallest) vertex indexes, */
/*              except that the sets of constraint triangles */
/*              (triangles contained in the closure of a con- */
/*              straint region) follow the non-constraint */
/*              triangles. */

/*       LCT = Array of length NCC containing the triangle */
/*             index of the first triangle of constraint J in */
/*             LCT(J).  Thus, the number of non-constraint */
/*             triangles is LCT(1)-1, and constraint J con- */
/*             tains LCT(J+1)-LCT(J) triangles, where */
/*             LCT(NCC+1) = NT+1. */

/*       IER = Error indicator. */
/*             IER = 0 if no errors were encountered. */
/*             IER = 1 if NCC, N, NROW, or an LCC entry is */
/*                     outside its valid range on input. */
/*             IER = 2 if the triangulation data structure */
/*                     (LIST,LPTR,LEND) is invalid.  Note, */
/*                     however, that these arrays are not */
/*                     completely tested for validity. */

/* Modules required by TRLIST:  None */

/* Intrinsic function called by TRLIST:  ABS */

/* *********************************************************** */


/* Test for invalid input parameters and store the index */
/*   LCC1 of the first constraint node (if any). */

    /* Parameter adjustments */
    --lcc;
    --lend;
    --list;
    --lptr;
    ltri_dim1 = *nrow;
    ltri_offset = 1 + ltri_dim1;
    ltri -= ltri_offset;
    --lct;

    /* Function Body */
    nn = *n;
    if (*ncc < 0 || *nrow != 6 && *nrow != 9) {
	goto L12;
    }
    lcc1 = nn + 1;
    if (*ncc == 0) {
	if (nn < 3) {
	    goto L12;
	}
    } else {
	for (i__ = *ncc; i__ >= 1; --i__) {
	    if (lcc1 - lcc[i__] < 3) {
		goto L12;
	    }
	    lcc1 = lcc[i__];
/* L1: */
	}
	if (lcc1 < 1) {
	    goto L12;
	}
    }

/* Initialize parameters for loop on triangles KT = (N1,N2, */
/*   N3), where N1 < N2 and N1 < N3.  This requires two */
/*   passes through the nodes with all non-constraint */
/*   triangles stored on the first pass, and the constraint */
/*   triangles stored on the second. */

/*   ARCS = TRUE iff arc indexes are to be stored. */
/*   KA,KT = Numbers of currently stored arcs and triangles. */
/*   N1ST = Starting index for the loop on nodes (N1ST = 1 on */
/*            pass 1, and N1ST = LCC1 on pass 2). */
/*   NM2 = Upper bound on candidates for N1. */
/*   PASS2 = TRUE iff constraint triangles are to be stored. */

    arcs = *nrow == 9;
    ka = 0;
    kt = 0;
    n1st = 1;
    nm2 = nn - 2;
    pass2 = FALSE_;

/* Loop on nodes N1:  J = constraint containing N1, */
/*                    JLAST = last node in constraint J. */

L2:
    j = 0;
    jlast = lcc1 - 1;
    i__1 = nm2;
    for (n1 = n1st; n1 <= i__1; ++n1) {
	if (n1 > jlast) {

/* N1 is the first node in constraint J+1.  Update J and */
/*   JLAST, and store the first constraint triangle index */
/*   if in pass 2. */

	    ++j;
	    if (j < *ncc) {
		jlast = lcc[j + 1] - 1;
	    } else {
		jlast = nn;
	    }
	    if (pass2) {
		lct[j] = kt + 1;
	    }
	}

/* Loop on pairs of adjacent neighbors (N2,N3).  LPLN1 points */
/*   to the last neighbor of N1, and LP2 points to N2. */

	lpln1 = lend[n1];
	lp2 = lpln1;
L3:
	lp2 = lptr[lp2];
	n2 = list[lp2];
	lp = lptr[lp2];
	n3 = (i__2 = list[lp], abs(i__2));
	if (n2 < n1 || n3 < n1) {
	    goto L10;
	}

/* (N1,N2,N3) is a constraint triangle iff the three nodes */
/*   are in the same constraint and N2 < N3.  Bypass con- */
/*   straint triangles on pass 1 and non-constraint triangles */
/*   on pass 2. */

	cstri = n1 >= lcc1 && n2 < n3 && n3 <= jlast;
	if (cstri && ! pass2 || ! cstri && pass2) {
	    goto L10;
	}

/* Add a new triangle KT = (N1,N2,N3). */

	++kt;
	ltri[kt * ltri_dim1 + 1] = n1;
	ltri[kt * ltri_dim1 + 2] = n2;
	ltri[kt * ltri_dim1 + 3] = n3;

/* Loop on triangle sides (I1,I2) with neighboring triangles */
/*   KN = (I1,I2,I3). */

	for (i__ = 1; i__ <= 3; ++i__) {
	    if (i__ == 1) {
		i1 = n3;
		i2 = n2;
	    } else if (i__ == 2) {
		i1 = n1;
		i2 = n3;
	    } else {
		i1 = n2;
		i2 = n1;
	    }

/* Set I3 to the neighbor of I1 which follows I2 unless */
/*   I2->I1 is a boundary arc. */

	    lpl = lend[i1];
	    lp = lptr[lpl];
L4:
	    if (list[lp] == i2) {
		goto L5;
	    }
	    lp = lptr[lp];
	    if (lp != lpl) {
		goto L4;
	    }

/*   I2 is the last neighbor of I1 unless the data structure */
/*     is invalid.  Bypass the search for a neighboring */
/*     triangle if I2->I1 is a boundary arc. */

	    if ((i__2 = list[lp], abs(i__2)) != i2) {
		goto L13;
	    }
	    kn = 0;
	    if (list[lp] < 0) {
		goto L8;
	    }

/*   I2->I1 is not a boundary arc, and LP points to I2 as */
/*     a neighbor of I1. */

L5:
	    lp = lptr[lp];
	    i3 = (i__2 = list[lp], abs(i__2));

/* Find L such that LTRI(L,KN) = I3 (not used if KN > KT), */
/*   and permute the vertex indexes of KN so that I1 is */
/*   smallest. */

	    if (i1 < i2 && i1 < i3) {
		l = 3;
	    } else if (i2 < i3) {
		l = 2;
		isv = i1;
		i1 = i2;
		i2 = i3;
		i3 = isv;
	    } else {
		l = 1;
		isv = i1;
		i1 = i3;
		i3 = i2;
		i2 = isv;
	    }

/* Test for KN > KT (triangle index not yet assigned). */

	    if (i1 > n1 && ! pass2) {
		goto L9;
	    }

/* Find KN, if it exists, by searching the triangle list in */
/*   reverse order. */

	    for (kn = kt - 1; kn >= 1; --kn) {
		if (ltri[kn * ltri_dim1 + 1] == i1 && ltri[kn * ltri_dim1 + 2]
			 == i2 && ltri[kn * ltri_dim1 + 3] == i3) {
		    goto L7;
		}
/* L6: */
	    }
	    goto L9;

/* Store KT as a neighbor of KN. */

L7:
	    ltri[l + 3 + kn * ltri_dim1] = kt;

/* Store KN as a neighbor of KT, and add a new arc KA. */

L8:
	    ltri[i__ + 3 + kt * ltri_dim1] = kn;
	    if (arcs) {
		++ka;
		ltri[i__ + 6 + kt * ltri_dim1] = ka;
		if (kn != 0) {
		    ltri[l + 6 + kn * ltri_dim1] = ka;
		}
	    }
L9:
	    ;
	}

/* Bottom of loop on triangles. */

L10:
	if (lp2 != lpln1) {
	    goto L3;
	}
/* L11: */
    }

/* Bottom of loop on nodes. */

    if (! pass2 && *ncc > 0) {
	pass2 = TRUE_;
	n1st = lcc1;
	goto L2;
    }

/* No errors encountered. */

    *nt = kt;
    *ier = 0;
    return 0;

/* Invalid input parameter. */

L12:
    *nt = 0;
    *ier = 1;
    return 0;

/* Invalid triangulation data structure:  I1 is a neighbor of */
/*   I2, but I2 is not a neighbor of I1. */

L13:
    *nt = 0;
    *ier = 2;
    return 0;
} /* trlist_ */


/* Subroutine */ int trmesh_(integer *n, doublereal *x, doublereal *y, 
	integer *list, integer *lptr, integer *lend, integer *lnew, integer *
	near__, integer *next, doublereal *dist, integer *ier)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2;

    /* Local variables */
    doublereal d__;
    integer i__, j, k;
    doublereal d1, d2, d3;
    integer i0, lp, nn, km1, lcc[1], ncc, lpl;
    doublereal eps;
    extern logical left_(doublereal *, doublereal *, doublereal *, doublereal 
	    *, doublereal *, doublereal *);
    integer nexti;
    extern doublereal store_(doublereal *);
    extern /* Subroutine */ int addnod_(integer *, doublereal *, doublereal *,
	     integer *, integer *, integer *, integer *, doublereal *, 
	    doublereal *, integer *, integer *, integer *, integer *, integer 
	    *);


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   06/28/98 */

/*   This subroutine creates a Delaunay triangulation of a */
/* set of N arbitrarily distributed points in the plane re- */
/* ferred to as nodes.  The Delaunay triangulation is defined */
/* as a set of triangles with the following five properties: */

/*  1)  The triangle vertices are nodes. */
/*  2)  No triangle contains a node other than its vertices. */
/*  3)  The interiors of the triangles are pairwise disjoint. */
/*  4)  The union of triangles is the convex hull of the set */
/*        of nodes (the smallest convex set which contains */
/*        the nodes). */
/*  5)  The interior of the circumcircle of each triangle */
/*        contains no node. */

/* The first four properties define a triangulation, and the */
/* last property results in a triangulation which is as close */
/* as possible to equiangular in a certain sense and which is */
/* uniquely defined unless four or more nodes lie on a common */
/* circle.  This property makes the triangulation well-suited */
/* for solving closest point problems and for triangle-based */
/* interpolation. */

/*   The triangulation can be generalized to a constrained */
/* Delaunay triangulation by a call to Subroutine ADDCST. */
/* This allows for user-specified boundaries defining a non- */
/* convex and/or multiply connected region. */

/*   The algorithm for constructing the triangulation has */
/* expected time complexity O(N*log(N)) for most nodal dis- */
/* tributions.  Also, since the algorithm proceeds by adding */
/* nodes incrementally, the triangulation may be updated with */
/* the addition (or deletion) of a node very efficiently. */
/* The adjacency information representing the triangulation */
/* is stored as a linked list requiring approximately 13N */
/* storage locations. */


/*   The following is a list of the software package modules */
/* which a user may wish to call directly: */

/*  ADDCST - Generalizes the Delaunay triangulation to allow */
/*             for user-specified constraints. */

/*  ADDNOD - Updates the triangulation by appending or */
/*             inserting a new node. */

/*  AREAP  - Computes the area bounded by a closed polygonal */
/*             curve such as the boundary of the triangula- */
/*             tion or of a constraint region. */

/*  BNODES - Returns an array containing the indexes of the */
/*             boundary nodes in counterclockwise order. */
/*             Counts of boundary nodes, triangles, and arcs */
/*             are also returned. */

/*  CIRCUM - Computes the area, circumcenter, circumradius, */
/*             and, optionally, the aspect ratio of a trian- */
/*             gle defined by user-specified vertices. */

/*  DELARC - Deletes a boundary arc from the triangulation. */

/*  DELNOD - Updates the triangulation with the deletion of a */
/*             node. */

/*  EDGE   - Forces a pair of nodes to be connected by an arc */
/*             in the triangulation. */

/*  GETNP  - Determines the ordered sequence of L closest */
/*             nodes to a given node, along with the associ- */
/*             ated distances.  The distance between nodes is */
/*             taken to be the length of the shortest connec- */
/*             ting path which intersects no constraint */
/*             region. */

/*  INTSEC - Determines whether or not an arbitrary pair of */
/*             line segments share a common point. */

/*  JRAND  - Generates a uniformly distributed pseudo-random */
/*             integer. */

/*  LEFT   - Locates a point relative to a line. */

/*  NEARND - Returns the index of the nearest node to an */
/*             arbitrary point, along with its squared */
/*             distance. */

/*  STORE  - Forces a value to be stored in main memory so */
/*             that the precision of floating point numbers */
/*             in memory locations rather than registers is */
/*             computed. */

/*  TRLIST - Converts the triangulation data structure to a */
/*             triangle list more suitable for use in a fin- */
/*             ite element code. */

/*  TRLPRT - Prints the triangle list created by Subroutine */
/*             TRLIST. */

/*  TRMESH - Creates a Delaunay triangulation of a set of */
/*             nodes. */

/*  TRMSHR - Creates a Delaunay triangulation (more effici- */
/*             ently than TRMESH) of a set of nodes lying at */
/*             the vertices of a (possibly skewed) rectangu- */
/*             lar grid. */

/*  TRPLOT - Creates a level-2 Encapsulated Postscript (EPS) */
/*             file containing a triangulation plot. */

/*  TRPRNT - Prints the triangulation data structure and, */
/*             optionally, the nodal coordinates. */


/* On input: */

/*       N = Number of nodes in the triangulation.  N .GE. 3. */

/*       X,Y = Arrays of length N containing the Cartesian */
/*             coordinates of the nodes.  (X(K),Y(K)) is re- */
/*             ferred to as node K, and K is referred to as */
/*             a nodal index.  The first three nodes must not */
/*             be collinear. */

/* The above parameters are not altered by this routine. */

/*       LIST,LPTR = Arrays of length at least 6N-12. */

/*       LEND = Array of length at least N. */

/*       NEAR,NEXT,DIST = Work space arrays of length at */
/*                        least N.  The space is used to */
/*                        efficiently determine the nearest */
/*                        triangulation node to each un- */
/*                        processed node for use by ADDNOD. */

/* On output: */

/*       LIST = Set of nodal indexes which, along with LPTR, */
/*              LEND, and LNEW, define the triangulation as a */
/*              set of N adjacency lists -- counterclockwise- */
/*              ordered sequences of neighboring nodes such */
/*              that the first and last neighbors of a bound- */
/*              ary node are boundary nodes (the first neigh- */
/*              bor of an interior node is arbitrary).  In */
/*              order to distinguish between interior and */
/*              boundary nodes, the last neighbor of each */
/*              boundary node is represented by the negative */
/*              of its index. */

/*       LPTR = Set of pointers (LIST indexes) in one-to-one */
/*              correspondence with the elements of LIST. */
/*              LIST(LPTR(I)) indexes the node which follows */
/*              LIST(I) in cyclical counterclockwise order */
/*              (the first neighbor follows the last neigh- */
/*              bor). */

/*       LEND = Set of pointers to adjacency lists.  LEND(K) */
/*              points to the last neighbor of node K for */
/*              K = 1,...,N.  Thus, LIST(LEND(K)) < 0 if and */
/*              only if K is a boundary node. */

/*       LNEW = Pointer to the first empty location in LIST */
/*              and LPTR (list length plus one).  LIST, LPTR, */
/*              LEND, and LNEW are not altered if IER < 0, */
/*              and are incomplete if IER > 0. */

/*       NEAR,NEXT,DIST = Garbage. */

/*       IER = Error indicator: */
/*             IER =  0 if no errors were encountered. */
/*             IER = -1 if N < 3 on input. */
/*             IER = -2 if the first three nodes are */
/*                      collinear. */
/*             IER = -4 if an error flag was returned by a */
/*                      call to SWAP in ADDNOD.  This is an */
/*                      internal error and should be reported */
/*                      to the programmer. */
/*             IER =  L if nodes L and M coincide for some */
/*                      M > L.  The linked list represents */
/*                      a triangulation of nodes 1 to M-1 */
/*                      in this case. */

/* Modules required by TRMESH:  ADDNOD, BDYADD, INSERT, */
/*                                INTADD, JRAND, LEFT, */
/*                                LSTPTR, STORE, SWAP, */
/*                                SWPTST, TRFIND */

/* Intrinsic function called by TRMESH:  ABS */

/* *********************************************************** */


/* Local parameters: */

/* D =        Squared distance from node K to node I */
/* D1,D2,D3 = Squared distances from node K to nodes 1, 2, */
/*              and 3, respectively */
/* EPS =      Half the machine precision */
/* I,J =      Nodal indexes */
/* I0 =       Index of the node preceding I in a sequence of */
/*              unprocessed nodes:  I = NEXT(I0) */
/* K =        Index of node to be added and DO-loop index: */
/*              K > 3 */
/* KM1 =      K-1 */
/* LCC(1) =   Dummy array */
/* LP =       LIST index (pointer) of a neighbor of K */
/* LPL =      Pointer to the last neighbor of K */
/* NCC =      Number of constraint curves */
/* NEXTI =    NEXT(I) */
/* NN =       Local copy of N */
/* SWTOL =    Tolerance for function SWPTST */

    /* Parameter adjustments */
    --dist;
    --next;
    --near__;
    --lend;
    --y;
    --x;
    --list;
    --lptr;

    /* Function Body */
    nn = *n;
    if (nn < 3) {
	*ier = -1;
	return 0;
    }

/* Compute a tolerance for function SWPTST:  SWTOL = 10* */
/*   (machine precision) */

    eps = 1.;
L1:
    eps /= 2.;
    d__1 = eps + 1.;
    swpcom_1.swtol = store_(&d__1);
    if (swpcom_1.swtol > 1.) {
	goto L1;
    }
    swpcom_1.swtol = eps * 20.;

/* Store the first triangle in the linked list. */

    if (! left_(&x[1], &y[1], &x[2], &y[2], &x[3], &y[3])) {

/*   The initial triangle is (3,2,1) = (2,1,3) = (1,3,2). */

	list[1] = 3;
	lptr[1] = 2;
	list[2] = -2;
	lptr[2] = 1;
	lend[1] = 2;

	list[3] = 1;
	lptr[3] = 4;
	list[4] = -3;
	lptr[4] = 3;
	lend[2] = 4;

	list[5] = 2;
	lptr[5] = 6;
	list[6] = -1;
	lptr[6] = 5;
	lend[3] = 6;

    } else if (! left_(&x[2], &y[2], &x[1], &y[1], &x[3], &y[3])) {

/*   The initial triangle is (1,2,3). */

	list[1] = 2;
	lptr[1] = 2;
	list[2] = -3;
	lptr[2] = 1;
	lend[1] = 2;

	list[3] = 3;
	lptr[3] = 4;
	list[4] = -1;
	lptr[4] = 3;
	lend[2] = 4;

	list[5] = 1;
	lptr[5] = 6;
	list[6] = -2;
	lptr[6] = 5;
	lend[3] = 6;

    } else {

/*   The first three nodes are collinear. */

	*ier = -2;
	return 0;
    }

/* Initialize LNEW and test for N = 3. */

    *lnew = 7;
    if (nn == 3) {
	*ier = 0;
	return 0;
    }

/* A nearest-node data structure (NEAR, NEXT, and DIST) is */
/*   used to obtain an expected-time (N*log(N)) incremental */
/*   algorithm by enabling constant search time for locating */
/*   each new node in the triangulation. */

/* For each unprocessed node K, NEAR(K) is the index of the */
/*   triangulation node closest to K (used as the starting */
/*   point for the search in Subroutine TRFIND) and DIST(K) */
/*   is an increasing function of the distance between nodes */
/*   K and NEAR(K). */

/* Since it is necessary to efficiently find the subset of */
/*   unprocessed nodes associated with each triangulation */
/*   node J (those that have J as their NEAR entries), the */
/*   subsets are stored in NEAR and NEXT as follows:  for */
/*   each node J in the triangulation, I = NEAR(J) is the */
/*   first unprocessed node in J's set (with I = 0 if the */
/*   set is empty), L = NEXT(I) (if I > 0) is the second, */
/*   NEXT(L) (if L > 0) is the third, etc.  The nodes in each */
/*   set are initially ordered by increasing indexes (which */
/*   maximizes efficiency) but that ordering is not main- */
/*   tained as the data structure is updated. */

/* Initialize the data structure for the single triangle. */

    near__[1] = 0;
    near__[2] = 0;
    near__[3] = 0;
    for (k = nn; k >= 4; --k) {
/* Computing 2nd power */
	d__1 = x[k] - x[1];
/* Computing 2nd power */
	d__2 = y[k] - y[1];
	d1 = d__1 * d__1 + d__2 * d__2;
/* Computing 2nd power */
	d__1 = x[k] - x[2];
/* Computing 2nd power */
	d__2 = y[k] - y[2];
	d2 = d__1 * d__1 + d__2 * d__2;
/* Computing 2nd power */
	d__1 = x[k] - x[3];
/* Computing 2nd power */
	d__2 = y[k] - y[3];
	d3 = d__1 * d__1 + d__2 * d__2;
	if (d1 <= d2 && d1 <= d3) {
	    near__[k] = 1;
	    dist[k] = d1;
	    next[k] = near__[1];
	    near__[1] = k;
	} else if (d2 <= d1 && d2 <= d3) {
	    near__[k] = 2;
	    dist[k] = d2;
	    next[k] = near__[2];
	    near__[2] = k;
	} else {
	    near__[k] = 3;
	    dist[k] = d3;
	    next[k] = near__[3];
	    near__[3] = k;
	}
/* L2: */
    }

/* Add the remaining nodes.  Parameters for ADDNOD are as */
/*   follows: */

/*   K = Index of the node to be added. */
/*   NEAR(K) = Index of the starting node for the search in */
/*             TRFIND. */
/*   NCC = Number of constraint curves. */
/*   LCC = Dummy array (since NCC = 0). */
/*   KM1 = Number of nodes in the triangulation. */

    ncc = 0;
    i__1 = nn;
    for (k = 4; k <= i__1; ++k) {
	km1 = k - 1;
	addnod_(&k, &x[k], &y[k], &near__[k], &ncc, lcc, &km1, &x[1], &y[1], &
		list[1], &lptr[1], &lend[1], lnew, ier);
	if (*ier != 0) {
	    return 0;
	}

/* Remove K from the set of unprocessed nodes associated */
/*   with NEAR(K). */

	i__ = near__[k];
	if (near__[i__] == k) {
	    near__[i__] = next[k];
	} else {
	    i__ = near__[i__];
L3:
	    i0 = i__;
	    i__ = next[i0];
	    if (i__ != k) {
		goto L3;
	    }
	    next[i0] = next[k];
	}
	near__[k] = 0;

/* Loop on neighbors J of node K. */

	lpl = lend[k];
	lp = lpl;
L4:
	lp = lptr[lp];
	j = (i__2 = list[lp], abs(i__2));

/* Loop on elements I in the sequence of unprocessed nodes */
/*   associated with J:  K is a candidate for replacing J */
/*   as the nearest triangulation node to I.  The next value */
/*   of I in the sequence, NEXT(I), must be saved before I */
/*   is moved because it is altered by adding I to K's set. */

	i__ = near__[j];
L5:
	if (i__ == 0) {
	    goto L6;
	}
	nexti = next[i__];

/* Test for the distance from I to K less than the distance */
/*   from I to J. */

/* Computing 2nd power */
	d__1 = x[k] - x[i__];
/* Computing 2nd power */
	d__2 = y[k] - y[i__];
	d__ = d__1 * d__1 + d__2 * d__2;
	if (d__ < dist[i__]) {

/* Replace J by K as the nearest triangulation node to I: */
/*   update NEAR(I) and DIST(I), and remove I from J's set */
/*   of unprocessed nodes and add it to K's set. */

	    near__[i__] = k;
	    dist[i__] = d__;
	    if (i__ == near__[j]) {
		near__[j] = nexti;
	    } else {
		next[i0] = nexti;
	    }
	    next[i__] = near__[k];
	    near__[k] = i__;
	} else {
	    i0 = i__;
	}

/* Bottom of loop on I. */

	i__ = nexti;
	goto L5;

/* Bottom of loop on neighbors J. */

L6:
	if (lp != lpl) {
	    goto L4;
	}
/* L7: */
    }
    return 0;
} /* trmesh_ */

/* Subroutine */ int trmshr_(integer *n, integer *nx, doublereal *x, 
	doublereal *y, integer *nit, integer *list, integer *lptr, integer *
	lend, integer *lnew, integer *ier)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    doublereal d__1;

    /* Local variables */
    integer i__, j, k, m1, m2, m3, m4, n0, n1, n2, n3, n4, ni, nj, lp, nn, 
	    kp1, nm1, nnb, lpf, lpk, lpl;
    doublereal eps;
    integer lpp;
    logical tst;
    extern logical left_(doublereal *, doublereal *, doublereal *, doublereal 
	    *, doublereal *, doublereal *);
    integer iter;
    extern /* Subroutine */ int swap_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *);
    extern integer nbcnt_(integer *, integer *);
    integer maxit;
    extern doublereal store_(doublereal *);
    extern /* Subroutine */ int insert_(integer *, integer *, integer *, 
	    integer *, integer *);
    extern integer lstptr_(integer *, integer *, integer *, integer *);
    extern logical swptst_(integer *, integer *, integer *, integer *, 
	    doublereal *, doublereal *);


/* *********************************************************** */

/*                                               From TRIPACK */
/*                                            Robert J. Renka */
/*                                  Dept. of Computer Science */
/*                                       Univ. of North Texas */
/*                                           renka@cs.unt.edu */
/*                                                   06/27/98 */

/*   This subroutine creates a Delaunay triangulation of a */
/* set of N nodes in the plane, where the nodes are the vert- */
/* ices of an NX by NY skewed rectangular grid with the */
/* natural ordering.  Thus, N = NX*NY, and the nodes are */
/* ordered from left to right beginning at the top row so */
/* that adjacent nodes have indexes which differ by 1 in the */
/* x-direction and by NX in the y-direction.  A skewed rec- */
/* tangular grid is defined as one in which each grid cell is */
/* a strictly convex quadrilateral (and is thus the convex */
/* hull of its four vertices).  Equivalently, any transfor- */
/* mation from a rectangle to a grid cell which is bilinear */
/* in both components has an invertible Jacobian. */

/*   If the nodes are not distributed and ordered as defined */
/* above, Subroutine TRMESH must be called in place of this */
/* routine.  Refer to Subroutine ADDCST for the treatment of */
/* constraints. */

/*   The first phase of the algorithm consists of construc- */
/* ting a triangulation by choosing a diagonal arc in each */
/* grid cell.  If NIT = 0, all diagonals connect lower left */
/* to upper right corners and no error checking or additional */
/* computation is performed.  Otherwise, each diagonal arc is */
/* chosen to be locally optimal, and boundary arcs are added */
/* where necessary in order to cover the convex hull of the */
/* nodes.  (This is the first iteration.)  If NIT > 1 and no */
/* error was detected, the triangulation is then optimized by */
/* a sequence of up to NIT-1 iterations in which interior */
/* arcs of the triangulation are tested and swapped if appro- */
/* priate.  The algorithm terminates when an iteration */
/* results in no swaps and/or when the allowable number of */
/* iterations has been performed.  NIT = 0 is sufficient to */
/* produce a Delaunay triangulation if the original grid is */
/* actually rectangular, and NIT = 1 is sufficient if it is */
/* close to rectangular.  Note, however, that the ordering */
/* and distribution of nodes is not checked for validity in */
/* the case NIT = 0, and the triangulation will not be valid */
/* unless the rectangular grid covers the convex hull of the */
/* nodes. */


/* On input: */

/*       N = Number of nodes in the grid.  N = NX*NY for some */
/*           NY .GE. 2. */

/*       NX = Number of grid points in the x-direction.  NX */
/*            .GE. 2. */

/*       X,Y = Arrays of length N containing coordinates of */
/*             the nodes with the ordering and distribution */
/*             defined in the header comments above. */
/*             (X(K),Y(K)) is referred to as node K. */

/* The above parameters are not altered by this routine. */

/*       NIT = Nonnegative integer specifying the maximum */
/*             number of iterations to be employed.  Refer */
/*             to the header comments above. */

/*       LIST,LPTR = Arrays of length at least 6N-12. */

/*       LEND = Array of length at least N. */

/* On output: */

/*       NIT = Number of iterations employed. */

/*       LIST,LPTR,LEND,LNEW = Data structure defining the */
/*                             triangulation.  Refer to Sub- */
/*                             routine TRMESH. */

/*       IER = Error indicator: */
/*             IER = 0 if no errors were encountered. */
/*             IER = K if the grid element with upper left */
/*                     corner at node K is not a strictly */
/*                     convex quadrilateral.  The algorithm */
/*                     is terminated when the first such */
/*                     occurrence is detected.  Note that */
/*                     this test is not performed if NIT = 0 */
/*                     on input. */
/*             IER = -1 if N, NX, or NIT is outside its valid */
/*                      range on input. */
/*             IER = -2 if NIT > 1 on input, and the optimi- */
/*                      zation loop failed to converge within */
/*                      the allowable number of iterations. */
/*                      The triangulation is valid but not */
/*                      optimal in this case. */

/* Modules required by TRMSHR:  INSERT, LEFT, LSTPTR, NBCNT, */
/*                                STORE, SWAP, SWPTST */

/* Intrinsic function called by TRMSHR:  ABS */

/* *********************************************************** */


/* Store local variables and test for errors in input */
/*   parameters. */

    /* Parameter adjustments */
    --lend;
    --y;
    --x;
    --list;
    --lptr;

    /* Function Body */
    ni = *nx;
    nj = *n / ni;
    nn = ni * nj;
    maxit = *nit;
    *nit = 0;
    if (*n != nn || nj < 2 || ni < 2 || maxit < 0) {
	*ier = -1;
	return 0;
    }
    *ier = 0;

/* Compute a tolerance for function SWPTST:  SWTOL = 10* */
/*   (machine precision) */

    eps = 1.;
L1:
    eps /= 2.;
    d__1 = eps + 1.;
    swpcom_1.swtol = store_(&d__1);
    if (swpcom_1.swtol > 1.) {
	goto L1;
    }
    swpcom_1.swtol = eps * 20.;

/* Loop on grid points (I,J) corresponding to nodes K = */
/*   (J-1)*NI + I.  TST = TRUE iff diagonals are to be */
/*   chosen by the swap test.  M1, M2, M3, and M4 are the */
/*   slopes (-1, 0, or 1) of the diagonals in quadrants 1 */
/*   to 4 (counterclockwise beginning with the upper right) */
/*   for a coordinate system with origin at node K. */

    tst = maxit > 0;
    m1 = 0;
    m4 = 0;
    lp = 0;
    kp1 = 1;
    i__1 = nj;
    for (j = 1; j <= i__1; ++j) {
	i__2 = ni;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    m2 = m1;
	    m3 = m4;
	    k = kp1;
	    kp1 = k + 1;
	    lpf = lp + 1;
	    if (j == nj && i__ != ni) {
		goto L2;
	    }
	    if (i__ != 1) {
		if (j != 1) {

/*   K is not in the top row, leftmost column, or bottom row */
/*     (unless K is the lower right corner).  Take the first */
/*     neighbor to be the node above K. */

		    ++lp;
		    list[lp] = k - ni;
		    lptr[lp] = lp + 1;
		    if (m2 <= 0) {
			++lp;
			list[lp] = k - 1 - ni;
			lptr[lp] = lp + 1;
		    }
		}

/*   K is not in the leftmost column.  The next (or first) */
/*     neighbor is to the left of K. */

		++lp;
		list[lp] = k - 1;
		lptr[lp] = lp + 1;
		if (j == nj) {
		    goto L3;
		}
		if (m3 >= 0) {
		    ++lp;
		    list[lp] = k - 1 + ni;
		    lptr[lp] = lp + 1;
		}
	    }

/*   K is not in the bottom row.  The next (or first) */
/*     neighbor is below K. */

	    ++lp;
	    list[lp] = k + ni;
	    lptr[lp] = lp + 1;

/*   Test for a negative diagonal in quadrant 4 unless K is */
/*     in the rightmost column.  The quadrilateral associated */
/*     with the quadrant is tested for strict convexity un- */
/*     less NIT = 0 on input. */

	    if (i__ == ni) {
		goto L3;
	    }
	    m4 = 1;
	    if (! tst) {
		goto L2;
	    }
	    if (left_(&x[kp1], &y[kp1], &x[k + ni], &y[k + ni], &x[k], &y[k]) 
		    || left_(&x[k], &y[k], &x[kp1 + ni], &y[kp1 + ni], &x[k + 
		    ni], &y[k + ni]) || left_(&x[k + ni], &y[k + ni], &x[kp1],
		     &y[kp1], &x[kp1 + ni], &y[kp1 + ni]) || left_(&x[kp1 + 
		    ni], &y[kp1 + ni], &x[k], &y[k], &x[kp1], &y[kp1])) {
		goto L12;
	    }
	    i__3 = k + ni;
	    i__4 = kp1 + ni;
	    if (swptst_(&kp1, &i__3, &k, &i__4, &x[1], &y[1])) {
		goto L2;
	    }
	    m4 = -1;
	    ++lp;
	    list[lp] = kp1 + ni;
	    lptr[lp] = lp + 1;

/*   The next (or first) neighbor is to the right of K. */

L2:
	    ++lp;
	    list[lp] = kp1;
	    lptr[lp] = lp + 1;

/*   Test for a positive diagonal in quadrant 1 (the neighbor */
/*     of K-NI which follows K is not K+1) unless K is in the */
/*     top row. */

	    if (j == 1) {
		goto L3;
	    }
	    if (tst) {
		m1 = -1;
		lpk = lstptr_(&lend[k - ni], &k, &list[1], &lptr[1]);
		lpk = lptr[lpk];
		if (list[lpk] != kp1) {
		    m1 = 1;
		    ++lp;
		    list[lp] = kp1 - ni;
		    lptr[lp] = lp + 1;
		}
	    }

/*   If K is in the leftmost column (and not the top row) or */
/*     in the bottom row (and not the rightmost column), then */
/*     the next neighbor is the node above K. */

	    if (i__ != 1 && j != nj) {
		goto L4;
	    }
	    ++lp;
	    list[lp] = k - ni;
	    lptr[lp] = lp + 1;
	    if (i__ == 1) {
		goto L3;
	    }

/*   K is on the bottom row (and not the leftmost or right- */
/*     most column). */

	    if (m2 <= 0) {
		++lp;
		list[lp] = k - 1 - ni;
		lptr[lp] = lp + 1;
	    }
	    ++lp;
	    list[lp] = k - 1;
	    lptr[lp] = lp + 1;

/*   K is a boundary node. */

L3:
	    list[lp] = -list[lp];

/*   Bottom of loop.  Store LEND and correct LPTR(LP). */
/*     LPF and LP point to the first and last neighbors */
/*     of K. */

L4:
	    lend[k] = lp;
	    lptr[lp] = lpf;
/* L5: */
	}
/* L6: */
    }

/* Store LNEW, and terminate the algorithm if NIT = 0 on */
/*   input. */

    *lnew = lp + 1;
    if (maxit == 0) {
	return 0;
    }

/* Add boundary arcs where necessary in order to cover the */
/*   convex hull of the nodes.  N1, N2, and N3 are consecu- */
/*   tive boundary nodes in counterclockwise order, and N0 */
/*   is the starting point for each loop around the boundary. */

    n0 = 1;
    n1 = n0;
    n2 = ni + 1;

/*   TST is set to TRUE if an arc is added.  The boundary */
/*     loop is repeated until a traversal results in no */
/*     added arcs. */

L7:
    tst = FALSE_;

/*   Top of boundary loop.  Set N3 to the first neighbor of */
/*     N2, and test for N3 LEFT N1 -> N2. */

L8:
    lpl = lend[n2];
    lp = lptr[lpl];
    n3 = list[lp];
    if (left_(&x[n1], &y[n1], &x[n2], &y[n2], &x[n3], &y[n3])) {
	n1 = n2;
    }
    if (n1 != n2) {

/*   Add the boundary arc N1-N3.  If N0 = N2, the starting */
/*     point is changed to N3, since N2 will be removed from */
/*     the boundary.  N3 is inserted as the first neighbor of */
/*     N1, N2 is changed to an interior node, and N1 is */
/*     inserted as the last neighbor of N3. */

	tst = TRUE_;
	if (n2 == n0) {
	    n0 = n3;
	}
	lp = lend[n1];
	insert_(&n3, &lp, &list[1], &lptr[1], lnew);
	list[lpl] = -list[lpl];
	lp = lend[n3];
	list[lp] = n2;
	i__1 = -n1;
	insert_(&i__1, &lp, &list[1], &lptr[1], lnew);
	lend[n3] = *lnew - 1;
    }

/*   Bottom of loops.  Test for termination. */

    n2 = n3;
    if (n1 != n0) {
	goto L8;
    }
    if (tst) {
	goto L7;
    }

/* Terminate the algorithm if NIT = 1 on input. */

    *nit = 1;
    if (maxit == 1) {
	return 0;
    }

/* Optimize the triangulation by applying the swap test and */
/*   appropriate swaps to the interior arcs.  The loop is */
/*   repeated until no swaps are performed or MAXIT itera- */
/*   tions have been applied.  ITER is the current iteration, */
/*   and TST is set to TRUE if a swap occurs. */

    iter = 1;
    nm1 = nn - 1;
L9:
    ++iter;
    tst = FALSE_;

/*   Loop on interior arcs N1-N2, where N2 > N1 and */
/*     (N1,N2,N3) and (N2,N1,N4) are adjacent triangles. */

/*   Top of loop on nodes N1. */

    i__1 = nm1;
    for (n1 = 1; n1 <= i__1; ++n1) {
	lpl = lend[n1];
	n4 = list[lpl];
	lpf = lptr[lpl];
	n2 = list[lpf];
	lp = lptr[lpf];
	n3 = list[lp];
	nnb = nbcnt_(&lpl, &lptr[1]);

/*   Top of loop on neighbors N2 of N1.  NNB is the number of */
/*                                       neighbors of N1. */

	i__2 = nnb;
	for (i__ = 1; i__ <= i__2; ++i__) {

/*   Bypass the swap test if N1 is a boundary node and N2 is */
/*     the first neighbor (N4 < 0), N2 < N1, or N1-N2 is a */
/*     diagonal arc (already locally optimal) when ITER = 2. */

	    if (n4 > 0 && n2 > n1 && (iter != 2 || (i__3 = n1 + ni - n2, abs(
		    i__3)) != 1)) {
		if (swptst_(&n3, &n4, &n1, &n2, &x[1], &y[1])) {

/*   Swap diagonal N1-N2 for N3-N4, set TST to TRUE, and set */
/*     N2 to N4 (the neighbor preceding N3). */

		    swap_(&n3, &n4, &n1, &n2, &list[1], &lptr[1], &lend[1], &
			    lpp);
		    if (lpp != 0) {
			tst = TRUE_;
			n2 = n4;
		    }
		}
	    }

/*   Bottom of neighbor loop. */

	    if (list[lpl] == -n3) {
		goto L11;
	    }
	    n4 = n2;
	    n2 = n3;
	    lp = lstptr_(&lpl, &n2, &list[1], &lptr[1]);
	    lp = lptr[lp];
	    n3 = (i__3 = list[lp], abs(i__3));
/* L10: */
	}
L11:
	;
    }

/*   Test for termination. */

    if (tst && iter < maxit) {
	goto L9;
    }
    *nit = iter;
    if (tst) {
	*ier = -2;
    }
    return 0;

/* Invalid grid cell encountered. */

L12:
    *ier = k;
    return 0;
} /* trmshr_ */

}

#ifdef __cplusplus
	}
#endif
