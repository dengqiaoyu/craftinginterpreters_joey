cmake_minimum_required(VERSION 3.10.0)
project(cpplox VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
	-Wall # Enable most common warnings
	-Wextra # Enable extra warnings
	-Wpedantic # Enforce strict ISO C++ compliance
	-Werror # Treat warnings as errors
	-Weverything # Enable all Clang warnings
	-Wno-c++98-compat # Disable warnings related to C++98 compatibility
	-Wno-c++98-compat-pedantic # Disable warnings related to C++98 compatibility
	-Wno-unsafe-buffer-usage # Disable warnings related to unsafe buffer usage.
	-fsafe-buffer-usage-suggestions # Receive code hardening suggestions.
)

include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# === Target: cpplox ===
set(CPPLOX_SOURCES

	# src/expr.ixx
	src/lox.cpp
	src/main.cpp
	src/scanner.cpp
	src/token.cpp
)

add_executable(cpplox ${CPPLOX_SOURCES})

# === Target: ast_module_generator ===
set(AST_MODULE_GENERATOR_SOURCES
	src/tools/ast_module_generator.cpp
)

add_executable(ast_module_generator ${AST_MODULE_GENERATOR_SOURCES})
