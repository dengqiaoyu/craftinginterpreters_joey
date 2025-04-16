#ifndef expr_H
#define expr_H

#include <any>
#include <memory>
#include <ostream>
#include <string>

#include "token.h"
#include "value.h"

// Forward declarations.
class Assign;
class Binary;
class Grouping;
class Literal;
class Ternary;
class Unary;
class Variable;

// =====================================================================================================================
// Visitor class
class ExprVisitor
{
public:
	ExprVisitor() = default;
	ExprVisitor(const ExprVisitor&) = default;
	ExprVisitor& operator=(const ExprVisitor&) = default;
	ExprVisitor(ExprVisitor&&) noexcept = default;
	ExprVisitor& operator=(ExprVisitor&&) noexcept = default;
	virtual ~ExprVisitor();

	[[nodiscard]] virtual std::any visit_assign_expr(const Assign& expr) = 0;
	[[nodiscard]] virtual std::any visit_binary_expr(const Binary& expr) = 0;
	[[nodiscard]] virtual std::any visit_grouping_expr(const Grouping& expr) = 0;
	[[nodiscard]] virtual std::any visit_literal_expr(const Literal& expr) = 0;
	[[nodiscard]] virtual std::any visit_ternary_expr(const Ternary& expr) = 0;
	[[nodiscard]] virtual std::any visit_unary_expr(const Unary& expr) = 0;
	[[nodiscard]] virtual std::any visit_variable_expr(const Variable& expr) = 0;
};

// =====================================================================================================================
// Base Expr
class Expr
{
public:
	Expr() = default;
	Expr(const Expr&) = default;
	Expr& operator=(const Expr&) = default;
	Expr(Expr&&) noexcept = default;
	Expr& operator=(Expr&&) noexcept = default;
	virtual ~Expr();

	[[nodiscard]] virtual std::any accept(ExprVisitor& visitor) const = 0;
	[[nodiscard]] virtual std::string to_string() const = 0;
	friend std::ostream& operator<<(std::ostream& out_s, const Expr& expr);
};

// =====================================================================================================================
class Assign : public Expr
{
public:
	Assign(Token name, std::shared_ptr<const Expr> value);

	[[nodiscard]] const Token& get_name() const;
	[[nodiscard]] const std::shared_ptr<const Expr>& get_value() const;

	[[nodiscard]] std::any accept(ExprVisitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;
private:
	Token m_name;
	std::shared_ptr<const Expr> m_value;
};

// =====================================================================================================================
class Binary : public Expr
{
public:
	Binary(std::shared_ptr<const Expr> left, Token opr, std::shared_ptr<const Expr> right);

	[[nodiscard]] const std::shared_ptr<const Expr>& get_left() const;
	[[nodiscard]] const Token& get_opr() const;
	[[nodiscard]] const std::shared_ptr<const Expr>& get_right() const;

	[[nodiscard]] std::any accept(ExprVisitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;
private:
	std::shared_ptr<const Expr> m_left;
	Token m_opr;
	std::shared_ptr<const Expr> m_right;
};

// =====================================================================================================================
class Grouping : public Expr
{
public:
	explicit Grouping(std::shared_ptr<const Expr> expr);

	[[nodiscard]] const std::shared_ptr<const Expr>& get_expr() const;

	[[nodiscard]] std::any accept(ExprVisitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;
private:
	std::shared_ptr<const Expr> m_expr;
};

// =====================================================================================================================
class Literal : public Expr
{
public:
	explicit Literal(Value value);

	[[nodiscard]] const Value& get_value() const;

	[[nodiscard]] std::any accept(ExprVisitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;
private:
	Value m_value;
};

// =====================================================================================================================
class Ternary : public Expr
{
public:
	Ternary(std::shared_ptr<const Expr> condition, Token qmark, std::shared_ptr<const Expr> then_branch, Token colon, std::shared_ptr<const Expr> else_branch);

	[[nodiscard]] const std::shared_ptr<const Expr>& get_condition() const;
	[[nodiscard]] const Token& get_qmark() const;
	[[nodiscard]] const std::shared_ptr<const Expr>& get_then_branch() const;
	[[nodiscard]] const Token& get_colon() const;
	[[nodiscard]] const std::shared_ptr<const Expr>& get_else_branch() const;

	[[nodiscard]] std::any accept(ExprVisitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;
private:
	std::shared_ptr<const Expr> m_condition;
	Token m_qmark;
	std::shared_ptr<const Expr> m_then_branch;
	Token m_colon;
	std::shared_ptr<const Expr> m_else_branch;
};

// =====================================================================================================================
class Unary : public Expr
{
public:
	Unary(Token opr, std::shared_ptr<const Expr> right);

	[[nodiscard]] const Token& get_opr() const;
	[[nodiscard]] const std::shared_ptr<const Expr>& get_right() const;

	[[nodiscard]] std::any accept(ExprVisitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;
private:
	Token m_opr;
	std::shared_ptr<const Expr> m_right;
};

// =====================================================================================================================
class Variable : public Expr
{
public:
	explicit Variable(Token name);

	[[nodiscard]] const Token& get_name() const;

	[[nodiscard]] std::any accept(ExprVisitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;
private:
	Token m_name;
};

#endif // expr_H
