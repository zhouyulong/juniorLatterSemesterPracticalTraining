//
//  tool.c
//  chatServer
//
//  Created by zyl on 2017/6/22.
//  Copyright © 2017年 zyl. All rights reserved.
//
//


#include "tool.h"
#include "list.h"

/*分割字符串*/
void splitStringByCh(char *str,char ch,char *pre,char *next)
{
    while (*str!=ch)
    {
        *pre=*str;
        pre++;
        str++;
    }
    *pre=0;
    str++;
    while (*str)
    {
        *next=*str;
        next++;
        str++;
    }
    *next=0;
}

/*计算字符串冒号的个数*/
int getColonCountByString(char *str)
{
    int count=0;
    while (*str)
    {
        if(*str==':')
        {
            count++;
        }
        str++;
    }
    return count;
}

/*专门处理字符串*/
int handleString(char *str,char (*person)[MINSIZE],char *msg)
{
    //x1:x2:x3:zaima
    int colonCount=getColonCountByString(str);
    int i=0;
    char *next=str;
    while (i<colonCount)
    {
        splitStringByCh(next, ':', person[i], next);
        i++;
    }
    strcpy(msg, next);
    return colonCount;
}

/*拿到当前的时间*/
char *getCurrentTime(char *str)
{
    time_t p;
    //拿到一个相对时间
    time(&p);
    //将时间本地化
    struct tm *t=localtime(&p);
    sprintf(str, "%d-%d-%d %d:%d:%d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
    
    return str;
}


/*检查文件是否存在  -1:代表不存在或者文件打开失败   0:代表文件存在*/
int checkFileIsExist(char *fileName)
{
    DIR *dir=opendir(SERVERFILE);
    if(dir==NULL)
    {
        return -1;
    }
    struct dirent *d;
    while ((d=readdir(dir)))
    {
        if(d->d_type==DT_REG&&strcmp(d->d_name, fileName)==0)
        {
            return 0;
        }
    }
    closedir(dir);
    return -1;
}

