#include <unistd.h>
#include <tools.h>
#include <string.h>

void
tshowMem(const char *p) {
	showMem(p, strlen(p) + 2);
}
