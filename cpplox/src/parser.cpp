#include "parser.h"

#include <memory>
#include <stdexcept>

#include "asts/expr.h"
#include "asts/stmt.h"
#include "general.h"
#include "lox.h"
#include "token.h"
#include "token_type.h"

namespace {

class ParserError : public std::runtime_error // NOLINT(hicpp-special-member-functions,
											  // cppcoreguidelines-special-member-functions)
{
public:
	explicit ParserError(const std::string& message) : std::runtime_error(message)
	{
		// Empty constructor.
	}
	~ParserError() override;
};

ParserError
error(const Token& token, const std::string& message)
{
	Lox::error(token, message);
	return ParserError(message);
}

} // namespace

// =====================================================================================================================
// Public methods.

std::vector<std::shared_ptr<Stmt>>
Parser::parse()
{
	std::vector<std::shared_ptr<Stmt>> statements;

	while (!is_at_end()) {
		statements.push_back(statement());
	}

	return statements;
}

// =====================================================================================================================
// Private methods.

// =====================================================================================================================
// Expression grammar.

// <comma_expression> -> <expression> ( "," <expression> )*
std::shared_ptr<Expr>
Parser::comma_expression() // NOLINT(misc-no-recursion)
{
	std::shared_ptr<Expr> expr = conditional_expression();
	while (match(TokenType::COMMA)) {
		const Token& comma_opr = previous();
		std::shared_ptr<Expr> right = conditional_expression();
		expr = std::make_shared<Binary>(expr, comma_opr, right);
	}
	return expr;
}

// =====================================================================================================================

// conditional_expression	-> expression ? expression : conditional_expression:
// 							| expression
std::shared_ptr<Expr>
Parser::conditional_expression() // NOLINT(misc-no-recursion)
{
	std::shared_ptr<Expr> expr = expression();
	if (match(TokenType::QUESTION)) {
		const Token& qmark = previous();
		std::shared_ptr<Expr> then_branch = expression();
		consume(TokenType::COLON, "Expect ':' after expression.");
		std::shared_ptr<Expr> else_branch = conditional_expression();
		expr = std::make_shared<Ternary>(expr, qmark, then_branch, previous(), else_branch);
	}
	return expr;
}

// =====================================================================================================================

// <expression> -> <equality>
std::shared_ptr<Expr>
Parser::expression() // NOLINT(misc-no-recursion)
{
	return equality();
}

// =====================================================================================================================

// <equality> -> <comparison> ( ( "!=" | "==" ) <comparison> )*
std::shared_ptr<Expr>
Parser::equality() // NOLINT(misc-no-recursion)
{
	// Error handling for equality that does not have a left operand.
	if (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
		const Token& eq_opr = previous();
		error(eq_opr, "Expect left operand before equality operator.");
		// Parse the right operand and continue parsing.
		std::shared_ptr<Expr> right = comparison();
		return right;
	}

	// Think about `a == b == c == d == e`.
	std::shared_ptr<Expr> expr = comparison();
	while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
		const Token& eq_opr = previous();
		std::shared_ptr<Expr> right = comparison();
		expr = std::make_shared<Binary>(expr, eq_opr, right);
	}
	return expr;
}

// =====================================================================================================================

// <comparison> -> <term> ( ( ">" | ">=" | "<" | "<=" ) <term> )*
std::shared_ptr<Expr>
Parser::comparison() // NOLINT(misc-no-recursion)
{

	// Error handling for comparison that does not have a left operand.
	if (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
		const Token& cmp_opr = previous();
		error(cmp_opr, "Expect left operand before comparison operator.");
		// Parse the right operand and continue parsing.
		std::shared_ptr<Expr> right = term();
		return right;
	}

	std::shared_ptr<Expr> expr = term();
	while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
		const Token& cmp_opr = previous();
		std::shared_ptr<Expr> right = term();
		expr = std::make_shared<Binary>(expr, cmp_opr, right);
	}
	return expr;
}

// =====================================================================================================================

// <term> -> <factor> ( ( "-" | "+" ) <factor> )*
std::shared_ptr<Expr>
Parser::term() // NOLINT(misc-no-recursion)
{
	// Error handling for term that does not have a left operand.
	// Note that `-` is a unary operator, so we don't need to check for it here.
	if (match(TokenType::PLUS)) {
		const Token& add_opr = previous();
		error(add_opr, "Expect left operand before term operator.");
		// Parse the right operand and continue parsing.
		std::shared_ptr<Expr> right = factor();
		return right;
	}

	std::shared_ptr<Expr> expr = factor();
	while (match(TokenType::MINUS, TokenType::PLUS)) {
		const Token& add_opr = previous();
		std::shared_ptr<Expr> right = factor();
		expr = std::make_shared<Binary>(expr, add_opr, right);
	}
	return expr;
}

// =====================================================================================================================

// <factor> -> <unary> ( ( "/" | "*" ) <unary> )*
std::shared_ptr<Expr>
Parser::factor() // NOLINT(misc-no-recursion)
{
	// Error handling for factor that does not have a left operand.
	if (match(TokenType::SLASH, TokenType::STAR)) {
		const Token& mul_opr = previous();
		error(mul_opr, "Expect left operand before factor operator.");
		// Parse the right operand and continue parsing.
		std::shared_ptr<Expr> right = unary();
		return right;
	}

	std::shared_ptr<Expr> expr = unary();
	while (match(TokenType::SLASH, TokenType::STAR)) {
		const Token& mul_opr = previous();
		std::shared_ptr<Expr> right = unary();
		expr = std::make_shared<Binary>(expr, mul_opr, right);
	}
	return expr;
}

// =====================================================================================================================

// <unary> -> ( "!" | "-" ) <unary> | <primary>
std::shared_ptr<Expr>
Parser::unary() // NOLINT(misc-no-recursion)
{
	if (match(TokenType::BANG, TokenType::MINUS)) {
		const Token& unary_opr = previous();
		std::shared_ptr<Expr> right = unary();
		return std::make_shared<Unary>(unary_opr, right);
	}
	return primary();
}

// =====================================================================================================================

// <primary> -> NUMBER | STRING | "true" | "false" | "nil" | "(" <comma_expression> ")"
std::shared_ptr<Expr>
Parser::primary() // NOLINT(misc-no-recursion)
{
	if (match(TokenType::FALSE)) {
		return std::make_shared<Literal>(Value(false));
	}
	if (match(TokenType::TRUE)) {
		return std::make_shared<Literal>(Value(true));
	}
	if (match(TokenType::NIL)) {
		return std::make_shared<Literal>(Value());
	}
	if (match(TokenType::NUMBER, TokenType::STRING)) {
		return std::make_shared<Literal>(previous().get_literal());
	}
	if (match(TokenType::LEFT_PAREN)) {
		std::shared_ptr<Expr> comma_expr = comma_expression();
		consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
		return std::make_shared<Grouping>(comma_expr);
	}

	throw error(peek(), "Expect expression.");
}

// =====================================================================================================================
// Statement grammar.

// =====================================================================================================================
// statement	-> <expr_stmt> | <print_stmt>

std::shared_ptr<Stmt>
Parser::statement()
{
	if (match(TokenType::PRINT)) {
		return print_statement();
	}
	return expression_statement();
}

// =====================================================================================================================
// expression_statement -> <expression> ";"
std::shared_ptr<Stmt>
Parser::expression_statement()
{
	std::shared_ptr<Expr> expr = expression();
	consume(TokenType::SEMICOLON, "Expect ';' after expression.");
	return std::make_shared<Expression>(expr);
}

// =====================================================================================================================
// print_statement -> "print" <expression> ";"
std::shared_ptr<Stmt>
Parser::print_statement()
{
	std::shared_ptr<Expr> expr = expression();
	consume(TokenType::SEMICOLON, "Expect ';' after expression.");
	return std::make_shared<Print>(expr);
}

// =====================================================================================================================
// Helper methods.

// =====================================================================================================================

ParserError::~ParserError() = default;

// =====================================================================================================================

const std::vector<Token>&
Parser::get_tokens() const
{
	return m_tokens;
}

// =====================================================================================================================

Token
Parser::advance()
{
	if (!is_at_end()) {
		current++;
	}
	return previous();
}

// =====================================================================================================================

Token
Parser::consume(const TokenType type, const std::string& message)
{
	const bool consumed = check(type);
	require_throw(consumed, error(peek(), message));
	return advance();
}

// =====================================================================================================================

void
Parser::synchronize()
{
	advance();
	while (!is_at_end()) {
		if (previous().get_type() == TokenType::SEMICOLON) {
			return;
		}
		ignore_warning_begin("-Wswitch-enum");
		switch (peek().get_type()) {
		case TokenType::CLASS:
		case TokenType::FOR:
		case TokenType::FUN:
		case TokenType::IF:
		case TokenType::PRINT:
		case TokenType::RETURN:
		case TokenType::VAR:
		case TokenType::WHILE: return;
		default: advance();
		}
		ignore_warning_end();
	}
}

// =====================================================================================================================

bool
Parser::check(const TokenType type) const
{
	require_return_value(!is_at_end(), false);
	const Token& token = peek();
	return token.get_type() == type;
}

// =====================================================================================================================

bool
Parser::is_at_end() const
{
	const Token& token = peek();
	return token.get_type() == TokenType::END_OF_FILE;
}

// =====================================================================================================================

const Token&
Parser::peek() const
{
	return get_tokens().at(current);
}

// =====================================================================================================================

const Token&
Parser::previous() const
{
	return get_tokens().at(current - 1);
}

// =====================================================================================================================
