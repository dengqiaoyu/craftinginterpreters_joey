#ifndef LOX_READLINE_H
#define LOX_READLINE_H

#include <string>

class LoxReadline
{
public:
	explicit LoxReadline(std::string history_path = "/tmp/lox_history.txt");
	~LoxReadline();
	LoxReadline(const LoxReadline&) = delete;
	LoxReadline& operator=(const LoxReadline&) = delete;
	LoxReadline(LoxReadline&&) = delete;
	LoxReadline& operator=(LoxReadline&&) = delete;

	static std::pair<std::string, bool> read_line(const std::string& prompt);

private:
	const std::string m_history_path;

	void save_history();
};

#endif
