#include <iostream>
#include <string>

#include "ansi.h"
using namespace std;

int main(int, char **) {
	ansi::ansistream as(cout, cerr);
	as << "Hello." << endl;
}
