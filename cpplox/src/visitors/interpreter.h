#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <any>
#include <memory>

#include "asts/expr.h"
#include "asts/stmt.h"
#include "environment.h"
#include "general.h"

class Interpreter : public ExprVisitor, public StmtVisitor
{
public:
	Interpreter();

	// Visit expression.
	[[nodiscard]] std::any visit_assign_expr(const Assign& expr) override;
	[[nodiscard]] std::any visit_binary_expr(const Binary& expr) override;
	[[nodiscard]] std::any visit_ternary_expr(const Ternary& expr) override;
	[[nodiscard]] std::any visit_grouping_expr(const Grouping& expr) override;
	[[nodiscard]] std::any visit_literal_expr(const Literal& expr) override;
	[[nodiscard]] std::any visit_variable_expr(const Variable& expr) override;
	[[nodiscard]] std::any visit_unary_expr(const Unary& expr) override;

	// Visit statement.
	[[nodiscard]] std::any visit_var_stmt(const Var& stmt) override;
	[[nodiscard]] std::any visit_expression_stmt(const Expression& stmt) override;
	[[nodiscard]] std::any visit_expressionresult_stmt(const ExpressionResult& stmt) override;
	[[nodiscard]] std::any visit_print_stmt(const Print& stmt) override;
	[[nodiscard]] std::any visit_block_stmt(const Block& stmt) override;

	void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);
	void print_expression(const std::shared_ptr<const Expr>& expr);

	// Reset the state of last expression (used when starting a new parsing run)
	void reset_last_expression_state();
	std::any get_last_expression_result(bool& out_last_expression_evaluated);
	[[nodiscard]] static std::string stringify(const std::any& any, bool is_print_statement = false);

private:
	std::unique_ptr<Environment> m_environment;
	std::any m_last_expression_result;
	bool m_last_expression_evaluated = false;
	CLASS_PADDING(7);

	[[nodiscard]] std::any evaluate(const std::shared_ptr<const Expr>& expr);
	void execute(const std::shared_ptr<const Stmt>& statement);
	void execute_block(const std::vector<std::shared_ptr<const Stmt>>& statements,
		std::unique_ptr<Environment> block_environment);
};

#endif // INTERPRETER_H
