#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <any>

#include "asts/expr.h"
#include "asts/stmt.h"

class Interpreter : public ExprVisitor, public StmtVisitor
{
public:
	// Visit expression.
	[[nodiscard]] std::any visit_binary_expr(const Binary& expr) const override;
	[[nodiscard]] std::any visit_ternary_expr(const Ternary& expr) const override;
	[[nodiscard]] std::any visit_grouping_expr(const Grouping& expr) const override;
	[[nodiscard]] std::any visit_literal_expr(const Literal& expr) const override;
	[[nodiscard]] std::any visit_unary_expr(const Unary& expr) const override;

	// Visit statement.
	[[nodiscard]] std::any visit_expression_stmt(const Expression& stmt) const override;
	[[nodiscard]] std::any visit_print_stmt(const Print& stmt) const override;

	void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);

private:
	[[nodiscard]] std::any evaluate(const std::shared_ptr<const Expr>& expr) const;
	void execute(const std::shared_ptr<const Stmt>& statement) const;
};

#endif // INTERPRETER_H
