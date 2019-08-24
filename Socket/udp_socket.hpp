#pragma once
#include <stdio.h>
#include <string.h>
#include <cassert>
#include <string>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

class UdpSocket{
	public:
		UdpSocket():_fd(-1)
	{}
		//创建套接字
		bool Socket()
		{
			_fd=socket(AF_INET,SOCK_DGRAM,0);
			if(_fd<0)
			{
				perror("socket");
				return false;
			}
			return true;
		}

		bool Close()
		{
			close(_fd);
			return true;
		}

		//文件信息与网络信息绑定
		bool Bind(const std::string& ip,uint16_t port)
		{
			sockaddr_in addr;
			addr.sin_family=AF_INET; //公共字段
			addr.sin_addr.s_addr=inet_addr(ip.c_str());  //IP地址转换，字符串转in_addr函数
			addr.sin_port=htons(port);  //端口号，转换字节序
			int ret=bind(_fd,(sockaddr*)&addr,sizeof(addr));
			if(ret<0)
			{
				perror("bind");
				return false;
			}
			return true;
		}

		//读取信息
		bool RecvFrom(std::string* buf,std::string* ip=NULL,uint16_t* port=NULL)
		{
			char tmp[1024*10]={0}; //缓冲区
			sockaddr_in peer;
			socklen_t len=sizeof(peer);
			ssize_t r=recvfrom(_fd,tmp,sizeof(tmp)-1,0,(sockaddr*)&peer,&len);
			if(r<0)
			{
				perror("recvfrom");
				return false;
			}
			buf->assign(tmp,r);
			if(ip!=NULL)
			{
				*ip=inet_ntoa(peer.sin_addr);
			}
			if(port!=NULL)
			{
				*port=ntohs(peer.sin_port);
			}
			return true;
		}

		//发送信息
		bool SendTo(const std::string &buf,const std::string& ip,uint16_t port)
		{
			sockaddr_in addr;
			addr.sin_family=AF_INET;
			addr.sin_port=htons(port);
			addr.sin_addr.s_addr=inet_addr(ip.c_str());
			ssize_t w=sendto(_fd,buf.data(),buf.size(),0,(sockaddr*)& addr,sizeof(addr));
			if(w<0)
			{
				perror("sendto");
				return false;
			}
			return true;
		}

	private:
		int _fd;
};


