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

#define HOST "translate.my-api.workers.dev"
#define PORT htons(80)
#define HEADER \
"GET /translate_a/single?client=gtx&dt=t&dj=1&ie=UTF-8&sl=%s&tl=%s&q=%s HTTP/1.1\r\n"\
"Host: " HOST "\r\n"\
"User-Agent: Chrome/12.8\r\n"\
"Accept: application/json\r\n"\
"Connection: close\r\n"\
"\r\n"

#define DEBUG 0

#if !DEBUG
#define log(args, ...) {}
#else
#define log(args, ...) printf(args"\n", ##__VA_ARGS__)
#endif

int
translate(const char *target);
void
handle(const char *response);
const char *
getBody(const char *response);
char *
URIEncode(const char *uri);
char *
getJSONValue(const char *json, const char *key);

char *sl = "auto";
char *tl = "zh_CN";
char *target = NULL;

struct sockaddr_in remoteAddr = { 0 };

int
main(int argc, char *argv[]) {
	int isShell = 0;
	switch (argc) {
	case 1: // 交互式 OR 管道式
		isShell = 1;
		break;
	case 2:
		if (strcmp("-h", argv[argc - 1]) == 0)
			goto DEFAULT;
		break;
	case 3: // 指定了目标语言
		tl = argv[1];
		break;
	case 4: // 指定了源和目标语言
		sl = argv[1];
		tl = argv[2];
		break;
DEFAULT:
	default:
		printf("翻译程序\n\t%s [sl] [tl] <tc>\n\t交互式Shell中可以切换语种, "\
			"使用\":sl tl\"命令即可\n\t例如\":en_US zh_CN\"\n\n"\
			"\t%s\n\n", argv[0], __DATE__ " - " __TIME__);
		return 1;
	}

	// 解析 IP 地址
	struct in_addr addr = { 0 };
	if (resolveIP(HOST, AF_INET, &addr) != 0) {
		puts("解析服务器地址失败");
		return 2;
	}
	char buf[128] = { 0 };
	inet_ntop(AF_INET, &addr, buf, sizeof buf);
	log("服务器地址: %s", buf);

	// 设置远程主机地址和端口
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = PORT;
	remoteAddr.sin_addr.s_addr = addr.s_addr;

	// 新特性
	if (strcmp("--", argv[argc - 1]) == 0)
		isShell = 1;

	if (isShell)
GETLINE:
		while (1) {
			char buf[10240] = { 0 };
			if (fgets(buf, 10240, stdin) == NULL)
				return 0;
			if (buf[0] == ':') {
				// 切换源和目标语言
				char *bufSL = (char *)malloc(128);
				char *bufTL = (char *)malloc(128);
				int n = sscanf(buf, ":%s %s", bufSL, bufTL);
				if (n > 0) {
					if (n == 1)
						tl = bufSL;
					else {
						sl = bufSL;
						tl = bufTL;
					}
					printf("[INFO]语种切换: %s -> %s.\n", sl, tl);
					goto GETLINE;
				}
			}
			buf[strlen(buf) - 1] = '\0';
			if (strcmp("", buf) == 0 || strcmp(" ", buf) == 0)
				continue;
			log("翻译%s", buf);
			translate(buf);
		}
	else {
		translate(argv[argc - 1]);
	}
}


int
translate(const char *target) {
	// HTTP GET 请求
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 1) {
		perror("socket");
		return 3;
	}

	log("连接服务器中...");
	int n = connect(sock, (void *)&remoteAddr, sizeof remoteAddr);
	if (n != 0) {
		printf("disconnect: %s\n", hstrerror(h_errno));
		return 4;
	}
	log("已连接服务器.");

	// 发送请求之前处理URI编码
	target = URIEncode(target);

	// 单线程发送, 接收数据
	char buf[10240] = { 0 };
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
	while (1) {
		char bufSegm[10240] = { 0 };
		n = read(sock, bufSegm, sizeof bufSegm);
		if (n < 1) {
			log("连接被关闭");
			break;
		}
		log("读取%d字节", n);
		strcat(buf, bufSegm);
	}
	log("RESPONSE:\n%s", buf);

	// 处理response
	handle(buf);
	close(sock);

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
	log("关注body:\n%s", body);
	char *result = (char *)malloc(strlen(body));
	result = getJSONValue(body, "trans");
	if (result == NULL) {
		printf("翻译失败: %s\n", body);
		return;
	}
	//result[strlen(result) - 1] = '\0';
	printf("%s\n", result);
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
URIEncode(const char *uri) {
	int n = strlen(uri);
	char *cp = (char *)malloc(n * 3);
	memset(cp, 0, n * 3);
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
	char *result = (char *)malloc(n);
	memset(result, 0, n);
	int isSucceed = 0;
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
				int offs = strcspn(&json[i], ":"); // "key"(:.*)
				int offe = n - 1;
				for (int k = offs; k < n; k ++ ) {
					if (json[i + k] == ',' && json[i + k + 1] == '"') {
							offe = k;
							break;
					}
				}
				log("s&e %d %d", offs, offe);
				strncpy(buf, &json[i + offs], offe - offs);
				//return buf;
				// cat buf to result
				buf[strlen(buf) - 1] = '\0';
				log("找到:%s", &buf[2]);
				strcat(result, &buf[2]);
				isSucceed ++ ;
			}
		}
	}
	if (isSucceed)
		return result;
	return NULL;
}

