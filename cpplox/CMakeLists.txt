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
# Include directories
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# ======================================================================================================================
set(COMMON_SOURCES
	src/lox.cpp
	src/scanner.cpp
	src/token.cpp
	src/value.cpp
)

# ======================================================================================================================
# Target: cpplox
set(CPPLOX_SOURCES
	src/main.cpp
)

add_executable(cpplox ${COMMON_SOURCES} ${CPPLOX_SOURCES})

# ======================================================================================================================
# Target: ast_module_generator
set(AST_MODULE_GENERATOR_SOURCES
	src/tools/ast_module_generator.cpp
)

add_executable(ast_module_generator ${AST_MODULE_GENERATOR_SOURCES})

# ======================================================================================================================
# Target: ast_printer
set(AST_PRINTER_SOURCES
	src/tools/ast_printer.cpp
)

add_executable(ast_printer ${COMMON_SOURCES} ${AST_PRINTER_SOURCES})
