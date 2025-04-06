#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include <any>
#include <format>
#include <string>

#include "expr.h"

class AstPrinter : public Visitor
{
public:
	[[nodiscard]] std::string convert_string(const Expr& expr) const;
	[[nodiscard]] std::any visit_binary_expr(const Binary& expr) const override;
	[[nodiscard]] std::any visit_ternary_expr(const Ternary& expr) const override;
	[[nodiscard]] std::any visit_grouping_expr(const Grouping& expr) const override;
	[[nodiscard]] std::any visit_literal_expr(const Literal& expr) const override;
	[[nodiscard]] std::any visit_unary_expr(const Unary& expr) const override;

private:
	template <typename... Args>
		requires(std::is_same_v<std::decay_t<Args>, std::shared_ptr<const Expr>> && ...)
	[[nodiscard]] std::string parenthesize(const std::string& name, const Args&... exprs) const
	{
		std::string result = "(" + name;
		(..., (result += std::format(" {}", std::any_cast<std::string>(exprs->accept(*this)))));
		result += ")";
		return result;
	}
};

#endif // AST_PRINTER_H
