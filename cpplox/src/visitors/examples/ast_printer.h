#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include <any>
#include <format>
#include <string>

#include "asts/expr.h"

class AstPrinter : public ExprVisitor
{
public:
	[[nodiscard]] std::string convert_string(const Expr& expr);
	[[nodiscard]] std::any visit_assign_expr(const Assign& expr) override;
	[[nodiscard]] std::any visit_binary_expr(const Binary& expr) override;
	[[nodiscard]] std::any visit_ternary_expr(const Ternary& expr) override;
	[[nodiscard]] std::any visit_grouping_expr(const Grouping& expr) override;
	[[nodiscard]] std::any visit_literal_expr(const Literal& expr) override;
	[[nodiscard]] std::any visit_variable_expr(const Variable& expr) override;
	[[nodiscard]] std::any visit_unary_expr(const Unary& expr) override;

private:
	template <typename... Args>
		requires(std::is_same_v<std::decay_t<Args>, std::shared_ptr<const Expr>> && ...)
	[[nodiscard]] std::string parenthesize(const std::string& name, const Args&... exprs)
	{
		std::string result = "(" + name;
		(..., (result += std::format(" {}", std::any_cast<std::string>(exprs->accept(*this)))));
		result += ")";
		return result;
	}

	template <typename... Args>
		requires(std::is_same_v<std::decay_t<Args>, std::shared_ptr<const Expr>> && ...)
	[[nodiscard]] std::string parenthesize(const std::string& name, const Token& token, const Args&... exprs)
	{
		std::string result = "(" + name + " " + token.get_lexeme();
		(..., (result += std::format(" {}", std::any_cast<std::string>(exprs->accept(*this)))));
		result += ")";
		return result;
	}
};

#endif // AST_PRINTER_H
