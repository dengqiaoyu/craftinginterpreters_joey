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

// Formatter specialization for std::vector<Stmt>
template <>
// NOLINTNEXTLINE(altera-struct-pack-align)
struct std::formatter<std::vector<std::shared_ptr<const Stmt>>> : std::formatter<std::string> {
	auto format(const std::vector<std::shared_ptr<const Stmt>>& stmts, format_context& ctx) const
	{
		std::string result = "[";
		for (const auto& stmt : stmts) {
			result += std::format("{}, ", stmt->to_string());
		}
		result.pop_back(); // Remove the last space.
		result.pop_back(); // Remove the last comma.
		result += "]";
		return std::formatter<std::string>::format(result, ctx);
	}
};

// =====================================================================================================================
// Block

Block::Block(std::vector<std::shared_ptr<const Stmt>> statements) : m_statements(std::move(statements))
{
	// Empty constructor.
}

const std::vector<std::shared_ptr<const Stmt>>&
Block::get_statements() const
{
	return m_statements;
}

std::any
Block::accept(StmtVisitor& visitor) const
{
	return visitor.visit_block_stmt(*this);
}

std::string
Block::to_string() const
{
	return std::format("Block stmt{{statements={}}}", m_statements);
}

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
Expression::accept(StmtVisitor& visitor) const
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
Print::accept(StmtVisitor& visitor) const
{
	return visitor.visit_print_stmt(*this);
}

std::string
Print::to_string() const
{
	return std::format("Print stmt{{expr={}}}", m_expr->to_string());
}

// =====================================================================================================================
// Var

Var::Var(Token name, std::shared_ptr<const Expr> initializer)
	: m_name(std::move(name)), m_initializer(std::move(initializer))
{
	// Empty constructor.
}

const Token&
Var::get_name() const
{
	return m_name;
}

const std::shared_ptr<const Expr>&
Var::get_initializer() const
{
	return m_initializer;
}

std::any
Var::accept(StmtVisitor& visitor) const
{
	return visitor.visit_var_stmt(*this);
}

std::string
Var::to_string() const
{
	return std::format("Var stmt{{name={}, initializer={}}}", m_name.to_string(), m_initializer->to_string());
}
