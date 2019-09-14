#include <iostream>
#include <stdexcept>
#include <string>

#include "ansi.h"

using std::string;

namespace ansi {
	std::ofstream dbgout(".log", std::ofstream::app);
	ansistream dbgstream(dbgout, dbgout);
	ansistream out(std::cout, std::cerr);

	color_pair fg(ansi::color color) {
		return {color, color_type::foreground};
	}

	color_pair bg(ansi::color color) {
		return {color, color_type::background};
	}

	string get_fg(ansi::color color) {
		return "\e[3" + color_bases.at(color) + "m";
	}

	string get_bg(ansi::color color) {
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

	void write(std::ostream &os, const std::string &str) {
		os << str;
		os.flush();
	}

	void write(const std::string &str) {
		write(std::cout, str);
	}

	std::string get_name(ansi::color color) {
		return get_fg(color) + color_names.at(color) + reset_fg;
	}

	std::string strip(const std::string &str) {
		std::string out = "";
		size_t len = str.length();
		char c;
		for (size_t i = 0; i < len; ++i) {
			c = str[i];
			if (c != '\x1b') {
				out.push_back(c);
				continue;
			}

			for (; str[i] < 0x40 || 0x7e < str[i]; ++i);
		}

		return out;
	}

	std::string bold(const std::string &str) {
		return wrap(str, style::bold);
	}

	std::string dim(const std::string &str) {
		return wrap(str, style::dim);
	}

#define MKCOLOR(x) std::string x(const std::string &str) { return wrap(str, color::x); }
	MKCOLOR(red)
	MKCOLOR(orange)
	MKCOLOR(yellow)
	MKCOLOR(yeen)
	MKCOLOR(green)
	MKCOLOR(blue)
	MKCOLOR(cyan)
	MKCOLOR(magenta)
	MKCOLOR(purple)
	MKCOLOR(black)
	MKCOLOR(gray)
	MKCOLOR(lightgray)
	MKCOLOR(white)
	MKCOLOR(pink)
	MKCOLOR(sky)
#undef MKCOLOR

	string color_pair::left() const {
		return type == color_type::background? get_bg(color) : get_fg(color);
	}

	string color_pair::right() const {
		return type == color_type::background? reset_bg : reset_fg;
	}

	ansistream::ansistream(): content_out(std::cout), style_out(std::cerr) {}


// Private instance methods


	ansistream & ansistream::left_paren() {
		if (parens_on) {
			*this << style::dim;
			content_out << "(";
			*this >> style::dim;
		}

		return *this;
	}

	ansistream & ansistream::right_paren() {
		if (parens_on) {
			parens_on = false;
			*this << style::dim;
			content_out << ")";
			*this >> style::dim;
		}

		return *this;
	}


// Public static methods


	ansistream & ansistream::err() {
		static ansistream as(std::cerr, std::cerr);
		return as;
	}


// Public instance methods


	ansistream & ansistream::flush() {
		content_out.flush();
		style_out.flush();
		return *this;
	}

	ansistream & ansistream::clear() {
		style_out << "\e[2J";
		return *this;
	}

	ansistream & ansistream::jump(int x, int y) {
		if (0 <= x && 0 <= y) {
			style_out << "\e[" << (y + 1) << ";" << (x + 1) << "H";
		} else if (0 <= x) {
			style_out << "\e[" << (x + 1) << "G";
		} else if (0 <= y) {
			style_out << "\e[999999A";
			if (0 < y)
				style_out << "\e[" + std::to_string(y) + "B";
		} else {
			throw std::runtime_error("Invalid jump: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
		}

		return *this;
	}

	ansistream & ansistream::jump() {
		jump(0, 0);
		return *this;
	}

	ansistream & ansistream::save() {
		style_out << "\e[s";
		return *this;
	}

	ansistream & ansistream::restore() {
		style_out << "\e[u";
		return *this;
	}

	ansistream & ansistream::clear_line() {
		style_out << "\e[2K";
		return *this;
	}

	ansistream & ansistream::clear_left() {
		style_out << "\e[1K";
		return *this;
	}

	ansistream & ansistream::clear_right() {
		style_out << "\e[K";
		return *this;
	}

	ansistream & ansistream::up(int rows) {
		if (rows != 0)
			style_out << "\e[" + std::to_string(rows) + "A";
		return *this;
	}

	ansistream & ansistream::down(int rows) {
		if (rows != 0)
			style_out << "\e[" + std::to_string(rows) + "B";
		return *this;
	}

	ansistream & ansistream::right(int cols) {
		if (cols != 0)
			style_out << "\e[" + std::to_string(cols) + "C";
		return *this;
	}

	ansistream & ansistream::left(int cols) {
		if (cols != 0)
			style_out << "\e[" + std::to_string(cols) + "D";
		return *this;
	}

	ansistream & ansistream::vpos(int y) {
		up(999999);
		if (y > 0)
			down(y);
		return *this;
	}

	ansistream & ansistream::hpos(int x) {
		style_out << "\e[" + std::to_string(x + 1) + "H";
		return *this;
	}

	ansistream & ansistream::scroll_up(int lines) {
		style_out << "\e[" + std::to_string(lines) + "S";
		return *this;
	}

	ansistream & ansistream::scroll_down(int lines) {
		style_out << "\e[" + std::to_string(lines) + "T";
		return *this;
	}

	ansistream & ansistream::delete_chars(int count) { // DCH
		style_out << "\e[" + std::to_string(count) + "P";
		return *this;
	}

	ansistream & ansistream::set_origin() {
		style_out << "\e[?6h";
		return *this;
	}

	ansistream & ansistream::reset_origin() {
		style_out << "\e[?6l";
		return *this;
	}

	ansistream & ansistream::hmargins(int left, int right) {
		style_out << "\e[" + std::to_string(left + 1) + ";" + std::to_string(right + 1) + "s";
		return *this;
	}

	ansistream & ansistream::hmargins() {
		style_out << "\e[s";
		return *this;
	}

	ansistream & ansistream::enable_hmargins() {
		style_out << "\e[?69h";
		return *this;
	}

	ansistream & ansistream::disable_hmargins() {
		style_out << "\e[?69l";
		return *this;
	}

	ansistream & ansistream::vmargins(int top, int bottom) {
		const std::string top_str = top == -1? "" : std::to_string(top + 1);
		const std::string bottom_str = bottom == -1? "" : std::to_string(bottom + 1);
		style_out << "\e[" + top_str + ";" + bottom_str + "r";
		return *this;
	}

	ansistream & ansistream::vmargins() {
		style_out << "\e[r";
		return *this;
	}

	ansistream & ansistream::margins(int top, int bottom, int left, int right) {
		vmargins(top, bottom);
		hmargins(left, right);
		return *this;
	}

	ansistream & ansistream::margins() {
		hmargins();
		vmargins();
		return *this;
	}

	ansistream & ansistream::reset_colors() {
		style_out << "\e[39;49m";
		return *this;
	}


// Public operators


	ansistream & ansistream::operator<<(const ansi::color &c) {
		// Adds a text color: "as << red"
		fg_color = c;
		style_out << get_fg(c);
		return *this;
	}

	ansistream & ansistream::operator<<(const ansi::color_pair &p) {
		// Adds a color, either text or background:
		// - "as << fg(red)"
		// - "as << bg(red)"

		if (p.type == color_type::background)
			style_out << get_bg(bg_color = p.color);
		else
			style_out << get_fg(fg_color = p.color);

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
		if (pair.add)
			return *this << pair.value;
		return *this >> pair.value;
	}

	ansistream & ansistream::operator<<(const ansi::action &action) {
		// Performs an action on the stream: "as << reset"
		switch (action) {
			case action::end_line:      *this << "\e[0m" << std::endl; break;
			case action::reset:         *this << reset_all; break;
			case action::check:         *this << "["_d << wrap(str_check, color::green)  << "] "_d; break;
			case action::nope:          *this << "["_d << wrap(str_nope,  color::red)    << "] "_d; break;
			case action::warning:       *this << "["_d << wrap("~",       color::yellow) << "] "_d; break;
			case action::information:   *this << "["_d << wrap("i",       color::blue)   << "] "_d; break;
			case action::open_paren:    *this << wrap("(", style::dim); break;
			case action::close_paren:   *this << wrap(")", style::dim); break;
			case action::enable_parens: parens_on = true; break;
			default:
				throw std::invalid_argument("Invalid action: " + std::to_string(static_cast<int>(action)));
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
		left_paren();
		content_out << t;
		right_paren();
		return *this;
	}

	ansistream & ansistream::operator>>(const ansi::style &style) {
		// Removes a style: "as >> bold"
		styles.erase(style);
		style_out << style_resets.at(style);
		return *this;
	}
}

std::string operator"" _b(const char *str, unsigned long) { return ansi::wrap(str, ansi::style::bold); }
std::string operator"" _d(const char *str, unsigned long) { return ansi::wrap(str, ansi::style::dim); }
std::string operator"" _i(const char *str, unsigned long) { return ansi::wrap(str, ansi::style::italic); }
std::string operator"" _u(const char *str, unsigned long) { return ansi::wrap(str, ansi::style::underline); }
std::string operator"" _bd(const char *str, unsigned long) {
	return ansi::wrap(ansi::wrap(str, ansi::style::bold), ansi::style::dim);
}
