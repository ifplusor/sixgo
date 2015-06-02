#ifndef SIXGO_H_LINETYPR
#define SIXGO_H_LINETYPE

#include "connect_six.h"

#define NEW
#define _K_		//简明开关


#define TableSize 1048576     //新线型表大小
#define TabSize 1048512		//老线型表大小，为了读取老线型表文件而设

#ifdef NEW
//新线型表是对老线型表的扩展，通过对新线型表类型从标记可得就线型表，故可复用新线型表生成代码生成老线型表

#define WIN 27						//已胜线型
#define THREAT_five_THIRD 26
#define THREAT_five_DOUBLE 25
#define THREAT_five_SINGLE 24
#define THREAT_four_THIRD 23
#define THREAT_four_SUPERPOTEN 22
#define THREAT_four_DOUBLE 21
#define THREAT_four_BIGPOTEN 20
#define THREAT_four_ADDITION 19
#define THREAT_four_POTEN 18
#define THREAT_four_SINGLE 17
#define POTEN_three_SUPERPOTEN 16
#define POTEN_three_DOUBLE 15
#define POTEN_three_BIGPOTEN 14
#define POTEN_three_ADDITION 13
#define POTEN_three_ANDPOTEN 12
#define POTEN_three_ORPOTEN 11
#define POTEN_three_SINGLE 10
#define POTEN_two_TOBIGPOTEN 9
#define POTEN_two_TOADDITION 8
#define POTEN_two_TOANDPOTEN 7
#define POTEN_two_TOPOTEN 6
#define VOID_one_TOBIGPOTEN 5
#define VOID_one_TOADDITION 4
#define VOID_one_TOANDPOTEN 3
#define VOID_one_TOPOTEN 2
#define ZERO 1						//空线型
#define VALUELESS 0

#else

#define THREAT 9
#define WIN 15
#define THREAT_THIRD 14
#define THREAT_five_DOUBLE 13
#define THREAT_five_SINGLE 12
#define THREAT_four_DOUBLE 11
#define THREAT_four_POTEN 10
#define THREAT_four_SINGLE 9
#define POTEN_three_DOUBLE 8
#define POTEN_three_twoDOUBLE 7
#define POTEN_three_SINGLE 6
#define POTEN_two_DOUBLE 5
#define POTEN_two_SINGLE 4
#define POTEN_two_SINGLE2 3
#define one_DOUBLE 2
#define one_SINGLE 1
#define ZERO one_SINGLE
#define VALUELESS -1

#define THREAT_five_THIRD THREAT_THIRD
#define THREAT_four_THIRD THREAT_THIRD
#define THREAT_four_SUPERPOTEN THREAT_four_DOUBLE
#define THREAT_four_BIGPOTEN THREAT_four_POTEN
#define THREAT_four_ADDITION THREAT_four_POTEN
#define POTEN_three_SUPERPOTEN POTEN_three_DOUBLE
#define POTEN_three_BIGPOTEN POTEN_three_twoDOUBLE
#define POTEN_three_ADDITION POTEN_three_twoDOUBLE
#define POTEN_three_ANDPOTEN POTEN_three_twoDOUBLE
#define POTEN_three_ORPOTEN POTEN_three_twoDOUBLE
#define POTEN_two_TOBIGPOTEN POTEN_two_DOUBLE
#define POTEN_two_TOADDITION POTEN_two_SINGLE
#define POTEN_two_TOANDPOTEN POTEN_two_SINGLE
#define POTEN_two_TOPOTEN POTEN_two_SINGLE2
#define VOID_one_TOBIGPOTEN one_DOUBLE
#define VOID_one_TOADDITION one_SINGLE
#define VOID_one_TOANDPOTEN one_SINGLE
#define VOID_one_TOPOTEN one_SINGLE

#endif



typedef unsigned char BYTE;
typedef BYTE* LPBYTE;

typedef int iPoint;
typedef struct _iStep
{
	iPoint first,second;
}iStep;

typedef struct _linetypeinfo
{
	int lineType;			//线的类型  0：无威胁；1：单潜力；2：双潜力；3：多潜力；4:单威胁；5：双威胁；6：多威胁；7：已胜；

	int win;//5->6：致胜点
	int willWin;//4->5:即将致胜点（只在单威胁生成致胜步时使用）
	int triThreat;//4.2->5.1/4.1 4.4->4.1:多威胁点（只在不受威胁的情况下使用）
	//以上三类点用于特殊情况，一个线型只取一个即可

	vector<iStep> defStepList;//防御步，用于破解双威胁
	vector<iPoint> defPointList;//防御点，用于破解单威胁

	vector<iPoint> duoThreatList;//双威胁点，可以生成双威胁的点
	vector<iPoint> solThreatList;//单威胁点，可以生成单威胁的点
	vector<iPoint> duoPotenList;//双潜力点，可以生成双潜力的点
	vector<iPoint> solPotenList;//单潜力点，可以生成单潜力的点
	vector<iPoint> toDuoTwoList;//潜双潜力点

	//以下成员不重要
	vector<iPoint> toSolTwoList;//潜单潜力点
	int toOne;//一阶点
}LineTypeInfo;

typedef struct _linesum
{
	unsigned int duoThreat;//可以生成双威胁的点
	unsigned int solThreat;//可以生成单威胁的点
	unsigned int duoPoten;//可以生成双潜力的点
	unsigned int solPoten;//可以生成单潜力的点
	unsigned int toDuoTwo;
	unsigned int toSolTwo;
	unsigned int defPoint;
	unsigned int defStep;
}LineSum;

extern BYTE preTable[TableSize];					//线型表项
extern LineTypeInfo linetypeInfo[TableSize];				//线型对应的关键信息

int initialLineTypeTable(LPBYTE preTable);
void initialLineInfoTable(LineTypeInfo *linetypeInfo);


#endif