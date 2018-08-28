#ifndef LOGIC_H
#define LOGIC_H
struct use_db{
	int mark;
	char db_name[1000];
};
struct use_db using_db;

//初始化数据配置
void __init();
void __index();
void __help();
void print_error(char *p);
void show(char *p);
void __creat();
void use();
void creatTable();
void delete_all_space(char *str);
//插入
void insert();
void _insert();
//查询
void my_select();
void _select();
//插入
void my_update();
void _update();
//删除
void my_delete();
void _delete();
#endif
