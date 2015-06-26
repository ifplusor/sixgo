#ifndef SIXGO_H_INFO
#define SIXGO_H_INFO

#include "connect_six.h"
#include "base.h"
#include "linetype.h"


//生成标志
#define TODEFENT		1
#define TOWIN			2
#define TOWILLWIN		4
#define TODUOTHREAT		8
#define TOSOLTHREAT		16
#define TODUOPOTEN		32
#define TOSOLPOTEN		64
#define TOCOMMON		128
#define TOALL			255 //255=1+2+4+8+16+32+64+128
#define FORNONTHREAT	248 //252=8+16+32+64+128
#define FORPOTEN		120 //120=8+16+32+64


typedef struct _syninfo
{
	vector<Step> defStepList;
	vector<Point> defPointList;

	vector<Point> winList;//5->6：致胜点
	vector<Point> willWinList;//4->5:即将致胜点，只在生成致胜步时使用
	//多威胁点所涉及到的线型为废型，在正常对弈过程中出现概率近似为0，为减少处理时耗将其废弃

	vector<Point> duoThreatList;//可以生成双威胁的点
	vector<Point> solThreatList;//可以生成单威胁的点
	vector<Point> duoPotenList;//可以生成双潜力的点
	vector<Point> solPotenList;//可以生成单潜力的点
	vector<Point> toDuoTwoList;//可以生成潜双潜力的点
}SynInfo;//棋局信息

typedef struct _lineinfo
{
	vector<Step> defStepList;
	vector<Point> defPointList;

	vector<Point> winList;//5->6：致胜点
	vector<Point> willWinList;//4->5:即将致胜点

	vector<Point> duoThreatList;//可以生成双威胁的点
	vector<Point> solThreatList;//可以生成单威胁的点
	vector<Point> duoPotenList;//可以生成双潜力的点
	vector<Point> solPotenList;//可以生成单潜力的点
	vector<Point> toDuoTwoList;//可以生成潜双潜力的点

	int LineType;			//局面的线的威胁类型：位表示单威胁线型数量，十位表示双威胁线型数量，百位表示多威胁线型数量，千位表示已胜线型数量
	int value;
}LineInfo;//线信息


void initialAllLine();
void UpdateLineForCross(const Point point, const BYTE side, int tag = TOALL);
int GetBoardType(const BYTE side);
int GetBoardValue(const BYTE side);
SynInfo GetBoardInfo(const BYTE side, int tag);
void MakeMove(const Point point, LineInfo tempLine[2][4], const BYTE side, int tag = TOALL);
void BackMove(const Point point, LineInfo tempLine[2][4], const BYTE side);
int CalculateStepValue(const Step step, const BYTE side);


extern int virtualBoard[edge][edge];


#endif
