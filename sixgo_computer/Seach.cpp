#include "Seach.h"


//int LineTypeValue[WIN+1]={0, 0, 0,0,0,1, 3,4,5,9, 8,12,15,16,20,31,33, 39,45,47,54,89,100,1000, 37,76,1000, 10000 };//线型价值表
//                             0| 1| 2  3  4  5| 6  7  8   9| 10 *11  12  13 *14  15 *16| 17  18   19  *20   21  *22    *23| 24   25    *26|     27
int LineTypeValue[WIN + 1] = { 0, 0, 1, 1, 1, 3, 5, 6, 8, 11, 15, 18, 20, 24, 28, 31, 39, 75, 90, 110, 115, 186, 236, 99999, 94, 196, 99999, 100000 };//线型价值表
//无价值，|0潜潜双潜力，1潜单潜力，2潜弱双潜力，3潜双潜力，4潜多潜力，5单潜力，
//         6弱双潜力，7双潜力，8多潜力，9单威胁，10弱双威胁，11双威胁|，多威胁，已胜
//             潜多潜力16->多潜力100->多威胁1000->已胜
//潜潜双潜力3->潜双潜力11->双潜力31->双威胁155->已胜10000
//            潜弱双潜力7->弱双潜力19->弱双威胁95
//             潜单潜力5->单潜力15->单威胁75->已胜

int LineTypeThreat[WIN + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 10, 10, 100, 1, 10, 100, 1000 };//线型威胁类型表：1单威胁、10双威胁、100三威胁、1000已胜
int LineTypeType[WIN + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 10, 10, 11, 11, 10, 100, 100, 1000, 1100, 1100, 1000, 2000, 2000, 3000, 1000, 2000, 3000, 10000 };//线型类型表

int virtualBoard[edge][edge];//虚拟棋盘
BoardCode boardCode;//棋盘编码
LineInfo lineInfo[2][92];//线的信息表

int SearchDepth = DEPTH;//规定搜索深度
int MaxDepth = MAXDEPTH;//最大搜索深度

/**
 * addHash - 插入置换表
 * @data:	待插入数据
 */
inline void addHash(HashInfo &data)
{
	unsigned long hash = hashCode(data.code);//直接地址映射
	//新加入点的时间戳必然较新
	hashList[hash] = data;
}

/**
 * sixgo_carry - 六子棋博弈引擎核心，引擎工作入口
 * @return：	返回引擎计算得出的着法
 * @moveStep:	对手行棋着法
 * @side:		棋手执棋颜色
 */
Step sixgo_carry(const Step moveStep, const int nBoard[19][19], const BYTE side)
{
	int i, j;
	Point point;
	//复制虚拟棋盘
	initialCode(boardCode);
	for (i = 0; i < edge; i++)
		for (j = 0; j < edge; j++)
		{
			if (nBoard[i][j] != 2)
			{
				point.x = i; point.y = j;
				moveCodeP(boardCode, point, nBoard[i][j]);//进行哈希编码
			}
			virtualBoard[i][j] = nBoard[i][j];
		}
	debugger.InitDebugger(HandNum, moveStep);
	//搜索最佳招法
	Step step = SeachValuableStep(side);
	debugger.BackMove(step);
	return step;
}

/**
 * SeachValuavleStep - 搜索最佳招法，搜索引擎入口
 * @return:		返回引擎决策的最佳着法
 * @side:			棋手执棋颜色
 * @seachDepth:	搜索引擎对博弈树进行展开深度
 */
Step SeachValuableStep(BYTE side)
{
	Step step = { { -1, -1 }, { -1, -1 }, -WINLOSE };//初始化为弃子，防止出现返回乱码
	int myType, denType;
	LineInfo tempLine[2][4], tempLine2[2][4];
	vector<Step> stepList;
	vector<Step>::iterator iterS;
	int alpha = -WINLOSE, unside = 1 - side;
	HashInfo *hashP;

	//向历史哈希表中查找
	hashP = findHash(boardCode);
	if (hashP != NULL && hashP->full == true)//如果存在历史,且具有完整着法列表，则直接调用历史走法表
	{
		if (hashP->cut == false)//cut字段为false，证明stepList中每一项的value评价标准一致。此时可以对stepList中的着法进行重新排序，以提高剪枝效率
			sort(hashP->stepList.begin(), hashP->stepList.end(), cmpStepValue);
		stepList = hashP->stepList;
	}
	else
	{
		myType = GetBoardType(side);
		denType = GetBoardType(unside);
		if (myType > 0)//若本方能对对方构成威胁，这是可胜局面，生成必胜步（点）
			stepList = MakeStepListForWin(side, 1);
		else if (denType == 1)//受到单威胁
			stepList = MakeStepListForDefendSingle(side, 20);
		else if (denType > 1)//受到双威胁，多威胁，伪双威胁，伪多威胁
			stepList = MakeStepListForDefendDouble(side, 18);
		else//处理无威胁情形（不存在4型和5型）
			stepList = MakeStepListForNone(side, 23);
	}
	debugger.OutputStep(stepList, side);
	if (stepList.size() == 0)//必须返回一个着法，不能弃子
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
			iterS->value = -nega_alpha_beta(unside, -WINLOSE, -alpha, 1);//负极大值搜索alpha-beta剪枝求局面价值
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
	debugger.OutSelect(step);
	return step;
}

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
	Step select;

	if (findEndLib(boardCode))
		return WINLOSE - depth;

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
		hashP.myType = GetBoardType(side);//获取本方威胁类型
		hashP.denType = GetBoardType(unside);//获取反方威胁类型
	}
	hashP.timestamp = HandNum;//更新哈希表项时间戳

	//递归出口
	//在进入搜索的必要条件为对方未取胜，而在搜索中若发现己方可胜则返回，故递归的终止条件不包括对方已胜
	if (hashP.myType > 0)//存在威胁对方的线型
	{
		InsertEndLib(boardCode);
		hashP.value = WINLOSE - depth;
		addHash(hashP);
		return hashP.value;
	}
	else if (depth >= SearchDepth)//搜索到限制深度
	{
		if (HandNum <= 6)
		{
			//达到最大搜索深度直接返回，不进行扩展搜索
			hashP.value = GetBoardValue(side)*0.85 - GetBoardValue(unside);
			addHash(hashP);
			return hashP.value;
		}
		else
		{
			//扩展搜索
			hashP.value = ExtendSeach(side, alpha, beta, depth);
			if (hashP.value > MaxDepth - WINLOSE && hashP.value < WINLOSE - MaxDepth)
				hashP.value = hashP.value*0.4 + (GetBoardValue(side)*0.85 - GetBoardValue(unside))*0.6;
			addHash(hashP);
			return hashP.value;
		}
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
	else
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
		if (hashP.denType == 2 || hashP.denType == 10)
		{
			debugger.OutputMessage("处理双单威胁出错！", side);
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
			if (iterS->value >= WINLOSE - MaxDepth)//只要有一个着法可以获胜，将必胜
			{
				InsertEndLib(boardCode);
				return iterS->value;
			}
			alpha = iterS->value;
			select = *iterS;
			if (beta <= alpha)//max层用beta剪枝
			{
				hashP.value = alpha;
				addHash(hashP);
				return hashP.value;
			}
		}
	}
	hashP.cut = false;//未被剪枝
	hashP.value = alpha;
	addHash(hashP);
#ifdef DEBUGVALUE
	debugger.OutSelect(select);
#endif
	return hashP.value;
}

/**
 * ExtendSeach - 扩展负极大值搜索，通过DTSS/STSS进行优化，或使局面到达一个平稳状态再行估值
 * @return:	返回搜索得出的局面潜能特征值（局面价值）
 * @side:		模拟行棋方执棋颜色
 * @alpha:		指导剪枝的区间下限
 * @beta:		指导剪枝的区间上限
 * @depth:		当前搜索深度
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

	//递归出口
	if (myType > 0)//存在威胁对方的线型
	{
		InsertEndLib(boardCode);
		return WINLOSE - depth;
	}
	else if (depth >= MaxDepth)//到达搜索最大深度进行返回
		return GetBoardValue(side) - GetBoardValue(unside);

	//扩展搜索中不需要保存着法列表，因其不完整性，在常规搜索中将不会被引用
	if (denType == 1)//如果受到单威胁，触发连续单威胁搜索
		stepList = MakeStepListForDefendSingleEx(side, 6);
	else if (denType > 1)//受到双威胁，必进行破招，属于双迫招搜索的一部分（双威胁线型最大生成4个着法，为了防止伪双应与单威胁保持一致）
		stepList = MakeStepListForDefendDoubleEx(side, 6);
	else//无威胁时寻找发起 双破招搜索 的机会
		stepList = MakeStepListForDouble(side, 8);

	if (stepList.size() == 0)
	{
		if (denType > 10 || (denType > 2 && denType < 10))//多威胁不可破解
			return depth - WINLOSE;
		else//不满足加深搜索条件
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
			if (val >= WINLOSE - MaxDepth)//只要有一个着法可以获胜，将必胜
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
 * UpdataBoard - 通过招法局部更新棋局信息，即线的信息
 * @return:		无返回值
 * @step:		行棋着法
 */
void UpdataBoard(Step step)
{
	UpdateLineForCross(step.first, BLACK);
	UpdateLineForCross(step.first, WHITE);
	UpdateLineForCross(step.second, BLACK);
	UpdateLineForCross(step.second, WHITE);
}

/**
 * UpdateLineForCross - 通过点更新线的信息
 * @return:		无返回值
 * @point:		落子点
 * @side:		点所属方的执棋颜色
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
 * AnalyzeLine - 通过线的起始点和方向分析线
 * @return:		返回线信息
 * @start:		线的起始点绝对坐标
 * @lineDirec:	线的方向标志
 * @side:		分析的线信息所属方执棋颜色
 */
LineInfo AnalyzeLine(Point start, BYTE lineDirec, BYTE side, int tag)
{
	int shapeIndex = 0;	// 线型索引
	int x = start.x;	// 从起点开始找
	int y = start.y;
	Point LinePos;		//被分析的线形的起点绝对坐标。用-1表示还未确定出被分析线形的起点（棋盘的有效点的坐标值都不小于0）。
	int len = 0;		//记录当前位置在所要求的段中的位置, 可以理解为当前段长
	int value = 0;
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
 * ValueType - 分析指定线型
 * @return:	返回线信息，包括：线形威胁类型、估值，防御步、防御点、威胁点、潜力点、潜潜力点的绝对坐标。
 * @style：	线型索引，由Analizeline函数提取的棋形的二进制描述。
 * @start:		线型的起始点绝对坐标
 * @lineDirec:	线型所在线的方向
 */
LineInfo ValuateType(int style, Point start, BYTE lineDirec, int tag)
{
	int i, len;	//len:线型的长度。
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

	for (i = 19; i >= 0; i--)//计算style的有效二进制位数，即线型长度。至少为6位，这是由AnalyzeLine函数决定的。
		if (GetABit(style, i))
		{
			len = i; break;
		}

	if (ShapeStyleId >= THREAT_four_SINGLE)//威胁型
	{
		if (tag&TODEFENT)
		{
			if (linetypeInfo[style].defStepList.size() != 0)//防御步
				for (iterS = linetypeInfo[style].defStepList.begin(); iterS != linetypeInfo[style].defStepList.end(); iterS++)
				{
					tempStep.first.x = start.x + iterS->first*lineVector[lineDirec][0];
					tempStep.first.y = start.y + iterS->first*lineVector[lineDirec][1];
					tempStep.second.x = start.x + iterS->second*lineVector[lineDirec][0];
					tempStep.second.y = start.y + iterS->second*lineVector[lineDirec][1];
					lineInfo.defStepList.push_back(tempStep);
				}
			if (linetypeInfo[style].defPointList.size() != 0)//防御点
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
 * GetBoardType - 收集棋局威胁类型信息
 * @side	应收集信息的棋手方标志
 */
int GetBoardType(BYTE side)
{
	int i, type = 0;
	for (i = 0; i < 92; i++)//查找92条线
		type += lineInfo[side][i].LineType;//累加线的威胁类型作为局面的威胁类型
	return type;
}

/**
 * GetBoardValue - 收集棋局威胁类型信息
 * @side	应收集信息的棋手方标志
 */
int GetBoardValue(BYTE side)
{
	int i, value = 0;
	for (i = 0; i < 92; i++)//查找92条线
		value += lineInfo[side][i].value;//累加线的价值作为局面价值
	return value;
}

/**
 * GetBoardInfo - 收集棋局信息
 * @side	应收集信息的棋手方标志
 * @tag	收集信息策略按位标志，除1标志外棋局价值和威胁类型为默认收集信息
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
			if (lineInfo[side][i].defPointList.size() != 0)
				for (iterP = lineInfo[side][i].defPointList.begin(); iterP != lineInfo[side][i].defPointList.end(); iterP++)
					tempSynInfo.defPointList.push_back(*iterP);
			if (lineInfo[side][i].defStepList.size() != 0)
				for (iterS = lineInfo[side][i].defStepList.begin(); iterS != lineInfo[side][i].defStepList.end(); iterS++)
					tempSynInfo.defStepList.push_back(*iterS);
		}
		if ((tag&TOWIN) && lineInfo[side][i].winList.size() != 0)//致胜点
			tempSynInfo.winList.push_back(lineInfo[side][i].winList[0]);
		if ((tag&TOWILLWIN) && lineInfo[side][i].willWinList.size() != 0)//即将胜利点
			tempSynInfo.willWinList.push_back(lineInfo[side][i].willWinList[0]);
		if ((tag&TODUOTHREAT) && lineInfo[side][i].duoThreatList.size() != 0)//双威胁点
			for (iterP = lineInfo[side][i].duoThreatList.begin(); iterP != lineInfo[side][i].duoThreatList.end(); iterP++)
				tempSynInfo.duoThreatList.push_back(*iterP);
		if ((tag&TOSOLTHREAT) && lineInfo[side][i].solThreatList.size() != 0)//单威胁定
			for (iterP = lineInfo[side][i].solThreatList.begin(); iterP != lineInfo[side][i].solThreatList.end(); iterP++)
				tempSynInfo.solThreatList.push_back(*iterP);
		if ((tag&TODUOPOTEN) && lineInfo[side][i].duoPotenList.size() != 0)//双潜力点
			for (iterP = lineInfo[side][i].duoPotenList.begin(); iterP != lineInfo[side][i].duoPotenList.end(); iterP++)
				tempSynInfo.duoPotenList.push_back(*iterP);
		if ((tag&TOSOLPOTEN) && lineInfo[side][i].solPotenList.size() != 0)//单潜力点
			for (iterP = lineInfo[side][i].solPotenList.begin(); iterP != lineInfo[side][i].solPotenList.end(); iterP++)
				tempSynInfo.solPotenList.push_back(*iterP);
		if ((tag&TOCOMMON) && lineInfo[side][i].toDuoTwoList.size() != 0)//普通点
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


//招法生成器
//=====================================================

/**
 * MakeStepListForWin - 致胜招法生成器
 * @return:	返回着法列表
 * @myInfo:	执棋方当前局面综合信息
 * @denInfo:	对方当前局面综合信息
 * @limit:		生成着法的数量限制
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
 * @return:	返回着法列表
 * @myInfo:	执棋方当前局面综合信息
 * @denInfo:	对方当前局面综合信息
 * @limit:		生成着法的数量限制
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
		pointList.resize(0);
	}
	for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)//加入由THREAT_four_ADDITION引起的额外破解步
	{
		iterS->value = CalculateStepValue(*iterS, side);
		stepList.push_back(*iterS);
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
	return stepList;
}

/**
 * MakeStepListForDefendDouble - 防御双威胁等招法生成器
 * @return:	返回着法列表
 * @myInfo:	执棋方当前局面综合信息
 * @denInfo:	对方当前局面综合信息
 * @limit:		生成着法的数量限制
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
		stepList.resize(0);
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
			if (stepList.size() != 0)//可破解
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
					pointList.resize(0);
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
			if (stepList.size() != 0)//可破解
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
 * MakeStepListForNone - 平静着法生成器
 * @return:		返回着法列表
 * @myInfo:		执棋方当前局面综合信息
 * @denInfo:	对方当前局面综合信息
 * @limit:		生成着法的数量限制
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

	//先选取本方的<双威胁点，双潜力点，单威胁点，单潜力点>，对方<双威胁点，双潜力点，单威胁点> 作为第一批点进行试走
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
	if (pointList.size() < 5)//若第一批点少于5个，首先加入本方潜双潜力点
	{
		if (myInfo.toDuoTwoList.size() > 0)
		{
			for (iterP = myInfo.toDuoTwoList.begin(); iterP != myInfo.toDuoTwoList.end(); iterP++)
				pointList.push_back(*iterP);
		}
		else if (pointList.size() == 0)//再加入对方单潜力点和潜双潜力点
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
		pointList2.resize(0);
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
 * @return:		返回着法列表
 * @myInfo:		执棋方当前局面综合信息
 * @denInfo:	对方当前局面综合信息
 * @limit:		生成着法的数量限制
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
		//		UniquePoint(pointList);//去重操作
		BackMove(tempStep.first, tempLine, side);
		for (iterP2 = pointList.begin(); iterP2 != pointList.end(); iterP2++)
		{
			tempStep.second = *iterP2;
			tempStep.value = CalculateStepValue(tempStep, side);
			stepList.push_back(tempStep);
		}
		pointList.resize(0);
	}
	for (iterS = denInfo.defStepList.begin(); iterS != denInfo.defStepList.end(); iterS++)//加入由THREAT_four_ADDITION引起的额外破解步
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
 * MakeStepListForDefendDoubleEx - 防御双威胁等着法扩展生成器，用于扩展搜索
 * @return:	返回着法列表
 * @myInfo:	执棋方当前局面综合信息
 * @denInfo:	对方当前局面综合信息
 * @limit:		生成着法的数量限制
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
		stepList.resize(0);
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
			if (stepList.size() != 0)//可破解
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
					pointList.resize(0);
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
			if (stepList.size() != 0)//可破解
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
				stepList.resize(0);
				tempStep.second = tempMy.duoThreatList[0];
				tempStep.value = WINLOSE - 2;
				stepList.push_back(tempStep);
				BackMove(tempStep.first, tempLine, side);
				return stepList;
			}
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
			pointList.resize(0);
		}
	}
	if (myInfo.solThreatList.size() > 0)//存在单威胁&&双潜力点
	{
		for (iterP = myInfo.solThreatList.begin(); iterP != myInfo.solThreatList.end(); iterP++)
		{
			tempStep.first = *iterP;
			MakeMove(tempStep.first, tempLine, TODUOTHREAT);
			tempMy = GetBoardInfo(side, TODUOTHREAT);
			BackMove(tempStep.first, tempLine, side);
			if (tempMy.duoThreatList.size() != 0)
			{
				for (iterP2 = tempMy.duoThreatList.begin(); iterP2 != tempMy.duoThreatList.end(); iterP2++)//本方双威胁点
				{
					tempStep.second = *iterP2;
					stepList.push_back(tempStep);
				}
			}
			pointList.resize(0);
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


//备份线信息，针对点所在的4条线，将其信息储存到临时变量中
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
//恢复线信息，针对点所在的4条线，用临时变量中的信息覆盖当前信息
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
 * MakeMove - 试走，同时对变更的线进行备份
 * @point:	落子点坐标
 * @tempLine:	线信息备份储存器
 * @side:	执棋颜色
 * @tag:	标签
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
 * BackMove - 取消试走，同时恢复变更的线信息
 * @point:	试走点坐标
 * @tempLine:	备份信息储存器
 * @side:	执棋颜色
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
 * CalSingleLineValue - 线价值计算器
 * @return:	返回线的价值
 * @start:		线的起始点坐标
 * @Direct:	线的方向标志
 * @side:		被计算的线的价值信息所属方的执棋颜色
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
 * @return:	返回线的类型，属性成分
 * @start:		线的起始点坐标
 * @Direct:	线的方向标志
 * @side:		被计算的线的类型信息所属方的执棋颜色
 */
int CalSingleLineType(const Point start, const BYTE Direc, const BYTE side) //计算点所在Direc方向的线的价值
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
				value += LineTypeType[preTable[shapeIndex]];
			}
			shapeIndex = 0;	// 偏移量归零
			len = 0;		// 长度归零
		}
		Increment(x, y, Direc);
	}
	if (len > 5)	//长度超过5个才会重视。
	{
		shapeIndex += (1 << len);	  // 得到的棋型
		value += LineTypeType[preTable[shapeIndex]];
	}
	return value;
}


#define USEVALUE

/**
 * CalculateStepValue - 着法价值计算器
 * @return:	返回着法价值
 * @step:		需要被计算的着法
 * @side:		着法所属方执棋颜色
 */
int CalculateStepValue(const Step step, const BYTE side)
#ifdef USEVALUE
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