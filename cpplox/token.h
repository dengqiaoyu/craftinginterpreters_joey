#ifndef TOKEN_H
#define TOKEN_H

#include <any>
#include <cstddef>
#include <iostream>
#include <string>

#include "token_type.h"

class Token
{
public:
	Token(TokenType type, size_t line, std::string lexeme = "", std::any literal = std::any());
	friend std::ostream& operator<<(std::ostream& output_s, const Token& token);

private:
	size_t m_line;
	std::string m_lexeme;
	std::any m_literal;
	TokenType m_type;
	CLASS_PADDING(4);
};

#endif // TOKEN_H
