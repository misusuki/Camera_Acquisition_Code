#ifndef _MYTIMER_H_
#define _MYTIMER_H_

#include <time.h>
#include <unistd.h>

class MyTimer
{
private:
	struct timespec start;

public:
	MyTimer() {
		Start();
	}
	void Start() {
		clock_gettime(CLOCK_REALTIME, &start);
	}
	long Elapsed() {
		struct timespec now;
		clock_gettime(CLOCK_REALTIME, &now);
		long sec = now.tv_sec - start.tv_sec;
		long nsec = now.tv_nsec - start.tv_nsec;
		while (nsec < 0){ sec--; nsec += 1000000000L; }
		return ((sec * 1000000L) + (nsec / 1000L));
	}
};

#endif
