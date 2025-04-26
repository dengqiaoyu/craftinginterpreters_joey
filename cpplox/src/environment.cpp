#include "environment.h"

#include "general.h"
#include "runtime_error.h"

// =====================================================================================================================
// Public methods
// =====================================================================================================================

Environment::Environment(Environment* enclosing) : m_enclosing(enclosing)
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
	if (m_enclosing != nullptr) {
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
	std::any value;
	bool defined = false;
	if (it != m_values.end()) {
		value = it->second;
		defined = true;
	}
	if (m_enclosing != nullptr) {
		value = m_enclosing->get(name);
		defined = true;
	}
	require_throw(defined, RuntimeError(name, std::format("Undefined variable '{}'.", name.get_lexeme())));
	require_throw(value.has_value(),
		RuntimeError(name, std::format("Uninitialized variable '{}'.", name.get_lexeme())));
	return value;
}
