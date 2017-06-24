//
//  tool.c
//  19-聊天客户端
//
//  Created by ccj on 2017/6/21.
//  Copyright © 2017年 ccj. All rights reserved.
//

#include "tool.h"

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

/*拿到字符串当中的所有文件名称保存到字符串数组*/
int getFileNames(char *str,char (*fileName)[MEDIUMSIZE])
{
    //    app.png:iOS远程推送.docx:Qt.pdf:registerUser.txt
    int colonCount=getColonCountByString(str);
    int i=0;
    char *next=str;
    while (i<colonCount)
    {
        splitStringByCh(next, ':', fileName[i], next);
        i++;
    }
    strcpy(fileName[i], next);
    return colonCount+1;
}


/*本地验证消息的正确性  -1:错误  0:正确*/
int checkMsgIsCorrect(char *msg)
{
    //1.有没有冒号
    while (*msg)
    {
        if(*msg==':')
        {
            break;
        }
        else if(*msg!=':')
        {
            msg++;
        }
    }
    if(*msg==0)
    {
        //说明没有冒号
        return -1;
    }
    else
    {
        //说明有冒号,有冒号就判断\0前面一个字符是否是:
        while (*msg)
        {
            msg++;
        }
        msg--;
        if(*msg==':')
        {
            //来到这说明消息内容为空
            return -1;
        }
        else
        {
            return 0;
        }
    }
    return 0;
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

/*拿到最后一个文件名*/
void getLastFileName(char *str,char *lastFileName)
{
    // /Users/ccj/Desktop/app.png\0
    char *end=str+strlen(str)-1;//end就是指向字符串的最后一个字符，\0前面的字符
    while (*end!='/')
    {
        end--;
    }
    end++;//end->'a'
    while (*end)
    {
        *lastFileName=*end;
        end++;
        lastFileName++;
    }
    *lastFileName=0;
}


