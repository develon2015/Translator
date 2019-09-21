#include <stdio.h>


void
showMem(const void *p, size_t le) {
#define	RL	16	// 换行数
	printf("显示内存 %p :\n", p);
	printf("=======================================================\n");
	const unsigned char *cp = p;
	for (int i = 0; i < le; i++) {
		int c = cp[i];
		printf("%c%X ", c < 0x10 ? '0' : 0, c);
		if ((i + 1) % RL == 0 || i == le - 1)
			puts("");
	}
	printf("=======================================================\n");
}
