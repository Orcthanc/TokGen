#pragma once

#include <string>
#include <ostream>
#include <unordered_map>
#include <vector>
#include <map>

#ifndef LEXER_HPP_NAMESPACE
#define LEXER_HPP_NAMESPACE Lexer
#endif //LEXER_HPP_NAMESPACE

namespace LEXER_HPP_NAMESPACE {
	using token_id = uint32_t;

	struct Automata;

	struct Token {
		token_id id;
		std::string match_text;
		size_t line;
		size_t column;
	};

	struct Lexer {
		public:
			//Currently supports literals, "\w", "\d", "\s", "?", "*", "+"
			token_id push_rule( const std::string& name, const std::string& rule );
			void start( const char* imput );
			void debug_dump( std::ostream& o );

			Lexer& operator>>( Token& t );

			const std::unordered_map<std::string, token_id>& get_tokens();
		private:
			std::unordered_map<std::string, token_id> name_to_tok;
			std::unordered_map<token_id, const std::string&> tok_to_name;

			std::vector<std::pair<Automata, token_id>> automata;
			token_id curr_id = {};
			const char* input;
			size_t pos;
			size_t line;
			size_t column;
	};


	struct Automata {
		public:
			Automata( const std::string& rule );

			void reset();
			//Returns false if Automata errored out, true otherwise
			bool consume( char c );
			void debug_dump( std::ostream& o );
			uint32_t longest_match();

		private:
			using State = uint32_t;

			uint32_t longest;
			uint32_t counter;
			bool valid;
			State curr_state;
			std::map<std::pair<State, char>, State> c_transitions;
			std::map<State, std::vector<std::pair<int(*)(int), State>>> f_transitions;
			std::vector<bool> accepting;
	};

	std::ostream& operator<<( std::ostream&, const Token& t );
}
