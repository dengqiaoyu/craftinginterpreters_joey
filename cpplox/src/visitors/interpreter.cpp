#include "interpreter.h"

#include "general.h"
#include <any>
#include <cstddef>
#include <cstdlib>

// =====================================================================================================================
// Static methods

namespace {

bool
is_truthy(const std::any& any)
{
	if (any.type() == typeid(std::nullptr_t)) {
		return false;
	}
	if (any.type() == typeid(bool)) {
		return std::any_cast<bool>(any);
	}
	return true;
}

} // namespace

// =====================================================================================================================
// Public methods

std::any
Interpreter::visit_grouping_expr(const Grouping& expr) const
{
	return evaluate(expr.get_expr());
}

// =====================================================================================================================

std::any
Interpreter::visit_literal_expr(const Literal& expr) const
{
	return expr.get_value();
}

// =====================================================================================================================

std::any
Interpreter::visit_unary_expr(const Unary& expr) const
{
	const std::any right = evaluate(expr.get_right());

	try {
		ignore_warning_begin("-Wswitch-enum");
		switch (expr.get_opr().get_type()) {
		case TokenType::BANG: return !is_truthy(right);
		case TokenType::MINUS: return -std::any_cast<double>(right);
		default: break;
		}
		ignore_warning_end();
	} catch (const std::bad_any_cast& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		// FIXME: Handle error.
		abort();
	}

	return {};
}

// =====================================================================================================================
// Private methods

std::any
Interpreter::evaluate(const std::shared_ptr<const Expr>& expr) const
{
	require_assert(expr);
	return expr->accept(*this);
}
