#include <net.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int
resolveIP(const char *domain, int type, void *ip) {
	if (domain == NULL || ip == NULL)
		return -1;
	if (type != AF_INET && type != AF_INET6)
		return -1;
	struct hostent *ent = gethostbyname2(domain, type);
	if (ent == NULL)
		return -1;
	if (type == AF_INET)
		*(struct in_addr *)ip = *(struct in_addr*)ent->h_addr;
	else
		*(struct in6_addr *)ip = *(struct in6_addr*)ent->h_addr;
	return 0;
}
