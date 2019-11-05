#ifndef FORMICINE_ANSI_H_
#define FORMICINE_ANSI_H_

#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <unordered_set>

#ifdef NODEBUG
#define DBGX(x)
#define DBG(x)
#else
#define DBGX(x) "\e[2m[" << std::right << std::setw(25) << std::setfill(' ') << std::string(__FILE__).substr(0, 25) << \
	":" << std::setw(3) << __LINE__ << "]\e[0m " << x << std::endl
#define DBG(x) ansi::dbgstream << DBGX(x) << ansi::action::reset
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

namespace ansi {
	enum class color: int {
		normal, red, orange, yellow, yeen, green, blue, cyan, magenta, purple, black, gray, lightgray, white, pink, sky,
		verydark, blood, brown
	};

	enum class style {bold, dim, italic, underline, inverse};
	enum class color_type: int {background = 1, foreground = 2, both = 3};
	enum class action: int {reset, end_line, check, nope, warning, information, open_paren, close_paren, enable_parens};

	/** Replaces escapes that start with '^' with ANSI escapes. */
	std::string format(const std::string &str);

	template <typename T>
	struct ansi_pair {
		T value;
		bool add;
		ansi_pair(T value, bool add): value(value), add(add) {}
	};

	struct color_pair {
		ansi::color color;
		ansi::color_type type;

		color_pair(ansi::color color, ansi::color_type type): color(color), type(type) {}
		color_pair(ansi::color color): color(color), type(color_type::foreground) {}

		std::string left() const;
		std::string right() const;
	};

	class ansistream {
		private:
			ansi::color fg_color = ansi::color::normal;
			ansi::color bg_color = ansi::color::normal;
			std::unordered_set<ansi::style> styles;
			bool parens_on = false;
			bool origin_on = false;
			bool hidden = false;

			ansistream & left_paren();
			ansistream & right_paren();
			ansistream & move(int, char);

		public:
			std::ostream &content_out;
			std::ostream &style_out;

			ansistream();
			ansistream(std::ostream &stream): content_out(stream), style_out(stream) {}
			ansistream(std::ostream &c, std::ostream &s): content_out(c), style_out(s) {}

			ansistream(const ansistream &) = delete;
			ansistream(ansistream &&) = delete;

			ansistream & operator=(const ansistream &&) = delete;
			ansistream & operator=(ansistream &) = delete;

			static ansistream & err();
			ansistream & flush();
			ansistream & clear();

			/** Moves the cursor to a given position. Arguments are expected to be zero-based. */
			ansistream & jump(int x, int y);
			/** Moves the cursor to the top-left corner. */
			ansistream & jump();

			/** Saves the cursor position via CSI s. */
			ansistream & save();
			/** Restores the cursor position via CSI u. */
			ansistream & restore();

			/** Clears the entire line at the cursor. */
			ansistream & clear_line();
			/** Clears all text to the left of the cursor. */
			ansistream & clear_left();
			/** Clears all text to the right of the cursor. */
			ansistream & clear_right();

			/** Makes the cursor visible. */
			ansistream & show();
			/** Makes the cursor invisible. */
			ansistream & hide();

			/* These functions move the cursor in one direction by a given offset. */ /**/
			ansistream & up(int = 1);
			ansistream & down(int = 1);
			ansistream & right(int = 1);
			ansistream & left(int = 1);

			/** Sets the absolute vertical position of the cursor. Zero-based. */
			ansistream & vpos(int);
			/** Sets the absolute horizontal position of the cursor. Zero-based. */
			ansistream & hpos(int);

			/** Scrolls up by a number of lines. Lines at the bottom are replaced with blank lines. */
			ansistream & scroll_up(int);
			/** Scrolls down by a number of lines. Lines at the top are replaced with blank lines. */
			ansistream & scroll_down(int);

			/** Deletes a number of characters starting at the cursor. */
			ansistream & delete_chars(int = 1);
			
			/** Enables origin mode: the home position is set to the top-left corner of the margins. */
			ansistream & set_origin();
			/** Disables origin mode. */
			ansistream & reset_origin();

			/** Sets the horizontal margins of the scrollable area. Zero-based. */
			ansistream & hmargins(int left, int right);
			/** Resets the horizontal margins of the scrollable area. */
			ansistream & hmargins();
			/** Enables horizontal margins. This must be called before calling hmargins. */
			ansistream & enable_hmargins();
			/** Disables horizontal margins. */
			ansistream & disable_hmargins();
			/** Sets the vertical margins of the scrollable area. Zero-based. */
			ansistream & vmargins(int top, int bottom);
			/** Resets the vertical margins of the scrollable area. */
			ansistream & vmargins();
			/** Sets the vertical and horizontal margins of the scrollable area. Zero-based. */
			ansistream & margins(int top, int bottom, int left, int right);
			/** Resets the vertical and horizontal margins of the scrollable area. */
			ansistream & margins();

			/** Restores the foreground and background colors to the terminal's default colors. */
			ansistream & reset_colors();

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
			ansistream & operator<<(const T &value) {
				// Piping miscellaneous values into the ansistream simply forwards them as-is to the content stream.
				left_paren();
				content_out << value;
				right_paren();
				return *this;
			}

			ansistream & operator>>(const ansi::style &);
	};

	extern ansistream out;
	static action endl   = action::end_line;
	static action good   = action::check;
	static action bad    = action::nope;
	static action warn   = action::warning;
	static action info   = action::information;
	static action oparen = action::open_paren;
	static action cparen = action::close_paren;
	static action parens = action::enable_parens;

	std::string get_fg(ansi::color, bool bright = false);
	std::string get_bg(ansi::color, bool bright = false);
	color_pair fg(ansi::color color);
	color_pair bg(ansi::color color);
	ansi_pair<ansi::style> remove(ansi::style);
	std::string wrap(const std::string &, const color_pair &);
	std::string wrap(const std::string &, const color &);
	std::string wrap(const std::string &, const style &);
	void write(std::ostream &, const std::string &);
	void write(const std::string &);
	std::string get_name(ansi::color);
	ansi::color get_color(const std::string &);

	/** Strips the ANSI escape sequences from a string. */
	std::string strip(const std::string &);

	/** Finds a substring without ANSI escapes affecting the character count. */
	std::string substr(const std::string &, size_t, size_t = std::string::npos);

	/** Returns the length of a string without counting ANSI escapes. */
	size_t length(const std::string &);

	/** Boldens a string by wrapping it with the enable-bold and disable-bold escapes. */
	std::string bold(const std::string &);

	/** Dims a string by wrapping it with the enable-dim and disable-dim escapes. */
	std::string dim(const std::string &);

	/** Erases part of a string (ANSI aware). */
	std::string & erase(std::string &, size_t pos = 0, size_t len = std::string::npos);

	/** Adjusts an index in a string to account for ANSI escapes. */
	size_t get_pos(const std::string &, size_t);

	/** Inserts something into a string (ANSI aware). */
	template <typename T>
	std::string & insert(std::string &str, size_t pos, const T &obj) {
		return str.insert(get_pos(str, pos), obj);
	}

#define MKCOLOR(x) std::string x(const std::string &);
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
	MKCOLOR(blood)
#undef MKCOLOR

	const extern std::string reset_all;
	const extern std::string reset_fg;
	const extern std::string reset_bg;

	const extern std::string str_check;
	const extern std::string str_nope;
	const extern std::string str_warning;

	const extern std::map<color, std::string> color_names;
	const extern std::map<color, std::string> color_bases;
	const extern std::map<style, std::string> style_codes;
	const extern std::map<style, std::string> style_resets;

	extern std::ofstream dbgout;
	extern ansistream dbgstream;
}

std::string operator"" _b(const char *str, unsigned long);
std::string operator"" _d(const char *str, unsigned long);
std::string operator"" _i(const char *str, unsigned long);
std::string operator"" _u(const char *str, unsigned long);
std::string operator"" _bd(const char *str, unsigned long);

#pragma GCC diagnostic pop

#endif
