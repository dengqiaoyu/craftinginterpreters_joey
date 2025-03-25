#include "visitors/ast_printer.h"

#include <iostream>
#include <memory>

#include "token.h"

std::string
AstPrinter::visit_binary_expr(const Binary<std::string>& expr) const
{
	return parenthesize(expr.get_opr().get_lexeme(), expr.get_left(), expr.get_right());
}

std::string
AstPrinter::visit_grouping_expr(const Grouping<std::string>& expr) const
{
	return parenthesize("group", expr.get_expr());
}

std::string
AstPrinter::visit_literal_expr(const Literal<std::string>& expr) const
{
	return std::format("{}", expr.get_value());
}

std::string
AstPrinter::visit_unary_expr(const Unary<std::string>& expr) const
{
	return parenthesize(expr.get_opr().get_lexeme(), expr.get_right());
}

int
main(const int argc, const char* const argv[])
{
	(void)argc;
	(void)argv;
	std::cout << "Hello, World!" << std::endl;

	// clang-format off
	auto expr = std::make_shared<Binary<std::string>>(
		std::make_shared<Unary<std::string>>(
			Token(TokenType::MINUS, "-"), std::make_shared<Literal<std::string>>(Value(123))),
		Token(TokenType::STAR, "*"),
		std::make_shared<Grouping<std::string>>(std::make_shared<Literal<std::string>>(Value(45.67))));
	// clang-format on

	AstPrinter astPrinter;
	std::cout << astPrinter.print(*expr) << std::endl;

	return 0;
}
