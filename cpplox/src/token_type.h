#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include <format>
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

inline std::string
to_string(const TokenType type)
{
	// clang-format off
	ignore_warning_begin("-Wswitch-default");
	// clang-format on
	switch (type) {
	case TokenType::LEFT_PAREN: return "LEFT_PAREN";
	case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
	case TokenType::LEFT_BRACE: return "LEFT_BRACE";
	case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
	case TokenType::COMMA: return "COMMA";
	case TokenType::DOT: return "DOT";
	case TokenType::MINUS: return "MINUS";
	case TokenType::PLUS: return "PLUS";
	case TokenType::SEMICOLON: return "SEMICOLON";
	case TokenType::SLASH: return "SLASH";
	case TokenType::STAR: return "STAR";
	case TokenType::BANG: return "BANG";
	case TokenType::BANG_EQUAL: return "BANG_EQUAL";
	case TokenType::EQUAL: return "EQUAL";
	case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
	case TokenType::GREATER: return "GREATER";
	case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
	case TokenType::LESS: return "LESS";
	case TokenType::LESS_EQUAL: return "LESS_EQUAL";
	case TokenType::IDENTIFIER: return "IDENTIFIER";
	case TokenType::STRING: return "STRING";
	case TokenType::NUMBER: return "NUMBER";
	case TokenType::AND: return "AND";
	case TokenType::CLASS: return "CLASS";
	case TokenType::ELSE: return "ELSE";
	case TokenType::FALSE: return "FALSE";
	case TokenType::FUN: return "FUN";
	case TokenType::FOR: return "FOR";
	case TokenType::IF: return "IF";
	case TokenType::NIL: return "NIL";
	case TokenType::OR: return "OR";
	case TokenType::PRINT: return "PRINT";
	case TokenType::RETURN: return "RETURN";
	case TokenType::SUPER: return "SUPER";
	case TokenType::THIS: return "THIS";
	case TokenType::TRUE: return "TRUE";
	case TokenType::VAR: return "VAR";
	case TokenType::WHILE: return "WHILE";
	case TokenType::END_OF_FILE: return "EOF";
	}
	ignore_warning_end();
	return "Unknown TokenType";
}

inline std::ostream&
operator<<(std::ostream& out_s, const TokenType& token_type)
{
	return out_s << to_string(token_type);
}

template <>
struct std::formatter<TokenType> : std::formatter<std::string> {
	auto format(const TokenType& token_type, std::format_context& ctx) const
	{
		return std::formatter<std::string>::format(to_string(token_type), ctx);
	}
};

#endif // TOKEN_TYPE_H
