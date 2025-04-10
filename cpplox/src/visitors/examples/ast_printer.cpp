#include "ast_printer.h"

#include "token.h"

std::string
AstPrinter::convert_string(const Expr& expr) const
{
	return std::any_cast<std::string>(expr.accept(*this));
}

std::any
AstPrinter::visit_binary_expr(const Binary& expr) const
{
	return parenthesize(expr.get_opr().get_lexeme(), expr.get_left(), expr.get_right());
}

std::any
AstPrinter::visit_ternary_expr(const Ternary& expr) const
{
	return parenthesize("ternary", expr.get_condition(), expr.get_then_branch(), expr.get_else_branch());
}

std::any
AstPrinter::visit_grouping_expr(const Grouping& expr) const
{
	return parenthesize("group", expr.get_expr());
}

std::any
AstPrinter::visit_literal_expr(const Literal& expr) const
{
	const bool is_string = expr.get_value().is_string();
	if (is_string) {
		return std::format("\"{}\"", expr.get_value());
	}
	return std::format("{}", expr.get_value());
}

std::any
AstPrinter::visit_unary_expr(const Unary& expr) const
{
	return parenthesize(expr.get_opr().get_lexeme(), expr.get_right());
}
