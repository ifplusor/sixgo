#ifndef SIXGO_H_SEACH
#define SIXGO_H_SEACH

#include "hash.h"
#include "MaintainInfo.h"
#include "StepGenerator.h"
#include "Debug.h"


#define DEPTH 5
#define MAXDEPTH 13


Step sixgo_carry(const Step moveStep,const int nBoard[19][19],const BYTE side);


#endif