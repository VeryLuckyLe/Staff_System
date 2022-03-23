#include "cli_head.h"
int choose(int sfd,MSG user_msg)
{
	while(1)
	{
		memset(&user_msg,0,sizeof(user_msg));
		//功能选择
		printf("*********** 1、注册 **********\n");
		printf("*********** 2、登录 **********\n");
		printf("*********** 3、退出 **********\n");
		scanf("%d",&user_msg.type);
		while(getchar()!=10);
		if(send(sfd,&user_msg,sizeof(user_msg),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		switch(user_msg.type)
		{
		//注册
		case 1:
			user_add(sfd,user_msg);break; 	//注册新用户
			//登录
		case 2:
			if(user_login(sfd,user_msg)== 1) 	//返回值为1 登录成功
			{
				printf("确认权限:%s\n",temp);
				if(!(strcmp(temp,"su"))){
					printf("\033c");
					su_user(sfd,user_msg);
				}
				if(!(strcmp(temp,"u"))){
					printf("\033c");
					user(sfd,user_msg);
				}
			}break;
		case 3:
			exit(0);		//退出
		}
	}
}



//注册新用户
int user_add(int sfd,MSG user_msg)
{
	int num=0;
	//将类型初始化为0
	printf("请选择您的账户权限>>\n");
	printf("-------1.普通用户------\n");
	printf("------2.管理员用户------\n");
	scanf("%d",&num);
	while(getchar()!=10);
	if(num == 1){
		strcpy(user_msg.perm,"u");
	}
	if(num == 2){
		strcpy(user_msg.perm,"su");
	}
	bzero(user_msg.name,sizeof(user_msg.name));
	printf("请输入用户名:");
	fgets(user_msg.name,sizeof(user_msg.name),stdin);
	user_msg.name[strlen(user_msg.name)-1]=0;

	printf("请输入密码:");
	bzero(user_msg.pwd,sizeof(user_msg.pwd));
	fgets(user_msg.pwd,sizeof(user_msg.pwd),stdin);
	user_msg.pwd[strlen(user_msg.pwd)-1]=0;
	printf("%s %s %s\n",user_msg.perm,user_msg.name,user_msg.pwd);
	//将权限、用户名、密码一块发送出去
	if(send(sfd,&user_msg,sizeof(user_msg),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	bzero(user_msg.text,sizeof(user_msg.text));
	//阻塞等待服务器回复，判断该用户名是否可用
	if(recv(sfd,&user_msg,sizeof(user_msg),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(user_msg.text[0]=='F')
	{
		printf("该用户已存在，请重新注册!\n");
		return 0;
	}
	if(user_msg.text[0]=='T')
	{
		printf("恭喜您注册成功^_^\n");
		printf("用户名:%s\n密码:%s\n",user_msg.name,user_msg.pwd);
		return 0;
	}
}

//登录
int user_login(int sfd,MSG user_msg)
{
	char name[20];
	printf("请输入用户名:");
	bzero(user_msg.name,sizeof(user_msg.name));
	fgets(user_msg.name,sizeof(user_msg.name),stdin);
	user_msg.name[strlen(user_msg.name)-1]=0;
	strcpy(name,user_msg.name);
	
	printf("请输入密码:");
	bzero(user_msg.pwd,sizeof(user_msg.pwd));
	fgets(user_msg.pwd,sizeof(user_msg.pwd),stdin);
	user_msg.pwd[strlen(user_msg.pwd)-1]=0;
	if(send(sfd,&user_msg,sizeof(user_msg),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	printf("用户名:%s\n密码:%s\n",user_msg.name,user_msg.pwd);
	if(recv(sfd,&user_msg,sizeof(user_msg),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(user_msg.text[0]=='2')
	{
		printf("登录成功\n");
		printf("确认权限:%s\n",user_msg.perm);
		strcpy(temp,user_msg.perm);
		return 1;
	}
	else if(user_msg.text[0]=='1')
	{
		printf("该用户不存在,请先注册\n");
		return -1;
	}
}
//管理员登录界面
int su_user(int sfd,MSG user_msg)
{
	while(1){
		printf("--------------------\n"); 
		printf("| 管理员登录界面 |\n");   
		printf("--------------------\n"); 
		printf("1、查看个人信息\n");
		printf("2、修改个人信息\n");      
		printf("3、打印员工信息\n");      
		printf("4、修改员工信息\n");
		printf("5、删除员工信息\n");
		printf("6、返回上一级\n");        
		scanf("%d",&user_msg.type);
		while(getchar() != 10);
		//发送查信息/改信息协议
		if(send(sfd,&user_msg,sizeof(user_msg),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		switch(user_msg.type){
		case 1:                      
			show_self_msg(sfd,user_msg);  
			break;
		case 2:                      
			update_self_msg(sfd,user_msg);
			break;
		case 3:                      
			su_show_all(sfd,user_msg);    
			break;
		case 4:                      
			su_update_all(sfd,user_msg);  
			break;
		case 5:
			su_del(sfd,user_msg);
			break;
		case 6:
			return 0;
		}
	}
}
//普通用户登录界面
int user(int sfd,MSG user_msg)
{
	while(1){
		printf("\033c");
		printf("--------------------\n");
		printf("| 普通用户登录界面 |\n");
		printf("--------------------\n");
		printf("1、查看个人信息\n");
		printf("2、修改个人信息\n");
		printf("3、返回上一级\n");
		scanf("%d",&user_msg.type);
		while(getchar() != 10);
		//发送查信息/改信息协议
		if(send(sfd,&user_msg,sizeof(user_msg),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		switch(user_msg.type){
		case 1:
			show_self_msg(sfd,user_msg); 
			break;
		case 2:
			update_self_msg(sfd,user_msg);
			break;
		case 3:
			return 0;
		}
	}
}

int show_self_msg(int sfd,MSG user_msg)
{
	printf("--------------------\n");
	printf("|    个人信息表    |\n");
	for(int i=1;i<9;i++){
		bzero(user_msg.text,sizeof(user_msg.text));
		if(recv(sfd,&user_msg,sizeof(user_msg),0)<0){
			ERR_MSG("recv");
			return -1;
		}
		switch(i){
		case 1:
			printf("| 姓名：%-10s |\n",user_msg.text);
			break;
		case 2:
			printf("| 性别：%-10s |\n",user_msg.text);
			break;
		case 3:
			printf("| 年龄：%-10s |\n",user_msg.text);
			break;
		case 4:
			printf("| 电话：%-10s |\n",user_msg.text);
			break;
		case 5:
			printf("| 地址：%-10s |\n",user_msg.text);
			break;
		case 6:
			printf("| 工号：%-10s |\n",user_msg.text);
			break;
		case 7:
			printf("| 薪资：%-10s |\n",user_msg.text);
			break;
		case 8:
			printf("| 部门：%-10s |\n",user_msg.text);
			printf("--------------------\n");
			return 0;
		}
	}


	return 0;
}

int update_self_msg(int sfd,MSG user_msg)
{
	int num,i=0;
	while(1){
		printf("\033c");
		printf("请根据序号修改对应信息: \n");
		printf("1、姓名 \n");
		printf("2、性别 \n");
		printf("3、年龄 \n");
		printf("4、电话 \n");
		printf("5、地址 \n");
		printf("6、返回 \n");
		scanf("%d",&num);
		while(getchar() != 10);
		i++;
		user_msg.flag[i]=num;
		switch(num){
		case 1:
			printf("请输入姓名: ");
			fgets(user_msg.myname,sizeof(user_msg.myname),stdin);
			user_msg.myname[strlen(user_msg.myname)-1]=0;
			break;
		case 2:
			printf("请输入性别: ");
			fgets(user_msg.sex,sizeof(user_msg.sex),stdin);
			user_msg.sex[strlen(user_msg.sex)-1]=0;
			break;
		case 3:
			printf("请输入年龄: ");
			scanf("%d",&user_msg.age);
			while(getchar() != 10);
			break;
		case 4:
			printf("请输入新的电话: ");
			fgets(user_msg.tel,sizeof(user_msg.tel),stdin);
			user_msg.tel[strlen(user_msg.tel)-1]=0;
			break;
		case 5:
			printf("请输入修改后的地址: ");
			fgets(user_msg.address,sizeof(user_msg.address),stdin);
			user_msg.address[strlen(user_msg.address)-1]=0;
			break;
		case 6:	
			user_msg.flag[0]=i-1;//下标0统计修改的次数
			if(send(sfd,&user_msg,sizeof(user_msg),0)<0){
				ERR_MSG("send");
				return -1;
			}
			return 0;
		}
	}
}

int su_show_all(int sfd,MSG user_msg)
{	
	int row,column,num=0;
	//接收行、列数
	if(recv(sfd,&user_msg,sizeof(user_msg),0)<0){
		ERR_MSG("recv");
		return -1;
	}
	printf("row = %d, column = %d\n",user_msg.flag[0],user_msg.flag[1]);
	row = user_msg.flag[0];
	column = user_msg.flag[1];
	for(int i=1;i<=(row+1)*(column);i++){
		bzero(user_msg.text,sizeof(user_msg.text));
		if(recv(sfd,&user_msg,sizeof(user_msg),0)<0){
			ERR_MSG("recv");
			return -1;
		}
		printf("%-8s",user_msg.text);
		if(i%11 == 0)
			putchar(10);
	}
	return 0;
}
int su_update_all(int sfd,MSG user_msg)
{

	printf("\033c");
	printf("修改员工信息\n");
	printf("请输入该员工账号:");
	fgets(user_msg.name,sizeof(user_msg.name),stdin);
	user_msg.name[strlen(user_msg.name)-1]=0;
	//发送输入的用户名
	if(send(sfd,&user_msg,sizeof(user_msg),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	printf("用户名:%s\n",user_msg.name);

	printf("--------------------\n");
	printf("|   员工详细信息   |\n");
	for(int i=1;i<9;i++){
		bzero(user_msg.text,sizeof(user_msg.text));
		if(recv(sfd,&user_msg,sizeof(user_msg),0)<0){
			ERR_MSG("recv");
			return -1;
		}
		switch(i){
		case 1:
			printf("| 1.姓名：%-10s |\n",user_msg.text);
			break;
		case 2:
			printf("| 2.性别：%-10s |\n",user_msg.text);
			break;
		case 3:
			printf("| 3.年龄：%-10s |\n",user_msg.text);
			break;
		case 4:
			printf("| 4.电话：%-10s |\n",user_msg.text);
			break;
		case 5:
			printf("| 5.地址：%-10s |\n",user_msg.text);
			break;
		case 6:
			printf("| 6.工号：%-10s |\n",user_msg.text);
			break;
		case 7:
			printf("| 7.薪资：%-10s |\n",user_msg.text);
			break;
		case 8:
			printf("| 8.部门：%-10s |\n",user_msg.text);
			printf("--------------------\n");
		}
	}
	int num,i=0;
	while(1){
	printf("\033c");
		printf("1、姓名 \n");
		printf("2、性别 \n");
		printf("3、年龄 \n");
		printf("4、电话 \n");
		printf("5、地址 \n");
		printf("6、工号 \n");
		printf("7、薪资 \n");
		printf("8、部门 \n");
		printf("9、返回 \n");

		printf("请输入要修改的选项>>");

		scanf("%d",&num);
		while(getchar() != 10);
		i++;
		user_msg.flag[i]=num;
		switch(num){
		case 1:
			printf("请输入姓名: ");
			fgets(user_msg.myname,sizeof(user_msg.myname),stdin);
			user_msg.myname[strlen(user_msg.myname)-1]=0;
			break;
		case 2:
			printf("请输入性别: ");
			fgets(user_msg.sex,sizeof(user_msg.sex),stdin);
			user_msg.sex[strlen(user_msg.sex)-1]=0;
			break;
		case 3:
			printf("请输入年龄: ");
			scanf("%d",&user_msg.age);
			while(getchar() != 10);
			break;
		case 4:
			printf("请输入新的电话: ");
			fgets(user_msg.tel,sizeof(user_msg.tel),stdin);
			user_msg.tel[strlen(user_msg.tel)-1]=0;
			break;
		case 5:
			printf("请输入修改后的地址: ");
			fgets(user_msg.address,sizeof(user_msg.address),stdin);
			user_msg.address[strlen(user_msg.address)-1]=0;
			break;
		case 6:
			printf("请输入工号: ");
			fgets(user_msg.id,sizeof(user_msg.id),stdin);
			user_msg.id[strlen(user_msg.id)-1]=0;
			break;
		case 7:
			printf("请输入薪资: ");
			scanf("%ld",&user_msg.wage);
			while(getchar() != 10);
			break;
		case 8:
			printf("请输入部门: ");
			fgets(user_msg.department,sizeof(user_msg.department),stdin);
			user_msg.department[strlen(user_msg.department)-1]=0;
			break;
		case 9:	
			user_msg.flag[0]=i-1;//下标0统计修改的次数
			if(send(sfd,&user_msg,sizeof(user_msg),0)<0){
				ERR_MSG("send");
				return -1;
			}
			return 0;
		}
	}
	return 0;
}

int su_del(int sfd,MSG user_msg)
{
	printf("请输入要删除的员工账号>>\n");
	bzero(user_msg.name,sizeof(user_msg.name));
	fgets(user_msg.name,sizeof(user_msg.name),stdin);
	user_msg.name[strlen(user_msg.name)-1]=0;
	//发送输入的用户名
	if(send(sfd,&user_msg,sizeof(user_msg),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	printf("%s已从信息库中移除\n",user_msg.name);
	return 0;
}
