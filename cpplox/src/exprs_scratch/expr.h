#ifndef EXPR_H
#define EXPR_H

#include <memory>
#include <string>

#include "token.h"

template <typename R>
class Binary;

template <typename R>
class Visitor
{
public:
	Visitor() = default;
	Visitor(const Visitor&) = default;
	Visitor& operator=(const Visitor&) = default;
	Visitor(Visitor&&) noexcept = default;
	Visitor& operator=(Visitor&&) noexcept = default;
	virtual ~Visitor() = default;

	virtual R visit_binary_expr(const Binary<R>& expr) = 0;
};

template <typename R>
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

	virtual R accept(Visitor<R>& visitor) = 0;

	friend std::ostream& operator<<(std::ostream& out_s, const Expr& expr)
	{
		return out_s << expr.to_string();
	}
};

template <typename R>
struct std::formatter<Expr<R>> : std::formatter<std::string> { // NOLINT(altera-struct-pack-align)
	auto format(const Expr<R>& expr, format_context& ctx) const
	{
		return std::formatter<std::string>::format(expr.to_string(), ctx);
	}
};

template <typename R>
class Binary : public Expr<R>
{
public:
	Binary(std::shared_ptr<Expr<R>> left, Token opr, std::shared_ptr<Expr<R>> right);
	[[nodiscard]] std::string to_string() const override;

	[[nodiscard]] const Expr<R>& get_left() const;
	[[nodiscard]] const Token& get_opr() const;
	[[nodiscard]] const Expr<R>& get_right() const;

private:
	std::shared_ptr<Expr<R>> m_left;
	Token m_opr;
	std::shared_ptr<Expr<R>> m_right;
};

#endif // EXPR_H
