/**
 * net.h
 * 网络工具包
 */

// 解析IP地址
int
resolveIP(const char *domain, int type, void *addr_t);

// IPv6判断
int
isIPv6Supported();

