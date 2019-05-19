#include <iostream>
#include <string>

#include "ansi.h"
using namespace std;

int main(int, char **) {
	ansi::ansistream as(cout, cerr);
	as << "Hello." << endl;
	as << "Color: " << ansi::fg(ansi::green) << "green." << ansi::reset << endl;
	as << "Normal" << ansi::bold << " then bold" << ansi::red << " and red" >> ansi::bold << " just red"
	   << ansi::green << fg(ansi::sky) << " now sky" << ansi::dim << " and dim" << ansi::italic << " and italic "
	   << ansi::underline << "and underlined" << ansi::reset << " and plain."
	   << endl;
	as << "Normal" << ansi::yellow << " yellow " << bg(ansi::green) << "yellow+green" << ansi::normal << " normal+green"
	   << fg(ansi::red) << " red+green" << bg(ansi::normal) << " red"
	   << endl;
}
