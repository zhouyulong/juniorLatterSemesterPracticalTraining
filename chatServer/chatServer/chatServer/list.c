//
//  list.c
//  chatServer
//
//  Created by zyl on 2017/6/22.
//  Copyright © 2017年 zyl. All rights reserved.
//

#include "list.h"
pUserLink userHead=NULL;//注册链表的头

pOnlineLink onlineHead=NULL;//在线链表的头

pUnsendLink unsendHead=NULL;//未发送信息的链表头

pDownLink downHead=NULL;//正在下载的文件头
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
            return;
        }
    }
}

/*创建未发送信息的链表*/
void creatUnsendList()
{
    if(unsendHead==NULL)
    {
        unsendHead=(pUnsendLink)malloc(sizeof(UnsendLink));
        unsendHead->next=NULL;
    }
}

/*创建在线链表*/
void creatOnlineUserList()
{
    if(onlineHead==NULL)
    {
        onlineHead=(pOnlineLink)malloc(sizeof(OnlineLink));
        onlineHead->next=NULL;
    }
}

/*创建注册链表*/
void creatRegisterUserList()
{
    if(userHead==NULL)
    {
        userHead=(pUserLink)malloc(sizeof(UserLink));
        userHead->next=NULL;
    }
}


/*添加未发送信息--->未发送信息链表*/
void addUnsendMsg(UnsendMsg data)
{
    pUnsendLink p=(pUnsendLink)malloc(sizeof(UnsendLink));
    p->data=data;
    
    p->next=unsendHead->next;
    unsendHead->next=p;
}



/*添加在线用户--->在线链表*/
void addOlineUser(OnlineUser data)
{
    pOnlineLink p=(pOnlineLink)malloc(sizeof(OnlineLink));
    p->data=data;
    
    p->next=onlineHead->next;
    onlineHead->next=p;
}


/*判断是否在线*/
OnlineType hasOnlineUserByName(char *name)
{
    pOnlineLink p=onlineHead->next;
    for (; p; p=p->next)
    {
        if(strcmp(p->data.name, name)==0)
        {
            return OnlineTypeYes;
        }
    }
    return OnlineTypeNo;
}


/*添加注册用户--->注册链表*/
void addRegisterUser(User data)
{
    //头插最简单
    pUserLink p=(pUserLink)malloc(sizeof(UserLink));
    p->data=data;
    
    p->next=userHead->next;
    userHead->next=p;
}

/*将未发送的信息保存到文件*/
void saveUnsendMsgToFile()
{
    FILE *fp=fopen(UNSENDMSGFILE, "w");
    if(fp==NULL)
    {
        perror("fopen failed");
        return;
    }
    pUnsendLink p=unsendHead->next;
    for (; p; p=p->next)
    {
        fprintf(fp, "sender=%s receiver=%s msg=%s\n",p->data.sender,p->data.receiver,p->data.msg);
    }
    fclose(fp);
}

/*从文件当中读取未发送的信息*/
void readUnsendMsgFromFile()
{
    creatUnsendList();
    pUnsendLink tail=unsendHead;
    FILE *fp=fopen(UNSENDMSGFILE, "r");
    if(fp==NULL)
    {
        perror("fopen failed");
        return;
    }
    UnsendMsg unsendMsg;
    while (fscanf(fp, "sender=%s receiver=%s msg=%s\n",unsendMsg.sender,unsendMsg.receiver,unsendMsg.msg)!=-1)
    {
        pUnsendLink p=(pUnsendLink)malloc(sizeof(UnsendLink));
        p->data=unsendMsg;
        
        tail->next=p;
        p->next=NULL;
        tail=p;
    }
    fclose(fp);
}




/*将注册信息保存到文件*/
void saveRegisterUserToFile()
{
    FILE *fp=fopen(USERFILENAME, "w");
    pUserLink p=userHead->next;
    for (; p; p=p->next)
    {
        fprintf(fp, "name=%s passwd=%s question=%s answer=%s\n",p->data.name,p->data.passwd,p->data.attachMsg,p->data.attachMsg+MEDIUMSIZE);
    }
    fclose(fp);
}

/*判断注册用户是否存在*/
ExistType hasExistRegisterUser(User user)
{
    pUserLink p=userHead->next;
    for (; p; p=p->next)
    {
        if(strcmp(p->data.name,user.name)==0)
        {
            return ExistTypeYes;
        }
    }
    return ExistTypeNo;
}


/*通过user找到这个人的位置*/
pUserLink getUserData(User user)
{
    pUserLink p=userHead->next;
    for (; p; p=p->next)
    {
        if(strcmp(p->data.name, user.name)==0)
        {
            return p;
        }
    }
    return NULL;
}


/*判断账号和密码的正确性   -1:代表错误;0代表正确*/
int checkNameAndPasswd(User user)
{
    pUserLink p=userHead->next;
    for (; p; p=p->next)
    {
        
        if(strcmp(p->data.name, user.name)==0&&strcmp(p->data.passwd, user.passwd)==0)
        {
            return 0;
        }
    }
    return -1;
}


/*该注册用户的信息*/
void changeUserPasswd(User newUser)
{
    pUserLink p=userHead->next;
    for (; p; p=p->next)
    {
        if(strcmp(p->data.name, newUser.name)==0)
        {
            strcpy(p->data.passwd, newUser.passwd);
            break;
        }
    }
    
}


/*根据在线用户的姓名拿到通信套接字*/
int getSockfdByOnlineUserName(char *name)
{
    pOnlineLink p=onlineHead->next;
    for (; p; p=p->next)
    {
        if(strcmp(p->data.name, name)==0)
        {
            return p->data.fd;
        }
    }
    return -1;
}
/*根据在线套接字拿到在线用户姓名*/
pOnlineLink getOnlineUserNameBySockfd(int fd)
{
    pOnlineLink p=onlineHead->next;
    for (; p; p=p->next)
    {
        if(p->data.fd==fd)
        {
            return p;
        }
    }
    return NULL;
}



/*将重新连接过来的同名的用户里面的sockfd改掉*/
void changeOnlineSockfdByName(char *name,int currentfd)
{
    pOnlineLink p=onlineHead->next;
    for (; p; p=p->next)
    {
        if(strcmp(p->data.name, name)==0)
        {
            p->data.fd=currentfd;
            return;
        }
    }
}


/*当客户端关闭连接的那一刻，删除在线链表中对应用户*/
void outOnlineUserBySockfd(int fd)
{
    pOnlineLink p=onlineHead;
    for (; p->next!=NULL; p=p->next)
    {
        if(p->next->data.fd==fd)
        {
            pOnlineLink q=p->next;
            p->next=q->next;
            free(q);
            printf("删除节点\n");
            return;
        }
    }
}


/*读取文件当中的信息加载链表里面*/
void readRegisterUserFromFile()
{
    creatRegisterUserList();
    pUserLink tail=userHead;
    FILE *fp=fopen(USERFILENAME, "r");
    if(fp==NULL)
    {
        perror("fopen faield");
        return;
    }
    User user;
    while (fscanf(fp, "name=%s passwd=%s question=%s answer=%s\n",user.name,user.passwd,user.attachMsg,user.attachMsg+MEDIUMSIZE)!=-1)
    {
        pUserLink p=(pUserLink)malloc(sizeof(UserLink));
        p->data=user;
        
        tail->next=p;
        p->next=NULL;
        tail=p;
    }
    fclose(fp);
    
}
