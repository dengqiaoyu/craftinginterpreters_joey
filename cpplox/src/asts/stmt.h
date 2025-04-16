#ifndef stmt_H
#define stmt_H

#include <any>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "expr.h"
#include "token.h"

// Forward declarations.
class Block;
class Expression;
class Print;
class Var;

// =====================================================================================================================
// Visitor class
class StmtVisitor
{
public:
	StmtVisitor() = default;
	StmtVisitor(const StmtVisitor&) = default;
	StmtVisitor& operator=(const StmtVisitor&) = default;
	StmtVisitor(StmtVisitor&&) noexcept = default;
	StmtVisitor& operator=(StmtVisitor&&) noexcept = default;
	virtual ~StmtVisitor();

	[[nodiscard]] virtual std::any visit_block_stmt(const Block& stmt) = 0;
	[[nodiscard]] virtual std::any visit_expression_stmt(const Expression& stmt) = 0;
	[[nodiscard]] virtual std::any visit_print_stmt(const Print& stmt) = 0;
	[[nodiscard]] virtual std::any visit_var_stmt(const Var& stmt) = 0;
};

// =====================================================================================================================
// Base Stmt
class Stmt
{
public:
	Stmt() = default;
	Stmt(const Stmt&) = default;
	Stmt& operator=(const Stmt&) = default;
	Stmt(Stmt&&) noexcept = default;
	Stmt& operator=(Stmt&&) noexcept = default;
	virtual ~Stmt();

	[[nodiscard]] virtual std::any accept(StmtVisitor& visitor) const = 0;
	[[nodiscard]] virtual std::string to_string() const = 0;
	friend std::ostream& operator<<(std::ostream& out_s, const Stmt& stmt);
};

// =====================================================================================================================
class Block : public Stmt
{
public:
	explicit Block(std::vector<std::shared_ptr<const Stmt>> statements);

	[[nodiscard]] const std::vector<std::shared_ptr<const Stmt>>& get_statements() const;

	[[nodiscard]] std::any accept(StmtVisitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;
private:
	std::vector<std::shared_ptr<const Stmt>> m_statements;
};

// =====================================================================================================================
class Expression : public Stmt
{
public:
	explicit Expression(std::shared_ptr<const Expr> expr);

	[[nodiscard]] const std::shared_ptr<const Expr>& get_expr() const;

	[[nodiscard]] std::any accept(StmtVisitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;
private:
	std::shared_ptr<const Expr> m_expr;
};

// =====================================================================================================================
class Print : public Stmt
{
public:
	explicit Print(std::shared_ptr<const Expr> expr);

	[[nodiscard]] const std::shared_ptr<const Expr>& get_expr() const;

	[[nodiscard]] std::any accept(StmtVisitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;
private:
	std::shared_ptr<const Expr> m_expr;
};

// =====================================================================================================================
class Var : public Stmt
{
public:
	Var(Token name, std::shared_ptr<const Expr> initializer);

	[[nodiscard]] const Token& get_name() const;
	[[nodiscard]] const std::shared_ptr<const Expr>& get_initializer() const;

	[[nodiscard]] std::any accept(StmtVisitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;
private:
	Token m_name;
	std::shared_ptr<const Expr> m_initializer;
};

#endif // stmt_H
