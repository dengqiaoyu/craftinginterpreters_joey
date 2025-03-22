#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include <concepts>
#include <format>
#include <string>

#include "expr.h"

template <typename T>
concept AstPrinterExprStringType = std::derived_from<T, Expr<std::string>>;

class AstPrinter : public Visitor<std::string>
{
public:
	[[nodiscard]] std::string convert_string(const Expr<std::string>& expr) const;
	[[nodiscard]] std::string visit_binary_expr(const Binary<std::string>& expr) const override;
	[[nodiscard]] std::string visit_grouping_expr(const Grouping<std::string>& expr) const override;
	[[nodiscard]] std::string visit_literal_expr(const Literal<std::string>& expr) const override;
	[[nodiscard]] std::string visit_unary_expr(const Unary<std::string>& expr) const override;

private:
	template <AstPrinterExprStringType... Args>
	[[nodiscard]] std::string parenthesize(const std::string& name, const Args&... exprs) const
	{
		std::string result = "(" + name;
		(..., (result += std::format(" {}", exprs.accept(*this))));
		result += ")";
		return result;
	}
};

#endif // AST_PRINTER_H
