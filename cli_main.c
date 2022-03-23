#include "cli_head.h"
int main(int argc, const char *argv[])
{

	//创建流式套接字
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0)
	{
		ERR_MSG("socket");
		return -1;
	}

	//填充地址信息结构体
	struct sockaddr_in sin;
	sin.sin_family=AF_INET;        //地址族，固定填AF__INET
	sin.sin_port=htons(PORT);      //端口号的网络字节序
	sin.sin_addr.s_addr=inet_addr(IP);//IP的网络字节序

	//连接服务器
	if(connect(sfd,(struct sockaddr*)&sin,sizeof(sin))<0)
	{
		ERR_MSG("connect");
		return -1;
	}
	printf("连接成功\n");
	
	MSG user_msg;
	//功能选择

	choose(sfd,user_msg);

	char buf[128]="";
	ssize_t res=0;
	ssize_t snd=0;
	while(1)
	{
		//发送数据给服务器
		bzero(buf,sizeof(buf));
		fgets(buf,sizeof(buf),stdin);
		snd=send(sfd,buf,sizeof(buf),0);
		if(snd<0)
		{
			ERR_MSG("send");
			return -1;
		}
		else if(0==snd)
		{
			printf("客户端关闭\n");
			break;
		}
		printf("成功发送%ld个字符\n",(strlen(buf)-1));
		bzero(buf,sizeof(buf));
/*		//接收数据
		res=recv(sfd,buf,sizeof(buf),0);
		if(res<0)
		{
			ERR_MSG("recv");
			return -1;
		}
		else if(0==res)
		{
			printf("客户端关闭\n");
			break;
		}
*/
		//将sin中的网络字节序port转换成本机字节序输出 IP转换成点分十进制输出
		printf("[%s | %d]\n",inet_ntoa(sin.sin_addr),ntohs(sin.sin_port));
	}
	//关闭文件描述符
	close(sfd);
	return 0;
}

