#include "Seach.h"


BoardCode boardCode;//����״̬����

int SearchDepth = DEPTH;//�涨�������
int MaxDepth = MAXDEPTH;//����������


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
#ifdef DEBUGVALUE
	Step select;
#endif

	if (findEndLib(boardCode))//�����о�
	{
#ifdef DEBUGVALUE
		debugger.OutputMessage("info: �����о�!", 0);
#endif
		return depth - WINLOSE;
	}

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
		hashP.denType = GetBoardType(unside);//��ȡ������в����
	}

	//�ݹ����
	//�ڽ��������ı�Ҫ����Ϊ�Է�δȡʤ�����������������ּ�����ʤ�򷵻أ��ʵݹ����ֹ�����������Է���ʤ
	//�����������У�������ʤ�������ǶԷ�δ����������ȫ������в���ͣ����������ᵼ�¶Է����ܲ����ŷ����ʵݹ���ֹ����������������ʤ
	if (depth >= SearchDepth)//�������������
	{
		//�ﵽ���������ȼ�������ֵ
		//GetBoardValue(side) * 0.85 - GetBoardValue(unside)
		hashP.value = GetBoardValue(side) * 17 / 20 - GetBoardValue(unside);
		//��ʱ����������ŷ��б���Ӧ�����û���
		return hashP.value;
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
	else//��һ����ʷ����
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
/*		if (hashP.denType == 2 || hashP.denType == 10)
		{
			debugger.OutputMessage("����˫����в����", side);
			printf("error: can't create step!\n");
		}*/
#endif
		hashP.value = depth - WINLOSE;
		updateHash(hashP);
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
			alpha = iterS->value;
#ifdef DEBUGVALUE
			select = *iterS;
#endif
			//max����beta��֦
			if (beta <= alpha || iterS->value >= WINLOSE - MaxDepth)//ֻҪ��һ���ŷ����Ի�ʤ������ʤ
			{
				hashP.value = alpha;
				updateHash(hashP);
				return hashP.value;
			}
		}
	}
	hashP.cut = false;//δ����֦
	hashP.value = alpha;
	updateHash(hashP);
#ifdef DEBUGVALUE
	debugger.OutSelect(select);
#endif
	return hashP.value;
}

/**
 * TSS - ��в�ռ�������ͨ��DTSS/STSS��ϲ��Լ�������
 * @return:		���������ó��ľ���Ǳ������ֵ�������ֵ��
 * @side:		ģ�����巽ִ����ɫ
 * @alpha:		ָ����֦����������
 * @beta:		ָ����֦����������
 * @depth:		��ǰ�������
 */
int TSS(BYTE side, int alpha, int beta, const int depth)
{
	int unside = 1 - side, denType;
	LineInfo tempLine[2][4], tempLine2[2][4];
	vector<Step> stepList;
	vector<Step>::iterator iterS;

	if (findEndLib(boardCode))
	{
#ifdef DEBUGVALUE
		debugger.OutputMessage("info: �����о�!", 0);
#endif
		return depth - WINLOSE;
	}

	denType = GetBoardType(unside);

	//�ݹ����
	if (depth >= MaxDepth)//�������������Ƚ��з���
		return GetBoardValue(side) - GetBoardValue(unside);

	//��չ�����в���Ҫ�����ŷ��б����䲻�����ԣ��ڳ��������н����ᱻ����
	if (denType == 1)//����ܵ�����в��������������в����
		stepList = MakeStepListForDefendSingleEx(side, 6);
	else if (denType > 1)//�ܵ�˫��в���ؽ������У�����˫����������һ���֣�˫��в�����������4���ŷ���Ϊ�˷�ֹα˫Ӧ�뵥��в����һ�£�
		stepList = MakeStepListForDefendDoubleEx(side, 6);
	else//����вʱѰ�ҷ��� ˫�������� �Ļ���
		stepList = MakeStepListForDouble(side, 8);

#ifdef DEBUGVALUE
	debugger.OutputStep(stepList, side);
#endif
	if (stepList.size() == 0)
	{
		if (denType > 10 || (denType > 2 && denType < 10))//����в�����ƽ�
			return depth - WINLOSE;
		else//�����������������
			//GetBoardValue(side) * 0.85 - GetBoardValue(unside)
			return GetBoardValue(side) * 17 / 20 - GetBoardValue(unside);
	}

	for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
	{
		moveCodeS(boardCode, *iterS, side);//����
		MakeMove(iterS->first, tempLine, side);
		MakeMove(iterS->second, tempLine2, side);
#ifdef DEBUGVALUE
		debugger.MakeMove(*iterS);
#endif
		iterS->value = -TSS(unside, -beta, -alpha, depth + 1);
#ifdef DEBUGVALUE
		debugger.BackMove(*iterS);
#endif
		BackMove(iterS->second, tempLine2, side);
		BackMove(iterS->first, tempLine, side);
		moveCodeS(boardCode, *iterS, side);//�ָ�
		if (alpha < iterS->value)
		{
			alpha = iterS->value;
			if (beta <= alpha || iterS->value >= WINLOSE - MaxDepth)//ֻҪ��һ���ŷ����Ի�ʤ������ʤ
				return alpha;
		}
	}
	if (alpha <= MaxDepth - WINLOSE)
	{
		if (denType >= 2)
		{
			InsertEndLib(boardCode);
#ifdef DEBUGVALUE
			debugger.OutputMessage("info: ϰ�òо�!", 0);
#endif
		}
		else//������˫��в�������ıذܲ�����ذ�
			return MaxDepth - WINLOSE + 1;
	}

	return alpha;
}

/**
 * ExtendSeach - ��չ������ֵ������ͨ��TSS����������ʹ���浽��һ��ƽ��״̬���й�ֵ
 * @return:		���������ó��ľ���Ǳ������ֵ�������ֵ��
 * @side:		ģ�����巽ִ����ɫ
 * @alpha:		ָ����֦����������
 * @beta:		ָ����֦����������
 * @depth:		��ǰ�������
 */
int ExtendSeach(BYTE side, int alpha, int beta, const int depth)
{
	int unside = 1 - side;
	LineInfo tempLine[2][4], tempLine2[2][4];
	vector<Step>::iterator iterS;
	HashInfo *hashT, hashP;//��ϣ�ڵ�
#ifdef DEBUGVALUE
	Step select;
#endif

	if (findEndLib(boardCode))//�����о֣��о�Ϊ�ذܾ֣��Ӳоֿ��Ե�����ʤ��
	{
#ifdef DEBUGVALUE
		debugger.OutputMessage("info: �����о�!", 0);
#endif
		return depth - WINLOSE;
	}

	hashT = findHash(boardCode);//��ȡ�û�������ʷ
	if (hashT != NULL)//���������ʷ������ֱ�ӵ�����ʷ
	{
		//ͬһ���״̬����չ���Ĳ�����������ͬ��������Ⱥ㶨��������ֵ���������Ͻ����ģ�alpha-beta������ǰ�̳У�beta�����ݼ���
		if (hashT->timestamp == HandNum)//���������ʷ����ʱ����뵱ǰһ�£����ǳ��汾���������½��������ֱ�ӷ��ر��б���Ĳ���ֵ
			return hashT->value;
		else
			hashP = *hashT;
	}
	else//��������ʷ������������ϣ��
	{
		hashP.code = boardCode;//��ֵ����Ψһ��ʶ
		hashP.cut = true;//��ʼ��Ϊtrue������������֦
		hashP.full = false;//��ʼ��Ϊfalse�����������������ŷ��б�
		hashP.denType = GetBoardType(unside);//��ȡ������в����
	}

	//�ݹ����
	//�ڽ��������ı�Ҫ����Ϊ�Է�δȡʤ�����������������ּ�����ʤ�򷵻أ��ʵݹ����ֹ�����������Է���ʤ
	//�����������У�������ʤ�������ǶԷ�δ����������ȫ������в���ͣ����������ᵼ�¶Է����ܲ����ŷ����ʵݹ���ֹ����������������ʤ
	if (depth >= SearchDepth)//������������ȣ����м�������
	{
		hashP.value = TSS(side, alpha, beta, depth);//ͬһ�������
		if (hashP.value > MaxDepth - WINLOSE && hashP.value < WINLOSE - MaxDepth)//����ʶ��ʤ�� 
			//hashP.value * 0.4 + (GetBoardValue(side) * 0.85 - GetBoardValue(unside)) * 0.6
			hashP.value = (hashP.value * 40 + GetBoardValue(side) * 51  - GetBoardValue(unside) * 60) / 100;
		//��ʱ����������ŷ��б���Ӧ�����û�����������һ��ʱ����Ϊ�޿������ŷ�������
		return hashP.value;
	}

	//����ȫ���ŷ�
	if (hashP.full == false)//����ϣ�����full�ֶ�ֵΪfalseʱ��֤���ñ����stepList�ֶ�ֵ�����������ܱ�ֱ�����á�
	{
		if (hashP.denType == 1)//�ܵ�����в
			hashP.stepList = MakeStepListForDefendSingle(side, 20);
		else if (hashP.denType >= 2)//�ܵ�˫��в������в��α˫��в��α����в��˫��в�����������4���ŷ���Ϊ�˷�ֹα˫Ӧ�뵥��в����һ�£�
			hashP.stepList = MakeStepListForDefendDouble(side, 20);
		else//��������в���Σ�������4�ͺ�5�ͣ�
			hashP.stepList = MakeStepListForNone(side, 23);
		hashP.full = true;
	}
	else//��һ����ʷ����
	{
		if (hashP.cut == false)//cut�ֶ�Ϊfalse(δ��֦)��֤��stepList��ÿһ���value���۱�׼һ�¡���ʱ���Զ�stepList�е��ŷ�����������������߼�֦Ч��
		{
			sort(hashP.stepList.begin(), hashP.stepList.end(), cmpStepValue);
			hashP.cut = true;
		}
	}
	if (hashP.stepList.size() == 0)//��Ϊ�����ƽ���棬���������ŷ����������֧������
	{
#ifdef DEBUGVALUE
/*		if (hashP.denType == 2 || hashP.denType == 10)
		{
			debugger.OutputMessage("����˫����в����", side);
			printf("error: can't create step!\n");
		}*/
#endif
		hashP.value = depth - WINLOSE;
		updateHash(hashP);
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
		iterS->value = -ExtendSeach(unside, -beta, -alpha, depth + 1);
#ifdef DEBUGVALUE
		debugger.BackMove(*iterS);
#endif
		BackMove(iterS->second, tempLine2, side);
		BackMove(iterS->first, tempLine, side);
		moveCodeS(boardCode, *iterS, side);//�ָ�
		if (alpha < iterS->value)
		{
			alpha = iterS->value;
#ifdef DEBUGVALUE
			select = *iterS;
#endif
			//max����beta��֦
			if (beta <= alpha || iterS->value >= WINLOSE - MaxDepth)//ֻҪ��һ���ŷ����Ի�ʤ������ʤ
			{
				hashP.value = alpha;
				updateHash(hashP);
				return hashP.value;
			}
		}
	}
	hashP.cut = false;//δ����֦
	hashP.value = alpha;
	if (alpha <= MaxDepth - WINLOSE)
	{
		if (hashP.denType >= 2)
		{
			InsertEndLib(boardCode);
#ifdef DEBUGVALUE
			debugger.OutputMessage("info: ϰ�òо�!", 0);
#endif
		}
		else//������˫��в�������ıذܲ�����ذ�
			hashP.value = MaxDepth - WINLOSE + 1;
	}
	updateHash(hashP);
#ifdef DEBUGVALUE
	debugger.OutSelect(select);
#endif
	return hashP.value;
}

/**
* SeachValuavleStep - ��������з��������������
* @return:		����������ߵ�����ŷ�
* @side:		����ִ����ɫ
* @seachDepth:	��������Բ���������չ�����
*/
Step SeachValuableStep(Step denStep, BYTE side)
{
	Step step = { { -1, -1 }, { -1, -1 }, -WINLOSE };//��ʼ��Ϊ���ӣ���ֹ���ַ�������
	int alpha = -WINLOSE, unside = 1 - side, myType, denType = GetBoardType(unside);
	LineInfo tempLine[2][4], tempLine2[2][4];
	vector<Step> stepList;
	vector<Step>::iterator iterS;
	HashInfo *hashP;

	//����ʷ��ϣ���в���
	hashP = findHash(boardCode);
	if (hashP != NULL && hashP->full == true)//���������ʷ,�Ҿ��������ŷ��б���ֱ�ӵ�����ʷ�߷���
	{
		if (hashP->cut == false)//cut�ֶ�Ϊfalse��֤��stepList��ÿһ���value���۱�׼һ�¡���ʱ���Զ�stepList�е��ŷ�����������������߼�֦Ч��
			sort(hashP->stepList.begin(), hashP->stepList.end(), cmpStepValue);
		stepList = hashP->stepList;
#ifdef DEBUGVALUE
		debugger.OutputMessage("get step list from hashList", side);
#endif
	}
	else
	{
		if (HandNum == 2)//�ڶ��ֱȽ����⣬��ʹ����Ϊ�涨���������ֽ�������
		{
			Point temp;
			int type = 0;
			if (denStep.first.x != -1 && denStep.first.y != -1)
			{
				temp = denStep.first;
				if (temp.x>9)type = 2;
				if (temp.y>9)type++;
				switch (type)
				{
				case 0:
					step.first.x = temp.x + 1; step.first.y = temp.y;
					step.second.x = temp.x; step.second.y = temp.y + 1;
					break;
				case 1:
					step.first.x = temp.x + 1; step.first.y = temp.y;
					step.second.x = temp.x; step.second.y = temp.y - 1;
					break;
				case 2:
					step.first.x = temp.x - 1; step.first.y = temp.y;
					step.second.x = temp.x; step.second.y = temp.y + 1;
					break;
				case 3:
					step.first.x = temp.x - 1; step.first.y = temp.y;
					step.second.x = temp.x; step.second.y = temp.y - 1;
					break;
				}
			}
			else if (denStep.second.x != -1 && denStep.second.y != -1)
			{
				temp = denStep.second;
				if (temp.x>9) type = 2;
				if (temp.y>9) type++;
				switch (type)
				{
				case 0:
					step.first.x = temp.x++; step.first.y = temp.y;
					step.second.x = temp.x; step.second.y = temp.y++;
					break;
				case 1:
					step.first.x = temp.x++; step.first.y = temp.y;
					step.second.x = temp.x; step.second.y = temp.y--;
					break;
				case 2:
					step.first.x = temp.x--; step.first.y = temp.y;
					step.second.x = temp.x; step.second.y = temp.y++;
					break;
				case 3:
					step.first.x = temp.x--; step.first.y = temp.y;
					step.second.x = temp.x; step.second.y = temp.y--;
					break;
				}
			}
			else
			{
				step.first.x = step.first.y = 9;
				step.second.x = step.second.y = 10;
			}
			return step;
		}
		else
		{
			myType = GetBoardType(side);
			if (myType > 0)//�������ܶԶԷ�������в�����ǿ�ʤ���棬���ɱ�ʤ�����㣩
				stepList = MakeStepListForWin(side, 1);
			else if (denType == 1)//�ܵ�����в
				stepList = MakeStepListForDefendSingle(side, 20);
			else if (denType > 1)//�ܵ�˫��в������в��α˫��в��α����в
				stepList = MakeStepListForDefendDouble(side, 18);
			else//��������в���Σ�������4�ͺ�5�ͣ�
				stepList = MakeStepListForNone(side, 23);
		}
	}
	debugger.OutputStep(stepList, side);
	if (stepList.size() == 0)//���뷵��һ���ŷ�����������
	{
		printf("The engine don't create step!\n");
		bool first = true;
		if (denType > 0)
		{
			SynInfo denInfo = GetBoardInfo(unside, TODEFENT);
			if (denInfo.defStepList.size() > 0)
				return denInfo.defStepList[0];
			else if (denInfo.defPointList.size() > 0)
			{
				LineInfo tempLine[2][4];//��ʱ����Ϣ����
				step.first = denInfo.defPointList[0];
				MakeMove(step.first, tempLine, side, TODEFENT);
				SynInfo denInfo2 = GetBoardInfo(unside, TODEFENT);
				if (denInfo2.defPointList.size() > 0)
				{
					step.second = denInfo2.defPointList[0];
					return step;
				}
				first = false;
			}
		}
		for (int i = 0; i < edge; i++)
		{
			for (int j = 0; j < edge; j++)
			{
				if (virtualBoard[i][j] == EMPTY)
				{
					if (first)
					{
						step.first.x = i; step.first.y = j;
						first = false;
					}
					else
					{
						step.second.x = i; step.second.y = j;
						return step;
					}
				}
			}
		}
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
			if (HandNum <= 6)//��Ϊ���ֽ׶Σ�ǰ6�֣���������Ϣ���٣����˽�����չ����
				iterS->value = -nega_alpha_beta(unside, -WINLOSE, -alpha, 1);//������ֵ����alpha-beta��֦������ֵ
			else
				iterS->value = -ExtendSeach(unside, -WINLOSE, -alpha, 1);
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
	debugger.OutputStep(stepList, side);
	debugger.OutSelect(step);
	return step;
}

/**
 * sixgo_carry - �����岩��������ģ����湤�����
 * @return��		�����������ó����ŷ�
 * @moveStep:	���������ŷ�
 * @side:		����ִ����ɫ
 */
Step sixgo_carry(const Step moveStep, const int nBoard[19][19], const BYTE side)
{
	int i, j;
	Point point;
	//������������
	initialHashCode(boardCode);
	for (i = 0; i < edge; i++)
		for (j = 0; j < edge; j++)
		{
			if (nBoard[i][j] != EMPTY)
			{
				point.x = i; point.y = j;
				moveCodeP(boardCode, point, nBoard[i][j]);//���й�ϣ����
			}
			virtualBoard[i][j] = nBoard[i][j];
		}
	debugger.InitDebugger(HandNum, moveStep);
	//��������з�
	Step step = SeachValuableStep(moveStep, side);
	debugger.BackMove(step);
	return step;
}
