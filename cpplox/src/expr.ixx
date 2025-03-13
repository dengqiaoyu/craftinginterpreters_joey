module;

#include <any>
#include <memory>
#include <ostream>
#include <string>

#include "general.h"
#include "token.h"

export module expr_module;

export
{
	// Forward declarations.
	template <typename R>
	class Binary;

	template <typename R>
	class Grouping;

	template <typename R>
	class Literal;

	template <typename R>
	class Unary;

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
		virtual R visit_grouping_expr(const Grouping<R>& expr) = 0;
		virtual R visit_literal_expr(const Literal<R>& expr) = 0;
		virtual R visit_unary_expr(const Unary<R>& expr) = 0;
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

		friend std::ostream& operator<<(std::ostream& out_s, const Expr& expr);
	};

	template <typename R>
	class Binary : public Expr<R>
	{
	public:
		Binary(std::shared_ptr<Expr<R>> left, Token opr, std::shared_ptr<Expr<R>> right);
		[[nodiscard]] R accept(Visitor<R>& visitor) override;
		[[nodiscard]] std::string to_string() const override;

		[[nodiscard]] const Expr<R>& get_left() const;
		[[nodiscard]] const Token& get_opr() const;
		[[nodiscard]] const Expr<R>& get_right() const;

	private:
		std::shared_ptr<Expr<R>> m_left;
		Token m_opr;
		std::shared_ptr<Expr<R>> m_right;
	};

	template <typename R>
	class Grouping : public Expr<R>
	{
	public:
		explicit Grouping(std::shared_ptr<Expr<R>> expr);
		[[nodiscard]] R accept(Visitor<R>& visitor) override;
		[[nodiscard]] std::string to_string() const override;

		[[nodiscard]] const Expr<R>& get_expr() const;

	private:
		std::shared_ptr<Expr<R>> m_expr;
	};

	template <typename R>
	class Literal : public Expr<R>
	{
	public:
		explicit Literal(std::any value);
		[[nodiscard]] R accept(Visitor<R>& visitor) override;
		[[nodiscard]] std::string to_string() const override;

		[[nodiscard]] const std::any& get_value() const;

	private:
		std::any m_value;
	};

	template <typename R>
	class Unary : public Expr<R>
	{
	public:
		Unary(Token opr, std::shared_ptr<Expr<R>> right);
		[[nodiscard]] R accept(Visitor<R>& visitor) override;
		[[nodiscard]] std::string to_string() const override;

		[[nodiscard]] const Token& get_opr() const;
		[[nodiscard]] const Expr<R>& get_right() const;

	private:
		Token m_opr;
		std::shared_ptr<Expr<R>> m_right;
	};
}

// =====================================================================================================================
// Module-private implementation.
// This is not part of the module interface.
// =====================================================================================================================

module :private;

// =====================================================================================================================
// Expr

template <typename R>
Expr<R>::~Expr() = default;

template <typename R>
std::ostream& operator<<(std::ostream& out_s, const Expr<R>& expr)
{
	return out_s << expr.to_string();
}

// =====================================================================================================================
// Binary

template <typename R>
Binary<R>::Binary(std::shared_ptr<Expr<R>> left, Token opr, std::shared_ptr<Expr<R>> right)
	: m_left(std::move(left)), m_opr(std::move(opr)), m_right(std::move(right))
{
	// Empty constructor.
}

template <typename R>
std::string Binary<R>::to_string() const
{
	return fmt_str("Binary Expr{left=%s, opr=%s, right=%s}", m_left->to_string().c_str(), m_opr.to_string().c_str(),
		m_right->to_string().c_str());
}

template <typename R>
const Expr<R>& Binary<R>::get_left() const
{
	return *m_left;
}

template <typename R>
const Token& Binary<R>::get_opr() const
{
	return m_opr;
}

template <typename R>
const Expr<R>& Binary<R>::get_right() const
{
	return *m_right;
}

template <typename R>
R Binary<R>::accept(Visitor<R>& visitor)
{
	return visitor.visit_binary_expr(*this);
}

// =====================================================================================================================
// Grouping

template <typename R>
Grouping<R>::Grouping(std::shared_ptr<Expr<R>> expr) : m_expr(std::move(expr))
{
	// Empty constructor.
}

template <typename R>
std::string Grouping<R>::to_string() const
{
	return fmt_str("Grouping Expr{expr=%s}", m_expr->to_string().c_str());
}

template <typename R>
const Expr<R>& Grouping<R>::get_expr() const
{
	return *m_expr;
}

template <typename R>
R Grouping<R>::accept(Visitor<R>& visitor)
{
	return visitor.visit_grouping_expr(*this);
}

// =====================================================================================================================
// Literal

template <typename R>
Literal<R>::Literal(std::any value) : m_value(std::move(value))
{
	// Empty constructor.
}

template <typename R>
std::string Literal<R>::to_string() const
{
	return fmt_str("Literal Expr{value=%s}", "std::any object");
}

template <typename R>
const std::any& Literal<R>::get_value() const
{
	return m_value;
}

template <typename R>
R Literal<R>::accept(Visitor<R>& visitor)
{
	return visitor.visit_literal_expr(*this);
}

// =====================================================================================================================
// Unary

template <typename R>
Unary<R>::Unary(Token opr, std::shared_ptr<Expr<R>> right) : m_opr(std::move(opr)), m_right(std::move(right))
{
	// Empty constructor.
}

template <typename R>
std::string Unary<R>::to_string() const
{
	return fmt_str("Unary Expr{opr=%s, right=%s}", m_opr.to_string().c_str(), m_right->to_string().c_str());
}

template <typename R>
const Token& Unary<R>::get_opr() const
{
	return m_opr;
}

template <typename R>
const Expr<R>& Unary<R>::get_right() const
{
	return *m_right;
}

template <typename R>
R Unary<R>::accept(Visitor<R>& visitor)
{
	return visitor.visit_unary_expr(*this);
}
