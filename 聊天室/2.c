
#include "StdSqlite.h"
#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>

char *count = "0";

int callback(void *data, int argc, char **argv, char **azColName)
{
    if (argc > 0 && argv[0])
    {
        // 获取查询结果字符串的长度
        int length = strlen(argv[0]);

        // 为 count 分配足够的空间，包括 null 字符
        count = (char *)malloc((length + 1) * sizeof(char));

        // 复制查询结果字符串到 count，并在末尾添加 null 字符
        strncpy(count, argv[0], length);
        count[length] = '\0';
    }

    return 0;
}

int main()
{
    SQL *s = InitSqlite("Users.db");

    char *prolist[] = {"count", "text", "friend", "text"};
    //char *tableName = "friend";
    CreateTable(s,"friend",prolist,sizeof(prolist) / sizeof(prolist[0]) / 2);
    // char *name = "shaojun";
    // char *password = "121";
    // char sql[4096] = {0};
    // sprintf(sql, "select count from %s where name = '%s' and password = '%s';", tableName, name, password);
    // if (sqlite3_exec(s->db, sql, callback, NULL, NULL) != SQLITE_OK)
    // {
    //     printf("insert table error :%s\n!", sqlite3_errmsg(s->db));
    // }
    // printf("%s\n", count);
    return 0;
}