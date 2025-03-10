#include "lox.h"

#include <cerrno>
#include <iostream>
#include <string>
#include <vector>

#include "general.h"

// NOLINTNEXTLINE(modernize-use-trailing-return-type)
int main(const int argc, const char* const argv[])
{
	require_action_return_value(argc <= 2, std::cout << "Usage: cpplox [script]" << std::endl, EINVAL);

	std::vector<std::string> args(argv, argv + argc);

	if (argc == 2) {
		Lox::run_file(args[1]);
	} else {
		// argc == 1
		Lox::run_prompt();
	}
	return 0;
}
