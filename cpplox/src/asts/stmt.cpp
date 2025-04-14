#include "stmt.h"

// =====================================================================================================================
// visitor
StmtVisitor::~StmtVisitor() = default;

// =====================================================================================================================
// Stmt

Stmt::~Stmt() = default;

std::ostream&
operator<<(std::ostream& out_s, const Stmt& stmt)
{
	return out_s << stmt.to_string();
}

// Formatter specialization for Stmt
template <>
struct std::formatter<Stmt> : std::formatter<std::string> // NOLINT(altera-struct-pack-align)
{
	auto format(const Stmt& stmt, format_context& ctx) const
	{
		return std::formatter<std::string>::format(stmt.to_string(), ctx);
	}
};

// =====================================================================================================================
// Expression

Expression::Expression(std::shared_ptr<const Expr> expr) : m_expr(std::move(expr))
{
	// Empty constructor.
}

const std::shared_ptr<const Expr>&
Expression::get_expr() const
{
	return m_expr;
}

std::any
Expression::accept(const StmtVisitor& visitor) const
{
	return visitor.visit_expression_stmt(*this);
}

std::string
Expression::to_string() const
{
	return std::format("Expression stmt{{expr={}}}", m_expr->to_string());
}

// =====================================================================================================================
// Print

Print::Print(std::shared_ptr<const Expr> expr) : m_expr(std::move(expr))
{
	// Empty constructor.
}

const std::shared_ptr<const Expr>&
Print::get_expr() const
{
	return m_expr;
}

std::any
Print::accept(const StmtVisitor& visitor) const
{
	return visitor.visit_print_stmt(*this);
}

std::string
Print::to_string() const
{
	return std::format("Print stmt{{expr={}}}", m_expr->to_string());
}
