#include "scanner.h"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "general.h"
#include "lox.h"
#include "token.h"
#include "token_type.h"

// =====================================================================================================================
// Public methods.

Scanner::Scanner(std::string source) : m_source(std::move(source))
{
	scan_tokens();
}

const std::vector<Token>& Scanner::get_tokens() const
{
	return m_tokens;
}

void Scanner::scan_tokens()
{
	while (!is_at_end()) {
		m_start = m_current;
		scan_token();
	}
	m_tokens.emplace_back(TokenType::END_OF_FILE, m_line);
}

// =====================================================================================================================
// Private methods.

const std::string& Scanner::get_source() const
{
	return m_source;
}

bool Scanner::is_at_end() const
{
	return m_current >= get_source().size();
}

void Scanner::scan_token()
{
	const char c = advance(); // NOLINT(readability-identifier-length)
	switch (c) {
	case '(': add_token(TokenType::LEFT_PAREN); break;
	case ')': add_token(TokenType::RIGHT_PAREN); break;
	case '{': add_token(TokenType::LEFT_BRACE); break;
	case '}': add_token(TokenType::RIGHT_BRACE); break;
	case ',': add_token(TokenType::COMMA); break;
	case '.': add_token(TokenType::DOT); break;
	case '-': add_token(TokenType::MINUS); break;
	case '+': add_token(TokenType::PLUS); break;
	case ';': add_token(TokenType::SEMICOLON); break;
	case '*': add_token(TokenType::STAR); break;

	// Two-character tokens.
	case '!': add_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
	case '=': add_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
	case '<': add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
	case '>': add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;

	// Longer tokens.
	case '/': {
		if (match('/')) {
			while (peek() != '\n' && !is_at_end()) {
				advance();
			}
		} else if (match('*')) {
			skip_slash_star_comment();
		} else {
			add_token(TokenType::SLASH);
		}
		break;
	}

	// Whitespace characters.
	case ' ':
	case '\r':
	case '\t':
		// Ignore whitespace characters.
		break;

	case '\n': m_line++; break;

	// Literals.
	// String literals.
	case '"': add_string_literal(); break;

	// Number literals.
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9': add_number_literal(); break;

	default:
		if (is_alpha(c)) {
			add_identifier();
		} else {
			Lox::error(m_line, fmt_str("Unexpected character: %c", c));
		}
		break;
	}
}

char Scanner::advance()
{
	return get_source()[m_current++];
}

void Scanner::add_token(const TokenType type, const std::any& literal)
{
	const std::string lexeme = get_source().substr(m_start, m_current - m_start);
	m_tokens.emplace_back(type, m_line, lexeme, literal);
}

char Scanner::peek() const
{
	if (is_at_end()) {
		return '\0';
	}
	return get_source()[m_current];
}

char Scanner::peek_next() const
{
	if (m_current + 1 >= get_source().size()) {
		return '\0';
	}
	return get_source()[m_current + 1];
}

bool Scanner::match(const char expected)
{
	if (is_at_end()) {
		return false;
	}
	if (peek() != expected) {
		return false;
	}
	m_current++;
	return true;
}

void Scanner::skip_slash_star_comment()
{
	size_t depth = 1;
	while (depth > 0) {
		if (is_at_end()) {
			Lox::error(m_line, fmt_str("Unterminated comment."));
			return;
		}
		if (peek() == '\n') {
			m_line++;
		}
		if (peek() == '/' && peek_next() == '*') {
			depth++;
			advance();
		} else if (peek() == '*' && peek_next() == '/') {
			depth--;
			advance();
		}
		advance();
	}
}

void Scanner::add_string_literal()
{
	while (peek() != '"' && !is_at_end()) {
		if (peek() == '\n') {
			m_line++;
		}
		advance();
	}

	if (is_at_end()) {
		Lox::error(m_line, fmt_str("Unterminated string."));
		return;
	}

	advance(); // Consume the closing quote.

	const std::string value = get_source().substr(m_start + 1, m_current - m_start - 2);
	add_token(TokenType::STRING, value);
}

void Scanner::add_number_literal()
{
	while (is_digit(peek())) {
		advance();
	}
	if (peek() == '.' && is_digit(peek_next())) {
		// Consume the dot.
		advance();
		while (is_digit(peek())) {
			advance();
		}
	}
	const double value = std::stod(get_source().substr(m_start, m_current - m_start));
	add_token(TokenType::NUMBER, value);
}

void Scanner::add_identifier()
{
	while (is_alpha_numeric(peek())) {
		advance();
	}
	const std::string text = get_source().substr(m_start, m_current - m_start);
	const auto token_it = keyword_map().find(text);
	TokenType type = TokenType::IDENTIFIER;
	if (token_it != keyword_map().end()) {
		type = token_it->second;
	}
	add_token(type);
}

// =====================================================================================================================
// Static methods.

bool Scanner::is_digit(const char ch) // NOLINT(readability-identifier-length)
{
	return ch >= '0' && ch <= '9';
}

bool Scanner::is_alpha(const char ch) // NOLINT(readability-identifier-length)
{
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

bool Scanner::is_alpha_numeric(const char ch) // NOLINT(readability-identifier-length)
{
	return is_alpha(ch) || is_digit(ch);
}

// clang-format off
IGNORE_WARNING_BEGIN(-Wexit-time-destructors)
const std::unordered_map<std::string, TokenType>& Scanner::keyword_map()
{
	static const std::unordered_map<std::string, TokenType> keywords = {
		{"and", TokenType::AND},
		{"class", TokenType ::CLASS},
		{"else", TokenType::ELSE},
		{"false", TokenType::FALSE},
		{"for", TokenType::FOR},
		{"fun", TokenType::FUN},
		{"if", TokenType::IF},
		{"nil", TokenType::NIL},
		{"or", TokenType::OR},
		{"print", TokenType::PRINT},
		{"return", TokenType::RETURN},
		{"super", TokenType::SUPER},
		{"this", TokenType::THIS},
		{"true", TokenType::TRUE},
		{"var", TokenType::VAR},
		{"while", TokenType::WHILE},
	};
	return keywords;
}
IGNORE_WARNING_END()
// clang-format on
