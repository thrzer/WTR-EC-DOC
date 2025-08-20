#include "HighPrecisionTime/high_precision_time.h"

static unsigned long RunTimeStatusStartUs = 0;

void configureTimerForRunTimeStats(void)
{
    RunTimeStatusStartUs = HPT_GetUs();
}

unsigned long getRunTimeCounterValue(void)
{
    return HPT_GetUs() - RunTimeStatusStartUs;
}
