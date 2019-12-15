#include "futil.h"

namespace formicine::util {
	std::vector<std::string> split(const std::string &str, const std::string &delimeter, bool condense) {
		if (str.empty())
			return {};

		size_t next = str.find(delimeter);
		if (next == std::string::npos)
			return {str};

		std::vector<std::string> out {};
		const size_t delimeter_length = delimeter.size();
		size_t last = 0;

		out.push_back(str.substr(0, next));

		while (next != std::string::npos) {
			last = next;
			next = str.find(delimeter, last + delimeter_length);
			std::string sub = str.substr(last + delimeter_length, next - last - delimeter_length);
			if (!sub.empty() || !condense)
				out.push_back(std::move(sub));
		}

		return out;
	}

	std::string filter(const std::string &str, const std::string &allowed_chars) {
		return filter(str, [&](char ch) { return allowed_chars.find(ch) != std::string::npos; });
	}

	std::string antifilter(const std::string &str, const std::string &forbidden_chars) {
		return filter(str, [&](char ch) { return forbidden_chars.find(ch) == std::string::npos; });
	}

	std::string lower(std::string str) {
		// TODO: Unicode. Of course.
		for (size_t i = 0, length = str.length(); i < length; ++i) {
			if ('A' <= str[i] && str[i] <= 'Z')
				str[i] += 'a' - 'A';
		}

		return str;
	}

	std::string upper(std::string str) {
		// TODO: Unicode. Of course.
		for (size_t i = 0, length = str.length(); i < length; ++i) {
			if ('a' <= str[i] && str[i] <= 'z')
				str[i] -= 'a' - 'A';
		}

		return str;
	}

	std::string nth_word(const std::string &str, size_t n, bool condense) {
		if (condense) {
			const std::vector<std::string> words = split(str, " ", true);
			return n < words.size()? words[n] : "";
		}

		if (n == 0)
			return str.substr(0, str.find(' '));

		const size_t index = nth_index(str, ' ', n);
		if (index == std::string::npos)
			return "";
		return str.substr(index + 1, nth_index(str, ' ', n + 1) - index - 1);
	}

	size_t word_count(const std::string &str, bool condense) {
		size_t count = 0, i = 0, length = str.length();
		if (length == 0)
			return 0;

		// If condense is true, count all spaces except the ones at the beginning, then count and subtract the ones at
		// the end.

		if (condense) {
			for (; i < length && str[i] == ' '; ++i);
			if (i == length)
				return 0;
		}

		for (; i < length; ++i) {
			if (str[i] == ' ' && (!condense || i == 0 || str[i - 1] != ' '))
				++count;
		}

		if (condense) {
			for (i = length - 1; 0 <= static_cast<long>(i) && str[i] == ' '; --i)
				--count;
		}

		return count + 1;
	}

	std::string & trim(std::string &str) {
		return ltrim(rtrim(str));
	}

	std::string trim(const std::string &str) {
		std::string copy {str};
		return trim(copy);
	}

	std::string & ltrim(std::string &str) {
		if (str.empty())
			return str;

		const size_t first = str.find_first_not_of(" \t\r\n");
		if (first == std::string::npos)
			str.clear();
		else
			str.erase(0, first);

		return str;
	}

	std::string ltrim(const std::string &str) {
		std::string copy {str};
		return ltrim(copy);
	}

	std::string & rtrim(std::string &str) {
		if (str.empty())
			return str;

		char back = str.back();
		while (back == ' ' || back == '\t' || back == '\r' || back == '\n') {
			str.pop_back();
			back = str.back();
		}

		return str;
	}

	std::string rtrim(const std::string &str) {
		std::string copy {str};
		return rtrim(copy);
	}

	std::pair<ssize_t, ssize_t> word_indices(const std::string &str, size_t cursor) {
		const size_t length = str.length();

		ssize_t word_index = -1;
		ssize_t sub_index  = -1;
		char prev_char = '\0';

		if (str.empty())
			return {-1, -1};

		if (cursor == 0)
			return str[0] == ' '? std::pair(-1, -1) : std::pair(0, 0);

		for (size_t i = 0; i < length; ++i) {
			char ch = str[i];

			if (ch != ' ' && (prev_char == '\0' || prev_char == ' ')) {
				++word_index;
				sub_index = 0;
			}

			if (ch == ' ') {
				if (prev_char != ' ') {
					// We've reached the end of a word. If this is where the cursor is, we're done.
					// Otherwise, it's time to increment the word index.
					if (i == cursor)
						return {word_index, sub_index};
				} else if (i == cursor) {
					// If we're within a group of spaces and this is the where the cursor is, return a negative number
					// because that means the cursor isn't in a word.
					return {-word_index - 2, -1};
				}
			} else {
				if (i == cursor)
					return {word_index, sub_index};

				++sub_index;
			}

			prev_char = ch;
		}

		if (cursor == length && prev_char != ' ')
			return {word_index, sub_index};

		return {-word_index - 2, -1};
	}

	size_t index_of_word(const std::string &str, size_t n) {
		const size_t length = str.length();
		size_t word_index = 0;

		char prev_char = '\0';
		for (size_t i = 0; i < length; ++i) {
			char ch = str[i];

			if (ch == ' ' && (prev_char != ' ' && prev_char != '\0')) {
				// We're at a space and the previous character wasn't a space (or the beginning of the string).
				// This means we've just left a word, so increment the word index.
				++word_index;
			}

			if (ch != ' ' && (prev_char == ' ' || prev_char == '\0') && word_index == n)
				return i;

			prev_char = ch;
		}

		return length;
	}

	std::string skip_words(const std::string &str, size_t n) {
		return str.substr(index_of_word(str, n));
	}

	size_t last_index_of_word(const std::string &str, size_t n) {
		const size_t length = str.length();
		size_t word_index = 0;

		char prev_char = '\0';
		for (size_t i = 0; i < length; ++i) {
			char ch = str[i];

			if (ch == ' ' && (prev_char != ' ' && prev_char != '\0')) {
				// We're at a space and the previous character wasn't a space (or the beginning of the string).
				// This means we've just left a word, so check whether we're at the requested word index.
				if (word_index++ == n)
					return i;
			}

			prev_char = ch;
		}

		return length;
	}

	ssize_t replace_word(std::string &str, size_t n, const std::string &word) {
		const size_t index = index_of_word(str, n);
		const size_t original_length = str.length();
		if (index == original_length)
			return -1;

		size_t original_word_width = 0;
		for (size_t i = index; i < original_length; ++i) {
			if (std::isspace(str[i]))
				break;
			++original_word_width;
		}

		str.replace(index, original_word_width, word);
		return index + word.length();
	}

	std::string & remove_suffix(std::string &word, const std::string &suffix) {
		if (word.empty() || suffix.empty())
			return word;

		const size_t pos = word.rfind(suffix);
		if (pos != std::string::npos && pos + suffix.length() == word.length())
			word.erase(pos);

		return word;
	}

	std::string remove_suffix(const std::string &word, const std::string &suffix) {
		std::string word_copy {word};
		remove_suffix(word_copy, suffix);
		return word_copy;
	}

	bool parse_long(const std::string &str, long &out) {
		const char *c_str = str.c_str();
		char *end;
		long parsed = strtol(c_str, &end, 10);
		if (end == c_str + str.length()) {
			out = parsed;
			return true;
		}

		return false;
	}

	std::string replace_all(std::string str, const std::string &to_replace, const std::string &replace_with) {
		const size_t replaced_length = to_replace.length();
		size_t pos;
		while ((pos = str.find(to_replace)) != std::string::npos)
			str.replace(pos, replaced_length, replace_with);
		return str;
	}

	std::string remove_html(std::string str) {
		for (size_t open = str.find('<'); open != std::string::npos; open = str.find('<')) {
			const size_t close = str.find('>', open);
			if (close == std::string::npos) {
				// No closing angle bracket? Give up.
				return str;
			}

			str.erase(open, close - open + 1);
		}

		return str;
	}
}
