#ifndef GENERAL_H
#define GENERAL_H

#include <array> // IWYU pragma: keep
#include <cassert>
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

/*
 *	@brief
 *		Ignore the unused variable warning.
 */
#if defined(__GNUC__) || defined(__clang__)
	#define UNUSED [[maybe_unused]]
#else
	#define UNUSED
#endif

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

/*
 *	@brief
 *		Require that a condition is true, otherwise throw an exception.
 *
 *	@param condition
 *		The condition to check.
 *
 *	@param action
 *		The action to take if the condition is false.
 *
 *	@param ret
 *		The return value if the condition is false.
 */
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

/*
 *	@brief
 *		Require that a condition is true, otherwise return.
 *
 *	@param condition
 *		The condition to check.
 */
#ifndef require_return
	#define require_return(condition) require_action_return(condition, EMPTY_DO_WHILE())
#endif

/*
 *	@brief
 *		Require that a pointer is not null, otherwise return.
 *
 *	@param ptr
 *		The pointer to check.
 */
#ifndef require_nonnull_return
	#define require_nonnull_return(ptr) require_return((ptr) != nullptr)
#endif

/*
 *	@brief
 *		Require that a condition is true, otherwise throw an exception.
 *
 *	@param condition
 *		The condition to check.
 *
 *	@param ret
 *		The return value if the condition is false.
 */
#ifndef require_return_value
	#define require_return_value(condition, ret) require_action_return_value(condition, EMPTY_DO_WHILE(), ret)
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

/*
 *	@brief
 *		Require that a condition is true, otherwise throw an exception.
 *
 *	@param condition
 *		The condition to check.
 */
#ifndef require_assert
	#define require_assert(condition) assert((condition) && "assertion violated: " #condition)
#endif

// Helper macros for stringification
#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x)		STRINGIFY_HELPER(x)

#ifndef require_assert_message
	// NOLINTBEGIN(misc-static-assert, cert-dcl03-c, hicpp-static-assert)
	#define require_assert_message(condition, message)                                                                 \
		assert((condition) && (message " [" __FILE__ ":" STRINGIFY(__LINE__) "]"))
	// NOLINTEND(misc-static-assert, cert-dcl03-c, hicpp-static-assert)
#endif

// =====================================================================================================================

constexpr double DEFAULT_EPSILON = 1e-9;

static inline bool
is_close(double a, double b, double epsilon = DEFAULT_EPSILON)
{
	return std::abs(a - b) < epsilon;
}

static inline bool
is_close_to_zero(double a, double epsilon = DEFAULT_EPSILON)
{
	return is_close(a, 0.0, epsilon);
}

// =====================================================================================================================
/*
 *	@brief
 *		Format a string using the given format and arguments.
 *
 *	@param fmt
 *		The format string.
 *
 *	@param args
 *		The arguments to format.
 *
 *	@return
 *		The formatted string.
 */

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
