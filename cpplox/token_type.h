#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include <iostream>

#include "general.h"

enum class TokenType
{
	// Single-character tokens.
	LEFT_PAREN,	 // (
	RIGHT_PAREN, // )
	LEFT_BRACE,	 // {
	RIGHT_BRACE, // }
	COMMA,		 // ,
	DOT,		 // .
	MINUS,		 // -
	PLUS,		 // +
	SEMICOLON,	 // ;
	SLASH,		 // /
	STAR,		 // *

	// One or two character tokens.
	BANG,		   // !
	BANG_EQUAL,	   // !=
	EQUAL,		   // =
	EQUAL_EQUAL,   // ==
	GREATER,	   // >
	GREATER_EQUAL, // >=
	LESS,		   // <
	LESS_EQUAL,	   // <=

	// Literals.
	IDENTIFIER, // like variable names.
	STRING,		// like "hello world".
	NUMBER,		// like 123 or 123.123.

	// Keywords.
	AND,	// and
	CLASS,	// class
	ELSE,	// else
	FALSE,	// false
	FUN,	// fun
	FOR,	// for
	IF,		// if
	NIL,	// nil
	OR,		// or
	PRINT,	// print
	RETURN, // return
	SUPER,	// super
	THIS,	// this
	TRUE,	// true
	VAR,	// var
	WHILE,	// while

	// End of file.
	END_OF_FILE, // EOF
};

inline std::ostream& operator<<(std::ostream& out_s, const TokenType& token_type)
{
	// clang-format off
	ignore_warning_begin(-Wswitch-default);
	// clang-format on
	switch (token_type) {
	case TokenType::LEFT_PAREN: out_s << "LEFT_PAREN"; break;
	case TokenType::RIGHT_PAREN: out_s << "RIGHT_PAREN"; break;
	case TokenType::LEFT_BRACE: out_s << "LEFT_BRACE"; break;
	case TokenType::RIGHT_BRACE: out_s << "RIGHT_BRACE"; break;
	case TokenType::COMMA: out_s << "COMMA"; break;
	case TokenType::DOT: out_s << "DOT"; break;
	case TokenType::MINUS: out_s << "MINUS"; break;
	case TokenType::PLUS: out_s << "PLUS"; break;
	case TokenType::SEMICOLON: out_s << "SEMICOLON"; break;
	case TokenType::SLASH: out_s << "SLASH"; break;
	case TokenType::STAR: out_s << "STAR"; break;
	case TokenType::BANG: out_s << "BANG"; break;
	case TokenType::BANG_EQUAL: out_s << "BANG_EQUAL"; break;
	case TokenType::EQUAL: out_s << "EQUAL"; break;
	case TokenType::EQUAL_EQUAL: out_s << "EQUAL_EQUAL"; break;
	case TokenType::GREATER: out_s << "GREATER"; break;
	case TokenType::GREATER_EQUAL: out_s << "GREATER_EQUAL"; break;
	case TokenType::LESS: out_s << "LESS"; break;
	case TokenType::LESS_EQUAL: out_s << "LESS_EQUAL"; break;
	case TokenType::IDENTIFIER: out_s << "IDENTIFIER"; break;
	case TokenType::STRING: out_s << "STRING"; break;
	case TokenType::NUMBER: out_s << "NUMBER"; break;
	case TokenType::AND: out_s << "AND"; break;
	case TokenType::CLASS: out_s << "CLASS"; break;
	case TokenType::ELSE: out_s << "ELSE"; break;
	case TokenType::FALSE: out_s << "FALSE"; break;
	case TokenType::FUN: out_s << "FUN"; break;
	case TokenType::FOR: out_s << "FOR"; break;
	case TokenType::IF: out_s << "IF"; break;
	case TokenType::NIL: out_s << "NIL"; break;
	case TokenType::OR: out_s << "OR"; break;
	case TokenType::PRINT: out_s << "PRINT"; break;
	case TokenType::RETURN: out_s << "RETURN"; break;
	case TokenType::SUPER: out_s << "SUPER"; break;
	case TokenType::THIS: out_s << "THIS"; break;
	case TokenType::TRUE: out_s << "TRUE"; break;
	case TokenType::VAR: out_s << "VAR"; break;
	case TokenType::WHILE: out_s << "WHILE"; break;
	case TokenType::END_OF_FILE: out_s << "EOF"; break;
	}
	ignore_warning_end();
	return out_s;
}

#endif // TOKEN_TYPE_H
