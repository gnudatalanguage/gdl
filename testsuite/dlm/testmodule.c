#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "idl_export.h"
// TEST1: HELLO WORLD
void handsig(int signo){
  printf("signal trapped\n");
}
void printvar(IDL_VPTR v); 
void printArrayDescr(IDL_ARRAY *arr){
			printf("Array[");
			int i;
			for (i=0; i< arr->n_dim-1; ++i) {
				printf ("%d,",arr->dim[i]);
			}
			printf("%d ]", arr->dim[i]);
}
void printStruct(IDL_SREF s){
  printArrayDescr(s.arr);
  if (s.sdef->id != NULL) printf("ident: %s ,", s.sdef->id->name);
  printf("ntags %d\n", s.sdef->ntags);
  for (int i=0; i< 1; ++i) { printf ("TAG %s ",s.sdef->tags[i].id->name); printvar( &(s.sdef->tags[i].var) ); printf("\n");}
}
// print a full var
void printvar(IDL_VPTR v) {
  if (v->flags & IDL_V_STRUCT) {printf("STRUCTURE:\n"); printStruct(v->value.s);}
  else if (v->flags & IDL_V_ARR) printArrayDescr(v->value.arr);
  printf("\t%s\n",IDL_VarName(v));
}
  
IDL_MEMINT data_offset(struct _idl_structure *vv, IDL_MEMINT i){
  fprintf(stderr,"tag %d, offset=%d\n",i, vv->tags[i].offset);
  return vv->tags[i].offset;
  /* if (vv->tags[i].var.flags & IDL_V_ARR) return vv->tags[i].var.value.arr->arr_len; */
  /* else return IDL_TypeSize[vv->tags[i].var.type]; */
}
			  
IDL_VPTR hello_world_not_same_name_to_test_IDL_Load(int argc, IDL_VPTR argv[])
{
  /* char* toto="zekjfezkfjezlfkjze"; */
  /* IDL_STRING s_src,s_dst; */
  /* IDL_StrStore(&s_src,toto); */
  /* s_dst = s_src; /\* Copy the descriptor *\/ */
  /* IDL_StrDup(&s_dst, 1L); /\* Duplicate the string *\/ */
  /* printf("type=%d, after: %d , slen=%d after: %d\n",s_src.stype,s_dst.stype,s_src.slen,s_dst.slen); */
  /* IDL_StrDelete(&s_src, 1); */
  /* printf("toto=%s\n",toto); */
  /* printf("type=%d, after: %d , slen=%d after: %d\n",s_src.stype,s_dst.stype,s_src.slen,s_dst.slen); */
  /* IDL_StrDelete(&s_dst, 1); */
  /* printf("type=%d, after: %d , slen=%d after: %d\n",s_src.stype,s_dst.stype,s_src.slen,s_dst.slen); */


  if (argc > 0) {
    //    printf("using IDL_VarGetString: %s\n", IDL_VarGetString(argv[0]));
    IDL_VPTR v=argv[0];
    
//    struct  _idl_structure* s=v->value.s.sdef;
//   fprintf(stderr,"named structure length: %d, data_length: %d, ntags:%d, tag_array_mem: %d\n data_length:%d,mod 8: %d, length:%d,mod 8: %d\n",s->data_length, s->length, s->ntags, s->tag_array_mem,s->data_length, s->data_length%8, s->length, s->length%8);
//   for (int i=0; i<s->ntags; ++i)
//     fprintf(stderr,"tagdef name: %s, tag type: %d, tagdef offset:%d (%d)\n",s->tags[i].id->name, s->tags[i].var.type, s->tags[i].offset, s->tags[i].offset%8);
// //  IDL_STRUCTURE *subv=(IDL_STRUCTURE*)s->tags[4].var.value.s.sdef;
//   for (int i=0; i<subv->ntags; ++i)
//     fprintf(stderr,"SUBTAGDEF name: %s, offset:%d (%d) \n",subv->tags[i].id->name, subv->tags[i].offset, subv->tags[i].offset%8 );
//  struct  _idl_structure* vv=v->value.s.sdef;
//  // for (int i=5; i<vv->ntags; ++i) fprintf(stderr,"tag %d : %d\n",i,vv->tags[i].var.value.s.sdef->ntags);
//  IDL_LONG64 l=data_offset(vv,0);
//  IDL_STRING* sss=(IDL_STRING*)(v->value.arr->data+l);fprintf(stderr,"at addr #%lld :\n",sss);
//  fprintf(stderr,"%s\n",sss->s);
//  l=data_offset(vv,1);
//  IDL_LONG* toto3=(IDL_LONG*)(v->value.arr->data+l); fprintf(stderr,"at addr #%lld :\n",toto3); for (int i=0; i<24;++i) fprintf(stderr,"%d,",toto3[i]);fprintf(stderr,"\n");
//  l=data_offset(vv,2);
//  float * fp=(float*)(v->value.arr->data+l);fprintf(stderr,"at addr #%lld :\n",fp);
//  fprintf(stderr,"%f",fp[0]); fprintf(stderr,"\n");
//  l=data_offset(vv,3);
//  float* toto4=(float*)(v->value.arr->data+l);fprintf(stderr,"at addr #%lld :\n",toto4);  for (int i=0; i<48;++i) fprintf(stderr,"%f, ",toto4[i]);fprintf(stderr,"\n");
//  l=data_offset(vv,4);
//  struct  _idl_structure* vvv=v->value.s.sdef->tags[4].var.value.s.sdef;
//  IDL_LONG64 l2=data_offset(vvv,2);
//  float* toto5=(float*)(v->value.arr->data+l+l2); fprintf(stderr,"at addr #%lld :\n",toto5); for (int i=0; i<24;++i) fprintf(stderr,"%f, ",toto5[i]);fprintf(stderr,"\n");
//  l=data_offset(vv,5);
// IDL_STRING* s4=(IDL_STRING*)(v->value.arr->data+l);fprintf(stderr,"at addr #%lld :\n",s4);
// for (int i=0; i<10;++i) fprintf(stderr,"%s, ",s4[i].s);fprintf(stderr,"\n");
//  IDL_VPTR zz=IDL_Gettmp();
//  // will throw:  IDL_StructTagInfoByName(s, "TOTO", IDL_MSG_LONGJMP, &zz);
//  // will throw:   IDL_StructTagInfoByIndex(s,100, IDL_MSG_LONGJMP, &zz);
//  char * structname;
//  fprintf(stderr,"%s : of struct ",IDL_StructTagNameByIndex(s,1, IDL_MSG_LONGJMP, &structname));
//  fprintf(stderr,"%s\n",structname);
// 
//  fprintf(stderr,"arr_len: %d, elt_len: %d, n_elts: %d", v->value.s.arr->arr_len,v->value.s.arr->elt_len,v->value.s.arr->n_elts);
    return argv[0];
  }
  /* for (int i=1; i<32; ++i) { */
  /* IDL_SignalRegister(i, handsig, 0); */
  /* IDL_SignalUnregister(i, handsig, 0); */
  /* } */
return(IDL_StrToSTRING("Hello World!"));
}
 IDL_VPTR IDL_rsum1(int argc, IDL_VPTR argv[])
{
IDL_VPTR v;
IDL_VPTR r;
float *f_src;
float *f_dst;
IDL_MEMINT n;


 v = argv[0];
 if (v->type != IDL_TYP_FLOAT)
    IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP,
    "argument must be float");
 IDL_ENSURE_ARRAY(v);
 IDL_EXCLUDE_FILE(v);

 f_dst = (float *)
    IDL_VarMakeTempFromTemplate(v, IDL_TYP_FLOAT,
       (IDL_StructDefPtr) 0, &r, FALSE);
 f_src = (float *) v->value.arr->data;
 n = v->value.arr->n_elts - 1;
 *f_dst++ = *f_src++;/* First element */
 for (; n--; f_dst++) *f_dst = *(f_dst - 1) + *f_src++;

 return r;
 }
  IDL_VPTR IDL_rsum2(int argc, IDL_VPTR argv[])
{
IDL_VPTR v;
IDL_VPTR r;
float *f_src;
float *f_dst;
IDL_MEMINT n;


 v = IDL_BasicTypeConversion(1, argv, IDL_TYP_FLOAT);
 IDL_VarGetData(v, &n, (char **) &f_src, FALSE);

 /* Get a result var, reusing the input if possible */
 if (v->flags & IDL_V_TEMP) {
 r = v;
 f_dst = f_src;
*f_dst++ = *f_src++;/* First element */
 n--;
 for (; n--; f_dst++) *f_dst = *(f_dst - 1) + *f_src++;

 return r;
 } else {
IDL_VPTR z;
 f_dst = (float *) IDL_VarMakeTempFromTemplate(v, IDL_TYP_FLOAT, (IDL_StructDefPtr) 0, &z, FALSE);
 for (int i=0; i<10; ++i) fprintf(stderr,"avant: %f\n",f_dst[i]);
 for (int i=0; i<10; ++i) fprintf(stderr,"z: %f\n",((float*)(z->value.arr->data))[i]);
 *f_dst++ = *f_src++;/* First element */
 n--;
 for (; n--; f_dst++) *f_dst = *(f_dst - 1) + *f_src++;
 for (int i=0; i<10; ++i) fprintf(stderr,"apres: %f\n",f_dst[i]);
 return z;
 }

 
 }

  IDL_VPTR IDL_rsum3(int argc, IDL_VPTR argv[])
{
IDL_VPTR v, r;
union {
char *sc; /* Standard char */
UCHAR *c; /* IDL_TYP_BYTE */
IDL_INT *i; /* IDL_TYP_INT */
IDL_UINT *ui; /* IDL_TYP_UINT */
IDL_LONG *l; /* IDL_TYP_LONG */
 IDL_ULONG *ul; /* IDL_TYP_ULONG */
 IDL_LONG64 *l64; /* IDL_TYP_LONG64 */
 IDL_ULONG64 *ul64; /* IDL_TYP_ULONG64 */
 float *f; /* IDL_TYP_FLOAT */
 double *d; /* IDL_TYP_DOUBLE */
 IDL_COMPLEX *cmp; /* IDL_TYP_COMPLEX */
 IDL_DCOMPLEX *dcmp; /* IDL_TYP_DCOMPLEX */
 } src, dst;
 IDL_LONG64 n;


 v = argv[0];
 if (v->type == IDL_TYP_STRING)
 v = IDL_BasicTypeConversion(1, argv, IDL_TYP_FLOAT);
 IDL_VarGetData(v, &n, &(src.sc), TRUE);
 n--; /* First is a special case */

 /* Get a result var, reusing the input if possible */
 if (v->flags & IDL_V_TEMP) {
 r = v;
 dst = src;
 } else {
 dst.sc = IDL_VarMakeTempFromTemplate(v, v->type,
    (IDL_StructDefPtr) 0, &r, FALSE);
 }

 #define DOCASE(type, field) \
 case type: for (*dst.field++ = *src.field++; n--;\
 dst.field++)\
 *dst.field = *(dst.field - 1) + *src.field++; break
 #define DOCASE_CMP(type, field) case type: \
 for (*dst.field++ = *src.field++; n--; \
 dst.field++, src.field++) { \
 dst.field->r = (dst.field - 1)->r + src.field->r; \
 dst.field->i = (dst.field - 1)->i + src.field->i; } \
 break

 switch (v->type) {
 DOCASE(IDL_TYP_BYTE, c);
 DOCASE(IDL_TYP_INT, i);
 DOCASE(IDL_TYP_LONG, l);
 DOCASE(IDL_TYP_FLOAT, f);
 DOCASE(IDL_TYP_DOUBLE, d);
 DOCASE_CMP(IDL_TYP_COMPLEX, cmp);
 DOCASE_CMP(IDL_TYP_DCOMPLEX, dcmp);
 DOCASE(IDL_TYP_UINT, ui);
 DOCASE(IDL_TYP_ULONG, ul);
 DOCASE(IDL_TYP_LONG64, l64);
 DOCASE(IDL_TYP_ULONG64, ul64);
 default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP,
    "unexpected type");
 }
 #undef DOCASE
 #undef DOCASE_CMP

 return r;
 }
 IDL_VPTR mult2(int argc, IDL_VPTR argv[])
{
IDL_VPTR dst, src;
float *src_d, *dst_d;
int n;
src = dst = argv[0];
IDL_ENSURE_SIMPLE(src);
IDL_ENSURE_ARRAY(src);
  if (src->type != IDL_TYP_FLOAT)  src = dst = IDL_CvtFlt(1, argv);
  printf("n_elts: %d\n", src->value.arr->n_elts);
  src_d = dst_d = (float *) src->value.arr->data;
  if (!(src->flags & IDL_V_TEMP))
     dst_d = (float *) IDL_MakeTempArray(IDL_TYP_FLOAT,src->value.arr->n_dim,src->value.arr->dim, IDL_ARR_INI_NOP, &dst);
   for (n = src->value.arr->n_elts; n--; )      *dst_d++ = 2.0 * *src_d++;
  return(dst);
}
void free_cb(UCHAR *data){
  fprintf(stderr,"Call Back activated 0x%d\n",data);
  free(data);
}
//this should not call free_cb as the data is passed to the return value
IDL_VPTR function_returning_importarray(int argc, IDL_VPTR *argv, char *argk){
 double *arr=(double*) malloc(17*sizeof(double));
 IDL_MEMINT dim[1]={17};
 for (int i=0; i<17; ++i) arr[i]=log10(i+1);
 IDL_VPTR v=IDL_ImportArray(1, dim, IDL_TYP_DOUBLE, (UCHAR *)arr, free_cb, NULL);
 IDL_VPTR save=IDL_Gettmp();
  IDL_VarCopy(v,save);
   IDL_Deltmp(save);
   IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO,IDL_VarName(v));
 IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO,
	     "The program should say\"FUNCTION_RETURNING_IMPORTARRAY: Variable is undefined: <UNDEFINED>.\"");
 IDL_Print(1,&v,NULL);
 return v;
}
// this should call free_cb
void procedure_using_importarray(int argc, IDL_VPTR *argv, char *argk){
 double *arr=(double*) malloc(32*sizeof(double));
 IDL_MEMINT dim[1]={32};
 for (int i=0; i<32; ++i) arr[i]=log10(i+1);
 IDL_VPTR v=IDL_ImportArray(1, dim, IDL_TYP_DOUBLE, (UCHAR *)arr, free_cb, NULL);
 IDL_VPTR save=IDL_Gettmp();
 IDL_VarCopy(v,save);
 IDL_Deltmp(save);
 IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO,
	     "The program should say\"PROCEDURE_USING_IMPORTARRAY: Variable is undefined: <UNDEFINED>.\"");
 IDL_Print(1,&v,NULL);
 
}
//this should call free_cb only when "IMPORTED_ARRAY" is changed in $MAIN$
void test_importnamedarray(int argc, IDL_VPTR *argv, char *argk){
  double *arr=(double*) malloc(11*17*sizeof(double));
  IDL_MEMINT dim[2]={11,17};
 for (int i=0; i<11*17; ++i) arr[i]=log10(i+1);
 IDL_VPTR v=IDL_ImportNamedArray("IMPORTED_ARRAY",2, dim, IDL_TYP_DOUBLE, (UCHAR *)arr, free_cb, NULL);
 IDL_VPTR id=IDL_GettmpFILEINT(-1);
 IDL_VPTR z[2]; z[0]=id; z[1]=v;
 for (int i=0; i<16; ++i) printf("%d,",IDL_OutputFormatLen[i]);
 // IDL_PrintF(2,z,NULL);
 //IDL_ExecuteStr("plot,imported_array");
}
 void keyword_demo(int argc, IDL_VPTR *argv, char *argk)
 {
 
 typedef struct {
 IDL_KW_RESULT_FIRST_FIELD; /* Must be first entry in structure */
 IDL_LONG l;
 float f;
 double d;
 int d_there;
 IDL_STRING s;
 int s_there;
 IDL_LONG arr_data[10];
 int arr_there;
 IDL_MEMINT arr_n;
 IDL_VPTR var;
 IDL_VPTR out;
 } KW_RESULT;
 static IDL_KW_ARR_DESC_R arr_d = { IDL_KW_OFFSETOF(arr_data),
    3, 10,IDL_KW_OFFSETOF(arr_n) };

 static IDL_KW_PAR kw_pars[] = {
   IDL_KW_FAST_SCAN, /* ignored by GDL but needed for comatibility */
 /* IDL needs them sorted by alphabetic name. GDL does not. */
 /* keyword,    type,          mask, flags                       , specified (adddress or off), value */  
 { "ARRAY",     IDL_TYP_LONG,     1, IDL_KW_ARRAY                , IDL_KW_OFFSETOF(arr_there), IDL_CHARA(arr_d)   },
 { "DOUBLE",    IDL_TYP_DOUBLE,   1,                            0, IDL_KW_OFFSETOF(d_there)  , IDL_KW_OFFSETOF(d) },
 { "FLOAT",     IDL_TYP_FLOAT,    1,                  /*IDL_KW_ZERO*/0,                          0, IDL_KW_OFFSETOF(f) },
 { "LONG",      IDL_TYP_LONG,     1,  IDL_KW_ZERO|IDL_KW_VALUE|15,                          0, IDL_KW_OFFSETOF(l) },
 { "READWRITE", IDL_TYP_UNDEF,    1,       IDL_KW_OUT|IDL_KW_ZERO,                          0, IDL_KW_OFFSETOF(var) },
 {"OUTPUT_ONLY",IDL_TYP_UNDEF,    1,       IDL_KW_OUT|IDL_KW_ZERO,                          0, IDL_KW_OFFSETOF(out) },
 {"STRING",     IDL_TYP_STRING,   1,                            0,   IDL_KW_OFFSETOF(s_there), IDL_KW_OFFSETOF(s) },
 { NULL }
 };

 KW_RESULT kw;
 int i;
 IDL_ALLTYPES newval;

 (void) IDL_KWProcessByOffset(argc, argv, argk, kw_pars, (IDL_VPTR *) 0, 1, &kw);
 printf("float: %f\n",kw.f);
  printf("FIELD:%d\n\n", kw._idl_kw_free);
   printf("LONG: <%spresent>\n", kw.l ? "": "not ");
 printf("FLOAT: %f\n", kw.f);
 printf("DOUBLE: <%spresent>\n", kw.d_there ? "": "not ");
 if (kw.d_there) printf("double: %lf\n",kw.d);
 printf("STRING: %s\n",
    kw.s_there ? IDL_STRING_STR(&kw.s) : "<not present>");
  printf("ARRAY: ");
 if (kw.arr_there)
 for (i = 0; i < kw.arr_n; i++)
    printf(" %d", kw.arr_data[i]);
 else
 printf("<not present>");
 printf("\n");

 printf("READWRITE: ");
 if (kw.var) {
 IDL_Print(1, &kw.var, (char *) 0);
 newval.l = 42;
 IDL_StoreScalar(kw.var, IDL_TYP_LONG, &newval);
 } else {
 printf("<not present>");
 }
 printf("\n");
 newval.l = 33;
 if (kw.out) IDL_StoreScalar(kw.out, IDL_TYP_LONG, &newval);
  IDL_KW_FREE;
 }


//dealing with structures
  IDL_VPTR givebackexamplestruct(int argc, IDL_VPTR argv[]) {
static IDL_MEMINT one = 1;
static IDL_MEMINT ten = 10;
static IDL_MEMINT dimension3[] = { 3, 2, 3, 4};
static IDL_MEMINT dimension1[] = { 1, 10 };
IDL_STRING zzzz={21,0,"012345678901234567890"};
IDL_STRING zzzz2={10,0,"0123456789"};
static IDL_STRUCT_TAG_DEF substructure_tags[] = {
{ "SUB1", 0, (void *) IDL_TYP_BYTE},
{ "SUB2", 0, (void *) IDL_TYP_LONG},
{ "SUB3", dimension3, (void *) IDL_TYP_FLOAT},
{ "SUB4", dimension1, (void *) IDL_TYP_STRING},
{ 0 }
};
//choice between named structure or not
 
IDL_STRUCTURE *substruct= IDL_MakeStruct(NULL, substructure_tags); 
//IDL_STRUCTURE *n= IDL_MakeStruct("NAMED_STRUCTURE", substructure_tags);
typedef struct data_substructure {
  UCHAR tag1_data;
  IDL_LONG tag1bis_data;
  float tag2_data [24];
  IDL_STRING tag3_data [10];
} DATA_SUBSTRUCTURE;
static DATA_SUBSTRUCTURE theDataOfSubstructure;
 theDataOfSubstructure.tag1_data=33;
 theDataOfSubstructure.tag1bis_data=66;
  for (int i=0; i<24; ++i) theDataOfSubstructure.tag2_data[i]=i;
static DATA_SUBSTRUCTURE theDataOfSubstructure2;
 theDataOfSubstructure2.tag1_data=77;
 theDataOfSubstructure2.tag1bis_data=4096675;
 for (int i=0; i<24; ++i) theDataOfSubstructure2.tag2_data[i]=sqrt(i);
 theDataOfSubstructure.tag3_data[3]=zzzz;
//  IDL_VPTR SubstructureVPTR= IDL_ImportArray(1, &one, IDL_TYP_STRUCT,   (UCHAR *) &theDataOfSubstructure, 0, substruct);
//printvar(SubstructureVPTR);
 static IDL_MEMINT zero[2]={1,10};
 
static IDL_STRUCT_TAG_DEF structure_tags[] = {
  /* 2 */ { "TAG1", 0, (void *) IDL_TYP_DOUBLE},
  /* 0 */ { "TAG2", 0, (void *) IDL_TYP_STRING},
  /* 1 */ { "TAG3", dimension3, (void *) IDL_TYP_INT},
  /* 3 */ { "TAG4", dimension3, (void *) IDL_TYP_COMPLEX},
  /* 4 */ { "TAG5", 0, NULL },
  /* 5 */ { "TAG6", dimension1, (void *) IDL_TYP_STRING},
//{ "NAMED_STRUCTURE",0 ,0 },
{ 0 }
};
//pass the substructure type here
 structure_tags[4].type=substruct;
 IDL_STRUCTURE *s= IDL_MakeStruct("DUPONT", structure_tags); //named structure 

typedef struct data_struct2 {
  double tag1_data;
  IDL_STRING stri ;
  IDL_INT tag2_data [24];
  IDL_COMPLEX tag2_data_cpx [24];
  DATA_SUBSTRUCTURE stru;
  IDL_STRING tag_3_data [10];
} DATA_STRUCT2;
 static DATA_STRUCT2 structureData[10];
IDL_VPTR v;
 char*ss="ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ";
 structureData[0].stri.s=ss;
 structureData[0].stri.slen=strlen(ss);
 structureData[0].stri.stype=0;
 structureData[0].tag1_data=999.999;
 structureData[0].tag_3_data[3]=zzzz;
 structureData[0].stru=theDataOfSubstructure;
 for (int i=0; i<24; ++i) (structureData[0].tag2_data [i])=i;
 for (int i=0; i<24; ++i) (structureData[0].tag2_data_cpx [i]).r=i;
 char*ss2="we are at element number 7";
 structureData[6].stri.s=ss2;
 structureData[6].stri.slen=strlen(ss2);
 structureData[6].stri.stype=0;
 structureData[6].tag1_data=777.777;
 structureData[6].tag_3_data[0]=zzzz2;
 structureData[6].tag_3_data[5]=zzzz;
 structureData[6].stru=theDataOfSubstructure2;
 for (int i=0; i<24; ++i) (structureData[6].tag2_data [i])=i*7;
 for (int i=0; i<24; ++i) (structureData[6].tag2_data_cpx [i]).r=i*7;

 v = IDL_ImportArray(1, &ten, IDL_TYP_STRUCT,   (UCHAR *) &structureData, 0, s);
 }

void print_name(int argc, IDL_VPTR argv[], char *argk)
{
printf("%s\n", IDL_VarName(argv[0]));
}

 void teststruct(int argc, IDL_VPTR argv[]) {

	typedef struct zzzz {
		UCHAR b[9];
		IDL_LONG c[1];
      		IDL_COMPLEX d [1];
	} DATA_STRUCT1;
	DATA_STRUCT1* s = (DATA_STRUCT1*) (argv[0]->value.s.arr->data);
	printf("expected pad = %d\n",argv[0]->value.s.sdef->length - argv[0]->value.s.sdef->data_length);
	printf("elt_len :%d, arr_len:%d n_elts:%d\n",argv[0]->value.s.arr->elt_len,argv[0]->value.s.arr->arr_len, argv[0]->value.s.arr->n_elts);
	UCHAR* test=(UCHAR*) (argv[0]->value.s.arr->data);
	for  (int i = 0; i < argv[0]->value.s.arr->arr_len; ++i) printf("%d,",test[i]);printf("\n");

	//	for (int j=0; j< 3; ++j) {
	/* for (int i = 0; i < 9; ++i) { */
	/* 	printf(IDL_OutputFormatFunc(argv[0]->value.s.sdef->tags[0].var.type), s->b[i]); */
	/* 	printf(","); */
	/* }; */
	/* printf("\n"); */
	/* for (int i = 0; i < 1; ++i) { */
	/* 	printf(IDL_OutputFormatFunc(argv[0]->value.s.sdef->tags[1].var.type), s->c[i]); */
	/* 	printf(","); */
	/* }; */
	/* printf("\n"); */
	/* for (int i = 0; i < 1; ++i) { */
	/* 	printf(IDL_OutputFormatFunc(argv[0]->value.s.sdef->tags[2].var.type), s->d[i]); */
	/* 	printf(","); */
	/* }; */
	/* printf("\n----------\n"); */
	//}
	
}

IDL_MEMINT dimension3[] = { 3, 2, 3, 4};
IDL_MEMINT dimension1[] = { 1, 10 };

//for acceptance by IDL, GDL does not need it.
int IDL_Load(void) {
 IDL_STRUCT_TAG_DEF NOUVELLESTRUCT_tags[] = {
{ "SUB1", 0, (void *) IDL_TYP_BYTE},
{ "SUB2", 0, (void *) IDL_TYP_LONG},
{ "SUB3", dimension3, (void *) IDL_TYP_FLOAT},
{ "SUB4", dimension1, (void *) IDL_TYP_STRING},
{ 0 }
};

 //IDL_StructDefPtr IDL_StructDefPtr_NOUVELLESTRUCT(){
 //return IDL_MakeStruct("NOUVELLESTRUCT", NOUVELLESTRUCT_tags);
 //}
 static IDL_SYSFUN_DEF2 function_addr[] = {
{(IDL_SYSRTN_GENERIC) hello_world_not_same_name_to_test_IDL_Load,"HELLO_WORLD",0,1,0,0},
{(IDL_SYSRTN_GENERIC) IDL_rsum1,"IDL_RSUM1",1,1,0,0},
{(IDL_SYSRTN_GENERIC) IDL_rsum2,"IDL_RSUM2",1,1,0,0},
{(IDL_SYSRTN_GENERIC) IDL_rsum3,"IDL_RSUM3",1,1,0,0},
{(IDL_SYSRTN_GENERIC) mult2,"MULT2",1,1,0,0},
{(IDL_SYSRTN_GENERIC) givebackexamplestruct,"GIVEBACKEXAMPLESTRUCT",0,0,0,0},
{(IDL_SYSRTN_GENERIC) function_returning_importarray,"FUNCTION_RETURNING_IMPORTARRAY",0,0,0,0},
  };
static IDL_SYSFUN_DEF2 procedure_addr[] = {
{(IDL_SYSRTN_GENERIC) keyword_demo,"KEYWORD_DEMO",0,0,IDL_SYSFUN_DEF_F_KEYWORDS,0},
{(IDL_SYSRTN_GENERIC) print_name,"PRINT_NAME",1,1,0,0},
{(IDL_SYSRTN_GENERIC) teststruct,"TESTSTRUCT",1,1,0,0},
{(IDL_SYSRTN_GENERIC) test_importnamedarray,"TEST_IMPORTNAMEDARRAY",0,0,0,0},
{(IDL_SYSRTN_GENERIC) procedure_using_importarray,"PROCEDURE_USING_IMPORTARRAY",0,0,0,0}
};
 IDL_STRUCTURE *s= IDL_MakeStruct("NOUVELLESTRUCT", NOUVELLESTRUCT_tags); //named structure 
return IDL_SysRtnAdd(function_addr, TRUE,
IDL_CARRAY_ELTS(function_addr))
&& IDL_SysRtnAdd(procedure_addr, FALSE,
IDL_CARRAY_ELTS(procedure_addr));
}

