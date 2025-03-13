#ifndef TOKEN_H
#define TOKEN_H

#include <any>
#include <cstddef>
#include <format>
#include <iostream>
#include <string>

#include "token_type.h"

class Token
{
public:
	Token(TokenType type, size_t line, std::string lexeme = "", std::any literal = std::any());
	[[nodiscard]] std::string to_string() const;

	friend std::ostream& operator<<(std::ostream& output_s, const Token& token);

private:
	size_t m_line;
	std::string m_lexeme;
	std::any m_literal;
	TokenType m_type;
	CLASS_PADDING(4);
};

template <>
struct std::formatter<Token> : std::formatter<std::string> {
	auto format(const Token& token, format_context& ctx) const
	{
		return std::formatter<std::string>::format(token.to_string(), ctx);
	}
};

#endif // TOKEN_H
