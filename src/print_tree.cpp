/***************************************************************************
                          print_tree.cpp  -  description
   Print an ANTLR abstract syntax tree in operator prefix form.
   Used for debugging only.
   Modified from antlr package
                             -------------------
    begin                : ?
    copyright            : Software rights: http://www.antlr.org/license.html
    email                : ?
 ***************************************************************************/

#include "includefirst.hpp"

#include <cassert>
#include <cstdio>
#include "print_tree.hpp"

#include "dnode.hpp"

namespace antlr {

/*
 * pr_name
 *

   Print the character string associated with an ANTLR tree node.

 */
void print_tree::pr_name( RefAST node )
{
    std::string str;

    RefDNode dNode = static_cast<RefDNode>( node);

//    str =  dNode->getText();
    str = std::string("[") + i2s(dNode->getType()) + ":" + i2s(dNode->CData()) + "]" + node->getText();
//     printf("%s ", str.c_str());
    printf("%s(%d) ", str.c_str(), dNode->getLine());
} // pr_name
void print_tree::pr_name( ProgNodeP node )
{
    std::string str;

    str = node->getText();
    printf("%s(%d) ", str.c_str(), node->getLine());
} // pr_name


/*
 * pr_indent
 *

   Print indentation for a node.

 */
void print_tree::pr_indent()
{
  const SizeT BUFSIZE = 80;
  char buf[ BUFSIZE+1 ];
  unsigned i;

  for (i = 0; i < indent_level && i < BUFSIZE; i++) {
    buf[i] = ' ';
  }
  buf[i] = '\0';
  printf("%s", buf );
} // pr_indent



void print_tree::pr_open_angle()
{
  if ( indent_level )
    printf("\n");

  pr_indent();

  printf("<");
  indent_level += INDENT;
}  // pr_open_angle



/*
 * pr_close_angle
 *

   Print the ">" bracket to show the close of a tree production.

 */
void print_tree::pr_close_angle(bool first)
{
  assert( indent_level > 0 );

  indent_level -= INDENT;

  if (!first) {
    printf("\n");
    pr_indent();
  }
  printf(">");
} // pr_close_angle


/*
 * pr_leaves
 *

   Print the leaves of an AST node

 */
void print_tree::pr_leaves( RefAST top )
{
  RefAST t;
  
  for( t = ( (top && is_nonleaf(top)) ? top->getFirstChild() : (RefAST)NULL );
       t; t = t->getNextSibling() )
    {
      if (is_nonleaf( t ))
	pr_top( t );
      else
	pr_name( t );
    }
} // pr_leaves
void print_tree::pr_leaves( ProgNodeP top )
{
  ProgNodeP t;
  
  for( t = ( (top && is_nonleaf(top)) ? top->getFirstChild() : NULL );
       t; t = t->getNextSibling() )
    {
      if (is_nonleaf( t ))
	pr_top( t );
      else
	pr_name( t );
      if( t->getNextSibling() != NULL && t->KeepRight())
      {
	printf("^");
	break;
      }
    }
} // pr_leaves



/*
 * pr_top
 *

   Recursively print a tree (or a sub-tree) from the top down.

 */
void print_tree::pr_top( RefAST top )
{
  RefAST t;
  bool first = true;
  
  pr_open_angle();
  
  pr_name( top );
  
  if (is_nonleaf( top )) {
    for( t = ( (top && is_nonleaf(top)) ? top->getFirstChild() : (RefAST)NULL );
	 t; t = t->getNextSibling() )
      {    
	if (is_nonleaf( t ))
	  first = false;
      }
    pr_leaves( top );
  }
  
  pr_close_angle( first );
} // pr_top
void print_tree::pr_top( ProgNodeP top )
{
  ProgNodeP t;
  bool first = true;
  
  pr_open_angle();
  
  pr_name( top );
  
  if (is_nonleaf( top )) {
    for( t = ( (top && is_nonleaf(top)) ? top->getFirstChild() : NULL );
	 t; t = t->getNextSibling() )
      {    
	if (is_nonleaf( t ))
	  first = false;
	if( t->getNextSibling() != NULL && t->KeepRight())
	{
	  printf("^");
	  break;
	}
      }
    pr_leaves( top );
  }
  
  pr_close_angle( first );
} // pr_top



/*
 * pr_tree
 *

   Main entry point for tree print.

 */
void print_tree::pr_tree( RefAST top )
{
  RefAST t;

  for (t = top; t != NULL; t = t->getNextSibling()) {
    indent_level = 0;
    pr_top( t );
    printf("\n");
  }
} // pr_tree
void print_tree::pr_tree( ProgNodeP top )
{
  ProgNodeP t;

  for (t = top; t != NULL; t = t->getNextSibling()) {
    indent_level = 0;
    pr_top( t );
    printf("\n");
    if( t->getNextSibling() != NULL && t->KeepRight())
    {
      printf("^");
      break;
    }
  }
} // pr_tree

} // namespace
