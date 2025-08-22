#include "timer.h"

Timer::Timer(TimerID id): id(id)
{
	switch(id)
	{
#ifdef CSR_TIMER0_BASE
		case TIMER0: timer_base_addr = CSR_TIMER0_BASE; break;
#endif

#ifdef CSR_TIMER1_BASE
		case TIMER1: timer_base_addr = CSR_TIMER1_BASE; break;
#endif

#ifdef CSR_TIMER2_BASE
		case TIMER2: timer_base_addr = CSR_TIMER2_BASE; break;
#endif
	}
}

void Timer::start()
{
	csr_write_simple(0x01, timer_base_addr + TIMER_EN_OFFSET);
}

void Timer::stop()
{
	csr_write_simple(0x00, timer_base_addr + TIMER_EN_OFFSET);
}

void Timer::reset()
{
	setTime(0);
}

void Timer::setUpperLimit(uint32_t limit)
{
	csr_write_simple(limit, timer_base_addr + TIMER_RELOAD_OFFSET);
}

void Timer::setTime(uint32_t newtime)
{
	csr_write_simple(newtime, timer_base_addr + TIMER_LOAD_OFFSET);
}

uint32_t Timer::getTime()
{
	csr_write_simple(0x01, timer_base_addr + TIMER_UPDATE_OFFSET);
	return csr_read_simple(timer_base_addr + TIMER_VALUE_OFFSET);
}

uint32_t Timer::passed(uint32_t since)
{
	uint32_t now = getTime();

	if(now <= since)
	{
		return (since - now);
	}
	else
	{
		// rollover has occured
		return (since + (1 + upper_limit - now));
	}
}

TimerID Timer::getID()
{
	return id;
}
