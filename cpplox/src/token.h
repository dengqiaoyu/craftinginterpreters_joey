#ifndef TOKEN_H
#define TOKEN_H

#include <cstddef>
#include <format>
#include <iostream>
#include <string>

#include "token_type.h"
#include "value.h"

class Token
{
public:
	Token(TokenType type, std::string lexeme);
	Token(TokenType type, size_t line);
	Token(TokenType type, size_t line, Value literal, std::string lexeme);
	[[nodiscard]] std::string to_string() const;

	friend std::ostream& operator<<(std::ostream& output_s, const Token& token);

	[[nodiscard]] std::string get_lexeme() const;

private:
	size_t m_line;
	std::string m_lexeme;
	Value m_literal;
	TokenType m_type;
	CLASS_PADDING(4);
};

template <>
struct std::formatter<Token> : std::formatter<std::string> { // NOLINT(altera-struct-pack-align)
	auto format(const Token& token, format_context& ctx) const
	{
		return std::formatter<std::string>::format(token.to_string(), ctx);
	}
};

#endif // TOKEN_H
