#ifndef SIXGO_H_DEBUG
#define SIXGO_H_DEBUG

#include "Seach.h"

void OutputStep(vector<Step> &stepList,const int side,int tag);
int CheckChess(const int nBoard[edge][edge], const int side);
void OutputBoard(const int nBoard[edge][edge], const int side);


#endif