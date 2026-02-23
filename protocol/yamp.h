int YAMPConnect(const char *server, int *socket_out);
int SplitAddress(char *address, char **username, char **server);
int YAMPLogin(int fd, char *username, char *password);
int YAMPListBuddies(int fd);
int YAMPSendIM(int fd, char* where, char *content);
char *MakeDMChannel(const char *a, const char *b);
char* GetOtherFromChannel(const char *channel, const char *me);