#define DEFINE_CONTAINER_TAGS(XXX)          \
  unsigned GDLContainerVersionTag = XXX->Desc()->TagIndex( "IDLCONTAINERVERSION"); \
  unsigned PHEAD = XXX->Desc()->TagIndex( "PHEAD"); \
  unsigned PTAIL = XXX->Desc()->TagIndex( "PTAIL"); \
  unsigned NLIST = XXX->Desc()->TagIndex( "NLIST");

#define DEFINE_LIST_TAGS(XXX)          \
  unsigned GDLContainerVersionTag = XXX->Desc()->TagIndex( "IDLCONTAINERVERSION"); \
  unsigned PHEAD = XXX->Desc()->TagIndex( "PHEAD"); \
  unsigned PTAIL = XXX->Desc()->TagIndex( "PTAIL"); \
  unsigned NLIST = XXX->Desc()->TagIndex( "NLIST");

//#define GET_CONTAINER_VERSION(XXX) XXX->GetTag( XXX->Desc()->TagIndex( "IDLCONTAINERVERSION"), 0)
//#define GET_CONTAINER_PHEAD(XXX) XXX->GetTag( XXX->Desc()->TagIndex( "PHEAD"), 0)
//#define GET_CONTAINER_PTAIL(XXX) XXX->GetTag( XXX->Desc()->TagIndex( "PTAIL"), 0)
//#define GET_CONTAINER_NLIST(XXX) XXX->GetTag( XXX->Desc()->TagIndex( "NLIST"), 0)
#define CONTAINER_VERSION(XXX) ((*static_cast<DIntGDL*> (XXX->GetTag(XXX->Desc()->TagIndex( "IDLCONTAINERVERSION"), 0)))[0])
#define DPtrHEAD(XXX) ((*static_cast<DPtrGDL*> (XXX->GetTag(XXX->Desc()->TagIndex( "PHEAD"), 0)))[0])
#define DPtrTAIL(XXX) ((*static_cast<DPtrGDL*> (XXX->GetTag(XXX->Desc()->TagIndex( "PTAIL"), 0)))[0])
#define DPtrGDLHEAD(XXX) static_cast<DPtrGDL*>(XXX->GetTag(XXX->Desc()->TagIndex( "PHEAD"), 0))
#define DPtrGDLTAIL(XXX) static_cast<DPtrGDL*>(XXX->GetTag(XXX->Desc()->TagIndex( "PTAIL"), 0))
#define NLIST(XXX) ((*static_cast<DLongGDL*> (XXX->GetTag(XXX->Desc()->TagIndex( "NLIST"), 0)))[0])
//'Fast' version if DEFINE_LIST_TAGS(XXX) or DEFINE_CONTAINER_TAGS(XXX) has been called 
#define FAST_DPtrGDLHEAD(XXX) static_cast<DPtrGDL*>(XXX->GetTag(PHEAD, 0))
#define FAST_DPtrGDLTAIL(XXX) static_cast<DPtrGDL*>(XXX->GetTag(PTAIL, 0))
#define FAST_DPtrHEAD(XXX) ((*static_cast<DPtrGDL*> (XXX->GetTag(PHEAD, 0)))[0])
#define FAST_DPtrTAIL(XXX) ((*static_cast<DPtrGDL*> (XXX->GetTag(PTAIL, 0)))[0])
#define FAST_NLIST(XXX) ((*static_cast<DLongGDL*> (XXX->GetTag(NLIST, 0)))[0])

//#define DEFINE_CONTAINER_NODE_TAGS(XXX)            \
//    static unsigned pNextTag = XXX->Desc()->TagIndex( "PNEXT");  \
//    static unsigned pDataTag = XXX->Desc()->TagIndex( "PDATA");
#define PNEXT 0
#define PDATA 1
#define DPtrNEXT(XXX) ((*static_cast<DPtrGDL*>(XXX->GetTag(PNEXT, 0)))[0])
#define DPtrDATA(XXX) ((*static_cast<DPtrGDL*>(XXX->GetTag(PDATA, 0)))[0])
#define DPtrGDLNEXT(XXX) static_cast<DPtrGDL*>(XXX->GetTag(PNEXT, 0))
#define BaseGDL_DATA(XXX) XXX->GetTag(PDATA, 0) //Already BaseGDL

//#define GET_LIST_VERSION(XXX) XXX->GetTag( XXX->Desc()->TagIndex( "IDLCONTAINERVERSION"), 0)
//#define GET_LIST_PHEAD(XXX) XXX->GetTag( XXX->Desc()->TagIndex( "PHEAD"), 0)
//#define GET_LIST_PTAIL(XXX) XXX->GetTag( XXX->Desc()->TagIndex( "PTAIL"), 0)
//#define GET_LIST_NLIST(XXX) XXX->GetTag( XXX->Desc()->TagIndex( "NLIST"), 0)

#define DEFINE_HASH_TAGS(XXX)           \
     unsigned TableBitsTag = XXX->Desc()->TagIndex( "TABLE_BITS");   \
     unsigned pTableTag = XXX->Desc()->TagIndex( "TABLE_DATA");  \
     unsigned TableSizeTag = XXX->Desc()->TagIndex( "TABLE_SIZE");   \
     unsigned TableCountTag = XXX->Desc()->TagIndex( "TABLE_COUNT");

#define DPtrTABLE_DATA(XXX) ((*static_cast<DPtrGDL*>(XXX->GetTag( XXX->Desc()->TagIndex( "TABLE_DATA"), 0)))[0])
#define TABLE_BITS(XXX) ((*static_cast<DLongGDL*>(XXX->GetTag( XXX->Desc()->TagIndex( "TABLE_BITS"), 0)))[0])
#define TABLE_SIZE(XXX) ((*static_cast<DLongGDL*>(XXX->GetTag( XXX->Desc()->TagIndex( "TABLE_SIZE"), 0)))[0])
#define TABLE_COUNT(XXX) ((*static_cast<DLongGDL*>(XXX->GetTag( XXX->Desc()->TagIndex( "TABLE_COUNT"), 0)))[0])
#define HASH_PTR(XXX) (*static_cast<DPtrGDL*>(XXX->GetTag(XXX->Desc()->TagIndex( "TABLE_DATA"), 0)))[0]

//#define DEFINE_HASHTABLEENTRY_TAGS(XXX)            \
//    static unsigned pKeyTag = XXX->Desc()->TagIndex( "PKEY"); \
//    static unsigned pValueTag = XXX->Desc()->TagIndex( "PVALUE");
#define PKEY 0
#define PVALUE 1
#define DPtrKEY(XXX,YYY) ((*static_cast<DPtrGDL*>(XXX->GetTag( PKEY, YYY)))[0])
#define DPtrVALUE(XXX,YYY) ((*static_cast<DPtrGDL*>(XXX->GetTag( PVALUE, YYY)))[0])

#define MAKE_LONGGDL(X, XLong) \
        DLongGDL* XLong=0; \
        Guard<DLongGDL> XLongGuard; \
        if(X != 0) { \
          if(X->Type() == GDL_LONG) \
            XLong = static_cast<DLongGDL*>( X); \
          else { \
            try { \
              XLong = static_cast<DLongGDL*>( X->Convert2( GDL_LONG, BaseGDL::COPY)); \
            } \
            catch( GDLException& ex) { \
              ThrowFromInternalUDSub( e, ex.ANTLRException::getMessage()); \
            } \
            XLongGuard.Init( XLong); \
          } \
    }
