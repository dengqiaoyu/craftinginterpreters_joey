#include "lox.h"

#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <string>
#include <sysexits.h> // EX_DATAERR (65)
#include <vector>

#include "general.h"
#include "scanner.h"

void Lox::run_file(const std::string& path)
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

void Lox::run_prompt()
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

bool Lox::m_had_error = false;
void Lox::error(const size_t line, const std::string& message)
{
	m_had_error = true;
	std::cout << "[line " << line << "] Error: " << message << std::endl;
}

void Lox::run(const std::string& content)
{
	const Scanner scanner(content);
	const std::vector<Token>& tokens = scanner.get_tokens();
	for (const Token& token : tokens) {
		std::cout << token << std::endl;
	}
}
