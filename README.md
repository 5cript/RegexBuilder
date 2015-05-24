# RegexBuilder
A regex builder written in C++ with Qt and boost.

Add a "stdlib.txt" containing the path to the standard library to the executables directory.

Use classes to make regex parts reusable:
```
regex Number { // 
  contains: \d;
}

regex Main {
  contains: ~Number;
}
```

The following example becomes (\d\d):(\d\d):(\d\d)
```
group Section : capture {
  contains: \d\d;
}

regex Time {
  contains: ~Section : ~Section : ~Section;
}
```

A set example:  (becomes: 0x[a-zA-Z0-9]+)
```
set Hex {
  contains: a-z;
  contains: A-Z;
  contains: 0-9;
}

regex CppHex {
  contains: 0x~Hex*;
}
```

An example using the stdlib
```
#include <fundamental/number.srx>
#include <fundamental/repeat.srx>
#include <fundamental/inline.srx>

// (?:\d)+
regex Main_1 {
  contains: repeat(~number);
}

// (?:\d){1}
regex Main_2 {
  contains: repeat(~number, 1);
}

// (?:\d){1, 2}
regex Main_3 {
  contains: repeat(~number, 1, 2);
}

// (?:\d)*
regex Main_4 {
  contains: repeat(~number, KLEENE);
}

// \d{2, 3}
regex Main_5 {
	contains: \d range(2, 3);
}

// (?:abc)
regex Main_6 {
	contains: group(abc);
}
```
