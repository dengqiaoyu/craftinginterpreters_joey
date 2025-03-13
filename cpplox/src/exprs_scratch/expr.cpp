#include "expr.h"

#include <string>

#include "general.h"

// =====================================================================================================================
// Expr

template<typename R>
Expr<R>::~Expr() = default;

// =====================================================================================================================
// Binary
template<typename R>
Binary<R>::Binary(std::shared_ptr<Expr<R>> left, Token opr, std::shared_ptr<Expr<R>> right)
	: m_left(std::move(left)), m_opr(std::move(opr)), m_right(std::move(right))
{
	// Empty constructor.
}

template<typename R>
std::string Binary<R>::to_string() const
{
	return fmt_str("Binary Expr{left=%s, opr:%s, right=%s}", m_left->to_string().c_str(), m_opr.to_string().c_str(),
		m_right->to_string().c_str());
}

template<typename R>
const Expr<R>& Binary<R>::get_left() const
{
	return *m_left;
}

template<typename R>
const Token& Binary<R>::get_opr() const
{
	return m_opr;
}

template<typename R>
const Expr<R>& Binary<R>::get_right() const
{
	return *m_right;
}
