#include "cjson/cJSON.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "yamp.h"
#define YAMP_PORT 5224
int YAMPConnect(const char *server, int *socket_out) {
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	*socket_out = sock;
	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(YAMP_PORT); /*converts short to
	                                       short with network byte order*/
	addr.sin_addr.s_addr = inet_addr(server);
	return connect(sock,(const struct sockaddr*)&addr,sizeof(addr));
}
int SplitAddress(char *address, char **username, char **server) {
	char *newAddr = strdup(address);
	char *at = strchr(newAddr, '@');
	if (!at) {
		return 0; //get gud get @
	}
	*at = '\0';
	*username = newAddr;
	*server = at + 1;
	return 1;
}