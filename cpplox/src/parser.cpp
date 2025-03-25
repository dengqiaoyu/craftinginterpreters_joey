#include "parser.h"

#include <stdexcept>

#include "general.h"
#include "lox.h"
#include "token.h"

Parser::ParserError::~ParserError() = default;

// =====================================================================================================================
// Private methods.

const std::vector<Token>&
Parser::get_tokens() const
{
	return m_tokens;
}

Token
Parser::advance()
{
	if (!is_at_end()) {
		current++;
	}
	return previous();
}

Token
Parser::consume(const TokenType type, const std::string& message)
{
	const bool consumed = check(type);
	require_throw(consumed, error(peek(), message));
	return advance();
}

void
Parser::synchronize()
{
	advance();
	while (!is_at_end()) {
		if (previous().get_type() == TokenType::SEMICOLON) {
			return;
		}
		ignore_warning_begin("-Wswitch-enum");
		switch (peek().get_type()) {
		case TokenType::CLASS:
		case TokenType::FOR:
		case TokenType::FUN:
		case TokenType::IF:
		case TokenType::PRINT:
		case TokenType::RETURN:
		case TokenType::VAR:
		case TokenType::WHILE: return;
		default: advance();
		}
		ignore_warning_end();
	}
}

bool
Parser::check(const TokenType type) const
{
	require_return_value(is_at_end(), false);
	return peek().get_type() == type;
}

bool
Parser::is_at_end() const
{
	return peek().get_type() == TokenType::END_OF_FILE;
}

const Token&
Parser::peek() const
{
	return get_tokens().at(current);
}

const Token&
Parser::previous() const
{
	return get_tokens().at(current - 1);
}

Parser::ParserError
Parser::error(const Token& token, const std::string& message)
{
	Lox::error(token, message);
	return ParserError(message);
}
