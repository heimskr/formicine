#include "futil.h"

namespace formicine {
	std::vector<std::string> util::split(const std::string &str, const std::string &delimeter, bool condense) {
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

	std::string util::filter(const std::string &str, const std::string &allowed_chars) {
		return filter(str, [&](char ch) { return allowed_chars.find(ch) != std::string::npos; });
	}

	std::string util::antifilter(const std::string &str, const std::string &forbidden_chars) {
		return filter(str, [&](char ch) { return forbidden_chars.find(ch) == std::string::npos; });
	}

	std::string util::lower(std::string str) {
		// TODO: Unicode. Of course.
		for (size_t i = 0, length = str.length(); i < length; ++i) {
			if ('A' <= str[i] && str[i] <= 'Z')
				str[i] += 'a' - 'A';
		}

		return str;
	}

	std::string util::upper(std::string str) {
		// TODO: Unicode. Of course.
		for (size_t i = 0, length = str.length(); i < length; ++i) {
			if ('a' <= str[i] && str[i] <= 'z')
				str[i] -= 'a' - 'A';
		}

		return str;
	}
}
