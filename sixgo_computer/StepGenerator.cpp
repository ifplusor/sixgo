#include "StepGenerator.h"


/**
 * MakeStepListForWin - 致胜招法生成器
 * @myInfo:		执棋方当前局面综合信息
 * @denInfo:	对方当前局面综合信息
 * @limit:		生成着法的数量限制
 * @return:		返回着法列表
 */
vector<Step> MakeStepListForWin(int side, unsigned int limit)
{
	SynInfo myInfo = GetBoardInfo(side, TOWIN | TOWILLWIN);
	LineInfo tempLine[2][4];//临时线信息备份
	Step tempStep;
	vector<Point>::iterator iterP, iterP2;
	vector<Step> stepList;//招法列表

	//如果存在致胜点，则直接返回第一个制胜点作为招法
	//否则选取第一个即将致胜点进行试下，并用其余试下后的第一个致胜点进行组步
	if (myInfo.winList.size() == 0)
	{
		tempStep.second = myInfo.willWinList[0];
		MakeMove(tempStep.second, tempLine, side, TOWIN);//因为必胜，故只对致胜点进行收集
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
 * MakeStepListForDefendSingle - 防御单威胁招法生成器
 * @myInfo:		执棋方当前局面综合信息
 * @denInfo:	对方当前局面综合信息
 * @limit:		生成着法的数量限制
 * @return:		返回着法列表
 */
vector<Step> MakeStepListForDefendSingle(int side, unsigned int limit)
{
	int unside = 1 - side;
	SynInfo denInfo = GetBoardInfo(unside, TODEFENT);
	SynInfo tempMy, tempDen;//临时局面信息储存
	LineInfo tempLine[2][4];//临时线信息备份
	Step tempStep;
	vector<Step> stepList;//招法列表
	vector<Point> pointList;//点列表
	vector<Point>::iterator iterP, iterP2;
	vector<Step>::iterator iterS;

	//选取一个破解点进行试下，然后针对试下后的局面进行组步，一条单威胁线型至多包含两个破解点
	for (iterP = denInfo.defPointList.begin(); iterP != denInfo.defPointList.end(); iterP++)
	{
		tempStep.first = *iterP;
		MakeMove(tempStep.first, tempLine, side, FORNONTHREAT);//试下后收集全部信息
		tempMy = GetBoardInfo(side, FORNONTHREAT);//针对无威胁情况收集本方信息
		tempDen = GetBoardInfo(unside, FORPOTEN);//收集对手的潜力信息（无）
		//将本方<双威胁点，单威胁点，双潜力点>和对方<双威胁点，单威胁点，双潜力点>作为备选点
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
		UniquePoint(pointList);//去重操作
		if (pointList.size() < 10)//如果备选点少于10个，则启用本方单潜力点，潜双潜力点和对方单潜力点
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
		printf("异常: On make step list for Defend solthreat!\n");
	}
	else
	{
		UniqueStep(stepList);
		sort(stepList.begin(), stepList.end(), cmpStepValue);

		unsigned int size = stepList.size();
		if (size > limit)
			stepList.resize(limit);
	}
	if (denInfo.defStepList.size() > 0)//额外着法必保留
	{
		for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)//加入由THREAT_four_ADDITION引起的额外破解步
			stepList.push_back(*iterS);
	}
	return stepList;
}

/**
 * MakeStepListForDefendDouble - 防御双威胁等招法生成器
 * @myInfo:		执棋方当前局面综合信息
 * @denInfo:	对方当前局面综合信息
 * @limit:		生成着法的数量限制
 * @return:		返回着法列表
 */
vector<Step> MakeStepListForDefendDouble(int side, unsigned int limit)
{
	int unside = 1 - side;
	SynInfo denInfo = GetBoardInfo(unside, TODEFENT);
	SynInfo tempMy, tempDen;//临时局面信息储存
	LineInfo tempLine[2][4], tempLine2[2][4];//临时线信息备份
	Step tempStep;
	vector<Step> stepList;//招法列表
	vector<Point> pointList;//点列表
	vector<Point>::iterator iterP, iterP2;
	vector<Step>::iterator iterS;
	int denType = GetBoardType(unside);

	if (denType >= 20)//对方存在多威胁线型或已胜线型，或存在两个以上双威胁线型，本方必败直接返回空招法列表
	{
		return stepList;
	}
	else if (denType == 10)//此局面只存在一个双威胁线型，故只将破解步作为备选招法加入招法列表
	{
		for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)
			stepList.push_back(*iterS);
		UniqueStep(stepList);
		for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
			iterS->value = CalculateStepValue(*iterS, side);
		sort(stepList.begin(), stepList.end(), cmpStepValue);
		return stepList;
	}
	else//含有单威胁线型的棋局，多个单威胁（可能存在伪双），一个双威胁加单威胁（可能存在伪多）
	{
		if (denType > 10 && denType < 20)//存在一个双威胁线型，同时存在单威胁线型（可能存在伪多威胁）
		{
			for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)//以双威胁线型为主要成分，若可破解必使用双威胁线型的破解步
			{
				MakeMove(iterS->first, tempLine, side, 0);//查看是否可以破解威胁，只要求线的类型，故不用收集更新的点信息
				MakeMove(iterS->second, tempLine2, side, 0);
				if (GetBoardType(unside) == 0)//判断是否破解成功，可能因为THREAT_four_ADDITION的额外破解法当成双威胁
					stepList.push_back(*iterS);
				BackMove(iterS->second, tempLine2, side);
				BackMove(iterS->first, tempLine, side);
			}
			if (stepList.size() > 0)//可破解
			{
				UniqueStep(stepList);
				for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
					iterS->value = CalculateStepValue(*iterS, side);
				sort(stepList.begin(), stepList.end(), cmpStepValue);
			}
			return stepList;//包含不可破解局面，若不可破解返回空招法列表
		}
		else//只存在多个单威胁线型（可能存在伪双）
		{
			for (iterP = denInfo.defPointList.begin(); iterP != denInfo.defPointList.end(); iterP++)
			{
				tempStep.first = *iterP;
				MakeMove(tempStep.first, tempLine, side, FORNONTHREAT + TODEFENT);
				tempDen = GetBoardInfo(unside, FORPOTEN + TODEFENT);//一子试走后进行二次分析，这样可避免针对伪双威胁的招法的第二子出现单子破解状态，从而使第一子无用
				if (GetBoardType(unside) == 0)//一子破解，伪双威胁，按单威胁方案生成；根据无侥幸行棋逻辑，不存在可单子破解的三子以上威胁
				{
					tempMy = GetBoardInfo(side, FORNONTHREAT);
					//将本方<双威胁点，单威胁点，双潜力点>和对方<多威胁点，双威胁点，单威胁点，双潜力点>作为备选点
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
				else//两子破解，包括伪双威胁
				{
					for (iterP2 = tempDen.defPointList.begin(); iterP2 != tempDen.defPointList.end(); iterP2++)
					{
						MakeMove(*iterP2, tempLine2, side, 0);
						if (GetBoardType(unside) == 0)//判断是否破解成功
						{
							tempStep.second = *iterP2;
							stepList.push_back(tempStep);
						}
						BackMove(*iterP2, tempLine2, side);
					}
				}
				BackMove(tempStep.first, tempLine, side);
			}
			if (stepList.size() > 0)//可破解
			{
				UniqueStep(stepList);
				for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
					iterS->value = CalculateStepValue(*iterS, side);
				sort(stepList.begin(), stepList.end(), cmpStepValue);
				if (stepList.size() > limit)
					stepList.resize(limit);
			}
			return stepList;//包含不可破解局面，若不可破解返回空招法列表
		}
	}
}

/**
 * MakeStepListForNone - 平稳局面着法生成器
 * @myInfo:		执棋方当前局面综合信息
 * @denInfo:	对方当前局面综合信息
 * @limit:		生成着法的数量限制
 * @return:		返回着法列表
 */
vector<Step> MakeStepListForNone(int side, unsigned int limit)
{
	int unside = 1 - side;
	SynInfo myInfo = GetBoardInfo(side, FORNONTHREAT);
	SynInfo denInfo = GetBoardInfo(unside, FORNONTHREAT);
	SynInfo tempMy, tempDen;//临时局面信息储存
	LineInfo tempLine[2][4];//临时线信息备份
	Step tempStep;
	vector<Step> stepList;//招法列表
	vector<Point> pointList, pointList2;//点列表
	vector<Point>::iterator iterP, iterP2;
	vector<Step>::iterator iterS;

	//先选取本方的<双威胁点，双潜力点，单威胁点>，对方<双威胁点，双潜力点，单威胁点> 作为第一批点进行试走
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
	if (pointList.size() < 5)//若第一批点少于5个，首先加入本方潜双潜力点
	{
		if (myInfo.toDuoTwoList.size() > 0)
		{
			for (iterP = myInfo.toDuoTwoList.begin(); iterP != myInfo.toDuoTwoList.end(); iterP++)
				pointList.push_back(*iterP);
		}
		else if (pointList.size() == 0)//再加入本方单潜力点，对方单潜力点和潜双潜力点
		{
			if (myInfo.solPotenList.size() > 0)//本方单潜力点放在此处是为了抑制双子单威胁着法的生成
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
		//把本方的<双威胁点，单威胁点，双潜力点>，对方的<双威胁点，双潜力点，单威胁点>加入第二批点
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
		//如果没有以上第二批点，则启用本方的单潜力点，潜双潜力点和对方的单潜力点
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
	if (stepList.size() == 0)//未能获取到较好的点
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
 * MakeStepListForDefendSingleEx - 防御单威胁着法扩展生成器，用于扩展搜索
 * @myInfo:		执棋方当前局面综合信息
 * @denInfo:	对方当前局面综合信息
 * @limit:		生成着法的数量限制
 * @return:		返回着法列表
 */
vector<Step> MakeStepListForDefendSingleEx(int side, unsigned int limit)
{
	int unside = 1 - side;
	SynInfo denInfo = GetBoardInfo(unside, TODEFENT);
	SynInfo tempMy;//临时局面信息储存
	LineInfo tempLine[2][4];//临时线信息备份
	Step tempStep;
	vector<Step> stepList;//招法列表
	vector<Point> pointList;//点列表
	vector<Point>::iterator iterP, iterP2;
	vector<Step>::iterator iterS;

	//选取一个破解点进行试下，然后针对试下后的局面进行组步，一条单威胁线型至多包含两个破解点
	for (iterP = denInfo.defPointList.begin(); iterP != denInfo.defPointList.end(); iterP++)
	{
		tempStep.first = *iterP;
		MakeMove(tempStep.first, tempLine, side, TODUOTHREAT + TOSOLTHREAT);//试下后收集全部信息
		tempMy = GetBoardInfo(side, TODUOTHREAT + TOSOLTHREAT);//针对无威胁情况收集本方信息
		//将本方<双威胁点，单威胁点>作为备选点
		for (iterP2 = tempMy.duoThreatList.begin(); iterP2 != tempMy.duoThreatList.end(); iterP2++)
			pointList.push_back(*iterP2);
		for (iterP2 = tempMy.solThreatList.begin(); iterP2 != tempMy.solThreatList.end(); iterP2++)
			pointList.push_back(*iterP2);
		//因计算价值代价小，不进行对点的去重操作
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
	if (denInfo.defStepList.size() > 0)//额外着法必保留
	{
		for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)//加入由THREAT_four_ADDITION引起的额外破解步，与破解点不重合
			stepList.push_back(*iterS);
	}
	return stepList;
}

/**
 * MakeStepListForDefendDoubleEx - 防御双威胁等着法扩展生成器，用于扩展搜索
 * @myInfo:		执棋方当前局面综合信息
 * @denInfo:	对方当前局面综合信息
 * @limit:		生成着法的数量限制
 * @return:		返回着法列表
 */
vector<Step> MakeStepListForDefendDoubleEx(int side, unsigned int limit)
{
	int unside = 1 - side;
	SynInfo denInfo = GetBoardInfo(unside, TODEFENT);
	SynInfo tempMy, tempDen;//临时局面信息储存
	LineInfo tempLine[2][4], tempLine2[2][4];//临时线信息备份
	Step tempStep;
	vector<Step> stepList;//招法列表
	vector<Point> pointList;//点列表
	vector<Point>::iterator iterP, iterP2;
	vector<Step>::iterator iterS;
	int denType = GetBoardType(unside);

	if (denType >= 20)//对方存在多威胁线型或已胜线型，或存在两个以上双威胁线型，本方必败直接返回空招法列表
	{
		return stepList;
	}
	else if (denType == 10)//此局面只存在一个双威胁线型，故只将破解步作为备选招法加入招法列表
	{
		for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)
			stepList.push_back(*iterS);
		//UniqueStep(stepList);
		for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
			iterS->value = CalculateStepValue(*iterS, side);
		sort(stepList.begin(), stepList.end(), cmpStepValue);
		return stepList;
	}
	else//含有单威胁线型的棋局，多个单威胁（可能存在伪双），一个双威胁加单威胁（可能存在伪多）
	{
		if (denType > 10 && denType < 20)//存在一个双威胁线型，同时存在单威胁线型（可能存在伪多威胁）
		{
			for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)//以双威胁线型为主要成分，若可破解必使用双威胁线型的破解步
			{
				MakeMove(iterS->first, tempLine, side, 0);//查看是否可以破解威胁，只要求线的类型，故不用收集更新的点信息
				MakeMove(iterS->second, tempLine2, side, 0);
				if (GetBoardType(unside) == 0)//判断是否破解成功，可能因为THREAT_four_ADDITION的额外破解法当成双威胁
					stepList.push_back(*iterS);
				BackMove(iterS->second, tempLine2, side);
				BackMove(iterS->first, tempLine, side);
			}
			if (stepList.size() > 0)//可破解
			{
				UniqueStep(stepList);
				for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
					iterS->value = CalculateStepValue(*iterS, side);
				sort(stepList.begin(), stepList.end(), cmpStepValue);
			}
			return stepList;//包含不可破解局面，若不可破解返回空招法列表
		}
		else//只存在多个单威胁线型（可能存在伪双）
		{
			for (iterP = denInfo.defPointList.begin(); iterP != denInfo.defPointList.end(); iterP++)
			{
				tempStep.first = *iterP;
				MakeMove(tempStep.first, tempLine, side, TODUOTHREAT + TOSOLTHREAT);
				//一子试走后进行二次分析，这样可避免针对伪双威胁的招法的第二子出现单子破解状态，从而使第一子无用
				if (GetBoardType(unside) == 0)//一子破解，伪双威胁，按单威胁方案生成；根据无侥幸行棋逻辑，不存在可单子破解的三子以上威胁
				{
					tempMy = GetBoardInfo(side, TODUOTHREAT + TOSOLTHREAT);
					//将本方<双威胁点，单威胁点>
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
				else//两子破解，包括伪双威胁
				{
					tempDen = GetBoardInfo(unside, 2);
					for (iterP2 = tempDen.defPointList.begin(); iterP2 != tempDen.defPointList.end(); iterP2++)
					{
						MakeMove(*iterP2, tempLine2, side, 0);
						if (GetBoardType(unside) == 0)//判断是否破解成功
						{
							tempStep.second = *iterP2;
							stepList.push_back(tempStep);
						}
						BackMove(*iterP2, tempLine2, side);
					}
				}
				BackMove(tempStep.first, tempLine, side);
			}
			if (stepList.size() > 0)//可破解
			{
				UniqueStep(stepList);
				for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
					iterS->value = CalculateStepValue(*iterS, side);
				sort(stepList.begin(), stepList.end(), cmpStepValue);
				if (stepList.size() > limit)
					stepList.resize(limit);
			}
			return stepList;//包含不可破解局面，若不可破解返回空招法列表
		}
	}
}

/**
* MakeStepListForDouble - 双威胁招法生成器
* @return:	返回着法列表
* @myInfo:	执棋方当前局面综合信息
* @denInfo:	对方当前局面综合信息
* @limit:		生成着法的数量限制
*/
vector<Step> MakeStepListForDouble(int side, unsigned int limit)
{
	int unside = 1 - side;
	SynInfo myInfo = GetBoardInfo(side, TODUOTHREAT + TOSOLTHREAT + TODUOPOTEN);
	SynInfo tempMy, tempDen;//临时局面信息储存
	LineInfo tempLine[2][4];//临时线信息备份
	Step tempStep;
	vector<Step> stepList;//招法列表
	vector<Point> pointList, pointList2;//点列表
	vector<Point>::iterator iterP, iterP2;
	vector<Step>::iterator iterS;

	if (myInfo.duoThreatList.size() > 0)//存在双威胁点
	{
		for (iterP = myInfo.duoThreatList.begin(); iterP != myInfo.duoThreatList.end(); iterP++)
		{
			tempStep.first = *iterP;
			MakeMove(tempStep.first, tempLine, side, FORNONTHREAT);
			tempMy = GetBoardInfo(side, FORNONTHREAT);
			if (tempMy.duoThreatList.size() > 0)//两条双威胁线型必不可破解
			{
				stepList.clear();
				tempStep.second = tempMy.duoThreatList[0];
				tempStep.value = WINLOSE - MaxDepth - 1;
				stepList.push_back(tempStep);
				BackMove(tempStep.first, tempLine, side);
				return stepList;
			}
			//else if (tempMy.solThreatList.size() > 0)//一条双威胁
			tempDen = GetBoardInfo(unside, FORPOTEN);
			//将本方<单威胁点，双潜力点，单潜力点>和对方<双威胁点，单威胁点，双潜力点>作为备选点
			for (iterP2 = tempMy.solThreatList.begin(); iterP2 != tempMy.solThreatList.end(); iterP2++)//本方单威胁点
				pointList.push_back(*iterP2);
			for (iterP2 = tempMy.duoPotenList.begin(); iterP2 != tempMy.duoPotenList.end(); iterP2++)//本方双潜力点
				pointList.push_back(*iterP2);
			if (tempDen.duoThreatList.size() > 0)
			{
				for (iterP2 = tempDen.duoThreatList.begin(); iterP2 != tempDen.duoThreatList.end(); iterP2++)//对方双威胁点
					pointList.push_back(*iterP2);
			}
			else
			{
				for (iterP2 = tempMy.solPotenList.begin(); iterP2 != tempMy.solPotenList.end(); iterP2++)//本方单潜力点
					pointList.push_back(*iterP2);
				for (iterP2 = tempDen.solThreatList.begin(); iterP2 != tempDen.solThreatList.end(); iterP2++)//对方单威胁点
					pointList.push_back(*iterP2);
				for (iterP2 = tempDen.duoPotenList.begin(); iterP2 != tempDen.duoPotenList.end(); iterP2++)//对方双潜力点
					pointList.push_back(*iterP2);
			}
			if (pointList.size() == 0)
			{
				for (iterP2 = tempMy.toDuoTwoList.begin(); iterP2 != tempMy.toDuoTwoList.end(); iterP2++)//本方潜双潜力点
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
	if (myInfo.solThreatList.size() > 0)//单威胁点
	{
		for (iterP = myInfo.solThreatList.begin(); iterP != myInfo.solThreatList.end(); iterP++)
		{
			tempStep.first = *iterP;
			MakeMove(tempStep.first, tempLine, TODUOTHREAT | TOSOLTHREAT);
			tempMy = GetBoardInfo(side, TODUOTHREAT | TOSOLTHREAT);
			BackMove(tempStep.first, tempLine, side);
			if (tempMy.duoThreatList.size() > 0)//双威胁点
			{
				for (iterP2 = tempMy.duoThreatList.begin(); iterP2 != tempMy.duoThreatList.end(); iterP2++)//本方双威胁点
				{
					tempStep.second = *iterP2;
					stepList.push_back(tempStep);
				}
			}
			if (tempMy.solThreatList.size() > 0)//另一个单威胁点
			{
				for (iterP2 = tempMy.solThreatList.begin(); iterP2 != tempMy.solThreatList.end(); iterP2++)//本方双威胁点
				{
					tempStep.second = *iterP2;
					stepList.push_back(tempStep);
				}
			}
		}
	}
	if (myInfo.duoPotenList.size() > 0)//存在双潜力点
	{
		for (iterP = myInfo.duoPotenList.begin(); iterP != myInfo.duoPotenList.end(); iterP++)
		{
			tempStep.first = *iterP;
			MakeMove(tempStep.first, tempLine, side, TODUOTHREAT);
			tempMy = GetBoardInfo(side, TODUOTHREAT);//生成双威胁步
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
