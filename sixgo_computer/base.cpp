#include "Seach.h"


/** Increment - 位置变换
 *	@return:	无返回值
 *	@x:			x坐标
 *	@y:			y坐标
 *	@lineDirec:	方向标志
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
		printf("要分析的线的类型错误!\n");
	}
}

/** deIncrement - 反向位置变换
 *	@return:	无返回值
 *	@x:			x坐标
 *	@y:			y坐标
 *	@lineDirec:	方向标志
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
		printf("要分析的线的类型错误!\n");
	}
}

/**	GetLineKey - 线映射,获取点所在的线的编号
 *	@return:	线的编号，-1表示线无效
 *	@point:		点坐标
 *	@start:		线起始点坐标
 *	@lineDirec:	线的方向标志
 */
int GetLineKey(Point point,Point *start,BYTE lineDirec)		
{
	int key=-1;
	switch(lineDirec)
	{
	case ANGLE0://横向由上至下0~18
		start->y=point.y;
		start->x=0;
		key=start->y;
		return key;
	case ANGLE90://纵向由左至右
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

//对容器中的点进行去重
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

//对容器中的步进行去重
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

/**	CopyLineInfo - 线信息复制
 *	@return:	无返回值
 *	@dest:		进行复制的目标引用
 *	@src:		进行复制的源引用
 */
void CopyLineInfo(LineInfo &dest,LineInfo &src,int tag)
{
	dest.value=src.value;
	dest.LineType=src.LineType;
	if(tag&TODEFENT)
	{
		vector<Point>().swap(dest.defPointList);//释放内存
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

/**	initialLine - 初始化LineInfo变量
 *	@return:	无返回值
 *	@lineInfo:	需要进行初始化的变量指针
 */
void initialLine(LineInfo *lineInfo)
{
	lineInfo->winList.clear();//5->6：致胜点
	lineInfo->willWinList.clear();//4->5:即将致胜点
	lineInfo->duoThreatList.clear();//可以生成双威胁的点
	lineInfo->solThreatList.clear();//可以生成单威胁的点
	lineInfo->duoPotenList.clear();//可以生成双潜力的点
	lineInfo->solPotenList.clear();//可以生成单潜力的点
	lineInfo->toDuoTwoList.clear();//可以生成潜双潜力的点
	lineInfo->defPointList.clear();//防御单威胁线型的点
	lineInfo->defStepList.clear();//防御双威胁线型的步
	lineInfo->value=0;
	lineInfo->LineType=0;			//局面的线的类型  -1:胜  0:无威胁;  1:单威胁;  2:双威胁  3:多威胁
}
