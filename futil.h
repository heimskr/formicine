#ifndef FORMICINE_FUTIL_H_
#define FORMICINE_FUTIL_H_

#include <algorithm>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace formicine::util {
	/** Splits a string by a given delimiter. If condense is true, empty strings won't be included in the output. */
	std::vector<std::string> split(const std::string &str, const std::string &delimiter, bool condense = true);

	template <typename Iter>
	std::string join(Iter begin, Iter end, const std::string &delim = " ") {
		if (begin == end)
			return "";

		std::ostringstream oss;
		bool first = true;
		while (begin != end) {
			if (!first) {
				oss << delim;
			} else {
				first = false;
			}

			oss << *begin;
			++begin;
		}

		return oss.str();
	}

	template <typename Container>
	std::string join(const Container &cont, const std::string &delim = " ") {
		return join(cont.begin(), cont.end(), delim);
	}

	template <typename Pred, typename std::enable_if<std::is_invocable<Pred, char>::value>::type * = nullptr>
	std::string filter(const std::string &str, Pred predicate) {
		std::string out;
		out.reserve(str.size());
		for (char ch: str) {
			if (predicate(ch))
				out.push_back(ch);
		}

		return out;
	}

	std::string filter(const std::string &str, const std::string &allowed_chars);
	std::string antifilter(const std::string &str, const std::string &allowed_chars);

	std::string lower(std::string);
	std::string upper(std::string);

	template <typename T>
	size_t nth_index(const std::string &str, const T &to_find, int n) {
		size_t index = 0;
		for (int i = 0; i < n; ++i)
			index = str.find(to_find, i? index + 1 : i);
		return index;
	}

	std::string nth_word(const std::string &, size_t, bool condense = false);
	size_t word_count(const std::string &, bool condense = true);

	/** Trims spaces and tabs from both ends of a string. */
	std::string & trim(std::string &);
	/** Trims spaces and tabs from both ends of a string. */
	std::string   trim(const std::string &);

	/** Trims spaces and tabs from the start of a string. */
	std::string & ltrim(std::string &);
	/** Trims spaces and tabs from the start of a string. */
	std::string   ltrim(const std::string &);

	/** Trims spaces and tabs from the end of a string. */
	std::string & rtrim(std::string &);
	/** Trims spaces and tabs from the end of a string. */
	std::string   rtrim(const std::string &);

	/** Returns the index of the word that a given index is in in addition to the index within the word.
	 *  If the cursor is within a group of multiple spaces between two words, the first value will be negative.
	 *  If the first value is -1, the cursor is before the first word. -2 indicates that the cursor is before the
	 *  second word, -3 before the third, and so on. The second value will be -1 if the first value is negative. */
	std::pair<ssize_t, ssize_t> word_indices(const std::string &, size_t);

	/** Returns the index of the first character in the n-th word of a string. If n is greater than the number of
	 *  words in the string, the length of the string is returned. */
	size_t index_of_word(const std::string &, size_t n);

	/** Returns a substring that begins at the n-th word of a string. */
	std::string skip_words(const std::string &, size_t n = 1);

	/** Returns the index of first character after the n-th word of a string. If n is greater than the number of
	 *  words in the string, the length of the string is returned. */
	size_t last_index_of_word(const std::string &, size_t n);

	/** Replaces the n-th word of a string with another string and returns the position of the first character
	 *  after the replacement word (or -1 if the string has no n-th word). */
	ssize_t replace_word(std::string &, size_t n, const std::string &);

	/** Removes a suffix from a word if one is present and returns a reference to the now-modified string. */
	std::string & remove_suffix(std::string &word, const std::string &suffix);

	/** Removes a suffix from a word if one is present. */
	std::string remove_suffix(const std::string &word, const std::string &suffix);

	/** Attempts to parse a string as a long. If the string is a valid representation of a long, the parsed long is
	 *  stored in the given long reference and the function returns true. Otherwise, the function just returns
	 *  false. */
	bool parse_long(const std::string &, long &);

	/** Replaces all occurrences of a substring in a string with another string. */
	std::string replace_all(std::string str, const std::string &to_replace, const std::string &replace_with);

	/** Makes a naïve attempt to strip HTML tags from a string. */
	std::string remove_html(std::string);

	/** Returns a vector of all elements in a range that begin with a given string. */
	template <typename T, typename Iter>
	std::vector<T> starts_with(Iter start, Iter end, const std::string &prefix) {
		std::vector<T> out {};
		std::copy_if(start, end, std::back_inserter(out), [&](const std::string &str) {
			return str.find(prefix) == 0;
		});
		return out;
	}

	/** Treats a range like a circular buffer and finds the next value after a given value. If the given value isn't
	 *  present in the range, the function returns the value at the beginning of the range. If the range is empty,
	 *  the function throws std::invalid_argument. */
	template <typename Iter>
	std::string & next_in_sequence(Iter begin, Iter end, const std::string &str) {
		if (begin == end)
			throw std::invalid_argument("Empty string");

		for (Iter iter = begin; iter != end; ++iter) {
			if (*iter == str) {
				++iter;
				return iter == end? *begin : *iter;
			}
		}

		return *begin;
	}

	template <typename Iter>
	void insensitive_sort(Iter begin, Iter end) {
		std::sort(begin, end, [&](const std::string &left, const std::string &right) {
			const auto mismatch = std::mismatch(left.cbegin(), left.cend(), right.cbegin(), right.cend(),
				[](const unsigned char lchar, const unsigned char rchar) {
					return tolower(lchar) == tolower(rchar);
				});

			if (mismatch.second != right.cend())
				return false;

			return mismatch.first == left.cend() || tolower(*mismatch.first) < tolower(*mismatch.second);
		});
	}
}

#endif
