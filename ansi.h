#ifndef ANSI_H_
#define ANSI_H_

#include <iostream>
#include <map>
#include <string>

using std::string;

namespace ansi {
	enum color {
		RED, ORANGE, YELLOW, YEEN, GREEN, BLUE, CYAN, MAGENTA, PURPLE, BLACK, GRAY, LIGHTGRAY, WHITE, PINK, SKY
	};

	enum style {BOLD, DIM, ITALIC, UNDERLINE};

	class ansistream {
		private:
			std::ostream &out;
			ansi::color color;

		public:
			ansistream(std::ostream &out_): out(out_) { }
			ansistream & operator<<(const ansi::color c);
			ansistream & operator<<(const string &str);
	};

	const string reset        = "\e[0m";
	const string reset_fg     = "\e[39m";
	const string reset_bg     = "\e[49m";
	const string reset_bold   = "\e[22m";
	const string reset_dim    = "\e[22m";
	const string reset_italic = "\e[23m";
	const string reset_under  = "\e[24m";

	const string bold   = "\e[1m";
	const string dim    = "\e[2m";
	const string italic = "\e[3m";
	const string under  = "\e[4m";

	const std::map<color, string> colors = {
		{RED,       "\e[31m"},
		{ORANGE,    "\e[38;5;202m"},
		{YELLOW,    "\e[33m"},
		{YEEN,      "\e[38;5;112m"},
		{GREEN,     "\e[32m"},
		{BLUE,      "\e[34m"},
		{CYAN,      "\e[36m"},
		{MAGENTA,   "\e[35m"},
		{PURPLE,    "\e[38;5;56m"},
		{BLACK,     "\e[30m"},
		{GRAY,      "\e[38;5;8m"},
		{LIGHTGRAY, "\e[38;5;7m"},
		{WHITE,     "\e[37m"},
		{PINK,      "\e[38;5;219m"},
		{SKY,       "\e[38;5;153m"},
	};
}

#endif
