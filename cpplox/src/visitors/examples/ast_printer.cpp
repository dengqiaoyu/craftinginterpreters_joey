#include "ast_printer.h"

#include "token.h"

std::string
AstPrinter::convert_string(const Expr<std::string>& expr) const
{
	return expr.accept(*this);
}

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
