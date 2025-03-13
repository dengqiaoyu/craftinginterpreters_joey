#ifndef GENERAL_H
#define GENERAL_H

#include <array> // IWYU pragma: keep
#include <cstdarg>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

// NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
#define EMPTY_DO_WHILE()                                                                                               \
	do {                                                                                                               \
	} while (false)
// NOLINTEND(cppcoreguidelines-avoid-do-while)

#define PRAGMA_WITH_STRINGIZED_ARGUMENT(ARG) _Pragma(#ARG)

#define IGNORE_WARNING_BEGIN(WARNING)                                                                                  \
	_Pragma("clang diagnostic push") PRAGMA_WITH_STRINGIZED_ARGUMENT(clang diagnostic ignored #WARNING)

#define IGNORE_WARNING_ADD(WARNING) PRAGMA_WITH_STRINGIZED_ARGUMENT(clang diagnostic ignored #WARNING)

#define IGNORE_WARNING_END() _Pragma("clang diagnostic pop")

// clang-format off
#define ignore_warning_begin(WARNING)                                                                                  \
	IGNORE_WARNING_BEGIN(WARNING)                                                                                      \
	EMPTY_DO_WHILE()

#define ignore_warning_add(WARNING)                                                                                     \
	IGNORE_WARNING_ADD(WARNING)                                                                                         \
	EMPTY_DO_WHILE()

#define ignore_warning_end()                                                                                           \
	IGNORE_WARNING_END()                                                                                               \
	EMPTY_DO_WHILE()

#define CLASS_PADDING(BYTES)                                                                                           \
	IGNORE_WARNING_BEGIN(-Wzero-length-array)                                                                          \
	IGNORE_WARNING_ADD(-Wunused-private-field)                                                                         \
	std::array<char, (BYTES)> _unused_padding = {}                                                                     \
	IGNORE_WARNING_END()
// clang-format on

#ifndef require_action_return
	// NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
	#define require_action_return(condition, action)                                                                   \
		do {                                                                                                           \
			if (!(condition)) {                                                                                        \
				action;                                                                                                \
				return;                                                                                                \
			}                                                                                                          \
		} while (0)
	// NOLINTEND(cppcoreguidelines-avoid-do-while)
#endif

#ifndef require_action_return_value
	// NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
	#define require_action_return_value(condition, action, ret)                                                        \
		do {                                                                                                           \
			if (!(condition)) {                                                                                        \
				action;                                                                                                \
				return ret;                                                                                            \
			}                                                                                                          \
		} while (0)
	// NOLINTEND(cppcoreguidelines-avoid-do-while)
#endif

#ifndef require_throw
	// NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
	#define require_throw(condition, exception)                                                                        \
		do {                                                                                                           \
			if (!(condition)) {                                                                                        \
				throw exception;                                                                                       \
			}                                                                                                          \
		} while (0)
	// NOLINTEND(cppcoreguidelines-avoid-do-while)
#endif

// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)

template <typename... Args>
inline std::string fmt_str(const char* fmt, Args&&... args)
{
	// clang-format off
	ignore_warning_begin(-Wformat-nonliteral);
	ignore_warning_add(-Wformat-security);
	// clang-format on

	const int size = std::snprintf(nullptr, 0, fmt, std::forward<Args>(args)...);
	require_throw(size >= 0, std::runtime_error("Error during formatting -- format: " + std::string(fmt)));

	std::vector<char> buf(static_cast<size_t>(size) + 1);
	(void)std::snprintf(buf.data(), buf.size(), fmt, std::forward<Args>(args)...);
	ignore_warning_end();

	return {buf.data(), static_cast<size_t>(size)};
}

// NOLINTEND(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)

#endif // GENERAL_H
