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

const std::string& ASTClass::get_class_name() const
{
	return m_class_name;
}

const std::vector<std::pair<std::string, std::string>>& ASTClass::get_members() const
{
	return m_members;
}

static std::string tolower(const std::string& str)
{
	std::string lower_str(str);
	std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return lower_str;
}

static std::string extract_type(const std::string& ptr_type)
{
	const size_t start = ptr_type.find('<');
	const size_t end = ptr_type.rfind('>');
	if (start == std::string::npos || end == std::string::npos) {
		return ptr_type;
	}
	return ptr_type.substr(start + 1, end - start - 1);
}

static bool is_ptr_type(const std::string& type)
{
	return type.find("ptr") != std::string::npos;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
int main(const int argc, const char* const argv[])
{
	require_action_return_value(argc == 2,
		std::cout << "Usage: ast_module_generator <output directory path>" << std::endl, EINVAL);
	const std::string output_dir_path(argv[1]);

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
				{"std::any", 					"value"}
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

	// Generate module file.
	const std::string module_file_name = tolower(base_class_name) + ".ixx";
	const std::string module_file = output_dir_path + "/" + module_file_name;

	// Delete existing module file if it exists.
	if (std::filesystem::exists(module_file)) {
		std::cout << "Deleting existing " << module_file << "..." << std::endl;
		std::filesystem::remove(module_file);
	}

	std::cout << "Generating " << module_file << "..." << std::endl;
	std::ofstream ms(module_file);
	require_action_return_value(ms.is_open(), std::cout << "Failed to open: " << module_file << std::endl, EINVAL);

	// Module header
	ms << fmt_str("module;\n\n");
	ms << fmt_str("#include <any>\n");
	ms << fmt_str("#include <memory>\n");
	ms << fmt_str("#include <ostream>\n");
	ms << fmt_str("#include <string>\n\n");
	ms << fmt_str("#include \"token.h\"\n");
	ms << fmt_str("#include \"general.h\"\n\n");
	ms << fmt_str("export module %s_module;\n\n", tolower(base_class_name).c_str());

	// Export block
	ms << fmt_str("export\n");
	ms << fmt_str("{\n");

	// Forward declarations
	ms << fmt_str("	// Forward declarations.\n");
	for (const auto& ast_class : ast_classes) {
		ms << fmt_str("	template <typename R>\n");
		ms << fmt_str("	class %s;\n\n", ast_class.get_class_name().c_str());
	}

	// Visitor class
	ms << fmt_str("	template <typename R>\n");
	ms << fmt_str("	class Visitor\n");
	ms << fmt_str("	{\n");
	ms << fmt_str("	public:\n");
	ms << fmt_str("		Visitor() = default;\n");
	ms << fmt_str("		Visitor(const Visitor&) = default;\n");
	ms << fmt_str("		Visitor& operator=(const Visitor&) = default;\n");
	ms << fmt_str("		Visitor(Visitor&&) noexcept = default;\n");
	ms << fmt_str("		Visitor& operator=(Visitor&&) noexcept = default;\n");
	ms << fmt_str("		virtual ~Visitor() = default;\n\n");

	// Visitor methods
	for (const auto& ast_class : ast_classes) {
		ms << fmt_str("		virtual R visit_%s_expr(const %s<R>& expr) = 0;\n",
			tolower(ast_class.get_class_name()).c_str(), ast_class.get_class_name().c_str());
	}
	ms << fmt_str("	};\n\n");

	// Base class
	ms << fmt_str("	template <typename R>\n");
	ms << fmt_str("	class %s\n", bcls_n);
	ms << fmt_str("	{\n");
	ms << fmt_str("	public:\n");
	ms << fmt_str("		%s() = default;\n", bcls_n);
	ms << fmt_str("		%s(const %s&) = default;\n", bcls_n, bcls_n);
	ms << fmt_str("		%s& operator=(const %s&) = default;\n", bcls_n, bcls_n);
	ms << fmt_str("		%s(%s&&) noexcept = default;\n", bcls_n, bcls_n);
	ms << fmt_str("		%s& operator=(%s&&) noexcept = default;\n", bcls_n, bcls_n);
	ms << fmt_str("		virtual ~%s();\n", bcls_n);
	ms << fmt_str("		[[nodiscard]] virtual std::string to_string() const = 0;\n\n");
	ms << fmt_str("		virtual R accept(Visitor<R>& visitor) = 0;\n\n");
	ms << fmt_str("		friend std::ostream& operator<<(std::ostream& out_s, const %s& expr);\n", bcls_n);
	ms << fmt_str("	};\n\n");

	// Derived classes
	for (const auto& ast_class : ast_classes) {
		const std::string& class_name = ast_class.get_class_name();
		const std::vector<std::pair<std::string, std::string>>& members = ast_class.get_members();

		ms << fmt_str("	template <typename R>\n");
		ms << fmt_str("	class %s : public %s<R>\n", class_name.c_str(), bcls_n);
		ms << fmt_str("	{\n");
		ms << fmt_str("	public:\n");
		if (members.size() == 1) {
			ms << fmt_str("		explicit %s(", class_name.c_str());
		} else {
			ms << fmt_str("		%s(", class_name.c_str());
		}
		for (size_t i = 0; i < members.size(); ++i) {
			ms << fmt_str("%s %s", members[i].first.c_str(), members[i].second.c_str());
			if (i != members.size() - 1) {
				ms << ", ";
			}
		}
		ms << ");\n";
		ms << fmt_str("		[[nodiscard]] R accept(Visitor<R>& visitor) override;\n");
		ms << fmt_str("		[[nodiscard]] std::string to_string() const override;\n\n");

		for (const auto& member : members) {
			ms << fmt_str("		[[nodiscard]] const %s& get_%s() const;\n", extract_type(member.first).c_str(),
				member.second.c_str());
		}
		ms << "\n";
		ms << fmt_str("	private:\n");
		for (const auto& member : members) {
			ms << fmt_str("		%s m_%s;\n", member.first.c_str(), member.second.c_str());
		}
		ms << fmt_str("	};\n");
		ms << "\n";
	}
	ms << fmt_str("}\n\n");

	// clang-format off
	// Module implementation
	ms << "// =====================================================================================================================\n";
	ms << "// Module-private implementation.\n";
	ms << "// This is not part of the module interface.\n";
	ms << "// =====================================================================================================================\n\n";
	ms << "module :private;\n\n";

	// Base class implementation
	ms << "// =====================================================================================================================\n";
	// clang-format on

	ms << fmt_str("// %s\n\n", bcls_n);
	ms << fmt_str("template <typename R>\n");
	ms << fmt_str("%s<R>::~%s() = default;\n\n", bcls_n, bcls_n);

	ms << fmt_str("template <typename R>\n");
	ms << fmt_str("std::ostream& operator<<(std::ostream& out_s, const %s<R>& expr)\n", bcls_n);
	ms << fmt_str("{\n");
	ms << fmt_str("	return out_s << expr.to_string();\n");
	ms << fmt_str("}\n\n");

	// Derived class implementations
	for (const auto& ast_class : ast_classes) {
		const std::string& class_name = ast_class.get_class_name();
		const std::vector<std::pair<std::string, std::string>>& members = ast_class.get_members();

		// clang-format off
		ms << "// =====================================================================================================================\n";
		// clang-format on
		ms << fmt_str("// %s\n\n", class_name.c_str());

		// Constructor
		ms << fmt_str("template <typename R>\n");
		ms << fmt_str("%s<R>::%s(", class_name.c_str(), class_name.c_str());
		for (size_t i = 0; i < members.size(); ++i) {
			ms << fmt_str("%s %s", members[i].first.c_str(), members[i].second.c_str());
			if (i != members.size() - 1) {
				ms << ", ";
			}
		}
		ms << ")\n";
		ms << fmt_str("	: ");
		for (size_t i = 0; i < members.size(); ++i) {
			ms << fmt_str("m_%s(std::move(%s))", members[i].second.c_str(), members[i].second.c_str());
			if (i != members.size() - 1) {
				ms << ", ";
			}
		}
		ms << "\n";
		ms << "{\n";
		ms << fmt_str("	// Empty constructor.\n");
		ms << "}\n\n";

		// to_string method
		ms << fmt_str("template <typename R>\n");
		ms << fmt_str("std::string %s<R>::to_string() const\n", class_name.c_str());
		ms << "{\n";
		ms << fmt_str("	return fmt_str(\"%s Expr{", class_name.c_str());
		for (size_t i = 0; i < members.size(); ++i) {
			ms << fmt_str("%s=%%s", members[i].second.c_str());
			if (i != members.size() - 1) {
				ms << ", ";
			}
		}
		ms << "}\", ";
		for (size_t i = 0; i < members.size(); ++i) {
			if (is_ptr_type(members[i].first)) {
				ms << fmt_str("m_%s->to_string().c_str()", members[i].second.c_str());
			} else if (members[i].first == "std::any") {
				ms << fmt_str("\"std::any object\"");
			} else {
				ms << fmt_str("m_%s.to_string().c_str()", members[i].second.c_str());
			}
			if (i != members.size() - 1) {
				ms << ", ";
			}
		}
		ms << ");\n";
		ms << "}\n\n";

		// Getters
		for (const auto& member : members) {
			ms << fmt_str("template <typename R>\n");
			ms << fmt_str("const %s& %s<R>::get_%s() const\n", extract_type(member.first).c_str(), class_name.c_str(),
				member.second.c_str());
			ms << "{\n";
			if (is_ptr_type(member.first)) {
				ms << fmt_str("	return *m_%s;\n", member.second.c_str());
			} else {
				ms << fmt_str("	return m_%s;\n", member.second.c_str());
			}
			ms << "}\n\n";
		}

		// Accept method
		ms << fmt_str("template <typename R>\n");
		ms << fmt_str("R %s<R>::accept(Visitor<R>& visitor)\n", class_name.c_str());
		ms << "{\n";
		ms << fmt_str("	return visitor.visit_%s_expr(*this);\n", tolower(class_name).c_str());
		ms << "}\n\n";
	}

	ms.close();
	std::cout << "Done." << std::endl;

	return 0;
}
