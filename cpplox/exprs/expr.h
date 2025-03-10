#ifndef EXPR_H
#define EXPR_H

#include "token.h"

class Expr // NOLINT(hicpp-special-member-functions, cppcoreguidelines-special-member-functions)
{
public:
	virtual ~Expr() = 0;
};

class Binary : public Expr
{
public:
	Binary(Expr left, Token op, Expr right);
	Expr left;
	Token op;
	Expr right;
};

#endif // EXPR_H
