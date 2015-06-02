#ifndef SIXGO_H_SEACH
#define SIXGO_H_SEACH

#include "linetype.h"
#include "hash.h"
#include "Debug.h"

#define CM

#define DEPTH 5
#define MAXDEPTH 15

//生成标志
#define TODEFENT 1
#define TOWIN 2
#define TOWILLWIN 4
#define TODUOTHREAT 8
#define TOSOLTHREAT 16
#define TODUOPOTEN 32
#define TOSOLPOTEN 64
#define TOCOMMON 128
#define TOALL 255 //255=1+2+4+8+16+32+64+128
#define FORNONTHREAT 248 //252=8+16+32+64+128
#define FORPOTEN 120 //120=8+16+32+64

//棋盘上某点4条线的方向特征。
#define ANGLE0	 0			// 0度角
#define ANGLE45	 1			// 45度角
#define ANGLE90	 2			// 90度角
#define ANGLE135 3			// 135度角

//按照线行走时，线上的点的坐标变化规律公式。
#define Increment0(x, y)		{(x)++;}
#define Increment45(x, y)		{(x)++;(y)++;}
#define Increment90(x, y)		{(y)++;}
#define Increment135(x, y)		{(x)++;(y)--;}

#define Decrement0(x, y)		{(x)--;}
#define Decrement45(x, y)		{(x)--;(y)--;}
#define Decrement90(x, y)		{(y)--;}
#define Decrement135(x, y)		{(x)--;(y)++;}

typedef struct _syninfo	//有用点信息
{
 	vector<Point> defPointList;

	vector<Point> winList;//5->6：致胜点
	vector<Point> willWinList;//4->5:即将致胜点，只在生车致胜步时使用
	vector<Point> triThreatList;//多威胁点
	//上面三类只保存一个即可

	vector<Point> duoThreatList;//可以生成双威胁的点
	vector<Point> solThreatList;//可以生成单威胁的点
	vector<Point> duoPotenList;//可以生成双潜力的点
	vector<Point> solPotenList;//可以生成单潜力的点
	vector<Point> toDuoTwoList;//可以生成潜双潜力的点
}SynInfo;

typedef struct _lineinfo//线信息
{
	vector<Step> defStepList;
	vector<Point> defPointList;

	vector<Point> winList;//5->6：致胜点
	vector<Point> willWinList;//4->5:即将致胜点
	vector<Point> triThreatList;//多威胁点
	//上面三类只保存一个即可

	vector<Point> duoThreatList;//可以生成双威胁的点
	vector<Point> solThreatList;//可以生成单威胁的点
	vector<Point> duoPotenList;//可以生成双潜力的点
	vector<Point> solPotenList;//可以生成单潜力的点
	vector<Point> toDuoTwoList;//可以生成潜双潜力的点

//	BYTE side;
	int LineType;			//局面的线的威胁类型  各位表示单威胁线型数量，十位表示双威胁线型数量，百位表示多威胁线型数量，千位表示已胜线型数量
	int value;
}LineInfo;


void initialAllLine();
Step sixgo_carry(const Step moveStep,const BYTE side);
Step SeachValuableStep(const BYTE side);
int nega_alpha_beta(const BYTE side,int alpha,int beta,const int depth);//负极大值搜索
int ExtendSeach(BYTE side,int alpha,int beta,const int depth);
vector<Step> MakeStepListForWin(int side,unsigned int limit);
vector<Step> MakeStepListForDefendSingle(int side,unsigned int limit);
vector<Step> MakeStepListForDefendDouble(int side,unsigned int limit);
vector<Step> MakeStepListForNone(int side,unsigned int limit);
vector<Step> MakeStepListForDefendSingleEx(int side,unsigned int limit);
vector<Step> MakeStepListForDefendDoubleEx(int side,unsigned int limit);
vector<Step> MakeStepListForDouble(int side,unsigned int limit);


void UpdataBoard(const Step step);
void UpdateLineForCross(const Point point,const BYTE side,int tag=TOALL);
LineInfo AnalyzeLine(const Point start,const BYTE lineDirec,const BYTE side,int tag=TOALL);
LineInfo ValuateType(const int style,const Point start,const BYTE lineDirec,int tag=TOALL);		//类型分析
int GetBoardType(const BYTE side);
int GetBoardValue(const BYTE side);
SynInfo GetBoardInfo(const BYTE side,int tag);
void BackupLine(const Point point,LineInfo tempLine[4],const BYTE side);
void RecoverLine(const Point point,LineInfo tempLine[4],const BYTE side);
void MakeMove(const Point point,LineInfo tempLine[2][4],const BYTE side,int tag=TOALL);
void BackMove(const Point point,LineInfo tempLine[2][4],const BYTE side);
int CalculateStepValue(const Step step,const BYTE side);
int CalSingleLineValue(const Point start,const BYTE Direc,const BYTE side);

void Increment(int &x, int &y, BYTE lineDirec);
void deIncrement(int &x, int &y, BYTE lineDirec);
int GetLineKey(const Point point,Point *start,const BYTE lineDirec);		//线映射,确定点所在的线的编号
void UniquePoint(vector<Point> &PointList);
void UniqueStep(vector<Step> &StepList);
void CopyLineInfo(LineInfo &dest,LineInfo &src,int tag=TOALL);
void initialLine(LineInfo *lineInfo);
bool cmpStepValue(const Step &s1,const Step &s2);


#endif