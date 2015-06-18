#include "Seach.h"


/** Increment - λ�ñ任
 *	@return:	�޷���ֵ
 *	@x:			x����
 *	@y:			y����
 *	@lineDirec:	�����־
 */
void Increment(int &x, int &y, BYTE lineDirec)	
{
	switch(lineDirec)
	{
	case ANGLE0:
		Increment0(x, y);	break;
	case ANGLE45:
		Increment45(x, y);	break;
	case ANGLE90:
		Increment90(x, y);	break;
	case ANGLE135:
		Increment135(x, y);	break;
	default:
		printf("Ҫ�������ߵ����ʹ���!\n");
	}
}

/** deIncrement - ����λ�ñ任
 *	@return:	�޷���ֵ
 *	@x:			x����
 *	@y:			y����
 *	@lineDirec:	�����־
 */
void deIncrement(int &x, int &y, BYTE lineDirec)	
{
	switch(lineDirec)
	{
	case ANGLE0:
		Decrement0(x, y);	break;
	case ANGLE45:
		Decrement45(x, y);	break;
	case ANGLE90:
		Decrement90(x, y);	break;
	case ANGLE135:
		Decrement135(x, y);	break;
	default:
		printf("Ҫ�������ߵ����ʹ���!\n");
	}
}

/**	GetLineKey - ��ӳ��,��ȡ�����ڵ��ߵı��
 *	@return:	�ߵı�ţ�-1��ʾ����Ч
 *	@point:		������
 *	@start:		����ʼ������
 *	@lineDirec:	�ߵķ����־
 */
int GetLineKey(Point point,Point *start,BYTE lineDirec)		
{
	int key=-1;
	switch(lineDirec)
	{
	case ANGLE0://������������0~18
		start->y=point.y;
		start->x=0;
		key=start->y;
		return key;
	case ANGLE90://������������
		start->x=point.x;
		start->y=0;
		key=19+start->x;
		return key;
	case ANGLE45:
		if(point.x>=point.y)
		{
			start->x=point.x-point.y;
			if(start->x>13)
				return -1;
			start->y=0;
			key=51+start->x;
		}
		else
		{
			start->y=point.y-point.x;
			if(start->y>13)
				return -1;
			start->x=0;
			key=51-start->y;
		}
		return key;
	case ANGLE135:
		if(point.x>=18-point.y)
		{
			start->x=point.x+point.y-18;
			if(start->x>13)
				return -1;
			start->y=18;
			key=78-start->x;
		}
		else
		{
			start->y=point.y+point.x;
			if(start->y<5)
				return -1;
			start->x=0;
			key=78+18-start->y;
		}
		return key;
	}
	return key;
}


bool cmpStepValue(const Step &s1,const Step &s2)
{		
	return s1.value>s2.value;
}
bool cmpPointValue(const Step &p1,const Step &p2)
{		
	return p1.value>p2.value;
}
bool cmpPointPosition(const Point &p1, const Point &p2)
{
	if(p1.x==p2.x)
		return p1.y<p2.y;
	else
		return p1.x<p2.x;
}
bool EqualPoint(const Point &p1,const Point &p2)
{
	return p1.x==p2.x&&p1.y==p2.y;
}

//�������еĵ����ȥ��
void UniquePoint(vector<Point> &PointList)
{
	vector<Point>::iterator iter;
	sort(PointList.begin(),PointList.end(),cmpPointPosition);
	iter=unique( PointList.begin(), PointList.end(),EqualPoint);
	if(iter!=PointList.end())
	{
		PointList.erase(iter,PointList.end());
		PointList.resize(PointList.size());
	}
}

bool cmpStepPosition(Step &s1,Step &s2)
{
	Point p;
	int t1,t2,t3,t4,t;
	t1=s1.first.y*100+s1.first.x;
	t2=s1.second.y*100+s1.second.x;
	if(t1>t2)
	{
		p=s1.first;
		s1.first=s1.second;
		s1.second=p;
		t=t1;t1=t2;t2=t;
	}
	t3=s2.first.y*100+s2.first.x;
	t4=s2.second.y*100+s2.second.x;
	if(t3>t4)
	{
		p=s2.first;
		s2.first=s2.second;
		s2.second=p;
		t=t3;t3=t4;t4=t;
	}
	return (t1*10000+t2)>(t3*10000+t4);
}
bool EqualStep(const Step &s1,const Step &s2)
{
	return (s1.first.x==s2.first.x && s1.first.y==s2.first.y && s1.second.x==s2.second.x && s1.second.y==s2.second.y);
}

//�������еĲ�����ȥ��
void UniqueStep(vector<Step> &StepList)
{
	vector<Step>::iterator iter;
	sort(StepList.begin(),StepList.end(),cmpStepPosition);
	iter=unique( StepList.begin(), StepList.end(),EqualStep);
	if(iter!=StepList.end())
	{
		StepList.erase(iter,StepList.end());
		StepList.resize(StepList.size());
	}
}

/**	CopyLineInfo - ����Ϣ����
 *	@return:	�޷���ֵ
 *	@dest:		���и��Ƶ�Ŀ������
 *	@src:		���и��Ƶ�Դ����
 */
void CopyLineInfo(LineInfo &dest,LineInfo &src,int tag)
{
	dest.value=src.value;
	dest.LineType=src.LineType;
	if(tag&TODEFENT)
	{
		vector<Point>().swap(dest.defPointList);//�ͷ��ڴ�
		dest.defPointList=src.defPointList;
		vector<Step>().swap(dest.defStepList);
		dest.defStepList=src.defStepList;
	}
	if(tag&TOWIN)
	{
		vector<Point>().swap(dest.winList);
		dest.winList=src.winList;
	}
	if(tag&TOWILLWIN)
	{
		vector<Point>().swap(dest.willWinList);
		dest.willWinList=src.willWinList;
	}
	if(tag&TODUOTHREAT)
	{
		vector<Point>().swap(dest.duoThreatList);
		dest.duoThreatList=src.duoThreatList;
	}
	if(tag&TOSOLTHREAT)
	{
		vector<Point>().swap(dest.solThreatList);
		dest.solThreatList=src.solThreatList;
	}
	if(tag&TODUOPOTEN)
	{
		vector<Point>().swap(dest.duoPotenList);
		dest.duoPotenList=src.duoPotenList;
	}
	if(tag&TOSOLPOTEN)
	{
		vector<Point>().swap(dest.solPotenList);
		dest.solPotenList=src.solPotenList;
	}
	if(tag&TOCOMMON)
	{
		vector<Point>().swap(dest.toDuoTwoList);
		dest.toDuoTwoList=src.toDuoTwoList;
	}
}

/**	initialLine - ��ʼ��LineInfo����
 *	@return:	�޷���ֵ
 *	@lineInfo:	��Ҫ���г�ʼ���ı���ָ��
 */
void initialLine(LineInfo *lineInfo)
{
	lineInfo->winList.clear();//5->6����ʤ��
	lineInfo->willWinList.clear();//4->5:������ʤ��
	lineInfo->duoThreatList.clear();//��������˫��в�ĵ�
	lineInfo->solThreatList.clear();//�������ɵ���в�ĵ�
	lineInfo->duoPotenList.clear();//��������˫Ǳ���ĵ�
	lineInfo->solPotenList.clear();//�������ɵ�Ǳ���ĵ�
	lineInfo->toDuoTwoList.clear();//��������Ǳ˫Ǳ���ĵ�
	lineInfo->defPointList.clear();//��������в���͵ĵ�
	lineInfo->defStepList.clear();//����˫��в���͵Ĳ�
	lineInfo->value=0;
	lineInfo->LineType=0;			//������ߵ�����  -1:ʤ  0:����в;  1:����в;  2:˫��в  3:����в
}
