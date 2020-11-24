#include "Lexer.hpp"

#include <iostream>

using namespace LEXER_HPP_NAMESPACE;

int main( int argc, char** argv ){
	::LEXER_HPP_NAMESPACE::Lexer lex;

	lex.push_rule( "Object", "Object" );
	lex.push_rule( "Camera", "Camera" );
	lex.push_rule( "Identifier", "\\w+" );
	lex.push_rule( "Plus", "\\+" );
	lex.push_rule( "Minus", "-" );
	lex.push_rule( "Times", "\\*" );
	lex.push_rule( "Divide", "/" );
	lex.push_rule( "Modulo", "%" );
	lex.push_rule( "Float", "\\d*\\.\\d+" );
	lex.push_rule( "Int", "\\d" );
	lex.push_rule( "Assignment", "=" );
	lex.push_rule( "CurlyBracketOpen", "{" );
	lex.push_rule( "CurlyBracketClose", "}" );
	lex.push_rule( "ParenthesisOpen", "(" );
	lex.push_rule( "ParenthesisClose", ")" );
	lex.push_rule( "BracketOpen", "[" );
	lex.push_rule( "BracketClose", "]" );
	lex.push_rule( "Comma", "," );
	lex.push_rule( "Dot", "\\." );
	lex.push_rule( "WhiteSpace", "\\s" );
	lex.push_rule( "String", "\".*\"" );

	lex.debug_dump( std::cout );

	lex.start( "123.3e1.2etesttet\"aa/asdf.ponfgjdh.asdf\"" );

	Token t;

	do {
		lex >> t;
		std::cout << t << std::endl;
	} while( t.id != 0 );
}
