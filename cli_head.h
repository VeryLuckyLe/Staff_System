#ifndef __CLI_HEAD__ 
#define __CLI_HEAD__ 
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define N 128
#define ERR_MSG(msg) do{\
	fprintf(stderr,"__%d__",__LINE__);\
	perror(msg);\
}while(0)

#define PORT 7788 //范围：1024~49151;
#define  IP "192.168.1.22" //填本机IP，终端输入 ifconfig查找
char temp[20];
//账号信息结构体
typedef struct
{
	//账号信息
    int type;   //账号类型
	int flag[10];
	char perm[10];//s管理员，u用户
    char name[20];
    char pwd[20];
    char text[N];	
	//用户信息
	char id[20];
	char myname[20];
	char sex[10];
	int age;
	char tel[20];
	char address[128];
	long int wage;
	char department[20];
}MSG;

void clrscr(void);

int choose(int sfd,MSG user_msg);
int user_add(int sfd,MSG user_msg);
int user_login(int sfd,MSG user_msg);
int search_work(int sfd,MSG user_msg);
int select_list(void);
int history(int sfd,MSG user_msg);
int show_msg(int sfd,MSG user_msg);
int update_msg(int sfd,MSG user_msg); 			//改信息
int su_user(int sfd,MSG user_msg);
int user(int sfd,MSG user_msg);
int select_func(int sfd,MSG user_msg); //调用功能选择函数
int update_msg(int sfd,MSG user_msg); 			//改信息

int show_self_msg(int sfd,MSG user_msg);  
int update_self_msg(int sfd,MSG user_msg);
                                            
int su_update_all(int sfd,MSG user_msg);  
int su_show_all(int sfd,MSG user_msg);    

int su_del(int sfd,MSG user_msg);

#endif 

