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
    if (sock < 0) {
        return -1;
    }
    
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(YAMP_PORT);
    
    if (inet_pton(AF_INET, server, &addr.sin_addr) <= 0) {
        close(sock);
        return -1;
    }
    
    if (connect(sock, (const struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }
    
    *socket_out = sock;
    return 0;
}
int YAMPSend(int fd,void *payload, uint32_t size) {
	uint32_t NlSize = htonl(size);
	send(fd,payload,NlSize,0);
}
int YAMPLogin(int fd, char *username, char *password, char* reqId) {
	cJSON *payload = cJSON_CreateObject();
	cJSON_AddStringToObject(payload, "username", username);
	cJSON_AddStringToObject(payload, "password", password);
	cJSON_AddStringToObject(payload, "reqid", reqId);
	cJSON_AddStringToObject(payload, "type", "request");
	cJSON_AddStringToObject(payload, "endpoint", "login");
	char* finalPayload = cJSON_Print(payload);
	YAMPSend(fd, finalPayload, strlen(finalPayload) + 1);
	return 0;
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