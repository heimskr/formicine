#ifndef FORMICINE_FUTIL_H_
#define FORMICINE_FUTIL_H_

#include <string>
#include <vector>

namespace formicine {
	struct util {
		static std::vector<std::string> split(const std::string &str, const std::string &delimeter);
	};
}

#endif
