#ifndef RPN_PRINTER_H
#define RPN_PRINTER_H

#include <string>

#include "expr.h"

class RpnPrinter : public Visitor<std::string>
{
public:
	[[nodiscard]] std::string convert_string(const Expr<std::string>& expr) const;
	[[nodiscard]] std::string visit_binary_expr(const Binary<std::string>& expr) const override;
	[[nodiscard]] std::string visit_grouping_expr(const Grouping<std::string>& expr) const override;
	[[nodiscard]] std::string visit_literal_expr(const Literal<std::string>& expr) const override;
	[[nodiscard]] std::string visit_unary_expr(const Unary<std::string>& expr) const override;
};

#endif // RPN_PRINTER_H
