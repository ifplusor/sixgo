#ifndef SIXGO_H_HASH
#define SIXGO_H_HASH

#define MAXSIZE 1048576

#include "connect_six.h"
#include <time.h>

typedef struct _BoardCode
{
	unsigned long a[23];
	long hash32;
}BoardCode;

typedef struct _HashInfo
{
	BoardCode code;
	vector<Step> stepList;
	int value,myType,denType;//保存威胁类型的主要目的是为了指导招法生成
	bool cut,full;//cut：剪枝标记；full：着法列表完整性标记，常规搜索生成完整着法列表，扩展搜索生成特殊着法
	int index;//哈希表项储存时间戳，保存行棋数（HandNum）
	int depth;
}HashInfo;

extern HashInfo hashList[MAXSIZE];

void initialCode(BoardCode &code);
void moveCodeP(BoardCode &code,Point point,int side);
void moveCodeS(BoardCode &code,Step step,int side);
void initialHash();
long hashCode(BoardCode code);
bool compareCode(BoardCode &a,BoardCode &b);
HashInfo *findHash(BoardCode &code,int index);
void ReadCM(int color);

#endif