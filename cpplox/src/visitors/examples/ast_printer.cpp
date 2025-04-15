#include "ast_printer.h"

#include "token.h"
#include <any>

std::string
AstPrinter::convert_string(const Expr& expr)
{
	return std::any_cast<std::string>(expr.accept(*this));
}

std::any
AstPrinter::visit_binary_expr(const Binary& expr)
{
	return parenthesize(expr.get_opr().get_lexeme(), expr.get_left(), expr.get_right());
}

std::any
AstPrinter::visit_ternary_expr(const Ternary& expr)
{
	return parenthesize("ternary", expr.get_condition(), expr.get_then_branch(), expr.get_else_branch());
}

std::any
AstPrinter::visit_grouping_expr(const Grouping& expr)
{
	return parenthesize("group", expr.get_expr());
}

std::any
AstPrinter::visit_literal_expr(const Literal& expr)
{
	const bool is_string = expr.get_value().is_string();
	if (is_string) {
		return std::format("\"{}\"", expr.get_value());
	}
	return std::format("{}", expr.get_value());
}

std::any
AstPrinter::visit_variable_expr(const Variable& expr)
{
	return std::format("(var {})", expr.get_name().get_lexeme());
}

std::any
AstPrinter::visit_unary_expr(const Unary& expr)
{
	return parenthesize(expr.get_opr().get_lexeme(), expr.get_right());
}
