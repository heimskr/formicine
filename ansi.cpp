#include <iostream>
#include <string>

#include "ansi.h"

using std::string;

namespace ansi {
	color_pair bg(ansi::color color) {
		return {color, BG};
	}

	string ansistream::get_text(const ansi::color &color) {
		return "\e[3" + color_bases.at(color) + "m";
	}

	string ansistream::get_bg(const ansi::color &color) {
		return "\e[4" + color_bases.at(color) + "m";
	}

	ansistream & ansistream::operator<<(const ansi::color &c) {
		text_color = c;
		style_out << get_text(c);
		return *this;
	}

	ansistream & ansistream::operator<<(const ansi::color_pair &p) {
		if (p.type == BG) {
			style_out << get_bg(bg_color = p.color);
		} else {
			style_out << get_text(text_color = p.color);
		}

		return *this;
	}

	ansistream & ansistream::operator<<(const ansi::style &style) {
		styles.insert(style);
		style_out << style_codes.at(style);
		return *this;
	}

	ansistream & ansistream::operator>>(const ansi::style &style) {
		styles.erase(style);
		style_out << style_resets.at(style);
		return *this;
	}

	ansistream & ansistream::operator<<(std::ostream & (*fn)(std::ostream &)) {
		fn(content_out);
		return *this;
	}

	ansistream & ansistream::operator<<(std::ostream & (*fn)(std::ios &)) {
		fn(content_out);
		return *this;
	}

	ansistream & ansistream::operator<<(std::ostream & (*fn)(std::ios_base &)) {
		fn(content_out);
		return *this;
	}

	ansistream & ansistream::operator<<(const char *t) { content_out << t; return *this; }

	template <typename T>
	ansistream & ansistream::operator<<(const T &value) {
		content_out << value;
		return *this;
	}
}
