#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <any>
#include <string>
#include <unordered_map>

#include "token.h"

class Environment
{

public:
	explicit Environment(Environment* enclosing = nullptr);

	void define(const Token& name, const std::any& value);
	void assign(const Token& name, const std::any& value);

	std::any get(const Token& name);

private:
	Environment* m_enclosing;
	std::unordered_map<std::string, std::any> m_values;
};

#endif // ENVIRONMENT_H
