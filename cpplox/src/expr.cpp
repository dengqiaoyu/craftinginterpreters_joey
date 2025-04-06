#include "expr.h"

#include "general.h"

// =====================================================================================================================
// visitor
Visitor::~Visitor() = default;

// =====================================================================================================================
// Expr

Expr::~Expr() = default;

std::ostream&
operator<<(std::ostream& out_s, const Expr& expr)
{
	return out_s << expr.to_string();
}

// Formatter specialization for Expr
template <>
struct std::formatter<Expr> : std::formatter<std::string> // NOLINT(altera-struct-pack-align)
{
	auto
	format(const Expr& expr, format_context& ctx) const
	{
		return std::formatter<std::string>::format(expr.to_string(), ctx);
	}
};

// =====================================================================================================================
// Binary

Binary::Binary(std::shared_ptr<const Expr> left, Token opr, std::shared_ptr<const Expr> right)
	: m_left(std::move(left)), m_opr(std::move(opr)), m_right(std::move(right))
{
	// Empty constructor.
}

std::string
Binary::to_string() const
{
	return fmt_str("Binary Expr{left=%s, opr=%s, right=%s}", m_left->to_string().c_str(), m_opr.to_string().c_str(), m_right->to_string().c_str());
}

const std::shared_ptr<const Expr>&
Binary::get_left() const
{
	return m_left;
}

const Token&
Binary::get_opr() const
{
	return m_opr;
}

const std::shared_ptr<const Expr>&
Binary::get_right() const
{
	return m_right;
}

std::any
Binary::accept(const Visitor& visitor) const
{
	return visitor.visit_binary_expr(*this);
}

// =====================================================================================================================
// Grouping

Grouping::Grouping(std::shared_ptr<const Expr> expr)
	: m_expr(std::move(expr))
{
	// Empty constructor.
}

std::string
Grouping::to_string() const
{
	return fmt_str("Grouping Expr{expr=%s}", m_expr->to_string().c_str());
}

const std::shared_ptr<const Expr>&
Grouping::get_expr() const
{
	return m_expr;
}

std::any
Grouping::accept(const Visitor& visitor) const
{
	return visitor.visit_grouping_expr(*this);
}

// =====================================================================================================================
// Literal

Literal::Literal(Value value)
	: m_value(std::move(value))
{
	// Empty constructor.
}

std::string
Literal::to_string() const
{
	return fmt_str("Literal Expr{value=%s}", m_value.to_string().c_str());
}

const Value&
Literal::get_value() const
{
	return m_value;
}

std::any
Literal::accept(const Visitor& visitor) const
{
	return visitor.visit_literal_expr(*this);
}

// =====================================================================================================================
// Unary

Unary::Unary(Token opr, std::shared_ptr<const Expr> right)
	: m_opr(std::move(opr)), m_right(std::move(right))
{
	// Empty constructor.
}

std::string
Unary::to_string() const
{
	return fmt_str("Unary Expr{opr=%s, right=%s}", m_opr.to_string().c_str(), m_right->to_string().c_str());
}

const Token&
Unary::get_opr() const
{
	return m_opr;
}

const std::shared_ptr<const Expr>&
Unary::get_right() const
{
	return m_right;
}

std::any
Unary::accept(const Visitor& visitor) const
{
	return visitor.visit_unary_expr(*this);
}

