#include <iostream>
#include <string>

#include "ansi.h"
using namespace std;

int main(int, char **) {
	ansi::ansistream as(cout, cerr);
	as << "Hello.\n";
	as << "Color: " << ansi::fg(ansi::green) << "green." << ansi::reset << "\n";
	as << "Normal" << ansi::bold << " then bold" << ansi::red << " and red" >> ansi::bold << " just red"
	   << ansi::green << fg(ansi::sky) << " now sky" << ansi::dim << " and dim" << ansi::italic << " and italic "
	   << ansi::underline << "and underlined" << ansi::reset << " and plain.\n";
	as << "Normal" << ansi::yellow << " yellow " << bg(ansi::green) << "yellow+green" << ansi::normal << " normal+green"
	   << fg(ansi::red) << " red+green" << bg(ansi::normal) << " red"
	   << ansi::reset << "\n";
	as << "Normal " << ansi::wrap("bold", ansi::bold) << " not bold\n";
	as << "Bold "_b << "italic "_i << "underlined"_u << " dim "_d << "bold+dim"_bd << "\n";
}
