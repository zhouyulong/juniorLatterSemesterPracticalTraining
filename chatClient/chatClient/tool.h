//
//  tool.h
//  19-聊天客户端
//
//  Created by ccj on 2017/6/21.
//  Copyright © 2017年 ccj. All rights reserved.
//

#ifndef tool_h
#define tool_h

#include <stdio.h>
#include "all.h"
int getFileNames(char *str,char (*fileName)[MEDIUMSIZE]);

int checkMsgIsCorrect(char *msg);

void getLastFileName(char *str,char *lastFileName);
char *getCurrentTime(char *str);
#endif /* tool_h */
