#include "Seach.h"


BoardCode boardCode;//棋盘状态编码

int SearchDepth = DEPTH;//规定搜索深度
int MaxDepth = MAXDEPTH;//最大搜索深度


/**
 * nega-alpha-beta - 负极大值搜索，通过alpha-beta剪枝进行优化。alpha-beta剪枝是一个缩小窗口的过程，被剪枝的状态必是其值超出窗口范围
 * @return:	返回搜索得出的局面潜能特征值（局面价值）
 * @side:		模拟行棋方执棋颜色
 * @alpha:		指导剪枝的区间下限
 * @beta:		指导剪枝的区间上限
 * @depth:		当前搜索深度
 */
int nega_alpha_beta(BYTE side, int alpha, int beta, const int depth)
{
	int unside = 1 - side;
	LineInfo tempLine[2][4], tempLine2[2][4];
	vector<Step>::iterator iterS;
	HashInfo *hashT, hashP;//哈希节点
#ifdef DEBUGVALUE
	Step select;
#endif

	if (findEndLib(boardCode))//锁定残局
	{
#ifdef DEBUGVALUE
		debugger.OutputMessage("info: 锁定残局!", 0);
#endif
		return depth - WINLOSE;
	}

	hashT = findHash(boardCode);//提取历史
	if (hashT != NULL)//如果存在历史，优先直接调用历史
	{
		//如果存在历史，且时间戳与当前一致（必是常规本轮搜索最新结果），则直接返回表中保存的博弈值
		//同一棋局状态向下展开的博弈树子树相同（所处深度恒定），博弈值是由下向上建立的，alpha-beta窗口向前继承（beta单调递减）
		if (hashT->timestamp == HandNum)
			return hashT->value;
		else
			hashP = *hashT;
	}
	else//不存在历史则分配表项并插入哈希表
	{
		hashP.code = boardCode;//赋值表项唯一标识
		hashP.cut = true;//初始化为true，表明经过剪枝
		hashP.full = false;//初始化为false，表明不具有完整着法列表
		hashP.denType = GetBoardType(unside);//获取反方威胁类型
	}

	//递归出口
	//在进入搜索的必要条件为对方未取胜，而在搜索中若发现己方可胜则返回，故递归的终止条件不包括对方已胜
	//在搜索过程中，己方可胜的条件是对方未能消除己方全部的威胁线型，而该条件会导致对方不能产生着法，故递归终止条件不包含己方可胜
	if (depth >= SearchDepth)//搜索到限制深度
	{
		//达到最大搜索深度计算评估值
		//GetBoardValue(side) * 0.85 - GetBoardValue(unside)
		hashP.value = GetBoardValue(side) * 17 / 20 - GetBoardValue(unside);
		//此时并不会产生着法列表，不应插入置换表
		return hashP.value;
	}

	//生成全部着法
	if (hashP.full == false)//当哈希表项的full字段值为false时，证明该表项的stepList字段值不完整，不能被直接利用。
	{
		if (hashP.denType == 1)//受到单威胁
			hashP.stepList = MakeStepListForDefendSingle(side, 20);
		else if (hashP.denType > 1)//受到双威胁，多威胁，伪双威胁，伪多威胁（双威胁线型最大生成4个着法，为了防止伪双应与单威胁保持一致）
			hashP.stepList = MakeStepListForDefendDouble(side, 20);
		else//处理无威胁情形（不存在4型和5型）
			hashP.stepList = MakeStepListForNone(side, 23);
		hashP.full = true;
	}
	else//上一轮历史启发
	{
		if (hashP.cut == false)//cut字段为false(未剪枝)，证明stepList中每一项的value评价标准一致。此时可以对stepList中的着法进行重新排序，以提高剪枝效率
		{
			sort(hashP.stepList.begin(), hashP.stepList.end(), cmpStepValue);
			hashP.cut = true;
		}
	}
	if (hashP.stepList.size() == 0)//因为不可破解局面，不能生成着法
	{
#ifdef DEBUGVALUE
/*		if (hashP.denType == 2 || hashP.denType == 10)
		{
			debugger.OutputMessage("处理双单威胁出错！", side);
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
		moveCodeS(boardCode, *iterS, side);//走子
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
		moveCodeS(boardCode, *iterS, side);//恢复
		if (alpha < iterS->value)
		{
			alpha = iterS->value;
#ifdef DEBUGVALUE
			select = *iterS;
#endif
			//max层用beta剪枝
			if (beta <= alpha || iterS->value >= WINLOSE - MaxDepth)//只要有一个着法可以获胜，将必胜
			{
				hashP.value = alpha;
				updateHash(hashP);
				return hashP.value;
			}
		}
	}
	hashP.cut = false;//未被剪枝
	hashP.value = alpha;
	updateHash(hashP);
#ifdef DEBUGVALUE
	debugger.OutSelect(select);
#endif
	return hashP.value;
}

/**
 * TSS - 威胁空间搜索，通过DTSS/STSS混合策略加深搜索
 * @return:		返回搜索得出的局面潜能特征值（局面价值）
 * @side:		模拟行棋方执棋颜色
 * @alpha:		指导剪枝的区间下限
 * @beta:		指导剪枝的区间上限
 * @depth:		当前搜索深度
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
		debugger.OutputMessage("info: 锁定残局!", 0);
#endif
		return depth - WINLOSE;
	}

	denType = GetBoardType(unside);

	//递归出口
	if (depth >= MaxDepth)//到达搜索最大深度进行返回
		return GetBoardValue(side) - GetBoardValue(unside);

	//扩展搜索中不需要保存着法列表，因其不完整性，在常规搜索中将不会被引用
	if (denType == 1)//如果受到单威胁，触发连续单威胁搜索
		stepList = MakeStepListForDefendSingleEx(side, 6);
	else if (denType > 1)//受到双威胁，必进行破招，属于双迫招搜索的一部分（双威胁线型最大生成4个着法，为了防止伪双应与单威胁保持一致）
		stepList = MakeStepListForDefendDoubleEx(side, 6);
	else//无威胁时寻找发起 双破招搜索 的机会
		stepList = MakeStepListForDouble(side, 8);

#ifdef DEBUGVALUE
	debugger.OutputStep(stepList, side);
#endif
	if (stepList.size() == 0)
	{
		if (denType > 10 || (denType > 2 && denType < 10))//多威胁不可破解
			return depth - WINLOSE;
		else//不满足加深搜索条件
			//GetBoardValue(side) * 0.85 - GetBoardValue(unside)
			return GetBoardValue(side) * 17 / 20 - GetBoardValue(unside);
	}

	for (iterS = stepList.begin(); iterS != stepList.end(); iterS++)
	{
		moveCodeS(boardCode, *iterS, side);//走子
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
		moveCodeS(boardCode, *iterS, side);//恢复
		if (alpha < iterS->value)
		{
			alpha = iterS->value;
			if (beta <= alpha || iterS->value >= WINLOSE - MaxDepth)//只要有一个着法可以获胜，将必胜
				return alpha;
		}
	}
	if (alpha <= MaxDepth - WINLOSE)
	{
		if (denType >= 2)
		{
			InsertEndLib(boardCode);
#ifdef DEBUGVALUE
			debugger.OutputMessage("info: 习得残局!", 0);
#endif
		}
		else//不是由双威胁引导出的必败不是真必败
			return MaxDepth - WINLOSE + 1;
	}

	return alpha;
}

/**
 * ExtendSeach - 扩展负极大值搜索，通过TSS加深搜索，使局面到达一个平稳状态再行估值
 * @return:		返回搜索得出的局面潜能特征值（局面价值）
 * @side:		模拟行棋方执棋颜色
 * @alpha:		指导剪枝的区间下限
 * @beta:		指导剪枝的区间上限
 * @depth:		当前搜索深度
 */
int ExtendSeach(BYTE side, int alpha, int beta, const int depth)
{
	int unside = 1 - side;
	LineInfo tempLine[2][4], tempLine2[2][4];
	vector<Step>::iterator iterS;
	HashInfo *hashT, hashP;//哈希节点
#ifdef DEBUGVALUE
	Step select;
#endif

	if (findEndLib(boardCode))//锁定残局，残局为必败局，从残局可以导出必胜局
	{
#ifdef DEBUGVALUE
		debugger.OutputMessage("info: 锁定残局!", 0);
#endif
		return depth - WINLOSE;
	}

	hashT = findHash(boardCode);//提取置换表中历史
	if (hashT != NULL)//如果存在历史，优先直接调用历史
	{
		//同一棋局状态向下展开的博弈树子树相同（所处深度恒定），博弈值是由下向上建立的，alpha-beta窗口向前继承（beta单调递减）
		if (hashT->timestamp == HandNum)//如果存在历史，且时间戳与当前一致（必是常规本轮搜索最新结果），则直接返回表中保存的博弈值
			return hashT->value;
		else
			hashP = *hashT;
	}
	else//不存在历史则分配表项并插入哈希表
	{
		hashP.code = boardCode;//赋值表项唯一标识
		hashP.cut = true;//初始化为true，表明经过剪枝
		hashP.full = false;//初始化为false，表明不具有完整着法列表
		hashP.denType = GetBoardType(unside);//获取反方威胁类型
	}

	//递归出口
	//在进入搜索的必要条件为对方未取胜，而在搜索中若发现己方可胜则返回，故递归的终止条件不包括对方已胜
	//在搜索过程中，己方可胜的条件是对方未能消除己方全部的威胁线型，而该条件会导致对方不能产生着法，故递归终止条件不包含己方可胜
	if (depth >= SearchDepth)//搜索到限制深度，进行加深搜索
	{
		hashP.value = TSS(side, alpha, beta, depth);//同一结点搜索
		if (hashP.value > MaxDepth - WINLOSE && hashP.value < WINLOSE - MaxDepth)//不能识别胜负 
			//hashP.value * 0.4 + (GetBoardValue(side) * 0.85 - GetBoardValue(unside)) * 0.6
			hashP.value = (hashP.value * 40 + GetBoardValue(side) * 51  - GetBoardValue(unside) * 60) / 100;
		//此时并不会产生着法列表，不应插入置换表，否则在下一轮时将因为无可利用着法而出错
		return hashP.value;
	}

	//生成全部着法
	if (hashP.full == false)//当哈希表项的full字段值为false时，证明该表项的stepList字段值不完整，不能被直接利用。
	{
		if (hashP.denType == 1)//受到单威胁
			hashP.stepList = MakeStepListForDefendSingle(side, 20);
		else if (hashP.denType >= 2)//受到双威胁，多威胁，伪双威胁，伪多威胁（双威胁线型最大生成4个着法，为了防止伪双应与单威胁保持一致）
			hashP.stepList = MakeStepListForDefendDouble(side, 20);
		else//处理无威胁情形（不存在4型和5型）
			hashP.stepList = MakeStepListForNone(side, 23);
		hashP.full = true;
	}
	else//上一轮历史启发
	{
		if (hashP.cut == false)//cut字段为false(未剪枝)，证明stepList中每一项的value评价标准一致。此时可以对stepList中的着法进行重新排序，以提高剪枝效率
		{
			sort(hashP.stepList.begin(), hashP.stepList.end(), cmpStepValue);
			hashP.cut = true;
		}
	}
	if (hashP.stepList.size() == 0)//因为不可破解局面，不能生成着法（不可入分支？？）
	{
#ifdef DEBUGVALUE
/*		if (hashP.denType == 2 || hashP.denType == 10)
		{
			debugger.OutputMessage("处理双单威胁出错！", side);
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
		moveCodeS(boardCode, *iterS, side);//走子
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
		moveCodeS(boardCode, *iterS, side);//恢复
		if (alpha < iterS->value)
		{
			alpha = iterS->value;
#ifdef DEBUGVALUE
			select = *iterS;
#endif
			//max层用beta剪枝
			if (beta <= alpha || iterS->value >= WINLOSE - MaxDepth)//只要有一个着法可以获胜，将必胜
			{
				hashP.value = alpha;
				updateHash(hashP);
				return hashP.value;
			}
		}
	}
	hashP.cut = false;//未被剪枝
	hashP.value = alpha;
	if (alpha <= MaxDepth - WINLOSE)
	{
		if (hashP.denType >= 2)
		{
			InsertEndLib(boardCode);
#ifdef DEBUGVALUE
			debugger.OutputMessage("info: 习得残局!", 0);
#endif
		}
		else//不是由双威胁引导出的必败不是真必败
			hashP.value = MaxDepth - WINLOSE + 1;
	}
	updateHash(hashP);
#ifdef DEBUGVALUE
	debugger.OutSelect(select);
#endif
	return hashP.value;
}

/**
* SeachValuavleStep - 搜索最佳招法，搜索引擎入口
* @return:		返回引擎决策的最佳着法
* @side:		棋手执棋颜色
* @seachDepth:	搜索引擎对博弈树进行展开深度
*/
Step SeachValuableStep(Step denStep, BYTE side)
{
	Step step = { { -1, -1 }, { -1, -1 }, -WINLOSE };//初始化为弃子，防止出现返回乱码
	int alpha = -WINLOSE, unside = 1 - side, myType, denType = GetBoardType(unside);
	LineInfo tempLine[2][4], tempLine2[2][4];
	vector<Step> stepList;
	vector<Step>::iterator iterS;
	HashInfo *hashP;

	//向历史哈希表中查找
	hashP = findHash(boardCode);
	if (hashP != NULL && hashP->full == true)//如果存在历史,且具有完整着法列表，则直接调用历史走法表
	{
		if (hashP->cut == false)//cut字段为false，证明stepList中每一项的value评价标准一致。此时可以对stepList中的着法进行重新排序，以提高剪枝效率
			sort(hashP->stepList.begin(), hashP->stepList.end(), cmpStepValue);
		stepList = hashP->stepList;
#ifdef DEBUGVALUE
		debugger.OutputMessage("get step list from hashList", side);
#endif
	}
	else
	{
		if (HandNum == 2)//第二手比较特殊，故使用人为规定，贴着先手进行落子
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
			if (myType > 0)//若本方能对对方构成威胁，这是可胜局面，生成必胜步（点）
				stepList = MakeStepListForWin(side, 1);
			else if (denType == 1)//受到单威胁
				stepList = MakeStepListForDefendSingle(side, 20);
			else if (denType > 1)//受到双威胁，多威胁，伪双威胁，伪多威胁
				stepList = MakeStepListForDefendDouble(side, 18);
			else//处理无威胁情形（不存在4型和5型）
				stepList = MakeStepListForNone(side, 23);
		}
	}
	debugger.OutputStep(stepList, side);
	if (stepList.size() == 0)//必须返回一个着法，不能弃子
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
				LineInfo tempLine[2][4];//临时线信息备份
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
			if (HandNum <= 6)//因为开局阶段（前6手）可利用信息过少，不宜进行扩展搜索
				iterS->value = -nega_alpha_beta(unside, -WINLOSE, -alpha, 1);//负极大值搜索alpha-beta剪枝求局面价值
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
				if (alpha >= WINLOSE - MaxDepth)//可获胜步直接返回
					break;
			}
		}
	}
	debugger.OutputStep(stepList, side);
	debugger.OutSelect(step);
	return step;
}

/**
 * sixgo_carry - 六子棋博弈引擎核心，引擎工作入口
 * @return：		返回引擎计算得出的着法
 * @moveStep:	对手行棋着法
 * @side:		棋手执棋颜色
 */
Step sixgo_carry(const Step moveStep, const int nBoard[19][19], const BYTE side)
{
	int i, j;
	Point point;
	//复制虚拟棋盘
	initialHashCode(boardCode);
	for (i = 0; i < edge; i++)
		for (j = 0; j < edge; j++)
		{
			if (nBoard[i][j] != EMPTY)
			{
				point.x = i; point.y = j;
				moveCodeP(boardCode, point, nBoard[i][j]);//进行哈希编码
			}
			virtualBoard[i][j] = nBoard[i][j];
		}
	debugger.InitDebugger(HandNum, moveStep);
	//搜索最佳招法
	Step step = SeachValuableStep(moveStep, side);
	debugger.BackMove(step);
	return step;
}
