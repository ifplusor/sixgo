#ifndef SIXGO_H_INFO
#define SIXGO_H_INFO

#include "connect_six.h"
#include "base.h"
#include "linetype.h"


//���ɱ�־
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

	vector<Point> winList;//5->6����ʤ��
	vector<Point> willWinList;//4->5:������ʤ�㣬ֻ��������ʤ��ʱʹ��
	//����в�����漰��������Ϊ���ͣ����������Ĺ����г��ָ��ʽ���Ϊ0��Ϊ���ٴ���ʱ�Ľ������

	vector<Point> duoThreatList;//��������˫��в�ĵ�
	vector<Point> solThreatList;//�������ɵ���в�ĵ�
	vector<Point> duoPotenList;//��������˫Ǳ���ĵ�
	vector<Point> solPotenList;//�������ɵ�Ǳ���ĵ�
	vector<Point> toDuoTwoList;//��������Ǳ˫Ǳ���ĵ�
}SynInfo;//�����Ϣ

typedef struct _lineinfo
{
	vector<Step> defStepList;
	vector<Point> defPointList;

	vector<Point> winList;//5->6����ʤ��
	vector<Point> willWinList;//4->5:������ʤ��

	vector<Point> duoThreatList;//��������˫��в�ĵ�
	vector<Point> solThreatList;//�������ɵ���в�ĵ�
	vector<Point> duoPotenList;//��������˫Ǳ���ĵ�
	vector<Point> solPotenList;//�������ɵ�Ǳ���ĵ�
	vector<Point> toDuoTwoList;//��������Ǳ˫Ǳ���ĵ�

	int LineType;			//������ߵ���в���ͣ�λ��ʾ����в����������ʮλ��ʾ˫��в������������λ��ʾ����в����������ǧλ��ʾ��ʤ��������
	int value;
}LineInfo;//����Ϣ


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
