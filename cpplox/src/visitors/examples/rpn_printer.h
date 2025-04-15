#ifndef RPN_PRINTER_H
#define RPN_PRINTER_H

#include <any>
#include <string>

#include "asts/expr.h"

class RpnPrinter : public ExprVisitor
{
public:
	[[nodiscard]] std::string convert_string(const Expr& expr);
	[[nodiscard]] std::any visit_binary_expr(const Binary& expr) override;
	[[nodiscard]] std::any visit_ternary_expr(const Ternary& expr) override;
	[[nodiscard]] std::any visit_grouping_expr(const Grouping& expr) override;
	[[nodiscard]] std::any visit_literal_expr(const Literal& expr) override;
	[[nodiscard]] std::any visit_variable_expr(const Variable& expr) override;
	[[nodiscard]] std::any visit_unary_expr(const Unary& expr) override;
};

#endif // RPN_PRINTER_H
