#ifndef RPN_PRINTER_H
#define RPN_PRINTER_H

#include <any>
#include <string>

#include "expr.h"

class RpnPrinter : public Visitor
{
public:
	[[nodiscard]] std::string convert_string(const Expr& expr) const;
	[[nodiscard]] std::any visit_binary_expr(const Binary& expr) const override;
	[[nodiscard]] std::any visit_ternary_expr(const Ternary& expr) const override;
	[[nodiscard]] std::any visit_grouping_expr(const Grouping& expr) const override;
	[[nodiscard]] std::any visit_literal_expr(const Literal& expr) const override;
	[[nodiscard]] std::any visit_unary_expr(const Unary& expr) const override;
};

#endif // RPN_PRINTER_H
