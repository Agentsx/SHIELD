#include "frame.h"
#include <sys/types.h> 
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int setnonblocking(int fd)
{
    int flag;
    flag = fcntl(fd, F_GETFD);
    return fcntl(fd, flag | O_NONBLOCK);
}

int accept_fd(int sockfd)
{
    struct sockaddr_in sa;
    socklen_t addrlen = sizeof(sa);
    memset(&sa, 0x00, sizeof(sa));

    int connfd;
    while (1) {
        connfd = accept(sockfd, (struct sockaddr *)&sa, &addrlen);
        if (connfd < 0) {
            if (errno == EPROTO || errno == ECONNABORTED)
                continue;
            else 
                break;
        }
        return connfd;
    }
    return -1;
}

int init_net(int port)
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("[%s][%d]socket error\n", __FL__);
        return -1;
    }

    setnonblocking(sockfd);

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        printf("[%s][%d] setsockopt error\n", __FL__);
		goto error;
    }

    struct sockaddr_in svraddr;

    svraddr.sin_family = AF_INET;
    svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    svraddr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)&svraddr, sizeof(svraddr)) == -1) {
        printf("[%s][%d] bind socket error\n", __FL__);
		goto error;
    }

    if (-1 == listen(sockfd, 5)) {
        printf("[%s][%d] listen error\n", __FL__);
		goto error;
    }
    
    return sockfd;

error:
	close(sockfd);
	return -1;
}

int close_fd(int fd)
{
	return close(fd);
}
