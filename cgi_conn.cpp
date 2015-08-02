#include "cgi_conn.h"

int cgi_conn::m_epollfd = -1;

static void removefd(int epollfd, int fd)
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
	close(fd);
}

//��ʼ���ͻ����ӣ���ջ�����
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
	//ѭ����ȡ�ͷ����ͻ�����
	while(true)
	{
		idx = m_read_idx;
		ret = recv(m_sockfd, m_buf + idx, BUFFER_SIZE-1-idx, 0);

		//�������������������رտͻ����ӡ����������ʱ�����ݿɶ������˳�ѭ��
		if (ret < 0)
		{
			if (errno != EAGAIN)
			{
				removefd(m_epollfd, m_sockfd);
			}
			break;
		}
		//����Է��ر����ӣ��������Ҳ�ر�����
		else if (ret == 0)
		{
			removefd(m_epollfd, m_sockfd);
			break;
		}
		else
		{
			m_read_idx += ret;
			printf("user content is : %s\n", m_buf);
			//��������ַ�'\r''\n'����ʼ����ͻ�����
			for (; idx < m_read_idx; ++idx)
			{
				if ((idx >= 1) && (m_buf[idx - 1] == '\r') && (m_buf[idx] == '\n'))
				{
					break;
				}
			}
			//���û�������ַ�'\r''\n'������Ҫ��ȡ����Ŀͻ�����
			if (idx == m_read_idx)
			{
				continue;
			}
			m_buf[idx - 1] = '\0';

			char* file_name = m_buf;
			//�жϿͻ�Ҫ���е�CGI�����Ƿ����
			if (access(file_name, F_OK) == -1)
			{
				removefd(m_epollfd, m_sockfd);
				break;
			}
			//�����ӽ�����ִ��CGI����
			ret = fork();
			if (ret == -1)
			{
				removefd(m_epollfd, m_sockfd);
				break;
			}
			else if (ret > 0)
			{
				//������ֻ��ر�����
				removefd(m_epollfd, m_sockfd);
				break;
			}
			else
			{
				//�ӽ��̽���׼�������m_sockfd����ִ��CGI����
				close(STDOUT_FILENO);
				dup(m_sockfd);
				execl(m_buf, m_buf, 0);
				exit(0);
			}
		}
	}
}



