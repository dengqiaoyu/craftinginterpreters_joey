#include "token.h"

#include <format>

Token::Token(TokenType type, std::string lexeme) : m_line(1), m_lexeme(std::move(lexeme)), m_type(type)
{
	// Empty constructor.
}

Token::Token(TokenType type, size_t line) : m_line(line), m_type(type)
{
	// Empty constructor.
}

Token::Token(TokenType type, size_t line, Value literal, std::string lexeme)
	: m_line(line), m_lexeme(std::move(lexeme)), m_literal(std::move(literal)), m_type(type)
{
	// Empty constructor.
}

std::string
Token::to_string() const
{
	return std::format("Token{{type={}, line={}, lexeme={}, literal={}}}", m_type, m_line, m_lexeme, m_literal);
}

std::string
Token::get_lexeme() const
{
	return m_lexeme;
}

std::ostream&
operator<<(std::ostream& output_s, const Token& token)
{
	return output_s << token.to_string();
}
