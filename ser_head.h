#ifndef __STAFF__
#define __STAFF__

#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

#define N 128
#define PORT 7788 //范围：1024~49151;                        
#define  IP "192.168.1.22" //填本机IP，终端输入 ifconfig查找


//打印错误信息
#define ERR_MSG(msg) do{\
	fprintf(stderr,"__%d__",__LINE__);\
	perror(msg);\
}while(0)                              

//数据包结构体
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




typedef void (*sighandler_t)(int);
void handler(int sig);
void sqlite_init(sqlite3* db);
//导入单词表
void recv_cli_info(int newfd,struct sockaddr_in cin);
int choose(int newfd,MSG user_msg,sqlite3* db);
int user_add(int newfd,MSG user_msg,sqlite3* db);
int user_login(int newfd,MSG user_msg,sqlite3* db,char* p);
int search_word(int newfd,MSG user_msg,sqlite3* db,char* p);
int history(int newfd,MSG user_msg,sqlite3* db,char* p);

int su_user(int sfd,MSG user_msg,sqlite3* db);
int user(int sfd,MSG user_msg,sqlite3* db);    


int show_self_msg(int newfd,MSG user_msg,sqlite3* db);         //查信息    
int update_self_msg(int newfd,MSG user_msg,sqlite3* db);       //改信息
int su_update_all(int newfd,MSG user_msg,sqlite3* db);      
int su_show_all(int newfd,MSG user_msg,sqlite3* db);            

int su_delete(int newfd,MSG user_msg,sqlite3* db);      

#endif
