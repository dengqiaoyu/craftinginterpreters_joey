#include "lox.h"

#include <cstddef>
#include <cstdlib>
#include <format>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <string>
#include <sysexits.h> // EX_DATAERR (65)
#include <vector>

#include "general.h"
#include "scanner.h"

void
Lox::run_file(const std::string& path)
{
	std::ifstream file(path, std::ios::binary);
	require_throw(file, std::ios_base::failure("Failed to open file: " + path));

	// Read file into a byte buffer.
	const std::vector<char> content_bytes((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

	// Run the source.
	run(std::string(content_bytes.begin(), content_bytes.end()));

	if (m_had_error) {
		std::quick_exit(EX_DATAERR);
	}
}

void
Lox::run_prompt()
{
	std::string line;

	while (true) {
		std::cout << "> ";
		if (!std::getline(std::cin, line)) {
			break;
		}
		run(line);
		m_had_error = false;
	}
}

void
Lox::error(const size_t line, const std::string& message)
{
	report(line, "", message);
}

void
Lox::error(const Token& token, const std::string& message)
{
	if (token.get_type() == TokenType::END_OF_FILE) {
		report(token.get_line(), " at end", message);
	} else {
		report(token.get_line(), std::format("at '{}'", token.get_lexeme()), message);
	}
}

// =====================================================================================================================
// Private methods.

void
Lox::run(const std::string& content)
{
	const Scanner scanner(content);
	const std::vector<Token>& tokens = scanner.get_tokens();
	for (const Token& token : tokens) {
		std::cout << token << std::endl;
	}
}

bool Lox::m_had_error = false;
void
Lox::report(const size_t line, const std::string& where, const std::string& message)
{
	std::cout << std::format("[line {}] Error {}: {}\n", line, where, message);
	m_had_error = true;
}
