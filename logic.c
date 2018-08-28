#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include "logic.h"
#include "common_func.h"
//帮助界面
void __help(){
	printf("**********help**********\n");
	printf("************************\n");
	printf("************************\n");
	printf("************************\n");
	printf("************************\n");
	printf("************************\n");
}
//删除字符串多余的空格
void del_str_space(char *str){
	char *p;
	while(p=strstr(str, "  ")){
		strcpy(p, p+1);
	}
}
void __init(){
	DIR *fd;
	fd = opendir("./data");
	if(fd == NULL){//数据文件夹不存在
		//创建数据文件夹
		int ret = mkdir("data", 0666);
		if(ret!=0)
			goto mkdir_err;
	}
	closedir(fd);
	return;
mkdir_err:
	perror("mkdir");
	exit(0);
}
//初始界面
void __index(){
	int database_label=0;
	system("clear");
	printf("Welcome to the MySQL monitor.\n");
	printf("Server version: 1.0\n");
	printf("Copyright (c) 2016. All rights reserved.\n");
	printf("Type 'help;' or '\\h' for help. Type '\\c' to clear the current input statement.\n");
	while(1){
		printf("mysql>");
		char select[100];
		gets(select);
		del_str_space(select);//删除字符串select多余的空格
		char *p  = strtok(select, " ");
		if(p==NULL){
			continue;
		}
		if(strcmp(p, "quit")==0){
			printf("end...\n");
			break;
		}
		else if(strcmp(p, "help")==0 || strcmp(p, "\\h")==0){
			__help();
		}
		else if(strcmp(p, "clear")==0 || strcmp(p, "\\c")==0){
			system("clear");
		}
		else if(strcmp(p, "show")==0){
			show(p);
		}
		else if(strcmp(p, "use")==0){
			use();//选中某一个数据库
			//printf("using:%s\n", using_db.db_name);
		}
		else if(strcmp(p, "creat")==0){
			__creat();//creat处理函数
		}
		else if(strcmp(p, "insert")==0){
			insert();//insert处理函数
		}
		else if(strcmp(p, "update")==0){
			my_update();
		}
		else if(strcmp(p, "delete")==0){
			my_delete();
		}
		else if(strcmp(p, "select")==0){
			my_select();
		}
		else{
			print_error(p);
		}
	}
}
//打印错误信息
void print_error(char *p){
	printf("ERROR: You have an error in your SQL syntax;\"%s\"\n", p);
}
//删除字符串中所有的空格
void delete_all_space(char *str){
	while(*str!='\0'){
		if(*str==' '){
			strcmp(str, str+1);
			str--;
		}
		str++;
	}
}
//使用一个数据库
void use(){
	char *p;
	p = strtok(NULL, " ");
	using_db.mark = 1;
	strcpy(using_db.db_name, p);
}
//创建表信息
void creatTable(){
	char *p, *q;
	p = strtok(NULL, " ");
	char tableName[1000]="";
	char tableInfo[1000]="";
	q = strchr(p, '(');
	*q = '\0';
	strcpy(tableName, p);//将表名复制出来
	//tableName[strlen(tableName)] = '\n';
	p = strchr(q+1, ')');
	*p = '\0';
	delete_all_space(q+1);
	strcpy(tableInfo, q+1);
	tableInfo[strlen(tableInfo)] = '\n';
	//printf("%s    %s", tableName, tableInfo);
	writeTableStruct(tableName, tableInfo);//将表结构写入数据库
}
//creat组
void __creat(){
	char *p;
	p = strtok(NULL, " ");
	if(p==NULL){
		print_error(p);
		return;
	}
	if(strcmp(p, "database")==0){
		p = strtok(NULL, " ");
		creatDataBase(p);//创建数据库表
	}
	else if(strcmp(p, "table")==0){
		if(using_db.mark!=0){
			creatTable();
			//创建数据库表
		}
		else{
			printf("ERROR: No database selected\n");
		}
	}
	else{
		print_error(p);
	}
}
//show组
void show(char *p){
	p = strtok(NULL, " ");
	if(p==NULL){
		print_error(p);
		return;
	}
	if(strcmp(p, "databases")==0){
		showDataBases();//显示所有数据库
	}
	else if(strcmp(p, "tables")==0){
		if(using_db.mark!=0){//判断是否选用数据库
			showTables();
		}
		else{
			printf("ERROR: No database selected\n");
		}
	}
	else{
		print_error(p);
	}
}
//insert命令处理函数 insert into stu value(1,liuxin);
void insert(){
	if(using_db.mark!=0){//判断是否选用数据库
		_insert();
	}
	else{
		printf("ERROR: No database selected\n");
	}
}
void _insert(){
	char *p;
	char tableName[100];//保存表名
	char tableValues[100];//插入的内容
	p = strtok(NULL, " ");
	if(p==NULL){
		printf("running: %d\n", __LINE__);
		return;
	}
	if(strcmp(p, "into")==0){
		p = strtok(NULL, " ");//stu
		if(p==NULL){
			printf("running: %d\n", __LINE__);
			return;
		}
		strcpy(tableName, p);
		char *q;
		p = strtok(NULL, " ");//values(liuxin)
		q = strchr(p, '(');
		if(q==NULL){
			print_error(p);
			printf("running: %d\n", __LINE__);
			return;
		}
		*q = '\0';
		if(strcmp(p, "values")==0){
			p = strrchr(q+1, ')');
			if(p==NULL){
				print_error(q);
				printf("running: %d\n", __LINE__);
				return;
			}
			*p = '\0';
			strcpy(tableValues, q+1);
		}else{
			print_error(p);
			printf("running: %d\n", __LINE__);
			return;
		}
		//printf("tableName:%s, tableValues:%s\n", tableName, tableValues);
		if(__insert(tableName, tableValues)){
			printf("insert success.\n");
		}else{
			printf("insert err.\n");
		}
	}else{
		printf("running: %d\n", __LINE__);
		print_error(p);
	}
}
//查询
void my_select(){
	if(using_db.mark!=0){//判断是否选用数据库
		_select();
	}
	else{
		printf("ERROR: No database selected\n");
	}
}
void _select(){
	char *p;
	char tableName[100];//保存表名
	p = strtok(NULL, " ");
	if(p==NULL){
		printf("running: %d\n", __LINE__);
		return;
	}
	if(strcmp(p, "*")==0){
		p = strtok(NULL, " ");//from
		if(p==NULL){
			printf("running: %d\n", __LINE__);
			return;
		}
		if(strcmp(p, "from")==0){
			p = strtok(NULL, " ");//tablename
			if(p==NULL){
				printf("running: %d\n", __LINE__);
				return;
			}
			strcpy(tableName, p);//将表名复制出来
		}else{
			print_error(p);
			printf("running: %d\n", __LINE__);
			return;
		}
		//调用底层查询函数
		if(__select(tableName)){
			printf("select success.\n");
		}else{
			printf("select err.\n");
		}
	}else{
		printf("running: %d\n", __LINE__);
		print_error(p);
	}
}
void my_update(){
	if(using_db.mark!=0){//判断是否选用数据库
		_update();
	}
	else{
		printf("ERROR: No database selected\n");
	}
}
void _update(){
	char *p;
	char *tableName;//保存表名
	char *condition;//保存条件
	char *values;	//保存修改的字段和值
	p = strtok(NULL, " ");//表名
	if(p==NULL){
		printf("running: %d\n", __LINE__);
		return;
	}
	tableName = p;
	p = strtok(NULL, " ");//set
	if(p==NULL){
		printf("running: %d\n", __LINE__);
		return;
	}
	if(strcmp(p, "set")==0){
		p = strtok(NULL, " ");//修还的字段和值
		if(p==NULL){
			printf("running: %d\n", __LINE__);
			return;
		}
		values = p;
		p = strtok(NULL, " ");//where
		if(p==NULL){
			printf("running: %d\n", __LINE__);
			return;
		}
		if(strcmp(p, "where")==0){
			p = strtok(NULL, " ");//条件
			if(p==NULL){
				printf("running: %d\n", __LINE__);
				return;
			}
			condition = p;
		}else{
			print_error(p);
			printf("running: %d\n", __LINE__);
			return;
		}
		//调用底层查询函数
		if(__update(tableName, condition, values)){
			printf("update success.\n");
		}else{
			printf("update err.\n");
		}
	}else{
		printf("running: %d\n", __LINE__);
		print_error(p);
	}

}
void my_delete(){
	if(using_db.mark!=0){//判断是否选用数据库
		_delete();
	}
	else{
		printf("ERROR: No database selected\n");
	}
}
void _delete(){
	char *p;
	char *tableName;//保存表名
	char *condition;//保存条件
	p = strtok(NULL, " ");//from
	if(p==NULL){
		printf("running: %d\n", __LINE__);
		return;
	}
	if(strcmp(p, "from")==0){
		p = strtok(NULL, " ");//修还的字段和值
		if(p==NULL){
			printf("running: %d\n", __LINE__);
			return;
		}
		tableName = p;
		p = strtok(NULL, " ");//where
		if(p==NULL){
			printf("running: %d\n", __LINE__);
			return;
		}
		if(strcmp(p, "where")==0){
			p = strtok(NULL, " ");//条件
			if(p==NULL){
				printf("running: %d\n", __LINE__);
				return;
			}
			condition = p;
		}else{
			print_error(p);
			printf("running: %d\n", __LINE__);
			return;
		}
		//调用底层查询函数
		if(__delete(tableName, condition)){
			printf("delete success.\n");
		}else{
			printf("delete err.\n");
		}
	}else{
		printf("running: %d\n", __LINE__);
		print_error(p);
	}
}
