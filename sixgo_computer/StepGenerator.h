#ifndef SIXGO_H_STEP
#define SIXGO_H_STEP

#include "connect_six.h"
#include "MaintainInfo.h"


vector<Step> MakeStepListForWin(int side, unsigned int limit);
vector<Step> MakeStepListForDefendSingle(int side, unsigned int limit);
vector<Step> MakeStepListForDefendDouble(int side, unsigned int limit);
vector<Step> MakeStepListForNone(int side, unsigned int limit);
vector<Step> MakeStepListForDefendSingleEx(int side, unsigned int limit);
vector<Step> MakeStepListForDefendDoubleEx(int side, unsigned int limit);
vector<Step> MakeStepListForDouble(int side, unsigned int limit);


#endif