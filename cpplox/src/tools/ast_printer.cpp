#include <concepts>
#include <format>
#include <iostream>

#include "expr.h"

template <typename T>
concept ExprStringType = std::derived_from<T, Expr<std::string>>;

class AstPrinter : public Visitor<std::string>
{
public:
	[[nodiscard]] std::string print(const Expr<std::string>& expr) const
	{
		return expr.accept(*this);
	}
	std::string visit_binary_expr(const Binary<std::string>& expr) override;
	std::string visit_grouping_expr(const Grouping<std::string>& expr) override;
	std::string visit_literal_expr(const Literal<std::string>& expr) override;
	std::string visit_unary_expr(const Unary<std::string>& expr) override;

private:
	template <ExprStringType... Args>
	std::string parenthesize(const std::string& name, const Args&... exprs)
	{
		std::string result = "(" + name;
		(..., (result += std::format(" {}", exprs.accept(*this))));
		result += ")";
		return result;
	}
};

std::string
AstPrinter::visit_binary_expr(const Binary<std::string>& expr)
{
	return parenthesize(expr.get_opr().get_lexeme(), expr.get_left(), expr.get_right());
}

std::string
AstPrinter::visit_grouping_expr(const Grouping<std::string>& expr)
{
	return parenthesize("group", expr.get_expr());
}

std::string
AstPrinter::visit_literal_expr(const Literal<std::string>& expr)
{
	return std::format("{}", expr.get_value());
}

std::string
AstPrinter::visit_unary_expr(const Unary<std::string>& expr)
{
	return parenthesize(expr.get_opr().get_lexeme(), expr.get_right());
}

int
main(const int argc, const char* const argv[])
{
	(void)argc;
	(void)argv;
	std::cout << "Hello, World!" << std::endl;

	return 0;
}
