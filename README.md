# LD_PRELOAD library to fix https://feedback.bistudio.com/T190201

1. run make
2. cp `fix_dayz.so` to a place where the DayZ server binary can access it.
3. `export LD_PRELOAD=/path/to/fix_dayz.so` before running the DayZ server (or put it in your Docker environment, whatever works.

# LICENSE
Copyright (c) 2025 Bernd Zeimetz <bzed@debian.org>. All rights reserved.

Redistribution and use in source and binary forms are permitted
provided that the above copyright notice and this paragraph are
duplicated in all such forms and that any documentation,
advertising materials, and other materials related to such 
distribution and use acknowledge that the software was developed 
by the copyright holder. The name of the copyright holder may
not be used to endorse or promote products derived from this
software without specific prior written permission.

**THIS SOFTWARE IS PROVIDED `'AS IS″ AND WITHOUT ANY EXPRESS OR 
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.**
