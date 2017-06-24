//
//  tool.h
//  chatServer
//
//  Created by zyl on 2017/6/22.
//  Copyright © 2017年 zyl. All rights reserved.
//


#ifndef tool_h
#define tool_h

#include <stdio.h>
#include "all.h"

char *getCurrentTime(char *str);
void splitStringByCh(char *str,char ch,char *pre,char *next);
int handleString(char *str,char (*person)[MINSIZE],char *msg);

int checkFileIsExist(char *fileName);

#endif /* tool_h */
