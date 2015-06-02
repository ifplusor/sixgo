#ifndef SIXGO_H_SEACH
#define SIXGO_H_SEACH

#include "linetype.h"
#include "hash.h"
#include "Debug.h"

#define CM

#define DEPTH 5
#define MAXDEPTH 15

//���ɱ�־
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

//������ĳ��4���ߵķ���������
#define ANGLE0	 0			// 0�Ƚ�
#define ANGLE45	 1			// 45�Ƚ�
#define ANGLE90	 2			// 90�Ƚ�
#define ANGLE135 3			// 135�Ƚ�

//����������ʱ�����ϵĵ������仯���ɹ�ʽ��
#define Increment0(x, y)		{(x)++;}
#define Increment45(x, y)		{(x)++;(y)++;}
#define Increment90(x, y)		{(y)++;}
#define Increment135(x, y)		{(x)++;(y)--;}

#define Decrement0(x, y)		{(x)--;}
#define Decrement45(x, y)		{(x)--;(y)--;}
#define Decrement90(x, y)		{(y)--;}
#define Decrement135(x, y)		{(x)--;(y)++;}

typedef struct _syninfo	//���õ���Ϣ
{
 	vector<Point> defPointList;

	vector<Point> winList;//5->6����ʤ��
	vector<Point> willWinList;//4->5:������ʤ�㣬ֻ��������ʤ��ʱʹ��
	vector<Point> triThreatList;//����в��
	//��������ֻ����һ������

	vector<Point> duoThreatList;//��������˫��в�ĵ�
	vector<Point> solThreatList;//�������ɵ���в�ĵ�
	vector<Point> duoPotenList;//��������˫Ǳ���ĵ�
	vector<Point> solPotenList;//�������ɵ�Ǳ���ĵ�
	vector<Point> toDuoTwoList;//��������Ǳ˫Ǳ���ĵ�
}SynInfo;

typedef struct _lineinfo//����Ϣ
{
	vector<Step> defStepList;
	vector<Point> defPointList;

	vector<Point> winList;//5->6����ʤ��
	vector<Point> willWinList;//4->5:������ʤ��
	vector<Point> triThreatList;//����в��
	//��������ֻ����һ������

	vector<Point> duoThreatList;//��������˫��в�ĵ�
	vector<Point> solThreatList;//�������ɵ���в�ĵ�
	vector<Point> duoPotenList;//��������˫Ǳ���ĵ�
	vector<Point> solPotenList;//�������ɵ�Ǳ���ĵ�
	vector<Point> toDuoTwoList;//��������Ǳ˫Ǳ���ĵ�

//	BYTE side;
	int LineType;			//������ߵ���в����  ��λ��ʾ����в����������ʮλ��ʾ˫��в������������λ��ʾ����в����������ǧλ��ʾ��ʤ��������
	int value;
}LineInfo;


void initialAllLine();
Step sixgo_carry(const Step moveStep,const BYTE side);
Step SeachValuableStep(const BYTE side);
int nega_alpha_beta(const BYTE side,int alpha,int beta,const int depth);//������ֵ����
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
LineInfo ValuateType(const int style,const Point start,const BYTE lineDirec,int tag=TOALL);		//���ͷ���
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
int GetLineKey(const Point point,Point *start,const BYTE lineDirec);		//��ӳ��,ȷ�������ڵ��ߵı��
void UniquePoint(vector<Point> &PointList);
void UniqueStep(vector<Step> &StepList);
void CopyLineInfo(LineInfo &dest,LineInfo &src,int tag=TOALL);
void initialLine(LineInfo *lineInfo);
bool cmpStepValue(const Step &s1,const Step &s2);


#endif