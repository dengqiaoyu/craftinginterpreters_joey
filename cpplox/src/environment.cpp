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

void
Environment::assign(const Token& name, const std::any& value)
{
	auto it = m_values.find(name.get_lexeme());
	if (it != m_values.end()) {
		it->second = value;
		return;
	}
	throw RuntimeError(name, std::format("Undefined variable '{}'.", name.get_lexeme()));
}

// =====================================================================================================================

std::any
Environment::get(const Token& name)
{
	auto it = m_values.find(name.get_lexeme());
	if (it != m_values.end()) {
		return it->second;
	}
	throw RuntimeError(name, std::format("Undefined variable '{}'.", name.get_lexeme()));
}

// =====================================================================================================================

const std::any&
Environment::get(const Token& name) const
{
	auto it = m_values.find(name.get_lexeme());
	if (it != m_values.end()) {
		return it->second;
	}
	throw RuntimeError(name, std::format("Undefined variable '{}'.", name.get_lexeme()));
}

// =====================================================================================================================

std::any
Environment::operator[](const Token& name)
{
	return this->get(name);
}

// =====================================================================================================================

const std::any&
Environment::operator[](const Token& name) const
{
	return this->get(name);
}
