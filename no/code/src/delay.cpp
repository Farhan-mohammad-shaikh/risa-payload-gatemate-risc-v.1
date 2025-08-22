#include "logging.h"
#include "delay.h"

Timer* timer;

void delayUseTimer(Timer* new_timer)
{
	timer = new_timer;
}

void delayUS(uint32_t us)
{
	if(timer)
	{
		uint32_t start = timer->getTime();
		uint32_t delay = (us * MICROSECOND);

		while(timer->passed(start) < delay)
		{
			// Waiting...
		}
	}
	else
	{
		LOGWARN("no timer defined for delay");
	}
}