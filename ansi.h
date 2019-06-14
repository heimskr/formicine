#ifndef ANSI_H_
#define ANSI_H_

#include <iostream>
#include <map>
#include <string>
#include <unordered_set>

namespace formicine {
	enum color {
		normal, red, orange, yellow, yeen, green, blue, cyan, magenta, purple, black, gray, lightgray, white, pink, sky
	};

	enum style {bold, dim, italic, underline};
	enum color_type {text, background};
	enum action {reset, end_line, check, nope, warning, open_paren, close_paren, enable_parens};

	template <typename T>
	struct ansi_pair {
		T value;
		bool add;
		ansi_pair(T value, bool add): value(value), add(add) {}
	};

	struct color_pair {
		formicine::color color;
		formicine::color_type type;

		color_pair(formicine::color color, formicine::color_type type): color(color), type(type) {}
		color_pair(formicine::color color): color(color), type(text) {}

		std::string left() const;
		std::string right() const;
	};

	class ansistream {
		private:
			color text_color;
			color bg_color;
			std::unordered_set<style> styles;
			bool parens_on = false;
			void left();
			void right();

		public:
			std::ostream &content_out;
			std::ostream &style_out;

			ansistream();
			ansistream(std::ostream &stream): content_out(stream), style_out(stream) {}
			ansistream(std::ostream &c, std::ostream &s): content_out(c), style_out(s) {}
			void flush();
			ansistream & operator<<(const color &);
			ansistream & operator<<(const color_pair &);
			ansistream & operator<<(const style &);
			ansistream & operator<<(const ansi_pair<style> &);
			ansistream & operator<<(const action &);
			ansistream & operator<<(std::ostream & (*fn)(std::ostream &));
			ansistream & operator<<(std::ostream & (*fn)(std::ios &));
			ansistream & operator<<(std::ostream & (*fn)(std::ios_base &));
			ansistream & operator<<(const char *t);

			template <typename T>
			ansistream & operator<<(const T &value) {
				// Piping miscellaneous values into the ansistream simply forwards them as-is to the content stream.
				left();
				content_out << value;
				right();
				return *this;
			}

			ansistream & operator>>(const style &);
	};

	class ansi {
		// I don't like using a class as a collection of static members, but VS Code's C++ extension
		// didn't like it when I had methods directly under namespaces. ¯\_(ツ)_/¯
		public:
			static ansistream out;
			static action endl;
			static action good;
			static action bad;
			static action warn;
			static action oparen;
			static action cparen;
			static action parens;

			static std::string get_text(const color &);
			static std::string get_bg(const color &);
			static color_pair fg(color color);
			static color_pair bg(color color);
			static ansi_pair<style> remove(style);
			static std::string wrap(const std::string &, const color_pair &);
			static std::string wrap(const std::string &, const color &);
			static std::string wrap(const std::string &, const style &);
			static void write(const std::string &);
			static void clear();

			/** Moves the cursor to a given position. Arguments are expected to be zero-based. */
			static void jump(int x, int y);
			/** Moves the cursor to the top-left corner. */
			static void jump();

			/** Saves the cursor position via CSI s. */
			static void save();
			/** Restores the cursor position via CSI u. */
			static void restore();

			/** Clears the entire line at the cursor. */
			static void clear_line();
			/** Clears all text to the left of the cursor. */
			static void clear_left();
			/** Clears all text to the right of the cursor. */
			static void clear_right();

			// These functions move the cursor in one direction by a given offset.
			static void up(size_t = 1);
			static void down(size_t = 1);
			static void right(size_t = 1);
			static void left(size_t = 1);

			static std::string reset_all;
			static std::string reset_fg;
			static std::string reset_bg;

			static std::string str_check;
			static std::string str_nope;
			static std::string str_warning;

			static std::map<color, std::string> color_bases;
			static std::map<style, std::string> style_codes;
			static std::map<style, std::string> style_resets;
	};
}

std::string operator"" _b(const char *str, unsigned long);
std::string operator"" _d(const char *str, unsigned long);
std::string operator"" _i(const char *str, unsigned long);
std::string operator"" _u(const char *str, unsigned long);
std::string operator"" _bd(const char *str, unsigned long);

#endif
