module;

#include <memory>
#include <ostream>
#include <string>

#include "token.h"

export module expr_module;

export
{
	// Forward declaration.
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
	return fmt_str("Binary Expr{left=%s, opr:%s, right=%s}", m_left->to_string().c_str(), m_opr.to_string().c_str(),
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
