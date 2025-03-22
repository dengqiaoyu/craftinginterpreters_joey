#include "rpn_printer.h"

#include "token.h"

// =====================================================================================================================
// Public methods

std::string
RpnPrinter::convert_string(const Expr<std::string>& expr) const
{
	return expr.accept(*this);
}

// =====================================================================================================================

std::string
RpnPrinter::visit_binary_expr(const Binary<std::string>& expr) const
{
	return std::format("{} {} {}", expr.get_left().accept(*this), expr.get_right().accept(*this),
		expr.get_opr().get_lexeme());
}

// =====================================================================================================================

std::string
RpnPrinter::visit_grouping_expr(const Grouping<std::string>& expr) const
{
	return expr.get_expr().accept(*this);
}

// =====================================================================================================================

std::string
RpnPrinter::visit_literal_expr(const Literal<std::string>& expr) const
{
	return std::format("{}", expr.get_value());
}

// =====================================================================================================================

std::string
RpnPrinter::visit_unary_expr(const Unary<std::string>& expr) const
{
	return std::format("{} {}", expr.get_right().accept(*this), expr.get_opr().get_lexeme());
}
