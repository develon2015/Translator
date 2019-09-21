#include <net.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

void
tresolveIP(const char *p) {
	const char *domain = p;
	printf("%s %s\n", __func__, domain);
	struct in6_addr addr;
	char buf[1024];
	if (resolveIP(domain, AF_INET, &addr) == 0) 
		printf("%s -> %s\n", domain, inet_ntop(AF_INET, &addr, buf, sizeof addr));
	else
		printf("%s\n", hstrerror(h_errno));
	if (resolveIP(domain, AF_INET6, &addr) == 0)
		printf("%s -> %s\n", domain, inet_ntop(AF_INET6, &addr, buf, sizeof buf));
	else
		printf("%s\n", hstrerror(h_errno));
}
