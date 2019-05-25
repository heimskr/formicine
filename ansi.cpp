#include <iostream>
#include <stdexcept>
#include <string>

#include "ansi.h"

using std::string;

namespace ansi {
	ansistream out = {std::cout, std::cerr};

	color_pair fg(ansi::color color) {
		return {color, text};
	}

	color_pair bg(ansi::color color) {
		return {color, background};
	}

	string get_text(const ansi::color &color) {
		return "\e[3" + color_bases.at(color) + "m";
	}

	string get_bg(const ansi::color &color) {
		return "\e[4" + color_bases.at(color) + "m";
	}

	ansi_pair<style> remove(ansi::style style) {
		return {style, false};
	}

	string wrap(const string &str, const ansi::color_pair &pair) {
		return pair.left() + str + pair.right();
	}

	string wrap(const string &str, const ansi::color &color) {
		return wrap(str, color_pair(color));
	}

	string wrap(const string &str, const ansi::style &style) {
		return style_codes.at(style) + str + style_resets.at(style);
	}

	string color_pair::left() const {
		return type == background? get_bg(color) : get_text(color);
	}

	string color_pair::right() const {
		return type == background? reset_bg : reset_fg;
	}

	ansistream::ansistream(): content_out(std::cout), style_out(std::cerr) {}

	ansistream & ansistream::operator<<(const ansi::color &c) {
		// Adds a text color: "as << red"
		text_color = c;
		style_out << get_text(c);
		return *this;
	}

	ansistream & ansistream::operator<<(const ansi::color_pair &p) {
		// Adds a color, either text or background:
		// - "as << fg(red)"
		// - "as << bg(red)"

		if (p.type == background) {
			style_out << get_bg(bg_color = p.color);
		} else {
			style_out << get_text(text_color = p.color);
		}

		return *this;
	}

	ansistream & ansistream::operator<<(const ansi::style &style) {
		// Adds a style: "as << bold"
		styles.insert(style);
		style_out << style_codes.at(style);
		return *this;
	}

	ansistream & ansistream::operator<<(const ansi::ansi_pair<ansi::style> &pair) {
		// Removes a style: "as << remove(bold)"
		if (pair.add) {
			return *this << pair.value;
		}

		return *this >> pair.value;
	}

	ansistream & ansistream::operator<<(const ansi::action &action) {
		// Performs an action on the stream: "as << reset"
		if (action == reset) {
			return *this << reset_all;
		}

		throw std::invalid_argument("Invalid action");
	}

	// These next three are for manipulator support.

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

	ansistream & ansistream::operator>>(const ansi::style &style) {
		// Removes a style: "as >> bold"
		styles.erase(style);
		style_out << style_resets.at(style);
		return *this;
	}

}

std::string operator"" _b(const char *str, unsigned long) { return ansi::wrap(str, ansi::bold); }
std::string operator"" _d(const char *str, unsigned long) { return ansi::wrap(str, ansi::dim); }
std::string operator"" _i(const char *str, unsigned long) { return ansi::wrap(str, ansi::italic); }
std::string operator"" _u(const char *str, unsigned long) { return ansi::wrap(str, ansi::underline); }
std::string operator"" _bd(const char *str, unsigned long) { return ansi::wrap(ansi::wrap(str, ansi::bold), ansi::dim); }
