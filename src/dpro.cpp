/***************************************************************************
                          dpro.cpp  -  GDL procedures/functions
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// DPro/DFun classes

#include "includefirst.hpp"

#include "dpro.hpp"
#include "dvar.hpp"
#include "str.hpp"
#include "objects.hpp"

#include "GDLTreeParser.hpp"
#include "GDLParser.hpp" // SA: GDLParser::CompileOpt for isObsolete()/isHidden()

// print out AST tree
//#define GDL_DEBUG
//#undef GDL_DEBUG

#ifdef GDL_DEBUG
#include "print_tree.hpp"
#endif

SCCodeListT     sccList;

using namespace std;

  bool is_nonleaf(RefDNode node) {
	bool rslt = (node->getFirstChild() != NULL);
	return rslt;
  }
  void process_top(RefDNode top, SCCStructV &nodes, SCCodeAddresses &addrList);
  
  //write a "Node" : current memory address (serves as identifier), type, text, cdata, down( aka GetFirstChild) , right (aka getNextSibling)

  void convertNodeToStruct(RefDNode node, SCCStructV &nodes, SCCodeAddresses &addrList) {
    sccstruct savenode;
    SCCodeAddressesIterator it;
	// address is not that simple to retrieve. The address is what ' std::cout<<this " prints, and it is NOT easy to get it right. 'This is the way'.
	DNode* ast=node.get();
	if (ast->GetVar() != NULL) std::cerr<<"Var = "<<ast->GetVar()<<std::endl; //TBD: check if var is used.
	RefDNode dNode = static_cast<RefDNode> (node);
	//write node index
    it = addrList.find(ast);
	if (it != addrList.end()) savenode.node = (*it).second; else savenode.node =0;
	RefDNode right = dNode->GetNextSibling();
	ast = right.get();
    it = addrList.find(ast);
	if (it != addrList.end()) savenode.right = (*it).second; else savenode.right =0;
	RefDNode down = dNode->GetFirstChild();
	ast = down.get();
    it = addrList.find(ast);
	if (it != addrList.end()) savenode.down = (*it).second; else savenode.down =0;
	savenode.nodeType=dNode->getType();
	savenode.ligne = dNode->getLine();
	savenode.flags=dNode->GetCompileOpt();
	// trick to avoid saving a Text and a CData containing the same text, either as a string or a value converted to the good BaseGDL type.
	// if dNode->CData() is non-nil, set savenode.Text to "" and save CData as copy.
	if (dNode->CData() != NULL) {
	  savenode.var=dNode->CData()->Convert2(dNode->CData()->Type(),BaseGDL::COPY);
	  savenode.Text = "";
	} else  savenode.Text = dNode->getText(); 
    nodes.push_back(savenode);
  } 

  void process_leaves(RefDNode top, SCCStructV &nodes, SCCodeAddresses &addrList) {
	RefDNode t;

	for (t = ((top && is_nonleaf(top)) ? top->getFirstChild() : (RefDNode) NULL); t; t = t->getNextSibling()) {
	  if (is_nonleaf(t))
		process_top(t,nodes,addrList);
	  else
		convertNodeToStruct(t,nodes,addrList);
	}
  } 

  void process_top(RefDNode top, SCCStructV &nodes, SCCodeAddresses &addrList) {
	RefDNode t;
	bool first = true;

	convertNodeToStruct(top,nodes,addrList);

	if (is_nonleaf(top)) {
	  for (t = ((top && is_nonleaf(top)) ? top->getFirstChild() : (RefDNode) NULL); t; t = t->getNextSibling()) {
		if (is_nonleaf(t))
		  first = false;
	  }
	  process_leaves(top,nodes,addrList);
	}
  }
 //same as index_tree: no not use
  void process_tree(RefDNode top, SCCStructV &nodes, SCCodeAddresses &addrList) {
  std::cerr<<"use of process_tree, please report."<<std::endl;
  assert(false);
  RefDNode t;
	for (t = top; t != NULL; t = t->getNextSibling()) {
	  process_top(t,nodes,addrList);
	}
}
 
//walks and associate an integer index with node address.  
void index_top(RefDNode top, SCCodeAddresses &addrList, int &i);
void indexNodeAddress(RefDNode node, SCCodeAddresses &addrList, int &i) {
// associate node address to an increasing number starting at 1
  DNode* ast = node.get();
  addrList.insert(std::pair<DNode*,int>(ast, ++i)); //+1 as address 0 is special for all nodes
  }

void index_leaves(RefDNode top, SCCodeAddresses &addrList, int &i) {
  RefDNode t;
  for (t = ((top && is_nonleaf(top)) ? top->getFirstChild() : (RefDNode) NULL); t; t = t->getNextSibling()) {
	if (is_nonleaf(t))
	  index_top(t,addrList, i);
	else
	  indexNodeAddress(t,addrList, i);
  }
}

void index_top(RefDNode top, SCCodeAddresses &addrList, int &i) {
  RefDNode t;
  bool first = true;
  indexNodeAddress(top,addrList, i);
  if (is_nonleaf(top)) {
	for (t = ((top && is_nonleaf(top)) ? top->getFirstChild() : (RefDNode) NULL); t; t = t->getNextSibling()) {
	  if (is_nonleaf(t))
		first = false;
	}
	index_leaves(top,addrList, i);
  }
}

//this would be to index a full tree, but our use at the time is of index_top, that serialize only the current PRO or FUN .
void index_tree(RefDNode top, SCCodeAddresses &addrList, int &i) {
  std::cerr<<"use of index_tree, please report."<<std::endl;
  assert(false);
  RefDNode t;
  for (t = top; t != NULL; t = t->getNextSibling()) {
	index_top(t, addrList, i);
  }
}
// vtable
DSub::~DSub() {}


// DLib ******************************************************
DLib::DLib( const string& n, const string& o, const int nPar_, 
	    const string keyNames[],
	    const string warnKeyNames[], const int nParMin_, const bool use_threadpool, void* _mediator, bool usesKeywords)
  : DSub(n,o)
  , hideHelp( false)
  , mediator( _mediator)
{
  nPar=nPar_;
  nParMin = nParMin_;

  if (mediator == NULL) {
    // find out number of keywords and set them
  SizeT nKey_=0;
  if( keyNames != NULL)
    {
      while( keyNames[nKey_] != "") ++nKey_;
    }

    key.resize(nKey_);
  SizeT k=0;
  for( ; k<nKey_; ++k) key[k]=keyNames[k];

  if( nKey_ >= 1) {
    if( keyNames[0] == "_EXTRA")
      {
        extra_type = EXTRA;
        extraIx = 0;
      }
    else if( keyNames[0] == "_REF_EXTRA")
      {
        extra_type = REFEXTRA;
        extraIx = 0;
      }
    }

  SizeT nWarnKey_=0;
  if( warnKeyNames != NULL)
    {
      while( warnKeyNames[nWarnKey_] != "") ++nWarnKey_;
    }

    warnKey.resize(nWarnKey_);
  SizeT wk=0;
  for( ; wk<nWarnKey_; ++wk) warnKey[wk]=warnKeyNames[wk];
    //finally add threadpool kw if any, in warnkeys at the moment, since we do not really honor those kws.
    if (use_threadpool) {
      nWarnKey_ += 3;
      warnKey.resize(nWarnKey_);
      warnKey[wk++] = "TPOOL_MAX_ELTS";
      warnKey[wk++] = "TPOOL_MIN_ELTS";
      warnKey[wk++] = "TPOOL_NOTHREAD";
    }
  } else if (usesKeywords) { //DLM: allow anything, just define _EXTRA at this stage
    key.resize(1);
    key[0]="_REF_EXTRA";
    extra_type = REFEXTRA;
    extraIx = 0; // initialise the possibility of KEYWORDS.
  }
}

const string DLibPro::ToString()
{
  string s;
  if( object != "") s += object+"::";
  s += name;
  if( nPar == -1)
    s += ",[inf. Args]";
  else if( nPar == 1)
    s += ",["+i2s( nPar)+" Arg]";
  else if( nPar > 1)
    s += ",["+i2s( nPar)+" Args]";
  for( SizeT i=0; i<key.size(); ++i)
    s += ","+key[ i];//+"=val";
  return s;
}

const string DPro::ToString()
{
  string s;
  if( object != "") s += object+"::";
  s += name;
  if( nPar == -1)
    s += ",[inf. Args]";
  else if( nPar == 1)
    s += ",["+i2s( nPar)+" Arg]";
  else if( nPar > 1)
    s += ",["+i2s( nPar)+" Args]";
  for( SizeT i=0; i<key.size(); ++i)
    s += ","+key[ i];//+"=val";
  return s;
}

const string DLibFun::ToString()
{
  string s = "res=";
  if( object != "") s += object+"::";
  s += name+"(";
  if( nPar == -1)
    s += "[inf. Args]";
  else if( nPar == 1)
    s += "["+i2s( nPar)+" Arg]";
  else if( nPar > 1)
    s += "["+i2s( nPar)+" Args]";
  if( key.size() > 0)
    {
      if( nPar != 0) s += ",";
      for( SizeT i=0; i<key.size(); ++i)
	{
	  s += key[ i];//+"=val";
	  if( i+1 != key.size()) s += ",";
	}
    }
  s += ")";
  return s;
}

const string DFun::ToString()
{
  string s = "res=";
  if( object != "") s += object+"::";
  s += name+"(";
  if( nPar == -1)
    s += "[inf. Args]";
  else if( nPar == 1)
    s += "["+i2s( nPar)+" Arg]";
  else if( nPar > 1)
    s += "["+i2s( nPar)+" Args]";
  if( key.size() > 0)
    {
      if( nPar != 0) s += ",";
      for( SizeT i=0; i<key.size(); ++i)
	{
	  s += key[ i];//+"=val";
	  if( i+1 != key.size()) s += ",";
	}
    }
  s += ")";
  return s;
}


// const string DLibFunDirect::ToString()
// {
//   string s = "res=";
//   s += name+"(";
//   s += "["+i2s( nPar)+" Arg]";
//   s += ")";
//   return s;
// }

DLibPro::DLibPro( LibPro p, const string& n, const string& o, const int nPar_, 
		  const string keyNames[], const string warnKeyNames[], const int nParMin_)
  : DLib(n,o,nPar_,keyNames, warnKeyNames, nParMin_), pro(p)
{
  libProList.push_back(this);
//  sort(libProList.begin(), libProList.end(),CompLibFunName());
}
DLibPro::DLibPro( LibPro p, const string& n, const int nPar_, 
		  const string keyNames[], const string warnKeyNames[], const int nParMin_, const bool use_threadpool)
  : DLib(n,"",nPar_,keyNames, warnKeyNames, nParMin_, use_threadpool), pro(p)
{
  libProList.push_back(this);
//  sort(libProList.begin(), libProList.end(),CompLibFunName());
}

DLibPro::DLibPro( LibPro p, void* mediator, const string& n, const int nPar_, const int nParMin_, const bool hasKeys)
  : DLib(n,"",nPar_,NULL, NULL, nParMin_, false, mediator, hasKeys), pro(p)
{
  libProList.push_back(this);
//  sort(libProList.begin(), libProList.end(),CompLibFunName());
}

DLibFun::DLibFun( LibFun f, const string& n, const string& o, const int nPar_, 
		  const string keyNames[], const string warnKeyNames[], const int nParMin_)
  : DLib(n,o,nPar_,keyNames, warnKeyNames, nParMin_), fun(f)
{
  libFunList.push_back(this);
//  sort(libFunList.begin(), libFunList.end(),CompLibFunName());
}

DLibFun::DLibFun( LibFun f, const string& n, const int nPar_, 
		  const string keyNames[], const string warnKeyNames[], const int nParMin_, const bool use_threadpool)
  : DLib(n,"",nPar_,keyNames, warnKeyNames, nParMin_, use_threadpool), fun(f)
{
  libFunList.push_back(this);
//  sort(libFunList.begin(), libFunList.end(),CompLibFunName());
}

DLibFun::DLibFun( LibFun f, void* mediator, const string& n, const int nPar_, const int nParMin_, const bool hasKeys)
  : DLib(n,"",nPar_,NULL, NULL, nParMin_, false, mediator,hasKeys), fun(f)
{
  libFunList.push_back(this);
//  sort(libFunList.begin(), libFunList.end(),CompLibFunName());
}

DLibFunRetNew::DLibFunRetNew( LibFun f, const string& n, 
			      const string& o, const int nPar_, 
			      const string keyNames[], const string warnKeyNames[], const int nParMin_)
  : DLibFun(f,n,o,nPar_,keyNames, warnKeyNames, nParMin_)
{}

DLibFunRetNew::DLibFunRetNew( LibFun f, const string& n, const int nPar_, 
			      const string keyNames[], const string warnKeyNames[], bool rConstant,
			      const int nParMin_)
  : DLibFun(f,n,nPar_,keyNames, warnKeyNames, nParMin_), retConstant( rConstant)
{}

DLibFunRetNewTP::DLibFunRetNewTP( LibFun f, const string& n, const int nPar_, 
			      const string keyNames[], const string warnKeyNames[], bool rConstant,
			      const int nParMin_)
  : DLibFun(f,n,nPar_,keyNames, warnKeyNames, nParMin_, true), retConstant( rConstant)
{}
// DLibFunRetNew::DLibFunRetNew( LibFun f, const string& n, const int nPar_, 
// 			bool rConstant)
//   : DLibFun(f,n,nPar_,NULL, NULL), retConstant( rConstant)
// {}
// DLibFunRetNew::DLibFunRetNew( LibFun f, const string& n, bool rConstant)
//   : DLibFun(f,n), retConstant( rConstant)
// {}
DLibFunDirect::DLibFunDirect( LibFunDirect f, const std::string& n, bool rConstant)
  : DLibFunRetNew(NULL,n,1,NULL,NULL,rConstant,1), funDirect(f)
{}

DLibFunDirectTP::DLibFunDirectTP( LibFunDirect f, const std::string& n, bool rConstant)
  : DLibFunRetNewTP(NULL,n,1,NULL,NULL,rConstant,1), funDirect(f)
{}

// DSubUD ****************************************************
DSubUD::~DSubUD()
{
  // delete only common references (common blocks only if owner)
  CommonBaseListT::iterator it;
  for( it=common.begin(); it !=common.end(); ++it)
    {
      DCommonRef* cRef=dynamic_cast<DCommonRef*>(*it);
      delete cRef; // also ok if cRef is NULL
    }

  labelList.Clear();
  delete tree;
  sccList.erase(this);
}

DSubUD::DSubUD(const string& n,const string& o,const string& f) : 
  DSub(n,o), file(f),
  tree( NULL),
  compileOpt(GDLParser::NONE),
  labelList(),
  nForLoops( 0)
{
  if( o != "")
    AddPar( "SELF");
}

// for .RNEW and $MAIN$
// clears all COMMON blocks, VARIABLES, JUMPLABELS and the CODE
// name, keywords etc. are kept
void DSubUD::Reset()
{
  var.clear();

  // delete only common references (common blocks only if owner)
  CommonBaseListT::iterator it;
  for( it=common.begin(); it !=common.end(); ++it)
    {
      DCommonRef* cRef=dynamic_cast<DCommonRef*>(*it);
      delete cRef; // also ok if cRef is NULL
    }
  common.clear();

  DelTree();
}

void DSubUD::DelTree()
{
  labelList.Clear(); // labels are invalid after tree is deleted
  delete tree;
  tree = NULL; //static_cast<RefDNode>(antlr::nullAST);
  nForLoops = 0;
}

DSubUD* DSubUD::AddPar(const string& p)
{
  var.push_back(p);
  nPar++;
  return this;
}

unsigned DSubUD::AddVar(const string& v)
{
  var.push_back(v);
  return var.size()-1;
}

DSubUD* DSubUD::AddKey(const string& k, const string& v)
{
  if( k == "_REF_EXTRA")
    {
      if( extra_type == EXTRA)
	throw GDLException("Routines cannot be declared with both"
			   " _EXTRA and _REF_EXTRA.");
      extra_type=REFEXTRA;
      extraIx=0;
    }
  else if( k == "_EXTRA")
    {
      if( extra_type == REFEXTRA)
	throw GDLException("Routines cannot be declared with both"
			   " _EXTRA and _REF_EXTRA.");
      extra_type=EXTRA;
      extraIx=0;
    }
  else
    {
      if(extraIx != -1) extraIx++; // update extra ix index
    }
  // as we only push_front during compilation, we better use a vector here
  //   key.push_front(k);
  //   var.push_back(v);
  // we want push_front
  key.resize( key.size() + 1);
  for( int i= key.size()-1; i>0; --i)
    key[ i] = key[ i-1];
  key[ 0] = k;
  var.resize( var.size() + 1);
  for( int i= var.size()-1; i>0; --i)
    var[ i] = var[ i-1];
  var[ 0] = v;
  return this;
}

// must be done after the tree is converted
void DSubUD::ResolveAllLabels()
{
  ResolveLabel( tree);
}

void DSubUD::ResolveLabel( ProgNodeP p)
{
  if( p == NULL) return;

  // if( p->getNextSibling() != NULL)
  // 	std::cout << "Resolve("<< p->getLine()<<"): " << p << " keepRight: " << p->KeepRight()<< ": "<< p->getText() <<"  r: "<< p->GetNextSibling()->getText() << std::endl;
  // else
  // 	std::cout << "Resolve("<< p->getLine()<<"): " << p << " keepRight: " << p->KeepRight()<< ": "<< p->getText() <<"  r: NULL"<< std::endl;
  
  if( p->getType() == GDLTreeParser::ON_IOERROR || 
      p->getType() == GDLTreeParser::GOTO)
    {
      int ix = labelList.Find( p->getText());
      if( ix == -1)
	throw GDLException( p, ObjectName()+": Undefined label "+p->getText()+
			    " referenced in GOTO statement.",false,false);
      
      p->SetGotoIx( ix);
    }
  else if( p->getType() == GDLTreeParser::LABEL)
    {
      labelList.SetLabelNode( p);
    }
  
  if( !p->KeepDown())
    ResolveLabel( p->getFirstChild());
  if( !p->KeepRight())
    ResolveLabel( p->getNextSibling());
  //   else
  // 	ResolveLabel( p->getNextSibling());
}

// **************************************
// the final "compilation" takes part here 
// **************************************
void DSubUD::SetTree( RefDNode n)
{
  //
  // Converter Translation Transcription Rewrite RefDNode ProgNode ProgNodeP
  // ***************************************************
  // here the conversion RefDNode -> ProgNode is done
  // ***************************************************
  tree = ProgNode::NewProgNode( n);

  //   if( tree != NULL)
  nForLoops = ProgNode::NumberForLoops( tree);
  //   else
  // 	nForLoops = 0;
  
#ifdef GDL_DEBUG
  cout << "DSubUD::SetTree:" << endl;
  antlr::print_tree pt;
  pt.pr_tree(tree);
  cout << "DSubUD::SetTree end." << endl;
#endif

}
//converts a SemiCompiledCode (chained list of DNodes) to a 'flat' vector of sccstruct and insert the vector in the map pointed by "sccList"

void DSubUD::SetSCC(RefDNode n) {
  //first, walk RefDnode tree to associate right,down, and self addresses with a number in the list.
  SCCodeAddresses addrList;
  int i=0;
  //serialize only the current PRO or FUN by using index_top and *not* index_tree.
  index_top(n, addrList, i);
  SCCStructV sccv;
  process_top(n, sccv, addrList);
	//PRO or FUNCTION top node may contain a 'wrong' right pointer in the first node, due to the way the whole .pro file is compiled at once.
	//Remove it:
  sccv[0].right=0;
  sccList.insert(std::pair<DSubUD*,SCCStructV >(this, sccv));
}
bool DSubUD::GetCommonVarName(const BaseGDL* p, std::string& varName)
{
  CommonBaseListT::iterator it;
  for( it=common.begin(); it != common.end(); it++)
    {
      int vIx = (*it)->Find( p);
      if( vIx >= 0) 
	{
	  varName=(*it)->VarName( vIx);
	  return true;
	}
    }
  return false;
}

BaseGDL* DSubUD::GetCommonVarNameList()
{
  DStringGDL* res = new DStringGDL(dimension(this->CommonsSize()), BaseGDL::NOZERO);
  CommonBaseListT::iterator it;
  SizeT iname=0;
  for( it=common.begin(); it != common.end(); it++)
    {
    SizeT nVar=(*it)->NVar();
    for (SizeT i=0; i< nVar ; ++i) (*res)[iname++]=(*it)->VarName(i);
    }
  return res;
}

bool DSubUD::GetCommonVarName4Help(const BaseGDL* p, std::string& varName)
{
  CommonBaseListT::iterator it;
  for( it=common.begin(); it != common.end(); it++)
    {
      int vIx = (*it)->Find( p);
      if( vIx >= 0) 
	{
	  varName=(*it)->VarName( vIx) +" ("+(*it)->Name()+')';
	  return true;
	}
    }
  return false;
}


bool DSubUD::isObsolete()
{
  return compileOpt & GDLParser::OBSOLETE;
}

bool DSubUD::isStatic()
{
  return compileOpt & GDLParser::STATIC;
}

bool DSubUD::isHidden()
{
  return compileOpt & GDLParser::HIDDEN;
}

bool DSubUD::isGdlHidden()
{
  return compileOpt & GDLParser::GDL_HIDDEN;
}

bool DSubUD::isNoSave()
{
  return compileOpt & GDLParser::NOSAVE;
}

void DSubUD::AddHiddenToCompileOpt() {
  compileOpt |= GDLParser::GDL_HIDDEN;
}

