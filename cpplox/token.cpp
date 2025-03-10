#include "token.h"

Token::Token(const TokenType type, const size_t line, std::string lexeme, std::any literal)
	: m_line(line), m_lexeme(std::move(lexeme)), m_literal(std::move(literal)), m_type(type)
{
	// Empty constructor.
}

std::ostream& operator<<(std::ostream& output_s, const Token& token)
{
	output_s << "Token{type=" << token.m_type << ", line=" << token.m_line << ", lexeme=" << token.m_lexeme
			 << ", literal=" << token.m_literal.type().name() << "}";
	return output_s;
}
