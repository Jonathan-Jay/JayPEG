#include "Timer.h"

system_clock::time_point Timer::currentClock = system_clock::now();
system_clock::time_point Timer::lastClock = system_clock::now();
double Timer::minFrameTime = 1000.f / 60.f;
double Timer::deltaTime = 0;
double Timer::time = 0;

void Timer::Reset()
{
	//Grabs current clock and stores it in last and current clock
	lastClock = system_clock::now();
	currentClock = system_clock::now();
	//Resets the total time passed and time since last frame
	time = 0.f;
	deltaTime = 0.f;
}

void Timer::Update() {
	//find the amount of time that has passed since this function was last called
	currentClock = system_clock::now();
	duration<double, milli> work_time = currentClock - lastClock;

	//if the amount of time that has passed is smaller than the minimuim wanted frame time it'll find out how long it needs to wait and stop the program from running for a certain amount of time
	if (work_time.count() < minFrameTime) {
		duration<double, milli> delta_ms(minFrameTime - work_time.count());
		milliseconds delta_ms_duration = duration_cast<milliseconds>(delta_ms);
		sleep_for(milliseconds(delta_ms_duration.count()));
	}

	//finds how long the system has slept/stopped, then adds the length of the since this function's last call which gives delta time
	lastClock = system_clock::now();
	duration<double, milli> sleep_time = lastClock - currentClock;
	deltaTime = (work_time + sleep_time).count() / 1000;

	time += deltaTime;
}
