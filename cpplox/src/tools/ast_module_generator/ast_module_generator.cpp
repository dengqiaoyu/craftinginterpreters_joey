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

static bool
is_shared_ptr_type(const std::string& type)
{
	return type.find("shared_ptr") != std::string::npos;
}

// NOLINTBEGIN(readability-function-cognitive-complexity, bugprone-easily-swappable-parameters)
static int
generate_ast(const std::string& output_dir_path, const std::vector<std::string>& additional_headers,
	const std::string& base_class_name, const std::vector<ASTClass>& ast_classes)
// NOLINTEND(readability-function-cognitive-complexity, bugprone-easily-swappable-parameters)
{
	// Create output directory.
	std::cout << std::format("Creating output directory: {}", output_dir_path) << std::endl;
	std::filesystem::create_directories(output_dir_path);

	const char* const bcls_n = base_class_name.c_str();
	const std::string base_variable_name = tolower(base_class_name);
	const char* const bvar_n = base_variable_name.c_str();

	// Generate header file.
	const std::string header_file_name = tolower(base_class_name) + ".h";
	const std::string header_file = output_dir_path + "/" + header_file_name;

	// Print actions.
	std::cout << std::format("Generating AST classes -- class name: {}, file: {}", base_class_name, header_file_name)
			  << std::endl;

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
	std::vector<std::string> system_includes = {
		"<any>",
		"<memory>",
		"<ostream>",
		"<string>",
	};
	std::vector<std::string> user_includes;
	for (const auto& header : additional_headers) {
		if (header.find('<') != std::string::npos) {
			system_includes.push_back(header);
		} else {
			user_includes.push_back(header);
		}
	}
	std::sort(system_includes.begin(), system_includes.end());
	std::sort(user_includes.begin(), user_includes.end());

	for (const auto& header : system_includes) {
		hs << fmt_str("#include %s\n", header.c_str());
	}
	hs << "\n";
	for (const auto& header : user_includes) {
		hs << fmt_str("#include %s\n", header.c_str());
	}
	hs << "\n";

	// Forward declarations
	hs << fmt_str("// Forward declarations.\n");
	for (const auto& ast_class : ast_classes) {
		hs << fmt_str("class %s;\n", ast_class.get_class_name().c_str());
	}
	hs << "\n";

	// clang-format off
	hs << fmt_str("// =====================================================================================================================\n");
	// clang-format on

	// Visitor class
	hs << fmt_str("// Visitor class\n");
	hs << fmt_str("class %sVisitor\n", bcls_n);
	hs << fmt_str("{\n");
	hs << fmt_str("public:\n");
	hs << fmt_str("	%sVisitor() = default;\n", bcls_n);
	hs << fmt_str("	%sVisitor(const %sVisitor&) = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	%sVisitor& operator=(const %sVisitor&) = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	%sVisitor(%sVisitor&&) noexcept = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	%sVisitor& operator=(%sVisitor&&) noexcept = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	virtual ~%sVisitor();\n\n", bcls_n);

	// Visitor methods
	for (const auto& ast_class : ast_classes) {
		hs << fmt_str("	[[nodiscard]] virtual std::any visit_%s_%s(const %s& %s) = 0;\n",
			tolower(ast_class.get_class_name()).c_str(), bvar_n, ast_class.get_class_name().c_str(), bvar_n);
	}
	hs << fmt_str("};\n\n");
	// clang-format off
	hs << fmt_str("// =====================================================================================================================\n");
	// clang-format on

	// Base class
	hs << fmt_str("// Base %s\n", bcls_n);
	hs << fmt_str("class %s\n", bcls_n);
	hs << fmt_str("{\n");
	hs << fmt_str("public:\n");
	hs << fmt_str("	%s() = default;\n", bcls_n);
	hs << fmt_str("	%s(const %s&) = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	%s& operator=(const %s&) = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	%s(%s&&) noexcept = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	%s& operator=(%s&&) noexcept = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	virtual ~%s();\n", bcls_n);
	hs << fmt_str("\n");
	hs << fmt_str("	[[nodiscard]] virtual std::any accept(%sVisitor& visitor) const = 0;\n", bcls_n);
	hs << fmt_str("	[[nodiscard]] virtual std::string to_string() const = 0;\n");
	hs << fmt_str("	friend std::ostream& operator<<(std::ostream& out_s, const %s& %s);\n", bcls_n, bvar_n);
	hs << fmt_str("};\n\n");

	// Derived classes
	for (const auto& ast_class : ast_classes) {
		const std::string& class_name = ast_class.get_class_name();
		const std::vector<std::pair<std::string, std::string>>& members = ast_class.get_members();

		// clang-format off
		hs << fmt_str("// =====================================================================================================================\n");
		// clang-format on

		hs << fmt_str("class %s : public %s\n", class_name.c_str(), bcls_n);
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
		hs << fmt_str("\n");

		// Generate getters first
		for (const auto& member : members) {
			if (is_shared_ptr_type(member.first)) {
				hs << fmt_str("	[[nodiscard]] const %s& get_%s() const;\n", member.first.c_str(),
					member.second.c_str());
			} else {
				hs << fmt_str("	[[nodiscard]] const %s& get_%s() const;\n", extract_type(member.first).c_str(),
					member.second.c_str());
			}
		}
		hs << fmt_str("\n");

		// Then generate accept and to_string methods
		hs << fmt_str("	[[nodiscard]] std::any accept(%sVisitor& visitor) const override;\n", bcls_n);
		hs << fmt_str("	[[nodiscard]] std::string to_string() const override;");
		hs << "\n";
		hs << fmt_str("private:\n");
		for (const auto& member : members) {
			hs << fmt_str("	%s m_%s;\n", member.first.c_str(), member.second.c_str());
		}
		hs << fmt_str("};\n");
		hs << "\n";
	}

	hs << fmt_str("#endif // %s_H\n", tolower(base_class_name).c_str());
	hs.close();

	// Generate source file.
	const std::string source_file_name = tolower(base_class_name) + ".cpp";
	const std::string source_file = output_dir_path + "/" + source_file_name;

	// Delete existing source file if it exists.
	if (std::filesystem::exists(source_file)) {
		std::cout << "Deleting existing " << source_file << "..." << std::endl;
		std::filesystem::remove(source_file);
	}

	std::cout << "Generating " << source_file << "..." << std::endl;
	std::ofstream cs(source_file);
	require_action_return_value(cs.is_open(), std::cout << "Failed to open: " << source_file << std::endl, EINVAL);

	// Includes
	cs << fmt_str("#include \"%s\"\n\n", header_file_name.c_str());

	// clang-format off
	cs << fmt_str("// =====================================================================================================================\n");
	// clang-format on
	cs << fmt_str("// visitor\n");
	cs << fmt_str("%sVisitor::~%sVisitor() = default;\n\n", bcls_n, bcls_n);

	// clang-format off
	cs << fmt_str("// =====================================================================================================================\n");
	// clang-format on

	// Base class implementation
	cs << fmt_str("// %s\n\n", bcls_n);
	cs << fmt_str("%s::~%s() = default;\n\n", bcls_n, bcls_n);

	cs << fmt_str("std::ostream&\n");
	cs << fmt_str("operator<<(std::ostream& out_s, const %s& %s)\n", bcls_n, bvar_n);
	cs << fmt_str("{\n");
	cs << fmt_str("	return out_s << %s.to_string();\n", bvar_n);
	cs << fmt_str("}\n\n");

	// Formatter specialization for %s
	cs << fmt_str("// Formatter specialization for %s\n", bcls_n);
	cs << fmt_str("template <>\n");
	cs << fmt_str("struct std::formatter<%s> : std::formatter<std::string> // NOLINT(altera-struct-pack-align)\n",
		bcls_n);
	cs << fmt_str("{\n");
	cs << fmt_str("	auto\n");
	cs << fmt_str("	format(const %s& %s, format_context& ctx) const\n", bcls_n, bvar_n);
	cs << fmt_str("	{\n");
	cs << fmt_str("		return std::formatter<std::string>::format(%s.to_string(), ctx);\n", bvar_n);
	cs << fmt_str("	}\n");
	cs << fmt_str("};\n\n");

	// Derived class implementations
	for (const auto& ast_class : ast_classes) {
		const std::string& class_name = ast_class.get_class_name();
		const std::vector<std::pair<std::string, std::string>>& members = ast_class.get_members();

		// clang-format off
		cs << "// =====================================================================================================================\n";
		// clang-format on
		cs << fmt_str("// %s\n\n", class_name.c_str());

		// Constructor
		cs << fmt_str("%s::%s(", class_name.c_str(), class_name.c_str());
		for (size_t i = 0; i < members.size(); ++i) {
			cs << fmt_str("%s %s", members[i].first.c_str(), members[i].second.c_str());
			if (i != members.size() - 1) {
				cs << ", ";
			}
		}
		cs << ")\n";
		cs << fmt_str("	: ");
		for (size_t i = 0; i < members.size(); ++i) {
			cs << fmt_str("m_%s(std::move(%s))", members[i].second.c_str(), members[i].second.c_str());
			if (i != members.size() - 1) {
				cs << ", ";
			}
		}
		cs << "\n";
		cs << "{\n";
		cs << fmt_str("	// Empty constructor.\n");
		cs << "}\n\n";

		// Getters (moved up, after constructor)
		for (const auto& member : members) {
			if (is_shared_ptr_type(member.first)) {
				cs << fmt_str("const %s&\n", member.first.c_str());
			} else {
				cs << fmt_str("const %s&\n", extract_type(member.first).c_str());
			}
			cs << fmt_str("%s::get_%s() const\n", class_name.c_str(), member.second.c_str());
			cs << "{\n";
			if (is_ptr_type(member.first) && !is_shared_ptr_type(member.first)) {
				cs << fmt_str("	return *m_%s;\n", member.second.c_str());
			} else {
				cs << fmt_str("	return m_%s;\n", member.second.c_str());
			}
			cs << "}\n\n";
		}

		// Accept method (after getters)
		cs << fmt_str("std::any\n");
		cs << fmt_str("%s::accept(%sVisitor& visitor) const\n", class_name.c_str(), bcls_n);
		cs << "{\n";
		cs << fmt_str("	return visitor.visit_%s_%s(*this);\n", tolower(class_name).c_str(), bvar_n);
		cs << "}\n\n";

		// to_string method (moved to end)
		cs << fmt_str("std::string\n");
		cs << fmt_str("%s::to_string() const\n", class_name.c_str());
		cs << "{\n";
		cs << fmt_str("	return std::format(\"%s %s{{", class_name.c_str(), bvar_n);
		for (size_t i = 0; i < members.size(); ++i) {
			cs << fmt_str("%s={}", members[i].second.c_str());
			if (i != members.size() - 1) {
				cs << ", ";
			}
		}
		cs << "}}\", ";
		for (size_t i = 0; i < members.size(); ++i) {
			if (is_ptr_type(members[i].first)) {
				cs << fmt_str("m_%s->to_string()", members[i].second.c_str());
			} else {
				cs << fmt_str("m_%s.to_string()", members[i].second.c_str());
			}
			if (i != members.size() - 1) {
				cs << ", ";
			}
		}
		cs << ");\n";
		cs << "}\n\n";
	}

	cs.close();
	std::cout << "Done." << std::endl;

	return 0;
}

int
main()
{
	generate_ast("src/asts", {"\"token.h\"", "\"value.h\""}, "Expr",
		// clang-format off
		{
			ASTClass("Binary",
				{
					{"std::shared_ptr<const Expr>",	"left"},
					{"Token",						"opr"},
					{"std::shared_ptr<const Expr>",	"right"}
				}
			),
			ASTClass("Ternary",
				{
					{"std::shared_ptr<const Expr>",	"condition"},
					{"Token",						"qmark"},
					{"std::shared_ptr<const Expr>",	"then_branch"},
					{"Token",						"colon"},
					{"std::shared_ptr<const Expr>",	"else_branch"}
				}
			),
			ASTClass("Grouping",
				{
					{"std::shared_ptr<const Expr>",	"expr"}
				}
			),
			ASTClass("Literal",
				{
					{"Value", 						"value"}
				}
			),
			ASTClass(
				"Variable",
				{
					{"Token",						"name"}
				}
			),
			ASTClass("Unary",
				{
					{"Token",						"opr"},
					{"std::shared_ptr<const Expr>",	"right"}
				}
			)
		} // clang-format on
	);

	std::cout << std::format("======================") << std::endl;

	// clang-format off
	generate_ast("src/asts", {"\"expr.h\"", "\"token.h\""}, "Stmt",
		{
			ASTClass("Var",
				{
					{"Token",						"name"},
					{"std::shared_ptr<const Expr>",	"initializer"}
				}
			),
			ASTClass("Expression",
				{
					{"std::shared_ptr<const Expr>",	"expr"}
				}
			),
			ASTClass("Print",
				{
					{"std::shared_ptr<const Expr>",	"expr"}
				}
			),
		}
	);
	// clang-format on

	return 0;
}
