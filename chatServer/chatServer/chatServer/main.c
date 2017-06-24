
//
//  main.c
//  chatServer
//
//  Created by zyl on 2017/6/20.
//  Copyright © 2017年 zyl. All rights reserved.
//

#include "all.h"
#include "list.h"
#include "tool.h"
#include <unistd.h>
void handleRegisterRequest(int fd,User user)
{
    /*
     0>该user的信息是否已经被注册过
     1>将注册的用户保存到链表里面
     2>还需要将信息保存到文件里面，在程序一启动的时候就读取信息加载到链表
     */
    MsgContent msgContent;
    if(hasExistRegisterUser(user)==ExistTypeYes)
    {
        //来到这说明姓名已经被注册过
        //将注册的反馈发送给客户端
        msgContent.msgType=MsgTypeRegisterRespondFaild;
        strcpy(msgContent.attachMsg, "用户名已存在");
    }
    else
    {
        addRegisterUser(user);
        saveRegisterUserToFile();//把注册信息保存到注册文件
        msgContent.msgType=MsgTypeRegisterRespondSuccess;
        strcpy(msgContent.attachMsg, "注册成功");
    }
    write(fd, &msgContent, sizeof(msgContent));
    
}



void handleForgetPasswdRequest(int fd,User user)//User 一个结构体类型
{
    pUserLink p=getUserData(user);
    MsgContent msgContent;
    if(p==NULL)
    {
        msgContent.msgType=MsgTypeForgetPasswdRespondFailed;
        strcpy(msgContent.attachMsg, "账号不存在");
    }
    else
    {
        msgContent.msgType=MsgTypeForgetPasswdRespondSuccess;
        //发送密保问题
        strcpy(msgContent.attachMsg,p->data.attachMsg);
    }
    //将忘记密码的反馈信息，发送给客户端
    write(fd, &msgContent, sizeof(msgContent));
    
    read(fd, &msgContent, sizeof(msgContent));
    if(strcmp(msgContent.attachMsg,p->data.attachMsg+MEDIUMSIZE)==0)
    {
        //
        msgContent.msgType=MsgTypeForgetPasswdRespondSuccess;
        strcpy(msgContent.attachMsg,p->data.passwd);
    }
    else
    {
        msgContent.msgType=MsgTypeForgetPasswdRespondFailed;
        strcpy(msgContent.attachMsg, "密保答案错误");
    }
    write(fd, &msgContent, sizeof(msgContent));
    
}




/*处理客户端的修改密码请求*/
void handleChangePasswdRequest(char *name,int fd)
{
    char attachMsg[MAXSIZE];
    read(fd, attachMsg, sizeof(attachMsg));
    char oldPasswd[MEDIUMSIZE],newPasswd[MEDIUMSIZE];
    memcpy(oldPasswd, attachMsg, sizeof(oldPasswd));
    memcpy(newPasswd, attachMsg+MEDIUMSIZE, sizeof(newPasswd));
    
    User user;
    strcpy(user.name, name);
    strcpy(user.passwd, oldPasswd);
    int ret=checkNameAndPasswd(user);
    MsgContent msgContent;
    if(ret==0)
    {
        //账号和旧密码正确
        strcpy(user.passwd, newPasswd);
        //更改注册链表里面信息
        changeUserPasswd(user);
        //保存到文件
        saveRegisterUserToFile();
        msgContent.msgType=MsgTypeChangePasswdRespondSuccess;
        strcpy(msgContent.attachMsg, "修改成功");
        write(fd, &msgContent, sizeof(msgContent));
    }
    else if(ret<0)
    {
        msgContent.msgType=MsgTypeChangePasswdRespondFailed;
        strcpy(msgContent.attachMsg, "原密码输入错误");
        write(fd, &msgContent, sizeof(msgContent));
    }
}


/*发送消息给所有的在线用户*/
void sendMsgToAllOnlineUser(int fd,char *attachMsg)
{
    
    MsgContent msgContent;
    msgContent.msgType=MsgTypechatRespondSuccess;
    strcpy(msgContent.attachMsg, attachMsg);
    pOnlineLink p=onlineHead->next;
    for (; p; p=p->next)
    {//不能发给自己
        if(p->data.fd==fd)
        {
            continue;
        }
        write(p->data.fd,&msgContent, sizeof(msgContent));
    }
    
}

/*处理客户端发送消息请求*/
void handleChatRequest(char *sendName,int fd)
{
    char attachMsg[MAXSIZE];
    //all:daskddsada
    read(fd, attachMsg, sizeof(attachMsg));
    char person[MINSIZE][MINSIZE],msg[MEDIUMSIZE];
    if(strncmp(attachMsg, "all:",4)==0)
    {//来到这就说明需要将信息发送给所有的在线用户
        char temp[MINSIZE];
        splitStringByCh(attachMsg, ':', temp, msg);
        /*
         拿到注册链表里面的每一个人员的账号，遍历在线链表
         */
        pUserLink p=userHead->next;
        for (; p; p=p->next)
        {
            pOnlineLink q=onlineHead->next;
            for (; q; q=q->next)
            {
                //发送有用的信息
                if(strcmp(p->data.name, q->data.name)&&strcmp(msg, "我上线了...")&&strcmp(msg,"我下线了..."))
                {//来到这说明注册人员不在线
                    UnsendMsg unsendMsg;
                    strcpy(unsendMsg.sender, sendName);
                    strcpy(unsendMsg.receiver, p->data.name);
                    strcpy(unsendMsg.msg, msg);
                    addUnsendMsg(unsendMsg);
                }
            }
        }
        saveUnsendMsgToFile();
        
        strcpy(attachMsg, sendName);
        strcat(attachMsg, ":");
        strcat(attachMsg, msg);
        sendMsgToAllOnlineUser(fd, attachMsg);
        return;
    }
    
    int personCount=handleString(attachMsg, person, msg);
    int i=0;
    MsgContent msgContent,msgContent1;
    int flag=0;
    for (; i<personCount; i++)
    {//来到这才能拿到每一个收信人的信息
        
        //收信人在不在注册链表中
        User user;
        strcpy(user.name, person[i]);
        pUserLink p=getUserData(user);
        if(p==NULL)
        {//来到这说明收信人不在注册链表中
            if(flag==0)
            {
                msgContent1.msgType=MsgTypeChatRespondFailed;
                //第一次来到xx1
                strcpy(msgContent1.attachMsg, person[i]);
                flag++;
            }
            else
            {
                strcat(msgContent1.attachMsg, ":");
                strcat(msgContent1.attachMsg, person[i]);
            }
        }
        int getSockfdByOnlineUserName(char *name);
        int onlinefd=getSockfdByOnlineUserName(person[i]);
        if(strcmp(person[i], sendName)==0)
        {//多聊的时候不能发送给自己
            continue;
        }
        if(onlinefd<0)
        {//来到这说明收信人不在线
            if (p!=NULL)
            {//来到这说明在注册链表里面
                UnsendMsg unsendMsg;
                strcpy(unsendMsg.sender,sendName);
                strcpy(unsendMsg.receiver, p->data.name);
                strcpy(unsendMsg.msg, msg);
                addUnsendMsg(unsendMsg);
            }
            continue;
        }
        msgContent.msgType=MsgTypechatRespondSuccess;
        //拼接发送者的信息和发送的内容，再转发给另外一个客户端
        strcpy(msgContent.attachMsg,sendName);
        strcat(msgContent.attachMsg, ":");
        strcat(msgContent.attachMsg, msg);
        write(onlinefd, &msgContent, sizeof(msgContent));
    }
    if(flag>0)
    {
        strcat(msgContent1.attachMsg, "-->这些收信人没有注册过...");
        write(fd, &msgContent1, sizeof(msgContent1));
    }
    else if(flag==0)
    {
        msgContent1.msgType=MsgTypechatRespondSuccess;
        strcpy(msgContent1.attachMsg,"成功发送信息");
        write(fd, &msgContent1, sizeof(msgContent1));
    }
    saveUnsendMsgToFile();
}

/*处理发送离线消息*/
void sendOfflineMsg(char *name,int fd)
{
    MsgContent msgContent;
    msgContent.msgType=MsgTypechatRespondSuccess;
    pUnsendLink p=unsendHead;
    for (; p->next; )
    {
        if(strcmp(p->next->data.receiver, name)==0)
        {
            sprintf(msgContent.attachMsg, "%s:%s",p->next->data.sender,p->next->data.msg);
            write(fd, &msgContent, sizeof(msgContent));
            
            //发送完成之后，应该将该节点信息从链表中删除
            pUnsendLink q=p->next;
            p->next=q->next;
            free(q);
        }
        else
        {
            p=p->next;
        }
    }
    saveUnsendMsgToFile();
    
}

/*处理获取文件列表请求*/
void handleGetFileListRequest(int fd)
{
    MsgContent msgContent;
    DIR *dir=opendir(SERVERFILE);
    if(dir==NULL)
    {
        //来到这将错误信息发送给客户端
        msgContent.msgType=MsgTypeGetFileListRespondFailed;
        strcpy(msgContent.attachMsg, strerror(errno));
        write(fd, &msgContent, sizeof(msgContent));
        return;
    }
    msgContent.msgType=MsgTypeGetFileListRespondSuccess;
    struct dirent *d;
    int flag=0;
    while ((d=readdir(dir)))
    {
        if(d->d_type==DT_REG)
        {
            //来到这代表是普通文件
            if(flag==0)
            {
                strcpy(msgContent.attachMsg, d->d_name);
            }
            else
            {
                strcat(msgContent.attachMsg, ":");
                strcat(msgContent.attachMsg, d->d_name);
            }
            flag++;
        }
    }
    //    app.png:iOS远程推送.docx:Qt.pdf:registerUser.txt
    write(fd, &msgContent, sizeof(msgContent));
    closedir(dir);
}






/*异步线程处理下载任务*/
void *asyncHandleDownloadFile(void *sockfd)
{
    ThreadArgs *p=(ThreadArgs *)sockfd;
    int fd=p->fd;
    
    //来到这就说明文件存在
    File file;
    strcpy(file.fileName,p->fileName);
    getCurrentTime(file.time);
    free(p);
    char path[MEDIUMSIZE];
    sprintf(path, "%s%s",SERVERFILE,file.fileName);
    int fd1=open(path,O_RDONLY);
    long long left=lseek(fd1, 0, SEEK_END);
    lseek(fd1, 0, SEEK_SET);
    long long offset=0;
    int ret;
    
    while (left)
    {
        if(left>MAXIOSIZE)
        {
            ret=(int)read(fd1, file.text, MAXIOSIZE);
        }
        else
        {
            ret=(int)read(fd1, file.text, left);
        }
        left-=ret;
        file.offset=offset;
        offset+=ret;
        file.left=left;
        file.textSize=ret;
        MsgType msgType=MsgTypeDownloadFileRespondSuccess;
        ret=(int)write(fd, &msgType, sizeof(msgType));
        if(ret<0)
        {
            pthread_exit(0);
        }
        ret=(int)write(fd, &file, sizeof(file));
        if(ret<0)
        {
            pthread_exit(0);
        }
        usleep(100000);
    }
    close(fd1);
    
    return NULL;
}

/*处理客户端的下载文件的请求*/
void handleDownloadFileRequset(int fd)
{
    //先拿到文件名，去服务器比对文件名是否正确
    char attachMsg[MAXSIZE];
    read(fd, attachMsg, sizeof(attachMsg));
    int ret=checkFileIsExist(attachMsg);
    MsgContent msgContent;
    if(ret<0)
    {
        msgContent.msgType=MsgTypeDownloadFileRespondFailed;
        strcpy(msgContent.attachMsg, "文件不存在");
        write(fd, &msgContent, sizeof(msgContent));
        return;
    }
    /*
     读文件的操作比较耗时，会阻塞当前线程的运行，想到开线程执行任务，不影响接收其他信息
     */
    ThreadArgs *p=(ThreadArgs *)malloc(sizeof(ThreadArgs));
    p->fd=fd;
    strcpy(p->fileName,attachMsg);
    
    pthread_t pid;
    pthread_create(&pid, NULL, asyncHandleDownloadFile, (void *)p);
    
}

/*处理客户端的上传文件请求*/
void handleUploadFileRequest(int fd)
{
    File file;
    read(fd, &file, sizeof(file));
    
    DownFile downFile;
    strcpy(downFile.fileName,file.fileName);
    strcpy(downFile.time, file.time);
    int ret=isDownloadingFisrtPacket(downFile);
    char path[MEDIUMSIZE];
    sprintf(path, "%s%s",SERVERFILE,file.fileName);
    if(ret<0)
    {  //第一次接受到某个文件的小包，就去创建文件，写内容到文件
        
        int fd1=open(path, O_CREAT|O_WRONLY|O_TRUNC,0644);
        write(fd1, file.text, file.textSize);
        close(fd1);
        addDownFile(downFile);
    }
    else
    {//定位到偏移量的位置，写内容到文件
        
        int fd1=open(path, O_WRONLY);
        lseek(fd1, file.offset, SEEK_SET);
        write(fd1, file.text, file.textSize);
        close(fd1);
    }
    if(file.left==0)
    {
        deleteDownloadFile(file.fileName);
    }
    
}

/*处理客户端重命名文件的请求*/
void handleRenameFileRequest(int fd)
{
    char attachMsg[MAXSIZE];
    read(fd, attachMsg, sizeof(attachMsg));
    char oldName[MEDIUMSIZE],newName[MEDIUMSIZE];
    memcpy(oldName, attachMsg, sizeof(oldName));
    memcpy(newName, attachMsg+MEDIUMSIZE, sizeof(newName));
    
    //先检查服务器有没有oldName，如果有就改名称
    int ret=checkFileIsExist(oldName);
    char oldPath[MEDIUMSIZE],newPath[MEDIUMSIZE];
    MsgContent msgContent;
    if(ret==0)
    {//来到这说明文件存在
        
        sprintf(oldPath, "%s%s",SERVERFILE,oldName);
        sprintf(newPath, "%s%s",SERVERFILE,newName);
        ret=rename(oldPath, newPath);
        if(ret<0)
        {
            
            msgContent.msgType=MsgTypeRenameFileRespondFailed;
            strcpy(msgContent.attachMsg, strerror(errno));
        }
        else
        {
            msgContent.msgType=MsgTypeRenameFileRespondSuccess;
            strcpy(msgContent.attachMsg, "重命名成功...");
        }
    }
    else
    {//来到这说明文件不存在
        msgContent.msgType=MsgTypeRenameFileRespondFailed;
        strcpy(msgContent.attachMsg, "文件不存在...");
    }
    write(fd, &msgContent, sizeof(msgContent));
    
    
}

/*处理客户端的删除文件请求*/
void handleDeleteFileRequset(int fd)
{
    char attachMsg[MAXSIZE];
    read(fd, attachMsg, sizeof(attachMsg));
    
    //检查文件是否存在
    int ret=checkFileIsExist(attachMsg);
    char path[MEDIUMSIZE];
    MsgContent msgContent;
    if(ret==0)
    {
        sprintf(path, "%s%s",SERVERFILE,attachMsg);
        ret=remove(path);
        if(ret<0)
        {
            msgContent.msgType=MsgTypeDeleteFileRespondFailed;
            strcpy(msgContent.attachMsg, strerror(errno));
        }
        else
        {
            msgContent.msgType=MsgTypeDeleteFileRespondSuccess;
            strcpy(msgContent.attachMsg, "删除成功...");
        }
    }
    else
    {
        msgContent.msgType=MsgTypeDeleteFileRespondFailed;
        strcpy(msgContent.attachMsg, "文件不存在...");
    }
    write(fd, &msgContent, sizeof(msgContent));
}

/*处理登陆成功之后的请求*/
void handleLoginSuccess(char *name,int fd)
{
    //发送离线消息
    sendOfflineMsg(name,fd);
    
    MsgType msgType;
    int ret=0;
    while (1)
    {
        ret=(int)read(fd, &msgType, sizeof(msgType));
        if(ret==0)
        {
            break;
        }
        switch (msgType)
        {
            case MsgTypeChangePasswdRequest:
                handleChangePasswdRequest(name,fd);
                break;
            case MsgTypeChatRequest:
                handleChatRequest(name,fd);
                break;
            case MsgTypeGetFileListRequest:
                handleGetFileListRequest(fd);
                break;
            case MsgTypeDownloadFileRequest:
                handleDownloadFileRequset(fd);
                break;
            case MsgTypeUploadFileRequest:
                handleUploadFileRequest(fd);
                break;
            case MsgTypeRenameFileRequest:
                handleRenameFileRequest(fd);
                break;
            case MsgTypeDeleteFileRequest:
                handleDeleteFileRequset(fd);
                break;
            default:
                break;
        }
    }
}





/*处理登陆请求*/
void handleLoginRequest(int fd,User user)
{
    /*
     1.账号是否存在
     2.账号是否已经在线
     3.密码是否正确
     */
    MsgContent msgContent;
    //1.
    ExistType existType=hasExistRegisterUser(user);
    if(existType==ExistTypeNo)
    {
        msgContent.msgType=MsgTypeLoginRespondFailed;
        strcpy(msgContent.attachMsg, "账号或者密码错误");
        //发送登陆的反馈信息给客户端
        write(fd, &msgContent, sizeof(msgContent));
    }
    else if(existType==ExistTypeYes)
    {
        //2.
        OnlineType onlinType=hasOnlineUserByName(user.name);
        if(onlinType==OnlineTypeYes)
        {
            //来到这说明账号已经在线
            msgContent.msgType=MsgTypeLoginRespondFailed;
            strcpy(msgContent.attachMsg, "你已被迫下线,请及时修改密码...");
            //拿到在线用户对应通信套接字
            int onlinefd=getSockfdByOnlineUserName(user.name);
            write(onlinefd, &msgContent, sizeof(msgContent));
            
            //1.将原先的连接从在线链表中移除
            //2.将现在的连接添加到链表里面
            /*
             第一种方法：效率低
             void outOnlineUserBySockfd(int fd);
             outOnlineUserBySockfd(onlinefd);
             OnlineUser onlineUser;
             onlineUser.fd=fd;
             strcpy(onlineUser.name,user.name);
             addOlineUser(onlineUser);
             */
            
            //第二种方法
            changeOnlineSockfdByName(user.name, fd);
            
            memset(&msgContent, 0, sizeof(msgContent));
            msgContent.msgType=MsgTypeLoginRespondSuccess;
            strcpy(msgContent.attachMsg, "你换了机器登陆...");
            write(fd, &msgContent, sizeof(msgContent));
            
            
            //来到登陆完成的处理界面
            handleLoginSuccess(user.name,fd);
        }
        else if(onlinType==OnlineTypeNo)
        {
            
            //3.
            int ret=checkNameAndPasswd(user);
            if(ret<0)
            {
                msgContent.msgType=MsgTypeLoginRespondFailed;
                strcpy(msgContent.attachMsg, "账号或者密码错误");
                //发送登陆的反馈信息给客户端
                write(fd, &msgContent, sizeof(msgContent));
            }
            else if(ret==0)
            {
                //来到这说明账号和密码正确
                //添加到在线链表
                OnlineUser onlineUser;
                strcpy(onlineUser.name, user.name);
                onlineUser.fd=fd;
                addOlineUser(onlineUser);
                msgContent.msgType=MsgTypeLoginRespondSuccess;
                strcpy(msgContent.attachMsg, "登陆允许");
                //发送登陆的反馈信息给客户端
                write(fd, &msgContent, sizeof(msgContent));
                //来到登陆完成的处理界面
                handleLoginSuccess(user.name,fd);
                
            }
            
        }
        
    }
}



void *handleClient(void *sockfd)
{
    int *p=(int *)sockfd;
    int fd=*p;
    free(p);
    User user;
    int ret;
    while (1)
    {
        ret=(int)read(fd, &user, sizeof(user));
        if(ret==0)
        {
            /*
             0.发送自己离线消息给所有在线的用户
             1.来到这需要将通信套接字关闭
             2.将该用户从在线链表中删除
             */
            //0.
            MsgContent msgContent;
            pOnlineLink p=getOnlineUserNameBySockfd(fd);
            sprintf(msgContent.attachMsg, "%s:我下线了...",p->data.name);
            sendMsgToAllOnlineUser(fd, msgContent.attachMsg);
            
            printf("断开连接\n");
            //1.
            close(fd);
            //2.
            outOnlineUserBySockfd(fd);
            break;
        }
        switch (user.msgType)
        {
            case MsgTypeRegisterRequest:
                handleRegisterRequest(fd,user);
                break;
            case MsgTypeForgetPasswdRequest:
                handleForgetPasswdRequest(fd,user);
                break;
            case MsgTypeLoginRequest:
                handleLoginRequest(fd,user);
                break;
            default:
                break;
        }
        
    }
    return NULL;
}

void tcpServer()
{
    //1.
    int sockfd=socket(AF_INET, SOCK_STREAM, 0);
    CHECKERROR(sockfd, "socket failed")
    
    //设置端口重用
    int on=1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    
    //2.
    struct sockaddr_in serveraddr;
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(5556);
    inet_aton("127.0.0.1", &serveraddr.sin_addr);
    int ret=bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in));
    CHECKERROR(ret, "bind failed")
    //3.
    ret=listen(sockfd, SOMAXCONN);
    CHECKERROR(ret, "listen failed")
    //4.
    while (1)
    {
        //accept没有连接就会等待
        int connfd=accept(sockfd, NULL, NULL);
        if(connfd<0)
        {
            perror("acept failed");
            continue;
        }
        pthread_t pid;
        int *p=(int *)malloc(sizeof(int));
        *p=connfd;
        pthread_create(&pid, NULL, handleClient, (void *)p);
    }
}




int main()
{
    //程序一启动，创建下载链表
    creatDownList();
    //程序一启动，创建在线链表
    creatOnlineUserList();
    
    //读取注册信息保存到链表里面
    readRegisterUserFromFile();
    
    //读取未发送的信息保存到链表里
    readUnsendMsgFromFile();
    
    //开启服务器
    tcpServer();
    
    
    return 0;
}






