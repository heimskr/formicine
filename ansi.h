#ifndef ANSI_H_
#define ANSI_H_

#include <iostream>
#include <map>
#include <string>
#include <unordered_set>

using std::string;

namespace ansi {
	enum color {
		DEFAULT, RED, ORANGE, YELLOW, YEEN, GREEN, BLUE, CYAN, MAGENTA, PURPLE, BLACK, GRAY, LIGHTGRAY, WHITE, PINK, SKY
	};

	enum style {BOLD, DIM, ITALIC, UNDERLINE};
	enum color_type {TEXT, BG};

	struct color_pair {
		ansi::color color;
		ansi::color_type type;

		color_pair(ansi::color color, ansi::color_type type): color(color), type(type) { }
	};

	class ansistream {
		private:
			std::ostream &content_out;
			std::ostream &style_out;
			ansi::color text_color;
			ansi::color bg_color;
			std::unordered_set<ansi::style> styles;
			string get_text(const ansi::color &);
			string get_bg(const ansi::color &);

		public:
			ansistream(std::ostream &stream): content_out(stream), style_out(stream) { }
			ansistream(std::ostream &c, std::ostream &s): content_out(c), style_out(s) { }
			ansistream & operator<<(const ansi::color &);
			ansistream & operator<<(const ansi::color_pair &);
			ansistream & operator<<(const ansi::style &);
			ansistream & operator>>(const ansi::style &);
			template <typename T>
			ansistream & operator<<(const T &);
	};

	color_pair bg(ansi::color);

	const string reset    = "\e[0m";
	const string reset_fg = "\e[39m";
	const string reset_bg = "\e[49m";

	const std::map<color, string> color_bases = {
		{DEFAULT,   "9"},
		{RED,       "1"},
		{ORANGE,    "8;5;202"},
		{YELLOW,    "3"},
		{YEEN,      "8;5;112"},
		{GREEN,     "2"},
		{BLUE,      "4"},
		{CYAN,      "6"},
		{MAGENTA,   "5"},
		{PURPLE,    "8;5;56"},
		{BLACK,     "0"},
		{GRAY,      "8;5;8"},
		{LIGHTGRAY, "8;5;7"},
		{WHITE,     "7"},
		{PINK,      "8;5;219"},
		{SKY,       "8;5;153"},
	};

	const std::map<style, string> style_codes = {
		{BOLD,      "\e[1m"},
		{DIM,       "\e[2m"},
		{ITALIC,    "\e[3m"},
		{UNDERLINE, "\e[4m"},
	};

	const std::map<style, string> style_resets = {
		{BOLD,      "\e[22m"},
		{DIM,       "\e[22m"},
		{ITALIC,    "\e[23m"},
		{UNDERLINE, "\e[24m"},
	};
}

#endif
