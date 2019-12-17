#include <iostream>
#include <stdexcept>
#include <string>

#include "ansi.h"

namespace ansi {
	std::ofstream dbgout(".log", std::ofstream::app);
	ansistream dbgstream(dbgout, dbgout);
	ansistream out(std::cout, std::cerr);

	std::string format(const std::string &str) {
		std::string out;
		out.reserve(str.length() * 1.1);
		const size_t length = str.length();
		for (size_t i = 0; i < length; ++i) {
			const char ch = str[i];
			const size_t remaining = length - i - 1;
			if (ch != '^' || remaining == 0) {
				out.push_back(ch);
				continue;
			}

			const char next = str[++i];

			if (next == '^') {
				out += ch;
				continue;
			} else if (next == 'b') {
				out += style_codes.at(style::bold);
			} else if (next == 'd') {
				out += style_codes.at(style::dim);
			} else if (next == 'u') {
				out += style_codes.at(style::underline);
			} else if (next == 'i') {
				out += style_codes.at(style::italic);
			} else if (next == 'B') {
				out += style_resets.at(style::bold);
			} else if (next == 'D') {
				out += style_resets.at(style::dim);
			} else if (next == 'U') {
				out += style_resets.at(style::underline);
			} else if (next == 'I') {
				out += style_resets.at(style::italic);
			} else if (next == '0') {
				out += reset_all;
			} else if (next == '[' && 3 <= remaining) {
				const size_t close_pos = str.find(']', i + 2);
				std::string type = str.substr(i + 1, close_pos - (i + 1));
				bool bright = false;
				if (type.back() == '!') {
					bright = true;
					type.pop_back();
				}

				color col = get_color(type);

				if (type == "/f") {
					out += reset_fg;
				} else if (type == "/b") {
					out += reset_bg;
				} else if (type.front() == ':') {
					col = get_color(type.substr(1));
					if (col == color::normal && type != "normal")
						throw std::invalid_argument("Invalid format identifier");
					out += get_bg(col, bright);
				} else if (col == color::normal && type != "normal") {
					throw std::invalid_argument("Invalid format identifier");
				} else {
					out += get_fg(col, bright);
				}

				i = close_pos;
			} else {
				throw std::invalid_argument("Invalid next character in format");
			}
		}

		return out;
	}

	color_pair fg(ansi::color color) { return {color, color_type::foreground}; }
	color_pair bg(ansi::color color) { return {color, color_type::background}; }

	std::string get_fg(ansi::color color, bool bright) {
		const std::string &base = color_bases.at(color);
		if (bright && base.size() == 1)
			return "\e[9" + base + "m";
		return "\e[3" + base + "m";
	}

	std::string get_bg(ansi::color color, bool bright) {
		const std::string &base = color_bases.at(color);
		if (bright && base.size() == 1)
			return "\e[10" + base + "m";
		return "\e[4" + base + "m";
	}

	ansi_pair<style> remove(ansi::style style) {
		return {style, false};
	}

	std::string wrap(const std::string &str, const ansi::color_pair &pair) {
		return pair.left() + str + pair.right();
	}

	std::string wrap(const std::string &str, const ansi::color &color) {
		return wrap(str, color_pair(color));
	}

	std::string wrap(const std::string &str, const ansi::style &style) {
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

	ansi::color get_color(const std::string &name) {
		for (const auto &pair: color_names) {
			if (pair.second == name)
				return pair.first;
		}

		return ansi::color::normal;
	}

	bool has_color(const std::string &name) {
		for (const auto &pair: color_names) {
			if (pair.second == name)
				return true;
		}

		return false;
	}

	std::string strip(const std::string &str) {
		std::string out = "";
		size_t len = str.length();
		char c;
		for (size_t i = 0; i < len; ++i) {
			c = str[i];

			if (c == '\x1b') {
				// If there's only this \x1b character and the next, there's nothing left to do.
				if (i == len - 2) break;
				if (str[i + 1] == '[')
					for (i += 2; str[i] < 0x40 || 0x7e < str[i]; ++i);
			} else if (c == '^') {
				++i;
				if (i == len) break; // A ^ at the end does nothing.
				if (str[i] == '[')
					i = str.find(']', i);
			} else {
				out.push_back(c);
				continue;
			}
		}

		return out;
	}

	std::string substr(const std::string &str, size_t pos, size_t n) {
		const size_t start = get_pos(str, pos);
		if (n == std::string::npos)
			return str.substr(start);
		return str.substr(start, get_pos(str, pos + n) - start);
	}

	size_t length(const std::string &str) {
		size_t i = 0, length = str.length(), counted = 0;
		for (; i < length; ++i) {
			if (str[i] != '\x1b') {
				++counted;
			} else {
				if (i == length - 2) break;
				if (str[i + 1] == '[')
					for (i += 2; str[i] < 0x40 || 0x7e < str[i]; ++i);
			}
		}

		return counted;
	}

	std::string & erase(std::string &str, size_t pos, size_t len) {
		const size_t start = get_pos(str, pos);
		if (len == std::string::npos)
			return str.erase(start);
		return str.erase(start, get_pos(str, pos + len) - start);
	}

	size_t get_pos(const std::string &str, size_t old_pos) {
		if (old_pos == std::string::npos)
			return old_pos;
		const size_t length = str.length();
		size_t i, counted = 0;
		for (i = 0; i < length && counted != old_pos; ++i) {
			if (str[i] != '\x1b') {
				++counted;
			} else {
				if (i == length - 2) break;
				if (str[i + 1] == '[')
					for (i += 2; str[i] < 0x40 || 0x7e < str[i]; ++i);
			}
		}

		return i;
	}

	std::string bold(const std::string &str) {
		return wrap(str, style::bold);
	}

	std::string dim(const std::string &str) {
		return wrap(str, style::dim);
	}

	std::string underline(const std::string &str) {
		return wrap(str, style::underline);
	}

	std::string italic(const std::string &str) {
		return wrap(str, style::italic);
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
	MKCOLOR(verydark)
#undef MKCOLOR

	std::string color_pair::left() const {
		return type == color_type::background? get_bg(color) : get_fg(color);
	}

	std::string color_pair::right() const {
		return type == color_type::background? reset_bg : reset_fg;
	}

	ansistream::ansistream(): content_out(std::cout), style_out(std::cerr) {}


// Private instance methods


	ansistream & ansistream::left_paren() {
		if (parens_on) {
			*this << style::dim;
			FORMICINE_PRINT_CONTENT("(");
			*this >> style::dim;
		}

		return *this;
	}

	ansistream & ansistream::right_paren() {
		if (parens_on) {
			parens_on = false;
			*this << style::dim;
			FORMICINE_PRINT_CONTENT(")");
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
#ifndef FORMICINE_NOFLUSH
		content_out.flush();
		style_out.flush();
#endif
		return *this;
	}

	ansistream & ansistream::clear() {
		FORMICINE_PRINT_STYLE("\e[2J");
		return *this;
	}

	ansistream & ansistream::jump(int x, int y) {
		if (0 <= x && 0 <= y) {
#ifdef FORMICINE_PRINTF
			printf("\e[%d;%dH", y + 1, x + 1);
#else
			style_out << "\e[" << (y + 1) << ";" << (x + 1) << "H";
#endif
		} else if (0 <= x) {
#ifdef FORMICINE_PRINTF
			printf("\e[%dG", x + 1);
#else
			style_out << "\e[" << (x + 1) << "G";
#endif
		} else if (0 <= y) {
#ifdef FORMICINE_PRINTF
			printf("\e[999999A");
			if (0 < y) printf("\e[%dB", y);
#else
			style_out << "\e[999999A";
			if (0 < y) style_out << "\e[" << y << "B";
#endif
		} else {
			throw std::runtime_error("Invalid jump: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
		}

		return *this;
	}

	ansistream & ansistream::jump()        { jump(0, 0);             return *this; }
	ansistream & ansistream::save()        { FORMICINE_PRINT_STYLE("\e[s");    return *this; }
	ansistream & ansistream::restore()     { FORMICINE_PRINT_STYLE("\e[u");    return *this; }
	ansistream & ansistream::clear_line()  { FORMICINE_PRINT_STYLE("\e[2K");   return *this; }
	ansistream & ansistream::clear_left()  { FORMICINE_PRINT_STYLE("\e[1K");   return *this; }
	ansistream & ansistream::clear_right() { FORMICINE_PRINT_STYLE("\e[K");    return *this; }

	ansistream & ansistream::show() { FORMICINE_PRINT_STYLE("\e[?25h"); hidden = false; return *this; }
	ansistream & ansistream::hide() { FORMICINE_PRINT_STYLE("\e[?25l"); hidden = true;  return *this; }

	ansistream & ansistream::move(int n, char c) {
#ifdef FORMICINE_PRINTF
		if (n != 0) printf("\e[%d%c", n, c);
#else
		if (n != 0) style_out << "\e[" << std::to_string(n) << c;
#endif
		return *this;
	}

	ansistream & ansistream::up(int rows)    { return move(rows, 'A'); }
	ansistream & ansistream::down(int rows)  { return move(rows, 'B'); }
	ansistream & ansistream::right(int cols) { return move(cols, 'C'); }
	ansistream & ansistream::left(int cols)  { return move(cols, 'D'); }

	ansistream & ansistream::vpos(int y) {
		up(999999);
		if (y > 0)
			down(y);
		return *this;
	}

	ansistream & ansistream::hpos(int x) {
#ifdef FORMICINE_PRINTF
		printf("\e[%dH", x + 1);
#else
		style_out << "\e[" + std::to_string(x + 1) + "H";
#endif
		return *this;
	}

	ansistream & ansistream::scroll_up(int lines) {
#ifdef FORMICINE_PRINTF
		printf("\e[%dS", lines);
#else
		style_out << "\e[" + std::to_string(lines) + "S";
#endif
		return *this;
	}

	ansistream & ansistream::scroll_down(int lines) {
#ifdef FORMICINE_PRINTF
		printf("\e[%dT", lines);
#else
		style_out << "\e[" + std::to_string(lines) + "T";
#endif
		return *this;
	}

	ansistream & ansistream::delete_chars(int count) {
#ifdef FORMICINE_PRINTF
		printf("\e[%dP", count);
#else
		style_out << "\e[" + std::to_string(count) + "P";
#endif
		return *this;
	}

	ansistream & ansistream::set_origin()   { FORMICINE_PRINT_STYLE("\e[?6h"); origin_on = true;  return *this; }
	ansistream & ansistream::reset_origin() { FORMICINE_PRINT_STYLE("\e[?6l"); origin_on = false; return *this; }

	ansistream & ansistream::hmargins(int left, int right) {
#ifdef FORMICINE_PRINTF
		printf("\e[%d;%ds", left + 1, right + 1);
#else
		style_out << "\e[" + std::to_string(left + 1) + ";" + std::to_string(right + 1) + "s";
#endif
		return *this;
	}

	ansistream & ansistream::hmargins()         { FORMICINE_PRINT_STYLE("\e[s");    return *this; }
	ansistream & ansistream::enable_hmargins()  { FORMICINE_PRINT_STYLE("\e[?69h"); return *this; }
	ansistream & ansistream::disable_hmargins() { FORMICINE_PRINT_STYLE("\e[?69l"); return *this; }
	ansistream & ansistream::vmargins()         { FORMICINE_PRINT_STYLE("\e[r");    return *this; }

	ansistream & ansistream::vmargins(int top, int bottom) {
		const std::string top_str = top == -1? "" : std::to_string(top + 1);
		const std::string bottom_str = bottom == -1? "" : std::to_string(bottom + 1);
#ifdef FORMICINE_PRINTF
		printf("\e[%s;%sr", top_str.c_str(), bottom_str.c_str());
#else
		style_out << "\e[" + top_str + ";" + bottom_str + "r";
#endif
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

	ansistream & ansistream::reset_colors() { FORMICINE_PRINT_STYLE("\e[39;49m"); return *this; }


// Public operators


	ansistream & ansistream::operator<<(const ansi::color &c) {
		// Adds a text color: "as << red"
		fg_color = c;
		FORMICINE_PRINT_STYLE(get_fg(c).c_str());
		return *this;
	}

	ansistream & ansistream::operator<<(const ansi::color_pair &p) {
		// Adds a color, either text or background:
		// - "as << fg(red)"
		// - "as << bg(red)"

		if (p.type == color_type::background)
			FORMICINE_PRINT_STYLE(get_bg(bg_color = p.color).c_str());
		else
			FORMICINE_PRINT_STYLE(get_fg(fg_color = p.color).c_str());

		return *this;
	}

	ansistream & ansistream::operator<<(const ansi::style &style) {
		// Adds a style: "as << bold"
		styles.insert(style);
		FORMICINE_PRINT_STYLE(style_codes.at(style).c_str());
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

#ifndef FORMICINE_NOFLUSH
		this->content_out.flush();
		this->style_out.flush();
#endif
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
		FORMICINE_PRINT_CONTENT(t);
		right_paren();
		return *this;
	}

	ansistream & ansistream::operator>>(const ansi::style &style) {
		// Removes a style: "as >> bold"
		styles.erase(style);
		FORMICINE_PRINT_STYLE(style_resets.at(style).c_str());
		return *this;
	}

	const std::string reset_all = "\e[0m";
	const std::string reset_fg  = "\e[39m";
	const std::string reset_bg  = "\e[49m";

	const std::string str_check   = "\u2714";
	const std::string str_nope    = "\u2718";
	const std::string str_warning = "\u26a0\ufe0f";

	const std::map<color, std::string> color_names = {
		{color::normal,    "normal"},
		{color::red,       "red"},
		{color::orange,    "orange"},
		{color::yellow,    "yellow"},
		{color::yeen,      "yeen"},
		{color::green,     "green"},
		{color::blue,      "blue"},
		{color::cyan,      "cyan"},
		{color::magenta,   "magenta"},
		{color::purple,    "purple"},
		{color::black,     "black"},
		{color::gray,      "gray"},
		{color::lightgray, "light gray"},
		{color::white,     "white"},
		{color::pink,      "pink"},
		{color::sky,       "sky"},
		{color::verydark,  "verydark"},
		{color::blood,     "blood"},
		{color::brown,     "brown"},
	};

	const std::map<color, std::string> color_bases = {
		{color::normal,    "9"},
		{color::red,       "1"},
		{color::orange,    "8;5;202"},
		{color::yellow,    "3"},
		{color::yeen,      "8;5;112"},
		{color::green,     "2"},
		{color::blue,      "4"},
		{color::cyan,      "6"},
		{color::magenta,   "5"},
		{color::purple,    "8;5;56"},
		{color::black,     "0"},
		{color::gray,      "8;5;8"},
		{color::lightgray, "8;5;7"},
		{color::white,     "7"},
		{color::pink,      "8;5;219"},
		{color::sky,       "8;5;153"},
		{color::verydark,  "8;5;232"},
		{color::blood,     "8;5;52"},
		{color::brown,     "8;5;130"},
	};

	const std::map<style, std::string> style_codes = {
		{style::bold,      "\e[1m"},
		{style::dim,       "\e[2m"},
		{style::italic,    "\e[3m"},
		{style::underline, "\e[4m"},
		{style::inverse,   "\e[7m"},
	};

	const std::map<style, std::string> style_resets = {
		{style::bold,      "\e[22m"},
		{style::dim,       "\e[22m"},
		{style::italic,    "\e[23m"},
		{style::underline, "\e[24m"},
		{style::inverse,   "\e[27m"},
	};
}

std::string operator"" _b(const char *str, unsigned long) { return ansi::wrap(str, ansi::style::bold); }
std::string operator"" _d(const char *str, unsigned long) { return ansi::wrap(str, ansi::style::dim); }
std::string operator"" _i(const char *str, unsigned long) { return ansi::wrap(str, ansi::style::italic); }
std::string operator"" _u(const char *str, unsigned long) { return ansi::wrap(str, ansi::style::underline); }
std::string operator"" _bd(const char *str, unsigned long) {
	return ansi::wrap(ansi::wrap(str, ansi::style::bold), ansi::style::dim);
}
