#include "staff.h"
char temp[20];
char name[20];
//回收僵尸进程
void handler(int sig)
{
	while(waitpid(-1,NULL,WNOHANG)>0);
	return;
}

int choose(int newfd,MSG user_msg,sqlite3* db)
{
	char* p=name;
	while(1)
	{
		//功能选择
		memset(&user_msg,0,sizeof(user_msg));

		//接收客户端发送的协议
		int res=0;
		res=recv(newfd,&user_msg,sizeof(user_msg),0);
		if(res<0)
		{
			ERR_MSG("recv");
			return -1;
		}
		else if(res==0)
		{
			printf("该用户已退出\n");
			break;
		}

		//打印协议信息
		printf("type=%d\n",user_msg.type);

		//协议判断: 1 注册申请 	 2 登录申请
		switch(user_msg.type)
		{
		case 1:printf("注册申请\n");
			   user_add(newfd,user_msg,db);
			   break;
		case 2:printf("登录申请\n");
			   if(user_login(newfd,user_msg,db,p)==1) //登录成功返回1
			   {
				   printf("确认权限：%s\n",temp);

				   if(!(strcmp(temp,"su"))){  
					   su_user(newfd,user_msg,db); 
				   }                          
				   if(!(strcmp(temp,"u"))){   
					   user(newfd,user_msg,db);    
				   }                          
			   }
			   break;
		}
	}
}

//管理员登录界面
int su_user(int newfd,MSG user_msg,sqlite3* db)
{ 
	while(1){

		//接收查信息/改信息协议
		if(recv(newfd,&user_msg,sizeof(user_msg),0)<0)
		{
			ERR_MSG("recv");
			return -1;
		}
		switch(user_msg.type){                          
		case 1:                                         
			printf("查看个人信息\n");
			show_self_msg(newfd,user_msg,db);          
			break;
		case 2:                                         
			printf("修改个人信息\n");
			update_self_msg(newfd,user_msg,db);      
			break;
		case 3:                                         
			printf("打印员工信息\n");
			su_show_all(newfd,user_msg,db);            
			break;
		case 4:                                         
			printf("修改员工信息\n");
			su_update_all(newfd,user_msg,db);      
			break;
		case 5:                                         
			printf("删除员工信息\n");
			su_delete(newfd,user_msg,db);      
			break;
		case 6:
			printf("返回上一级\n");
			return 0;
		}                                               
	}
}
//普通用户登录界面
int user(int newfd,MSG user_msg,sqlite3* db)    
{	
	while(1){
		printf("--------------------\n");
		printf("| 普通用户登录界面 |\n");
		printf("--------------------\n");
		printf("1、查看个人信息\n");
		printf("2、修改个人信息\n");
		printf("3、返回上一级\n");
		if(recv(newfd,&user_msg,sizeof(user_msg),0)<0)
		{
			ERR_MSG("recv");
			return -1;
		}
		switch(user_msg.type){                          
		case 1:                                         
			printf("查看个人信息\n");
			show_self_msg(newfd,user_msg,db);         //查信息    
			break;
		case 2:                                         
			printf("修改个人信息\n");
			update_self_msg(newfd,user_msg,db);       //改信息
			break;
		case 3:
			return 0;
		}                                               
	}
}
//查看个人信息
int show_self_msg(int newfd,MSG user_msg,sqlite3* db)         //查信息    
{	
	char sql[512];
	char **pres=NULL;
	int row,column;
	char* errmsg=NULL;

	printf("查询信息的名字:%s\n",name);
	sprintf(sql,"select * from staff_system where name =\"%s\";",name);
	if(sqlite3_get_table(db,sql,&pres,&row,&column,&errmsg)!=SQLITE_OK)
	{
		fprintf(stderr,"sqlite3_exec:__%d__  %s\n",__LINE__,errmsg);
		return -1;
	}
	printf("row=%d column=%d\n",row,column);
#if 0
	//遍历查询结果
	for(int i=0;i<(row+1)*(column+1);i++)
	{
		printf("[ %d   %s ]\n",i,pres[i]);
	}
#endif
	for(int i=14;i<22;i++){
		bzero(user_msg.text,sizeof(user_msg.text));
		strcpy(user_msg.text,pres[i]);
		printf("[ %d   %s ]\n",i,pres[i]);
		if(send(newfd,&user_msg,sizeof(user_msg),0)<0){
			ERR_MSG("send");
			return -1;
		}
	}
	return 0;
}
//修改个人信息
int update_self_msg(int newfd,MSG user_msg,sqlite3* db)       //改信息
{
	int i;
	printf("修改个人信息\n");
	//接收客户端发送的修改信息协议
	if(recv(newfd,&user_msg,sizeof(user_msg),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	printf("修改了 %d 个地方\n",user_msg.flag[0]);
	printf("写入数据库的名字：%s\n",user_msg.myname);
	printf("查询信息的名字:%s\n",name);
	char sql[512];
	bzero(sql,sizeof(sql));
	char* errmsg=NULL;
	for(i=1;i<=user_msg.flag[0];i++){
		switch(user_msg.flag[i]){
		case 1:
			sprintf(sql,"update staff_system set myname=\"%s\" where name=\"%s\";",
					user_msg.myname,name);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
				return -1;
			}
			break;
		case 2:
			sprintf(sql,"update staff_system set sex=\"%s\" where name=\"%s\";",
					user_msg.sex,name);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
				return -1;
			}
			break;
		case 3:
			sprintf(sql,"update staff_system set age='%d' where name=\"%s\";",
					user_msg.age,name);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
				return -1;
			}
			break;
		case 4:
			sprintf(sql,"update staff_system set tel=\"%s\" where name=\"%s\";",
					user_msg.tel,name);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
				return -1;
			}
			break;
		case 5:
			sprintf(sql,"update staff_system set address=\"%s\" where name=\"%s\";",
					user_msg.address,name);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
				return -1;
			}
			break;
		}
	}


	//遍历查询结果
	//for(int i=0;i<6;i++)
	//	printf("%s  \n",pres[i]);
	return 0;
}
//查看所有员工信息
int su_show_all(int newfd,MSG user_msg,sqlite3* db)
{
	printf("打印所有员工信息\n");
	char sql[512];
	char **pres=NULL;
	int row,column;
	char* errmsg=NULL;

	sprintf(sql,"select * from staff_system;");
	if(sqlite3_get_table(db,sql,&pres,&row,&column,&errmsg)!=SQLITE_OK)
	{
		fprintf(stderr,"sqlite3_exec:__%d__  %s\n",__LINE__,errmsg);
		return -1;
	}
	printf("row=%d column=%d\n",row,column);
	bzero(user_msg.flag,sizeof(user_msg.flag));
	user_msg.flag[0]=row;
	user_msg.flag[1]=column;
	//发送行、列数
	if(send(newfd,&user_msg,sizeof(user_msg),0)<0){
		ERR_MSG("send");
		return -1;
	}
#if 0
	//遍历查询结果
	for(int i=0;i<(row+1)*(column+1);i++)
	{
		printf("[ %d   %s ]\n",i,pres[i]);
	}
#endif
	for(int i=0;i<(row+1)*(column);i++){
		bzero(user_msg.text,sizeof(user_msg.text));
		strcpy(user_msg.text,pres[i]);
		printf("[ %d   %s ]\n",i,user_msg.text);
		if(send(newfd,&user_msg,sizeof(user_msg),0)<0){
			ERR_MSG("send");
			return -1;
		}
	}


	return 0;
}
//修改所有员工信息
int su_update_all(int newfd,MSG user_msg,sqlite3* db)     
{
	printf("修改员工信息\n");
	if(recv(newfd,&user_msg,sizeof(user_msg),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	
	printf("修改信息的用户名:%s\n",user_msg.name);

	char sql[512]={0};
	char **pres=NULL;
	int row,column;
	char* errmsg=NULL;

	sprintf(sql,"select * from staff_system where name =\"%s\";",user_msg.name);
	if(sqlite3_get_table(db,sql,&pres,&row,&column,&errmsg)!=SQLITE_OK)
	{
		fprintf(stderr,"sqlite3_exec:__%d__  %s\n",__LINE__,errmsg);
		return -1;
	}
	printf("row=%d column=%d\n",row,column);
#if 0
	//遍历查询结果
	for(int i=0;i<(row+1)*(column+1);i++)
	{
		printf("[ %d   %s ]\n",i,pres[i]);
	}
#endif
	for(int i=14;i<22;i++){
		bzero(user_msg.text,sizeof(user_msg.text));
		strcpy(user_msg.text,pres[i]);
		printf("[ %d   %s ]\n",i,pres[i]);
		if(send(newfd,&user_msg,sizeof(user_msg),0)<0){
			ERR_MSG("send");
			return -1;
		}
	}

	int i;
	printf("修改该员工信息\n");
	//接收客户端发送的修改信息协议
	if(recv(newfd,&user_msg,sizeof(user_msg),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	printf("修改了 %d 个地方\n",user_msg.flag[0]);
	printf("写入数据库的名字：%s\n",user_msg.myname);
//	printf("查询信息的名字:%s\n",name);
	bzero(sql,sizeof(sql));
	for(i=1;i<=user_msg.flag[0];i++){
		switch(user_msg.flag[i]){
		case 1:
			sprintf(sql,"update staff_system set myname=\"%s\" where name=\"%s\";",
					user_msg.myname,user_msg.name);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
				return -1;
			}
			break;
		case 2:
			sprintf(sql,"update staff_system set sex=\"%s\" where name=\"%s\";",
					user_msg.sex,user_msg.name);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
				return -1;
			}
			break;
		case 3:
			sprintf(sql,"update staff_system set age='%d' where name=\"%s\";",
					user_msg.age,user_msg.name);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
				return -1;
			}
			break;
		case 4:
			sprintf(sql,"update staff_system set tel=\"%s\" where name=\"%s\";",
					user_msg.tel,user_msg.name);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
				return -1;
			}
			break;
		case 5:
			sprintf(sql,"update staff_system set address=\"%s\" where name=\"%s\";",
					user_msg.address,user_msg.name);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
				return -1;
			}
			break;
		case 6:
			sprintf(sql,"update staff_system set id=\"%s\" where name=\"%s\";",
					user_msg.id,user_msg.name);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
				return -1;
			}
			break;
		case 7:
			sprintf(sql,"update staff_system set wage='%ld' where name=\"%s\";",
					user_msg.wage,user_msg.name);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
				return -1;
			}
			break;
		case 8:
			sprintf(sql,"update staff_system set department=\"%s\" where name=\"%s\";",
					user_msg.department,user_msg.name);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
				return -1;
			}
			break;
		}
	}

	return 0;
}

int su_delete(int newfd,MSG user_msg,sqlite3* db)
{
	char sql[512]={0};
	char **pres=NULL;
	int row,column;
	char* errmsg=NULL;

	if(recv(newfd,&user_msg,sizeof(user_msg),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}

	printf("即将删除用户:%s\n",user_msg.name);

	sprintf(sql,"delete from staff_system where name=\"%s\";",
			user_msg.name);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
		return -1;
	}
	printf("删除成功\n");
}

//注册
int user_add(int newfd,MSG user_msg,sqlite3* db)
{
	//将获取到的用户名和密码存入数据库	
	bzero(user_msg.name,sizeof(user_msg.name));
	bzero(user_msg.pwd,sizeof(user_msg.pwd));
	if(recv(newfd,&user_msg,sizeof(user_msg),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}

	printf("权限：%s\n",user_msg.perm);
	printf("用户名:%s\n密码:%s\n",user_msg.name,user_msg.pwd);

	//将用户信息存入数据库
	char sql[512]="create table if not exists staff_system (perm char,name char primary key,pwd char,\
				   myname char,sex char,age int,tel char,address char,id char,wage char,\
				   department char);";
	char* errmsg=NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		fprintf(stderr,"sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
		return -1;
	}
//	printf("table user create success\n");
	
	//录入用户信息
	//初始化数据
	char none[20]="none";
	int n1=0;
	long int n2=0;
	sprintf(sql,"insert into staff_system(perm,name,pwd,myname,sex,age,tel,\
		address,id,wage,department) values('%s','%s','%s',\
		'%s','%d','%s','%s','%s','%s','%ld','%s');",user_msg.perm,user_msg.name,user_msg.pwd,\
		none,n1,none,none,none,none,n2,none);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		printf("该用户名已被使用\n");
		bzero(user_msg.text,sizeof(user_msg.text));
		user_msg.text[0]='F';
	if(send(newfd,&user_msg,sizeof(user_msg),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return 0;
	}

	bzero(user_msg.text,sizeof(user_msg.text));
	user_msg.text[0]='T';
	if(send(newfd,&user_msg,sizeof(user_msg),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}

	printf("注册成功\n");
	return 1;

}
//登录请求	   
int user_login(int newfd,MSG user_msg,sqlite3* db,char* p)
{
	printf("录入信息中\n");
	if(recv(newfd,&user_msg,sizeof(user_msg),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	
	printf("用户名:%s\n密码:%s\n",user_msg.name,user_msg.pwd);

	char sql[512]={0};
	char **pres=NULL;
	int row,column;
	char* errmsg=NULL;

	sprintf(sql,"select * from staff_system where name =\"%s\" and \"%s\";",user_msg.name,user_msg.pwd);
	if(sqlite3_get_table(db,sql,&pres,&row,&column,&errmsg)!=SQLITE_OK)
	{
		fprintf(stderr,"sqlite3_exec:__%d__  %s\n",__LINE__,errmsg);
		return -1;
	}
	printf("row=%d column=%d\n",row,column);

#if 0
	//遍历查询结果
	for(int i=0;i<(row+1)*(column+1);i++)
	{
		printf("[ %d   %s ]\n",i,pres[i]);
	}
#endif
	strcpy(user_msg.perm,pres[11]);
	strcpy(name,pres[12]);
	strcpy(temp,user_msg.perm);
	//对比输入的用户名和密码
	bzero(user_msg.text,sizeof(user_msg.text));
	if(row==0)
	{
		printf("该用户不存在\n");
		user_msg.text[0]='1';
		if(send(newfd,&user_msg,sizeof(user_msg),0)<0)
		{
			ERR_MSG("send");
			sqlite3_free_table(pres);
		return -1;
		}
		sqlite3_free_table(pres);
		return 0;
	}
	else
	{
		printf("%s登录成功\n",user_msg.name);
		strcpy(p,user_msg.name);
		user_msg.text[0]='2';
		if(send(newfd,&user_msg,sizeof(user_msg),0)<0)
		{
			ERR_MSG("send");
			sqlite3_free_table(pres);
			return -1;
		}
		sqlite3_free_table(pres);
		printf("user_msg.perm = %s\n",user_msg.perm);
		return 1;
	}

	//释放获取到的结果
	sqlite3_free_table(pres);
	return 0;
}

