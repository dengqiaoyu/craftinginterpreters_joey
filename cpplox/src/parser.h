#ifndef PARSER_H
#define PARSER_H

#include <algorithm>
#include <concepts>
#include <memory>
#include <vector>

#include "asts/expr.h"
#include "asts/stmt.h"
#include "general.h"
#include "token.h"
#include "token_type.h"

class Parser
{
public:
	template <typename T_TokenVector>
		requires std::convertible_to<T_TokenVector, std::vector<Token>>
	explicit Parser(T_TokenVector&& tokens) : m_tokens(std::forward<T_TokenVector>(tokens))
	{
		// Empty constructor.
	}

	std::vector<std::shared_ptr<Stmt>> parse();

private:
	std::vector<Token> m_tokens;
	size_t current = 0;

	[[nodiscard]] const std::vector<Token>& get_tokens() const;

	/*
	 * Expression grammar:
	 *
	 * comma_expression			-> expression ( "," expression )* ;
	 * conditional_expression	-> expression ? expression : conditional_expression:
	 *							| expression
	 * expression				-> assignment ;
	 * assignment				-> IDENTIFIER "=" assignment
	 *							| equality ;
	 * equality					-> comparison ( ( "!=" | "==" ) comparison )* ;
	 * comparison				-> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
	 * term						-> factor ( ( "-" | "+" ) factor )* ;
	 * factor					-> unary ( ( "/" | "*" ) unary )* ;
	 * unary					-> ( "!" | "-" ) unary
	 *							| primary ;
	 * primary					-> NUMBER | STRING | "true" | "false" | "nil"
	 *							| "(" comma_expression ")"
	 *							| IDENTIFIER;
	 */

	std::shared_ptr<Expr> comma_expression();
	std::shared_ptr<Expr> conditional_expression();
	std::shared_ptr<Expr> expression();
	std::shared_ptr<Expr> assignment();
	std::shared_ptr<Expr> equality();
	std::shared_ptr<Expr> comparison();
	std::shared_ptr<Expr> term();
	std::shared_ptr<Expr> factor();
	std::shared_ptr<Expr> unary();
	std::shared_ptr<Expr> primary();

	/*
	 * Statement grammar:
	 *
	 * program					-> declaration* EOF ;
	 * declaration				-> variable_declaration
	 *							|statement ;
	 * variable_declaration		-> "var" IDENTIFIER ( "=" expression )? ";" ;
	 * statement				-> expression_statement
	 *							| print_statement ;
	 * expression_statement		-> expression ";" ;
	 * print_statement			-> "print" expression ";" ;
	 */

	std::shared_ptr<Stmt> declaration();
	std::shared_ptr<Stmt> variable_declaration();
	std::shared_ptr<Stmt> statement();
	std::shared_ptr<Stmt> expression_statement();
	std::shared_ptr<Stmt> print_statement();

	template <typename... VT_TokenType>
		requires((std::is_same_v<VT_TokenType, TokenType>) && ...)
	bool match(VT_TokenType... types)
	{
		std::initializer_list<TokenType> token_types = {types...};
		const auto it = std::ranges::find_if(token_types, [this](const TokenType& type) { return check(type); });
		require_return_value(it != token_types.end(), false);

		advance();
		return true;
	}

	Token advance();
	Token consume(TokenType type, const std::string& message);
	void synchronize();

	[[nodiscard]] bool check(TokenType type) const;
	[[nodiscard]] bool is_at_end() const;
	[[nodiscard]] const Token& peek() const;
	[[nodiscard]] const Token& previous() const;
};

#endif // PARSER_H
