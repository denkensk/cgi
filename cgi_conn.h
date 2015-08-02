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

//用于处理客户CGI请求的类，它可以作为processpool类的模板参数
class cgi_conn
{
public:
	cgi_conn(){}
	~cgi_conn(){}

	//初始化客户连接，清空缓冲区
	void init(int epollfd, int sockfd, const sockaddr_in& client_addr);
	void process();

private:
	static const int BUFFER_SIZE = 1024;	//读缓冲区的大小
	static int m_epollfd;
	int m_sockfd;
	sockaddr_in m_address;
	char m_buf[BUFFER_SIZE];
	int m_read_idx;							//标记读缓冲中已经读入的客户数据的最后一个字节的下一个位置
};
