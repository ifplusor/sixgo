#include "MaintainInfo.h"


//const int LineTypeValue[WIN+1]={0, 0, 0,0,0,1, 3,4,5,9, 8,12,15,16,20,31,33, 39,45,47,54,89,100,1000, 37,76,1000, 10000 };//���ͼ�ֵ��

//��*��ʾ����                         0| 1| 2  3  4  5| 6  7  8   9| 10 *11  12  13 *14  15 *16| 17  18  19  *20   21  *22    *23| 24   25    *26|     27
const int LineTypeValue[WIN + 1] = { 0, 0, 1, 1, 1, 3, 5, 6, 8, 16, 15, 18, 20, 24, 28, 31, 39, 75, 90, 110, 115, 186, 236, 99999, 94, 196, 99999, 100000 };//���ͼ�ֵ��
//�޼�ֵ��|0ǱǱ˫Ǳ����1Ǳ��Ǳ����2Ǳ��˫Ǳ����3Ǳ˫Ǳ����4Ǳ��Ǳ����5��Ǳ����
//         6��˫Ǳ����7˫Ǳ����8��Ǳ����9����в��10��˫��в��11˫��в|������в����ʤ
//             Ǳ��Ǳ��16->��Ǳ��100->����в1000->��ʤ
//ǱǱ˫Ǳ��3->Ǳ˫Ǳ��11->˫Ǳ��31->˫��в155->��ʤ10000
//            Ǳ��˫Ǳ��7->��˫Ǳ��19->��˫��в95
//             Ǳ��Ǳ��5->��Ǳ��15->����в75->��ʤ

const int LineTypeThreat[WIN + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 10, 10, 100, 1, 10, 100, 1000 };//������в���ͱ�1����в��10˫��в��100����в��1000��ʤ
const int LineTypeType[WIN + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 10, 10, 11, 11, 10, 100, 100, 1000, 1100, 1100, 1000, 2000, 2000, 3000, 1000, 2000, 3000, 10000 };//�������ͱ�

LineInfo lineInfo[2][92];//�ߵ���Ϣ��Ӧ�þֲ����¼���ά��������Ϣ
int virtualBoard[edge][edge];//��������


/**
 * initialLine - ��ʼ��LineInfo����
 * @lineInfo:	��Ҫ���г�ʼ���ı���ָ��
 * @return:		�޷���ֵ
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
	lineInfo->value = 0;
	lineInfo->LineType = 0;			//������ߵ�����  -1:ʤ  0:����в;  1:����в;  2:˫��в  3:����в
}

/**
 * initialAllLine - ��ʼ���������˫��92������Ϣ��lineInfo[2][92]����
 * @return:		�޷���ֵ
 */
void initialAllLine()
{
	int i, j;
	for (i = 0; i < 2; i++)
		for (j = 0; j < 92; j++)
			initialLine(&lineInfo[i][j]);
}

/**
 * CopyLineInfo - ����Ϣ����
 * @dest:	���и��Ƶ�Ŀ������
 * @src:	���и��Ƶ�Դ����
 * @return:	�޷���ֵ
 */
void CopyLineInfo(LineInfo &dest, LineInfo &src, int tag = TOALL)
{
	dest.value = src.value;
	dest.LineType = src.LineType;
	if (tag&TODEFENT)
	{
		vector<Point>().swap(dest.defPointList);//�ͷ��ڴ�
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
 * ValueType - ����ָ������
 * @style��		������������Analizeline������ȡ�����εĶ�����������
 * @start:		���͵���ʼ���������
 * @lineDirec:	���������ߵķ���
 * @return:		��������Ϣ��������������в���͡���ֵ���������������㡢��в�㡢Ǳ���㡢ǱǱ����ľ������ꡣ
 */
LineInfo ValuateType(int style, Point start, BYTE lineDirec, int tag)
{
	int ShapeStyleId;	//�������ͱ�ţ�1~14����
	Point tempPoint;
	Step tempStep;
	LineInfo lineInfo;
	vector<iPoint>::iterator iterP;
	vector<iStep>::iterator iterS;

	ShapeStyleId = preTable[style];//�����������
	lineInfo.value = LineTypeValue[ShapeStyleId];//������ͼ�ֵ
	lineInfo.LineType = LineTypeThreat[ShapeStyleId];//������͵���в����

	if (ShapeStyleId == ZERO)//������
		return lineInfo;
	if (ShapeStyleId >= WIN)//��ʤ����
		return lineInfo;

	if (ShapeStyleId >= THREAT_four_SINGLE)//��в��
	{
		if (tag&TODEFENT)
		{
			if (linetypeInfo[style].defStepList.size() > 0)//������
				for (iterS = linetypeInfo[style].defStepList.begin(); iterS != linetypeInfo[style].defStepList.end(); iterS++)
				{
					tempStep.first.x = start.x + iterS->first*lineVector[lineDirec][0];
					tempStep.first.y = start.y + iterS->first*lineVector[lineDirec][1];
					tempStep.second.x = start.x + iterS->second*lineVector[lineDirec][0];
					tempStep.second.y = start.y + iterS->second*lineVector[lineDirec][1];
					lineInfo.defStepList.push_back(tempStep);
				}
			if (linetypeInfo[style].defPointList.size() > 0)//������
				for (iterP = linetypeInfo[style].defPointList.begin(); iterP != linetypeInfo[style].defPointList.end(); iterP++)
				{
					tempPoint.x = start.x + (*iterP)*lineVector[lineDirec][0];
					tempPoint.y = start.y + (*iterP)*lineVector[lineDirec][1];
					lineInfo.defPointList.push_back(tempPoint);
				}
		}
		if ((tag&TOWIN) && linetypeInfo[style].win != -1)//��ʤ��
		{
			tempPoint.x = start.x + linetypeInfo[style].win*lineVector[lineDirec][0];
			tempPoint.y = start.y + linetypeInfo[style].win*lineVector[lineDirec][1];
			lineInfo.winList.push_back(tempPoint);
		}
		if ((tag&TOWILLWIN) && linetypeInfo[style].willWin != -1)//������ʤ��
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
 * AnalyzeLine - ͨ���ߵ���ʼ��ͷ��������
 * @start:		�ߵ���ʼ���������
 * @lineDirec:	�ߵķ����־
 * @side:		����������Ϣ������ִ����ɫ
 * @return:		��������Ϣ
 */
LineInfo AnalyzeLine(Point start, BYTE lineDirec, BYTE side, int tag)
{
	int shapeIndex = 0;	// ��������
	int x = start.x;	// ����㿪ʼ��
	int y = start.y;
	Point LinePos;		//�����������ε����������ꡣ��-1��ʾ��δȷ�������������ε���㣨���̵���Ч�������ֵ����С��0����
	int len = 0;		//��¼��ǰλ������Ҫ��Ķ��е�λ��, �������Ϊ��ǰ�γ�
	vector<Point>::iterator iterP;
	vector<Step>::iterator iterS;
	LineInfo lineInfo, lineInfoAssistor;

	lineInfo.value = 0;
	lineInfo.LineType = 0;
	LinePos.x = -1;

	//��ȡ����
	while (x<edge && y<edge && x>-1 && y>-1)	//	����������
	{
		if (virtualBoard[x][y] == EMPTY)//�յ�
		{
			len++;
			if (LinePos.x == -1)//ȷ����ʼλ��
			{
				LinePos.x = x; LinePos.y = y;
			}
		}
		else if (virtualBoard[x][y] == side)		// ����Ǳ������ӡ�
		{
			shapeIndex += (1 << len);
			len++;
			if (LinePos.x == -1)//ȷ����ʼλ��
			{
				LinePos.x = x; LinePos.y = y;
			}
		}
		else		//  ����ǶԷ����ӣ�����������ȷ����
		{
			if (len > 5)	//���ȳ���5���Ż����ӡ�
			{
				shapeIndex += (1 << len);	  //�������ͱ߽磬�õ�����������
				lineInfoAssistor = ValuateType(shapeIndex, LinePos, lineDirec, tag);//���������Ͱ�������Ϣ
				//����Ծ�����ʤ���͵���ʤ���������Ϣ�ɼ�
				lineInfo.value += lineInfoAssistor.value;//�ۼ��ߵļ�ֵ
				lineInfo.LineType += lineInfoAssistor.LineType;//�ۼ��ߵ���в����
				//����������Ϣ
				if (lineInfoAssistor.LineType)//��в��
				{
					if (lineInfoAssistor.defPointList.size() > 0)//������
						for (iterP = lineInfoAssistor.defPointList.begin(); iterP != lineInfoAssistor.defPointList.end(); iterP++)
							lineInfo.defPointList.push_back(*iterP);
					if (lineInfoAssistor.defStepList.size() > 0)//������
						for (iterS = lineInfoAssistor.defStepList.begin(); iterS != lineInfoAssistor.defStepList.end(); iterS++)
							lineInfo.defStepList.push_back(*iterS);
					if (lineInfoAssistor.winList.size() > 0)//��ʤ��
						lineInfo.winList.push_back(lineInfoAssistor.winList[0]);
					if (lineInfoAssistor.willWinList.size() > 0)//����ʤ����
						lineInfo.willWinList.push_back(lineInfoAssistor.willWinList[0]);
				}
				if (lineInfoAssistor.duoThreatList.size() > 0)//˫��в��
					for (iterP = lineInfoAssistor.duoThreatList.begin(); iterP != lineInfoAssistor.duoThreatList.end(); iterP++)
						lineInfo.duoThreatList.push_back(*iterP);
				if (lineInfoAssistor.solThreatList.size() > 0)//����в��
					for (iterP = lineInfoAssistor.solThreatList.begin(); iterP != lineInfoAssistor.solThreatList.end(); iterP++)
						lineInfo.solThreatList.push_back(*iterP);
				if (lineInfoAssistor.duoPotenList.size() > 0)//˫Ǳ����
					for (iterP = lineInfoAssistor.duoPotenList.begin(); iterP != lineInfoAssistor.duoPotenList.end(); iterP++)
						lineInfo.duoPotenList.push_back(*iterP);
				if (lineInfoAssistor.solPotenList.size() > 0)//��Ǳ����
					for (iterP = lineInfoAssistor.solPotenList.begin(); iterP != lineInfoAssistor.solPotenList.end(); iterP++)
						lineInfo.solPotenList.push_back(*iterP);
				if (lineInfoAssistor.toDuoTwoList.size() > 0)//Ǳ˫Ǳ����
					for (iterP = lineInfoAssistor.toDuoTwoList.begin(); iterP != lineInfoAssistor.toDuoTwoList.end(); iterP++)
						lineInfo.toDuoTwoList.push_back(*iterP);
			}
			shapeIndex = 0;	// ��������
			len = 0;		// ���ȹ���
			LinePos.x = -1;
		}
		Increment(x, y, lineDirec);
	}
	if (len > 5)	//���ȳ���5���Ż����ӡ�
	{
		shapeIndex += (1 << len);	  // �õ���������
		lineInfoAssistor = ValuateType(shapeIndex, LinePos, lineDirec, tag);
		//����Ծ�����ʤ���͵���ʤ���������Ϣ�ɼ�
		lineInfo.value += lineInfoAssistor.value;//�ۼ��ߵļ�ֵ
		lineInfo.LineType += lineInfoAssistor.LineType;//�ۼ��ߵ���в����
		if (lineInfoAssistor.LineType > 0)//��в��
		{
			if (lineInfoAssistor.defPointList.size() > 0)
				for (iterP = lineInfoAssistor.defPointList.begin(); iterP != lineInfoAssistor.defPointList.end(); iterP++)
					lineInfo.defPointList.push_back(*iterP);
			if (lineInfoAssistor.defStepList.size() > 0)
				for (iterS = lineInfoAssistor.defStepList.begin(); iterS != lineInfoAssistor.defStepList.end(); iterS++)
					lineInfo.defStepList.push_back(*iterS);
			if (lineInfoAssistor.winList.size() > 0)//��ʤ��
				lineInfo.winList.push_back(lineInfoAssistor.winList[0]);
			if (lineInfoAssistor.willWinList.size() > 0)//����ʤ����
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
 * UpdateLineForCross - ͨ��������ߵ���Ϣ
 * @point:	���ӵ�
 * @side:	����������ִ����ɫ
 * @return:	�޷���ֵ
 */
void UpdateLineForCross(Point point, BYTE side, int tag)
{
	int key;
	Point start;
	LineInfo lineTemp;
	if (point.x == -1)//���ӵ�
		return;
	key = GetLineKey(point, &start, ANGLE0);//��õ����ں����ߵ��������ߵ���ʼ��
	if (key != -1)
	{
		lineTemp = AnalyzeLine(start, ANGLE0, side, tag);//���ߵ���ʼ�㿪ʼ�������ߵ���Ϣ
		CopyLineInfo(lineInfo[side][key], lineTemp, tag);//�����ߵ���Ϣ������
	}
	key = GetLineKey(point, &start, ANGLE90);//����
	if (key != -1)
	{
		lineTemp = AnalyzeLine(start, ANGLE90, side, tag);
		CopyLineInfo(lineInfo[side][key], lineTemp, tag);
	}
	key = GetLineKey(point, &start, ANGLE45);//����б��
	if (key != -1)
	{
		lineTemp = AnalyzeLine(start, ANGLE45, side, tag);
		CopyLineInfo(lineInfo[side][key], lineTemp, tag);
	}
	key = GetLineKey(point, &start, ANGLE135);//����б��
	if (key != -1)
	{
		lineTemp = AnalyzeLine(start, ANGLE135, side, tag);
		CopyLineInfo(lineInfo[side][key], lineTemp, tag);
	}
}

/**
 * BackupLine - ��������Ϣ����Ե����ڵ�4���ߣ�������Ϣ���浽��ʱ������
 * @point:		Ŀ���
 * @tempLine:	��ʱ������
 * @side:		���ַ���־
 * @return:		�޷���ֵ
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
 * MakeMove - ���ߣ�ͬʱ�Ա�����߽��б���
 * @point:		���ӵ�����
 * @tempLine:	����Ϣ���ݴ�����
 * @side:		���ַ���־
 * @tag:		��ǩ
 * @return:		�޷���ֵ
 */
void MakeMove(Point point, LineInfo tempLine[2][4], BYTE side, int tag)
{
	//��Ϣ����
	BackupLine(point, tempLine[BLACK], BLACK);
	BackupLine(point, tempLine[WHITE], WHITE);

	//����
	virtualBoard[point.x][point.y] = side;

	//����
	UpdateLineForCross(point, BLACK, tag);
	UpdateLineForCross(point, WHITE, tag);
}

/**
 * RecoverLine - �ָ�����Ϣ����Ե����ڵ�4���ߣ�����ʱ�����е���Ϣ���ǵ�ǰ��Ϣ
 * @point:		Ŀ���
 * @tempLine:	��Ϣ������
 * @side:		���ַ���־
 * @return:		�޷���ֵ
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
 * BackMove - ���߻ָ���ͬʱ�ָ����������Ϣ
 * @point:		���ߵ�����
 * @tempLine:	������Ϣ������
 * @side:		���ַ���־
 * @return:		�޷���ֵ
 */
void BackMove(Point point, LineInfo tempLine[2][4], BYTE side)
{
	//ȡ������
	virtualBoard[point.x][point.y] = EMPTY;

	//��Ϣ�ָ�
	RecoverLine(point, tempLine[BLACK], BLACK);
	RecoverLine(point, tempLine[WHITE], WHITE);
}


/**
 * GetBoardType - ��ȡ��ǰ���״̬��в������Ϣ
 * @side:	���ַ���־
 * @return:	������в����
 */
int GetBoardType(BYTE side)
{
	int i, type = 0;
	for (i = 0; i < 92; i++)//����92����
		type += lineInfo[side][i].LineType;//�ۼ��ߵ���в������Ϊ�������в����
	return type;
}

/**
 * GetBoardValue - ��ȡ��ǰ���״̬��ֵ
 * @side:	���ַ���־
 * @return:	���״̬�����Թ�ֵ
 */
int GetBoardValue(BYTE side)
{
	int i, value = 0;
	for (i = 0; i < 92; i++)//����92����
		value += lineInfo[side][i].value;//�ۼ��ߵļ�ֵ��Ϊ�����ֵ
	return value;
}

/**
 * GetBoardInfo - ��ȡ��ǰ���״̬��Ϣ
 * @side:	���ַ���־
 * @tag:	�ռ���Ϣ���԰�λ��־����1��־����ּ�ֵ����в����ΪĬ���ռ���Ϣ
 * @return:	���������Ϣ
 */
SynInfo GetBoardInfo(BYTE side, int tag)
{
	int i;
	SynInfo tempSynInfo;
	vector<Point>::iterator iterP;
	vector<Step>::iterator iterS;

	for (i = 0; i < 92; i++)//����92����
	{
		if (tag&TODEFENT)//������Ϣ
		{
			if (lineInfo[side][i].defPointList.size() > 0)
				for (iterP = lineInfo[side][i].defPointList.begin(); iterP != lineInfo[side][i].defPointList.end(); iterP++)
					tempSynInfo.defPointList.push_back(*iterP);
			if (lineInfo[side][i].defStepList.size() > 0)
				for (iterS = lineInfo[side][i].defStepList.begin(); iterS != lineInfo[side][i].defStepList.end(); iterS++)
					tempSynInfo.defStepList.push_back(*iterS);
		}
		if ((tag&TOWIN) && lineInfo[side][i].winList.size() > 0)//��ʤ��
			tempSynInfo.winList.push_back(lineInfo[side][i].winList[0]);
		if ((tag&TOWILLWIN) && lineInfo[side][i].willWinList.size() > 0)//����ʤ����
			tempSynInfo.willWinList.push_back(lineInfo[side][i].willWinList[0]);
		if ((tag&TODUOTHREAT) && lineInfo[side][i].duoThreatList.size() > 0)//˫��в��
			for (iterP = lineInfo[side][i].duoThreatList.begin(); iterP != lineInfo[side][i].duoThreatList.end(); iterP++)
				tempSynInfo.duoThreatList.push_back(*iterP);
		if ((tag&TOSOLTHREAT) && lineInfo[side][i].solThreatList.size() > 0)//����в��
			for (iterP = lineInfo[side][i].solThreatList.begin(); iterP != lineInfo[side][i].solThreatList.end(); iterP++)
				tempSynInfo.solThreatList.push_back(*iterP);
		if ((tag&TODUOPOTEN) && lineInfo[side][i].duoPotenList.size() > 0)//˫Ǳ����
			for (iterP = lineInfo[side][i].duoPotenList.begin(); iterP != lineInfo[side][i].duoPotenList.end(); iterP++)
				tempSynInfo.duoPotenList.push_back(*iterP);
		if ((tag&TOSOLPOTEN) && lineInfo[side][i].solPotenList.size() > 0)//��Ǳ����
			for (iterP = lineInfo[side][i].solPotenList.begin(); iterP != lineInfo[side][i].solPotenList.end(); iterP++)
				tempSynInfo.solPotenList.push_back(*iterP);
		if ((tag&TOCOMMON) && lineInfo[side][i].toDuoTwoList.size() > 0)//��ͨ��
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
 * CalSingleLineValue - �߹�ֵ������
 * @start:	�ߵ���ʼ������
 * @Direct:	�ߵķ����־
 * @side:	��������ߵļ�ֵ��Ϣ��������ִ����ɫ
 * @return:	�����ߵĹ�ֵ
 */
int CalSingleLineValue(const Point start, const BYTE Direc, const BYTE side) //���������Direc������ߵļ�ֵ
{
	int shapeIndex = 0;	// ���ͱ��
	int x = start.x;	// ����㿪ʼ��
	int y = start.y;
	int len = 0;				//  ��¼��ǰλ������Ҫ��Ķ��е�λ��, �������Ϊ��ǰ�γ�
	int value = 0;
	while (x<edge&&y<edge&&x>-1 && y>-1)	//	����������
	{
		if (virtualBoard[x][y] == EMPTY)
		{
			len++;
		}
		else if (virtualBoard[x][y] == side)		// ����Ǳ������ӡ�
		{
			shapeIndex += (1 << len);
			len++;
		}
		else		//  ����ǶԷ����ӡ�
		{
			if (len > 5)	//���ȳ���5���Ż����ӡ�
			{
				shapeIndex += (1 << len);	  // �õ���������֪ʶ���е�λ�á�
				value += LineTypeValue[preTable[shapeIndex]];
			}
			shapeIndex = 0;	// ƫ��������
			len = 0;		// ���ȹ���
		}
		Increment(x, y, Direc);
	}
	if (len > 5)	//���ȳ���5���Ż����ӡ�
	{
		shapeIndex += (1 << len);	  // �õ�������
		value += LineTypeValue[preTable[shapeIndex]];
	}
	return value;
}

/**
 * CalSingleLineType - �����ͼ�����
 * @start:	�ߵ���ʼ������
 * @Direct:	�ߵķ����־
 * @side:	��������ߵ�������Ϣ��������ִ����ɫ
 * @return:	�����ߵ����ͣ����Գɷ�
 */
int CalSingleLineType(const Point start, const BYTE Direc, const BYTE side) //���������Direc������ߵļ�ֵ
{
	int shapeIndex = 0;	// ���ͱ��
	int x = start.x;	// ����㿪ʼ��
	int y = start.y;
	int len = 0;				//  ��¼��ǰλ������Ҫ��Ķ��е�λ��, �������Ϊ��ǰ�γ�
	int type = 0;
	while (x<edge&&y<edge&&x>-1 && y>-1)	//	����������
	{
		if (virtualBoard[x][y] == EMPTY)
		{
			len++;
		}
		else if (virtualBoard[x][y] == side)		// ����Ǳ������ӡ�
		{
			shapeIndex += (1 << len);
			len++;
		}
		else		//  ����ǶԷ����ӡ�
		{
			if (len > 5)	//���ȳ���5���Ż����ӡ�
			{
				shapeIndex += (1 << len);	  // �õ���������֪ʶ���е�λ�á�
				type += LineTypeType[preTable[shapeIndex]];
			}
			shapeIndex = 0;	// ƫ��������
			len = 0;		// ���ȹ���
		}
		Increment(x, y, Direc);
	}
	if (len > 5)	//���ȳ���5���Ż����ӡ�
	{
		shapeIndex += (1 << len);	  // �õ�������
		type += LineTypeType[preTable[shapeIndex]];
	}
	return type;
}


/**
 * CalculateStepValue - �ŷ���ֵ������
 * @step:	��Ҫ��������ŷ�
 * @side:	�ŷ�������ִ����ɫ
 * @return:	�����ŷ���ֵ
 */
int CalculateStepValue(const Step step, const BYTE side)
{
	int key1, key2, i, unside = 1 - side;
	int myC = 0, denC = 0;//˫���仯�����ͼ�ֵ��
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
