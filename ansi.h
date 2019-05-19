#ifndef ANSI_H_
#define ANSI_H_

#include <iostream>
#include <map>
#include <string>
#include <unordered_set>

using std::string;

namespace ansi {
	enum color {
		normal, red, orange, yellow, yeen, green, blue, cyan, magenta, purple, black, gray, lightgray, white, pink, sky
	};

	enum style {bold, dim, italic, underline};
	enum color_type {text, background};
	enum action {reset};

	template <typename T>
	struct ansi_pair {
		T value;
		bool add;
		ansi_pair(T value, bool add): value(value), add(add) { }
	};

	struct color_pair {
		ansi::color color;
		ansi::color_type type;

		color_pair(ansi::color color, ansi::color_type type): color(color), type(type) { }
		color_pair(ansi::color color): color(color), type(text) { }
	};

	class ansistream {
		private:
			std::ostream &content_out;
			std::ostream &style_out;
			ansi::color text_color;
			ansi::color bg_color;
			std::unordered_set<ansi::style> styles;

		public:
			ansistream(std::ostream &stream): content_out(stream), style_out(stream) { }
			ansistream(std::ostream &c, std::ostream &s): content_out(c), style_out(s) { }
			ansistream & operator<<(const ansi::color &);
			ansistream & operator<<(const ansi::color_pair &);
			ansistream & operator<<(const ansi::style &);
			ansistream & operator<<(const ansi::ansi_pair<ansi::style> &);
			ansistream & operator<<(const ansi::action &);
			ansistream & operator<<(std::ostream & (*fn)(std::ostream &));
			ansistream & operator<<(std::ostream & (*fn)(std::ios &));
			ansistream & operator<<(std::ostream & (*fn)(std::ios_base &));
			ansistream & operator<<(const char *t);
			template <typename T>
			ansistream & operator<<(const T &);

			ansistream & operator>>(const ansi::style &);
	};

	string get_text(const ansi::color &);
	string get_bg(const ansi::color &);
	color_pair fg(ansi::color color);
	color_pair bg(ansi::color color);
	ansi_pair<ansi::style> remove(ansi::style);

	const string reset_all = "\e[0m";
	const string reset_fg  = "\e[39m";
	const string reset_bg  = "\e[49m";

	const std::map<color, string> color_bases = {
		{normal,    "9"},
		{red,       "1"},
		{orange,    "8;5;202"},
		{yellow,    "3"},
		{yeen,      "8;5;112"},
		{green,     "2"},
		{blue,      "4"},
		{cyan,      "6"},
		{magenta,   "5"},
		{purple,    "8;5;56"},
		{black,     "0"},
		{gray,      "8;5;8"},
		{lightgray, "8;5;7"},
		{white,     "7"},
		{pink,      "8;5;219"},
		{sky,       "8;5;153"},
	};

	const std::map<style, string> style_codes = {
		{bold,      "\e[1m"},
		{dim,       "\e[2m"},
		{italic,    "\e[3m"},
		{underline, "\e[4m"},
	};

	const std::map<style, string> style_resets = {
		{bold,      "\e[22m"},
		{dim,       "\e[22m"},
		{italic,    "\e[23m"},
		{underline, "\e[24m"},
	};
}

#endif
