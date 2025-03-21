#include "value.h"
#include "general.h"
#include <stdexcept>
#include <variant>

// Constructors

Value::Value() : m_type(ValueType::NIL) {}
Value::Value(bool value) : m_value(value), m_type(ValueType::BOOL) {}
Value::Value(double value) : m_value(value), m_type(ValueType::NUMBER) {}
Value::Value(const std::string& value) : m_value(value), m_type(ValueType::STRING) {}

// Public methods

[[nodiscard]] bool
Value::is_nil() const
{
	return std::holds_alternative<std::monostate>(m_value);
}

[[nodiscard]] bool
Value::is_bool() const
{
	return std::holds_alternative<bool>(m_value);
}

[[nodiscard]] bool
Value::is_number() const
{
	return std::holds_alternative<double>(m_value);
}

[[nodiscard]] bool
Value::is_string() const
{
	return std::holds_alternative<std::string>(m_value);
}

[[nodiscard]] ValueType
Value::get_type() const
{
	return m_type;
}

[[nodiscard]] bool
Value::as_bool() const
{
	return std::get<bool>(m_value);
}

[[nodiscard]] double
Value::as_number() const
{
	require_throw(is_number(), std::runtime_error("Value is not a number"));
	return std::get<double>(m_value);
}
[[nodiscard]] const std::string&
Value::as_string() const
{

	return std::get<std::string>(m_value);
}

[[nodiscard]] std::string
Value::to_string() const
{
	return std::visit(
		[](const auto& arg) -> std::string {
			// Getting the original template type T of the variant.
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, std::monostate>) {
				return "nil";
			} else if constexpr (std::is_same_v<T, bool>) {
				return arg ? "true" : "false";
			} else if constexpr (std::is_same_v<T, double>) {
				return std::to_string(arg);
			} else if constexpr (std::is_same_v<T, std::string>) {
				return arg;
			} else {
				static_assert(false, "non-exhaustive visitor!");
			}
		},
		m_value);
}

std::ostream&
operator<<(std::ostream& out_s, const Value& value)
{
	out_s << value.to_string();
	return out_s;
}

bool
Value::operator==(const Value& other) const
{
	if (m_type != other.m_type) {
		return false;
	}
	return m_value == other.m_value;
}

bool
Value::operator!=(const Value& other) const
{
	if (m_type != other.m_type) {
		return true;
	}
	return m_value != other.m_value;
}
