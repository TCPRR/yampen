int YAMPConnect(const char *server, int *socket_out);
int SplitAddress(char *address, char **username, char **server);
int YAMPLogin(int fd, char *username, char *password);
int YAMPListBuddies(int fd);