bool amIRelaxed=true;
//int inloop=0;

#include "gdlLexer.h"
#include "gdlParser.h"

using namespace antlr4;
using namespace std;

// test program will be faster on a list of files as every previoulsy compiled file contributes to the warmup,
// see https://github.com/antlr/antlr4/issues/3111 discussion
// by splitting input files in smaller procedures/functions one would have it even better.
// this can be done also at the parser level, by presenting all tokens between PRO|FUNCTION and END (provided
// the lexer isolates the procedure END from the other possible END tokens inside the procedure).
// see https://groups.google.com/g/antlr-discussion/c/q-8MPVI9lrw for possible ameliorations to this test
// program using the last version of the cpp runtime.

int main(int argc, char *argv[]){
    bool showtree=false;
    bool showtokens=false;
    bool token_only=false;
    bool interactive=false;
    std::vector<string> filenames;
    for( auto a=1; a< argc; ++a)
    {
      if( string( argv[a]) == "--help" || string( argv[a]) == "-h") {
      cerr << "Usage: cat xxx.pro | demo [-options]" << endl;
      cerr << "options:" << endl;
      cerr << "  --help (-h)        display this message" << endl;
      cerr << "  --tree (-t)    show tree" << endl;
      cerr << "  --tokens (-o)    show tokens" << endl;
      cerr << "  --strict (-s)    set strictarray" << endl;
      cerr << "  --interactive (-i)  will parse as in interactive mode" << endl;
      return 0;
      }
      else if (string(argv[a])=="--tree" || string(argv[a])=="-t" || string(argv[a])=="-T")
	{
	  showtree=true;
	}
      else if (string(argv[a])=="--tokens" || string(argv[a])=="-o" || string(argv[a])=="-O")
	{
	  showtokens=true;
	} 
      else if (string(argv[a])=="--token-only" || string(argv[a])=="-x" || string(argv[a])=="-X")
	{
	  token_only=true;
	}
      else if (string(argv[a])=="--strict" || string(argv[a])=="-s" || string(argv[a])=="-S")
	{
	  amIRelaxed=false;
	}
      else if (string(argv[a])=="--interactive" || string(argv[a])=="-i" || string(argv[a])=="-I")
	{
	  interactive=true;
	}
       else filenames.push_back(string(argv[a]));
   }
  
 if (interactive) {
  ANTLRInputStream input(std::cin); // read stdin
  gdlLexer lexer(&input);

  CommonTokenStream tokens(&lexer);
  gdlParser parser(&tokens);

  tree::ParseTree *tree;
  if (interactive)  tree = parser.interactive();
  if (showtokens) {
    for (auto token : tokens.getTokens()) std::cout << token->toString() << std::endl;
  }
  if (showtree) std::cout << tree->toStringTree(&parser) << std::endl << std::endl;
 } else {

for (auto i = 0; i< filenames.size(); ++i) {
  std::filebuf fb;
  if (!fb.open (filenames[i],std::ios::in)) continue;
  std::istream is(&fb);
  ANTLRInputStream input(is);
  gdlLexer lexer(&input);

  CommonTokenStream tokens(&lexer);
  gdlParser parser(&tokens);

  tree::ParseTree *tree;
  auto start = std::chrono::steady_clock::now();
  tree = parser.translation_unit();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start);
  if (showtokens) {
    for (auto token : tokens.getTokens()) std::cout << token->toString() << std::endl;
  }
  if (showtree) std::cout << tree->toStringTree(&parser) << std::endl << std::endl;
  std::cout  << filenames[i]<<"  "<< duration.count()/1000000. << std::endl;

  }
 }
  return 0;
}
