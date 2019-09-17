#include <iostream>
#include <string>

#include "ansi.h"
using namespace std;

int main(int, char **) {
	ansi::ansistream as(cout, cerr);
	as << "Hello.\n";
	as << "Color: " << ansi::fg(ansi::color::green) << "green." << ansi::action::reset << "\n";
	as << "Normal" << ansi::style::bold << " then bold" << ansi::color::red << " and red" >> ansi::style::bold
	   << " just red" << ansi::color::green << fg(ansi::color::sky) << " now sky" << ansi::style::dim << " and dim"
	   << ansi::style::italic << " and italic " << ansi::style::underline << "and underlined" << ansi::action::reset
	   << " and plain.\n";
	as << "Normal" << ansi::color::yellow << " yellow " << bg(ansi::color::green) << "yellow+green"
	   << ansi::color::normal << " normal+green" << fg(ansi::color::red) << " red+green" << bg(ansi::color::normal)
	   << " red" << ansi::action::reset << "\n";
	as << "Normal " << ansi::wrap("bold", ansi::style::bold) << " not bold\n";
	as << "Bold "_b << "italic "_i << "underlined"_u << " dim "_d << "bold+dim"_bd << "\n";
}
