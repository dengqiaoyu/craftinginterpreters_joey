#include "lox.h"

#include <any>
#include <cstddef>
#include <cstdlib>
#include <format>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <sysexits.h> // EX_DATAERR (65)
#include <vector>

#include "asts/expr.h"
#include "general.h"
#include "parser.h"
#include "scanner.h"
#include "token_type.h"
#include "utilities/lox_readline.h"
#include "visitors/interpreter.h"

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
	if (m_had_runtime_error) {
		std::quick_exit(EX_SOFTWARE);
	}
}

void
Lox::run_prompt()
{
	LoxReadline lox_readline;

	while (true) {
		auto [line, is_eof] = LoxReadline::read_line("> ");
		// Check for EOF (Ctrl+D) or error.
		if (is_eof || line == "exit" || line == "quit") {
			break;
		}
		// Ignore empty lines.
		if (line.empty()) {
			continue;
		}
		run(line, true);
		m_had_error = false;
		m_had_runtime_error = false;
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
		report(token.get_line(), "at the end", message);
	} else {
		report(token.get_line(), std::format("at '{}'", token.get_lexeme()), message);
	}
}

// =====================================================================================================================
// Private methods.

// ====================================================================================================================

Interpreter&
Lox::get_interpreter()
{
	static auto* interpreter = new Interpreter(); // NOLINT(cppcoreguidelines-owning-memory)
	return *interpreter;
}

// =====================================================================================================================

void
Lox::run(const std::string& content, const bool repl)
{
	// Reset interpreter state at the beginning to ensure no previous results are shown if parsing fails
	get_interpreter().reset_last_expression_state();

	const Scanner scanner(content);
	const std::vector<Token>& tokens = scanner.get_tokens();

	Parser parser(tokens, repl);
	try {
		std::vector<std::shared_ptr<Stmt>> statements = parser.parse();
		get_interpreter().interpret(statements);
	} catch (const std::exception& statements_e) {
		// Parsing or interpretation failed
		(void)statements_e;
	}

	// In REPL mode, check if the last expression was evaluated
	if (repl) {
		bool last_expression_evaluated = false;
		std::any last_expression_result = get_interpreter().get_last_expression_result(last_expression_evaluated);
		if (last_expression_evaluated) {
			std::cout << Interpreter::stringify(last_expression_result) << std::endl;
		}
	}
}

// =====================================================================================================================

bool Lox::m_had_error = false;
void
Lox::report(const size_t line, const std::string& where, const std::string& message)
{
	std::cout << std::format("[line {}] Error {}: {}\n", line, where, message);
	m_had_error = true;
}

// =====================================================================================================================

bool Lox::m_had_runtime_error = false;
void
Lox::runtime_error(const RuntimeError& error)
{
	std::cout << error.get_message() << std::endl;
	std::cout << std::format("[line {}]", error.get_token().get_line()) << std::endl;
	m_had_runtime_error = true;
}
