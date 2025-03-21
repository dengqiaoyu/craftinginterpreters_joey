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

// =====================================================================================================================
// Ignore Warning Macros:
// The following macros are used to ignore specific warnings in the code.

// _Pragma can only accept a single string literal argument, so we need to use this macro to concatenate the argument.
#define DO_PRAGMA(x) _Pragma(#x)

#define PUSH_DIAGNOSTIC()					   DO_PRAGMA(clang diagnostic push)
#define IGNORE_DIAGNOSTIC(DIAGNOSTIC_FLAG_STR) DO_PRAGMA(clang diagnostic ignored DIAGNOSTIC_FLAG_STR)
#define POP_DIAGNOSTIC()					   DO_PRAGMA(clang diagnostic pop)

#define IGNORE_WARNING_ADD(WARNING_FLAG_STR)	   IGNORE_DIAGNOSTIC(WARNING_FLAG_STR)
#define IGNORE_WARNING_BEGIN_1(W1)				   PUSH_DIAGNOSTIC() IGNORE_WARNING_ADD(W1)
#define IGNORE_WARNING_BEGIN_2(W1, W2)			   IGNORE_WARNING_BEGIN_1(W1) IGNORE_WARNING_ADD(W2)
#define IGNORE_WARNING_BEGIN_3(W1, W2, W3)		   IGNORE_WARNING_BEGIN_2(W1, W2) IGNORE_WARNING_ADD(W3)
#define IGNORE_WARNING_BEGIN_4(W1, W2, W3, W4)	   IGNORE_WARNING_BEGIN_3(W1, W2, W3) IGNORE_WARNING_ADD(W4)
#define IGNORE_WARNING_BEGIN_5(W1, W2, W3, W4, W5) IGNORE_WARNING_BEGIN_4(W1, W2, W3, W4) IGNORE_WARNING_ADD(W5)

#define COUNT_ARGS_SELECT(_1, _2, _3, _4, _5, N, ...) N
#define COUNT_ARGS(...)								  COUNT_ARGS_SELECT(__VA_ARGS__, 5, 4, 3, 2, 1)

#define SELECT_MACRO_EXPANDER(name, count) name##count
#define SELECT_MACRO(name, count)		   SELECT_MACRO_EXPANDER(name, count)

// The main macro for multiple warning suppression
#define IGNORE_WARNING_BEGIN(...) SELECT_MACRO(IGNORE_WARNING_BEGIN_, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define IGNORE_WARNING_END()	  POP_DIAGNOSTIC()

// =====================================================================================================================

#define ignore_warning_begin(...)                                                                                      \
	IGNORE_WARNING_BEGIN(__VA_ARGS__)                                                                                  \
	EMPTY_DO_WHILE()

#define ignore_warning_add(...)                                                                                        \
	IGNORE_WARNING_ADD(__VA_ARGS__)                                                                                    \
	EMPTY_DO_WHILE()

#define ignore_warning_end()                                                                                           \
	IGNORE_WARNING_END()                                                                                               \
	EMPTY_DO_WHILE()

// =====================================================================================================================

#define CLASS_PADDING(BYTES)                                                                                           \
	IGNORE_WARNING_BEGIN("-Wzero-length-array", "-Wunused-private-field")                                              \
	std::array<char, (BYTES)> _unused_padding = {} IGNORE_WARNING_END()

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
inline std::string
fmt_str(const char* fmt, Args&&... args)
{
	ignore_warning_begin("-Wformat-nonliteral", "-Wformat-security");

	const int size = std::snprintf(nullptr, 0, fmt, std::forward<Args>(args)...);
	require_throw(size >= 0, std::runtime_error("Error during formatting -- format: " + std::string(fmt)));

	std::vector<char> buf(static_cast<size_t>(size) + 1);
	(void)std::snprintf(buf.data(), buf.size(), fmt, std::forward<Args>(args)...);
	ignore_warning_end();

	return {buf.data(), static_cast<size_t>(size)};
}

// NOLINTEND(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)

#endif // GENERAL_H
