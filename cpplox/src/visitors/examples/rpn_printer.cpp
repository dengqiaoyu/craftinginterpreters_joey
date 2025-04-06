#include "rpn_printer.h"

#include "token.h"

// =====================================================================================================================
// Public methods

std::string
RpnPrinter::convert_string(const Expr& expr) const
{
	return std::any_cast<std::string>(expr.accept(*this));
}

// =====================================================================================================================

std::any
RpnPrinter::visit_binary_expr(const Binary& expr) const
{
	return std::format("{} {} {}", std::any_cast<std::string>(expr.get_left()->accept(*this)),
		std::any_cast<std::string>(expr.get_right()->accept(*this)), expr.get_opr().get_lexeme());
}

// =====================================================================================================================

std::any
RpnPrinter::visit_ternary_expr(const Ternary& expr) const
{
	return std::format("{} {} {} {}", std::any_cast<std::string>(expr.get_condition()->accept(*this)),
		std::any_cast<std::string>(expr.get_then_branch()->accept(*this)),
		std::any_cast<std::string>(expr.get_else_branch()->accept(*this)), "<ternary>");
}

// =====================================================================================================================

std::any
RpnPrinter::visit_grouping_expr(const Grouping& expr) const
{
	return expr.get_expr()->accept(*this);
}

// =====================================================================================================================

std::any
RpnPrinter::visit_literal_expr(const Literal& expr) const
{
	return std::format("{}", expr.get_value());
}

// =====================================================================================================================

std::any
RpnPrinter::visit_unary_expr(const Unary& expr) const
{
	return std::format("{} {}", std::any_cast<std::string>(expr.get_right()->accept(*this)),
		expr.get_opr().get_lexeme());
}
