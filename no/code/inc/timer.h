#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>
#include <generated/soc.h>
#include <generated/csr.h>

/* Time Units */
constexpr uint32_t SECOND = CONFIG_CLOCK_FREQUENCY;
constexpr uint32_t MILLISECOND = SECOND / 1000;
constexpr uint32_t MICROSECOND = MILLISECOND / 1000;

/* Control Registers */
constexpr uint32_t TIMER_LOAD_OFFSET = 0x00;
constexpr uint32_t TIMER_RELOAD_OFFSET = 0x04;
constexpr uint32_t TIMER_EN_OFFSET = 0x08;
constexpr uint32_t TIMER_UPDATE_OFFSET = 0x0C;
constexpr uint32_t TIMER_VALUE_OFFSET = 0x10;

/* Interrupt Registers*/
constexpr uint32_t TIMER_EV_STATUS_OFFSET = 0x14;
constexpr uint32_t TIMER_EV_PENDING_OFFSET = 0x18;
constexpr uint32_t TIMER_EV_ENABLE_OFFSET = 0x1C;


enum TimerID
{
	TIMER0,

#ifdef CSR_TIMER1_BASE
	TIMER1,
#endif

#ifdef CSR_TIMER2_BASE
	TIMER2,
#endif
};

/**
 * @brief This timer works in Countdown mode, getTime and passed work in the units of clock ticks
 */
class Timer
{
public:
	/**
	 * @brief Gives back an Object that can control the Timer
	 * 
	 * @param id the ID of the Timer, enum values can be found in this header
	 */
	Timer(TimerID id);

	/**
	 * @brief Makes the Timer start counting
	 */
	void start();

	/**
	 * @brief Makes the Timer stop counting
	 */
	void stop();

	/**
	 * @brief Sets the Timer to the UpperLimit Value
	 */
	void reset();

	/**
	 * @brief Sets the Upperlimit, Note: Since this is a Countdown timer its Basically 
	 * the Value it starts at once it reaches 0
	 * 
	 * @param limit the upper limit to be set in clock cycles
	 */
	void setUpperLimit(uint32_t limit);

	/**
	 * @brief Force Set the Current Value of the Timer
	 * 
	 * @param time the time to be set in clock cycles
	 */
	void setTime(uint32_t newtime);

	/**
	 * @brief Get the current Time
	 * 
	 * @retval returns time of the current timer 
	 */
	uint32_t getTime();

	/**
	 * @brief Gives the Time passed between now and since, has Overflow Correction
	 * 
	 * @retval Delta between the given and current time
	 */
	uint32_t passed(uint32_t since);

	/**
	 * @brief Gets the ID of the Timer, meaning whether it is Timer 0, 1 or 2
	 */
	TimerID getID();

private: 
	uint32_t upper_limit;
	uint32_t timer_base_addr;
	const TimerID id;
};

#endif /* _TIMER_H_ */