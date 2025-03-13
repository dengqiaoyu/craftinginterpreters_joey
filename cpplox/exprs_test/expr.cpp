#include "expr.h"

#include <string>

#include "general.h"

// =====================================================================================================================
// Expr

Expr::~Expr() = default;

// =====================================================================================================================
// Binary

Binary::Binary(std::shared_ptr<Expr> left, Token opr, std::shared_ptr<Expr> right) : m_left(std::move(left)), m_opr(std::move(opr)), m_right(std::move(right))
{
	// Empty constructor.
}

std::string Binary::to_string() const
{
	return fmt_str("Binary Expr{left=%s, opr=%s, right=%s}", m_left->to_string().c_str(), m_opr.to_string().c_str(), m_right->to_string().c_str());
}

const Expr& Binary::get_left() const
{
	return *m_left;
}

const Token& Binary::get_opr() const
{
	return m_opr;
}

const Expr& Binary::get_right() const
{
	return *m_right;
}

// =====================================================================================================================
// Grouping

Grouping::Grouping(std::shared_ptr<Expr> expr) : m_expr(std::move(expr))
{
	// Empty constructor.
}

std::string Grouping::to_string() const
{
	return fmt_str("Grouping Expr{expr=%s}", m_expr->to_string().c_str());
}

const Expr& Grouping::get_expr() const
{
	return *m_expr;
}

// =====================================================================================================================
// Literal

Literal::Literal(std::any value) : m_value(std::move(value))
{
	// Empty constructor.
}

std::string Literal::to_string() const
{
	return fmt_str("Literal Expr{value=%s}", "std::any object");
}

const std::any& Literal::get_value() const
{
	return m_value;
}

// =====================================================================================================================
// Unary

Unary::Unary(Token opr, std::shared_ptr<Expr> right) : m_opr(std::move(opr)), m_right(std::move(right))
{
	// Empty constructor.
}

std::string Unary::to_string() const
{
	return fmt_str("Unary Expr{opr=%s, right=%s}", m_opr.to_string().c_str(), m_right->to_string().c_str());
}

const Token& Unary::get_opr() const
{
	return m_opr;
}

const Expr& Unary::get_right() const
{
	return *m_right;
}
