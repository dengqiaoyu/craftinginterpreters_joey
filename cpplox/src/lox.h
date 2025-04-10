#ifndef LOX_H
#define LOX_H

#include "token.h"
#include <cstddef>
#include <string>

#include "runtime_error.h"
#include "visitors/interpreter.h"

class Lox
{
public:
	static void run_file(const std::string& path);
	static void run_prompt();
	static void error(size_t line, const std::string& message);
	static void error(const Token& token, const std::string& message);
	static void runtime_error(const RuntimeError& error);

private:
	static bool m_had_error;
	static bool m_had_runtime_error;

	static Interpreter& get_interpreter();
	static void report(size_t line, const std::string& where, const std::string& message);
	static void run(const std::string& content);
};

#endif // LOX_H
