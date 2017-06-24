//
//  main.c
//  19-聊天客户端
//
//  Created by ccj on 2017/6/14.
//  Copyright © 2017年 ccj. All rights reserved.
//

#include "all.h"
#include "list.h"
#include "tool.h"

#define CLIENTFILE "/Users/yl/Desktop/c/cltFl/"
/*连接服务器*/
int connectServer()
{
    //1.
    int sockfd=socket(AF_INET, SOCK_STREAM, 0);
    CHECKERROR(sockfd, "socket failed")
    
    //2.
    struct sockaddr_in serveraddr;
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(5556);
    inet_aton("127.0.0.1", &serveraddr.sin_addr);
    int ret=connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in));
    CHECKERROR(ret, "connect failed")
    return sockfd;
}

void registerRequest(int fd)
{
    User user;
    user.msgType=MsgTypeRegisterRequest;
    printf("注册姓名:");
    scanf("%s",user.name);
    printf("注册密码:");
    scanf("%s",user.passwd);
    printf("设置密保问题:");
    char question[MEDIUMSIZE],answer[MEDIUMSIZE];
    scanf("%s",question);
    printf("设置密保答案:");
    scanf("%s",answer);
    //内存拷贝函数memcpy
    memcpy(user.attachMsg, question, sizeof(question));
    memcpy(user.attachMsg+MEDIUMSIZE, answer, sizeof(answer));
    
    write(fd, &user, sizeof(user));
    
    //拿到服务器的反馈信息
    MsgContent msgContent;
    read(fd, &msgContent, sizeof(msgContent));
    printf("%s\n",msgContent.attachMsg);
    
}

void forgetPasswdRequest(int fd)
{
    User user;
    user.msgType=MsgTypeForgetPasswdRequest;
    printf("请输入账号:");
    scanf("%s",user.name);
    //将请求发送给服务器
    write(fd, &user, sizeof(user));
    
    MsgContent msgContent;
    read(fd, &msgContent, sizeof(msgContent));
    if(msgContent.msgType==MsgTypeForgetPasswdRespondFailed)
    {
        printf("%s\n",msgContent.attachMsg);
    }
    else
    {
        printf("密保问题:%s\n",msgContent.attachMsg);
        printf("请输入密保问题的答案:");
        msgContent.msgType=MsgTypeForgetPasswdRequest;
        scanf("%s",msgContent.attachMsg);
        //将密保答案发送给服务器验证
        write(fd, &msgContent, sizeof(msgContent));
        read(fd, &msgContent, sizeof(msgContent));
        if (msgContent.msgType==MsgTypeForgetPasswdRespondFailed)
        {
            printf("%s\n",msgContent.attachMsg);
        }
        else
        {
            printf("你的原密码是%s,请妥善保管\n",msgContent.attachMsg);
        }
    }
}


void handleRespondForMsgContent(int fd)
{
    char attachMsg[MAXSIZE];
    read(fd, attachMsg, sizeof(attachMsg));
    printf("%s\n",attachMsg);
    
}

/*处理服务器的获取文件列表的反馈*/
void handleGetFileListRespond(int fd)
{
    char attachMsg[MAXSIZE];
    read(fd, attachMsg, sizeof(attachMsg));
    char fileName[MEDIUMSIZE][MEDIUMSIZE];
    int count=getFileNames(attachMsg, fileName);
    printf("-------------------------------\n");
    printf("|文件列表如下:\n");
    int i;
    for (i=0; i<count; i++)
    {
        printf("|%s\n",fileName[i]);
    }
    printf("-------------------------------\n");
    
}



/*处理服务器下载文件的反馈信息*/
void handleDownloadFileRespond(int fd)
{
    File file;
    read(fd, &file, sizeof(file));
    
    DownFile downFile;
    strcpy(downFile.fileName,file.fileName);
    strcpy(downFile.time, file.time);
    int ret=isDownloadingFisrtPacket(downFile);
    char path[MEDIUMSIZE];
    sprintf(path, "%s%s",CLIENTFILE,file.fileName);
    if(ret<0)
    {  //第一次接受到某个文件的小包，就去创建文件，写内容到文件
        printf("下载中");
        int fd1=open(path, O_CREAT|O_WRONLY|O_TRUNC,0644);
        write(fd1, file.text, file.textSize);
        close(fd1);
        addDownFile(downFile);
    }
    else
    {//定位到偏移量的位置，写内容到文件
        printf(".");
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




/*专门处理登陆之后服务器返回过来的信息*/
void *handleSeverMsg(void *sockfd)
{
    int *p=(int *)sockfd;
    int fd=*p;
    
    free(p);
    MsgType msgType;
    int ret;
    while (1)
    {
        ret=(int)read(fd,&msgType, sizeof(msgType));
        if(ret==0)
        {
            printf("服务器正在抢修...\n");
            exit(0);
        }
        switch (msgType)
        {
            case MsgTypeLoginRespondFailed:
                handleRespondForMsgContent(fd);
                exit(0);
                break;
            case MsgTypeChangePasswdRespondFailed:
            case MsgTypeChangePasswdRespondSuccess:
            case MsgTypechatRespondSuccess:
            case MsgTypeChatRespondFailed:
            case MsgTypeGetFileListRespondFailed:
            case MsgTypeDownloadFileRespondFailed:
            case MsgTypeRenameFileRespondFailed:
            case MsgTypeRenameFileRespondSuccess:
            case MsgTypeDeleteFileRespondFailed:
            case MsgTypeDeleteFileRespondSuccess:
                handleRespondForMsgContent(fd);
                break;
            case MsgTypeGetFileListRespondSuccess:
                handleGetFileListRespond(fd);
                break;
            case MsgTypeDownloadFileRespondSuccess:
                handleDownloadFileRespond(fd);
                break;
            default:
                break;
        }
    }
    return NULL;
}

void changePasswdRequest(int fd)
{
    MsgContent msgContent;
    msgContent.msgType=MsgTypeChangePasswdRequest;
    int errorCount=2;
    char oldPasswd[MEDIUMSIZE],newPasswd[MEDIUMSIZE],confirmPasswd[MEDIUMSIZE];
    while (errorCount)
    {
        printf("请输入旧密码:");
        scanf("%s",oldPasswd);
        printf("请输入新密码:");
        scanf("%s",newPasswd);
        printf("确认新密码:");
        scanf("%s",confirmPasswd);
        if(strcmp(newPasswd, confirmPasswd)!=0)
        {
            errorCount--;
            continue;
        }
        else
        {
            break;
        }
    }
    //发送给服务器
    memcpy(msgContent.attachMsg,oldPasswd, sizeof(oldPasswd));
    memcpy(msgContent.attachMsg+MEDIUMSIZE, newPasswd, sizeof(newPasswd));
    write(fd, &msgContent, sizeof(msgContent));
    
}



/*聊天*/
void chatRequest(int fd)
{
    MsgContent msgContent;
    msgContent.msgType=MsgTypeChatRequest;
    printf("-------------------------------\n");
    printf("|         聊天规则如下           |\n");
    printf("|单聊--->收信人:信息内容          |\n");
    printf("|多聊--->收信人1:收信人2:信息内容  |\n");
    printf("|群聊--->all:信息内容            |\n");
    printf("|quit-->退出聊天                |\n");
    printf("-------------------------------\n");
    while (1)
    {
        scanf("%s",msgContent.attachMsg);
        if(strcmp(msgContent.attachMsg, "quit")==0)
        {
            break;
        }
        //本地验证消息的正确性
        /*
         错误信息:
         1>没有冒号  baskjbkd
         2>有冒号但是消息内容是空的  x1:\0   x1:x2:x3:\0
         */
        int ret=checkMsgIsCorrect(msgContent.attachMsg);
        if (ret==0)
        {
            //发送聊天消息给服务器
            write(fd, &msgContent, sizeof(msgContent));
            
        }
        else if(ret<0)
        {
            printf("消息发送规则错误\n");
        }
        
    }
}

/*发送获取文件列表请求*/
void getFileListRequest(int fd)
{
    MsgType msgType=MsgTypeGetFileListRequest;
    write(fd, &msgType, sizeof(msgType));
}


/*下载文件请求*/
void downloadFileRequest(int fd)
{
    /*
     先判断该文件是否正在下载，就不需要下载
     */
    MsgContent msgContent;
    msgContent.msgType=MsgTypeDownloadFileRequest;
    printf("请输入要下载的文件名:");
    scanf("%s",msgContent.attachMsg);
    int ret=isDownloadFile(msgContent.attachMsg);
    if(ret<0)
    {
        write(fd, &msgContent, sizeof(msgContent));
    }
    else
    {
        printf("正在下载中...\n");
    }
    
}


/*异步线程执行上传任务*/
void *asyncUploadFile(void *sockfd)
{
    ThreadArgs *p=(ThreadArgs *)sockfd;
    int fd=p->fd;
    char path[MEDIUMSIZE],subPath[MEDIUMSIZE];
    getLastFileName(p->fileName, subPath);
    strcpy(path, p->fileName);
    //释放堆空间
    free(p);
    
    File file;
    strcpy(file.fileName, subPath);
    getCurrentTime(file.time);
    //确定文件大小
    int fd1=open(path, O_RDONLY);
    long long left=lseek(fd1, 0, SEEK_END);
    lseek(fd1, 0, SEEK_SET);
    int ret;
    long long offset=0;
    while (left)
    {
        if (left>MAXIOSIZE)
        {
            ret=(int)read(fd1, file.text, MAXIOSIZE);
        }
        else
        {
            ret=(int)read(fd1, file.text, left);
        }
        left-=ret;
        file.left=left;
        file.offset=offset;
        offset+=ret;
        file.textSize=ret;
        
        MsgType msgType=MsgTypeUploadFileRequest;
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
    }
    close(fd1);
    return NULL;
}

/*上传文件请求*/
void uploadFileRequest(int fd)
{
    printf("请输入要上传的文件路径:");//路径指的是目录
    char dirpath[MEDIUMSIZE];
    scanf("%s",dirpath);
    /*
     /Users/ccj/Desktop
     /Users/ccj/Desktop/
     */
    //根据dirpath显示dirpath里面的所有普通文件
    DIR *dir=opendir(dirpath);
    if(dir==NULL)
    {
        printf("目录文件不存在\n");
        return;
    }
    struct dirent *d;
    printf("---------------------------\n");
    printf("可供上传的文件如下:\n");
    while ((d=readdir(dir)))
    {
        if(d->d_type==DT_REG)
        {
            printf("|%s\n",d->d_name);
        }
    }
    printf("---------------------------\n");
    closedir(dir);
    printf("请输入要上传的文件:");
    char fileName[MEDIUMSIZE],subPath[MEDIUMSIZE];
    scanf("%s",fileName);
    if(dirpath[strlen(dirpath)-1]=='/')
    {
        sprintf(subPath, "%s%s",dirpath,fileName);
    }
    else
    {
        sprintf(subPath, "%s/%s",dirpath,fileName);
    }
    int ret=access(subPath, F_OK);
    if(ret<0)
    {
        printf("普通文件不存在...\n");
        return;
    }
    //来到这文件是存在的，就分包发送给服务器
    //由于读文件比较耗时，所以开线程专门向服务器发送包的信息
    /*
     线程传参，如果是局部变量，有可能会出错，因为局部变量的空间有可能在开线程的那一刻已经消亡了
     */
    ThreadArgs *p=(ThreadArgs *)malloc(sizeof(ThreadArgs));
    p->fd=fd;
    strcpy(p->fileName,subPath);
    pthread_t pid;
    pthread_create(&pid, NULL, asyncUploadFile, (void *)p);
    
}


/*重命名文件*/
void renameFileRequest(int fd)
{
    /*
     需要重名服务器的哪个文件，重名成什么名称
     */
    MsgContent msgContent;
    char oldName[MEDIUMSIZE],newName[MEDIUMSIZE];
    printf("请输入要修改的文件名称:");
    scanf("%s",oldName);
    printf("修改之后的名称:");
    scanf("%s",newName);
    msgContent.msgType=MsgTypeRenameFileRequest;
    memcpy(msgContent.attachMsg,oldName,sizeof(oldName));
    memcpy(msgContent.attachMsg+MEDIUMSIZE, newName, sizeof(newName));
    write(fd, &msgContent, sizeof(msgContent));
}

/*发送删除文件请求*/
void deleteFileRequest(int fd)
{
    MsgContent msgContent;
    msgContent.msgType=MsgTypeDeleteFileRequest;
    printf("请输入要删除的文件名称:");
    scanf("%s",msgContent.attachMsg);
    write(fd, &msgContent, sizeof(msgContent));
}

/*成功登陆*/
void loginSuccess(int fd)
{
    //发送我在线的状态给所有的用户
    MsgContent msgContent;
    msgContent.msgType=MsgTypeChatRequest;
    strcpy(msgContent.attachMsg, "all:我上线了...");
    write(fd, &msgContent, sizeof(msgContent));
    
    //开线程，专门处理服务器返回的信息，比如聊天的相关信息
    int *p=(int *)malloc(sizeof(int));
    *p=fd;
    pthread_t pid;
    pthread_create(&pid, NULL,handleSeverMsg,(void *)p);
    
    int select;
    while(1)
    {
        //system("clear");清屏操作
        printf("===============\n");
        printf("====1.聊天======\n");
        printf("===2.修改密码====\n");
        printf("===3.获取文件列表=\n");
        printf("===4.下载文件====\n");
        printf("===5.上传文件====\n");
        printf("===6.重命名文件===\n");
        printf("===7.删除文件====\n");
        printf("===============\n");
        scanf("%d",&select);
        switch (select)
        {
            case 1:
                chatRequest(fd);
                break;
            case 2:
                changePasswdRequest(fd);
                break;
            case 3:
                getFileListRequest(fd);
                break;
            case 4:
                downloadFileRequest(fd);
                break;
            case 5:
                uploadFileRequest(fd);
                break;
            case 6:
                renameFileRequest(fd);
                break;
            case 7:
                deleteFileRequest(fd);
                break;
            default:
                break;
        }
    }
}

/*登陆请求*/
void loginRequest(int fd)
{
    User user;
    user.msgType=MsgTypeLoginRequest;
    printf("账号:");
    scanf("%s",user.name);
    printf("密码:");
    scanf("%s",user.passwd);
    write(fd, &user, sizeof(user));
    
    //读取服务器反馈的信息
    MsgContent msgContent;
    read(fd, &msgContent, sizeof(msgContent));
    if(msgContent.msgType==MsgTypeLoginRespondFailed)
    {
        printf("%s\n",msgContent.attachMsg);
    }
    else if(msgContent.msgType==MsgTypeLoginRespondSuccess)
    {
        
        //        printf("%s\n",msgContent.attachMsg);
        //来到这进入登陆界面
        loginSuccess(fd);
    }
    
}


int main()
{
    //创建下载文件链表
    creatDownList();
    int fd=connectServer();
    int select;
    while (1)
    {
        printf("=============\n");
        printf("====1.注册====\n");
        printf("====2.登陆====\n");
        printf("==3.忘记密码===\n");
        printf("=============\n");
        scanf("%d",&select);
        switch (select)
        {
            case 1:
                registerRequest(fd);
                break;
            case 2:
                loginRequest(fd);
                break;
            case 3:
                forgetPasswdRequest(fd);
                break;
            default:
                break;
        }
    }
    return 0;
}









