#include "environment.h"

#include "runtime_error.h"

// =====================================================================================================================
// Public methods
// =====================================================================================================================

Environment::Environment(std::unique_ptr<Environment> enclosing) : m_enclosing(std::move(enclosing))
{
	// Left blank intentionally.
}

// =====================================================================================================================

void
Environment::define(const Token& name, const std::any& value)
{
	m_values[name.get_lexeme()] = value;
}

// =====================================================================================================================

void
Environment::assign(const Token& name, const std::any& value) // NOLINT(misc-no-recursion)
{
	auto it = m_values.find(name.get_lexeme());
	if (it != m_values.end()) {
		it->second = value;
		return;
	}
	if (m_enclosing) {
		m_enclosing->assign(name, value);
		return;
	}
	throw RuntimeError(name, std::format("Undefined variable '{}'.", name.get_lexeme()));
}

// =====================================================================================================================

std::any
Environment::get(const Token& name) // NOLINT(misc-no-recursion)
{
	auto it = m_values.find(name.get_lexeme());
	if (it != m_values.end()) {
		return it->second;
	}
	if (m_enclosing) {
		return m_enclosing->get(name);
	}
	throw RuntimeError(name, std::format("Undefined variable '{}'.", name.get_lexeme()));
}
