#ifndef SIXGO_H_DEBUG
#define SIXGO_H_DEBUG

#include "Seach.h"
#include "direct.h"
#include "time.h"

#ifdef _DEBUG
#define DEBUGVALUE
#endif

class Debugger{
public:
	Debugger(){ _mkdir("SIXGO_DEBUG"); }
	void OutputStep(vector<Step> &stepList,const int side);
	void OutputMessage(char *message, const int side);
	void MakeMove(Step step);
	void BackMove(Step step);
	void InitDir();
	void InitDebugger(int timestamp, Step step);
	void OutSelect(Step step);

private:

	class StepStack{
	public:
		static const int STEPMAX = 181;
		Step stepStack[STEPMAX];
		int top;
		StepStack() {
			top = -1;
		}
		int push(Step step) {
			if (top >= STEPMAX - 1) {
				top = STEPMAX - 1;
				return -1;
			}
			stepStack[++top] = step;
			return 0;
		}
		int pop() {
			if (top < 0) {
				top = -1;
				return -1;
			}
			top--;
			return 0;
		}
	};

	StepStack stepStack;
	int timestamp;
	time_t timeDir;
	string pathRoot;

	string getPath();
	void showBoard(FILE *fp, const int nBoard[edge][edge]);
};


extern Debugger debugger;

#endif