#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "common_func.h"
#include "logic.h"
//显示所有的数据文件
void showDataBases(){
	DIR *pd;
	struct dirent *pdir;
	pd = opendir("./data");//打开数据文件夹
	if(pd==NULL)
		goto opendir_err;
	while(1){
		pdir = readdir(pd);
		if(pdir==NULL)
			break;
		if(strcmp(pdir->d_name, ".")==0 || strcmp(pdir->d_name, "..")==0)
			continue;
		if(isDateFile(pdir->d_name)){
			char *p = strrchr(pdir->d_name, '.');
			*p = '\0';
			printf("%s\n", pdir->d_name);
		}
	}
	return;
opendir_err:
	perror("");
	printf("running: %d\n", __LINE__);
	exit(0);
}
//判断是不是数据库文件
int isDateFile(char *fileName){
	char *p;
	p = strrchr(fileName, '.');
	if(strcmp(p, ".mdf")==0)
		return 1;
	return 0;
}
//创建数据库
void creatDataBase(char *dataBaseName){
	char buf[10000]="";
	int fd;
	sprintf(buf, "./data/%s.mdf", dataBaseName);
	fd = open(buf, O_WRONLY|O_CREAT|O_EXCL, 0666);
	if(fd<0){
		printf("dataBases exist.\n");
		return;
	}
	printf("create database databases success.\n");
	close(fd);
}
//检查表是不是已经存在
int tableIsExist(char *tableName){
	char table_head[100]="";//表头
	sprintf(table_head, "%s-start\n", tableName);//拼接表头
	char buf[1000] = "";
	sprintf(buf, "./data/%s.mdf", using_db.db_name);
	FILE *fp;
	fp = fopen(buf, "r");
	if(fp==NULL)
		goto fopen_err;
	char data[1000] = "";
	while(fgets(buf, sizeof(buf), fp)){
		if(strcmp(buf, table_head)==0){
			fclose(fp);
			return 1; 
		}
	}
	fclose(fp);
	return 0;
fopen_err:
	perror("");
	printf("running: %d\n", __LINE__);
	exit(0);
}
//将表结构写入数据库中
void writeTableStruct(char *tableName, char *tableInfo){
	//检查数据库中是否已经存在该表
	if(tableIsExist(tableName)){
		printf("dataBases exist.\n");
		return;
	}
	char table_head[100]="";//表头
	char table_end[100]="";//表尾
	sprintf(table_head, "%s-start\n", tableName);
	sprintf(table_end, "%s-end\n", tableName);
	char buf[1000] = "";
	sprintf(buf, "./data/%s.mdf", using_db.db_name);
	FILE *fp;
	fp = fopen(buf, "a");
	if(fp==NULL)
		goto fopen_err;
	//fputs("\n", fp);
	fputs(table_head, fp);
	fputs(tableInfo, fp);
	fputs(table_end, fp);//表尾
	fclose(fp);
	return;
fopen_err:
	perror("");
	printf("running: %d\n", __LINE__);
	exit(0);
}
void showTables(){
	char buf[1000] = "";
	sprintf(buf, "./data/%s.mdf", using_db.db_name);
	FILE *fp;
	fp = fopen(buf, "r");//打开当前use的数据库文件
	if(fp==NULL)
		goto fopen_err;
	char data[1000];
	char *p;
	while(fgets(data, sizeof(buf), fp)!=NULL){
		if((p=strstr(data, "-start"))!=NULL){
			*p = '\0';
			printf("%s\n", data);
		}
	}
	fclose(fp);//关闭文件
	return;
fopen_err:
	perror("");
	printf("running: %d\n", __LINE__);
	exit(0);
}
int __insert(char *tableName, char *tableValues){
	if(!tableIsExist(tableName)){
		printf("tabel %s is not exist in %s.\n", tableName, using_db.db_name);
		return 0;
	}
	char buf[100];
	sprintf(buf, "./data/%s.mdf", using_db.db_name);
	FILE *fpr;
	FILE *fpw;
	fpr = fopen(buf, "r");//以读写的方式打开文件
	if(fpr==NULL)
		goto fopen_err;
	fpw = fopen("./data/tmp", "w");//以读写的方式打开文件
	if(fpw==NULL)
		goto fopen_err;
	char table_head[100]="";//表头
	char table_end[100]="";//表尾
	sprintf(table_head, "%s-start\n", tableName);
	sprintf(table_end, "%s-end\n", tableName);
	char data[1000];
	while(fgets(data, sizeof(data), fpr)!=NULL){
		fputs(data, fpw);
		if(strcmp(data, table_head)==0){//遍历到表头
			int number=0;
			while(fgets(data, sizeof(data), fpr)!=NULL){
				if(strcmp(data, table_end)==0)//遍历到表尾
					break;
				fputs(data, fpw);
				number++;
			}
			fprintf(fpw, "%d %s\n", number, tableValues);
			fputs(data, fpw);
		}
	}
	fclose(fpr);//关闭文件
	fclose(fpw);//关闭文件
	//删除原始文件
	remove(buf);
	//修改临时文件的名字
	rename("./data/tmp", buf);
	return 1;
fopen_err:
	perror("");
	printf("running: %d\n", __LINE__);
	exit(0);
}
//查询函数
int __select(char *tableName){
	if(!tableIsExist(tableName)){
		printf("tabel %s is not exist in %s.\n", tableName, using_db.db_name);
		return 0;
	}
	char buf[100];//保存数据库名字
	sprintf(buf, "./data/%s.mdf", using_db.db_name);
	FILE *fpr;
	fpr = fopen(buf, "r");//以读写的方式打开数据库文件
	if(fpr==NULL)
		goto fopen_err;
	char table_head[100]="";//表头
	char table_end[100]="";//表尾
	sprintf(table_head, "%s-start\n", tableName);
	sprintf(table_end, "%s-end\n", tableName);
	char data[1000];//临时保存读取的数据
	int count;//统计记录数
	count = -1;//清零,去掉行属性的一行
	while(fgets(data, sizeof(data), fpr)!=NULL){
		if(strcmp(data, table_head)==0){//遍历到表头
			while(fgets(data, sizeof(data), fpr)!=NULL){
				if(strcmp(data, table_end)==0)//遍历到表尾
					break;
				fputs(data, stdout);
				count++;
			}
			printf("共查询到%d条记录.\n", count);
			break;
		}
	}
	fclose(fpr);//关闭文件
	return 1;
fopen_err:
	perror("");
	printf("running: %d\n", __LINE__);
	exit(0);
}
//获得查询字段在表结构中的位置(以逗号分隔)
int getMark(char *tableStructInfo, char *filed){
	int count=0;
	char *ptmp = strstr(tableStructInfo, filed);
	while(tableStructInfo!=ptmp){
		if(*tableStructInfo==','){
			count++;
		}
		tableStructInfo++;
	}
	return count;
}
char *findPosByMark(char *data, int filedMark){
	char *p;
	p = strchr(data, ' ');
	p++;
	int count = 0;
	while(*p){
		if(count==filedMark){
			return p;
		}
		if(*p==',')
			count++;
		p++;
	}
	return NULL;
}
int __update(char *tableName, char *condition, char *values){
	if(!tableIsExist(tableName)){
		printf("tabel %s is not exist in %s.\n", tableName, using_db.db_name);
		return 0;
	}
	char buf[100];//保存数据库文件名字
	sprintf(buf, "./data/%s.mdf", using_db.db_name);
	FILE *fpr;
	FILE *fpw;
	fpr = fopen(buf, "r");//以读写的方式打开文件
	if(fpr==NULL)
		goto fopen_err;
	fpw = fopen("./data/tmp", "w");//以读写的方式打开文件
	if(fpw==NULL)
		goto fopen_err;
	char table_head[100]="";//表头
	char table_end[100]="";//表尾
	sprintf(table_head, "%s-start\n", tableName);
	sprintf(table_end, "%s-end\n", tableName);
	char data[1000];//临时保存读取的数据
	int count;//统计修改的记录数
	count = 0;//清零
	while(fgets(data, sizeof(data), fpr)!=NULL){
		fputs(data, fpw);
		if(strcmp(data, table_head)==0){//遍历到表头
			fgets(data, sizeof(data), fpr);//读取表结构信息
			fputs(data, fpw);
			char *p = strchr(condition, '=');
			char *q = strchr(values, '=');
			*p = '\0';
			*q = '\0';
			p++;
			q++;
			char *conditionFiled = condition;
			char *conditionValue = p;
			char *setFiled = values;
			char *setValue = q;
			int conditionFiledMark = getMark(data, conditionFiled);
			int setFiledMark = getMark(data, setFiled);
			while(fgets(data, sizeof(data), fpr)!=NULL){
				if(strcmp(data, table_end)==0)//遍历到表尾
					break;
				char *ptmp;
				if((ptmp=findPosByMark(data, conditionFiledMark))!=NULL&&ptmp==strstr(ptmp, conditionValue)){
					ptmp = findPosByMark(data, setFiledMark);
					char *pmark = strchr(ptmp, ',');
					//字符串替换
					if(pmark==NULL){
						strcpy(ptmp, setValue);
						strcat(ptmp, "\n");
					}else{
						strcpy(ptmp, pmark);
						memmove(ptmp+strlen(setValue), ptmp, strlen(ptmp)+1);
						strncpy(ptmp, setValue, strlen(setValue));
					}
					count++;
				}
				fputs(data, fpw);
			}
			fputs(data, fpw);
			printf("共修改了%d条记录.\n", count);
			break;
		}
	}
	fclose(fpr);//关闭文件
	fclose(fpw);//关闭文件
	//删除原始文件
	remove(buf);
	//修改临时文件的名字
	rename("./data/tmp", buf);
	return 1;
fopen_err:
	perror("");
	printf("running: %d\n", __LINE__);
	exit(0);
	
}
int __delete(char *tableName, char *condition){
	if(!tableIsExist(tableName)){
		printf("tabel %s is not exist in %s.\n", tableName, using_db.db_name);
		return 0;
	}
	char buf[100];
	sprintf(buf, "./data/%s.mdf", using_db.db_name);
	FILE *fpr;
	FILE *fpw;
	fpr = fopen(buf, "r");//以读写的方式打开文件
	if(fpr==NULL)
		goto fopen_err;
	fpw = fopen("./data/tmp", "w");//以读写的方式打开文件
	if(fpw==NULL)
		goto fopen_err;
	char table_head[100]="";//表头
	char table_end[100]="";//表尾
	sprintf(table_head, "%s-start\n", tableName);
	sprintf(table_end, "%s-end\n", tableName);
	char data[1000];
	while(fgets(data, sizeof(data), fpr)!=NULL){
		fputs(data, fpw);
		if(strcmp(data, table_head)==0){//遍历到表头
			fgets(data, sizeof(data), fpr);//读取表结构信息
			fputs(data, fpw);
			char *p = strchr(condition, '=');
			*p = '\0';
			p++;
			char *conditionFiled = condition;
			char *conditionValue = p;
			int conditionFiledMark = getMark(data, conditionFiled);
			int count=0;
			while(fgets(data, sizeof(data), fpr)!=NULL){
				if(strcmp(data, table_end)==0)//遍历到表尾
					break;
				char *ptmp;
				if((ptmp=findPosByMark(data, conditionFiledMark))!=NULL&&ptmp==strstr(ptmp, conditionValue)){
					count++;
					continue;
				}
				fputs(data, fpw);
			}
			fputs(data, fpw);
			printf("共删除了%d条记录.\n", count);
		}
	}
	fclose(fpr);//关闭文件
	fclose(fpw);//关闭文件
	//删除原始文件
	remove(buf);
	//修改临时文件的名字
	rename("./data/tmp", buf);
	return 1;
fopen_err:
	perror("");
	printf("running: %d\n", __LINE__);
	exit(0);

}
