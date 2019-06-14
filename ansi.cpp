#include <iostream>
#include <stdexcept>
#include <string>

#include "ansi.h"

using std::string;

namespace formicine {
	ansistream ansi::out = {std::cout, std::cerr};

	action ansi::endl   = action::end_line;
	action ansi::good   = action::check;
	action ansi::bad    = action::nope;
	action ansi::warn   = action::warning;
	action ansi::oparen = action::open_paren;
	action ansi::cparen = action::close_paren;
	action ansi::parens = action::enable_parens;
	std::string ansi::reset_all   = "\e[0m";
	std::string ansi::reset_fg    = "\e[39m";
	std::string ansi::reset_bg    = "\e[49m";
	std::string ansi::str_check   = "\u2714";
	std::string ansi::str_nope    = "\u2718";
	std::string ansi::str_warning = "\u26a0\ufe0f";

	std::map<color, std::string> ansi::color_bases = {
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

	std::map<style, std::string> ansi::style_codes = {
		{bold,      "\e[1m"},
		{dim,       "\e[2m"},
		{italic,    "\e[3m"},
		{underline, "\e[4m"},
	};

	std::map<style, std::string> ansi::style_resets = {
		{bold,      "\e[22m"},
		{dim,       "\e[22m"},
		{italic,    "\e[23m"},
		{underline, "\e[24m"},
	};

	color_pair ansi::fg(color color) {
		return {color, text};
	}

	color_pair ansi::bg(color color) {
		return {color, background};
	}

	string ansi::get_text(const color &color) {
		return "\e[3" + ansi::color_bases.at(color) + "m";
	}

	string ansi::get_bg(const color &color) {
		return "\e[4" + ansi::color_bases.at(color) + "m";
	}

	ansi_pair<style> ansi::remove(style style) {
		return {style, false};
	}

	string ansi::wrap(const string &str, const color_pair &pair) {
		return pair.left() + str + pair.right();
	}

	string ansi::wrap(const string &str, const color &color) {
		return wrap(str, color_pair(color));
	}

	string ansi::wrap(const string &str, const style &style) {
		return ansi::style_codes.at(style) + str + ansi::style_resets.at(style);
	}

	void ansi::write(const std::string &str) {
		std::cout << str;
		std::cout.flush();
	}

	void ansi::clear() {
		write("\e[2J");
	}

	void ansi::jump(int x, int y) {
		std::cout << "\e[" << (y + 1) << ";" << (x + 1) << "H";
		std::cout.flush();
	}

	void ansi::jump() {
		jump(0, 0);
	}

	void ansi::save() {
		write("\e[s");
	}

	void ansi::restore() {
		write("\e[u");
	}

	void ansi::clear_line() {
		write("\e[2K");
	}

	void ansi::clear_left() {
		write("\e[1K");
	}

	void ansi::clear_right() {
		write("\e[K");
	}

	void ansi::up(size_t rows) {
		if (rows != 0)
			write("\e[" + std::to_string(rows) + "A");
	}

	void ansi::down(size_t rows) {
		if (rows != 0)
			write("\e[" + std::to_string(rows) + "B");
	}

	void ansi::right(size_t cols) {
		if (cols != 0)
			write("\e[" + std::to_string(cols) + "C");
	}

	void ansi::left(size_t cols) {
		if (cols != 0)
			write("\e[" + std::to_string(cols) + "D");
	}

	string color_pair::left() const {
		return type == background? ansi::get_bg(color) : ansi::get_text(color);
	}

	string color_pair::right() const {
		return type == background? ansi::reset_bg : ansi::reset_fg;
	}

	ansistream::ansistream(): content_out(std::cout), style_out(std::cerr) {}

	void ansistream::left() {
		if (parens_on) {
			*this << dim;
			content_out << "(";
			*this >> dim;
		}
	}

	void ansistream::right() {
		if (parens_on) {
			parens_on = false;
			*this << dim;
			content_out << ")";
			*this >> dim;
		}
	}

	void ansistream::flush() {
		content_out.flush();
		style_out.flush();
	}

	ansistream & ansistream::operator<<(const color &c) {
		// Adds a text color: "as << red"
		text_color = c;
		style_out << ansi::get_text(c);
		return *this;
	}

	ansistream & ansistream::operator<<(const color_pair &p) {
		// Adds a color, either text or background:
		// - "as << fg(red)"
		// - "as << bg(red)"

		if (p.type == background)
			style_out << ansi::get_bg(bg_color = p.color);
		else
			style_out << ansi::get_text(text_color = p.color);

		return *this;
	}

	ansistream & ansistream::operator<<(const style &style) {
		// Adds a style: "as << bold"
		styles.insert(style);
		style_out << ansi::style_codes.at(style);
		return *this;
	}

	ansistream & ansistream::operator<<(const ansi_pair<style> &pair) {
		// Removes a style: "as << remove(bold)"
		if (pair.add)
			return *this << pair.value;
		return *this >> pair.value;
	}

	ansistream & ansistream::operator<<(const action &action) {
		// Performs an action on the stream: "as << reset"
		switch (action) {
			case end_line:      *this << "\e[0m" << std::endl; break;
			case reset:         *this << ansi::reset_all; break;
			case check:         *this << "["_d << ansi::wrap(ansi::str_check, green) << "] "_d; break;
			case nope:          *this << "["_d << ansi::wrap(ansi::str_nope,  red)   << "] "_d; break;
			case warning:       *this << "["_d << ansi::wrap("~", yellow) << "] "_d; break;
			case open_paren:    *this << ansi::wrap("(", dim); break;
			case close_paren:   *this << ansi::wrap(")", dim); break;
			case enable_parens: parens_on = true; break;
			default:
				throw std::invalid_argument("Invalid action: " + std::to_string(action));
		}

		this->content_out.flush();
		this->style_out.flush();
		return *this;
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

	ansistream & ansistream::operator<<(const char *t) {
		left();
		content_out << t;
		right();
		return *this;
	}

	ansistream & ansistream::operator>>(const style &style) {
		// Removes a style: "as >> bold"
		styles.erase(style);
		style_out << ansi::style_resets.at(style);
		return *this;
	}
}

std::string operator"" _b(const char *str, unsigned long) {
	return formicine::ansi::wrap(str, formicine::bold);
}

std::string operator"" _d(const char *str, unsigned long) {
	return formicine::ansi::wrap(str, formicine::dim);
}

std::string operator"" _i(const char *str, unsigned long) {
	return formicine::ansi::wrap(str, formicine::italic);
}

std::string operator"" _u(const char *str, unsigned long) {
	return formicine::ansi::wrap(str, formicine::underline);
}

std::string operator"" _bd(const char *str, unsigned long) {
	return formicine::ansi::wrap(formicine::ansi::wrap(str, formicine::bold), formicine::dim);
}
