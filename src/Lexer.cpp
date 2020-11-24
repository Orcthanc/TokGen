#include "Lexer.hpp"

#include <iostream>
#include <iomanip>
#include <ctype.h>

using namespace std;

LEXER_HPP_NAMESPACE::token_id LEXER_HPP_NAMESPACE::Lexer::push_rule( const std::string& name, const std::string& rule ){
	auto temp = name_to_tok.emplace( name, ++curr_id );
	tok_to_name.emplace( curr_id, temp.first->first );

	automata.emplace_back( Automata( rule ), curr_id );

	return curr_id;
}

void LEXER_HPP_NAMESPACE::Lexer::start( const char* input ){
	this->input = input;
	line = 1;
	column = 0;
	pos = 0;
}

LEXER_HPP_NAMESPACE::Lexer& LEXER_HPP_NAMESPACE::Lexer::operator>>( Token& t ){

	t.line = line;
	t.column = column;

	size_t longest = 0;
	token_id toke = -1;
	for( auto& [aut, tok]: automata ){
		aut.reset();

		const char* c = input + pos;
		while( true ){
			//std::cout << c - input << std::endl;
			if( !aut.consume( *c )){
				if( aut.longest_match() > longest ){
					longest = aut.longest_match();
					toke = tok;
				}
				break;
			}

			++c;
		}
		//std::cout << std::endl;

	}


	if( longest == 0 )
		t.id = 0;
	else
		t.id = toke;

	t.match_text = std::string( input + pos, input + pos + longest );

	for( auto it: t.match_text ){
		++t.column;
		if( it == '\n' ){ //TODO \r\n
			t.column = 1;
			++t.line;
		}
	}

	pos += longest;
	return *this;
}

void LEXER_HPP_NAMESPACE::Lexer::debug_dump( std::ostream& o ){
	for( auto& [au, token_id]: automata ){
		o << token_id << ":" << endl;
		au.debug_dump( o );
		o << endl;
	}
}

const std::unordered_map<std::string, LEXER_HPP_NAMESPACE::token_id>& LEXER_HPP_NAMESPACE::Lexer::get_tokens(){
	return name_to_tok;
}


static int any_char( int c ){
	return c;
}

::LEXER_HPP_NAMESPACE::Automata::Automata( const std::string& rule ){
	State state_max = 0;
	char last = 0;
	int(*last_f)(int) = nullptr;
	bool last_was_c = false;
	bool last_was_opt = false;
	for( auto it = rule.begin(); it != rule.end(); ++it ){
		switch( *it ){
			case '\\':
			{
				switch( *++it ){
					case 'w':
					{
						if( last_was_opt ){
							f_transitions[state_max - 1].emplace_back( ::isalpha, state_max + 1 );
						}
						f_transitions[state_max].emplace_back( ::isalpha, state_max + 1 );
						last_f = ::isalpha;
						last_was_c = false;
						last_was_opt = false;
						++state_max;
					} break;
					case 'd':
					{
						if( last_was_opt ){
							f_transitions[state_max - 1].emplace_back( ::isdigit, state_max + 1 );
						}
						f_transitions[state_max].emplace_back( ::isdigit, state_max + 1 );
						last_f = ::isdigit;
						last_was_c = false;
						last_was_opt = false;
						++state_max;
					} break;
					case 's':
					{
						if( last_was_opt ){
							f_transitions[state_max - 1].emplace_back( ::isspace, state_max + 1 );
						}
						f_transitions[state_max].emplace_back( ::isspace, state_max + 1 );
						last_f = ::isspace;
						last_was_c = false;
						last_was_opt = false;
						++state_max;
					} break;
					case '\\':
					case '?':
					case '*':
					case '+':
					case '.':
					{
						if( last_was_opt ){
							c_transitions.emplace( std::make_pair( state_max - 1, *it ), state_max + 1 );
						}
						c_transitions.emplace( std::make_pair( state_max, *it ), state_max + 1 );
						last = *it;
						last_was_c = true;
						last_was_opt = false;
						++state_max;
					} break;
					default:
					{
						std::cout << "Found invalid backslash escape" << std::endl;
					}
				}
			} break;
			case '?':
			{
				last_was_opt = true;
			} break;
			case '*':
			{
				if( last_was_c ){
					c_transitions.emplace( std::make_pair( state_max, last ), state_max );
				} else {
					f_transitions[state_max].emplace_back( last_f, state_max );
				}
				last_was_opt = true;
			} break;
			case '+':
			{
				if( last_was_c ){
					c_transitions.emplace( std::make_pair( state_max, last ), state_max );
				} else {
					f_transitions[state_max].emplace_back( last_f, state_max );
				}
			} break;
			case '.':
			{
				if( last_was_opt ){
					f_transitions[state_max - 1].emplace_back( ::any_char, state_max + 1 );
				}
				f_transitions[state_max].emplace_back( ::any_char, state_max + 1 );
				last_f = ::any_char;
				last_was_c = false;
				last_was_opt = false;
				++state_max;
			} break;
			default:
			{
				if( last_was_opt ){
					c_transitions.emplace( std::make_pair( state_max - 1, *it ), state_max + 1 );
				}
				c_transitions.emplace( std::make_pair( state_max, *it ), state_max + 1 );
				last = *it;
				last_was_c = true;
				last_was_opt = false;
				++state_max;
			} break;
		}
	}

	accepting.resize( state_max + 1 );
	accepting[state_max] = true;
	if( last_was_opt )
		accepting[--state_max] = true;
}

void ::LEXER_HPP_NAMESPACE::Automata::reset(){
	curr_state = 0;
	longest = 0;
	valid = true;
	counter = 0;
}

bool ::LEXER_HPP_NAMESPACE::Automata::consume( char c ){
	//std::cout << " " << c << std::endl;
	//std::cout << curr_state << std::endl;
	if( !valid )
		return false;

	auto temp = c_transitions.find( std::make_pair( curr_state, c ));

	if( temp != c_transitions.end()){
		curr_state = temp->second;
		++counter;

		if( accepting[curr_state] )
			longest = counter;
	} else {
		auto temp2 = f_transitions.find( curr_state );
		if( temp2 == f_transitions.end() ){
			valid = false;
			return false;
		}

		bool found = false;
		for( auto& f: temp2->second ){
			if( f.first( c )){
				if( !found ){
					found = true;
					curr_state = f.second;
					++counter;

					if( accepting[curr_state] )
						longest = counter;
				} else {
					std::cout << "Warning, unsupported nondeterministic regex" << std::endl;
					break;
				}
			}
		}
		if( !found ){
			valid = false;
			return false;
		}
	}

	return true;
}

void ::LEXER_HPP_NAMESPACE::Automata::debug_dump( std::ostream& o ){
	o << "\tTransitions:" << endl;

	for( auto& [trans, state]: c_transitions ){
		o << "\t\t" << trans.first << " --" << trans.second << "-> " << state << endl;
	}

	for( auto& [state, trans]: f_transitions ){
		for( auto& [trans_f, nstate]: trans ){
			o << "\t\t" << state << " --*" << (void*)trans_f << "-> " << nstate << endl;
		}
	}

	o << "\tAccepting states:" << endl;

	for( size_t i = 0; i < accepting.size(); ++i ){
		if( accepting[i] ){
			o << "\t\t" << i << endl;
		}
	}
}

uint32_t LEXER_HPP_NAMESPACE::Automata::longest_match(){
	return longest;
}


std::ostream& ::LEXER_HPP_NAMESPACE::operator<<( std::ostream& o, const Token& t ){
	o << "(" << t.id << ";" << std::quoted( t.match_text ) << ";" << t.line << ";" << t.column << ")";

	return o;
}
