#ifndef VALUE_H
#define VALUE_H

#include <format>
#include <string>
#include <variant>

#include "general.h"

enum class ValueType
{
	BOOL,
	NIL,
	NUMBER,
	STRING,
};

[[nodiscard]] inline std::string
to_string(ValueType type)
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

class Value
{
public:
	explicit Value();
	explicit Value(bool value);
	explicit Value(int value);
	explicit Value(double value);
	explicit Value(const std::string& value);

	[[nodiscard]] ValueType get_type() const;

	[[nodiscard]] bool is_nil() const;
	[[nodiscard]] bool is_bool() const;
	[[nodiscard]] bool is_number() const;
	[[nodiscard]] bool is_string() const;

	[[nodiscard]] bool as_bool() const;
	[[nodiscard]] double as_number() const;
	[[nodiscard]] const std::string& as_string() const;

	[[nodiscard]] std::string to_string() const;
	friend std::ostream& operator<<(std::ostream& out_s, const Value& value);

	bool operator==(const Value& other) const;
	bool operator!=(const Value& other) const;

	// std::variant automatically handles the copy/move constructors and assignment operators.
	// The destructor is also automatically generated.

private:
	std::variant<std::monostate, bool, int, double, std::string> m_value;
	ValueType m_type;
	CLASS_PADDING(4);
};

template <>
struct std::formatter<Value> : std::formatter<std::string> {
	auto format(const Value& value, format_context& ctx) const
	{
		return std::formatter<std::string>::format(value.to_string(), ctx);
	}
};

#endif // VALUE_H
