#include "environment.h"

#include "runtime_error.h"

// =====================================================================================================================
// Public methods
// =====================================================================================================================

Environment::Environment() = default;

// =====================================================================================================================

void
Environment::define(const Token& name, const std::any& value)
{
	m_values[name.get_lexeme()] = value;
}

// =====================================================================================================================

std::any
Environment::operator[](const Token& name)
{
	auto it = m_values.find(name.get_lexeme());
	if (it != m_values.end()) {
		return it->second;
	}
	throw RuntimeError(name, std::format("Undefined variable '{}'.", name.get_lexeme()));
}

// =====================================================================================================================

const std::any&
Environment::operator[](const Token& name) const
{
	auto it = m_values.find(name.get_lexeme());
	if (it != m_values.end()) {
		return it->second;
	}
	throw RuntimeError(name, std::format("Undefined variable '{}'.", name.get_lexeme()));
}
