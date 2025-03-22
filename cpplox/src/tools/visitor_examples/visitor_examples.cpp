#include <iostream>
#include <memory>

#include "expr.h"
#include "visitors/examples/ast_printer.h"
#include "visitors/examples/rpn_printer.h"

int
main(const int argc, const char* const argv[])
{
	(void)argc;
	(void)argv;
	std::cout << "Hello, World!" << std::endl;

	// clang-format off
	// -123 * (45.67)
	auto expr_0 = std::make_shared<Binary<std::string>>(
		std::make_shared<Unary<std::string>>(
			Token(TokenType::MINUS, "-"), std::make_shared<Literal<std::string>>(Value(123))),
		Token(TokenType::STAR, "*"),
		std::make_shared<Grouping<std::string>>(std::make_shared<Literal<std::string>>(Value(45.67))));

	// (-1 + 2) * (4 - 3)
	auto expr_1 = std::make_shared<Binary<std::string>>(
		std::make_shared<Grouping<std::string>>(
			std::make_shared<Binary<std::string>>(
				std::make_shared<Unary<std::string>>(
					Token(TokenType::MINUS, "-"), std::make_shared<Literal<std::string>>(Value(1))),
				Token(TokenType::PLUS, "+"),
				std::make_shared<Literal<std::string>>(Value(2)))),
		Token(TokenType::STAR, "*"),
		std::make_shared<Grouping<std::string>>(
			std::make_shared<Binary<std::string>>(
				std::make_shared<Literal<std::string>>(Value(4)),
				Token(TokenType::MINUS, "-"),
				std::make_shared<Literal<std::string>>(Value(3)))));

	// clang-format on

	AstPrinter astPrinter;
	std::cout << "-123 * (45.67) ===> " << astPrinter.convert_string(*expr_0) << std::endl;
	std::cout << "(-1 + 2) * (4 - 3) ===> " << astPrinter.convert_string(*expr_1) << std::endl;

	std::cout << std::endl;

	RpnPrinter rpnPrinter;
	std::cout << "-123 * (45.67) ===> " << rpnPrinter.convert_string(*expr_0) << std::endl;
	std::cout << "(-1 + 2) * (4 - 3) ===> " << rpnPrinter.convert_string(*expr_1) << std::endl;

	return 0;
}
