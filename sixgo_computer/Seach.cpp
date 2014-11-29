#include "Seach.h"

//int LineTypeValue[WIN+1]={0, 0, 0,0,0,1, 3,4,5,9, 8,12,15,16,20,31,33, 39,45,47,54,89,100,1000, 37,76,1000, 10000 };//���ͼ�ֵ��
int LineTypeValue[WIN+1]={0, 0, 1,1,1,3, 5,6,8,11, 15,18,20,24,28,31,39, 75,90,110,115,186,236,1000, 94,196,1000, 10000 };//���ͼ�ֵ��
int LineTypeThreat[WIN+1]={0, 0, 0,0,0,0, 0,0,0,0, 0,0,0,0,0,0,0, 1,1,1,1,10,10,100, 1,10,100, 1000 };//������в���ͱ�
int LineTypeType[WIN+1]={0, 0, 0,0,0,0, 0,0,0,1, 10,10,11,11,10,100,100, 1000,1100,1100,1000,2000,2000,3000, 1000,2000,3000, 10000 };//�������ͱ�
//�޼�ֵ��|0ǱǱ˫Ǳ����1Ǳ��Ǳ����2Ǳ��˫Ǳ����3Ǳ˫Ǳ����4Ǳ��Ǳ����5��Ǳ����
//         6��˫Ǳ����7˫Ǳ����8��Ǳ����9����в��10��˫��в��11˫��в|������в����ʤ
//             Ǳ��Ǳ��16->��Ǳ��100->����в1000->��ʤ
//ǱǱ˫Ǳ��3->Ǳ˫Ǳ��11->˫Ǳ��31->˫��в155->��ʤ10000
//            Ǳ��˫Ǳ��7->��˫Ǳ��19->��˫��в95
//             Ǳ��Ǳ��5->��Ǳ��15->����в75->��ʤ

int virtualBoard[edge][edge];//��������
LineInfo lineInfo[2][92];//�ߵ���Ϣ��
int SearchDepth=DEPTH,MaxDepth=MAXDEPTH;
BoardCode boardCode;


inline void addHash(HashInfo &data)
{
	long hash=hashCode(data.code);
	if(data.index>hashList[hash].index||data.depth<hashList[hash].depth)//ʱ��������������С�ı���Ǿɱ���
		hashList[hash]=data;
}

/**	sixgo_carry - �����岩��������ģ����湤�����
 *	@return��	�����������ó����ŷ�
 *	@moveStep:	���������ŷ�
 *	@side:		����ִ����ɫ
 */
Step sixgo_carry(Step moveStep,BYTE side)
{
	int i,j;
	Point point;
	//������������
	initialCode(boardCode);
	for (i = 0; i<edge; i++)
		for (j = 0; j<edge; j++)
		{
			if(nBoard[i][j]!=2)
			{
				point.x=i;point.y=j;
				moveCodeP(boardCode,point,nBoard[i][j]);//���й�ϣ����
			}
			virtualBoard[i][j]=nBoard[i][j];
		}
	//��������з�
	return SeachValuableStep(side);
}

/**	SeachValuavleStep - ��������з��������������
 *	@return:		����������ߵ�����ŷ�
 *	@side:			����ִ����ɫ
 *	@seachDepth:	��������Բ���������չ�����
 */
Step SeachValuableStep(BYTE side)
{
	Step step={{-1,-1},{-1,-1},0};//��ʼ��Ϊ���ӣ���ֹ���ַ�������
	int myType,denType;
	LineInfo tempLine[2][4],tempLine2[2][4];
	vector<Step> stepList;
	vector<Step>::iterator iterS;
	int value=-WINLOSE,val,unside=1-side;

	//����ʷ��ϣ���в���
	long hash=hashCode(boardCode);
	bool haveHis=compareCode(hashList[hash].code,boardCode);
	if(haveHis==true && hashList[hash].full==true)//���������ʷ,�Ҿ��������ŷ��б���ֱ�ӵ�����ʷ�߷���
	{
		if(hashList[hash].cut==false)//cut�ֶ�Ϊfalse��֤��stepList��ÿһ���value���۱�׼һ�¡���ʱ���Զ�stepList�е��ŷ�����������������߼�֦Ч��
			sort(hashList[hash].stepList.begin(),hashList[hash].stepList.end(),cmpStepValue);
		stepList=hashList[hash].stepList;
	}
	else
	{
		if(HandNum<=6)
			SearchDepth=6;//�涨��ʼ�������
		else
			SearchDepth=5;//�涨��ʼ�������
		MaxDepth=15;
		myType=GetBoardType(side);
		denType=GetBoardType(1-side);
		if(myType>0)//�������ܶԶԷ�������в�����ǿ�ʤ���棬���ɱ�ʤ�����㣩
			stepList=MakeStepListForWin(side,20);
		else if(denType==1)//�ܵ�����в
			stepList=MakeStepListForDefendSingle(side,20);
		else if(denType>1)//�ܵ�˫��в������в��α˫��в��α����в
			stepList=MakeStepListForDefendDouble(side,20);
		else//��������в���Σ�������4�ͺ�5�ͣ�
			stepList=MakeStepListForNone(side,20);
	}
	if(stepList.size()==0)
	{
		printf("The engine don't create step,so the engine give up!\n");
	}
	else if(stepList.size()==1)
	{
		return stepList[0];
	}
	else
	{
		for(iterS=stepList.begin();iterS!=stepList.end();iterS++)
		{
			MakeMove(iterS->first,tempLine,side);
			MakeMove(iterS->second,tempLine2,side);
			moveCodeS(boardCode,*iterS,side);
			val=-nega_alpha_beta(unside,-WINLOSE,-value,1);//������ֵ����alpha-beta��֦������ֵ
			iterS->value=val;
			moveCodeS(boardCode,*iterS,side);
			BackMove(iterS->second,tempLine2,side);
			BackMove(iterS->first,tempLine,side);
			if(val>value)
			{
				if(val==WINLOSE)//�ɻ�ʤ��ֱ�ӷ���
					return *iterS;
				value=val;
				step=*iterS;
			}
		}
	}
	if(value>=WINLOSE-MAXDEPTH)
	{
		int depth=WINLOSE-value;
		if(depth>SearchDepth)
			MaxDepth=depth-2;
		else
			SearchDepth=MaxDepth=depth-2;
	}
	if(value==-WINLOSE)
	{
		if(stepList.size()!=0)
			step=stepList[0];
	}
	return step;
}

/**	nega-alpha-beta - ������ֵ������ͨ��alpha-beta��֦�����Ż���alpha-beta��֦��һ����С���ڵĹ��̣�����֦��״̬������ֵ�������ڷ�Χ
 *	@return:	���������ó��ľ���Ǳ������ֵ�������ֵ��
 *	@side:		ģ�����巽ִ����ɫ
 *	@alpha:		ָ����֦����������
 *	@beta:		ָ����֦����������
 *	@depth:		��ǰ�������
 */
int nega_alpha_beta(BYTE side,int alpha,int beta,const int depth)
{
	int unside=1-side;
	LineInfo tempLine[2][4],tempLine2[2][4];
	vector<Step>::iterator iterS;
	HashInfo hashP;
	long hash=hashCode(boardCode);

	if(compareCode(hashList[hash].code,boardCode))//���������ʷ������ֱ�ӵ�����ʷ
		hashP=hashList[hash];//��ȡ��ʷ
	else//��������ʷ������������ϣ��
	{
		hashP.code=boardCode;//��ֵ����Ψһ��ʶ
		hashP.cut=true;//��ʼ��Ϊtrue������������֦
		hashP.full=false;//��ʼ��Ϊfalse�����������������ŷ��б�
		hashP.myType=GetBoardType(side);
		hashP.denType=GetBoardType(unside);
	}
	hashP.index=HandNum;//���¹�ϣ����ʱ���
	hashP.depth=depth;

	//�ݹ����
	if(hashP.denType>=1000)//�Է���ʤ
	{
		hashP.value=depth-WINLOSE;
		addHash(hashP);
		return depth-WINLOSE;
	}
	else if(hashP.myType>0)//������в�Է������ͣ��ڵݹ�����У�����ʹ��һ�����������ڣ�
	{
		hashP.value=WINLOSE-depth;
		addHash(hashP);
		return WINLOSE-depth;
	}

	if(depth>=SearchDepth)//������������
	{
		if(HandNum<=6)
		{
			hashP.value=GetBoardValue(side)-GetBoardValue(unside);
			addHash(hashP);
			return hashP.value;
		}
		else
		{
			hashP.value=ExtendSeach(side,alpha,beta,depth);//��չ�����Զ��˵�ģ��Ϊ���
			if(hashP.value>MAXDEPTH-WINLOSE&&hashP.value<WINLOSE-MAXDEPTH)
				hashP.value=(hashP.value*2+(GetBoardValue(side)-GetBoardValue(unside))*8)/10;
			addHash(hashP);
			return hashP.value;
		}
	}
	else
	{
		//����ȫ���ŷ�
		if(hashP.full==false)//����ϣ�����full�ֶ�ֵΪfalseʱ��֤���ñ����stepList�ֶ�ֵ�����������ܱ�ֱ�����á�
		{
			if(hashP.myType>0)//�������ܶԶԷ�������в�����ǿ�ʤ���棬���ɱ�ʤ�����㣩
				hashP.stepList=MakeStepListForWin(side,20);
			else if(hashP.denType==1)//�ܵ�����в
				hashP.stepList=MakeStepListForDefendSingle(side,20);
			else if(hashP.denType>1)//�ܵ�˫��в������в��α˫��в��α����в
				hashP.stepList=MakeStepListForDefendDouble(side,20);
			else//��������в���Σ�������4�ͺ�5�ͣ�
				hashP.stepList=MakeStepListForNone(side,20);
//			if(depth>=5)
//			{
//				int limit=hashP.stepList.size();
//				if(limit>19)
//					hashP.stepList.resize(limit*0.8);
//			}
			hashP.full=true;
		}
		else
		{
			if(hashP.cut==false)//cut�ֶ�Ϊfalse��֤��stepList��ÿһ���value���۱�׼һ�¡���ʱ���Զ�stepList�е��ŷ�����������������߼�֦Ч��
			{
				sort(hashP.stepList.begin(),hashP.stepList.end(),cmpStepValue);
//				if(hashP.stepList.size()>15)
//					hashP.stepList.resize(hashP.stepList.size()*0.8);
				hashP.cut=true;
			}
		}
		if(hashP.stepList.size()==0)//��Ϊ�����ƽ���棬���������ŷ�
		{
			if(hashP.denType==2)
				printf("����˫����в����\n");
			else if(hashP.denType==10)
				printf("����˫��в����\n");
			hashP.value=depth-WINLOSE;
			addHash(hashP);
			return depth-WINLOSE;
		}
	}

	for(iterS=hashP.stepList.begin();iterS!=hashP.stepList.end();iterS++)
	{
		moveCodeS(boardCode,*iterS,side);
		hash=hashCode(boardCode);
		if(compareCode(hashList[hash].code,boardCode)&&hashList[hash].index==HandNum)//���������ʷ����ʱ����뵱ǰһ�£����ǳ�������������ֱ�ӷ��ر��б����ֵ
		{
			if(hashList[hash].value<-alpha)
				iterS->value=-hashList[hash].value;
			else
				iterS->value=alpha;
		}
		else 
		{
			MakeMove(iterS->first,tempLine,side);
			MakeMove(iterS->second,tempLine2,side);
			iterS->value=-nega_alpha_beta(unside,-beta,-alpha,depth+1);
			BackMove(iterS->second,tempLine2,side);
			BackMove(iterS->first,tempLine,side);
		}
		moveCodeS(boardCode,*iterS,side);
		if(alpha<iterS->value)
		{
			alpha=iterS->value;
			if(beta<=alpha)
			{
				hashP.value=beta;
				addHash(hashP);
				return beta;
			}
		}
	}
	hashP.cut=false;//δ����֦
	hashP.value=alpha;
	addHash(hashP);
	return alpha;
}

/**	ExtendSeach - ��չ������ֵ������ͨ��DTSS�����Ż�����ʹ���浽��һ��ƽ��״̬���й�ֵ
 *	@return:	���������ó��ľ���Ǳ������ֵ�������ֵ��
 *	@side:		ģ�����巽ִ����ɫ
 *	@alpha:		ָ����֦����������
 *	@beta:		ָ����֦����������
 *	@depth:		��ǰ�������
 */
int ExtendSeach(BYTE side,int alpha,int beta,const int depth)
{
	int unside=1-side,val;
	LineInfo tempLine[2][4],tempLine2[2][4];
	vector<Step> stepList;
	vector<Step>::iterator iterS;
	int myType,denType;

	myType=GetBoardType(side);
	denType=GetBoardType(unside);

	//�ݹ����
	if(denType>=1000)//��ʤ
		return depth-WINLOSE;
	else if(myType>0)//������в�Է������ͣ��ڵݹ�����У�����ʹ��һ�����������ڣ�
		return WINLOSE-depth;

	if(depth>=MaxDepth)//�������������Ƚ��з���
		return GetBoardValue(side)-GetBoardValue(unside);

	//��չ�����в���Ҫ�����ŷ��б����䲻�����ԣ��ڳ��������н����ᱻ����
	if(denType==1)//����ܵ�����в��������������в����
		stepList=MakeStepListForDefendSingleEx(side,3);
	else if(denType>1)//�ܵ�˫��в���ؽ������У�����˫����������һ����
		stepList=MakeStepListForDefendDoubleEx(side,4);
	else//����вʱѰ�ҷ��� ˫�������� �Ļ���
		stepList=MakeStepListForDouble(side,3);

	if(stepList.size()==0)
	{
		if(denType>2)//����в�����ƽ�
			return depth-WINLOSE;
		else//�����������������
			return GetBoardValue(side)-GetBoardValue(unside);
	}

	for(iterS=stepList.begin();iterS!=stepList.end();iterS++)
	{
		MakeMove(iterS->first,tempLine,side);
		MakeMove(iterS->second,tempLine2,side);
		val=-ExtendSeach(unside,-beta,-alpha,depth+1);
		BackMove(iterS->second,tempLine2,side);
		BackMove(iterS->first,tempLine,side);
		if(val>=WINLOSE-MaxDepth)//ֻҪ��һ���ŷ����Ի�ʤ������ʤ
			return val;
		if(alpha<val)
		{
			alpha=val;
			if(beta<=val)
				return beta;
		}
	}
	return alpha;
}

/**	initialAllLine - ��ʼ���������˫��92������Ϣ��lineInfo[2][92]����
 *	@return:	�޷���ֵ
 */
void initialAllLine()
{
	int i,j;
	for(i=0;i<2;i++)
		for(j=0;j<92;j++)
			initialLine(&lineInfo[i][j]);
}

/**	UpdataBoard - ͨ���з��ֲ����������Ϣ�����ߵ���Ϣ
 *	@return:	�޷���ֵ
 *	@step:		�����ŷ�
 */
void UpdataBoard(Step step)
{
	UpdateLineForCross(step.first,BLACK);
	UpdateLineForCross(step.first,WHITE);
	UpdateLineForCross(step.second,BLACK);
	UpdateLineForCross(step.second,WHITE);
}

/**	UpdateLineForCross - ͨ��������ߵ���Ϣ
 *	@return:	�޷���ֵ
 *	@point:		���ӵ�
 *	@side:		����������ִ����ɫ
 */
void UpdateLineForCross(Point point,BYTE side,int tag)
{
	int key;
	Point start;
	LineInfo lineTemp[4];
	if(point.x==-1)//���ӵ�
		return;
	key=GetLineKey(point,&start,ANGLE0);//��õ����ں����ߵ��������ߵ���ʼ��
	if(key!=-1)
	{
		lineTemp[ANGLE0]=AnalyzeLine(start,ANGLE0,side,tag);//���ߵ���ʼ�㿪ʼ�������ߵ���Ϣ
		CopyLineInfo(lineInfo[side][key],lineTemp[ANGLE0],tag);//�����ߵ���Ϣ������
	}
	key=GetLineKey(point,&start,ANGLE90);//����
	if(key!=-1)
	{
		lineTemp[ANGLE90]=AnalyzeLine(start,ANGLE90,side,tag);
		CopyLineInfo(lineInfo[side][key],lineTemp[ANGLE90],tag);
	}
	key=GetLineKey(point,&start,ANGLE45);//����б��
	if(key!=-1)
	{
		lineTemp[ANGLE45]=AnalyzeLine(start,ANGLE45,side,tag);
		CopyLineInfo(lineInfo[side][key],lineTemp[ANGLE45],tag);
	}
	key=GetLineKey(point,&start,ANGLE135);//����б��
	if(key!=-1)
	{
		lineTemp[ANGLE135]=AnalyzeLine(start,ANGLE135,side,tag);
		CopyLineInfo(lineInfo[side][key],lineTemp[ANGLE135],tag);
	}
}

/**	AnalyzeLine - ͨ���ߵ���ʼ��ͷ��������
 *	@return:	��������Ϣ
 *	@start:		�ߵ���ʼ���������
 *	@lineDirec:	�ߵķ����־
 *	@side:		����������Ϣ������ִ����ɫ
 */
LineInfo AnalyzeLine(Point start,BYTE lineDirec,BYTE side,int tag)
{
	int shapeIndex=0;	// ��������
	int x=start.x;	// ����㿪ʼ��
	int y=start.y;
	Point LinePos;		//�����������ε����������ꡣ��-1��ʾ��δȷ�������������ε���㣨���̵���Ч�������ֵ����С��0����
	int len=0;			//��¼��ǰλ������Ҫ��Ķ��е�λ��, �������Ϊ��ǰ�γ�
	int value=0;
	vector<Point>::iterator iterP; 
	vector<Step>::iterator iterS; 
	LineInfo lineInfo_1,lineInfo_2;

//	lineInfo_2.side=side;
	lineInfo_2.value=0;
	lineInfo_2.LineType=0;
	LinePos.x=-1;

	//��ȡ����
	while (x<edge&&y<edge&&x>-1 && y>-1)	//	����������
	{							
		if(virtualBoard[x][y]==EMPTY)//�յ�
		{
			len++;
			if(LinePos.x==-1)//ȷ����ʼλ��
			{ LinePos.x=x; LinePos.y=y; }
		}
		else if(virtualBoard[x][y]==side)		// ����Ǳ������ӡ�
		{
			shapeIndex+=(1<<len);	
			len++;
			if(LinePos.x==-1)//ȷ����ʼλ��
			{ LinePos.x=x; LinePos.y=y; }
		}	
		else		//  ����ǶԷ����ӣ�����������ȷ����
		{
			if(len>5)	//���ȳ���5���Ż����ӡ�
			{
				shapeIndex+=(1<<len);	  //�������ͱ߽磬�õ�����������
				lineInfo_1=ValuateType(shapeIndex,LinePos,lineDirec,tag);//���������Ͱ�������Ϣ
			/*  ����Ծ�����ʤ���͵���ʤ���������Ϣ�ɼ�
				if(lineInfo_1.LineType>=1000)//�����ʤ��ֱ�ӷ���
					return lineInfo_1;
			*/
				lineInfo_2.value+=lineInfo_1.value;//�ۼ��ߵļ�ֵ
				lineInfo_2.LineType+=lineInfo_1.LineType;//�ۼ��ߵ���в����
				//����������Ϣ
				if(lineInfo_1.LineType)//��в��
				{
					if(lineInfo_1.defPointList.size()>0)//������
						for(iterP=lineInfo_1.defPointList.begin();iterP!=lineInfo_1.defPointList.end();iterP++)
							lineInfo_2.defPointList.push_back(*iterP);
					if(lineInfo_1.defStepList.size()>0)//������
						for(iterS=lineInfo_1.defStepList.begin();iterS!=lineInfo_1.defStepList.end();iterS++)
							lineInfo_2.defStepList.push_back(*iterS);
					if(lineInfo_1.winList.size()>0)//��ʤ��
						for(iterP=lineInfo_1.winList.begin();iterP!=lineInfo_1.winList.end();iterP++)
							lineInfo_2.winList.push_back(*iterP);
					if(lineInfo_1.willWinList.size()>0)//����ʤ����
						for(iterP=lineInfo_1.willWinList.begin();iterP!=lineInfo_1.willWinList.end();iterP++)
							lineInfo_2.willWinList.push_back(*iterP);
					if(lineInfo_1.triThreatList.size()>0)//����в��
						for(iterP=lineInfo_1.triThreatList.begin();iterP!=lineInfo_1.triThreatList.end();iterP++)
							lineInfo_2.triThreatList.push_back(*iterP);
				}
				if(lineInfo_1.duoThreatList.size()>0)//˫��в��
					for(iterP=lineInfo_1.duoThreatList.begin();iterP!=lineInfo_1.duoThreatList.end();iterP++)
						lineInfo_2.duoThreatList.push_back(*iterP);
				if(lineInfo_1.solThreatList.size()>0)//����в��
					for(iterP=lineInfo_1.solThreatList.begin();iterP!=lineInfo_1.solThreatList.end();iterP++)
						lineInfo_2.solThreatList.push_back(*iterP);
				if(lineInfo_1.duoPotenList.size()>0)//˫Ǳ����
					for(iterP=lineInfo_1.duoPotenList.begin();iterP!=lineInfo_1.duoPotenList.end();iterP++)
						lineInfo_2.duoPotenList.push_back(*iterP);
				if(lineInfo_1.solPotenList.size()>0)//��Ǳ����
					for(iterP=lineInfo_1.solPotenList.begin();iterP!=lineInfo_1.solPotenList.end();iterP++)
						lineInfo_2.solPotenList.push_back(*iterP);
				if(lineInfo_1.toDuoTwoList.size()>0)//Ǳ˫Ǳ����
					for(iterP=lineInfo_1.toDuoTwoList.begin();iterP!=lineInfo_1.toDuoTwoList.end();iterP++)
						lineInfo_2.toDuoTwoList.push_back(*iterP);
			}
			shapeIndex=0;	// ��������
			len=0;		// ���ȹ���
			LinePos.x=-1;
		}
		Increment(x, y, lineDirec);
	}
	if(len>5)	//���ȳ���5���Ż����ӡ�
	{
		shapeIndex+=(1<<len);	  // �õ���������
		lineInfo_1=ValuateType(shapeIndex,LinePos,lineDirec,tag);
	/*  ����Ծ�����ʤ���͵���ʤ���������Ϣ�ɼ�
		if(lineInfo_1.LineType>=1000)//�����ʤ��ֱ�ӷ���
			return lineInfo_1;
	*/
		lineInfo_2.value+=lineInfo_1.value;//�ۼ��ߵļ�ֵ
		lineInfo_2.LineType+=lineInfo_1.LineType;//�ۼ��ߵ���в����
		if(lineInfo_1.LineType>0)//��в��
		{
			if(lineInfo_1.defPointList.size()>0)
				for(iterP=lineInfo_1.defPointList.begin();iterP!=lineInfo_1.defPointList.end();iterP++)
					lineInfo_2.defPointList.push_back(*iterP);
			if(lineInfo_1.defStepList.size()>0)
				for(iterS=lineInfo_1.defStepList.begin();iterS!=lineInfo_1.defStepList.end();iterS++)
					lineInfo_2.defStepList.push_back(*iterS);
			if(lineInfo_1.winList.size()>0)
				for(iterP=lineInfo_1.winList.begin();iterP!=lineInfo_1.winList.end();iterP++)
					lineInfo_2.winList.push_back(*iterP);
			if(lineInfo_1.willWinList.size()>0)
				for(iterP=lineInfo_1.willWinList.begin();iterP!=lineInfo_1.willWinList.end();iterP++)
					lineInfo_2.willWinList.push_back(*iterP);
			if(lineInfo_1.triThreatList.size()>0)
				for(iterP=lineInfo_1.triThreatList.begin();iterP!=lineInfo_1.triThreatList.end();iterP++)
					lineInfo_2.triThreatList.push_back(*iterP);
		}
		if(lineInfo_1.duoThreatList.size()>0)
			for(iterP=lineInfo_1.duoThreatList.begin();iterP!=lineInfo_1.duoThreatList.end();iterP++)
				lineInfo_2.duoThreatList.push_back(*iterP);
		if(lineInfo_1.solThreatList.size()>0)
			for(iterP=lineInfo_1.solThreatList.begin();iterP!=lineInfo_1.solThreatList.end();iterP++)
				lineInfo_2.solThreatList.push_back(*iterP);
		if(lineInfo_1.duoPotenList.size()>0)
			for(iterP=lineInfo_1.duoPotenList.begin();iterP!=lineInfo_1.duoPotenList.end();iterP++)
				lineInfo_2.duoPotenList.push_back(*iterP);
		if(lineInfo_1.solPotenList.size()>0)
			for(iterP=lineInfo_1.solPotenList.begin();iterP!=lineInfo_1.solPotenList.end();iterP++)
				lineInfo_2.solPotenList.push_back(*iterP);
		if(lineInfo_1.toDuoTwoList.size()>0)
			for(iterP=lineInfo_1.toDuoTwoList.begin();iterP!=lineInfo_1.toDuoTwoList.end();iterP++)
				lineInfo_2.toDuoTwoList.push_back(*iterP);
	}
	return lineInfo_2;
}

/**	ValueType - ����ָ������
 *	@return:	��������Ϣ��������������в���͡���ֵ���������������㡢��в�㡢Ǳ���㡢ǱǱ����ľ������ꡣ
 *	@style��	������������Analizeline������ȡ�����εĶ�����������
 *	@start:		���͵���ʼ���������
 *	@lineDirec:	���������ߵķ���
 */
LineInfo ValuateType(int style,Point start,BYTE lineDirec,int tag)
{
	int i,len;	//len:���͵ĳ��ȡ�
	int ShapeStyleId;	//�������ͱ�ţ�1~14����
	Point tempPoint;
	Step tempStep;
	LineInfo lineInfo;
	vector<iPoint>::iterator iterP;
	vector<iStep>::iterator iterS;

	ShapeStyleId=preTable[style];//�����������
	lineInfo.value=LineTypeValue[ShapeStyleId];//������ͼ�ֵ
	lineInfo.LineType=LineTypeThreat[ShapeStyleId];//������͵���в����

	if(ShapeStyleId==ZERO)//������
		return lineInfo;
	if(ShapeStyleId>=WIN)//��ʤ����
		return lineInfo;

	for(i=19;i>=0;i--)//����style����Ч������λ���������ͳ��ȡ�����Ϊ6λ��������AnalyzeLine���������ġ�
		if(GetABit(style,i))
		{ len=i; break; }

	if(ShapeStyleId>=THREAT_four_SINGLE)//��в��
	{
		if(tag&TODEFENT)
		{
			if(linetypeInfo[style].defStepList.size()!=0)//������
				for(iterS=linetypeInfo[style].defStepList.begin();iterS!=linetypeInfo[style].defStepList.end();iterS++)
				{
					tempStep.first.x=start.x+iterS->first*lineVector[lineDirec][0];
					tempStep.first.y=start.y+iterS->first*lineVector[lineDirec][1];
					tempStep.second.x=start.x+iterS->second*lineVector[lineDirec][0];
					tempStep.second.y=start.y+iterS->second*lineVector[lineDirec][1];
					lineInfo.defStepList.push_back(tempStep);
				}
			if(linetypeInfo[style].defPointList.size()!=0)//������
				for(iterP=linetypeInfo[style].defPointList.begin();iterP!=linetypeInfo[style].defPointList.end();iterP++)
				{
					tempPoint.x=start.x+(*iterP)*lineVector[lineDirec][0];
					tempPoint.y=start.y+(*iterP)*lineVector[lineDirec][1];
					lineInfo.defPointList.push_back(tempPoint);
				}
		}
		if((tag&TOWIN) && linetypeInfo[style].win!=-1)//��ʤ��
		{
			tempPoint.x=start.x+linetypeInfo[style].win*lineVector[lineDirec][0];
			tempPoint.y=start.y+linetypeInfo[style].win*lineVector[lineDirec][1];
			lineInfo.winList.push_back(tempPoint);
		}
		if((tag&TOWILLWIN) && linetypeInfo[style].willWin!=-1)//������ʤ��
		{
			tempPoint.x=start.x+linetypeInfo[style].willWin*lineVector[lineDirec][0];
			tempPoint.y=start.y+linetypeInfo[style].willWin*lineVector[lineDirec][1];
			lineInfo.willWinList.push_back(tempPoint);
		}
		if(linetypeInfo[style].triThreat!=-1)//����в��
		{
			tempPoint.x=start.x+linetypeInfo[style].triThreat*lineVector[lineDirec][0];
			tempPoint.y=start.y+linetypeInfo[style].triThreat*lineVector[lineDirec][1];
			lineInfo.triThreatList.push_back(tempPoint);
		}
	}
	if((tag&TODUOTHREAT) && linetypeInfo[style].duoThreatList.size()!=0)
		for(iterP=linetypeInfo[style].duoThreatList.begin();iterP!=linetypeInfo[style].duoThreatList.end();iterP++)
		{
			tempPoint.x=start.x+(*iterP)*lineVector[lineDirec][0];
			tempPoint.y=start.y+(*iterP)*lineVector[lineDirec][1];
			lineInfo.duoThreatList.push_back(tempPoint);
		}
	if((tag&TOSOLTHREAT) && linetypeInfo[style].solThreatList.size()!=0)
		for(iterP=linetypeInfo[style].solThreatList.begin();iterP!=linetypeInfo[style].solThreatList.end();iterP++)
		{
			tempPoint.x=start.x+(*iterP)*lineVector[lineDirec][0];
			tempPoint.y=start.y+(*iterP)*lineVector[lineDirec][1];
			lineInfo.solThreatList.push_back(tempPoint);
		}
	if((tag&TODUOPOTEN) &&  linetypeInfo[style].duoPotenList.size()!=0)
		for(iterP=linetypeInfo[style].duoPotenList.begin();iterP!=linetypeInfo[style].duoPotenList.end();iterP++)
		{
			tempPoint.x=start.x+(*iterP)*lineVector[lineDirec][0];
			tempPoint.y=start.y+(*iterP)*lineVector[lineDirec][1];
			lineInfo.duoPotenList.push_back(tempPoint);
		}
	if((tag&TOSOLPOTEN) && linetypeInfo[style].solPotenList.size()!=0)
		for(iterP=linetypeInfo[style].solPotenList.begin();iterP!=linetypeInfo[style].solPotenList.end();iterP++)
			{
				tempPoint.x=start.x+(*iterP)*lineVector[lineDirec][0];
				tempPoint.y=start.y+(*iterP)*lineVector[lineDirec][1];
				lineInfo.solPotenList.push_back(tempPoint);
			}
	if((tag&TOCOMMON) && linetypeInfo[style].toDuoTwoList.size()!=0)
		for(iterP=linetypeInfo[style].toDuoTwoList.begin();iterP!=linetypeInfo[style].toDuoTwoList.end();iterP++)
		{
			tempPoint.x=start.x+(*iterP)*lineVector[lineDirec][0];
			tempPoint.y=start.y+(*iterP)*lineVector[lineDirec][1];
			lineInfo.toDuoTwoList.push_back(tempPoint);
		}
	return lineInfo;
}


/**	GetBoardType - �ռ������в������Ϣ
 *	@side	Ӧ�ռ���Ϣ�����ַ���־
 */
int GetBoardType(BYTE side)
{
	int i,type=0;
	for(i=0;i<92;i++)//����92����
		type+=lineInfo[side][i].LineType;//�ۼ��ߵ���в������Ϊ�������в����
	return type;
}

/**	GetBoardValue - �ռ������в������Ϣ
 *	@side	Ӧ�ռ���Ϣ�����ַ���־
 */
int GetBoardValue(BYTE side)
{
	int i,value=0;
	for(i=0;i<92;i++)//����92����
		value+=lineInfo[side][i].value;//�ۼ��ߵļ�ֵ��Ϊ�����ֵ
	return value;
}

/**	GetBoardInfo - �ռ������Ϣ
 *	@side	Ӧ�ռ���Ϣ�����ַ���־
 *	@tag	�ռ���Ϣ���԰�λ��־����1��־����ּ�ֵ����в����ΪĬ���ռ���Ϣ
 */
SynInfo GetBoardInfo(BYTE side,int tag)
{
	int i;
	SynInfo tempSynInfo;
	vector<Point>::iterator iterP;	
	vector<Step>::iterator iterS;		

	for(i=0;i<92;i++)//����92����
	{
		if(tag&TODEFENT)
		{
			if(lineInfo[side][i].defPointList.size()!=0)
				for(iterP=lineInfo[side][i].defPointList.begin();iterP!=lineInfo[side][i].defPointList.end();iterP++)
					tempSynInfo.defPointList.push_back(*iterP);
			if(lineInfo[side][i].defStepList.size()!=0)
				for(iterS=lineInfo[side][i].defStepList.begin();iterS!=lineInfo[side][i].defStepList.end();iterS++)
					tempSynInfo.defStepList.push_back(*iterS);
		}
		if((tag&TOWIN) && lineInfo[side][i].winList.size()!=0)
			for(iterP=lineInfo[side][i].winList.begin();iterP!=lineInfo[side][i].winList.end();iterP++)
				tempSynInfo.winList.push_back(*iterP);
		if((tag&TOWILLWIN) && lineInfo[side][i].willWinList.size()!=0)
			for(iterP=lineInfo[side][i].willWinList.begin();iterP!=lineInfo[side][i].willWinList.end();iterP++)
				tempSynInfo.willWinList.push_back(*iterP);
		if((tag&TODUOPOTEN) && lineInfo[side][i].duoPotenList.size()!=0)
			for(iterP=lineInfo[side][i].duoPotenList.begin();iterP!=lineInfo[side][i].duoPotenList.end();iterP++)
				tempSynInfo.duoPotenList.push_back(*iterP);
		if((tag&TOSOLPOTEN) && lineInfo[side][i].solPotenList.size()!=0)
			for(iterP=lineInfo[side][i].solPotenList.begin();iterP!=lineInfo[side][i].solPotenList.end();iterP++)
				tempSynInfo.solPotenList.push_back(*iterP);
		if((tag&TOCOMMON) && lineInfo[side][i].toDuoTwoList.size()!=0)
			for(iterP=lineInfo[side][i].toDuoTwoList.begin();iterP!=lineInfo[side][i].toDuoTwoList.end();iterP++)
				tempSynInfo.toDuoTwoList.push_back(*iterP);
		if((tag&TODUOTHREAT)&&lineInfo[side][i].duoThreatList.size()!=0)
			for(iterP=lineInfo[side][i].duoThreatList.begin();iterP!=lineInfo[side][i].duoThreatList.end();iterP++)
				tempSynInfo.duoThreatList.push_back(*iterP);
		if((tag&TOSOLTHREAT)&&lineInfo[side][i].solThreatList.size()!=0)
			for(iterP=lineInfo[side][i].solThreatList.begin();iterP!=lineInfo[side][i].solThreatList.end();iterP++)
				tempSynInfo.solThreatList.push_back(*iterP);
		if(lineInfo[side][i].triThreatList.size()!=0)
			for(iterP=lineInfo[side][i].triThreatList.begin();iterP!=lineInfo[side][i].triThreatList.end();iterP++)
				tempSynInfo.triThreatList.push_back(*iterP);
	}
	if(tag&TODEFENT)
		UniquePoint(tempSynInfo.defPointList);
	if(tag&TODUOTHREAT)
		UniquePoint(tempSynInfo.duoThreatList);
	if(tag&TOSOLTHREAT)
		UniquePoint(tempSynInfo.solThreatList);
	if(tag&TODUOPOTEN)
		UniquePoint(tempSynInfo.duoPotenList);
	if(tag&TOSOLPOTEN)
		UniquePoint(tempSynInfo.solPotenList);
	if(tag&TOCOMMON)
		UniquePoint(tempSynInfo.toDuoTwoList);
	return tempSynInfo;
}

//�з�������

/**	MakeStepListForWin - ��ʤ�з�������
 *	@return:	�����ŷ��б�
 *	@myInfo:	ִ�巽��ǰ�����ۺ���Ϣ
 *	@denInfo:	�Է���ǰ�����ۺ���Ϣ
 *	@limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForWin(int side,unsigned int limit)
{
	SynInfo myInfo=GetBoardInfo(side,TOWIN+TOWILLWIN);
	SynInfo tempMy;//��ʱ������Ϣ����
	LineInfo tempLine[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Point>::iterator iterP,iterP2;
	vector<Step> stepList;//�з��б�

	//���������ʤ�㣬��ֱ�ӷ��ص�һ����ʤ����Ϊ�з�
	//����ѡȡ��һ��������ʤ��������£������������º�ĵ�һ����ʤ������鲽
	if(myInfo.winList.size()>0)
	{
		tempStep.first=myInfo.winList[0];
		tempStep.second.x=tempStep.second.y=-1;
		tempStep.value=WINLOSE;
		stepList.push_back(tempStep);
		return stepList;
	}
	else
	{
		for(iterP=myInfo.willWinList.begin();iterP!=myInfo.willWinList.end();iterP++)
		{
			tempStep.first=*iterP;
			MakeMove(tempStep.first,tempLine,side,TOWIN);//��Ϊ��ʤ����ֻ����ʤ������ռ�
			tempMy=GetBoardInfo(side,TOWIN);
			BackMove(tempStep.first,tempLine,side);
			for(iterP2=tempMy.winList.begin();iterP2!=tempMy.winList.end();iterP2++)
			{
				tempStep.second=*iterP2;
				tempStep.value=WINLOSE;
				stepList.push_back(tempStep);
				return stepList;
			}
		}
	}
	return stepList;
}

/**	MakeStepListForDefendSingle - ��������в�з�������
 *	@return:	�����ŷ��б�
 *	@myInfo:	ִ�巽��ǰ�����ۺ���Ϣ
 *	@denInfo:	�Է���ǰ�����ۺ���Ϣ
 *	@limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForDefendSingle(int side,unsigned int limit)
{
	int unside=1-side;
	SynInfo denInfo=GetBoardInfo(unside,TODEFENT);
	SynInfo tempMy,tempDen;//��ʱ������Ϣ����
	LineInfo tempLine[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Step> stepList;//�з��б�
	vector<Point> pointList;//���б�
	vector<Point>::iterator iterP,iterP2;
	vector<Step>::iterator iterS;

	//ѡȡһ���ƽ��������£�Ȼ��������º�ľ�������鲽��һ������в����������������ƽ��
	for(iterP=denInfo.defPointList.begin();iterP!=denInfo.defPointList.end();iterP++)
	{
		tempStep.first=*iterP;
		MakeMove(tempStep.first,tempLine,side,FORNONTHREAT);//���º��ռ�ȫ����Ϣ
		tempMy=GetBoardInfo(side,FORNONTHREAT);//�������в����ռ�������Ϣ
		if(tempMy.triThreatList.size()>0)//�����������ɶ���в���ͣ�����˫��в�����������ƽ������鲽�����أ���Ϊ�ж���в���ͣ�����������һ������ʱ��ʤ
		{
			stepList.resize(0);//�ͷ��з��б����Ѵ��ڵ��з���ֻ���������ɶ���в���͵��з�
			tempStep.second=tempMy.triThreatList[0];
			tempStep.value=WINLOSE-1;
			stepList.push_back(tempStep);
			BackMove(tempStep.first,tempLine,side);//��ΪҪ���з��أ���ȡ������
			return stepList;
		}
		tempDen=GetBoardInfo(unside,FORPOTEN);//�ռ����ֵ�Ǳ����Ϣ���ޣ�
		//������<˫��в�㣬����в�㣬˫Ǳ����>�ͶԷ�<˫��в�㣬����в�㣬˫Ǳ����>��Ϊ��ѡ��
		if(tempMy.duoThreatList.size()!=0)
			for(iterP2=tempMy.duoThreatList.begin();iterP2!=tempMy.duoThreatList.end();iterP2++)
					pointList.push_back(*iterP2);
		if(tempMy.solThreatList.size()!=0)
			for(iterP2=tempMy.solThreatList.begin();iterP2!=tempMy.solThreatList.end();iterP2++)
				pointList.push_back(*iterP2);
		if(tempMy.duoPotenList.size()!=0)
			for(iterP2=tempMy.duoPotenList.begin();iterP2!=tempMy.duoPotenList.end();iterP2++)
				pointList.push_back(*iterP2);
		if(tempDen.duoThreatList.size()!=0)
			for(iterP2=tempDen.duoThreatList.begin();iterP2!=tempDen.duoThreatList.end();iterP2++)
				pointList.push_back(*iterP2);
		if(tempDen.solThreatList.size()!=0)
			for(iterP2=tempDen.solThreatList.begin();iterP2!=tempDen.solThreatList.end();iterP2++)
				pointList.push_back(*iterP2);
		if(tempDen.duoPotenList.size()!=0)
			for(iterP2=tempDen.duoPotenList.begin();iterP2!=tempDen.duoPotenList.end();iterP2++)
				pointList.push_back(*iterP2);
		UniquePoint(pointList);//ȥ�ز���
		if(pointList.size()<10)//�����ѡ������10���������ñ�����Ǳ���㣬Ǳ˫Ǳ����ͶԷ���Ǳ����
		{
			for(iterP2=tempMy.solPotenList.begin();iterP2!=tempMy.solPotenList.end();iterP2++)
				pointList.push_back(*iterP2);
			for(iterP2=tempMy.toDuoTwoList.begin();iterP2!=tempMy.toDuoTwoList.end();iterP2++)
				pointList.push_back(*iterP2);
			for(iterP2=tempDen.solPotenList.begin();iterP2!=tempDen.solPotenList.end();iterP2++)
				pointList.push_back(*iterP2);
			UniquePoint(pointList);
			if(pointList.size()==0)
			{
				for(iterP2=tempDen.toDuoTwoList.begin();iterP2!=tempDen.toDuoTwoList.end();iterP2++)
					pointList.push_back(*iterP2);
				UniquePoint(pointList);
			}
		}
		BackMove(tempStep.first,tempLine,side);
		for(iterP2=pointList.begin();iterP2!=pointList.end();iterP2++)
		{
			tempStep.second=*iterP2;
			tempStep.value=CalculateStepValue(tempStep,side);
			stepList.push_back(tempStep);
		}
		pointList.resize(0);
	}
	for(iterS=denInfo.defStepList.begin();iterS!=denInfo.defStepList.end();iterS++)//������THREAT_four_ADDITION����Ķ����ƽⲽ
	{
		iterS->value=CalculateStepValue(*iterS,side);
		stepList.push_back(*iterS);
	}
	if(stepList.size()==0)
	{
		printf("�쳣: On make step list for Defend solthreat!\n");
	}
	else
	{
		UniqueStep(stepList);
		sort(stepList.begin(),stepList.end(),cmpStepValue);

		unsigned int size=stepList.size();
		if(size>limit)
			stepList.resize(limit);
	}
	return stepList;
}

/**	MakeStepListForDefendDouble - ����˫��в���з�������
 *	@return:	�����ŷ��б�
 *	@myInfo:	ִ�巽��ǰ�����ۺ���Ϣ
 *	@denInfo:	�Է���ǰ�����ۺ���Ϣ
 *	@limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForDefendDouble(int side,unsigned int limit)
{
	int unside=1-side;
	SynInfo denInfo=GetBoardInfo(unside,TODEFENT);
	SynInfo tempMy,tempDen;//��ʱ������Ϣ����
	LineInfo tempLine[2][4],tempLine2[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Step> stepList;//�з��б�
	vector<Point> pointList;//���б�
	vector<Point>::iterator iterP,iterP2;
	vector<Step>::iterator iterS;
	int denType=GetBoardType(unside);

	if(denType>=20)//�Է����ڶ���в���ͻ���ʤ���ͣ��������������˫��в���ͣ������ذ�ֱ�ӷ��ؿ��з��б�
	{
		stepList.resize(0);
		return stepList;
	}
	else if(denType==10)//�˾���ֻ����һ��˫��в���ͣ���ֻ���ƽⲽ��Ϊ��ѡ�з������з��б�
	{
		for(iterS=denInfo.defStepList.begin();iterS!=denInfo.defStepList.end();iterS++)
			stepList.push_back(*iterS);
		UniqueStep(stepList);
		for(iterS=stepList.begin();iterS!=stepList.end();iterS++)
			iterS->value=CalculateStepValue(*iterS,side);
		sort(stepList.begin(),stepList.end(),cmpStepValue);
		return stepList;
	}
	else//���е���в���͵���֣��������в�����ܴ���α˫����һ��˫��в�ӵ���в�����ܴ���α�ࣩ
	{
		if(denType>10&&denType<20)//����һ��˫��в���ͣ�ͬʱ���ڵ���в���ͣ����ܴ���α����в��
		{
			for(iterS=denInfo.defStepList.begin();iterS!=denInfo.defStepList.end();iterS++)//��˫��в����Ϊ��Ҫ�ɷ֣������ƽ��ʹ��˫��в���͵��ƽⲽ
			{
				MakeMove(iterS->first,tempLine,side,0);//�鿴�Ƿ�����ƽ���в��ֻҪ���ߵ����ͣ��ʲ����ռ����µĵ���Ϣ
				MakeMove(iterS->second,tempLine2,side,0);
				if(GetBoardType(unside)==0)//�ж��Ƿ��ƽ�ɹ���������ΪTHREAT_four_ADDITION�Ķ����ƽⷨ����˫��в
					stepList.push_back(*iterS);
				BackMove(iterS->second,tempLine2,side);
				BackMove(iterS->first,tempLine,side);
			}
			if(stepList.size()!=0)//���ƽ�
			{
				UniqueStep(stepList);
				for(iterS=stepList.begin();iterS!=stepList.end();iterS++)
					iterS->value=CalculateStepValue(*iterS,side);
				sort(stepList.begin(),stepList.end(),cmpStepValue);
			}
			return stepList;//���������ƽ���棬�������ƽⷵ�ؿ��з��б�
		}
		else//ֻ���ڶ������в���ͣ����ܴ���α˫��
		{
			for(iterP=denInfo.defPointList.begin();iterP!=denInfo.defPointList.end();iterP++)
			{
				tempStep.first=*iterP;
				MakeMove(tempStep.first,tempLine,side,FORNONTHREAT+TODEFENT);
				tempDen=GetBoardInfo(unside,FORPOTEN+TODEFENT);//һ�����ߺ���ж��η����������ɱ������α˫��в���з��ĵڶ��ӳ��ֵ����ƽ�״̬���Ӷ�ʹ��һ������
				if(GetBoardType(unside)==0)//һ���ƽ⣬α˫��в��������в�������ɣ������޽��������߼��������ڿɵ����ƽ������������в
				{
					tempMy=GetBoardInfo(side,FORNONTHREAT);
					if(tempMy.triThreatList.size()>0)//�����������ɶ���в���ͣ�����˫��в�����������ƽ������鲽�����أ���Ϊ�ж���в���ͣ�����������һ������ʱ��ʤ
					{
						stepList.resize(0);
						tempStep.second=tempMy.triThreatList[0];
						tempStep.value=WINLOSE-1;
						stepList.push_back(tempStep);
						BackMove(tempStep.first,tempLine,side);
						return stepList;
					}
					//������<˫��в�㣬����в�㣬˫Ǳ����>�ͶԷ�<����в�㣬˫��в�㣬����в�㣬˫Ǳ����>��Ϊ��ѡ��
					for(iterP2=tempMy.duoThreatList.begin();iterP2!=tempMy.duoThreatList.end();iterP2++)
						pointList.push_back(*iterP2);
					for(iterP2=tempMy.solThreatList.begin();iterP2!=tempMy.solThreatList.end();iterP2++)
						pointList.push_back(*iterP2);
					for(iterP2=tempMy.duoPotenList.begin();iterP2!=tempMy.duoPotenList.end();iterP2++)
						pointList.push_back(*iterP2);
					for(iterP2=tempDen.duoThreatList.begin();iterP2!=tempDen.duoThreatList.end();iterP2++)
						pointList.push_back(*iterP2);
					for(iterP2=tempDen.solThreatList.begin();iterP2!=tempDen.solThreatList.end();iterP2++)
						pointList.push_back(*iterP2);
					for(iterP2=tempDen.duoPotenList.begin();iterP2!=tempDen.duoPotenList.end();iterP2++)
						pointList.push_back(*iterP2);
					UniquePoint(pointList);
					if(pointList.size()<10)
					{
						for(iterP2=tempMy.solPotenList.begin();iterP2!=tempMy.solPotenList.end();iterP2++)
							pointList.push_back(*iterP2);
						for(iterP2=tempMy.toDuoTwoList.begin();iterP2!=tempMy.toDuoTwoList.end();iterP2++)
							pointList.push_back(*iterP2);
						for(iterP2=tempDen.solPotenList.begin();iterP2!=tempDen.solPotenList.end();iterP2++)
							pointList.push_back(*iterP2);
						UniquePoint(pointList);
					}
					for(iterP2=pointList.begin();iterP2!=pointList.end();iterP2++)
					{
						tempStep.second=*iterP2;
						stepList.push_back(tempStep);
					}
					pointList.resize(0);
				}
				else//�����ƽ⣬����α˫��в
				{
					for(iterP2=tempDen.defPointList.begin();iterP2!=tempDen.defPointList.end();iterP2++)
					{
						MakeMove(*iterP2,tempLine2,side,0);
						if(GetBoardType(unside)==0)//�ж��Ƿ��ƽ�ɹ�
						{
							tempStep.second=*iterP2;
							stepList.push_back(tempStep);
						}
						BackMove(*iterP2,tempLine2,side);
					}
				}
				BackMove(tempStep.first,tempLine,side);
			}
			if(stepList.size()!=0)//���ƽ�
			{
				UniqueStep(stepList);
				for(iterS=stepList.begin();iterS!=stepList.end();iterS++)
					iterS->value=CalculateStepValue(*iterS,side);
				sort(stepList.begin(),stepList.end(),cmpStepValue);
				if(stepList.size()>limit)
					stepList.resize(limit);
			}
			return stepList;//���������ƽ���棬�������ƽⷵ�ؿ��з��б�
		}
	}
}

/**	MakeStepListForNone - ƽ���ŷ�������
 *	@return:	�����ŷ��б�
 *	@myInfo:	ִ�巽��ǰ�����ۺ���Ϣ
 *	@denInfo:	�Է���ǰ�����ۺ���Ϣ
 *	@limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForNone(int side,unsigned int limit)
{
	int unside=1-side;
	SynInfo myInfo=GetBoardInfo(side,FORNONTHREAT);
	SynInfo denInfo=GetBoardInfo(unside,FORNONTHREAT);
	SynInfo tempMy,tempDen;//��ʱ������Ϣ����
	LineInfo tempLine[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Step> stepList;//�з��б�
	vector<Point> pointList,pointList2;//���б�
	vector<Point>::iterator iterP,iterP2;
	vector<Step>::iterator iterS;

	//����һ�ӷ��������ͷ��������У���Ǳ������ֻ������4���У�5�Ϳ��Ƕ�Ǳ�������壩��
	if(myInfo.triThreatList.size()>0)//�ɵ���˫��в���غϹ��ɵĶ���в��
	{
		for(iterP=myInfo.triThreatList.begin();iterP!=myInfo.triThreatList.end();iterP++)
			pointList.push_back(*iterP);
	}
	else
	{
		//��ѡȡ������<˫��в�㣬˫Ǳ���㣬����в�㣬��Ǳ����>���Է�<����в�㣨����˫��ɣ���˫��в�㣬˫Ǳ���㣬����в��> ��Ϊ��һ�����������
		if(myInfo.duoThreatList.size()>0)
			for(iterP=myInfo.duoThreatList.begin();iterP!=myInfo.duoThreatList.end();iterP++)
				pointList.push_back(*iterP);
		if(myInfo.duoPotenList.size()>0)
			for(iterP=myInfo.duoPotenList.begin();iterP!=myInfo.duoPotenList.end();iterP++)
				pointList.push_back(*iterP);
		if(myInfo.solThreatList.size()>0)
			for(iterP=myInfo.solThreatList.begin();iterP!=myInfo.solThreatList.end();iterP++)
				pointList.push_back(*iterP);
		if(myInfo.solPotenList.size()>0)
			for(iterP=myInfo.solPotenList.begin();iterP!=myInfo.solPotenList.end();iterP++)
				pointList.push_back(*iterP);
		if(denInfo.duoThreatList.size()>0)
			for(iterP=denInfo.duoThreatList.begin();iterP!=denInfo.duoThreatList.end();iterP++)
				pointList.push_back(*iterP);
		if(denInfo.duoPotenList.size()>0)
			for(iterP=denInfo.duoPotenList.begin();iterP!=denInfo.duoPotenList.end();iterP++)
				pointList.push_back(*iterP);
		if(denInfo.solThreatList.size()>0)
			for(iterP=denInfo.solThreatList.begin();iterP!=denInfo.solThreatList.end();iterP++)
				pointList.push_back(*iterP);
		UniquePoint(pointList);
		if(pointList.size()<5)//����һ��������5�������ȼ��뱾��Ǳ˫Ǳ����
		{
			if(myInfo.toDuoTwoList.size()>0)
				for(iterP=myInfo.toDuoTwoList.begin();iterP!=myInfo.toDuoTwoList.end();iterP++)
					pointList.push_back(*iterP);
			UniquePoint(pointList);
/*			if(pointList.size()<5)//�ټ���Է���Ǳ�����Ǳ˫Ǳ����
			{
				if(denInfo.solPotenList.size()>0)
					for(iterP=denInfo.solPotenList.begin();iterP!=denInfo.solPotenList.end();iterP++)
						pointList.push_back(*iterP);
				if(denInfo.toDuoTwoList.size()>0)
					for(iterP=denInfo.toDuoTwoList.begin();iterP!=denInfo.toDuoTwoList.end();iterP++)
						pointList.push_back(*iterP);
				UniquePoint(pointList);
			}*/
		}
	}
	for(iterP=pointList.begin();iterP!=pointList.end();iterP++)
	{
		tempStep.first=*iterP;
		MakeMove(tempStep.first,tempLine,side,FORNONTHREAT);
		tempMy=GetBoardInfo(side,FORNONTHREAT);
		if(tempMy.triThreatList.size()>0)//���ж���в�㣨�������ɶ���в���͵ĵ���ɵ���˫��в����ɵĵ㣩
		{
			stepList.resize(0);
			tempStep.second=tempMy.triThreatList[0];
			tempStep.value=WINLOSE-1;
			stepList.push_back(tempStep);
			BackMove(tempStep.first,tempLine,side);
			return stepList;
		}
		tempDen=GetBoardInfo(unside,FORPOTEN);
		//�ѱ�����˫��в�㣬����в�㣬˫Ǳ���㣬�Է��Ķ���в�㣬˫��в�㣬˫Ǳ���㣬����в�����ڶ�����
		if(tempMy.duoThreatList.size()>0)
			for(iterP2=tempMy.duoThreatList.begin();iterP2!=tempMy.duoThreatList.end();iterP2++)
				pointList2.push_back(*iterP2);
		if(tempMy.duoPotenList.size()>0)
			for(iterP2=tempMy.duoPotenList.begin();iterP2!=tempMy.duoPotenList.end();iterP2++)
				pointList2.push_back(*iterP2);
		if(tempMy.solThreatList.size()>0)
			for(iterP2=tempMy.solThreatList.begin();iterP2!=tempMy.solThreatList.end();iterP2++)
				pointList2.push_back(*iterP2);
		if(tempDen.duoThreatList.size()>0)
			for(iterP2=tempDen.duoThreatList.begin();iterP2!=tempDen.duoThreatList.end();iterP2++)
				pointList2.push_back(*iterP2);
		if(tempDen.duoPotenList.size()>0)
			for(iterP2=tempDen.duoPotenList.begin();iterP2!=tempDen.duoPotenList.end();iterP2++)
				pointList2.push_back(*iterP2);
		if(tempDen.solThreatList.size()>0)
			for(iterP2=tempDen.solThreatList.begin();iterP2!=tempDen.solThreatList.end();iterP2++)
				pointList2.push_back(*iterP2);
		UniquePoint(pointList2);
		//���û�����ϵڶ����㣬�����ñ����ĵ�Ǳ���㣬Ǳ˫Ǳ����ͶԷ��ĵ�Ǳ����
		if(pointList2.size()<5)
		{
			if(tempMy.solPotenList.size()>0)
				for(iterP2=tempMy.solPotenList.begin();iterP2!=tempMy.solPotenList.end();iterP2++)
					pointList2.push_back(*iterP2);
			if(tempMy.toDuoTwoList.size()>0)
				for(iterP2=tempMy.toDuoTwoList.begin();iterP2!=tempMy.toDuoTwoList.end();iterP2++)
					pointList2.push_back(*iterP2);
			if(tempDen.solPotenList.size()>0)
				for(iterP2=tempDen.solPotenList.begin();iterP2!=tempDen.solPotenList.end();iterP2++)
					pointList2.push_back(*iterP2);
			UniquePoint(pointList2);
		}
		BackMove(tempStep.first,tempLine,side);
		for(iterP2=pointList2.begin();iterP2!=pointList2.end();iterP2++)
		{
			tempStep.second=*iterP2;
			tempStep.value=CalculateStepValue(tempStep,side);
			stepList.push_back(tempStep);
		}
		pointList2.resize(0);
	}
	if(stepList.size()==0)
	{
		printf("�쳣: On make step list for none!\n");
		bool first=false;
		for(int i=0;i<edge;i++)
		{
			for(int j=0;j<edge;j++)
			{
				if(virtualBoard[i][j]==2)
				{
					if(!first)
					{
						tempStep.first.x=i;
						tempStep.first.y=j;
					}
					else
					{
						tempStep.second.x=i;
						tempStep.second.y=j;
						stepList.push_back(tempStep);
						return stepList;
					}
				}
			}
		}
	}
	else
	{
		UniqueStep(stepList);
		sort(stepList.begin(),stepList.end(),cmpStepValue);
		unsigned int size=stepList.size();
		if(size>limit)
		{
			unsigned int i;
			for(i=limit;i<size;i++)
				if(stepList[i].value!=stepList[limit-1].value)
					break;
//			if(i>25)
//				limit=25;
//			else
				limit=i;
			stepList.resize(limit);
		}
	}
	return stepList;
}

/**	MakeStepListForDefendSingleEx - ��������в�ŷ���չ��������������չ����
 *	@return:	�����ŷ��б�
 *	@myInfo:	ִ�巽��ǰ�����ۺ���Ϣ
 *	@denInfo:	�Է���ǰ�����ۺ���Ϣ
 *	@limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForDefendSingleEx(int side,unsigned int limit)
{
	int unside=1-side;
	SynInfo denInfo=GetBoardInfo(unside,TODEFENT);
	SynInfo tempMy;//��ʱ������Ϣ����
	LineInfo tempLine[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Step> stepList;//�з��б�
	vector<Point> pointList;//���б�
	vector<Point>::iterator iterP,iterP2;
	vector<Step>::iterator iterS;

	//ѡȡһ���ƽ��������£�Ȼ��������º�ľ�������鲽��һ������в����������������ƽ��
	for(iterP=denInfo.defPointList.begin();iterP!=denInfo.defPointList.end();iterP++)
	{
		tempStep.first=*iterP;
		MakeMove(tempStep.first,tempLine,side,TODUOTHREAT+TOSOLTHREAT);//���º��ռ�ȫ����Ϣ
		tempMy=GetBoardInfo(side,TODUOTHREAT+TOSOLTHREAT);//�������в����ռ�������Ϣ
		if(tempMy.triThreatList.size()>0)//�����������ɶ���в���ͣ�����˫��в�����������ƽ������鲽�����أ���Ϊ�ж���в���ͣ�����������һ������ʱ��ʤ
		{
			stepList.resize(0);//�ͷ��з��б����Ѵ��ڵ��з���ֻ���������ɶ���в���͵��з�
			tempStep.second=tempMy.triThreatList[0];
			tempStep.value=WINLOSE-1;
			stepList.push_back(tempStep);
			BackMove(tempStep.first,tempLine,side);//��ΪҪ���з��أ���ȡ������
			return stepList;
		}
		//������<˫��в�㣬����в��>��Ϊ��ѡ��
		for(iterP2=tempMy.duoThreatList.begin();iterP2!=tempMy.duoThreatList.end();iterP2++)
			pointList.push_back(*iterP2);
		for(iterP2=tempMy.solThreatList.begin();iterP2!=tempMy.solThreatList.end();iterP2++)
			pointList.push_back(*iterP2);
//		UniquePoint(pointList);//ȥ�ز���
		BackMove(tempStep.first,tempLine,side);
		for(iterP2=pointList.begin();iterP2!=pointList.end();iterP2++)
		{
			tempStep.second=*iterP2;
			tempStep.value=CalculateStepValue(tempStep,side);
			stepList.push_back(tempStep);
		}
		pointList.resize(0);
	}
	for(iterS=denInfo.defStepList.begin();iterS!=denInfo.defStepList.end();iterS++)//������THREAT_four_ADDITION����Ķ����ƽⲽ
		stepList.push_back(*iterS);
	if(stepList.size()!=0)
	{
		UniqueStep(stepList);
		sort(stepList.begin(),stepList.end(),cmpStepValue);

		unsigned int size=stepList.size();
		if(size>limit)
			stepList.resize(limit);
	}
	return stepList;
}

/**	MakeStepListForDefendDoubleEx - ����˫��в���ŷ���չ��������������չ����
 *	@return:	�����ŷ��б�
 *	@myInfo:	ִ�巽��ǰ�����ۺ���Ϣ
 *	@denInfo:	�Է���ǰ�����ۺ���Ϣ
 *	@limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForDefendDoubleEx(int side,unsigned int limit)
{
	int unside=1-side;
	SynInfo denInfo=GetBoardInfo(unside,TODEFENT);
	SynInfo tempMy,tempDen;//��ʱ������Ϣ����
	LineInfo tempLine[2][4],tempLine2[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Step> stepList;//�з��б�
	vector<Point> pointList;//���б�
	vector<Point>::iterator iterP,iterP2;
	vector<Step>::iterator iterS;
	int denType=GetBoardType(unside);

	if(denType>=20)//�Է����ڶ���в���ͻ���ʤ���ͣ��������������˫��в���ͣ������ذ�ֱ�ӷ��ؿ��з��б�
	{
		stepList.resize(0);
		return stepList;
	}
	else if(denType==10)//�˾���ֻ����һ��˫��в���ͣ���ֻ���ƽⲽ��Ϊ��ѡ�з������з��б�
	{
		for(iterS=denInfo.defStepList.begin();iterS!=denInfo.defStepList.end();iterS++)
			stepList.push_back(*iterS);
		UniqueStep(stepList);
		for(iterS=stepList.begin();iterS!=stepList.end();iterS++)
			iterS->value=CalculateStepValue(*iterS,side);
		sort(stepList.begin(),stepList.end(),cmpStepValue);
		return stepList;
	}
	else//���е���в���͵���֣��������в�����ܴ���α˫����һ��˫��в�ӵ���в�����ܴ���α�ࣩ
	{
		if(denType>10&&denType<20)//����һ��˫��в���ͣ�ͬʱ���ڵ���в���ͣ����ܴ���α����в��
		{
			for(iterS=denInfo.defStepList.begin();iterS!=denInfo.defStepList.end();iterS++)//��˫��в����Ϊ��Ҫ�ɷ֣������ƽ��ʹ��˫��в���͵��ƽⲽ
			{
				MakeMove(iterS->first,tempLine,side,0);//�鿴�Ƿ�����ƽ���в��ֻҪ���ߵ����ͣ��ʲ����ռ����µĵ���Ϣ
				MakeMove(iterS->second,tempLine2,side,0);
				if(GetBoardType(unside)==0)//�ж��Ƿ��ƽ�ɹ���������ΪTHREAT_four_ADDITION�Ķ����ƽⷨ����˫��в
					stepList.push_back(*iterS);
				BackMove(iterS->second,tempLine2,side);
				BackMove(iterS->first,tempLine,side);
			}
			if(stepList.size()!=0)//���ƽ�
			{
				UniqueStep(stepList);
				for(iterS=stepList.begin();iterS!=stepList.end();iterS++)
					iterS->value=CalculateStepValue(*iterS,side);
				sort(stepList.begin(),stepList.end(),cmpStepValue);
			}
			return stepList;//���������ƽ���棬�������ƽⷵ�ؿ��з��б�
		}
		else//ֻ���ڶ������в���ͣ����ܴ���α˫��
		{
			for(iterP=denInfo.defPointList.begin();iterP!=denInfo.defPointList.end();iterP++)
			{
				tempStep.first=*iterP;
				MakeMove(tempStep.first,tempLine,side,TODUOTHREAT+TOSOLTHREAT);
				//һ�����ߺ���ж��η����������ɱ������α˫��в���з��ĵڶ��ӳ��ֵ����ƽ�״̬���Ӷ�ʹ��һ������
				if(GetBoardType(unside)==0)//һ���ƽ⣬α˫��в��������в�������ɣ������޽��������߼��������ڿɵ����ƽ������������в
				{
					tempMy=GetBoardInfo(side,TODUOTHREAT+TOSOLTHREAT);
					if(tempMy.triThreatList.size()>0)//�����������ɶ���в���ͣ�����˫��в�����������ƽ������鲽�����أ���Ϊ�ж���в���ͣ�����������һ������ʱ��ʤ
					{
						stepList.resize(0);
						tempStep.second=tempMy.triThreatList[0];
						tempStep.value=WINLOSE-1;
						stepList.push_back(tempStep);
						BackMove(tempStep.first,tempLine,side);
						return stepList;
					}
					//������<˫��в�㣬����в��>
					for(iterP2=tempMy.duoThreatList.begin();iterP2!=tempMy.duoThreatList.end();iterP2++)
						pointList.push_back(*iterP2);
					for(iterP2=tempMy.solThreatList.begin();iterP2!=tempMy.solThreatList.end();iterP2++)
						pointList.push_back(*iterP2);
//					UniquePoint(pointList);
					for(iterP2=pointList.begin();iterP2!=pointList.end();iterP2++)
					{
						tempStep.second=*iterP2;
						stepList.push_back(tempStep);
					}
					pointList.resize(0);
				}
				else//�����ƽ⣬����α˫��в
				{
					tempDen=GetBoardInfo(unside,2);
					for(iterP2=tempDen.defPointList.begin();iterP2!=tempDen.defPointList.end();iterP2++)
					{
						MakeMove(*iterP2,tempLine2,side,0);
						if(GetBoardType(unside)==0)//�ж��Ƿ��ƽ�ɹ�
						{
							tempStep.second=*iterP2;
							stepList.push_back(tempStep);
						}
						BackMove(*iterP2,tempLine2,side);
					}
				}
				BackMove(tempStep.first,tempLine,side);
			}
			if(stepList.size()!=0)//���ƽ�
			{
				UniqueStep(stepList);
				for(iterS=stepList.begin();iterS!=stepList.end();iterS++)
					iterS->value=CalculateStepValue(*iterS,side);
				sort(stepList.begin(),stepList.end(),cmpStepValue);
				if(stepList.size()>limit)
					stepList.resize(limit);
			}
			return stepList;//���������ƽ���棬�������ƽⷵ�ؿ��з��б�
		}
	}
}

/**	MakeStepListForDouble - ˫��в�з�������
 *	@return:	�����ŷ��б�
 *	@myInfo:	ִ�巽��ǰ�����ۺ���Ϣ
 *	@denInfo:	�Է���ǰ�����ۺ���Ϣ
 *	@limit:		�����ŷ�����������
 */
vector<Step> MakeStepListForDouble(int side,unsigned int limit)
{
	int unside=1-side;
	SynInfo myInfo=GetBoardInfo(side,TODUOTHREAT+TOSOLTHREAT+TODUOPOTEN);
	SynInfo tempMy,tempDen;//��ʱ������Ϣ����
	LineInfo tempLine[2][4];//��ʱ����Ϣ����
	Step tempStep;
	vector<Step> stepList;//�з��б�
	vector<Point> pointList,pointList2;//���б�
	vector<Point>::iterator iterP,iterP2;
	vector<Step>::iterator iterS;

	if(myInfo.duoThreatList.size()>0)//����˫��в��
	{
		for(iterP=myInfo.duoThreatList.begin();iterP!=myInfo.duoThreatList.end();iterP++)
		{
			tempStep.first=*iterP;
			MakeMove(tempStep.first,tempLine,side,FORNONTHREAT);
			tempMy=GetBoardInfo(side,FORNONTHREAT);
			if(tempMy.triThreatList.size()>0)//�����ɶ���в���ͣ��Ӷ���4/�ļ�4�ݻ�
			{
				stepList.resize(0);
				tempStep.second=tempMy.triThreatList[0];
				tempStep.value=WINLOSE-1;
				stepList.push_back(tempStep);
				BackMove(tempStep.first,tempLine,side);
				return stepList;
			}
			if(tempMy.duoThreatList.size()>0)//����˫��в���ͱز����ƽ�
			{
				stepList.resize(0);
				tempStep.second=tempMy.duoThreatList[0];
				tempStep.value=WINLOSE-2;
				stepList.push_back(tempStep);
				BackMove(tempStep.first,tempLine,side);
				return stepList;
			}
			tempDen=GetBoardInfo(unside,FORPOTEN);
			//������<����в�㣬˫Ǳ���㣬��Ǳ����>�ͶԷ�<˫��в�㣬����в�㣬˫Ǳ����>��Ϊ��ѡ��
			for(iterP2=tempMy.solThreatList.begin();iterP2!=tempMy.solThreatList.end();iterP2++)//��������в��
				pointList.push_back(*iterP2);
			for(iterP2=tempMy.duoPotenList.begin();iterP2!=tempMy.duoPotenList.end();iterP2++)//����˫Ǳ����
				pointList.push_back(*iterP2);
			if(tempDen.duoThreatList.size()>0)
			{
				for(iterP2=tempDen.duoThreatList.begin();iterP2!=tempDen.duoThreatList.end();iterP2++)//�Է�˫��в��
					pointList.push_back(*iterP2);
			}
			else
			{
				for(iterP2=tempMy.solPotenList.begin();iterP2!=tempMy.solPotenList.end();iterP2++)//������Ǳ����
					pointList.push_back(*iterP2);
				for(iterP2=tempDen.solThreatList.begin();iterP2!=tempDen.solThreatList.end();iterP2++)//�Է�����в��
					pointList.push_back(*iterP2);
				for(iterP2=tempDen.duoPotenList.begin();iterP2!=tempDen.duoPotenList.end();iterP2++)//�Է�˫Ǳ����
					pointList.push_back(*iterP2);
			}
			if(pointList.size()==0)
			{
				for(iterP2=tempMy.toDuoTwoList.begin();iterP2!=tempMy.toDuoTwoList.end();iterP2++)//����Ǳ˫Ǳ����
					pointList.push_back(*iterP2);
			}
			BackMove(tempStep.first,tempLine,side);
			UniquePoint(pointList);
			for(iterP2=pointList.begin();iterP2!=pointList.end();iterP2++)
			{
				tempStep.second=*iterP2;
				stepList.push_back(tempStep);
			}
			pointList.resize(0);
		}
	}
	if(myInfo.solThreatList.size()>0)//���ڵ���в&&˫Ǳ����
	{
		for(iterP=myInfo.solThreatList.begin();iterP!=myInfo.solThreatList.end();iterP++)
		{
			tempStep.first=*iterP;
			MakeMove(tempStep.first,tempLine,TODUOTHREAT);
			tempMy=GetBoardInfo(side,TODUOTHREAT);
			if(tempMy.triThreatList.size()>0)//�����ɶ���в����
			{
				stepList.resize(0);
				tempStep.second=tempMy.triThreatList[0];
				tempStep.value=WINLOSE-1;
				stepList.push_back(tempStep);
				BackMove(tempStep.first,tempLine,side);
				return stepList;
			}
			BackMove(tempStep.first,tempLine,side);
			if(tempMy.duoThreatList.size()!=0)
			{
				for(iterP2=tempMy.duoThreatList.begin();iterP2!=tempMy.duoThreatList.end();iterP2++)//����˫��в��
				{
					tempStep.second=*iterP2;
					stepList.push_back(tempStep);
				}
			}
			pointList.resize(0);
		}
	}
	if(myInfo.duoPotenList.size()>0)//����˫Ǳ����
	{
		for(iterP=myInfo.duoPotenList.begin();iterP!=myInfo.duoPotenList.end();iterP++)
		{
			tempStep.first=*iterP;
			MakeMove(tempStep.first,tempLine,side,TODUOTHREAT);
			tempMy=GetBoardInfo(side,TODUOTHREAT);//����˫��в��
			if(tempMy.triThreatList.size()>0)//�����ɶ���в���ͣ��Ӷ���4/�ļ�4�ݻ�
			{
				stepList.resize(0);
				tempStep.second=tempMy.triThreatList[0];
				tempStep.value=WINLOSE-1;
				stepList.push_back(tempStep);
				BackMove(tempStep.first,tempLine,side);
				return stepList;
			}
			BackMove(tempStep.first,tempLine,side);
			for(iterP2=tempMy.duoThreatList.begin();iterP2!=tempMy.duoThreatList.end();iterP2++)
			{
				tempStep.second=*iterP2;
				stepList.push_back(tempStep);
			}
		}
	}
	if(stepList.size()>0)
	{
		UniqueStep(stepList);
		for(iterS=stepList.begin();iterS!=stepList.end();iterS++)
			iterS->value=CalculateStepValue(*iterS,side);
		sort(stepList.begin(),stepList.end(),cmpStepValue);
		if(stepList.size()>limit)
			stepList.resize(limit);
	}
	return stepList;
}


//��������Ϣ����Ե����ڵ�4���ߣ�������Ϣ���浽��ʱ������
void BackupLine(Point point,LineInfo tempLine[4],BYTE side)
{
	if(point.x==-1)return;
	int key;
	Point start;
	key=GetLineKey(point,&start,ANGLE0);
	if(key!=-1)
		CopyLineInfo(tempLine[ANGLE0],lineInfo[side][key]);
	key=GetLineKey(point,&start,ANGLE90);
	if(key!=-1)
		CopyLineInfo(tempLine[ANGLE90],lineInfo[side][key]);
	key=GetLineKey(point,&start,ANGLE45);
	if(key!=-1)
		CopyLineInfo(tempLine[ANGLE45],lineInfo[side][key]);
	key=GetLineKey(point,&start,ANGLE135);
	if(key!=-1)
		CopyLineInfo(tempLine[ANGLE135],lineInfo[side][key]);
}
//�ָ�����Ϣ����Ե����ڵ�4���ߣ�����ʱ�����е���Ϣ���ǵ�ǰ��Ϣ
void RecoverLine(Point point,LineInfo tempLine[4],BYTE side)
{
	if(point.x==-1)return;
	int key;
	Point start;
	key=GetLineKey(point,&start,ANGLE0);
	if(key!=-1)
		CopyLineInfo(lineInfo[side][key],tempLine[ANGLE0]);
	key=GetLineKey(point,&start,ANGLE90);
	if(key!=-1)
		CopyLineInfo(lineInfo[side][key],tempLine[ANGLE90]);
	key=GetLineKey(point,&start,ANGLE45);
	if(key!=-1)
		CopyLineInfo(lineInfo[side][key],tempLine[ANGLE45]);
	key=GetLineKey(point,&start,ANGLE135);
	if(key!=-1)
		CopyLineInfo(lineInfo[side][key],tempLine[ANGLE135]);
}

//���ߣ�ͬʱ�Ա�����߽��б���
void MakeMove(Point point,LineInfo tempLine[2][4],BYTE side,int tag)
{
	BackupLine(point,tempLine[BLACK],BLACK);
	BackupLine(point,tempLine[WHITE],WHITE);
	virtualBoard[point.x][point.y]=side;
	UpdateLineForCross(point,BLACK,tag);
	UpdateLineForCross(point,WHITE,tag);
}
//ȡ�����ߣ�ͬʱ�ָ����������Ϣ
void BackMove(Point point,LineInfo tempLine[2][4],BYTE side)
{
	virtualBoard[point.x][point.y]=EMPTY;
	RecoverLine(point,tempLine[BLACK],BLACK);
	RecoverLine(point,tempLine[WHITE],WHITE);
}

/**	CalSingleLineValue - �߼�ֵ������
 *	@return:	�����ߵļ�ֵ
 *	@start:		�ߵ���ʼ������
 *	@Direct:	�ߵķ����־
 *	@side:		��������ߵļ�ֵ��Ϣ��������ִ����ɫ
 */
int CalSingleLineValue(const Point start,const BYTE Direc,const BYTE side) //���������Direc������ߵļ�ֵ
{
	int shapeIndex=0;	// ���ͱ��
	int x=start.x;	// ����㿪ʼ��
	int y=start.y;
	int len=0;				//  ��¼��ǰλ������Ҫ��Ķ��е�λ��, �������Ϊ��ǰ�γ�
	int value=0;
	while(x<edge&&y<edge&&x>-1&&y>-1)	//	����������
	{							
		if(virtualBoard[x][y]==EMPTY)
		{
			len++;
		}
		else if(virtualBoard[x][y]==side)		// ����Ǳ������ӡ�
		{
			shapeIndex+=(1<<len);	
			len++;
		}	
		else		//  ����ǶԷ����ӡ�
		{
			if(len>5)	//���ȳ���5���Ż����ӡ�
			{
				shapeIndex+=(1<<len);	  // �õ���������֪ʶ���е�λ�á�
				value+=LineTypeValue[preTable[shapeIndex]];
			}
			shapeIndex=0;	// ƫ��������
			len=0;		// ���ȹ���
		}
		Increment(x, y, Direc);
	}
	if(len>5)	//���ȳ���5���Ż����ӡ�
	{
		shapeIndex+=(1<<len);	  // �õ�������
		value+=LineTypeValue[preTable[shapeIndex]];
	}
	return value;
}

/**	CalSingleLineType - �����ͼ�����
 *	@return:	�����ߵ����ͣ����Գɷ�
 *	@start:		�ߵ���ʼ������
 *	@Direct:	�ߵķ����־
 *	@side:		��������ߵ�������Ϣ��������ִ����ɫ
 */
int CalSingleLineType(const Point start,const BYTE Direc,const BYTE side) //���������Direc������ߵļ�ֵ
{
	int shapeIndex=0;	// ���ͱ��
	int x=start.x;	// ����㿪ʼ��
	int y=start.y;
	int len=0;				//  ��¼��ǰλ������Ҫ��Ķ��е�λ��, �������Ϊ��ǰ�γ�
	int value=0;
	while(x<edge&&y<edge&&x>-1&&y>-1)	//	����������
	{							
		if(virtualBoard[x][y]==EMPTY)
		{
			len++;
		}
		else if(virtualBoard[x][y]==side)		// ����Ǳ������ӡ�
		{
			shapeIndex+=(1<<len);	
			len++;
		}	
		else		//  ����ǶԷ����ӡ�
		{
			if(len>5)	//���ȳ���5���Ż����ӡ�
			{
				shapeIndex+=(1<<len);	  // �õ���������֪ʶ���е�λ�á�
				value+=LineTypeType[preTable[shapeIndex]];
			}
			shapeIndex=0;	// ƫ��������
			len=0;		// ���ȹ���
		}
		Increment(x, y, Direc);
	}
	if(len>5)	//���ȳ���5���Ż����ӡ�
	{
		shapeIndex+=(1<<len);	  // �õ�������
		value+=LineTypeType[preTable[shapeIndex]];
	}
	return value;
}


#define USEVALUE

/**	CalculateStepValue - �ŷ���ֵ������
 *	@return:	�����ŷ���ֵ
 *	@step:		��Ҫ��������ŷ�
 *	@side:		�ŷ�������ִ����ɫ
 */
int CalculateStepValue(const Step step,const BYTE side)
#ifdef USEVALUE
{
	int key1,key2,i,unside=1-side;
	int myC=0,denC=0;//˫���仯�����ͼ�ֵ��
	Point start;

	virtualBoard[step.first.x][step.first.y]=side;
	virtualBoard[step.second.x][step.second.y]=side;
	for(i=0;i<4;i++)
	{
		key1=GetLineKey(step.first,&start,i);
		if(key1!=-1)
		{
			myC+=CalSingleLineValue(start,i,side);
			denC+=CalSingleLineValue(start,i,unside);
			myC-=lineInfo[side][key1].value;
			denC-=lineInfo[unside][key1].value;
		}
		key2=GetLineKey(step.second,&start,i);
		if(key2==key1)
			continue;
		if(key2!=-1)
		{
			myC+=CalSingleLineValue(start,i,side);
			denC+=CalSingleLineValue(start,i,unside);
			myC-=lineInfo[side][key2].value;
			denC-=lineInfo[unside][key2].value;
		}
	}
	virtualBoard[step.first.x][step.first.y]=EMPTY;
	virtualBoard[step.second.x][step.second.y]=EMPTY;
//	return myC-denC;
	return (myC*4-denC*7)/10;
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