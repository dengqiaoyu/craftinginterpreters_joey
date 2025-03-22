#include "ast_module_generator.h"

#include <algorithm>
#include <cerrno>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

#include "general.h"

ASTClass::ASTClass(std::string class_name, std::vector<std::pair<std::string, std::string>> members)
	: m_class_name(std::move(class_name)), m_members(std::move(members))
{
	// Empty constructor.
}

const std::string&
ASTClass::get_class_name() const
{
	return m_class_name;
}

const std::vector<std::pair<std::string, std::string>>&
ASTClass::get_members() const
{
	return m_members;
}

static std::string
tolower(const std::string& str)
{
	std::string lower_str(str);
	std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return lower_str;
}

static std::string
extract_type(const std::string& ptr_type)
{
	const size_t start = ptr_type.find('<');
	const size_t end = ptr_type.rfind('>');
	if (start == std::string::npos || end == std::string::npos) {
		return ptr_type;
	}
	return ptr_type.substr(start + 1, end - start - 1);
}

static bool
is_ptr_type(const std::string& type)
{
	return type.find("ptr") != std::string::npos;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
int
main(const int argc, const char* const argv[])
{
	require_action_return_value(argc == 2,
		std::cout << "Usage: ast_module_generator <output directory path>" << std::endl, EINVAL);
	const std::string output_dir_path(argv[1]); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

	// Create output directory.
	std::cout << std::format("Creating output directory: {}", output_dir_path) << std::endl;
	std::filesystem::create_directories(output_dir_path);

	const std::string base_class_name = "Expr";

	// clang-format off
	const std::vector<ASTClass> ast_classes{
		ASTClass("Binary",
			{
				{"std::shared_ptr<Expr<R>>",	"left"},
				{"Token",						"opr"},
				{"std::shared_ptr<Expr<R>>",	"right"}
			}
		),
		ASTClass("Grouping",
			{
				{"std::shared_ptr<Expr<R>>",	"expr"}
			}
		),
		ASTClass("Literal",
			{
				{"Value", 						"value"}
			}
		),
		ASTClass("Unary",
			{
				{"Token",						"opr"},
				{"std::shared_ptr<Expr<R>>",	"right"}
			}
		)
	};
	// clang-format on

	const char* const bcls_n = base_class_name.c_str();

	// Generate header file.
	const std::string header_file_name = tolower(base_class_name) + ".h";
	const std::string header_file = output_dir_path + "/" + header_file_name;

	// Delete existing header file if it exists.
	if (std::filesystem::exists(header_file)) {
		std::cout << "Deleting existing " << header_file << "..." << std::endl;
		std::filesystem::remove(header_file);
	}

	std::cout << "Generating " << header_file << "..." << std::endl;
	std::ofstream hs(header_file);
	require_action_return_value(hs.is_open(), std::cout << "Failed to open: " << header_file << std::endl, EINVAL);

	// Header guard
	hs << fmt_str("#ifndef %s_H\n", tolower(base_class_name).c_str());
	hs << fmt_str("#define %s_H\n\n", tolower(base_class_name).c_str());

	// Includes
	hs << fmt_str("#include <any>\n");
	hs << fmt_str("#include <memory>\n");
	hs << fmt_str("#include <ostream>\n");
	hs << fmt_str("#include <string>\n\n");
	hs << fmt_str("#include \"token.h\"\n");
	hs << fmt_str("#include \"general.h\"\n");
	hs << fmt_str("#include \"value.h\"\n\n");

	// Forward declarations
	hs << fmt_str("// Forward declarations.\n");
	for (const auto& ast_class : ast_classes) {
		hs << fmt_str("template <typename R>\n");
		hs << fmt_str("class %s;\n", ast_class.get_class_name().c_str());
	}
	hs << "\n";

	// clang-format off
	hs << fmt_str("// =====================================================================================================================\n");
	// clang-format on
	hs << fmt_str("// Declarations\n\n");

	// Visitor class
	hs << fmt_str("// Visitor class\n");
	hs << fmt_str("template <typename R>\n");
	hs << fmt_str("class Visitor\n");
	hs << fmt_str("{\n");
	hs << fmt_str("public:\n");
	hs << fmt_str("	Visitor() = default;\n");
	hs << fmt_str("	Visitor(const Visitor&) = default;\n");
	hs << fmt_str("	Visitor& operator=(const Visitor&) = default;\n");
	hs << fmt_str("	Visitor(Visitor&&) noexcept = default;\n");
	hs << fmt_str("	Visitor& operator=(Visitor&&) noexcept = default;\n");
	hs << fmt_str("	virtual ~Visitor() = default;\n\n");

	// Visitor methods
	for (const auto& ast_class : ast_classes) {
		hs << fmt_str("	virtual R\n");
		hs << fmt_str("	visit_%s_expr(const %s<R>& expr) = 0;\n", tolower(ast_class.get_class_name()).c_str(),
			ast_class.get_class_name().c_str());
	}
	hs << fmt_str("};\n\n");
	// clang-format off
	hs << fmt_str("// =====================================================================================================================\n");
	// clang-format on

	// Base class
	hs << fmt_str("// Base %s\n", bcls_n);
	hs << fmt_str("template <typename R>\n");
	hs << fmt_str("class %s\n", bcls_n);
	hs << fmt_str("{\n");
	hs << fmt_str("public:\n");
	hs << fmt_str("	%s() = default;\n", bcls_n);
	hs << fmt_str("	%s(const %s&) = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	%s&\n", bcls_n);
	hs << fmt_str("	operator=(const %s&) = default;\n", bcls_n);
	hs << fmt_str("	%s(%s&&) noexcept = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	%s&\n", bcls_n);
	hs << fmt_str("	operator=(%s&&) noexcept = default;\n", bcls_n);
	hs << fmt_str("	virtual ~%s();\n", bcls_n);
	hs << fmt_str("	[[nodiscard]] virtual std::string\n");
	hs << fmt_str("	to_string() const = 0;\n\n");
	hs << fmt_str("	virtual R\n");
	hs << fmt_str("	accept(const Visitor<R>& visitor) const = 0;\n\n");
	hs << fmt_str("	friend std::ostream&\n");
	hs << fmt_str("	operator<<(std::ostream& out_s, const %s& expr);\n", bcls_n);
	hs << fmt_str("};\n\n");

	// Derived classes
	for (const auto& ast_class : ast_classes) {
		const std::string& class_name = ast_class.get_class_name();
		const std::vector<std::pair<std::string, std::string>>& members = ast_class.get_members();

		// clang-format off
		hs << fmt_str("// =====================================================================================================================\n");
		// clang-format on

		hs << fmt_str("template <typename R>\n");
		hs << fmt_str("class %s : public %s<R>\n", class_name.c_str(), bcls_n);
		hs << fmt_str("{\n");
		hs << fmt_str("public:\n");
		if (members.size() == 1) {
			hs << fmt_str("	explicit %s(", class_name.c_str());
		} else {
			hs << fmt_str("	%s(", class_name.c_str());
		}
		for (size_t i = 0; i < members.size(); ++i) {
			hs << fmt_str("%s %s", members[i].first.c_str(), members[i].second.c_str());
			if (i != members.size() - 1) {
				hs << ", ";
			}
		}
		hs << ");\n";
		hs << fmt_str("	[[nodiscard]] R\n");
		hs << fmt_str("	accept(const Visitor<R>& visitor) const override;\n");
		hs << fmt_str("	[[nodiscard]] std::string\n");
		hs << fmt_str("	to_string() const override;\n\n");

		for (const auto& member : members) {
			hs << fmt_str("	[[nodiscard]] const %s&\n", extract_type(member.first).c_str());
			hs << fmt_str("	get_%s() const;\n", member.second.c_str());
		}
		hs << "\n";
		hs << fmt_str("private:\n");
		for (const auto& member : members) {
			hs << fmt_str("	%s m_%s;\n", member.first.c_str(), member.second.c_str());
		}
		hs << fmt_str("};\n");
		hs << "\n";
	}

	// clang-format off
	hs << fmt_str("// =====================================================================================================================\n");
	// clang-format on
	hs << fmt_str("// Implementations\n\n");

	// Base class implementation
	hs << fmt_str("// %s\n", bcls_n);
	hs << fmt_str("template <typename R>\n");
	hs << fmt_str("%s<R>::~%s() = default;\n\n", bcls_n, bcls_n);

	hs << fmt_str("template <typename R>\n");
	hs << fmt_str("std::ostream&\n");
	hs << fmt_str("operator<<(std::ostream& out_s, const %s<R>& expr)\n", bcls_n);
	hs << fmt_str("{\n");
	hs << fmt_str("	return out_s << expr.to_string();\n");
	hs << fmt_str("}\n\n");

	// Formatter specialization for Expr
	hs << fmt_str("// Formatter specialization for Expr\n");
	hs << fmt_str("template <typename R>\n");
	hs << fmt_str("struct std::formatter<Expr<R>> : std::formatter<std::string> // NOLINT(altera-struct-pack-align)\n");
	hs << fmt_str("{\n");
	hs << fmt_str("	auto\n");
	hs << fmt_str("	format(const Expr<R>& expr, format_context& ctx) const\n");
	hs << fmt_str("	{\n");
	hs << fmt_str("		return std::formatter<std::string>::format(expr.to_string(), ctx);\n");
	hs << fmt_str("	}\n");
	hs << fmt_str("};\n\n");

	// Derived class implementations
	for (const auto& ast_class : ast_classes) {
		const std::string& class_name = ast_class.get_class_name();
		const std::vector<std::pair<std::string, std::string>>& members = ast_class.get_members();

		// clang-format off
		hs << "// =====================================================================================================================\n";
		// clang-format on
		hs << fmt_str("// %s\n\n", class_name.c_str());

		// Constructor
		hs << fmt_str("template <typename R>\n");
		hs << fmt_str("%s<R>::%s(", class_name.c_str(), class_name.c_str());
		for (size_t i = 0; i < members.size(); ++i) {
			hs << fmt_str("%s %s", members[i].first.c_str(), members[i].second.c_str());
			if (i != members.size() - 1) {
				hs << ", ";
			}
		}
		hs << ")\n";
		hs << fmt_str("	: ");
		for (size_t i = 0; i < members.size(); ++i) {
			hs << fmt_str("m_%s(std::move(%s))", members[i].second.c_str(), members[i].second.c_str());
			if (i != members.size() - 1) {
				hs << ", ";
			}
		}
		hs << "\n";
		hs << "{\n";
		hs << fmt_str("	// Empty constructor.\n");
		hs << "}\n\n";

		// to_string method
		hs << fmt_str("template <typename R>\n");
		hs << fmt_str("std::string\n");
		hs << fmt_str("%s<R>::to_string() const\n", class_name.c_str());
		hs << "{\n";
		hs << fmt_str("	return fmt_str(\"%s Expr{", class_name.c_str());
		for (size_t i = 0; i < members.size(); ++i) {
			hs << fmt_str("%s=%%s", members[i].second.c_str());
			if (i != members.size() - 1) {
				hs << ", ";
			}
		}
		hs << "}\", ";
		for (size_t i = 0; i < members.size(); ++i) {
			if (is_ptr_type(members[i].first)) {
				hs << fmt_str("m_%s->to_string().c_str()", members[i].second.c_str());
			} else {
				hs << fmt_str("m_%s.to_string().c_str()", members[i].second.c_str());
			}
			if (i != members.size() - 1) {
				hs << ", ";
			}
		}
		hs << ");\n";
		hs << "}\n\n";

		// Getters
		for (const auto& member : members) {
			hs << fmt_str("template <typename R>\n");
			hs << fmt_str("const %s&\n", extract_type(member.first).c_str());
			hs << fmt_str("%s<R>::get_%s() const\n", class_name.c_str(), member.second.c_str());
			hs << "{\n";
			if (is_ptr_type(member.first)) {
				hs << fmt_str("	return *m_%s;\n", member.second.c_str());
			} else {
				hs << fmt_str("	return m_%s;\n", member.second.c_str());
			}
			hs << "}\n\n";
		}

		// Accept method
		hs << fmt_str("template <typename R>\n");
		hs << fmt_str("R\n");
		hs << fmt_str("%s<R>::accept(const Visitor<R>& visitor) const\n", class_name.c_str());
		hs << "{\n";
		hs << fmt_str("	return visitor.visit_%s_expr(*this);\n", tolower(class_name).c_str());
		hs << "}\n\n";
	}
	hs << fmt_str("#endif // %s_H\n", tolower(base_class_name).c_str());

	hs.close();
	std::cout << "Done." << std::endl;

	return 0;
}
