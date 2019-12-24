BAO LE
Project 1

https://prog21.dadgum.com/179.html
Was trying to find out the number of bytes malloc gives. Turns out sizeof() solves most of these by calculating for us.

https://en.cppreference.com/w/cpp/string/byte/memcpy
Was reading on memcpy why bytes were printing out bunch of preceding F's. Next link solved this.

https://stackoverflow.com/questions/31090616/printf-adds-extra-ffffff-to-hex-print-from-a-char-array (helpful)
This solves the question above. Didn't know that regular char was implement as signed char, which promote to int when stored as int. Solved as unsigned char buffer.

https://stackoverflow.com/questions/7863499/conversion-of-char-to-binary-in-c
https://fresh2refresh.com/c-programming/c-type-casting/c-itoa-function/
Both of these helps in solving turning  the input char into decimal value so that it can be memcpy into the buffer as the inputed char value. Turns out this wasn't even needed, as the sscanf does most of the work.

https://stackoverflow.com/questions/1394169/converting-hex-string-stored-as-chars-to-decimal-in-c
This helps me understand shorts way to convert hex stored in char to decimal. This was a goood one.

http://www.cplusplus.com/reference/cstdlib/atof/
This helped in finding a function to convert string to floating point numbers. but the real question now is to print the decimal places. Turns out the sscanf and memcpy does a lot of that for you.

Also had good helps and hints from T.A. and class notes in understanding parts of the specs.