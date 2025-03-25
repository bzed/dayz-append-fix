
all: fix_dayz.so

%.so: %.c
	gcc -Wall -O3 -shared -fPIC -o $@ $< -ldl
