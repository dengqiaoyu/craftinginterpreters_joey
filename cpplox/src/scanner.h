#ifndef SCANNER_H
#define SCANNER_H

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "token.h"

class Scanner
{
public:
	explicit Scanner(std::string source);
	[[nodiscard]] const std::vector<Token>& get_tokens() const;

private:
	std::string m_source;
	std::vector<Token> m_tokens;

	size_t m_start = 0;
	size_t m_current = 0;
	size_t m_line = 1;

	[[nodiscard]] const std::string& get_source() const;
	void scan_tokens();
	[[nodiscard]] bool is_at_end() const;
	void scan_token();
	char advance();
	void add_token(TokenType type, const std::any& literal = std::any());
	[[nodiscard]] char peek() const;
	[[nodiscard]] char peek_next() const;
	bool match(char expected);
	void skip_slash_star_comment();
	void add_string_literal();
	void add_number_literal();
	void add_identifier();

	static bool is_digit(char ch);		   // NOLINT(readability-identifier-length)
	static bool is_alpha(char ch);		   // NOLINT(readability-identifier-length)
	static bool is_alpha_numeric(char ch); // NOLINT(readability-identifier-length)
	static const std::unordered_map<std::string, TokenType>& keyword_map();
};

#endif // SCANNER_H
