#include "interpreter.h"

#include <any>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <string>

#include "expr.h"
#include "general.h"
#include "lox.h"
#include "runtime_error.h"
#include "value.h"

// =====================================================================================================================
// Static methods

namespace {

void
check_comparison_operands(const Token& opr, const std::any& left, const std::any& right)
{
	if (left.has_value() && right.has_value() && left.type() == typeid(Value) && right.type() == typeid(Value)) {
		const Value& left_value = std::any_cast<Value>(left);
		const Value& right_value = std::any_cast<Value>(right);
		if ((left_value.is_number() && right_value.is_number()) ||
			(left_value.is_string() && right_value.is_string())) {
			return;
		}
	}
	throw RuntimeError(opr, "Operands must be numbers or strings at the same time.");
}

// =====================================================================================================================

void
check_number_operand(const Token& opr, const std::any& operand)
{
	if (operand.has_value() && operand.type() == typeid(Value)) {
		const Value& value = std::any_cast<Value>(operand);
		if (value.is_number()) {
			return;
		}
	}
	throw RuntimeError(opr, "Operand must be number.");
}

// =====================================================================================================================

void
check_number_operands(const Token& opr, const std::any& left, const std::any& right)
{
	if (left.has_value() && right.has_value() && left.type() == typeid(Value) && right.type() == typeid(Value)) {
		const Value& left_value = std::any_cast<Value>(left);
		const Value& right_value = std::any_cast<Value>(right);
		if (left_value.is_number() && right_value.is_number()) {
			return;
		}
	}
	throw RuntimeError(opr, "Operands must be numbers.");
}

// =====================================================================================================================

void
check_number_or_string_operands(const Token& opr, const std::any& left, const std::any& right)
{

	if (left.has_value() && right.has_value() && left.type() == typeid(Value) && right.type() == typeid(Value)) {
		const Value& left_value = std::any_cast<Value>(left);
		const Value& right_value = std::any_cast<Value>(right);
		if ((left_value.is_number() || left_value.is_string()) &&
			(right_value.is_number() || right_value.is_string())) {
			return;
		}
	}
	throw RuntimeError(opr, "Operands must be numbers or strings.");
}

// =====================================================================================================================

bool
is_truthy(const std::any& any)
{
	if (!any.has_value()) {
		return false;
	}
	if (any.type() == typeid(std::nullptr_t)) {
		return false;
	}
	if (any.type() == typeid(Value)) {
		const Value& value = std::any_cast<Value>(any);
		if (value.is_nil()) {
			return false;
		}
		if (value.is_bool()) {
			return value.as_bool();
		}
		if (value.is_number()) {
			return value.as_number() != 0.0;
		}
	}
	return true;
}

// =====================================================================================================================

bool
is_equal(const std::any& a, const std::any& b)
{
	// Check if both are empty.
	if (!a.has_value() && !b.has_value()) {
		return true;
	}
	// If only one is empty, return false.
	if (!a.has_value() || !b.has_value()) {
		return false;
	}

	// Check if both are of the same type.
	if (a.type() != b.type()) {
		return false;
	}

	if (a.type() == typeid(Value)) {
		const Value& a_value = std::any_cast<Value>(a);
		const Value& b_value = std::any_cast<Value>(b);
		return a_value == b_value;
	}
	// Asserts that unknown types are not handled.
	require_assert_message(false, "Unknown type in is_equal");
}

// =====================================================================================================================

std::string
stringify(const std::any& any)
{
	if (!any.has_value()) {
		return "nil";
	}
	if (any.type() == typeid(Value)) {
		const Value& value = std::any_cast<Value>(any);
		std::string value_str = std::any_cast<Value>(any).to_string();
		if (value.is_string()) {
			value_str = "\"" + value_str + "\"";
		}
		return value_str;
	}
	require_assert_message(false, "Unknown type in stringify");
}

} // namespace

// =====================================================================================================================
// Public methods

void
Interpreter::interpret(const std::shared_ptr<const Expr>& expr)
{
	try {
		std::any result = evaluate(expr);
		std::cout << stringify(result) << std::endl;
	} catch (const RuntimeError& error) {
		Lox::runtime_error(error);
	}
}

std::any
Interpreter::visit_binary_expr(const Binary& expr) const
{
	std::any left = evaluate(expr.get_left());
	std::any right = evaluate(expr.get_right());

	// Number operations.
	ignore_warning_begin("-Wswitch-enum");
	switch (expr.get_opr().get_type()) {

	// Equality.
	case TokenType::BANG_EQUAL: return Value(!is_equal(left, right));
	case TokenType::EQUAL_EQUAL: return Value(is_equal(left, right));

	// Comparison.
	case TokenType::GREATER:
		check_comparison_operands(expr.get_opr(), left, right);
		return Value(std::any_cast<Value>(left) > std::any_cast<Value>(right));
	case TokenType::GREATER_EQUAL:
		check_comparison_operands(expr.get_opr(), left, right);
		return Value(std::any_cast<Value>(left) >= std::any_cast<Value>(right));
	case TokenType::LESS:
		check_comparison_operands(expr.get_opr(), left, right);
		return Value(std::any_cast<Value>(left) < std::any_cast<Value>(right));
	case TokenType::LESS_EQUAL:
		check_comparison_operands(expr.get_opr(), left, right);
		return Value(std::any_cast<Value>(left) <= std::any_cast<Value>(right));

	// Addition and subtraction.
	case TokenType::MINUS:
		check_number_operands(expr.get_opr(), left, right);
		return Value(std::any_cast<Value>(left).as_number() - std::any_cast<Value>(right).as_number());
	case TokenType::PLUS:
		// Number or string addition.
		check_number_or_string_operands(expr.get_opr(), left, right);
		return Value(std::any_cast<Value>(left) + std::any_cast<Value>(right));

	// Factor.
	case TokenType::STAR:
		check_number_operands(expr.get_opr(), left, right);
		return Value(std::any_cast<Value>(left).as_number() * std::any_cast<Value>(right).as_number());
	case TokenType::SLASH:
		check_number_operands(expr.get_opr(), left, right);
		if (std::any_cast<Value>(right).as_number() == 0.0) {
			// Division by zero.
			throw RuntimeError(expr.get_opr(), "Division by zero.");
		}
		return Value(std::any_cast<Value>(left).as_number() / std::any_cast<Value>(right).as_number());
	default: break;
	}
	ignore_warning_end();

	return {};
}

// ====================================================================================================================

std::any
Interpreter::visit_ternary_expr(const Ternary& expr) const
{
	if (is_truthy(evaluate(expr.get_condition()))) {
		return evaluate(expr.get_then_branch());
	} else { // NOLINT(llvm-else-after-return, readability-else-after-return)
		return evaluate(expr.get_else_branch());
	}
}

// ====================================================================================================================

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

	ignore_warning_begin("-Wswitch-enum");
	switch (expr.get_opr().get_type()) {
	case TokenType::BANG: return Value(!is_truthy(right));
	case TokenType::MINUS: {
		check_number_operand(expr.get_opr(), right);
		return Value(-std::any_cast<Value>(right).as_number());
	}
	default: break;
	}
	ignore_warning_end();
	require_assert_message(false, "Unknown unary operator");
}

// =====================================================================================================================
// Private methods

std::any
Interpreter::evaluate(const std::shared_ptr<const Expr>& expr) const
{
	require_assert(expr);
	return expr->accept(*this);
}
