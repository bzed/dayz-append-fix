
all: fix_dayz.so

%.so: %.c
	gcc -shared -fPIC -o $@ $< -ldl
