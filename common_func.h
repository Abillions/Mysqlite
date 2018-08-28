#ifndef COMMON_FUNC_H
#define COMMON_FUNC_H

void showDataBases();
int isDateFile(char *fileName);
void creatDataBase(char *dataBaseName);
void writeTableStruct(char *tableName, char *tableInfo);
int tableIsExist(char *tableName);
void showTables();
int __select(char *tableName);
int __insert(char *tableName, char *tableValues);
int __update(char *tableName, char *condition, char *values);
int getMark(char *tableStructInfo, char *filed);
char *findPosByMark(char *data, int filedMark);
int __delete(char *tableName, char *condition);
#endif
