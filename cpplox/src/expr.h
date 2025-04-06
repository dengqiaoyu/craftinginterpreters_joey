#ifndef expr_H
#define expr_H

#include <any>
#include <memory>
#include <ostream>
#include <string>

#include "token.h"
#include "value.h"

// Forward declarations.
class Binary;
class Grouping;
class Literal;
class Unary;

// =====================================================================================================================
// Visitor class
class Visitor
{
public:
	Visitor() = default;
	Visitor(const Visitor&) = default;
	Visitor& operator=(const Visitor&) = default;
	Visitor(Visitor&&) noexcept = default;
	Visitor& operator=(Visitor&&) noexcept = default;
	virtual ~Visitor();

	[[nodiscard]] virtual std::any visit_binary_expr(const Binary& expr) const = 0;
	[[nodiscard]] virtual std::any visit_grouping_expr(const Grouping& expr) const = 0;
	[[nodiscard]] virtual std::any visit_literal_expr(const Literal& expr) const = 0;
	[[nodiscard]] virtual std::any visit_unary_expr(const Unary& expr) const = 0;
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

	[[nodiscard]] virtual std::any accept(const Visitor& visitor) const = 0;
	[[nodiscard]] virtual std::string to_string() const = 0;
	friend std::ostream& operator<<(std::ostream& out_s, const Expr& expr);
};

// =====================================================================================================================
class Binary : public Expr
{
public:
	Binary(std::shared_ptr<const Expr> left, Token opr, std::shared_ptr<const Expr> right);

	[[nodiscard]] std::any accept(const Visitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;

	[[nodiscard]] const std::shared_ptr<const Expr>& get_left() const;
	[[nodiscard]] const Token& get_opr() const;
	[[nodiscard]] const std::shared_ptr<const Expr>& get_right() const;

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

	[[nodiscard]] std::any accept(const Visitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;

	[[nodiscard]] const std::shared_ptr<const Expr>& get_expr() const;

private:
	std::shared_ptr<const Expr> m_expr;
};

// =====================================================================================================================
class Literal : public Expr
{
public:
	explicit Literal(Value value);

	[[nodiscard]] std::any accept(const Visitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;

	[[nodiscard]] const Value& get_value() const;

private:
	Value m_value;
};

// =====================================================================================================================
class Unary : public Expr
{
public:
	Unary(Token opr, std::shared_ptr<const Expr> right);

	[[nodiscard]] std::any accept(const Visitor& visitor) const override;
	[[nodiscard]] std::string to_string() const override;

	[[nodiscard]] const Token& get_opr() const;
	[[nodiscard]] const std::shared_ptr<const Expr>& get_right() const;

private:
	Token m_opr;
	std::shared_ptr<const Expr> m_right;
};

#endif // expr_H
