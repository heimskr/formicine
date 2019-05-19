#ifndef ANSI_H_
#define ANSI_H_

#include <iostream>
#include <map>
#include <string>

namespace ansi {
	enum color {RED, ORANGE, YELLOW, GREEN, BLUE, CYAN, MAGENTA, PURPLE, BLACK, GRAY, LIGHTGRAY, WHITE};

	class ansistream {

		private:
			std::ostream &out;
			ansi::color color;

		public:
			ansistream(std::ostream &out_): out(out_) { }
			ansistream & operator<<(const ansi::color c);
			ansistream & operator<<(const std::string &str);
	};

	const std::string reset_fg = "\x1b[39m";
	const std::map<color, std::string> colors = {
		{ORANGE,    "\x1b[38;5;202m"},
		{BLACK,     "\x1b[30m"},
		{RED,       "\x1b[31m"},
		{GREEN,     "\x1b[32m"},
		{YELLOW,    "\x1b[33m"},
		{BLUE,      "\x1b[34m"},
		{MAGENTA,   "\x1b[35m"},
		{CYAN,      "\x1b[36m"},
		{WHITE,     "\x1b[37m"},
		{LIGHTGRAY, "\x1b[38;5;7m"},
		{GRAY,      "\x1b[38;5;8m"},
	};
}

#endif
