#include "StepGenerator.h"


/**
 * MakeStepListForWin - ��ʤ�з�������
 * @myInfo:		ִ�巽��ǰ�����ۺ���Ϣ
 * @denInfo:	�Է���ǰ�����ۺ���Ϣ
 * @limit:		�����ŷ�����������
 * @return:		�����ŷ��б�
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
 * @myInfo:		ִ�巽��ǰ�����ۺ���Ϣ
 * @denInfo:	�Է���ǰ�����ۺ���Ϣ
 * @limit:		�����ŷ�����������
 * @return:		�����ŷ��б�
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
		if (tempMy.duoThreatList.size() > 0)
			for (iterP2 = tempMy.duoThreatList.begin(); iterP2 != tempMy.duoThreatList.end(); iterP2++)
				pointList.push_back(*iterP2);
		if (tempMy.solThreatList.size() > 0)
			for (iterP2 = tempMy.solThreatList.begin(); iterP2 != tempMy.solThreatList.end(); iterP2++)
				pointList.push_back(*iterP2);
		if (tempMy.duoPotenList.size() > 0)
			for (iterP2 = tempMy.duoPotenList.begin(); iterP2 != tempMy.duoPotenList.end(); iterP2++)
				pointList.push_back(*iterP2);
		if (tempDen.duoThreatList.size() > 0)
			for (iterP2 = tempDen.duoThreatList.begin(); iterP2 != tempDen.duoThreatList.end(); iterP2++)
				pointList.push_back(*iterP2);
		if (tempDen.solThreatList.size() > 0)
			for (iterP2 = tempDen.solThreatList.begin(); iterP2 != tempDen.solThreatList.end(); iterP2++)
				pointList.push_back(*iterP2);
		if (tempDen.duoPotenList.size() > 0)
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
		pointList.clear();
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
	if (denInfo.defStepList.size() > 0)//�����ŷ��ر���
	{
		for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)//������THREAT_four_ADDITION����Ķ����ƽⲽ
			stepList.push_back(*iterS);
	}
	return stepList;
}

/**
 * MakeStepListForDefendDouble - ����˫��в���з�������
 * @myInfo:		ִ�巽��ǰ�����ۺ���Ϣ
 * @denInfo:	�Է���ǰ�����ۺ���Ϣ
 * @limit:		�����ŷ�����������
 * @return:		�����ŷ��б�
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
			if (stepList.size() > 0)//���ƽ�
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
					pointList.clear();
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
			if (stepList.size() > 0)//���ƽ�
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
 * MakeStepListForNone - ƽ�Ⱦ����ŷ�������
 * @myInfo:		ִ�巽��ǰ�����ۺ���Ϣ
 * @denInfo:	�Է���ǰ�����ۺ���Ϣ
 * @limit:		�����ŷ�����������
 * @return:		�����ŷ��б�
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

	//��ѡȡ������<˫��в�㣬˫Ǳ���㣬����в��>���Է�<˫��в�㣬˫Ǳ���㣬����в��> ��Ϊ��һ�����������
	if (myInfo.duoThreatList.size() > 0)
		for (iterP = myInfo.duoThreatList.begin(); iterP != myInfo.duoThreatList.end(); iterP++)
			pointList.push_back(*iterP);
	if (myInfo.duoPotenList.size() > 0)
		for (iterP = myInfo.duoPotenList.begin(); iterP != myInfo.duoPotenList.end(); iterP++)
			pointList.push_back(*iterP);
	if (myInfo.solThreatList.size() > 0)
		for (iterP = myInfo.solThreatList.begin(); iterP != myInfo.solThreatList.end(); iterP++)
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
		else if (pointList.size() == 0)//�ټ��뱾����Ǳ���㣬�Է���Ǳ�����Ǳ˫Ǳ����
		{
			if (myInfo.solPotenList.size() > 0)//������Ǳ������ڴ˴���Ϊ������˫�ӵ���в�ŷ�������
				for (iterP = myInfo.solPotenList.begin(); iterP != myInfo.solPotenList.end(); iterP++)
					pointList.push_back(*iterP);
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
		pointList2.clear();
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
 * @myInfo:		ִ�巽��ǰ�����ۺ���Ϣ
 * @denInfo:	�Է���ǰ�����ۺ���Ϣ
 * @limit:		�����ŷ�����������
 * @return:		�����ŷ��б�
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
		//������ֵ����С�������жԵ��ȥ�ز���
		BackMove(tempStep.first, tempLine, side);
		for (iterP2 = pointList.begin(); iterP2 != pointList.end(); iterP2++)
		{
			tempStep.second = *iterP2;
			tempStep.value = CalculateStepValue(tempStep, side);
			stepList.push_back(tempStep);
		}
		pointList.clear();
	}
	if (stepList.size() > 0)
	{
		UniqueStep(stepList);
		sort(stepList.begin(), stepList.end(), cmpStepValue);

		unsigned int size = stepList.size();
		if (size > limit)
			stepList.resize(limit);
	}
	if (denInfo.defStepList.size() > 0)//�����ŷ��ر���
	{
		for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)//������THREAT_four_ADDITION����Ķ����ƽⲽ�����ƽ�㲻�غ�
			stepList.push_back(*iterS);
	}
	return stepList;
}

/**
 * MakeStepListForDefendDoubleEx - ����˫��в���ŷ���չ��������������չ����
 * @myInfo:		ִ�巽��ǰ�����ۺ���Ϣ
 * @denInfo:	�Է���ǰ�����ۺ���Ϣ
 * @limit:		�����ŷ�����������
 * @return:		�����ŷ��б�
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
			if (stepList.size() > 0)//���ƽ�
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
					pointList.clear();
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
			if (stepList.size() > 0)//���ƽ�
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
				stepList.clear();
				tempStep.second = tempMy.duoThreatList[0];
				tempStep.value = WINLOSE - MaxDepth - 1;
				stepList.push_back(tempStep);
				BackMove(tempStep.first, tempLine, side);
				return stepList;
			}
			//else if (tempMy.solThreatList.size() > 0)//һ��˫��в
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
			pointList.clear();
		}
	}
	if (myInfo.solThreatList.size() > 0)//����в��
	{
		for (iterP = myInfo.solThreatList.begin(); iterP != myInfo.solThreatList.end(); iterP++)
		{
			tempStep.first = *iterP;
			MakeMove(tempStep.first, tempLine, TODUOTHREAT | TOSOLTHREAT);
			tempMy = GetBoardInfo(side, TODUOTHREAT | TOSOLTHREAT);
			BackMove(tempStep.first, tempLine, side);
			if (tempMy.duoThreatList.size() > 0)//˫��в��
			{
				for (iterP2 = tempMy.duoThreatList.begin(); iterP2 != tempMy.duoThreatList.end(); iterP2++)//����˫��в��
				{
					tempStep.second = *iterP2;
					stepList.push_back(tempStep);
				}
			}
			if (tempMy.solThreatList.size() > 0)//��һ������в��
			{
				for (iterP2 = tempMy.solThreatList.begin(); iterP2 != tempMy.solThreatList.end(); iterP2++)//����˫��в��
				{
					tempStep.second = *iterP2;
					stepList.push_back(tempStep);
				}
			}
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
