#include "cgi_conn.h"

int cgi_conn::m_epollfd = -1;

static void removefd(int epollfd, int fd)
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
	close(fd);
}

//初始化客户连接，清空缓冲区
void cgi_conn::init(int epollfd, int sockfd, const sockaddr_in& client_addr)
{
	m_epollfd = epollfd;
	m_sockfd = sockfd;
	m_address = client_addr;
	memset(m_buf, '\0', BUFFER_SIZE);
	m_read_idx = 0;
}

void cgi_conn::process()
{
	int idx = 0;
	int ret = -1;
	//循环读取和分析客户数据
	while(true)
	{
		idx = m_read_idx;
		ret = recv(m_sockfd, m_buf + idx, BUFFER_SIZE-1-idx, 0);

		//如果读操作发生错误，则关闭客户连接。但如果是暂时无数据可读，则退出循环
		if (ret < 0)
		{
			if (errno != EAGAIN)
			{
				removefd(m_epollfd, m_sockfd);
			}
			break;
		}
		//如果对方关闭连接，则服务器也关闭连接
		else if (ret == 0)
		{
			removefd(m_epollfd, m_sockfd);
			break;
		}
		else
		{
			m_read_idx += ret;
			printf("user content is : %s\n", m_buf);
			//如果遇到字符'\r''\n'，则开始处理客户请求
			for (; idx < m_read_idx; ++idx)
			{
				if ((idx >= 1) && (m_buf[idx - 1] == '\r') && (m_buf[idx] == '\n'))
				{
					break;
				}
			}
			//如果没有遇到字符'\r''\n'，则需要读取更多的客户数据
			if (idx == m_read_idx)
			{
				continue;
			}
			m_buf[idx - 1] = '\0';

			char* file_name = m_buf;
			//判断客户要运行的CGI程序是否存在
			if (access(file_name, F_OK) == -1)
			{
				removefd(m_epollfd, m_sockfd);
				break;
			}
			//创建子进程来执行CGI程序
			ret = fork();
			if (ret == -1)
			{
				removefd(m_epollfd, m_sockfd);
				break;
			}
			else if (ret > 0)
			{
				//父进程只需关闭连接
				removefd(m_epollfd, m_sockfd);
				break;
			}
			else
			{
				//子进程将标准输出定向到m_sockfd，并执行CGI程序
				close(STDOUT_FILENO);
				dup(m_sockfd);
				execl(m_buf, m_buf, 0);
				exit(0);
			}
		}
	}
}



