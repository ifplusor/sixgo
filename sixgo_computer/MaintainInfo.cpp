#include "MaintainInfo.h"


//const int LineTypeValue[WIN+1]={0, 0, 0,0,0,1, 3,4,5,9, 8,12,15,16,20,31,33, 39,45,47,54,89,100,1000, 37,76,1000, 10000 };//线型价值表

//加*表示废型                         0| 1| 2  3  4  5| 6  7  8   9| 10 *11  12  13 *14  15 *16| 17  18  19  *20   21  *22    *23| 24   25    *26|     27
const int LineTypeValue[WIN + 1] = { 0, 0, 1, 1, 1, 3, 5, 6, 8, 16, 15, 18, 20, 24, 28, 31, 39, 75, 90, 110, 115, 186, 236, 99999, 94, 196, 99999, 100000 };//线型价值表
//无价值，|0潜潜双潜力，1潜单潜力，2潜弱双潜力，3潜双潜力，4潜多潜力，5单潜力，
//         6弱双潜力，7双潜力，8多潜力，9单威胁，10弱双威胁，11双威胁|，多威胁，已胜
//             潜多潜力16->多潜力100->多威胁1000->已胜
//潜潜双潜力3->潜双潜力11->双潜力31->双威胁155->已胜10000
//            潜弱双潜力7->弱双潜力19->弱双威胁95
//             潜单潜力5->单潜力15->单威胁75->已胜

const int LineTypeThreat[WIN + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 10, 10, 100, 1, 10, 100, 1000 };//线型威胁类型表：1单威胁、10双威胁、100三威胁、1000已胜
const int LineTypeType[WIN + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 10, 10, 11, 11, 10, 100, 100, 1000, 1100, 1100, 1000, 2000, 2000, 3000, 1000, 2000, 3000, 10000 };//线型类型表

LineInfo lineInfo[2][92];//线的信息表，应用局部更新技术维护表中信息
int virtualBoard[edge][edge];//虚拟棋盘


/**
 * initialLine - 初始化LineInfo变量
 * @lineInfo:	需要进行初始化的变量指针
 * @return:		无返回值
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
	lineInfo->value = 0;
	lineInfo->LineType = 0;			//局面的线的类型  -1:胜  0:无威胁;  1:单威胁;  2:双威胁  3:多威胁
}

/**
 * initialAllLine - 初始化保存对弈双方92条线信息的lineInfo[2][92]数组
 * @return:		无返回值
 */
void initialAllLine()
{
	int i, j;
	for (i = 0; i < 2; i++)
		for (j = 0; j < 92; j++)
			initialLine(&lineInfo[i][j]);
}

/**
 * CopyLineInfo - 线信息复制
 * @dest:	进行复制的目标引用
 * @src:	进行复制的源引用
 * @return:	无返回值
 */
void CopyLineInfo(LineInfo &dest, LineInfo &src, int tag = TOALL)
{
	dest.value = src.value;
	dest.LineType = src.LineType;
	if (tag&TODEFENT)
	{
		vector<Point>().swap(dest.defPointList);//释放内存
		dest.defPointList = src.defPointList;
		vector<Step>().swap(dest.defStepList);
		dest.defStepList = src.defStepList;
	}
	if (tag&TOWIN)
	{
		vector<Point>().swap(dest.winList);
		dest.winList = src.winList;
	}
	if (tag&TOWILLWIN)
	{
		vector<Point>().swap(dest.willWinList);
		dest.willWinList = src.willWinList;
	}
	if (tag&TODUOTHREAT)
	{
		vector<Point>().swap(dest.duoThreatList);
		dest.duoThreatList = src.duoThreatList;
	}
	if (tag&TOSOLTHREAT)
	{
		vector<Point>().swap(dest.solThreatList);
		dest.solThreatList = src.solThreatList;
	}
	if (tag&TODUOPOTEN)
	{
		vector<Point>().swap(dest.duoPotenList);
		dest.duoPotenList = src.duoPotenList;
	}
	if (tag&TOSOLPOTEN)
	{
		vector<Point>().swap(dest.solPotenList);
		dest.solPotenList = src.solPotenList;
	}
	if (tag&TOCOMMON)
	{
		vector<Point>().swap(dest.toDuoTwoList);
		dest.toDuoTwoList = src.toDuoTwoList;
	}
}

/**
 * ValueType - 分析指定线型
 * @style：		线型索引，由Analizeline函数提取的棋形的二进制描述。
 * @start:		线型的起始点绝对坐标
 * @lineDirec:	线型所在线的方向
 * @return:		返回线信息，包括：线形威胁类型、估值，防御步、防御点、威胁点、潜力点、潜潜力点的绝对坐标。
 */
LineInfo ValuateType(int style, Point start, BYTE lineDirec, int tag)
{
	int ShapeStyleId;	//棋形类型编号（1~14）。
	Point tempPoint;
	Step tempStep;
	LineInfo lineInfo;
	vector<iPoint>::iterator iterP;
	vector<iStep>::iterator iterS;

	ShapeStyleId = preTable[style];//获得线型类型
	lineInfo.value = LineTypeValue[ShapeStyleId];//获得线型价值
	lineInfo.LineType = LineTypeThreat[ShapeStyleId];//获得线型的威胁类型

	if (ShapeStyleId == ZERO)//空线型
		return lineInfo;
	if (ShapeStyleId >= WIN)//已胜线型
		return lineInfo;

	if (ShapeStyleId >= THREAT_four_SINGLE)//威胁型
	{
		if (tag&TODEFENT)
		{
			if (linetypeInfo[style].defStepList.size() > 0)//防御步
				for (iterS = linetypeInfo[style].defStepList.begin(); iterS != linetypeInfo[style].defStepList.end(); iterS++)
				{
					tempStep.first.x = start.x + iterS->first*lineVector[lineDirec][0];
					tempStep.first.y = start.y + iterS->first*lineVector[lineDirec][1];
					tempStep.second.x = start.x + iterS->second*lineVector[lineDirec][0];
					tempStep.second.y = start.y + iterS->second*lineVector[lineDirec][1];
					lineInfo.defStepList.push_back(tempStep);
				}
			if (linetypeInfo[style].defPointList.size() > 0)//防御点
				for (iterP = linetypeInfo[style].defPointList.begin(); iterP != linetypeInfo[style].defPointList.end(); iterP++)
				{
					tempPoint.x = start.x + (*iterP)*lineVector[lineDirec][0];
					tempPoint.y = start.y + (*iterP)*lineVector[lineDirec][1];
					lineInfo.defPointList.push_back(tempPoint);
				}
		}
		if ((tag&TOWIN) && linetypeInfo[style].win != -1)//制胜点
		{
			tempPoint.x = start.x + linetypeInfo[style].win*lineVector[lineDirec][0];
			tempPoint.y = start.y + linetypeInfo[style].win*lineVector[lineDirec][1];
			lineInfo.winList.push_back(tempPoint);
		}
		if ((tag&TOWILLWIN) && linetypeInfo[style].willWin != -1)//即将制胜点
		{
			tempPoint.x = start.x + linetypeInfo[style].willWin*lineVector[lineDirec][0];
			tempPoint.y = start.y + linetypeInfo[style].willWin*lineVector[lineDirec][1];
			lineInfo.willWinList.push_back(tempPoint);
		}
	}
	if ((tag&TODUOTHREAT) && linetypeInfo[style].duoThreatList.size() > 0)
		for (iterP = linetypeInfo[style].duoThreatList.begin(); iterP != linetypeInfo[style].duoThreatList.end(); iterP++)
		{
			tempPoint.x = start.x + (*iterP)*lineVector[lineDirec][0];
			tempPoint.y = start.y + (*iterP)*lineVector[lineDirec][1];
			lineInfo.duoThreatList.push_back(tempPoint);
		}
	if ((tag&TOSOLTHREAT) && linetypeInfo[style].solThreatList.size() > 0)
		for (iterP = linetypeInfo[style].solThreatList.begin(); iterP != linetypeInfo[style].solThreatList.end(); iterP++)
		{
			tempPoint.x = start.x + (*iterP)*lineVector[lineDirec][0];
			tempPoint.y = start.y + (*iterP)*lineVector[lineDirec][1];
			lineInfo.solThreatList.push_back(tempPoint);
		}
	if ((tag&TODUOPOTEN) && linetypeInfo[style].duoPotenList.size() > 0)
		for (iterP = linetypeInfo[style].duoPotenList.begin(); iterP != linetypeInfo[style].duoPotenList.end(); iterP++)
		{
			tempPoint.x = start.x + (*iterP)*lineVector[lineDirec][0];
			tempPoint.y = start.y + (*iterP)*lineVector[lineDirec][1];
			lineInfo.duoPotenList.push_back(tempPoint);
		}
	if ((tag&TOSOLPOTEN) && linetypeInfo[style].solPotenList.size() > 0)
		for (iterP = linetypeInfo[style].solPotenList.begin(); iterP != linetypeInfo[style].solPotenList.end(); iterP++)
		{
			tempPoint.x = start.x + (*iterP)*lineVector[lineDirec][0];
			tempPoint.y = start.y + (*iterP)*lineVector[lineDirec][1];
			lineInfo.solPotenList.push_back(tempPoint);
		}
	if ((tag&TOCOMMON) && linetypeInfo[style].toDuoTwoList.size() > 0)
		for (iterP = linetypeInfo[style].toDuoTwoList.begin(); iterP != linetypeInfo[style].toDuoTwoList.end(); iterP++)
		{
			tempPoint.x = start.x + (*iterP)*lineVector[lineDirec][0];
			tempPoint.y = start.y + (*iterP)*lineVector[lineDirec][1];
			lineInfo.toDuoTwoList.push_back(tempPoint);
		}
	return lineInfo;
}

/**
 * AnalyzeLine - 通过线的起始点和方向分析线
 * @start:		线的起始点绝对坐标
 * @lineDirec:	线的方向标志
 * @side:		分析的线信息所属方执棋颜色
 * @return:		返回线信息
 */
LineInfo AnalyzeLine(Point start, BYTE lineDirec, BYTE side, int tag)
{
	int shapeIndex = 0;	// 线型索引
	int x = start.x;	// 从起点开始找
	int y = start.y;
	Point LinePos;		//被分析的线形的起点绝对坐标。用-1表示还未确定出被分析线形的起点（棋盘的有效点的坐标值都不小于0）。
	int len = 0;		//记录当前位置在所要求的段中的位置, 可以理解为当前段长
	vector<Point>::iterator iterP;
	vector<Step>::iterator iterS;
	LineInfo lineInfo, lineInfoAssistor;

	lineInfo.value = 0;
	lineInfo.LineType = 0;
	LinePos.x = -1;

	//提取线型
	while (x<edge && y<edge && x>-1 && y>-1)	//	点在棋盘内
	{
		if (virtualBoard[x][y] == EMPTY)//空点
		{
			len++;
			if (LinePos.x == -1)//确定起始位置
			{
				LinePos.x = x; LinePos.y = y;
			}
		}
		else if (virtualBoard[x][y] == side)		// 如果是本方棋子。
		{
			shapeIndex += (1 << len);
			len++;
			if (LinePos.x == -1)//确定起始位置
			{
				LinePos.x = x; LinePos.y = y;
			}
		}
		else		//  如果是对方棋子，本方线型已确定。
		{
			if (len > 5)	//长度超过5个才会重视。
			{
				shapeIndex += (1 << len);	  //设置线型边界，得到线型索引。
				lineInfoAssistor = ValuateType(shapeIndex, LinePos, lineDirec, tag);//分析该线型包含的信息
				//不会对具有已胜线型的已胜局面进行信息采集
				lineInfo.value += lineInfoAssistor.value;//累加线的价值
				lineInfo.LineType += lineInfoAssistor.LineType;//累加线的威胁类型
				//复制线型信息
				if (lineInfoAssistor.LineType)//威胁型
				{
					if (lineInfoAssistor.defPointList.size() > 0)//防御点
						for (iterP = lineInfoAssistor.defPointList.begin(); iterP != lineInfoAssistor.defPointList.end(); iterP++)
							lineInfo.defPointList.push_back(*iterP);
					if (lineInfoAssistor.defStepList.size() > 0)//防御步
						for (iterS = lineInfoAssistor.defStepList.begin(); iterS != lineInfoAssistor.defStepList.end(); iterS++)
							lineInfo.defStepList.push_back(*iterS);
					if (lineInfoAssistor.winList.size() > 0)//致胜点
						lineInfo.winList.push_back(lineInfoAssistor.winList[0]);
					if (lineInfoAssistor.willWinList.size() > 0)//即将胜利点
						lineInfo.willWinList.push_back(lineInfoAssistor.willWinList[0]);
				}
				if (lineInfoAssistor.duoThreatList.size() > 0)//双威胁点
					for (iterP = lineInfoAssistor.duoThreatList.begin(); iterP != lineInfoAssistor.duoThreatList.end(); iterP++)
						lineInfo.duoThreatList.push_back(*iterP);
				if (lineInfoAssistor.solThreatList.size() > 0)//单威胁点
					for (iterP = lineInfoAssistor.solThreatList.begin(); iterP != lineInfoAssistor.solThreatList.end(); iterP++)
						lineInfo.solThreatList.push_back(*iterP);
				if (lineInfoAssistor.duoPotenList.size() > 0)//双潜力点
					for (iterP = lineInfoAssistor.duoPotenList.begin(); iterP != lineInfoAssistor.duoPotenList.end(); iterP++)
						lineInfo.duoPotenList.push_back(*iterP);
				if (lineInfoAssistor.solPotenList.size() > 0)//单潜力点
					for (iterP = lineInfoAssistor.solPotenList.begin(); iterP != lineInfoAssistor.solPotenList.end(); iterP++)
						lineInfo.solPotenList.push_back(*iterP);
				if (lineInfoAssistor.toDuoTwoList.size() > 0)//潜双潜力点
					for (iterP = lineInfoAssistor.toDuoTwoList.begin(); iterP != lineInfoAssistor.toDuoTwoList.end(); iterP++)
						lineInfo.toDuoTwoList.push_back(*iterP);
			}
			shapeIndex = 0;	// 索引归零
			len = 0;		// 长度归零
			LinePos.x = -1;
		}
		Increment(x, y, lineDirec);
	}
	if (len > 5)	//长度超过5个才会重视。
	{
		shapeIndex += (1 << len);	  // 得到线型索引
		lineInfoAssistor = ValuateType(shapeIndex, LinePos, lineDirec, tag);
		//不会对具有已胜线型的已胜局面进行信息采集
		lineInfo.value += lineInfoAssistor.value;//累加线的价值
		lineInfo.LineType += lineInfoAssistor.LineType;//累加线的威胁类型
		if (lineInfoAssistor.LineType > 0)//威胁型
		{
			if (lineInfoAssistor.defPointList.size() > 0)
				for (iterP = lineInfoAssistor.defPointList.begin(); iterP != lineInfoAssistor.defPointList.end(); iterP++)
					lineInfo.defPointList.push_back(*iterP);
			if (lineInfoAssistor.defStepList.size() > 0)
				for (iterS = lineInfoAssistor.defStepList.begin(); iterS != lineInfoAssistor.defStepList.end(); iterS++)
					lineInfo.defStepList.push_back(*iterS);
			if (lineInfoAssistor.winList.size() > 0)//致胜点
				lineInfo.winList.push_back(lineInfoAssistor.winList[0]);
			if (lineInfoAssistor.willWinList.size() > 0)//即将胜利点
				lineInfo.willWinList.push_back(lineInfoAssistor.willWinList[0]);
		}
		if (lineInfoAssistor.duoThreatList.size() > 0)
			for (iterP = lineInfoAssistor.duoThreatList.begin(); iterP != lineInfoAssistor.duoThreatList.end(); iterP++)
				lineInfo.duoThreatList.push_back(*iterP);
		if (lineInfoAssistor.solThreatList.size() > 0)
			for (iterP = lineInfoAssistor.solThreatList.begin(); iterP != lineInfoAssistor.solThreatList.end(); iterP++)
				lineInfo.solThreatList.push_back(*iterP);
		if (lineInfoAssistor.duoPotenList.size() > 0)
			for (iterP = lineInfoAssistor.duoPotenList.begin(); iterP != lineInfoAssistor.duoPotenList.end(); iterP++)
				lineInfo.duoPotenList.push_back(*iterP);
		if (lineInfoAssistor.solPotenList.size() > 0)
			for (iterP = lineInfoAssistor.solPotenList.begin(); iterP != lineInfoAssistor.solPotenList.end(); iterP++)
				lineInfo.solPotenList.push_back(*iterP);
		if (lineInfoAssistor.toDuoTwoList.size() > 0)
			for (iterP = lineInfoAssistor.toDuoTwoList.begin(); iterP != lineInfoAssistor.toDuoTwoList.end(); iterP++)
				lineInfo.toDuoTwoList.push_back(*iterP);
	}
	return lineInfo;
}

/**
 * UpdateLineForCross - 通过点更新线的信息
 * @point:	落子点
 * @side:	点所属方的执棋颜色
 * @return:	无返回值
 */
void UpdateLineForCross(Point point, BYTE side, int tag)
{
	int key;
	Point start;
	LineInfo lineTemp;
	if (point.x == -1)//弃子点
		return;
	key = GetLineKey(point, &start, ANGLE0);//获得点所在横向线的索引和线的起始点
	if (key != -1)
	{
		lineTemp = AnalyzeLine(start, ANGLE0, side, tag);//从线的起始点开始分析该线的信息
		CopyLineInfo(lineInfo[side][key], lineTemp, tag);//复制线的信息到表中
	}
	key = GetLineKey(point, &start, ANGLE90);//纵向
	if (key != -1)
	{
		lineTemp = AnalyzeLine(start, ANGLE90, side, tag);
		CopyLineInfo(lineInfo[side][key], lineTemp, tag);
	}
	key = GetLineKey(point, &start, ANGLE45);//向右斜下
	if (key != -1)
	{
		lineTemp = AnalyzeLine(start, ANGLE45, side, tag);
		CopyLineInfo(lineInfo[side][key], lineTemp, tag);
	}
	key = GetLineKey(point, &start, ANGLE135);//向左斜下
	if (key != -1)
	{
		lineTemp = AnalyzeLine(start, ANGLE135, side, tag);
		CopyLineInfo(lineInfo[side][key], lineTemp, tag);
	}
}

/**
 * BackupLine - 备份线信息，针对点所在的4条线，将其信息储存到临时变量中
 * @point:		目标点
 * @tempLine:	临时储存区
 * @side:		棋手方标志
 * @return:		无返回值
 */
void BackupLine(Point point, LineInfo tempLine[4], BYTE side)
{
	if (point.x == -1)return;
	int key;
	Point start;
	key = GetLineKey(point, &start, ANGLE0);
	if (key != -1)
		CopyLineInfo(tempLine[ANGLE0], lineInfo[side][key]);
	key = GetLineKey(point, &start, ANGLE90);
	if (key != -1)
		CopyLineInfo(tempLine[ANGLE90], lineInfo[side][key]);
	key = GetLineKey(point, &start, ANGLE45);
	if (key != -1)
		CopyLineInfo(tempLine[ANGLE45], lineInfo[side][key]);
	key = GetLineKey(point, &start, ANGLE135);
	if (key != -1)
		CopyLineInfo(tempLine[ANGLE135], lineInfo[side][key]);
}

/**
 * MakeMove - 试走，同时对变更的线进行备份
 * @point:		落子点坐标
 * @tempLine:	线信息备份储存器
 * @side:		棋手方标志
 * @tag:		标签
 * @return:		无返回值
 */
void MakeMove(Point point, LineInfo tempLine[2][4], BYTE side, int tag)
{
	//信息备份
	BackupLine(point, tempLine[BLACK], BLACK);
	BackupLine(point, tempLine[WHITE], WHITE);

	//试走
	virtualBoard[point.x][point.y] = side;

	//更新
	UpdateLineForCross(point, BLACK, tag);
	UpdateLineForCross(point, WHITE, tag);
}

/**
 * RecoverLine - 恢复线信息，针对点所在的4条线，用临时变量中的信息覆盖当前信息
 * @point:		目标点
 * @tempLine:	信息储存区
 * @side:		棋手方标志
 * @return:		无返回值
 */
void RecoverLine(Point point, LineInfo tempLine[4], BYTE side)
{
	if (point.x == -1)return;
	int key;
	Point start;
	key = GetLineKey(point, &start, ANGLE0);
	if (key != -1)
		CopyLineInfo(lineInfo[side][key], tempLine[ANGLE0]);
	key = GetLineKey(point, &start, ANGLE90);
	if (key != -1)
		CopyLineInfo(lineInfo[side][key], tempLine[ANGLE90]);
	key = GetLineKey(point, &start, ANGLE45);
	if (key != -1)
		CopyLineInfo(lineInfo[side][key], tempLine[ANGLE45]);
	key = GetLineKey(point, &start, ANGLE135);
	if (key != -1)
		CopyLineInfo(lineInfo[side][key], tempLine[ANGLE135]);
}

/**
 * BackMove - 试走恢复，同时恢复变更的线信息
 * @point:		试走点坐标
 * @tempLine:	备份信息储存器
 * @side:		棋手方标志
 * @return:		无返回值
 */
void BackMove(Point point, LineInfo tempLine[2][4], BYTE side)
{
	//取消试走
	virtualBoard[point.x][point.y] = EMPTY;

	//信息恢复
	RecoverLine(point, tempLine[BLACK], BLACK);
	RecoverLine(point, tempLine[WHITE], WHITE);
}


/**
 * GetBoardType - 获取当前棋局状态威胁类型信息
 * @side:	棋手方标志
 * @return:	返回威胁类型
 */
int GetBoardType(BYTE side)
{
	int i, type = 0;
	for (i = 0; i < 92; i++)//查找92条线
		type += lineInfo[side][i].LineType;//累加线的威胁类型作为局面的威胁类型
	return type;
}

/**
 * GetBoardValue - 获取当前棋局状态估值
 * @side:	棋手方标志
 * @return:	棋局状态的线性估值
 */
int GetBoardValue(BYTE side)
{
	int i, value = 0;
	for (i = 0; i < 92; i++)//查找92条线
		value += lineInfo[side][i].value;//累加线的价值作为局面价值
	return value;
}

/**
 * GetBoardInfo - 获取当前棋局状态信息
 * @side:	棋手方标志
 * @tag:	收集信息策略按位标志，除1标志外棋局价值和威胁类型为默认收集信息
 * @return:	返回棋局信息
 */
SynInfo GetBoardInfo(BYTE side, int tag)
{
	int i;
	SynInfo tempSynInfo;
	vector<Point>::iterator iterP;
	vector<Step>::iterator iterS;

	for (i = 0; i < 92; i++)//查找92条线
	{
		if (tag&TODEFENT)//防御信息
		{
			if (lineInfo[side][i].defPointList.size() > 0)
				for (iterP = lineInfo[side][i].defPointList.begin(); iterP != lineInfo[side][i].defPointList.end(); iterP++)
					tempSynInfo.defPointList.push_back(*iterP);
			if (lineInfo[side][i].defStepList.size() > 0)
				for (iterS = lineInfo[side][i].defStepList.begin(); iterS != lineInfo[side][i].defStepList.end(); iterS++)
					tempSynInfo.defStepList.push_back(*iterS);
		}
		if ((tag&TOWIN) && lineInfo[side][i].winList.size() > 0)//致胜点
			tempSynInfo.winList.push_back(lineInfo[side][i].winList[0]);
		if ((tag&TOWILLWIN) && lineInfo[side][i].willWinList.size() > 0)//即将胜利点
			tempSynInfo.willWinList.push_back(lineInfo[side][i].willWinList[0]);
		if ((tag&TODUOTHREAT) && lineInfo[side][i].duoThreatList.size() > 0)//双威胁点
			for (iterP = lineInfo[side][i].duoThreatList.begin(); iterP != lineInfo[side][i].duoThreatList.end(); iterP++)
				tempSynInfo.duoThreatList.push_back(*iterP);
		if ((tag&TOSOLTHREAT) && lineInfo[side][i].solThreatList.size() > 0)//单威胁定
			for (iterP = lineInfo[side][i].solThreatList.begin(); iterP != lineInfo[side][i].solThreatList.end(); iterP++)
				tempSynInfo.solThreatList.push_back(*iterP);
		if ((tag&TODUOPOTEN) && lineInfo[side][i].duoPotenList.size() > 0)//双潜力点
			for (iterP = lineInfo[side][i].duoPotenList.begin(); iterP != lineInfo[side][i].duoPotenList.end(); iterP++)
				tempSynInfo.duoPotenList.push_back(*iterP);
		if ((tag&TOSOLPOTEN) && lineInfo[side][i].solPotenList.size() > 0)//单潜力点
			for (iterP = lineInfo[side][i].solPotenList.begin(); iterP != lineInfo[side][i].solPotenList.end(); iterP++)
				tempSynInfo.solPotenList.push_back(*iterP);
		if ((tag&TOCOMMON) && lineInfo[side][i].toDuoTwoList.size() > 0)//普通点
			for (iterP = lineInfo[side][i].toDuoTwoList.begin(); iterP != lineInfo[side][i].toDuoTwoList.end(); iterP++)
				tempSynInfo.toDuoTwoList.push_back(*iterP);
	}
	if (tag&TODEFENT)
		UniquePoint(tempSynInfo.defPointList);
	if (tag&TODUOTHREAT)
		UniquePoint(tempSynInfo.duoThreatList);
	if (tag&TOSOLTHREAT)
		UniquePoint(tempSynInfo.solThreatList);
	if (tag&TODUOPOTEN)
		UniquePoint(tempSynInfo.duoPotenList);
	if (tag&TOSOLPOTEN)
		UniquePoint(tempSynInfo.solPotenList);
	if (tag&TOCOMMON)
		UniquePoint(tempSynInfo.toDuoTwoList);
	return tempSynInfo;
}


/**
 * CalSingleLineValue - 线估值计算器
 * @start:	线的起始点坐标
 * @Direct:	线的方向标志
 * @side:	被计算的线的价值信息所属方的执棋颜色
 * @return:	返回线的估值
 */
int CalSingleLineValue(const Point start, const BYTE Direc, const BYTE side) //计算点所在Direc方向的线的价值
{
	int shapeIndex = 0;	// 线型编号
	int x = start.x;	// 从起点开始找
	int y = start.y;
	int len = 0;				//  记录当前位置在所要求的段中的位置, 可以理解为当前段长
	int value = 0;
	while (x<edge&&y<edge&&x>-1 && y>-1)	//	点在棋盘内
	{
		if (virtualBoard[x][y] == EMPTY)
		{
			len++;
		}
		else if (virtualBoard[x][y] == side)		// 如果是本方棋子。
		{
			shapeIndex += (1 << len);
			len++;
		}
		else		//  如果是对方棋子。
		{
			if (len > 5)	//长度超过5个才会重视。
			{
				shapeIndex += (1 << len);	  // 得到的类型在知识库中的位置。
				value += LineTypeValue[preTable[shapeIndex]];
			}
			shapeIndex = 0;	// 偏移量归零
			len = 0;		// 长度归零
		}
		Increment(x, y, Direc);
	}
	if (len > 5)	//长度超过5个才会重视。
	{
		shapeIndex += (1 << len);	  // 得到的棋型
		value += LineTypeValue[preTable[shapeIndex]];
	}
	return value;
}

/**
 * CalSingleLineType - 线类型计算器
 * @start:	线的起始点坐标
 * @Direct:	线的方向标志
 * @side:	被计算的线的类型信息所属方的执棋颜色
 * @return:	返回线的类型，属性成分
 */
int CalSingleLineType(const Point start, const BYTE Direc, const BYTE side) //计算点所在Direc方向的线的价值
{
	int shapeIndex = 0;	// 线型编号
	int x = start.x;	// 从起点开始找
	int y = start.y;
	int len = 0;				//  记录当前位置在所要求的段中的位置, 可以理解为当前段长
	int type = 0;
	while (x<edge&&y<edge&&x>-1 && y>-1)	//	点在棋盘内
	{
		if (virtualBoard[x][y] == EMPTY)
		{
			len++;
		}
		else if (virtualBoard[x][y] == side)		// 如果是本方棋子。
		{
			shapeIndex += (1 << len);
			len++;
		}
		else		//  如果是对方棋子。
		{
			if (len > 5)	//长度超过5个才会重视。
			{
				shapeIndex += (1 << len);	  // 得到的类型在知识库中的位置。
				type += LineTypeType[preTable[shapeIndex]];
			}
			shapeIndex = 0;	// 偏移量归零
			len = 0;		// 长度归零
		}
		Increment(x, y, Direc);
	}
	if (len > 5)	//长度超过5个才会重视。
	{
		shapeIndex += (1 << len);	  // 得到的棋型
		type += LineTypeType[preTable[shapeIndex]];
	}
	return type;
}


/**
 * CalculateStepValue - 着法估值计算器
 * @step:	需要被计算的着法
 * @side:	着法所属方执棋颜色
 * @return:	返回着法估值
 */
int CalculateStepValue(const Step step, const BYTE side)
{
	int key1, key2, i, unside = 1 - side;
	int myC = 0, denC = 0;//双方变化后线型价值和
	Point start;

	virtualBoard[step.first.x][step.first.y] = side;
	virtualBoard[step.second.x][step.second.y] = side;
	for (i = 0; i < 4; i++)
	{
		key1 = GetLineKey(step.first, &start, i);
		if (key1 != -1)
		{
			myC += CalSingleLineValue(start, i, side);
			denC += CalSingleLineValue(start, i, unside);
			myC -= lineInfo[side][key1].value;
			denC -= lineInfo[unside][key1].value;
		}
		key2 = GetLineKey(step.second, &start, i);
		if (key2 == key1)
			continue;
		else if (key2 != -1)
		{
			myC += CalSingleLineValue(start, i, side);
			denC += CalSingleLineValue(start, i, unside);
			myC -= lineInfo[side][key2].value;
			denC -= lineInfo[unside][key2].value;
		}
	}
	virtualBoard[step.first.x][step.first.y] = EMPTY;
	virtualBoard[step.second.x][step.second.y] = EMPTY;
	return (myC * 9 - denC * 12) / 10;
}
