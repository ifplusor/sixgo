#include "Seach.h"


//int LineTypeValue[WIN+1]={0, 0, 0,0,0,1, 3,4,5,9, 8,12,15,16,20,31,33, 39,45,47,54,89,100,1000, 37,76,1000, 10000 };//���ͼ�ֵ��
//                             0| 1| 2  3  4  5| 6  7  8   9| 10 *11  12  13 *14  15 *16| 17  18   19  *20   21  *22    *23| 24   25    *26|     27
int LineTypeValue[WIN + 1] = { 0, 0, 1, 1, 1, 3, 5, 6, 8, 11, 15, 18, 20, 24, 28, 31, 39, 75, 90, 110, 115, 186, 236, 99999, 94, 196, 99999, 100000 };//���ͼ�ֵ��
//�޼�ֵ��|0ǱǱ˫Ǳ����1Ǳ��Ǳ����2Ǳ��˫Ǳ����3Ǳ˫Ǳ����4Ǳ��Ǳ����5��Ǳ����
//         6��˫Ǳ����7˫Ǳ����8��Ǳ����9����в��10��˫��в��11˫��в|������в����ʤ
//             Ǳ��Ǳ��16->��Ǳ��100->����в1000->��ʤ
//ǱǱ˫Ǳ��3->Ǳ˫Ǳ��11->˫Ǳ��31->˫��в155->��ʤ10000
//            Ǳ��˫Ǳ��7->��˫Ǳ��19->��˫��в95
//             Ǳ��Ǳ��5->��Ǳ��15->����в75->��ʤ

int LineTypeThreat[WIN + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 10, 10, 100, 1, 10, 100, 1000 };//������в���ͱ�1����в��10˫��в��100����в��1000��ʤ
int LineTypeType[WIN + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 10, 10, 11, 11, 10, 100, 100, 1000, 1100, 1100, 1000, 2000, 2000, 3000, 1000, 2000, 3000, 10000 };//�������ͱ�

int virtualBoard[edge][edge];//��������
BoardCode boardCode;//���̱���
LineInfo lineInfo[2][92];//�ߵ���Ϣ��

int SearchDepth = DEPTH;//�涨�������
int MaxDepth = MAXDEPTH;//����������

/**
 * addHash - �����û���
 * @data:	����������
 */
inline void addHash(HashInfo &data)
{
	unsigned long hash = hashCode(data.code);//ֱ�ӵ�ַӳ��
	//�¼�����ʱ�����Ȼ����
	hashList[hash] = data;
}

/**
 * sixgo_carry - �����岩��������ģ����湤�����
 * @return��	�����������ó����ŷ�
 * @moveStep:	���������ŷ�
 * @side:		����ִ����ɫ
 */
Step sixgo_carry(const Step moveStep, const int nBoard[19][19], const BYTE side)
{
	int i, j;
	Point point;
	//������������
	initialCode(boardCode);
	for (i = 0; i < edge; i++)
		for (j = 0; j < edge; j++)
		{
			if (nBoard[i][j] != 2)
			{
				point.x = i; point.y = j;
				moveCodeP(boardCode, point, nBoard[i][j]);//���й�ϣ����
			}
			virtualBoard[i][j] = nBoard[i][j];
		}
	debugger.InitDebugger(HandNum, moveStep);
	//��������з�
	Step step = SeachValuableStep(side);
	debugger.BackMove(step);
	return step;
}

/**
 * SeachValuavleStep - ��������з��������������
 * @return:		����������ߵ�����ŷ�
 * @side:			����ִ����ɫ
 * @seachDepth:	��������Բ���������չ�����
 */
Step SeachValuableStep(BYTE side)
{
	Step step = { { -1, -1 }, { -1, -1 }, -WINLOSE };//��ʼ��Ϊ���ӣ���ֹ���ַ�������
	int myType, denType;
	LineInfo tempLine[2][4], tempLine2[2][4];
	vector<Step> stepList;
	vector<Step>::iterator iterS;
	int alpha = -WINLOSE, unside = 1 - side;
	HashInfo *hashP;

	//����ʷ��ϣ���в���
	hashP = findHash(boardCode);
	if (hashP != NULL && hashP->full == true)//���������ʷ,�Ҿ��������ŷ��б���ֱ�ӵ�����ʷ�߷���
	{
		if (hashP->cut == false)//cut�ֶ�Ϊfalse��֤��stepList��ÿһ���value���۱�׼һ�¡���ʱ���Զ�stepList�е��ŷ�����������������߼�֦Ч��
			sort(hashP->stepList.begin(), hashP->stepList.end(), cmpStepValue);
		stepList = hashP->stepList;
	}
	else
	{
		myType = GetBoardType(side);
		denType = GetBoardType(unside);
		if (myType > 0)//�������ܶԶԷ�������в�����ǿ�ʤ���棬���ɱ�ʤ�����㣩
			stepList = MakeStepListForWin(side, 1);
		else if (denType == 1)//�ܵ�����в
			stepList = MakeStepListForDefendSingle(side, 20);
		else if (denType > 1)//�ܵ�˫��в������в��α˫��в��α����в
			stepList = MakeStepListForDefendDouble(side, 18);
		else//��������в���Σ�������4�ͺ�5�ͣ�
			stepList = MakeStepListForNone(side, 23);
	}
	debugger.OutputStep(stepList, side);
	if (stepList.size() == 0)//���뷵��һ���ŷ�����������
	{
		printf("The engine don't create step!\n");
		bool first = true;
		for (int i = 0; i < edge; i++)
		{
			for (int j = 0; j < edge; j++)
			{
				if (virtualBoard[i][j] == EMPTY)
				{
					if (first)
					{
						step.first.x = i;
						step.first.y = j;
					}
					else
					{
						step.second.x = i;
						step.second.y = j;
						return step;
					}
				}
			}
		}
		debugger.OutSelect(step);
		return step;
	}
	else if (stepList.size() == 1)
	{
		return stepList[0];
	}
	else
	{
		for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
		{
			MakeMove(iterS->first, tempLine, side);
			MakeMove(iterS->second, tempLine2, side);
			moveCodeS(boardCode, *iterS, side);
#ifdef DEBUGVALUE
			debugger.MakeMove(*iterS);
#endif
			iterS->value = -nega_alpha_beta(unside, -WINLOSE, -alpha, 1);//������ֵ����alpha-beta��֦������ֵ
#ifdef DEBUGVALUE
			debugger.BackMove(*iterS);
#endif
			moveCodeS(boardCode, *iterS, side);
			BackMove(iterS->second, tempLine2, side);
			BackMove(iterS->first, tempLine, side);
			if (iterS->value > alpha)
			{
				alpha = iterS->value;
				step = *iterS;
				if (alpha >= WINLOSE - MaxDepth)//�ɻ�ʤ��ֱ�ӷ���
					break;
			}
		}
	}
	debugger.OutSelect(step);
	return step;
}

/**
 * nega-alpha-beta - ������ֵ������ͨ��alpha-beta��֦�����Ż���alpha-beta��֦��һ����С���ڵĹ��̣�����֦��״̬������ֵ�������ڷ�Χ
 * @return:	���������ó��ľ���Ǳ������ֵ�������ֵ��
 * @side:		ģ�����巽ִ����ɫ
 * @alpha:		ָ����֦����������
 * @beta:		ָ����֦����������
 * @depth:		��ǰ�������
 */
int nega_alpha_beta(BYTE side, int alpha, int beta, const int depth)
{
	int unside = 1 - side;
	LineInfo tempLine[2][4], tempLine2[2][4];
	vector<Step>::iterator iterS;
	HashInfo *hashT, hashP;//��ϣ�ڵ�
	Step select;

	if (findEndLib(boardCode))
		return WINLOSE - depth;

	hashT = findHash(boardCode);//��ȡ��ʷ
	if (hashT != NULL)//���������ʷ������ֱ�ӵ�����ʷ
	{
		//���������ʷ����ʱ����뵱ǰһ�£����ǳ��汾���������½��������ֱ�ӷ��ر��б���Ĳ���ֵ
		//ͬһ���״̬����չ���Ĳ�����������ͬ��������Ⱥ㶨��������ֵ���������Ͻ����ģ�alpha-beta������ǰ�̳У�beta�����ݼ���
		if (hashT->timestamp == HandNum)
			return hashT->value;
		else
			hashP = *hashT;
	}
	else//��������ʷ������������ϣ��
	{
		hashP.code = boardCode;//��ֵ����Ψһ��ʶ
		hashP.cut = true;//��ʼ��Ϊtrue������������֦
		hashP.full = false;//��ʼ��Ϊfalse�����������������ŷ��б�
		hashP.myType = GetBoardType(side);//��ȡ������в����
		hashP.denType = GetBoardType(unside);//��ȡ������в����
	}
	hashP.timestamp = HandNum;//���¹�ϣ����ʱ���

	//�ݹ����
	//�ڽ��������ı�Ҫ����Ϊ�Է�δȡʤ�����������������ּ�����ʤ�򷵻أ��ʵݹ����ֹ�����������Է���ʤ
	if (hashP.myType > 0)//������в�Է�������
	{
		InsertEndLib(boardCode);
		hashP.value = WINLOSE - depth;
		addHash(hashP);
		return hashP.value;
	}
	else if (depth >= SearchDepth)//�������������
	{
		if (HandNum <= 6)
		{
			//�ﵽ����������ֱ�ӷ��أ���������չ����
			hashP.value = GetBoardValue(side)*0.85 - GetBoardValue(unside);
			addHash(hashP);
			return hashP.value;
		}
		else
		{
			//��չ����
			hashP.value = ExtendSeach(side, alpha, beta, depth);
			if (hashP.value > MaxDepth - WINLOSE && hashP.value < WINLOSE - MaxDepth)
				hashP.value = hashP.value*0.4 + (GetBoardValue(side)*0.85 - GetBoardValue(unside))*0.6;
			addHash(hashP);
			return hashP.value;
		}
	}

	//����ȫ���ŷ�
	if (hashP.full == false)//����ϣ�����full�ֶ�ֵΪfalseʱ��֤���ñ����stepList�ֶ�ֵ�����������ܱ�ֱ�����á�
	{
		if (hashP.denType == 1)//�ܵ�����в
			hashP.stepList = MakeStepListForDefendSingle(side, 20);
		else if (hashP.denType > 1)//�ܵ�˫��в������в��α˫��в��α����в��˫��в�����������4���ŷ���Ϊ�˷�ֹα˫Ӧ�뵥��в����һ�£�
			hashP.stepList = MakeStepListForDefendDouble(side, 20);
		else//��������в���Σ�������4�ͺ�5�ͣ�
			hashP.stepList = MakeStepListForNone(side, 23);
		hashP.full = true;
	}
	else
	{
		if (hashP.cut == false)//cut�ֶ�Ϊfalse(δ��֦)��֤��stepList��ÿһ���value���۱�׼һ�¡���ʱ���Զ�stepList�е��ŷ�����������������߼�֦Ч��
		{
			sort(hashP.stepList.begin(), hashP.stepList.end(), cmpStepValue);
			hashP.cut = true;
		}
	}
	if (hashP.stepList.size() == 0)//��Ϊ�����ƽ���棬���������ŷ�
	{
#ifdef DEBUGVALUE
		if (hashP.denType == 2 || hashP.denType == 10)
		{
			debugger.OutputMessage("����˫����в����", side);
			printf("error: can't create step!\n");
		}
#endif
		hashP.value = depth - WINLOSE;
		addHash(hashP);
		return hashP.value;
	}

#ifdef DEBUGVALUE
	debugger.OutputStep(hashP.stepList, side);
#endif
	for (iterS = hashP.stepList.begin(); iterS != hashP.stepList.end(); iterS++)
	{
		moveCodeS(boardCode, *iterS, side);//����
		MakeMove(iterS->first, tempLine, side);
		MakeMove(iterS->second, tempLine2, side);
#ifdef DEBUGVALUE
		debugger.MakeMove(*iterS);
#endif
		iterS->value = -nega_alpha_beta(unside, -beta, -alpha, depth + 1);
#ifdef DEBUGVALUE
		debugger.BackMove(*iterS);
#endif
		BackMove(iterS->second, tempLine2, side);
		BackMove(iterS->first, tempLine, side);
		moveCodeS(boardCode, *iterS, side);//�ָ�
		if (alpha < iterS->value)
		{
			if (iterS->value >= WINLOSE - MaxDepth)//ֻҪ��һ���ŷ����Ի�ʤ������ʤ
			{
				InsertEndLib(boardCode);
				return iterS->value;
			}
			alpha = iterS->value;
			select = *iterS;
			if (beta <= alpha)//max����beta��֦
			{
				hashP.value = alpha;
				addHash(hashP);
				return hashP.value;
			}
		}
	}
	hashP.cut = false;//δ����֦
	hashP.value = alpha;
	addHash(hashP);
#ifdef DEBUGVALUE
	debugger.OutSelect(select);
#endif
	return hashP.value;
}

/**
 * ExtendSeach - ��չ������ֵ������ͨ��DTSS/STSS�����Ż�����ʹ���浽��һ��ƽ��״̬���й�ֵ
 * @return:	���������ó��ľ���Ǳ������ֵ�������ֵ��
 * @side:		ģ�����巽ִ����ɫ
 * @alpha:		ָ����֦����������
 * @beta:		ָ����֦����������
 * @depth:		��ǰ�������
 */
int ExtendSeach(BYTE side, int alpha, int beta, const int depth)
{
	int unside = 1 - side, val;
	LineInfo tempLine[2][4], tempLine2[2][4];
	vector<Step> stepList;
	vector<Step>::iterator iterS;
	int myType, denType;

	if (findEndLib(boardCode))
		return WINLOSE - depth;

	myType = GetBoardType(side);
	denType = GetBoardType(unside);

	//�ݹ����
	if (myType > 0)//������в�Է�������
	{
		InsertEndLib(boardCode);
		return WINLOSE - depth;
	}
	else if (depth >= MaxDepth)//�������������Ƚ��з���
		return GetBoardValue(side) - GetBoardValue(unside);

	//��չ�����в���Ҫ�����ŷ��б����䲻�����ԣ��ڳ��������н����ᱻ����
	if (denType == 1)//����ܵ�����в��������������в����
		stepList = MakeStepListForDefendSingleEx(side, 6);
	else if (denType > 1)//�ܵ�˫��в���ؽ������У�����˫����������һ���֣�˫��в�����������4���ŷ���Ϊ�˷�ֹα˫Ӧ�뵥��в����һ�£�
		stepList = MakeStepListForDefendDoubleEx(side, 6);
	else//����вʱѰ�ҷ��� ˫�������� �Ļ���
		stepList = MakeStepListForDouble(side, 8);

	if (stepList.size() == 0)
	{
		if (denType > 10 || (denType > 2 && denType < 10))//����в�����ƽ�
			return depth - WINLOSE;
		else//�����������������
			return GetBoardValue(side)*0.85 - GetBoardValue(unside);
	}

	for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
	{
		MakeMove(iterS->first, tempLine, side);
		MakeMove(iterS->second, tempLine2, side);
		val = -ExtendSeach(unside, -beta, -alpha, depth + 1);
		BackMove(iterS->second, tempLine2, side);
		BackMove(iterS->first, tempLine, side);
		if (alpha < val)
		{
			if (val >= WINLOSE - MaxDepth)//ֻҪ��һ���ŷ����Ի�ʤ������ʤ
			{
				InsertEndLib(boardCode);
				return val;
			}
			alpha = val;
			if (beta <= alpha)
				return alpha;
		}
	}
	return alpha;
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
 * UpdataBoard - ͨ���з��ֲ����������Ϣ�����ߵ���Ϣ
 * @return:		�޷���ֵ
 * @step:		�����ŷ�
 */
void UpdataBoard(Step step)
{
	UpdateLineForCross(step.first, BLACK);
	UpdateLineForCross(step.first, WHITE);
	UpdateLineForCross(step.second, BLACK);
	UpdateLineForCross(step.second, WHITE);
}

/**
 * UpdateLineForCross - ͨ��������ߵ���Ϣ
 * @return:		�޷���ֵ
 * @point:		���ӵ�
 * @side:		����������ִ����ɫ
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
 * AnalyzeLine - ͨ���ߵ���ʼ��ͷ��������
 * @return:		��������Ϣ
 * @start:		�ߵ���ʼ���������
 * @lineDirec:	�ߵķ����־
 * @side:		����������Ϣ������ִ����ɫ
 */
LineInfo AnalyzeLine(Point start, BYTE lineDirec, BYTE side, int tag)
{
	int shapeIndex = 0;	// ��������
	int x = start.x;	// ����㿪ʼ��
	int y = start.y;
	Point LinePos;		//�����������ε����������ꡣ��-1��ʾ��δȷ�������������ε���㣨���̵���Ч�������ֵ����С��0����
	int len = 0;		//��¼��ǰλ������Ҫ��Ķ��е�λ��, �������Ϊ��ǰ�γ�
	int value = 0;
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
 * ValueType - ����ָ������
 * @return:	��������Ϣ��������������в���͡���ֵ���������������㡢��в�㡢Ǳ���㡢ǱǱ����ľ������ꡣ
 * @style��	������������Analizeline������ȡ�����εĶ�����������
 * @start:		���͵���ʼ���������
 * @lineDirec:	���������ߵķ���
 */
LineInfo ValuateType(int style, Point start, BYTE lineDirec, int tag)
{
	int i, len;	//len:���͵ĳ��ȡ�
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

	for (i = 19; i >= 0; i--)//����style����Ч������λ���������ͳ��ȡ�����Ϊ6λ��������AnalyzeLine���������ġ�
		if (GetABit(style, i))
		{
			len = i; break;
		}

	if (ShapeStyleId >= THREAT_four_SINGLE)//��в��
	{
		if (tag&TODEFENT)
		{
			if (linetypeInfo[style].defStepList.size() != 0)//������
				for (iterS = linetypeInfo[style].defStepList.begin(); iterS != linetypeInfo[style].defStepList.end(); iterS++)
				{
					tempStep.first.x = start.x + iterS->first*lineVector[lineDirec][0];
					tempStep.first.y = start.y + iterS->first*lineVector[lineDirec][1];
					tempStep.second.x = start.x + iterS->second*lineVector[lineDirec][0];
					tempStep.second.y = start.y + iterS->second*lineVector[lineDirec][1];
					lineInfo.defStepList.push_back(tempStep);
				}
			if (linetypeInfo[style].defPointList.size() != 0)//������
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
	if ((tag&TODUOTHREAT) && linetypeInfo[style].duoThreatList.size() != 0)
		for (iterP = linetypeInfo[style].duoThreatList.begin(); iterP != linetypeInfo[style].duoThreatList.end(); iterP++)
		{
			tempPoint.x = start.x + (*iterP)*lineVector[lineDirec][0];
			tempPoint.y = start.y + (*iterP)*lineVector[lineDirec][1];
			lineInfo.duoThreatList.push_back(tempPoint);
		}
	if ((tag&TOSOLTHREAT) && linetypeInfo[style].solThreatList.size() != 0)
		for (iterP = linetypeInfo[style].solThreatList.begin(); iterP != linetypeInfo[style].solThreatList.end(); iterP++)
		{
			tempPoint.x = start.x + (*iterP)*lineVector[lineDirec][0];
			tempPoint.y = start.y + (*iterP)*lineVector[lineDirec][1];
			lineInfo.solThreatList.push_back(tempPoint);
		}
	if ((tag&TODUOPOTEN) && linetypeInfo[style].duoPotenList.size() != 0)
		for (iterP = linetypeInfo[style].duoPotenList.begin(); iterP != linetypeInfo[style].duoPotenList.end(); iterP++)
		{
			tempPoint.x = start.x + (*iterP)*lineVector[lineDirec][0];
			tempPoint.y = start.y + (*iterP)*lineVector[lineDirec][1];
			lineInfo.duoPotenList.push_back(tempPoint);
		}
	if ((tag&TOSOLPOTEN) && linetypeInfo[style].solPotenList.size() != 0)
		for (iterP = linetypeInfo[style].solPotenList.begin(); iterP != linetypeInfo[style].solPotenList.end(); iterP++)
		{
			tempPoint.x = start.x + (*iterP)*lineVector[lineDirec][0];
			tempPoint.y = start.y + (*iterP)*lineVector[lineDirec][1];
			lineInfo.solPotenList.push_back(tempPoint);
		}
	if ((tag&TOCOMMON) && linetypeInfo[style].toDuoTwoList.size() != 0)
		for (iterP = linetypeInfo[style].toDuoTwoList.begin(); iterP != linetypeInfo[style].toDuoTwoList.end(); iterP++)
		{
			tempPoint.x = start.x + (*iterP)*lineVector[lineDirec][0];
			tempPoint.y = start.y + (*iterP)*lineVector[lineDirec][1];
			lineInfo.toDuoTwoList.push_back(tempPoint);
		}
	return lineInfo;
}


/**
 * GetBoardType - �ռ������в������Ϣ
 * @side	Ӧ�ռ���Ϣ�����ַ���־
 */
int GetBoardType(BYTE side)
{
	int i, type = 0;
	for (i = 0; i < 92; i++)//����92����
		type += lineInfo[side][i].LineType;//�ۼ��ߵ���в������Ϊ�������в����
	return type;
}

/**
 * GetBoardValue - �ռ������в������Ϣ
 * @side	Ӧ�ռ���Ϣ�����ַ���־
 */
int GetBoardValue(BYTE side)
{
	int i, value = 0;
	for (i = 0; i < 92; i++)//����92����
		value += lineInfo[side][i].value;//�ۼ��ߵļ�ֵ��Ϊ�����ֵ
	return value;
}

/**
 * GetBoardInfo - �ռ������Ϣ
 * @side	Ӧ�ռ���Ϣ�����ַ���־
 * @tag	�ռ���Ϣ���԰�λ��־����1��־����ּ�ֵ����в����ΪĬ���ռ���Ϣ
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
			if (lineInfo[side][i].defPointList.size() != 0)
				for (iterP = lineInfo[side][i].defPointList.begin(); iterP != lineInfo[side][i].defPointList.end(); iterP++)
					tempSynInfo.defPointList.push_back(*iterP);
			if (lineInfo[side][i].defStepList.size() != 0)
				for (iterS = lineInfo[side][i].defStepList.begin(); iterS != lineInfo[side][i].defStepList.end(); iterS++)
					tempSynInfo.defStepList.push_back(*iterS);
		}
		if ((tag&TOWIN) && lineInfo[side][i].winList.size() != 0)//��ʤ��
			tempSynInfo.winList.push_back(lineInfo[side][i].winList[0]);
		if ((tag&TOWILLWIN) && lineInfo[side][i].willWinList.size() != 0)//����ʤ����
			tempSynInfo.willWinList.push_back(lineInfo[side][i].willWinList[0]);
		if ((tag&TODUOTHREAT) && lineInfo[side][i].duoThreatList.size() != 0)//˫��в��
			for (iterP = lineInfo[side][i].duoThreatList.begin(); iterP != lineInfo[side][i].duoThreatList.end(); iterP++)
				tempSynInfo.duoThreatList.push_back(*iterP);
		if ((tag&TOSOLTHREAT) && lineInfo[side][i].solThreatList.size() != 0)//����в��
			for (iterP = lineInfo[side][i].solThreatList.begin(); iterP != lineInfo[side][i].solThreatList.end(); iterP++)
				tempSynInfo.solThreatList.push_back(*iterP);
		if ((tag&TODUOPOTEN) && lineInfo[side][i].duoPotenList.size() != 0)//˫Ǳ����
			for (iterP = lineInfo[side][i].duoPotenList.begin(); iterP != lineInfo[side][i].duoPotenList.end(); iterP++)
				tempSynInfo.duoPotenList.push_back(*iterP);
		if ((tag&TOSOLPOTEN) && lineInfo[side][i].solPotenList.size() != 0)//��Ǳ����
			for (iterP = lineInfo[side][i].solPotenList.begin(); iterP != lineInfo[side][i].solPotenList.end(); iterP++)
				tempSynInfo.solPotenList.push_back(*iterP);
		if ((tag&TOCOMMON) && lineInfo[side][i].toDuoTwoList.size() != 0)//��ͨ��
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


//�з�������
//=====================================================

/**
 * MakeStepListForWin - ��ʤ�з�������
 * @return:	�����ŷ��б�
 * @myInfo:	ִ�巽��ǰ�����ۺ���Ϣ
 * @denInfo:	�Է���ǰ�����ۺ���Ϣ
 * @limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForWin(int side, unsigned int limit)
{
	SynInfo myInfo = GetBoardInfo(side, TOWIN | TOWILLWIN);
	LineInfo tempLine[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Point>::iterator iterP, iterP2;
	vector<Step> stepList;//�з��б�

	//���������ʤ�㣬��ֱ�ӷ��ص�һ����ʤ����Ϊ�з�
	//����ѡȡ��һ��������ʤ��������£������������º�ĵ�һ����ʤ������鲽
	if (myInfo.winList.size() == 0)
	{
		tempStep.second = myInfo.willWinList[0];
		MakeMove(tempStep.second, tempLine, side, TOWIN);//��Ϊ��ʤ����ֻ����ʤ������ռ�
		myInfo = GetBoardInfo(side, TOWIN);
		BackMove(tempStep.second, tempLine, side);
	}
	else
	{
		tempStep.second.x = tempStep.second.y = -1;
	}
	tempStep.first = myInfo.winList[0];
	tempStep.value = WINLOSE;
	stepList.push_back(tempStep);
	return stepList;
}

/**
 * MakeStepListForDefendSingle - ��������в�з�������
 * @return:	�����ŷ��б�
 * @myInfo:	ִ�巽��ǰ�����ۺ���Ϣ
 * @denInfo:	�Է���ǰ�����ۺ���Ϣ
 * @limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForDefendSingle(int side, unsigned int limit)
{
	int unside = 1 - side;
	SynInfo denInfo = GetBoardInfo(unside, TODEFENT);
	SynInfo tempMy, tempDen;//��ʱ������Ϣ����
	LineInfo tempLine[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Step> stepList;//�з��б�
	vector<Point> pointList;//���б�
	vector<Point>::iterator iterP, iterP2;
	vector<Step>::iterator iterS;

	//ѡȡһ���ƽ��������£�Ȼ��������º�ľ�������鲽��һ������в����������������ƽ��
	for (iterP = denInfo.defPointList.begin(); iterP != denInfo.defPointList.end(); iterP++)
	{
		tempStep.first = *iterP;
		MakeMove(tempStep.first, tempLine, side, FORNONTHREAT);//���º��ռ�ȫ����Ϣ
		tempMy = GetBoardInfo(side, FORNONTHREAT);//�������в����ռ�������Ϣ
		tempDen = GetBoardInfo(unside, FORPOTEN);//�ռ����ֵ�Ǳ����Ϣ���ޣ�
		//������<˫��в�㣬����в�㣬˫Ǳ����>�ͶԷ�<˫��в�㣬����в�㣬˫Ǳ����>��Ϊ��ѡ��
		if (tempMy.duoThreatList.size() != 0)
			for (iterP2 = tempMy.duoThreatList.begin(); iterP2 != tempMy.duoThreatList.end(); iterP2++)
				pointList.push_back(*iterP2);
		if (tempMy.solThreatList.size() != 0)
			for (iterP2 = tempMy.solThreatList.begin(); iterP2 != tempMy.solThreatList.end(); iterP2++)
				pointList.push_back(*iterP2);
		if (tempMy.duoPotenList.size() != 0)
			for (iterP2 = tempMy.duoPotenList.begin(); iterP2 != tempMy.duoPotenList.end(); iterP2++)
				pointList.push_back(*iterP2);
		if (tempDen.duoThreatList.size() != 0)
			for (iterP2 = tempDen.duoThreatList.begin(); iterP2 != tempDen.duoThreatList.end(); iterP2++)
				pointList.push_back(*iterP2);
		if (tempDen.solThreatList.size() != 0)
			for (iterP2 = tempDen.solThreatList.begin(); iterP2 != tempDen.solThreatList.end(); iterP2++)
				pointList.push_back(*iterP2);
		if (tempDen.duoPotenList.size() != 0)
			for (iterP2 = tempDen.duoPotenList.begin(); iterP2 != tempDen.duoPotenList.end(); iterP2++)
				pointList.push_back(*iterP2);
		UniquePoint(pointList);//ȥ�ز���
		if (pointList.size() < 10)//�����ѡ������10���������ñ�����Ǳ���㣬Ǳ˫Ǳ����ͶԷ���Ǳ����
		{
			for (iterP2 = tempMy.solPotenList.begin(); iterP2 != tempMy.solPotenList.end(); iterP2++)
				pointList.push_back(*iterP2);
			for (iterP2 = tempMy.toDuoTwoList.begin(); iterP2 != tempMy.toDuoTwoList.end(); iterP2++)
				pointList.push_back(*iterP2);
			for (iterP2 = tempDen.solPotenList.begin(); iterP2 != tempDen.solPotenList.end(); iterP2++)
				pointList.push_back(*iterP2);
			UniquePoint(pointList);
			if (pointList.size() == 0)
			{
				for (iterP2 = tempDen.toDuoTwoList.begin(); iterP2 != tempDen.toDuoTwoList.end(); iterP2++)
					pointList.push_back(*iterP2);
				UniquePoint(pointList);
			}
		}
		BackMove(tempStep.first, tempLine, side);
		for (iterP2 = pointList.begin(); iterP2 != pointList.end(); iterP2++)
		{
			tempStep.second = *iterP2;
			tempStep.value = CalculateStepValue(tempStep, side);
			stepList.push_back(tempStep);
		}
		pointList.resize(0);
	}
	for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)//������THREAT_four_ADDITION����Ķ����ƽⲽ
	{
		iterS->value = CalculateStepValue(*iterS, side);
		stepList.push_back(*iterS);
	}
	if (stepList.size() == 0)
	{
		printf("�쳣: On make step list for Defend solthreat!\n");
	}
	else
	{
		UniqueStep(stepList);
		sort(stepList.begin(), stepList.end(), cmpStepValue);

		unsigned int size = stepList.size();
		if (size > limit)
			stepList.resize(limit);
	}
	return stepList;
}

/**
 * MakeStepListForDefendDouble - ����˫��в���з�������
 * @return:	�����ŷ��б�
 * @myInfo:	ִ�巽��ǰ�����ۺ���Ϣ
 * @denInfo:	�Է���ǰ�����ۺ���Ϣ
 * @limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForDefendDouble(int side, unsigned int limit)
{
	int unside = 1 - side;
	SynInfo denInfo = GetBoardInfo(unside, TODEFENT);
	SynInfo tempMy, tempDen;//��ʱ������Ϣ����
	LineInfo tempLine[2][4], tempLine2[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Step> stepList;//�з��б�
	vector<Point> pointList;//���б�
	vector<Point>::iterator iterP, iterP2;
	vector<Step>::iterator iterS;
	int denType = GetBoardType(unside);

	if (denType >= 20)//�Է����ڶ���в���ͻ���ʤ���ͣ��������������˫��в���ͣ������ذ�ֱ�ӷ��ؿ��з��б�
	{
		stepList.resize(0);
		return stepList;
	}
	else if (denType == 10)//�˾���ֻ����һ��˫��в���ͣ���ֻ���ƽⲽ��Ϊ��ѡ�з������з��б�
	{
		for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)
			stepList.push_back(*iterS);
		UniqueStep(stepList);
		for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
			iterS->value = CalculateStepValue(*iterS, side);
		sort(stepList.begin(), stepList.end(), cmpStepValue);
		return stepList;
	}
	else//���е���в���͵���֣��������в�����ܴ���α˫����һ��˫��в�ӵ���в�����ܴ���α�ࣩ
	{
		if (denType > 10 && denType < 20)//����һ��˫��в���ͣ�ͬʱ���ڵ���в���ͣ����ܴ���α����в��
		{
			for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)//��˫��в����Ϊ��Ҫ�ɷ֣������ƽ��ʹ��˫��в���͵��ƽⲽ
			{
				MakeMove(iterS->first, tempLine, side, 0);//�鿴�Ƿ�����ƽ���в��ֻҪ���ߵ����ͣ��ʲ����ռ����µĵ���Ϣ
				MakeMove(iterS->second, tempLine2, side, 0);
				if (GetBoardType(unside) == 0)//�ж��Ƿ��ƽ�ɹ���������ΪTHREAT_four_ADDITION�Ķ����ƽⷨ����˫��в
					stepList.push_back(*iterS);
				BackMove(iterS->second, tempLine2, side);
				BackMove(iterS->first, tempLine, side);
			}
			if (stepList.size() != 0)//���ƽ�
			{
				UniqueStep(stepList);
				for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
					iterS->value = CalculateStepValue(*iterS, side);
				sort(stepList.begin(), stepList.end(), cmpStepValue);
			}
			return stepList;//���������ƽ���棬�������ƽⷵ�ؿ��з��б�
		}
		else//ֻ���ڶ������в���ͣ����ܴ���α˫��
		{
			for (iterP = denInfo.defPointList.begin(); iterP != denInfo.defPointList.end(); iterP++)
			{
				tempStep.first = *iterP;
				MakeMove(tempStep.first, tempLine, side, FORNONTHREAT + TODEFENT);
				tempDen = GetBoardInfo(unside, FORPOTEN + TODEFENT);//һ�����ߺ���ж��η����������ɱ������α˫��в���з��ĵڶ��ӳ��ֵ����ƽ�״̬���Ӷ�ʹ��һ������
				if (GetBoardType(unside) == 0)//һ���ƽ⣬α˫��в��������в�������ɣ������޽��������߼��������ڿɵ����ƽ������������в
				{
					tempMy = GetBoardInfo(side, FORNONTHREAT);
					//������<˫��в�㣬����в�㣬˫Ǳ����>�ͶԷ�<����в�㣬˫��в�㣬����в�㣬˫Ǳ����>��Ϊ��ѡ��
					for (iterP2 = tempMy.duoThreatList.begin(); iterP2 != tempMy.duoThreatList.end(); iterP2++)
						pointList.push_back(*iterP2);
					for (iterP2 = tempMy.solThreatList.begin(); iterP2 != tempMy.solThreatList.end(); iterP2++)
						pointList.push_back(*iterP2);
					for (iterP2 = tempMy.duoPotenList.begin(); iterP2 != tempMy.duoPotenList.end(); iterP2++)
						pointList.push_back(*iterP2);
					for (iterP2 = tempDen.duoThreatList.begin(); iterP2 != tempDen.duoThreatList.end(); iterP2++)
						pointList.push_back(*iterP2);
					for (iterP2 = tempDen.solThreatList.begin(); iterP2 != tempDen.solThreatList.end(); iterP2++)
						pointList.push_back(*iterP2);
					for (iterP2 = tempDen.duoPotenList.begin(); iterP2 != tempDen.duoPotenList.end(); iterP2++)
						pointList.push_back(*iterP2);
					UniquePoint(pointList);
					if (pointList.size() < 10)
					{
						for (iterP2 = tempMy.solPotenList.begin(); iterP2 != tempMy.solPotenList.end(); iterP2++)
							pointList.push_back(*iterP2);
						for (iterP2 = tempMy.toDuoTwoList.begin(); iterP2 != tempMy.toDuoTwoList.end(); iterP2++)
							pointList.push_back(*iterP2);
						for (iterP2 = tempDen.solPotenList.begin(); iterP2 != tempDen.solPotenList.end(); iterP2++)
							pointList.push_back(*iterP2);
						UniquePoint(pointList);
					}
					for (iterP2 = pointList.begin(); iterP2 != pointList.end(); iterP2++)
					{
						tempStep.second = *iterP2;
						stepList.push_back(tempStep);
					}
					pointList.resize(0);
				}
				else//�����ƽ⣬����α˫��в
				{
					for (iterP2 = tempDen.defPointList.begin(); iterP2 != tempDen.defPointList.end(); iterP2++)
					{
						MakeMove(*iterP2, tempLine2, side, 0);
						if (GetBoardType(unside) == 0)//�ж��Ƿ��ƽ�ɹ�
						{
							tempStep.second = *iterP2;
							stepList.push_back(tempStep);
						}
						BackMove(*iterP2, tempLine2, side);
					}
				}
				BackMove(tempStep.first, tempLine, side);
			}
			if (stepList.size() != 0)//���ƽ�
			{
				UniqueStep(stepList);
				for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
					iterS->value = CalculateStepValue(*iterS, side);
				sort(stepList.begin(), stepList.end(), cmpStepValue);
				if (stepList.size() > limit)
					stepList.resize(limit);
			}
			return stepList;//���������ƽ���棬�������ƽⷵ�ؿ��з��б�
		}
	}
}

/**
 * MakeStepListForNone - ƽ���ŷ�������
 * @return:		�����ŷ��б�
 * @myInfo:		ִ�巽��ǰ�����ۺ���Ϣ
 * @denInfo:	�Է���ǰ�����ۺ���Ϣ
 * @limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForNone(int side, unsigned int limit)
{
	int unside = 1 - side;
	SynInfo myInfo = GetBoardInfo(side, FORNONTHREAT);
	SynInfo denInfo = GetBoardInfo(unside, FORNONTHREAT);
	SynInfo tempMy, tempDen;//��ʱ������Ϣ����
	LineInfo tempLine[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Step> stepList;//�з��б�
	vector<Point> pointList, pointList2;//���б�
	vector<Point>::iterator iterP, iterP2;
	vector<Step>::iterator iterS;

	//��ѡȡ������<˫��в�㣬˫Ǳ���㣬����в�㣬��Ǳ����>���Է�<˫��в�㣬˫Ǳ���㣬����в��> ��Ϊ��һ�����������
	if (myInfo.duoThreatList.size() > 0)
		for (iterP = myInfo.duoThreatList.begin(); iterP != myInfo.duoThreatList.end(); iterP++)
			pointList.push_back(*iterP);
	if (myInfo.duoPotenList.size() > 0)
		for (iterP = myInfo.duoPotenList.begin(); iterP != myInfo.duoPotenList.end(); iterP++)
			pointList.push_back(*iterP);
	if (myInfo.solThreatList.size() > 0)
		for (iterP = myInfo.solThreatList.begin(); iterP != myInfo.solThreatList.end(); iterP++)
			pointList.push_back(*iterP);
	if (myInfo.solPotenList.size() > 0)
		for (iterP = myInfo.solPotenList.begin(); iterP != myInfo.solPotenList.end(); iterP++)
			pointList.push_back(*iterP);
	if (denInfo.duoThreatList.size() > 0)
		for (iterP = denInfo.duoThreatList.begin(); iterP != denInfo.duoThreatList.end(); iterP++)
			pointList.push_back(*iterP);
	if (denInfo.duoPotenList.size() > 0)
		for (iterP = denInfo.duoPotenList.begin(); iterP != denInfo.duoPotenList.end(); iterP++)
			pointList.push_back(*iterP);
	if (denInfo.solThreatList.size() > 0)
		for (iterP = denInfo.solThreatList.begin(); iterP != denInfo.solThreatList.end(); iterP++)
			pointList.push_back(*iterP);
	UniquePoint(pointList);
	if (pointList.size() < 5)//����һ��������5�������ȼ��뱾��Ǳ˫Ǳ����
	{
		if (myInfo.toDuoTwoList.size() > 0)
		{
			for (iterP = myInfo.toDuoTwoList.begin(); iterP != myInfo.toDuoTwoList.end(); iterP++)
				pointList.push_back(*iterP);
		}
		else if (pointList.size() == 0)//�ټ���Է���Ǳ�����Ǳ˫Ǳ����
		{
			if (denInfo.solPotenList.size() > 0)
				for (iterP = denInfo.solPotenList.begin(); iterP != denInfo.solPotenList.end(); iterP++)
					pointList.push_back(*iterP);
			if (denInfo.toDuoTwoList.size() > 0)
				for (iterP = denInfo.toDuoTwoList.begin(); iterP != denInfo.toDuoTwoList.end(); iterP++)
					pointList.push_back(*iterP);
		}
		UniquePoint(pointList);
	}
	for (iterP = pointList.begin(); iterP != pointList.end(); iterP++)
	{
		tempStep.first = *iterP;
		MakeMove(tempStep.first, tempLine, side, FORNONTHREAT);
		tempMy = GetBoardInfo(side, FORNONTHREAT);
		tempDen = GetBoardInfo(unside, FORPOTEN);
		//�ѱ�����<˫��в�㣬����в�㣬˫Ǳ����>���Է���<˫��в�㣬˫Ǳ���㣬����в��>����ڶ�����
		if (tempMy.duoThreatList.size() > 0)
			for (iterP2 = tempMy.duoThreatList.begin(); iterP2 != tempMy.duoThreatList.end(); iterP2++)
				pointList2.push_back(*iterP2);
		if (tempMy.duoPotenList.size() > 0)
			for (iterP2 = tempMy.duoPotenList.begin(); iterP2 != tempMy.duoPotenList.end(); iterP2++)
				pointList2.push_back(*iterP2);
		if (tempMy.solThreatList.size() > 0)
			for (iterP2 = tempMy.solThreatList.begin(); iterP2 != tempMy.solThreatList.end(); iterP2++)
				pointList2.push_back(*iterP2);
		if (tempDen.duoThreatList.size() > 0)
			for (iterP2 = tempDen.duoThreatList.begin(); iterP2 != tempDen.duoThreatList.end(); iterP2++)
				pointList2.push_back(*iterP2);
		if (tempDen.duoPotenList.size() > 0)
			for (iterP2 = tempDen.duoPotenList.begin(); iterP2 != tempDen.duoPotenList.end(); iterP2++)
				pointList2.push_back(*iterP2);
		if (tempDen.solThreatList.size() > 0)
			for (iterP2 = tempDen.solThreatList.begin(); iterP2 != tempDen.solThreatList.end(); iterP2++)
				pointList2.push_back(*iterP2);
		UniquePoint(pointList2);
		//���û�����ϵڶ����㣬�����ñ����ĵ�Ǳ���㣬Ǳ˫Ǳ����ͶԷ��ĵ�Ǳ����
		if (pointList2.size() < 5)
		{
			if (tempMy.solPotenList.size() > 0)
				for (iterP2 = tempMy.solPotenList.begin(); iterP2 != tempMy.solPotenList.end(); iterP2++)
					pointList2.push_back(*iterP2);
			if (tempMy.toDuoTwoList.size() > 0)
				for (iterP2 = tempMy.toDuoTwoList.begin(); iterP2 != tempMy.toDuoTwoList.end(); iterP2++)
					pointList2.push_back(*iterP2);
			if (tempDen.solPotenList.size() > 0)
				for (iterP2 = tempDen.solPotenList.begin(); iterP2 != tempDen.solPotenList.end(); iterP2++)
					pointList2.push_back(*iterP2);
			UniquePoint(pointList2);
		}
		BackMove(tempStep.first, tempLine, side);
		for (iterP2 = pointList2.begin(); iterP2 != pointList2.end(); iterP2++)
		{
			tempStep.second = *iterP2;
			tempStep.value = CalculateStepValue(tempStep, side);
			stepList.push_back(tempStep);
		}
		pointList2.resize(0);
	}
	if (stepList.size() == 0)//δ�ܻ�ȡ���Ϻõĵ�
	{
		bool first = true;
		for (int i = 0; i < edge; i++)
			for (int j = 0; j < edge; j++)
				if (virtualBoard[i][j] == EMPTY)
					if (first)
					{
						tempStep.first.x = i;
						tempStep.first.y = j;
					}
					else
					{
						tempStep.second.x = i;
						tempStep.second.y = j;
						stepList.push_back(tempStep);
						return stepList;
					}
	}
	else
	{
		UniqueStep(stepList);
		sort(stepList.begin(), stepList.end(), cmpStepValue);
		unsigned int size = stepList.size();
		if (size > limit)
		{
			unsigned int i;
			for (i = limit; i < size; i++)
				if (stepList[i].value != stepList[limit - 1].value)
					break;
			limit = i;
			stepList.resize(limit);
		}
	}
	return stepList;
}

/**
 * MakeStepListForDefendSingleEx - ��������в�ŷ���չ��������������չ����
 * @return:		�����ŷ��б�
 * @myInfo:		ִ�巽��ǰ�����ۺ���Ϣ
 * @denInfo:	�Է���ǰ�����ۺ���Ϣ
 * @limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForDefendSingleEx(int side, unsigned int limit)
{
	int unside = 1 - side;
	SynInfo denInfo = GetBoardInfo(unside, TODEFENT);
	SynInfo tempMy;//��ʱ������Ϣ����
	LineInfo tempLine[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Step> stepList;//�з��б�
	vector<Point> pointList;//���б�
	vector<Point>::iterator iterP, iterP2;
	vector<Step>::iterator iterS;

	//ѡȡһ���ƽ��������£�Ȼ��������º�ľ�������鲽��һ������в����������������ƽ��
	for (iterP = denInfo.defPointList.begin(); iterP != denInfo.defPointList.end(); iterP++)
	{
		tempStep.first = *iterP;
		MakeMove(tempStep.first, tempLine, side, TODUOTHREAT + TOSOLTHREAT);//���º��ռ�ȫ����Ϣ
		tempMy = GetBoardInfo(side, TODUOTHREAT + TOSOLTHREAT);//�������в����ռ�������Ϣ
		//������<˫��в�㣬����в��>��Ϊ��ѡ��
		for (iterP2 = tempMy.duoThreatList.begin(); iterP2 != tempMy.duoThreatList.end(); iterP2++)
			pointList.push_back(*iterP2);
		for (iterP2 = tempMy.solThreatList.begin(); iterP2 != tempMy.solThreatList.end(); iterP2++)
			pointList.push_back(*iterP2);
		//		UniquePoint(pointList);//ȥ�ز���
		BackMove(tempStep.first, tempLine, side);
		for (iterP2 = pointList.begin(); iterP2 != pointList.end(); iterP2++)
		{
			tempStep.second = *iterP2;
			tempStep.value = CalculateStepValue(tempStep, side);
			stepList.push_back(tempStep);
		}
		pointList.resize(0);
	}
	for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)//������THREAT_four_ADDITION����Ķ����ƽⲽ
		stepList.push_back(*iterS);
	if (stepList.size() != 0)
	{
		UniqueStep(stepList);
		sort(stepList.begin(), stepList.end(), cmpStepValue);

		unsigned int size = stepList.size();
		if (size > limit)
			stepList.resize(limit);
	}
	return stepList;
}

/**
 * MakeStepListForDefendDoubleEx - ����˫��в���ŷ���չ��������������չ����
 * @return:	�����ŷ��б�
 * @myInfo:	ִ�巽��ǰ�����ۺ���Ϣ
 * @denInfo:	�Է���ǰ�����ۺ���Ϣ
 * @limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForDefendDoubleEx(int side, unsigned int limit)
{
	int unside = 1 - side;
	SynInfo denInfo = GetBoardInfo(unside, TODEFENT);
	SynInfo tempMy, tempDen;//��ʱ������Ϣ����
	LineInfo tempLine[2][4], tempLine2[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Step> stepList;//�з��б�
	vector<Point> pointList;//���б�
	vector<Point>::iterator iterP, iterP2;
	vector<Step>::iterator iterS;
	int denType = GetBoardType(unside);

	if (denType >= 20)//�Է����ڶ���в���ͻ���ʤ���ͣ��������������˫��в���ͣ������ذ�ֱ�ӷ��ؿ��з��б�
	{
		stepList.resize(0);
		return stepList;
	}
	else if (denType == 10)//�˾���ֻ����һ��˫��в���ͣ���ֻ���ƽⲽ��Ϊ��ѡ�з������з��б�
	{
		for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)
			stepList.push_back(*iterS);
		//UniqueStep(stepList);
		for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
			iterS->value = CalculateStepValue(*iterS, side);
		sort(stepList.begin(), stepList.end(), cmpStepValue);
		return stepList;
	}
	else//���е���в���͵���֣��������в�����ܴ���α˫����һ��˫��в�ӵ���в�����ܴ���α�ࣩ
	{
		if (denType > 10 && denType < 20)//����һ��˫��в���ͣ�ͬʱ���ڵ���в���ͣ����ܴ���α����в��
		{
			for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)//��˫��в����Ϊ��Ҫ�ɷ֣������ƽ��ʹ��˫��в���͵��ƽⲽ
			{
				MakeMove(iterS->first, tempLine, side, 0);//�鿴�Ƿ�����ƽ���в��ֻҪ���ߵ����ͣ��ʲ����ռ����µĵ���Ϣ
				MakeMove(iterS->second, tempLine2, side, 0);
				if (GetBoardType(unside) == 0)//�ж��Ƿ��ƽ�ɹ���������ΪTHREAT_four_ADDITION�Ķ����ƽⷨ����˫��в
					stepList.push_back(*iterS);
				BackMove(iterS->second, tempLine2, side);
				BackMove(iterS->first, tempLine, side);
			}
			if (stepList.size() != 0)//���ƽ�
			{
				UniqueStep(stepList);
				for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
					iterS->value = CalculateStepValue(*iterS, side);
				sort(stepList.begin(), stepList.end(), cmpStepValue);
			}
			return stepList;//���������ƽ���棬�������ƽⷵ�ؿ��з��б�
		}
		else//ֻ���ڶ������в���ͣ����ܴ���α˫��
		{
			for (iterP = denInfo.defPointList.begin(); iterP != denInfo.defPointList.end(); iterP++)
			{
				tempStep.first = *iterP;
				MakeMove(tempStep.first, tempLine, side, TODUOTHREAT + TOSOLTHREAT);
				//һ�����ߺ���ж��η����������ɱ������α˫��в���з��ĵڶ��ӳ��ֵ����ƽ�״̬���Ӷ�ʹ��һ������
				if (GetBoardType(unside) == 0)//һ���ƽ⣬α˫��в��������в�������ɣ������޽��������߼��������ڿɵ����ƽ������������в
				{
					tempMy = GetBoardInfo(side, TODUOTHREAT + TOSOLTHREAT);
					//������<˫��в�㣬����в��>
					for (iterP2 = tempMy.duoThreatList.begin(); iterP2 != tempMy.duoThreatList.end(); iterP2++)
						pointList.push_back(*iterP2);
					for (iterP2 = tempMy.solThreatList.begin(); iterP2 != tempMy.solThreatList.end(); iterP2++)
						pointList.push_back(*iterP2);
					//					UniquePoint(pointList);
					for (iterP2 = pointList.begin(); iterP2 != pointList.end(); iterP2++)
					{
						tempStep.second = *iterP2;
						stepList.push_back(tempStep);
					}
					pointList.resize(0);
				}
				else//�����ƽ⣬����α˫��в
				{
					tempDen = GetBoardInfo(unside, 2);
					for (iterP2 = tempDen.defPointList.begin(); iterP2 != tempDen.defPointList.end(); iterP2++)
					{
						MakeMove(*iterP2, tempLine2, side, 0);
						if (GetBoardType(unside) == 0)//�ж��Ƿ��ƽ�ɹ�
						{
							tempStep.second = *iterP2;
							stepList.push_back(tempStep);
						}
						BackMove(*iterP2, tempLine2, side);
					}
				}
				BackMove(tempStep.first, tempLine, side);
			}
			if (stepList.size() != 0)//���ƽ�
			{
				UniqueStep(stepList);
				for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
					iterS->value = CalculateStepValue(*iterS, side);
				sort(stepList.begin(), stepList.end(), cmpStepValue);
				if (stepList.size() > limit)
					stepList.resize(limit);
			}
			return stepList;//���������ƽ���棬�������ƽⷵ�ؿ��з��б�
		}
	}
}

/**
 * MakeStepListForDouble - ˫��в�з�������
 * @return:	�����ŷ��б�
 * @myInfo:	ִ�巽��ǰ�����ۺ���Ϣ
 * @denInfo:	�Է���ǰ�����ۺ���Ϣ
 * @limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForDouble(int side, unsigned int limit)
{
	int unside = 1 - side;
	SynInfo myInfo = GetBoardInfo(side, TODUOTHREAT + TOSOLTHREAT + TODUOPOTEN);
	SynInfo tempMy, tempDen;//��ʱ������Ϣ����
	LineInfo tempLine[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Step> stepList;//�з��б�
	vector<Point> pointList, pointList2;//���б�
	vector<Point>::iterator iterP, iterP2;
	vector<Step>::iterator iterS;

	if (myInfo.duoThreatList.size() > 0)//����˫��в��
	{
		for (iterP = myInfo.duoThreatList.begin(); iterP != myInfo.duoThreatList.end(); iterP++)
		{
			tempStep.first = *iterP;
			MakeMove(tempStep.first, tempLine, side, FORNONTHREAT);
			tempMy = GetBoardInfo(side, FORNONTHREAT);
			if (tempMy.duoThreatList.size() > 0)//����˫��в���ͱز����ƽ�
			{
				stepList.resize(0);
				tempStep.second = tempMy.duoThreatList[0];
				tempStep.value = WINLOSE - 2;
				stepList.push_back(tempStep);
				BackMove(tempStep.first, tempLine, side);
				return stepList;
			}
			tempDen = GetBoardInfo(unside, FORPOTEN);
			//������<����в�㣬˫Ǳ���㣬��Ǳ����>�ͶԷ�<˫��в�㣬����в�㣬˫Ǳ����>��Ϊ��ѡ��
			for (iterP2 = tempMy.solThreatList.begin(); iterP2 != tempMy.solThreatList.end(); iterP2++)//��������в��
				pointList.push_back(*iterP2);
			for (iterP2 = tempMy.duoPotenList.begin(); iterP2 != tempMy.duoPotenList.end(); iterP2++)//����˫Ǳ����
				pointList.push_back(*iterP2);
			if (tempDen.duoThreatList.size() > 0)
			{
				for (iterP2 = tempDen.duoThreatList.begin(); iterP2 != tempDen.duoThreatList.end(); iterP2++)//�Է�˫��в��
					pointList.push_back(*iterP2);
			}
			else
			{
				for (iterP2 = tempMy.solPotenList.begin(); iterP2 != tempMy.solPotenList.end(); iterP2++)//������Ǳ����
					pointList.push_back(*iterP2);
				for (iterP2 = tempDen.solThreatList.begin(); iterP2 != tempDen.solThreatList.end(); iterP2++)//�Է�����в��
					pointList.push_back(*iterP2);
				for (iterP2 = tempDen.duoPotenList.begin(); iterP2 != tempDen.duoPotenList.end(); iterP2++)//�Է�˫Ǳ����
					pointList.push_back(*iterP2);
			}
			if (pointList.size() == 0)
			{
				for (iterP2 = tempMy.toDuoTwoList.begin(); iterP2 != tempMy.toDuoTwoList.end(); iterP2++)//����Ǳ˫Ǳ����
					pointList.push_back(*iterP2);
			}
			BackMove(tempStep.first, tempLine, side);
			UniquePoint(pointList);
			for (iterP2 = pointList.begin(); iterP2 != pointList.end(); iterP2++)
			{
				tempStep.second = *iterP2;
				stepList.push_back(tempStep);
			}
			pointList.resize(0);
		}
	}
	if (myInfo.solThreatList.size() > 0)//���ڵ���в&&˫Ǳ����
	{
		for (iterP = myInfo.solThreatList.begin(); iterP != myInfo.solThreatList.end(); iterP++)
		{
			tempStep.first = *iterP;
			MakeMove(tempStep.first, tempLine, TODUOTHREAT);
			tempMy = GetBoardInfo(side, TODUOTHREAT);
			BackMove(tempStep.first, tempLine, side);
			if (tempMy.duoThreatList.size() != 0)
			{
				for (iterP2 = tempMy.duoThreatList.begin(); iterP2 != tempMy.duoThreatList.end(); iterP2++)//����˫��в��
				{
					tempStep.second = *iterP2;
					stepList.push_back(tempStep);
				}
			}
			pointList.resize(0);
		}
	}
	if (myInfo.duoPotenList.size() > 0)//����˫Ǳ����
	{
		for (iterP = myInfo.duoPotenList.begin(); iterP != myInfo.duoPotenList.end(); iterP++)
		{
			tempStep.first = *iterP;
			MakeMove(tempStep.first, tempLine, side, TODUOTHREAT);
			tempMy = GetBoardInfo(side, TODUOTHREAT);//����˫��в��
			BackMove(tempStep.first, tempLine, side);
			for (iterP2 = tempMy.duoThreatList.begin(); iterP2 != tempMy.duoThreatList.end(); iterP2++)
			{
				tempStep.second = *iterP2;
				stepList.push_back(tempStep);
			}
		}
	}
	if (stepList.size() > 0)
	{
		UniqueStep(stepList);
		for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
			iterS->value = CalculateStepValue(*iterS, side);
		sort(stepList.begin(), stepList.end(), cmpStepValue);
		if (stepList.size() > limit)
			stepList.resize(limit);
	}
	return stepList;
}


//��������Ϣ����Ե����ڵ�4���ߣ�������Ϣ���浽��ʱ������
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
//�ָ�����Ϣ����Ե����ڵ�4���ߣ�����ʱ�����е���Ϣ���ǵ�ǰ��Ϣ
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
 * MakeMove - ���ߣ�ͬʱ�Ա�����߽��б���
 * @point:	���ӵ�����
 * @tempLine:	����Ϣ���ݴ�����
 * @side:	ִ����ɫ
 * @tag:	��ǩ
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
 * BackMove - ȡ�����ߣ�ͬʱ�ָ����������Ϣ
 * @point:	���ߵ�����
 * @tempLine:	������Ϣ������
 * @side:	ִ����ɫ
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
 * CalSingleLineValue - �߼�ֵ������
 * @return:	�����ߵļ�ֵ
 * @start:		�ߵ���ʼ������
 * @Direct:	�ߵķ����־
 * @side:		��������ߵļ�ֵ��Ϣ��������ִ����ɫ
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
 * @return:	�����ߵ����ͣ����Գɷ�
 * @start:		�ߵ���ʼ������
 * @Direct:	�ߵķ����־
 * @side:		��������ߵ�������Ϣ��������ִ����ɫ
 */
int CalSingleLineType(const Point start, const BYTE Direc, const BYTE side) //���������Direc������ߵļ�ֵ
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
				value += LineTypeType[preTable[shapeIndex]];
			}
			shapeIndex = 0;	// ƫ��������
			len = 0;		// ���ȹ���
		}
		Increment(x, y, Direc);
	}
	if (len > 5)	//���ȳ���5���Ż����ӡ�
	{
		shapeIndex += (1 << len);	  // �õ�������
		value += LineTypeType[preTable[shapeIndex]];
	}
	return value;
}


#define USEVALUE

/**
 * CalculateStepValue - �ŷ���ֵ������
 * @return:	�����ŷ���ֵ
 * @step:		��Ҫ��������ŷ�
 * @side:		�ŷ�������ִ����ɫ
 */
int CalculateStepValue(const Step step, const BYTE side)
#ifdef USEVALUE
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
		if (key2 != -1)
		{
			myC += CalSingleLineValue(start, i, side);
			denC += CalSingleLineValue(start, i, unside);
			myC -= lineInfo[side][key2].value;
			denC -= lineInfo[unside][key2].value;
		}
	}
	virtualBoard[step.first.x][step.first.y] = EMPTY;
	virtualBoard[step.second.x][step.second.y] = EMPTY;
	//	return myC-denC;
	return (myC * 4 - denC * 7) / 10;
}
#else if
{
	int key1,key2,i,unside=1-side;
	int lmy,lden,nmy,nden;
	int pdpM,spM,dpM,tM,pdpD,spD,dpD,tD;
	Point start;

	lmy=lden=0;
	for(i=0;i<4;i++)
	{
		key1=GetLineKey(step.first,&start,i);
		if(key1!=-1)
		{
			lmy+=CalSingleLineType(start,i,side);
			lden+=CalSingleLineType(start,i,unside);
		}
		key2=GetLineKey(step.second,&start,i);
		if(key2==key1)
			continue;
		if(key2!=-1)
		{
			lmy+=CalSingleLineType(start,i,side);
			lden+=CalSingleLineType(start,i,unside);
		}
	}
	virtualBoard[step.first.x][step.first.y]=side;
	virtualBoard[step.second.x][step.second.y]=side;
	nmy=nden=0;
	for(i=0;i<4;i++)
	{
		key1=GetLineKey(step.first,&start,i);
		if(key1!=-1)
		{
			nmy+=CalSingleLineType(start,i,side);
			nden+=CalSingleLineType(start,i,unside);
		}
		key2=GetLineKey(step.second,&start,i);
		if(key2==key1)
			continue;
		if(key2!=-1)
		{
			nmy+=CalSingleLineType(start,i,side);
			nden+=CalSingleLineType(start,i,unside);
		}
	}
	virtualBoard[step.first.x][step.first.y]=EMPTY;
	virtualBoard[step.second.x][step.second.y]=EMPTY;

	pdpM=nmy%10-lmy%10;
	nmy/=10;lmy/=10;
	spM=nmy%10-lmy%10;
	nmy/=10;lmy/=10;
	dpM=nmy%10-lmy%10;
	nmy/=10;lmy/=10;
	tM=nmy-lmy;

	pdpD=nden%10-lden%10;
	nden/=10;lden/=10;
	spD=nden%10-lden%10;
	nden/=10;lden/=10;
	dpD=nden%10-lden%10;
	nden/=10;lden/=10;
	tD=nden-lden;

	return 8*tM+6*dpM+3*spM+2*pdpM-100*tD-9*dpD-2*spD-3*pdpD;
}

#endif