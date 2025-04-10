#include "runtime_error.h"

// =====================================================================================================================

RuntimeError::~RuntimeError() = default;

const std::string&
RuntimeError::get_message() const
{
	return m_message;
}

const Token&
RuntimeError::get_token() const
{
	return m_token;
}
