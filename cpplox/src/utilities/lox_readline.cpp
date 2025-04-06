#include "lox_readline.h"

#include <memory>
#include <readline/history.h>
#include <readline/readline.h>
#include <utility>

// =====================================================================================================================
// Constructors

LoxReadline::LoxReadline(std::string history_path) : m_history_path(std::move(history_path))
{
	// Initialize the readline history.
	using_history();

	// Load the history from the file.
	read_history(m_history_path.c_str());
}

// =====================================================================================================================
// Destructor

LoxReadline::~LoxReadline()
{
	// Save the history to the file.
	save_history();
}

// =====================================================================================================================
// Public methods

std::pair<std::string, bool>
LoxReadline::read_line(const std::string& prompt)
{
	// Read a line from the user.
	std::unique_ptr<char, decltype(&free)> line(readline(prompt.c_str()), &free);
	if (!line) {
		return std::make_pair("", true); // EOF or error.
	}

	// Add the line to the history.
	add_history(line.get());

	// Convert to std::string.
	std::string result(line.get());

	return std::make_pair(result, false);
}

// =====================================================================================================================
// Private methods

void
LoxReadline::save_history()
{
	// Save the history to the file.
	write_history(m_history_path.c_str());
}
