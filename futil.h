#ifndef FORMICINE_FUTIL_H_
#define FORMICINE_FUTIL_H_

#include <string>
#include <vector>

namespace formicine {
	struct util {
		/** Splits a string by a given delimiter. If condense is true, empty strings won't be included in the output. */
		static std::vector<std::string> split(const std::string &str, const std::string &delimeter,
			bool condense = true);
	};
}

#endif
