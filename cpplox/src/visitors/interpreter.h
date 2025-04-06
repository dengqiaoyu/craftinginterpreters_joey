#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "expr.h"

class Interpreter : public Visitor
{
public:
	[[nodiscard]] std::any visit_binary_expr(const Binary& expr) const override;
	[[nodiscard]] std::any visit_grouping_expr(const Grouping& expr) const override;
	[[nodiscard]] std::any visit_literal_expr(const Literal& expr) const override;
	[[nodiscard]] std::any visit_unary_expr(const Unary& expr) const override;

private:
	[[nodiscard]] std::any evaluate(const std::shared_ptr<const Expr>& expr) const;
};

#endif // INTERPRETER_H
