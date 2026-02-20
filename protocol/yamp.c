#include "cjson/cJSON.h"
#include "glib.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include "yamp.h"
#define YAMP_PORT 5224
extern void onYAMPBuddyListed(cJSON *Buddies);
extern void onYAMPUserDetailsFetched(cJSON *Detail);
extern void onYAMPLoggedIn();
extern void onYAMPReceiveIM(char* username, char* data);
int YAMPSend(int fd, void *payload, uint32_t size) {
	uint32_t NlSize = htonl(size);
	send(fd, &NlSize, 4, 0);
	send(fd, payload, size, 0);
}
int YAMPRecv(int fd, char **payload, uint32_t *len) {
	if (recv(fd, len, 4, 0) > 0) {
		*payload = malloc(ntohl(*len));
		recv(fd, *payload, ntohl(*len), 0);
		return 1;
	}
		return 0;
}
void *YAMPRecvLoop(void *fd) {
	uint32_t len;
	char *payload;
	while (1) {
		if (YAMPRecv(*(int *)fd, &payload, &len)) {
			cJSON *srvr = cJSON_Parse(payload);
			cJSON *type = cJSON_GetObjectItem(srvr, "type");
			if (strcmp(type->valuestring, "response") == 0) {
				printf("RESPONSE\n");
				cJSON *reqid = cJSON_GetObjectItem(srvr, "reqid");
				cJSON *response = cJSON_GetObjectItem(srvr, "response");
				if (strcmp(reqid->valuestring, "1") == 0) {
					printf("BUDDY LISTED\n");
					onYAMPBuddyListed(response);
				}
				if (strcmp(reqid->valuestring, "0") == 0) {
					printf("LOGIN RESP\n");
					onYAMPLoggedIn();
				}
			}
			if (strcmp(type->valuestring, "event") == 0) {
				cJSON *event = cJSON_GetObjectItem(srvr, "event");
				cJSON *eventdata = cJSON_GetObjectItem(srvr, "data");
				if (strcmp(event->valuestring, "recvim") == 0) {
					char *content = cJSON_GetObjectItem(eventdata, "content")->valuestring;
					char *author = cJSON_GetObjectItem(eventdata, "author")->valuestring;
					onYAMPReceiveIM(author, content);
				}
			}
			free(payload);
		}
	}
}
int YAMPConnect(const char *server, int *socket_out) {
    struct addrinfo hints = {0}, *res = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    char port_str[8];
    snprintf(port_str, sizeof(port_str), "%d", YAMP_PORT);

    int err = getaddrinfo(server, port_str, &hints, &res);
    if (err != 0) {
        return -1;
    }

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
        freeaddrinfo(res);
        return -1;
    }

    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        freeaddrinfo(res);
        close(sock);
        return -1;
    }

    freeaddrinfo(res);

    *socket_out = sock;
    int *argsock = malloc(sizeof(int));
    *argsock = sock;
    pthread_t *recvthread = malloc(sizeof(pthread_t));
    pthread_create(recvthread, NULL, YAMPRecvLoop, argsock);
    return 0;
}
int YAMPLogin(int fd, char *username, char *password) {
	cJSON *payload = cJSON_CreateObject();
	cJSON_AddStringToObject(payload, "username", username);
	cJSON_AddStringToObject(payload, "password", password);
	cJSON_AddStringToObject(payload, "reqid", "0");
	cJSON_AddStringToObject(payload, "type", "request");
	cJSON_AddStringToObject(payload, "endpoint", "login");
	char *finalPayload = cJSON_Print(payload);
	YAMPSend(fd, finalPayload, strlen(finalPayload) + 1);
	return 0;
}
int YAMPListBuddies(int fd) {
	cJSON *payload = cJSON_CreateObject();
	cJSON_AddStringToObject(payload, "reqid", "1");
	cJSON_AddStringToObject(payload, "type", "request");
	cJSON_AddStringToObject(payload, "endpoint", "buddylist");
	char *finalPayload = cJSON_Print(payload);
	YAMPSend(fd, finalPayload, strlen(finalPayload) + 1);
	return 0;
}
int YAMPSendIM(int fd,char *toWho, char *content) {
	cJSON *payload = cJSON_CreateObject();
	cJSON_AddStringToObject(payload, "reqid", toWho);
	cJSON_AddStringToObject(payload, "toWho", toWho);
	cJSON_AddStringToObject(payload, "type", "request");
	cJSON_AddStringToObject(payload, "endpoint", "sendim");
	cJSON_AddStringToObject(payload, "content", content);
	char *finalPayload = cJSON_Print(payload);
	YAMPSend(fd, finalPayload, strlen(finalPayload) + 1);
	return 0;
}
int SplitAddress(char *address, char **username, char **server) {
	char *newAddr = strdup(address);
	char *at = strchr(newAddr, '@');
	if (!at) {
		return 0; // get gud get @
	}
	*at = '\0';
	*username = newAddr;
	*server = at + 1;
	return 1;
}