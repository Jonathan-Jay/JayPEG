#ifndef __TIMER_H__
#define __TIMER_H__

#include <cstdio>
#include <chrono>
#include <thread>

using std::milli;
using std::chrono::system_clock;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::this_thread::sleep_for;

class Timer abstract
{
public:
	static double minFrameTime, deltaTime, time;
	static system_clock::time_point currentClock, lastClock;

	//Resets the clock to zero
	static void Reset();
	//Updates the clock using ctime
	static void Update();
};

#endif // !__TIMER_H__
