/***************************************************************************
                          print_tree.hpp
   Print an ANTLR abstract syntax tree in operator prefix form.
   Used for debugging only.
                             -------------------
    begin                : ?
    copyright            : ? (modified from ANTLR package)
    email                : ?
 ***************************************************************************/


#ifndef _PRINT_TREE_HPP_
#define _PRINT_TREE_HPP_

#include "typedefs.hpp"

#include "antlr/ASTRefCount.hpp"
#include "antlr/AST.hpp"

namespace antlr {


#define For_each_kid(t,top)     for(t=( (top && is_nonleaf(top)) ? top->getFirstChild() : (RefAST)NULL ); t; t = t->getNextSibling() )


class print_tree {
private:
  typedef enum { INDENT = 2 } bogus;
  unsigned indent_level;

private:
  void pr_name( RefAST node );
  void pr_indent();
  void pr_top( RefAST top );
  void pr_open_angle(void);
  void pr_close_angle(bool first);
  void pr_leaves( RefAST top );

  bool is_nonleaf( RefAST node )
  {
    bool rslt = (node->getFirstChild() != NULL);
    return rslt;
  }
public:
  void pr_tree( const RefAST top );
}; // print_tree

} // namespace
#endif
