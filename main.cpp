#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "cgi_conn.h"
#include "processpool.hpp"

int main(int argc, char* argv[])
{
//	if (argc <= 2)
//	{
//		printf("usage: %s ip_address port_number\n", basename(argv[0]));
//		return 1;
//	}

//	const char* ip = argv[1];
	const char* ip = "0.0.0.0";
//	int port = atoi(argv[2]);
	int port = 9000;
	int listenfd = socket(PF_INET, SOCK_STREAM, 0);
	assert(listenfd >=0 );

	int ret = 0;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
	assert(ret != -1);

	ret = listen(listenfd, 5);
	assert(ret != -1);

	processpool< cgi_conn >* pool = processpool< cgi_conn >::create(listenfd);
	if (pool)
	{
		pool->run();
		delete pool;
	}

	//main函数创建了文件描述符listenfd，那么就有它亲自关闭
	close(listenfd);
	return 0;
}
