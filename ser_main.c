#include "staff.h"

int main(int argc, const char *argv[])
{
	//打开数据库
	sqlite3* db=NULL;
	if(sqlite3_open("./staff.db",&db)!=SQLITE_OK)
	{
		printf("sqlite3_open failed\n");
		printf("%d\n",sqlite3_errcode(db));
		fprintf(stderr,"__%d__sqlite3_open:%s\n",__LINE__,sqlite3_errmsg(db));
		return -1;
	}
//		printf("sqlite3_open success\n");


	//捕获17号信号
	sighandler_t s=signal(17,handler);
	if(SIG_ERR==s)
	{
		ERR_MSG("signal");
		return -1;
	}
	//创建流式套接字
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0)
	{
		ERR_MSG("socket");
		return -1;
	}
	
	//允许端口快速重用
	int reuse=1; 	//允许
	if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0)
	{
		ERR_MSG("setsockopt");
		return -1;
	}
	
	//填充地址信息结构体
	struct sockaddr_in sin;
	sin.sin_family=AF_INET;
	sin.sin_port=htons(PORT); 			
	sin.sin_addr.s_addr=inet_addr(IP);
	//绑定服务器的IP和端口号
	if(bind(sfd,(struct sockaddr*)&sin,sizeof(sin))<0)
	{
		ERR_MSG("bind");
		return -1;
	}
//	printf("绑定成功\n");
	
	//将套接字设置为监听状态
	if(listen(sfd,10)<0)	
	{
		ERR_MSG("listen");
		return -1;
	}
	printf("        员工管理系统服务器\n");
	//声明cin存储客户端的信息
	struct sockaddr_in cin;
	int addrlen=sizeof(cin);
	MSG user_msg;
		
	while(1)
	{
		//主进程只负责连接客户端
		int newfd=accept(sfd,(struct sockaddr*)&cin,&addrlen);
		if(newfd<0)
		{
			ERR_MSG("accept");
			return -1;
		}
		//打印客户端的IP和端口号
		printf("[%s | %d] newfd=%d\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port),newfd);
		
		//子进程负责完成功能
		if(fork()==0)
		{
			//根据协议完成对应功能
			choose(newfd,user_msg,db);
		}
	}
	sqlite3_close(db);
	return 0;
}

