#include "rpn_printer.h"

#include "token.h"

// =====================================================================================================================
// Public methods

std::string
RpnPrinter::convert_string(const Expr& expr)
{
	return std::any_cast<std::string>(expr.accept(*this));
}

// =====================================================================================================================

std::any
RpnPrinter::visit_assign_expr(const Assign& expr)
{
	return std::format("(= {} {})", expr.get_name().get_lexeme(),
		std::any_cast<std::string>(expr.get_value()->accept(*this)));
}

// =====================================================================================================================

std::any
RpnPrinter::visit_binary_expr(const Binary& expr)
{
	return std::format("{} {} {}", std::any_cast<std::string>(expr.get_left()->accept(*this)),
		std::any_cast<std::string>(expr.get_right()->accept(*this)), expr.get_opr().get_lexeme());
}

// =====================================================================================================================

std::any
RpnPrinter::visit_ternary_expr(const Ternary& expr)
{
	return std::format("{} {} {} {}", std::any_cast<std::string>(expr.get_condition()->accept(*this)),
		std::any_cast<std::string>(expr.get_then_branch()->accept(*this)),
		std::any_cast<std::string>(expr.get_else_branch()->accept(*this)), "<ternary>");
}

// =====================================================================================================================

std::any
RpnPrinter::visit_grouping_expr(const Grouping& expr)
{
	return expr.get_expr()->accept(*this);
}

// =====================================================================================================================

std::any
RpnPrinter::visit_literal_expr(const Literal& expr)
{
	return std::format("{}", expr.get_value());
}

// =====================================================================================================================

std::any
RpnPrinter::visit_variable_expr(const Variable& expr)
{
	return std::format("(var {})", expr.get_name().get_lexeme());
}

// =====================================================================================================================

std::any
RpnPrinter::visit_unary_expr(const Unary& expr)
{
	return std::format("{} {}", std::any_cast<std::string>(expr.get_right()->accept(*this)),
		expr.get_opr().get_lexeme());
}
