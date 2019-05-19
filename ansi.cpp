#include <iostream>
#include <string>

#include "ansi.h"

namespace ansi {
	ansistream & ansistream::operator<<(const ansi::color c) {
		color = c;
		return *this;
	}

	ansistream & ansistream::operator<<(const std::string &str) {
		
		return *this;
	}
}
