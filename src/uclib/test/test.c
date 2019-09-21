#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <assert.h>

int
main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("测试程序t\n\tt <函数> [参数]\n");
		return 0;
	}
	char fn[1024] = { 0 };
	sprintf(fn, "t%s", argv[1]);
	void (*fp)(const char *) = dlsym(NULL, fn);
	if (fp == NULL) {
		printf("%s\n", dlerror());
		return 1;
	}
	fp(argv[2]);
	return 0;
}
