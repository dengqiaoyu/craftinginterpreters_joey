#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <any>
#include <string>
#include <unordered_map>

#include "token.h"

class Environment
{

public:
	Environment();

	void define(const Token& name, const std::any& value);
	void assign(const Token& name, const std::any& value);

	std::any operator[](const Token& name);
	const std::any& operator[](const Token& name) const;

private:
	std::unordered_map<std::string, std::any> m_values;
};

#endif // ENVIRONMENT_H
