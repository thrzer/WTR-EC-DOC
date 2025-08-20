#ifndef __ACTION_H
#define __ACTION_H

#include "usermain.h"

extern uint8_t down_flag;

void Reset_Action(void);

void Seed_Grip_Action(void);
void Seed_Plant_Action(void);

void Ball_Pick_Action(void);
void Ball_Fire_Action(void);

#endif