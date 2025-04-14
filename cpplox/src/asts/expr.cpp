#include "expr.h"

// =====================================================================================================================
// visitor
ExprVisitor::~ExprVisitor() = default;

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
Binary::accept(const ExprVisitor& visitor) const
{
	return visitor.visit_binary_expr(*this);
}

std::string
Binary::to_string() const
{
	return std::format("Binary expr{{left={}, opr={}, right={}}}", m_left->to_string(), m_opr.to_string(), m_right->to_string());
}

// =====================================================================================================================
// Ternary

Ternary::Ternary(std::shared_ptr<const Expr> condition, Token qmark, std::shared_ptr<const Expr> then_branch, Token colon, std::shared_ptr<const Expr> else_branch)
	: m_condition(std::move(condition)), m_qmark(std::move(qmark)), m_then_branch(std::move(then_branch)), m_colon(std::move(colon)), m_else_branch(std::move(else_branch))
{
	// Empty constructor.
}

const std::shared_ptr<const Expr>&
Ternary::get_condition() const
{
	return m_condition;
}

const Token&
Ternary::get_qmark() const
{
	return m_qmark;
}

const std::shared_ptr<const Expr>&
Ternary::get_then_branch() const
{
	return m_then_branch;
}

const Token&
Ternary::get_colon() const
{
	return m_colon;
}

const std::shared_ptr<const Expr>&
Ternary::get_else_branch() const
{
	return m_else_branch;
}

std::any
Ternary::accept(const ExprVisitor& visitor) const
{
	return visitor.visit_ternary_expr(*this);
}

std::string
Ternary::to_string() const
{
	return std::format("Ternary expr{{condition={}, qmark={}, then_branch={}, colon={}, else_branch={}}}", m_condition->to_string(), m_qmark.to_string(), m_then_branch->to_string(), m_colon.to_string(), m_else_branch->to_string());
}

// =====================================================================================================================
// Grouping

Grouping::Grouping(std::shared_ptr<const Expr> expr)
	: m_expr(std::move(expr))
{
	// Empty constructor.
}

const std::shared_ptr<const Expr>&
Grouping::get_expr() const
{
	return m_expr;
}

std::any
Grouping::accept(const ExprVisitor& visitor) const
{
	return visitor.visit_grouping_expr(*this);
}

std::string
Grouping::to_string() const
{
	return std::format("Grouping expr{{expr={}}}", m_expr->to_string());
}

// =====================================================================================================================
// Literal

Literal::Literal(Value value)
	: m_value(std::move(value))
{
	// Empty constructor.
}

const Value&
Literal::get_value() const
{
	return m_value;
}

std::any
Literal::accept(const ExprVisitor& visitor) const
{
	return visitor.visit_literal_expr(*this);
}

std::string
Literal::to_string() const
{
	return std::format("Literal expr{{value={}}}", m_value.to_string());
}

// =====================================================================================================================
// Variable

Variable::Variable(Token name)
	: m_name(std::move(name))
{
	// Empty constructor.
}

const Token&
Variable::get_name() const
{
	return m_name;
}

std::any
Variable::accept(const ExprVisitor& visitor) const
{
	return visitor.visit_variable_expr(*this);
}

std::string
Variable::to_string() const
{
	return std::format("Variable expr{{name={}}}", m_name.to_string());
}

// =====================================================================================================================
// Unary

Unary::Unary(Token opr, std::shared_ptr<const Expr> right)
	: m_opr(std::move(opr)), m_right(std::move(right))
{
	// Empty constructor.
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
Unary::accept(const ExprVisitor& visitor) const
{
	return visitor.visit_unary_expr(*this);
}

std::string
Unary::to_string() const
{
	return std::format("Unary expr{{opr={}, right={}}}", m_opr.to_string(), m_right->to_string());
}

