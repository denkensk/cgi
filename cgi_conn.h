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

//���ڴ���ͻ�CGI������࣬��������Ϊprocesspool���ģ�����
class cgi_conn
{
public:
	cgi_conn(){}
	~cgi_conn(){}

	//��ʼ���ͻ����ӣ���ջ�����
	void init(int epollfd, int sockfd, const sockaddr_in& client_addr);
	void process();

private:
	static const int BUFFER_SIZE = 1024;	//���������Ĵ�С
	static int m_epollfd;
	int m_sockfd;
	sockaddr_in m_address;
	char m_buf[BUFFER_SIZE];
	int m_read_idx;							//��Ƕ��������Ѿ�����Ŀͻ����ݵ����һ���ֽڵ���һ��λ��
};
