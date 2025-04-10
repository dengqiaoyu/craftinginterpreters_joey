#ifndef RUNTIME_ERROR_H
#define RUNTIME_ERROR_H

#include <stdexcept>
#include <string>

#include "token.h"

class RuntimeError : public std::runtime_error
{
public:
	template <typename T_Token>
		requires std::convertible_to<T_Token, Token>
	RuntimeError(T_Token&& Token, const std::string& message)
		: std::runtime_error(message), m_message(message), m_token(std::forward<T_Token>(Token))
	{
		// Empty constructor.
	}

	~RuntimeError() override;
	RuntimeError(const RuntimeError&) = delete;
	RuntimeError& operator=(const RuntimeError&) = delete;
	RuntimeError(RuntimeError&&) = delete;
	RuntimeError& operator=(RuntimeError&&) = delete;

	[[nodiscard]] const std::string& get_message() const;
	[[nodiscard]] const Token& get_token() const;

private:
	std::string m_message;
	Token m_token;
};

#endif
