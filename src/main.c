#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#include <uclib/inc/net.h>
#include <uclib/inc/tools.h>

#define HOST "translate.google.cn"
#define PORT htons(80)
#define HEADER \
"GET /translate_a/single?client=gtx&dt=t&dj=1&ie=UTF-8&sl=%s&tl=%s&q=%s HTTP/1.1\r\n"\
"Host: " HOST "\r\n"\
"User-Agent: Chrome/12.8\r\n"\
"Accept: application/json\r\n"\
"\r\n"

#define DEBUG 0

#if !DEBUG
#define log(args, ...) {}
#else
#define log(args, ...) printf(args"\n", ##__VA_ARGS__)
#endif

void
handle(const char *response);
const char *
getBody(const char *response);
char *
URIEncode(char *uri);
char *
getJSONValue(const char *json, const char *key);

char *sl = "auto";
char *tl = "zh_CN";
char *target = NULL;

int
main(int argc, char *argv[]) {
	switch (argc) {
	case 2:
		break;
	case 3: // 指定了目标语言
		tl = argv[1];
		break;
	case 4: // 指定了源和目标语言
		sl = argv[1];
		tl = argv[2];
		break;
	default:
		printf("翻译程序\n\t%s [sl] [tl] <tc>\n", argv[0]);
		return 1;
	}
	target = URIEncode(argv[argc - 1]);

	// 解析 IP 地址
	struct in_addr addr = { 0 };
	if (resolveIP(HOST, AF_INET, &addr) != 0) {
		puts("解析服务器地址失败");
		return 2;
	}
	char buf[10240] = { 0 };
	inet_ntop(AF_INET, &addr, buf, sizeof buf);
	log("服务器地址: %s", buf);

	// HTTP GET 请求
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 1) {
		perror("socket");
		return 3;
	}
	struct sockaddr_in remoteAddr = { 0 };
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = PORT;
	remoteAddr.sin_addr.s_addr = addr.s_addr;
	log("连接服务器中...");
	int n = connect(sock, (void *)&remoteAddr, sizeof remoteAddr);
	if (n != 0) {
		printf("disconnect: %s\n", hstrerror(h_errno));
		return 4;
	}
	log("已连接服务器.");

	// 单线程发送, 接收数据
	memset(buf, 0, sizeof buf);
	sprintf(buf, HEADER, sl, tl, target);
	log("HEADER:\n%ssize: %ld", buf, strlen(buf));
	n = send(sock, buf, strlen(buf), 0); // flag
	if (n < 0) {
		perror("发送失败");
		close(sock);
		return 0;
	}

	log("写入%d字节", n);

	memset(buf, 0, sizeof buf);
	n = read(sock, buf, sizeof buf);
	log("读取%d字节", n);
	log("RESPONSE:\n%s", buf);

	handle(buf);
	close(sock);

	// 处理response

	return 0;
}

void
handle(const char *response) {
	int statu = 0;
	if (sscanf(response, "%*s %d", &statu) != 1) {
		printf("无法识别状态码:\n%s\n", response);
		return;
	}
	if (statu != 200) {
		printf("错误的状态码: %d\n%s\n", statu, response);
		return;
	}
	const char *body = getBody(response);
	char *result = (char *)malloc(strlen(body));
	result = getJSONValue(body, "trans");
	if (result == NULL) {
		printf("翻译失败: %s\n", body);
		return;
	}
	printf("翻译结果%s\n", result);
}

// 获取 Response body 部分, 删除了头部的空行
const char *
getBody(const char *text) {
	int n = strlen(text);
	for (int i = 0; i < n; i ++ ) {
		if (text[i] == '\n' && ((text[i+1] == '\r' && text[i+2] == '\n') || text[i+1] == '\n')) {
			//return text[i + 2] == '\n' ? &text[i + 3] : &text[i + 2];
			for (i = i+2; i < n; i ++ ) {
				if (text[i] != '\n' && text[i] != '\r')
					return &text[i];
			}
		}
	}
	return NULL;
}

// "只有字母和数字[0-9a-zA-Z]、一些特殊符号"$-_.+!*'(),"[不包括双引号]、以及某些保留字，才可以不经过编码直接用于URL。"
static int
isURIChar(const char ch) {
	if (isalnum(ch))
		return 1;
	if (ch == ' ')
		return 0;
	switch (ch) {
	case '$':
	case '-':
	case '_':
	case '.':
	case '+':
	case '!':
	case '*':
	case '\'':
	case '(':
	case ')':
		return 1;
	default:
		return 0;
	}
}

char *
URIEncode(char *uri) {
	int n = strlen(uri);
	char *cp = (char *)malloc(n * 3);
	memset(cp, 0, n);
	int pn = 0;
	// scan uri
	for (int i = 0; i < n; i ++ ) {
		if (isURIChar(uri[i])) {
			cp[pn++] = uri[i];
			continue;
		}
		if (uri[i] == ' ') {
			cp[pn++] = '+';
			continue;
		}
		//log("uri[%d] 需要 编码", i);
		// UTF8
		char it[4] = { 0 };
		sprintf(it, "%%%X", (unsigned char)uri[i]);
		//log("it -> %s", it);
		strcat(cp, it);
		pn += 3;
	}
	log("%s. -> %s.\n", uri, cp);
	return cp;
}

char *
getJSONValue(const char *json, const char *key) {
	log("查找key:%s", key);
	int n = strlen(json);
	int nk = strlen(key);
	for (int i = 0; i < n; i ++ ) {
		if (json[i] == '"' && json[i+1] == *key) { // "k...
			log("有点儿意思:%s", &json[i]);
			int isTarget = 1;
			for (int j = 0; j < nk; j ++ ) {
				if (json[i + j + 1] == key[j])
					continue;
				isTarget = 0;
			}
			// "key?
			if (isTarget && json[i + nk + 1] == '"') { // "key"...
				char *buf = (char *)malloc(n);
				memset(buf, 0, n);
				int offs = strcspn(&json[i], ":");
				int offe = strcspn(&json[i], ",");
				log("s&e %d %d", offs, offe);
				strncpy(buf, &json[i + offs], offe - offs);
				log("找到:%s", buf);
				return buf;
			}
		}
	}
	return NULL;
}

