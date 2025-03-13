#ifndef AST_GENERATOR_H
#define AST_GENERATOR_H

#include <string>
#include <utility>
#include <vector>

class ASTClass
{
public:
	ASTClass(std::string class_name, std::vector<std::pair<std::string, std::string>> members);
	[[nodiscard]] const std::string& get_class_name() const;
	[[nodiscard]] const std::vector<std::pair<std::string, std::string>>& get_members() const;

private:
	std::string m_class_name;
	std::vector<std::pair<std::string, std::string>> m_members;
};

#endif // AST_GENERATOR_H
