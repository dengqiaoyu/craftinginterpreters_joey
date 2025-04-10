#include "value.h"
#include "general.h"
#include <stdexcept>
#include <variant>

namespace {

std::string
value_type_to_string(ValueType type)
{
	ignore_warning_begin("-Wswitch-default");
	switch (type) {
	case ValueType::BOOL: return "bool";
	case ValueType::NIL: return "nil";
	case ValueType::NUMBER: return "number";
	case ValueType::STRING: return "string";
	}
	ignore_warning_end();
}

void
remove_trailing_zeros(std::string& str)
{
	const size_t dot_pos = str.find('.');
	if (dot_pos != std::string::npos) {
		size_t end = str.size() - 1;
		while (str[end] == '0') {
			--end;
		}
		if (str[end] == '.') {
			--end;
		}
		str = str.substr(0, end + 1);
	}
}

} // namespace

// Constructors

Value::Value() : m_type(ValueType::NIL) {}
Value::Value(bool value) : m_value(value), m_type(ValueType::BOOL) {}
Value::Value(int value) : m_value(value), m_type(ValueType::NUMBER) {}
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
			} else if constexpr (std::is_same_v<T, int>) {
				return std::to_string(arg);
			} else if constexpr (std::is_same_v<T, double>) {
				std::string str = std::to_string(arg);
				remove_trailing_zeros(str);
				return str;
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

std::partial_ordering
Value::operator<=>(const Value& other) const
{
	if (m_type != other.m_type) {
		return std::partial_ordering::unordered;
	}
	if (m_value == other.m_value) {
		return std::partial_ordering::equivalent;
	}
	return m_value <=> other.m_value;
}

Value
Value::operator+(const Value& other) const
{
	if ((is_number() || is_string()) && (other.is_number() || other.is_string())) {
		if (is_number() && other.is_number()) {
			return Value(as_number() + other.as_number());
		}
		return Value(to_string() + other.to_string());
	}
	throw std::runtime_error(
		"Invalid operation: cannot add " + value_type_to_string(m_type) + " and " + value_type_to_string(other.m_type));
}
