/*------------------------------------------------------------------------------
 test suite for IDL/GDL call_external 

 Generate a shared object with something like
 cc -o libtest_ce.so libtest_ce.cpp -shared -fPIC   (gcc)
 cc -o libtest_ce.so libtest_ce.cpp -G      -KPIC   (sunstudio cc)

 then run GDL/IDL and call test_ce (from the file test_ce.pro)

-------------------------------------------------------------------------------*/
			  
extern "C" {

#include <stdio.h>
#include <string.h>

typedef struct {
   int slen;           /* length of the string         */
   short stype;        /* Type of string               */
   char *s;            /* Pointer to chararcter array  */
} STRING;
#define S(x) (((STRING*)x)->s)		/* The string itself */
#define L(x) ((long int)((STRING*)x)->slen)  /* Length of the string */ /* Long, weil wir Fortran rufen */

typedef struct {
  float r,i;
} IDL_COMPLEX;

typedef struct {
  double r,i;
} IDL_DCOMPLEX;

#define EPSILON 1e-4
#define DEPSILON 1e-8
#define ABS(x)   (x > 0.  ? x : -(x) )

char testce_byte(int argc, void* argv[]) {

    printf("Starting C part of testce_byte ... ");

    if (   argc != 3
	|| *((char*)argv[0]+0) != 1
	|| *((char*)argv[1]+0) != 2
	|| *((char*)argv[1]+1) != 3
//??	|| *((char*)argv[2]+0) != 4
    ) {

	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n\n", 3);
	printf("First arg:           Got: %d\n", *(char*)argv[0]);
	printf("                Expected: %d\n\n", 1);
	printf("Second arg:          Got: %d %d\n", *((char*)argv[1]+0), *((char*)argv[1]+1));
	printf("                Expected: %d %d\n\n", 2, 3);
//??	printf("Third arg:           Got: %d\n", *(char*)argv[2]);
//??	printf("                Expected: %d\n\n", 4);

    }
    else {
	printf("OK\n");

	*(char*)argv[0]     = 11;
	*((char*)argv[1]+0) = 12;
	*((char*)argv[1]+1) = 13;
    }

    return 14;
}


short testce_int(int argc, void* argv[]) {

    printf("Starting C part of testce_int ... ");

    if (   argc != 3
	|| *(short*)argv[0] != 1
	|| *((short*)argv[1]+0) != 2
	|| *((short*)argv[1]+1) != 3
//??	|| *(short*)argv[2] != 4
    ) {

	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n\n", 3);
	printf("First arg:           Got: %d\n", *(short*)argv[0]);
	printf("                Expected: %d\n\n", 1);
	printf("Second arg:          Got: %d %d\n", *((short*)argv[1]+0), *((short*)argv[1]+1));
	printf("                Expected: %d %d\n\n", 2, 3);
//??	printf("Third arg:           Got: %d\n", *(short*)argv[2]);
//??	printf("                Expected: %d\n\n", 4);

    }
    else {
	printf("OK\n");

	*(short*)argv[0]     = -11;
	*((short*)argv[1]+0) = 12;
	*((short*)argv[1]+1) = 13;
    }

    return 14;
}

int testce_long(int argc, void* argv[]) {

    printf("Starting C part of testce_long ... ");

    if (   argc != 3
	|| *(int*)argv[0] != 1
	|| *((int*)argv[1]+0) != 2
	|| *((int*)argv[1]+1) != 3
//??	|| *(int*)argv[2] != 4
    ) {

	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n\n", 3);
	printf("First arg:           Got: %d\n", *(int*)argv[0]);
	printf("                Expected: %d\n\n", 1);
	printf("Second arg:          Got: %d %d\n", *((int*)argv[1]+0), *((int*)argv[1]+1));
	printf("                Expected: %d %d\n\n", 2, 3);
//??	printf("Third arg:           Got: %d\n", *(int*)argv[2]);
//??	printf("                Expected: %d\n\n", 4);

    }
    else {
	printf("OK\n");

	*(int*)argv[0]     = -11;
	*((int*)argv[1]+0) = 12;
	*((int*)argv[1]+1) = 13;
    }

    return 14;
}


float testce_float(int argc, void* argv[]) {

    printf("Starting C part of testce_float ... ");

    float f;
    memcpy(&f, argv+2, sizeof(float));

    if (   argc != 3
	|| ABS( *(float*)argv[0] - 1.11 ) > EPSILON
	|| ABS( *((float*)argv[1]+0) - 2.22 ) > EPSILON
	|| ABS( *((float*)argv[1]+1) - 3.33 ) > EPSILON
	|| ABS( f - 4.44 ) > EPSILON
    ) {

	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n\n", 3);
	printf("First arg:           Got: %f\n", *(float*)argv[0]);
	printf("                Expected: %f\n\n", 1.11);
	printf("Second arg:          Got: %f %f\n", *((float*)argv[1]+0), *((float*)argv[1]+1));
	printf("                Expected: %f %f\n\n", 2.22, 3.33);
	printf("Third arg:           Got: %f\n", f);
	printf("                Expected: %f\n\n", 4.44);

    }
    else {
	printf("OK\n");

	*(float*)argv[0]     = 11.11;
	*((float*)argv[1]+0) = 12.12;
	*((float*)argv[1]+1) = 13.13;
    }

    return 14.14;
}


double testce_double(int argc, void* argv[]) {

    printf("Starting C part of testce_double ... ");

    double f;
    memcpy(&f, argv+2, sizeof(double));

    if (   argc != 2
	|| ABS( *(double*)argv[0] - 1.11 ) > EPSILON
	|| ABS( *((double*)argv[1]+0) - 2.22 ) > EPSILON
	|| ABS( *((double*)argv[1]+1) - 3.33 ) > EPSILON
    ) {

	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n\n", 2);
	printf("First arg:           Got: %f\n", *(double*)argv[0]);
	printf("                Expected: %f\n\n", 1.11);
	printf("Second arg:          Got: %f %f\n", *((double*)argv[1]+0), *((double*)argv[1]+1));
	printf("                Expected: %f %f\n\n", 2.22, 3.33);

    }
    else {
	printf("OK\n");

	*(double*)argv[0]     = 11.11;
	*((double*)argv[1]+0) = 12.12;
	*((double*)argv[1]+1) = 13.13;
    }

    return 14.14;
}

unsigned short testce_uint(int argc, void* argv[]) {

    printf("Starting C part of testce_uint ... ");

    if (   argc != 3
	|| *(unsigned short*)argv[0] != 1
	|| *((unsigned short*)argv[1]+0) != 2
	|| *((unsigned short*)argv[1]+1) != 3
//??	|| *(unsigned short*)argv[2] != 4
    ) {

	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n\n", 3);
	printf("First arg:           Got: %d\n", *(unsigned short*)argv[0]);
	printf("                Expected: %d\n\n", 1);
	printf("Second arg:          Got: %d %d\n", *((unsigned short*)argv[1]+0), *((unsigned short*)argv[1]+1));
	printf("                Expected: %d %d\n\n", 2, 3);
//??	printf("Third arg:           Got: %d\n", *(unsigned short*)argv[2]);
//??	printf("                Expected: %d\n\n", 4);

    }
    else {
	printf("OK\n");

	*(unsigned short*)argv[0]     = 65535;
	*((unsigned short*)argv[1]+0) = 12;
	*((unsigned short*)argv[1]+1) = 13;
    }

    return 14;
}

unsigned int testce_ulong(int argc, void* argv[]) {

    printf("Starting C part of testce_long ... ");

    if (   argc != 3
	|| *(unsigned int*)argv[0] != 1
	|| *((unsigned int*)argv[1]+0) != 2
	|| *((unsigned int*)argv[1]+1) != 3
//??	|| *(unsigned int*)argv[2] != 4
    ) {

	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n\n", 3);
	printf("First arg:           Got: %d\n", *(unsigned int*)argv[0]);
	printf("                Expected: %d\n\n", 1);
	printf("Second arg:          Got: %d %d\n", *((unsigned int*)argv[1]+0), *((unsigned int*)argv[1]+1));
	printf("                Expected: %d %d\n\n", 2, 3);
//??	printf("Third arg:           Got: %d\n", *(unsigned int*)argv[2]);
//??	printf("                Expected: %d\n\n", 4);

    }
    else {
	printf("OK\n");

	*(unsigned int*)argv[0]     = 4294967295U;
	*((unsigned int*)argv[1]+0) = 12;
	*((unsigned int*)argv[1]+1) = 13;
    }

    return 14;
}

long long testce_long64(int argc, void* argv[]) {

    printf("Starting C part of testce_long64 ... ");

    if (   argc != 2
	|| *(long long*)argv[0] != 1
	|| *((long long*)argv[1]+0) != 2
	|| *((long long*)argv[1]+1) != 3
    ) {

	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n\n", 2);
	printf("First arg:           Got: %lld\n", *(long long*)argv[0]);
	printf("                Expected: %d\n\n", 1);
	printf("Second arg:          Got: %lld %lld\n", *((long long*)argv[1]+0), *((long long*)argv[1]+1));
	printf("                Expected: %d %d\n\n", 2, 3);

    }
    else {
	printf("OK\n");

	*(long long*)argv[0]     = -11;
	*((long long*)argv[1]+0) = 12;
	*((long long*)argv[1]+1) = 13;
    }

    return 14;
}

unsigned long long testce_ulong64(int argc, void* argv[]) {

    printf("Starting C part of testce_ulong64 ... ");

    if (   argc != 2
	|| *(unsigned long long*)argv[0] != 1
	|| *((unsigned long long*)argv[1]+0) != 2
	|| *((unsigned long long*)argv[1]+1) != 3
    ) {

	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n\n", 3);
	printf("First arg:           Got: %llu\n", *(unsigned long long*)argv[0]);
	printf("                Expected: %d\n\n", 1);
	printf("Second arg:          Got: %llu %llu\n", *((unsigned long long*)argv[1]+0), *((unsigned long long*)argv[1]+1));
	printf("                Expected: %d %d\n\n", 2, 3);

    }
    else {
	printf("OK\n");

	*(unsigned long long*)argv[0]     = 18446744073709551615ULL;
	*((unsigned long long*)argv[1]+0) = 12;
	*((unsigned long long*)argv[1]+1) = 13;
    }

    return 14;
}

char* testce_string(int argc, void* argv[]) {

    printf("Starting C part of testce_string ... ");

    STRING* s1 = (STRING*)argv[0];
    STRING* s2 = (STRING*)argv[1];

    if (   argc != 3
	|| strncmp(s1->s, "One  ", s1->slen)
	|| strncmp((s2+0)->s, "Two  ", (s2+0)->slen)
	|| strncmp((s2+1)->s, "Three", (s2+1)->slen)
//??	|| strcmp(((STRING*)argv[2])->s, "Four ")
    ) {

	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n\n", 3);
	printf("First arg:           Got: '%s'\n", s1->s);
	printf("                Expected: '%s'\n\n", "One  ");
	printf("Second arg           Got: '%s' '%s'\n", (s2+0)->s, (s2+1)->s);
	printf("                Expected: '%s' '%s'\n\n", "Two  ", "Three");
//??	printf("Third arg:           Got: '%s'\n", argv[2]);
//??	printf("                Expected: '%s'\n\n", "Four ");

    }
    else {
	printf("OK\n");

	strcpy(s1->s, "  enO");
	strcpy((s2+0)->s, "  owT");
	strcpy((s2+1)->s, "eerhT");
    }

    return (char*)"Fourteen";
}


int testce_struct(int argc, void* argv[]) {

    printf("Starting C part of testce_struct ... ");

    struct inner {
	char      c;
	long long d;
	STRING    s[2];
    };

    struct outer {
	int	     l1;
	struct inner si;
	int          l2;
	char	     c;
    };

    struct outer* so = (struct outer*)argv[0];

    

    if (   argc != 1
	|| so->l1 != 1
	|| so->l2 != 2
	|| so->c  != 2
	|| so->si.c != 1
	|| so->si.d != 2
	|| strcmp(S(so->si.s),   "One") != 0
	|| strcmp(S(so->si.s+1), "Two") != 0
	|| (so+1)->l1 != 3
	|| (so+1)->l2 != 4
	|| (so+1)->c  != 4
	|| (so+1)->si.c != 3
	|| (so+1)->si.d != 4
	|| strcmp(S((so+1)->si.s),   "Thr") != 0
	|| strcmp(S((so+1)->si.s+1), "Fou") != 0
    ) {
	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n", 1);
	printf("First Element:       Got: %d / %d %lld ", 
	       so->l1, so->si.c, so->si.d );
	printf("'%s' ", so->si.s[0].s);
	printf("'%s' / ", so->si.s[1].s);
	printf("%d %d\n", so->l2, so->c);
	printf("                Expected: %d / %d %d '%s' '%s' / %d %d\n", 
	       1, 1, 2, "One", "Two", 2, 2);
	printf("Second Element       Got: %d / %d %lld ", 
	       (so+1)->l1, (so+1)->si.c, (so+1)->si.d );
	printf("'%s' ", (so+1)->si.s[0].s);
	printf("'%s' / ", (so+1)->si.s[1].s);
	printf("%d %d\n", (so+1)->l2, (so+1)->c);
	printf("                Expected: %d / %d %d '%s' '%s' / %d %d\n", 
	       3, 3, 4, "Thr", "Fou", 4, 4);

    }
    else {
	printf("OK\n");
	so->l1 = 11;
	strcpy(S((so+1)->si.s+1), "Fiv");
    }

    return 0;
}

float testce_complex(int argc, void* argv[]) {

    printf("Starting C part of testce_complex ... ");

    IDL_COMPLEX* c1 = (IDL_COMPLEX*)argv[0];
    IDL_COMPLEX* c2 = (IDL_COMPLEX*)argv[1];

    if (   argc != 2
	|| ABS( c1->r - 1.1) > EPSILON
	|| ABS( c1->i - 2.2) > EPSILON
	|| ABS( (c2+0)->r - 3.3) > EPSILON
	|| ABS( (c2+0)->i - 4.4) > EPSILON
	|| ABS( (c2+1)->r - 5.5) > EPSILON
	|| ABS( (c2+1)->i - 6.6) > EPSILON
    ) {

	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n\n", 2);
	printf("First arg:           Got: (%f,%f)\n", c1->r, c1->i);
	printf("                Expected: (%f,%f)\n\n", 1.1, 2.2);
	printf("Second arg:          Got: (%f,%f) (%f,%f)\n",
		(c2+0)->r, (c2+0)->i, 
		(c2+1)->r, (c2+1)->i); 
	printf("                Expected: (%f,%f) (%f,%f)\n\n",
		3.3, 4.4, 5.5, 6.6);
    }
    else {
	printf("OK\n");

	c1->i     = 22.22;
	(c2+1)->r = 55.55;
    }

    return 14.14;

}


double testce_dcomplex(int argc, void* argv[]) {

    printf("Starting C part of testce_complex ... ");

    IDL_DCOMPLEX* c1 = (IDL_DCOMPLEX*)argv[0];
    IDL_DCOMPLEX* c2 = (IDL_DCOMPLEX*)argv[1];

    if (   argc != 2
	|| ABS( c1->r - 1.1) > DEPSILON
	|| ABS( c1->i - 2.2) > DEPSILON
	|| ABS( (c2+0)->r - 3.3) > DEPSILON
	|| ABS( (c2+0)->i - 4.4) > DEPSILON
	|| ABS( (c2+1)->r - 5.5) > DEPSILON
	|| ABS( (c2+1)->i - 6.6) > DEPSILON
    ) {

	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n\n", 2);
	printf("First arg:           Got: (%lf,%lf)\n", c1->r, c1->i);
	printf("                Expected: (%lf,%lf)\n\n", 1.1, 2.2);
	printf("Second arg:          Got: (%lf,%lf) (%lf,%lf)\n",
		(c2+0)->r, (c2+0)->i, 
		(c2+1)->r, (c2+1)->i); 
	printf("                Expected: (%lf,%lf) (%lf,%lf)\n\n",
		3.3, 4.4, 5.5, 6.6);

    }
    else {
	printf("OK\n");

	c1->i     = 22.22;
	(c2+1)->r = 55.55;
    }

    return 14.14;
}


double testce_64(int argc, void* argv[]) {

    printf("Starting C part of testce_64 ... ");

    double      d;
    IDL_COMPLEX c;

    memcpy(&d, argv+0, sizeof(double));
    memcpy(&c, argv+1, sizeof(IDL_COMPLEX));

    if (   argc != 2
	|| ABS( d   - 3.3) > EPSILON
	|| ABS( c.r - 1.1) > EPSILON
	|| ABS( c.i - 2.2) > EPSILON
    ) {

	printf("Error:\n");
	printf("number of arguments: Got: %d\n", argc);
	printf("                Expected: %d\n\n", 2);
	printf("First arg:           Got: %f\n", d);
	printf("                Expected: %f\n\n", 3.3);
	printf("Seconf arg           Got: (%lf,%lf)\n",  c.r, c.i);
	printf("                Expected: (%lf,%lf)\n\n", 1.1, 2.2);
	return -1.0;

    }
    else {
	printf("OK\n");
	return 15.15;
    }

}

int doNothing(int argc, void* argv[]) {

    printf("doNothing called\n");
    return 0;
}

} // extern "C"
