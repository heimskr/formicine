#ifndef FORMICINE_FUTIL_H_
#define FORMICINE_FUTIL_H_

#include <sstream>
#include <string>
#include <vector>

namespace formicine {
	struct util {
		/** Splits a string by a given delimiter. If condense is true, empty strings won't be included in the output. */
		static std::vector<std::string> split(const std::string &str, const std::string &delimeter,
			bool condense = true);

		template <typename Iter>
		static std::string join(Iter begin, Iter end, const std::string &delim = " ") {
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
		static std::string join(const Container &cont, const std::string &delim = " ") {
			return join(cont.begin(), cont.end(), delim);
		}

		template <typename Pred, typename std::enable_if<std::is_invocable<Pred, char>::value>::type * = nullptr>
		static std::string filter(const std::string &str, Pred predicate) {
			std::string out;
			out.reserve(str.size());
			for (char ch: str) {
				if (predicate(ch))
					out.push_back(ch);
			}

			return out;
		}

		static std::string filter(const std::string &str, const std::string &allowed_chars);
		static std::string antifilter(const std::string &str, const std::string &allowed_chars);

		static std::string lower(std::string);
		static std::string upper(std::string);

		template <typename T>
		static size_t nth_index(const std::string &str, const T &to_find, int n) {
			size_t index = 0;
			for (int i = 0; i <= n; ++i)
				index = str.find(to_find, i? index + 1 : i);
			return index;
		}

		static std::string nth_word(const std::string &, size_t, bool condense = false);
	};
}

#endif
