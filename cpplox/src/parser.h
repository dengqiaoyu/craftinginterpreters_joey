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

	template <typename T_Generic>
	std::shared_ptr<Expr<T_Generic>> parse()
	{
		try {
			return expression<T_Generic>();
		} catch (UNUSED const ParserError& error) {
			return nullptr;
		}
	}

private:
	std::vector<Token> m_tokens;
	size_t current = 0;

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

	[[nodiscard]] const std::vector<Token>& get_tokens() const;

	// <expression> -> <equality>
	template <typename T_Generic>
	std::shared_ptr<Expr<T_Generic>> expression()
	{
		return equality<T_Generic>();
	}

	// <equality> -> <comparison> ( ( "!=" | "==" ) <comparison> )*
	template <typename T_Generic>
	[[nodiscard]] std::shared_ptr<Expr<T_Generic>> equality()
	{
		// Think about `a == b == c == d == e`.
		std::shared_ptr<Expr<T_Generic>> expr = comparison<T_Generic>();
		while (match(TokenType::BANG_EQUAL, TokenType::EQUAL)) {
			const Token& eq_opr = previous();
			std::shared_ptr<Expr<T_Generic>> right = std::make_shared<comparison<T_Generic>>();
			expr = std::make_shared<Binary<T_Generic>>(expr, eq_opr, right);
		}
		return expr;
	}

	// <comparison> -> <term> ( ( ">" | ">=" | "<" | "<=" ) <term> )*
	template <typename T_Generic>
	[[nodiscard]] std::shared_ptr<Expr<T_Generic>> comparison()
	{
		std::shared_ptr<Expr<T_Generic>> expr = term<T_Generic>();
		while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
			const Token& cmp_opr = previous();
			std::shared_ptr<Expr<T_Generic>> right = std::make_shared<term<T_Generic>>();
			expr = std::make_shared<Binary<T_Generic>>(expr, cmp_opr, right);
		}
		return expr;
	}

	// <term> -> <factor> ( ( "-" | "+" ) <factor> )*
	template <typename T_Generic>
	[[nodiscard]] std::shared_ptr<Expr<T_Generic>> term()
	{
		std::shared_ptr<Expr<T_Generic>> expr = factor<T_Generic>();
		while (match(TokenType::MINUS, TokenType::PLUS)) {
			const Token& add_opr = previous();
			std::shared_ptr<Expr<T_Generic>> right = std::make_shared<factor<T_Generic>>();
			expr = std::make_shared<Binary<T_Generic>>(expr, add_opr, right);
		}
		return expr;
	}

	// <factor> -> <unary> ( ( "/" | "*" ) <unary> )*
	template <typename T_Generic>
	[[nodiscard]] std::shared_ptr<Expr<T_Generic>> factor()
	{
		std::shared_ptr<Expr<T_Generic>> expr = unary<T_Generic>();
		while (match(TokenType::SLASH, TokenType::STAR)) {
			const Token& mul_opr = previous();
			std::shared_ptr<Expr<T_Generic>> right = std::make_shared<unary<T_Generic>>();
			expr = std::make_shared<Binary<T_Generic>>(expr, mul_opr, right);
		}
		return expr;
	}

	// <unary> -> ( "!" | "-" ) <unary> | <primary>
	template <typename T_Generic>
	[[nodiscard]] std::shared_ptr<Expr<T_Generic>> unary()
	{
		if (match(TokenType::BANG, TokenType::MINUS)) {
			const Token& unary_opr = previous();
			std::shared_ptr<Expr<T_Generic>> right = unary<T_Generic>();
			return std::make_shared<Unary<T_Generic>>(unary_opr, right);
		}
		return primary<T_Generic>();
	}

	// <primary> -> NUMBER | STRING | "true" | "false" | "nil" | "(" <expression> ")"
	template <typename T_Generic>
	[[nodiscard]] std::shared_ptr<Expr<T_Generic>> primary()
	{
		if (match(TokenType::FALSE)) {
			return std::make_shared<Literal<T_Generic>>(false);
		}
		if (match(TokenType::TRUE)) {
			return std::make_shared<Literal<T_Generic>>(true);
		}
		if (match(TokenType::NIL)) {
			return std::make_shared<Literal<T_Generic>>(nullptr);
		}
		if (match(TokenType::NUMBER, TokenType::STRING)) {
			return std::make_shared<Literal<T_Generic>>(std::move(previous().get_literal()));
		}
		if (match(TokenType::LEFT_PAREN)) {
			std::shared_ptr<Expr<T_Generic>> expr = expression<T_Generic>();
			consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
			return std::make_shared<Grouping<T_Generic>>(expr);
		}

		throw error(peek(), "Expect expression.");
	}

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

	static ParserError error(const Token& token, const std::string& message);
};

#endif // PARSER_H
