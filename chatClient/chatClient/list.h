//
//  list.h
//  19-聊天客户端
//
//  Created by ccj on 2017/6/21.
//  Copyright © 2017年 ccj. All rights reserved.
//

#ifndef list_h
#define list_h

#include <stdio.h>
#include "all.h"
typedef struct DownFile
{
    char fileName[MEDIUMSIZE];
    char time[MEDIUMSIZE];
    
}DownFile;

//宏：构建不同链表节点的类型
#define LinkConstruct(LinkName,dataType) \
typedef struct LinkName\
{\
dataType data;\
struct LinkName *next;\
}LinkName,*p##LinkName;

LinkConstruct(DownLink, DownFile)

//函数的声明
void addDownFile(DownFile data);

void deleteDownloadFile(char *fileNme);

int isDownloadingFisrtPacket(DownFile downFile);
int isDownloadFile(char *fileName);
void creatDownList();

#endif /* list_h */
