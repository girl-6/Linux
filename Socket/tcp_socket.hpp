#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <cassert>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

#define CHECK_RET(exp) if(!exp){\
return false;\
}

class TcpSocket{
	public:
		TcpSocket():_fd(-1)
	{}
		TcpSocket(int fd):_fd(fd)
	{}
		bool Socket()
		{
			_fd=socket(AF_INET,SOCK_STREAM,0);
			if(_fd<0)
			{
				perror("socket");
				return false;
			}
			printf("open fd=%d\n",_fd);
			return true;
		}

		bool Close()const
		{
			close(_fd);
			printf("close fd=%d\n",_fd);
			return true;
		}

		bool Bind(const std::string &ip,uint16_t port)const
		{
			sockaddr_in addr;
			addr.sin_family=AF_INET;
			addr.sin_port=htons(port);
			addr.sin_addr.s_addr=inet_addr(ip.c_str());
			int ret=bind(_fd,(sockaddr*)&addr,sizeof(addr));
			if(ret<0)
			{
				perror("bind");
				return false;
			}
			return true;
		}

		bool Listen(int num)const
		{
			int ret=listen(_fd,num);
			if(ret<0)
			{
				perror("listen");
				return false;
			}
			return true;
		}

		bool Accept(TcpSocket* peer,std::string *ip=NULL,uint16_t *port=NULL)const
		{
			sockaddr_in peer_addr;
			socklen_t len=sizeof(peer_addr);
			int new_sock=accept(_fd,(sockaddr*)&peer_addr,&len);
			if(new_sock<0)
			{
				perror("listen");
				return false;
			}
			printf("accept fd=%d\n",new_sock);
			peer->_fd=new_sock;
			if(ip!=NULL)
				*ip=inet_ntoa(peer_addr.sin_addr);
			if(port!=NULL)
				*port=ntohs(peer_addr.sin_port);
			return true;
		}

		bool Recv(std::string *buf)const
		{
			buf->clear();
			char tmp[1024*10]={0};
			ssize_t r=recv(_fd,tmp,sizeof(tmp),0);
			if(r<0)
			{
				perror("recv");
				return false;
			}
			if(r==0)
			{
				return false;
			}
			buf->assign(tmp,r);
			return true;
		}

		bool Send(const std::string& buf)const
		{
			ssize_t w=send(_fd,buf.data(),buf.size(),0);
			if(w<0)
			{
				perror("send");
				return false;
			}
			return true;
		}

		bool Connect(const std::string &ip,uint16_t port)const
		{
			sockaddr_in addr;
			addr.sin_family=AF_INET;
			addr.sin_port=htons(port);
			addr.sin_addr.s_addr=inet_addr(ip.c_str());
			int ret=connect(_fd,(sockaddr*)&addr,sizeof(addr));
			if(ret<0)
			{
				perror("connect");
				return false;
			}
			return true;
		}

		int GetFd()const
		{
			return _fd;
		}
	private:
		int _fd;
};

