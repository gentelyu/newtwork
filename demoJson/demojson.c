#include <stdio.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <stdlib.h>

int main()
{
    #if 0
    {"way" : 1, "name" : "zhangsan", "passwd" : "abc123"}
    #endif

    /* 新建json对象 */
    struct json_object * jsonObj = json_object_new_object();
    if (jsonObj == NULL)
    {
        perror("new json object error");
        exit(-1);
    }


    struct json_object * wayValue = json_object_new_int64(32);
    if (wayValue == NULL)
    {
        perror("new wayValue object error");
        // json_object_put(jsonObj);
        exit(-1);
    }
    int ret = json_object_object_add(jsonObj, "way", wayValue);
    if (ret < 0)
    {
        /* todo... */
    }


    struct json_object * nameValue = json_object_new_string("zhangsan");
    if (nameValue == NULL)
    {
        perror("new nameValue object error");
        exit(-1);
    }
    json_object_object_add(jsonObj, "name", nameValue);

   
    /* 将json转换成字符串 */
    const char * str = json_object_to_json_string(jsonObj);
    printf("str:%s\n", str);


/* demo 服务器 =》  */
    printf("=====================demo server=======================\n");
    /* 将字符串转化成json对象 */
#if 0
    // struct json_object * ageObj = json_object_new_string(str);
#else
    struct json_object * newJsonObj = json_tokener_parse(str);
#endif

    struct json_object * wayVal = json_object_object_get(newJsonObj, "way");
    if (wayVal == NULL)
    {
        printf("get wayValue error\n");
        exit(-1);
    }

    /* 解析int类型的json对象 */
    printf("way:%ld\n", json_object_get_int64(wayVal));


    struct json_object * nameVal = json_object_object_get(newJsonObj, "name");
    if (nameVal == NULL)
    {
        perror("get nameVal error\n");
        exit(-1);
    }

    printf("name:%s\n", json_object_get_string(nameVal));


    /* 释放json对象 */
    json_object_put(jsonObj);
    return 0;
}