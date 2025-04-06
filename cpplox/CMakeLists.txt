cmake_minimum_required(VERSION 3.10.0)
project(cpplox VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Ensure we're using Clang
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	message(FATAL_ERROR "This CMakeLists.txt is configured specifically for Clang")
endif()

# ======================================================================================================================
# Basic waring and error flags
add_compile_options(
	-Wall # Enable most common warnings
	-Wextra # Enable extra warnings
	-Wpedantic # Enforce strict ISO C++ compliance
	-Werror # Treat warnings as errors
	-Weverything # Enable all Clang warnings
)

# Some checking extra flags
add_compile_options(
	-fsafe-buffer-usage-suggestions # specific to Clang that helps identify potential buffer overflow vulnerabilities
)

# Disable some warnings
add_compile_options(
	-Wno-c++98-compat # Disable warnings related to C++98 compatibility
	-Wno-c++98-compat-pedantic # Disable warnings related to C++98 compatibility
	-Wno-unsafe-buffer-usage # Disable warnings related to unsafe buffer usage.
)

# ======================================================================================================================
# Check libraries dependencies

# Check for the Readline library
find_library(READLINE_LIBRARY NAMES readline)
find_path(READLINE_INCLUDE_DIR NAMES readline/readline.h)

if(NOT READLINE_LIBRARY OR NOT READLINE_INCLUDE_DIR)
	message(FATAL_ERROR "Readline library not found. Please install libreadline-dev or equivalent.")
endif()

# ======================================================================================================================
# Include directories

# External libraries
include_directories(
	${READLINE_INCLUDE_DIR}
)

# Project source directory
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# ======================================================================================================================
# Target: cpplox
set(CPPLOX_SOURCES

	# Core code
	src/expr.cpp
	src/lox.cpp
	src/main.cpp
	src/parser.cpp
	src/scanner.cpp
	src/token.cpp
	src/value.cpp
	src/visitors/examples/ast_printer.cpp

	# Helpers
	src/utilities/lox_readline.cpp
)

add_executable(cpplox ${CPPLOX_SOURCES})

# Link against the Readline library
target_link_libraries(cpplox ${READLINE_LIBRARY})

# ======================================================================================================================
# Target: ast_module_generator
set(AST_MODULE_GENERATOR_SOURCES
	src/tools/ast_module_generator/ast_module_generator.cpp
)

add_executable(ast_module_generator ${AST_MODULE_GENERATOR_SOURCES})

# ======================================================================================================================
# Target: visitor_examples
set(VISITOR_EXAMPLES_SOURCES
	src/expr.cpp
	src/token.cpp
	src/value.cpp

	src/visitors/examples/ast_printer.cpp
	src/visitors/examples/rpn_printer.cpp

	src/tools/visitor_examples/visitor_examples.cpp
)

add_executable(visitor_examples ${VISITOR_EXAMPLES_SOURCES})
