#ifndef EXPR_H
#define EXPR_H

#include <memory>
#include <string>

#include "token.h"

class Expr
{
public:
	Expr() = default;
	Expr(const Expr&) = default;
	Expr& operator=(const Expr&) = default;
	Expr(Expr&&) noexcept = default;
	Expr& operator=(Expr&&) noexcept = default;
	virtual ~Expr();
	[[nodiscard]] virtual std::string to_string() const = 0;

	friend std::ostream& operator<<(std::ostream& out_s, const Expr& expr)
	{
		return out_s << expr.to_string();
	}
};

template <>
struct std::formatter<Expr> : std::formatter<std::string> { // NOLINT(altera-struct-pack-align)
	auto format(const Expr& expr, format_context& ctx) const
	{
		return std::formatter<std::string>::format(expr.to_string(), ctx);
	}
};

class Binary : public Expr
{
public:
	Binary(std::shared_ptr<Expr> left, Token opr, std::shared_ptr<Expr> right);
	[[nodiscard]] std::string to_string() const override;

	[[nodiscard]] const Expr& get_left() const;
	[[nodiscard]] const Token& get_opr() const;
	[[nodiscard]] const Expr& get_right() const;

private:
	std::shared_ptr<Expr> m_left;
	Token m_opr;
	std::shared_ptr<Expr> m_right;
};

class Grouping : public Expr
{
public:
	explicit Grouping(std::shared_ptr<Expr> expr);
	[[nodiscard]] std::string to_string() const override;

	[[nodiscard]] const Expr& get_expr() const;

private:
	std::shared_ptr<Expr> m_expr;
};

class Literal : public Expr
{
public:
	explicit Literal(std::any value);
	[[nodiscard]] std::string to_string() const override;

	[[nodiscard]] const std::any& get_value() const;

private:
	std::any m_value;
};

class Unary : public Expr
{
public:
	Unary(Token opr, std::shared_ptr<Expr> right);
	[[nodiscard]] std::string to_string() const override;

	[[nodiscard]] const Token& get_opr() const;
	[[nodiscard]] const Expr& get_right() const;

private:
	Token m_opr;
	std::shared_ptr<Expr> m_right;
};

#endif // EXPR_H
