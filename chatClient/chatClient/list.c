//
//  list.c
//  19-聊天客户端
//
//  Created by ccj on 2017/6/21.
//  Copyright © 2017年 ccj. All rights reserved.
//

#include "list.h"
#include "all.h"


static pDownLink downHead=NULL;//正在下载的文件头
/*创建下载文件的链表头*/
void creatDownList()
{
    if(downHead==NULL)
    {
        downHead=(pDownLink)malloc(sizeof(DownLink));
        downHead->next=NULL;
    }
}
/*添加下载文件的信息--->下载文件链表里面*/
void addDownFile(DownFile data)
{
    pDownLink p=(pDownLink)malloc(sizeof(DownLink));
    p->data=data;
    
    p->next=downHead->next;
    downHead->next=p;
}

/*是否下载的小包是同一个大包的分包  -1:代表之前没有过这个包 0:代表之前有这个包*/
int isDownloadingFisrtPacket(DownFile downFile)
{
    pDownLink p=downHead->next;
    for (; p; p=p->next)
    {
        if(strcmp(p->data.fileName, downFile.fileName)==0&&strcmp(p->data.time, downFile.time)==0)
        {
            return 0;
        }
    }
    return -1;
}


/*从正在下载文件链表删除下载文件*/
void deleteDownloadFile(char *fileNme)
{
    pDownLink p=downHead;
    for (; p->next; p=p->next)
    {
        if(strcmp(p->next->data.fileName, fileNme)==0)
        {
            pDownLink q=p->next;
            p->next=q->next;
            free(q);
            printf("\n文件下载完成\n");
            return;
        }
    }
}


/*是否正在下载文件 -1:代表文件没有下载  0:代表正在下载中*/
int isDownloadFile(char *fileName)
{
    pDownLink p=downHead->next;
    for (; p; p=p->next)
    {
        if(strcmp(p->data.fileName, fileName)==0)
        {
            return 0;
        }
    }
    return -1;
}

