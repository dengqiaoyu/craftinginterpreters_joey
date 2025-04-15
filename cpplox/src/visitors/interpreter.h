#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <any>

#include "asts/expr.h"
#include "asts/stmt.h"
#include "environment.h"

class Interpreter : public ExprVisitor, public StmtVisitor
{
public:
	Interpreter();

	// Visit expression.
	[[nodiscard]] std::any visit_binary_expr(const Binary& expr) override;
	[[nodiscard]] std::any visit_ternary_expr(const Ternary& expr) override;
	[[nodiscard]] std::any visit_grouping_expr(const Grouping& expr) override;
	[[nodiscard]] std::any visit_literal_expr(const Literal& expr) override;
	[[nodiscard]] std::any visit_variable_expr(const Variable& expr) override;
	[[nodiscard]] std::any visit_unary_expr(const Unary& expr) override;

	// Visit statement.
	[[nodiscard]] std::any visit_var_stmt(const Var& stmt) override;
	[[nodiscard]] std::any visit_expression_stmt(const Expression& stmt) override;
	[[nodiscard]] std::any visit_print_stmt(const Print& stmt) override;

	void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);

private:
	Environment m_environment;

	[[nodiscard]] std::any evaluate(const std::shared_ptr<const Expr>& expr);
	void execute(const std::shared_ptr<const Stmt>& statement);
};

#endif // INTERPRETER_H
