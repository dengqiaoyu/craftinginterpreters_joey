#ifndef PARSER_H
#define PARSER_H

#include <algorithm>
#include <concepts>
#include <memory>
#include <vector>

#include "expr.h"
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

	std::shared_ptr<Expr> parse();

private:
	std::vector<Token> m_tokens;
	size_t current = 0;

	[[nodiscard]] const std::vector<Token>& get_tokens() const;

	/*
	 * Grammar:
	 *
	 * comma_expression			-> expression ( "," expression )* ;
	 * conditional_expression	-> expression ? expression : conditional_expression:
	 *							| expression
	 * expression				-> equality ;
	 * equality					-> comparison ( ( "!=" | "==" ) comparison )* ;
	 * comparison				-> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
	 * term						-> factor ( ( "-" | "+" ) factor )* ;
	 * factor					-> unary ( ( "/" | "*" ) unary )* ;
	 * unary					-> ( "!" | "-" ) unary
	 *							| primary ;
	 * primary					-> NUMBER | STRING | "true" | "false" | "nil"
	 *							| "(" comma_expression ")" ;
	 */

	std::shared_ptr<Expr> comma_expression();
	std::shared_ptr<Expr> conditional_expression();
	std::shared_ptr<Expr> expression();
	std::shared_ptr<Expr> equality();
	std::shared_ptr<Expr> comparison();
	std::shared_ptr<Expr> term();
	std::shared_ptr<Expr> factor();
	std::shared_ptr<Expr> unary();
	std::shared_ptr<Expr> primary();

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
