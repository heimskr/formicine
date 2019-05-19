# Formicine
A simple C++ library that wraps ostreams for use with ANSI escapes.

### Example
##### Code
```cpp
#include <iostream>
#include "ansi.h"

int main(int, char **) {
    ansi::ansistream as(std::cout, std::cerr);

    as << "Normal text" << std::endl;
    as << ansi::bold << "Bold text" << ansi::reset << std::endl;
    as << fg(ansi::green) << "Green text" << ansi::reset << std::endl;
    as << ansi::underline << "Underlined text,"
       << ansi::italic << " italic+underlined text"
       >> ansi::underline << " and just italic text."
       << ansi::reset << std::endl;
}
```

##### Output
> Normal text  
**Bold text**  
<span style="color:green">Green text</span>  
<span style="text-decoration:underline">Underlined text, *italic+underlined text*</span> *and just italic text.*