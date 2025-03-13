#include "ast_generator.h"

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

static std::string toupper(const std::string& str)
{
	std::string upper_str(str);
	std::transform(upper_str.begin(), upper_str.end(), upper_str.begin(),
		[](unsigned char c) { return std::toupper(c); });
	return upper_str;
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
	require_action_return_value(argc == 2, std::cout << "Usage: ast_generator <output directory path>" << std::endl,
		EINVAL);
	const std::string output_dir_path(argv[1]);

	// Create output directory.
	std::cout << std::format("Creating output directory: {}", output_dir_path) << std::endl;
	std::filesystem::create_directories(output_dir_path);

	const std::string base_class_name = "Expr";

	// clang-format off
	const std::vector<ASTClass> ast_classes{
		ASTClass("Binary",
			{
				{"std::shared_ptr<Expr>",	"left"},
				{"Token",					"opr"},
				{"std::shared_ptr<Expr>",	"right"}
			}
		),
		ASTClass("Grouping",
			{
				{"std::shared_ptr<Expr>",	"expr"}
			}
		),
		ASTClass("Literal",
			{
				{"std::any", 				"value"}
			}
		),
		ASTClass("Unary",
			{
				{"Token",					"opr"},
				{"std::shared_ptr<Expr>",	"right"}
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

	hs << fmt_str("#ifndef %s_H\n", toupper(base_class_name).c_str());
	hs << fmt_str("#define %s_H\n", toupper(base_class_name).c_str());
	hs << "\n";
	hs << fmt_str("#include <memory>\n");
	hs << fmt_str("#include <string>\n");
	hs << "\n";
	hs << fmt_str("#include \"token.h\"\n");
	hs << "\n";
	hs << fmt_str("class %s\n", bcls_n);
	hs << fmt_str("{\n");
	hs << fmt_str("public:\n");
	hs << fmt_str("	%s() = default;\n", bcls_n);
	hs << fmt_str("	%s(const %s&) = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	%s& operator=(const %s&) = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	%s(%s&&) noexcept = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	%s& operator=(%s&&) noexcept = default;\n", bcls_n, bcls_n);
	hs << fmt_str("	virtual ~%s();\n", bcls_n);
	hs << fmt_str("	[[nodiscard]] virtual std::string to_string() const = 0;\n");
	hs << "\n";
	hs << fmt_str("	friend std::ostream& operator<<(std::ostream& out_s, const %s& expr)\n", bcls_n);
	hs << fmt_str("	{\n");
	hs << fmt_str("		return out_s << expr.to_string();\n");
	hs << fmt_str("	}\n");
	hs << fmt_str("};\n");
	hs << "\n";
	hs << fmt_str("template <>\n");
	hs << fmt_str("struct std::formatter<%s> : std::formatter<std::string> { // NOLINT(altera-struct-pack-align)\n",
		bcls_n);
	hs << fmt_str("	auto format(const %s& expr, format_context& ctx) const\n", bcls_n);
	hs << fmt_str("	{\n");
	hs << fmt_str("		return std::formatter<std::string>::format(expr.to_string(), ctx);\n");
	hs << fmt_str("	}\n");
	hs << fmt_str("};\n");
	hs << "\n";
	for (const auto& ast_class : ast_classes) {
		const std::string& class_name = ast_class.get_class_name();
		const std::vector<std::pair<std::string, std::string>>& members = ast_class.get_members();

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
		hs << fmt_str("	[[nodiscard]] std::string to_string() const override;\n");
		hs << "\n";
		for (const auto& member : members) {
			hs << fmt_str("	[[nodiscard]] const %s& get_%s() const;\n", extract_type(member.first).c_str(),
				member.second.c_str());
		}
		hs << "\n";
		hs << fmt_str("private:\n");
		for (const auto& member : members) {
			hs << fmt_str("	%s m_%s;\n", member.first.c_str(), member.second.c_str());
		}
		hs << fmt_str("};\n");
		hs << "\n";
	}
	hs << fmt_str("#endif // %s_H\n", toupper(base_class_name).c_str());
	hs.close();
	std::cout << "Done." << std::endl;

	// Generate source file.

	const std::string source_file = output_dir_path + "/" + tolower(base_class_name) + ".cpp";

	// Delete existing source file if it exists.
	if (std::filesystem::exists(source_file)) {
		std::cout << "Deleting existing " << source_file << "..." << std::endl;
		std::filesystem::remove(source_file);
	}

	std::cout << "Generating " << source_file << "..." << std::endl;
	std::ofstream cs(source_file);
	require_action_return_value(cs.is_open(), std::cout << "Failed to open: " << source_file << std::endl, EINVAL);

	cs << fmt_str("#include \"%s\"\n", header_file_name.c_str());
	cs << "\n";
	cs << fmt_str("#include <string>\n");
	cs << "\n";
	cs << fmt_str("#include \"general.h\"\n");
	cs << "\n";
	// clang-format off
	cs << "// =====================================================================================================================\n";
	// clang-format on
	cs << fmt_str("// %s\n", bcls_n);
	cs << "\n";
	cs << fmt_str("%s::~%s() = default;\n", bcls_n, bcls_n);
	cs << "\n";
	for (const auto& ast_class : ast_classes) {
		const std::string& class_name = ast_class.get_class_name();
		const std::vector<std::pair<std::string, std::string>>& members = ast_class.get_members();
		// clang-format off
		cs << "// =====================================================================================================================\n";
		// clang-format on
		cs << fmt_str("// %s\n", class_name.c_str());
		cs << "\n";
		cs << fmt_str("%s::%s(", class_name.c_str(), class_name.c_str());
		for (size_t i = 0; i < members.size(); ++i) {
			cs << fmt_str("%s %s", members[i].first.c_str(), members[i].second.c_str());
			if (i != members.size() - 1) {
				cs << ", ";
			}
		}
		cs << ") : ";
		for (size_t i = 0; i < members.size(); ++i) {
			cs << fmt_str("m_%s(std::move(%s))", members[i].second.c_str(), members[i].second.c_str());
			if (i != members.size() - 1) {
				cs << ", ";
			}
		}
		cs << "\n";
		cs << "{\n";
		cs << fmt_str("	// Empty constructor.\n");
		cs << "}\n";
		cs << "\n";
		cs << fmt_str("std::string %s::to_string() const\n", class_name.c_str());
		cs << "{\n";
		cs << fmt_str("	return fmt_str(\"%s Expr{", class_name.c_str());
		for (size_t i = 0; i < members.size(); ++i) {
			cs << fmt_str("%s=%%s", members[i].second.c_str());
			if (i != members.size() - 1) {
				cs << ", ";
			}
		}
		cs << "}\", ";
		for (size_t i = 0; i < members.size(); ++i) {
			if (is_ptr_type(members[i].first)) {
				cs << fmt_str("m_%s->to_string().c_str()", members[i].second.c_str());
			} else if (members[i].first == "std::any") {
				cs << fmt_str("\"std::any object\"");
			} else {
				cs << fmt_str("m_%s.to_string().c_str()", members[i].second.c_str());
			}
			if (i != members.size() - 1) {
				cs << ", ";
			}
		}
		cs << ");\n";
		cs << "}\n";
		cs << "\n";
		for (const auto& member : members) {
			cs << fmt_str("const %s& %s::get_%s() const\n", extract_type(member.first).c_str(), class_name.c_str(),
				member.second.c_str());
			cs << "{\n";
			if (is_ptr_type(member.first)) {
				cs << fmt_str("	return *m_%s;\n", member.second.c_str());
			} else {
				cs << fmt_str("	return m_%s;\n", member.second.c_str());
			}
			cs << "}\n";
			cs << "\n";
		}
	}
	cs.close();
	std::cout << "Done." << std::endl;

	return 0;
}
