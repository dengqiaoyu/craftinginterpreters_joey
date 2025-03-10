cmake_minimum_required(VERSION 3.10.0)
project(cpplox VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

add_executable(
	cpplox
	exprs/expr.cpp
	lox.cpp
	main.cpp
	scanner.cpp
	token.cpp
)
