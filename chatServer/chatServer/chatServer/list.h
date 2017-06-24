//
//  list.h
//  chatServer
//
//  Created by zyl on 2017/6/22.
//  Copyright © 2017年 zyl. All rights reserved.
//
//

#ifndef list_h
#define list_h

#include <stdio.h>
#include "all.h"

//宏：构建不同链表节点的类型
#define LinkConstruct(LinkName,dataType) \
typedef struct LinkName\
{\
dataType data;\
struct LinkName *next;\
}LinkName,*p##LinkName;

LinkConstruct(UserLink, User)


typedef struct OnlineUser
{
    char name[MINSIZE];
    int fd;
}OnlineUser;

LinkConstruct(OnlineLink, OnlineUser)


typedef struct UnsendMsg
{
    char sender[MINSIZE];//发送者账号
    char receiver[MINSIZE];//接收者账号
    char msg[MEDIUMSIZE];//未发送的信息内容
}UnsendMsg;

LinkConstruct(UnsendLink, UnsendMsg)

typedef struct DownFile
{
    char fileName[MEDIUMSIZE];
    char time[MEDIUMSIZE];
}DownFile;

LinkConstruct(DownLink, DownFile)


//判断注册用户是否存在的类型
typedef enum ExistType
{
    ExistTypeYes,
    ExistTypeNo
    
}ExistType;


//判断在线用户的存在类型
typedef enum OnlineType
{
    OnlineTypeYes,
    OnlineTypeNo
    
}OnlineType;


//宏
#define USERFILENAME  "/Users/yl/Desktop/c/serFl/registerUser.txt"
#define UNSENDMSGFILE "/Users/yl/Desktop/c/serFl/unsendMsg.txt"
#define SERVERFILE "/Users/yl/Desktop/c/serFl/downFile/"

//函数的声明
ExistType hasExistRegisterUser(User user);

void addRegisterUser(User data);
void creatDownList();

void creatOnlineUserList();
void readRegisterUserFromFile();
void readUnsendMsgFromFile();
void outOnlineUserBySockfd(int fd);
pOnlineLink getOnlineUserNameBySockfd(int fd);
void addOlineUser(OnlineUser data);
void changeOnlineSockfdByName(char *name,int currentfd);
int getSockfdByOnlineUserName(char *name);
OnlineType hasOnlineUserByName(char *name);
void deleteDownloadFile(char *fileNme);
void addDownFile(DownFile data);
int isDownloadingFisrtPacket(DownFile downFile);
pUserLink getUserData(User user);
void saveUnsendMsgToFile();
void addUnsendMsg(UnsendMsg data);
void changeUserPasswd(User newUser);
int checkNameAndPasswd(User user);
void saveRegisterUserToFile();

//变量的声明
extern pUserLink userHead;//注册链表的头

extern pOnlineLink onlineHead;//在线链表的头

extern pUnsendLink unsendHead;//未发送信息的链表头

extern pDownLink downHead;//正在下载的文件头

#endif /* list_h */
