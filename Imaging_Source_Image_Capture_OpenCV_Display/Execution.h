#include <iostream>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>

double Execution[30];

class  Execution_Watch
{
public:
    // Constructor.
    Execution_Watch()
    {   
    }
    // Start counter.
    void TStart()
     { 
		clock_gettime(CLOCK_REALTIME, &start_time); 
	 }
    // Stop counter.
    void TStop()
    {   
		clock_gettime(CLOCK_REALTIME, &end_time); 
     }
    // Get duration.
    double Execution_Time()
    {   
		double sec = end_time.tv_sec - start_time.tv_sec;
		double nsec = end_time.tv_nsec - start_time.tv_nsec;
		double mseconds = ((sec) * 1000000L + nsec/1000000L) ;
		return mseconds;
    }
       
	
private:
	struct timespec start_time; 
	struct timespec end_time;  
};

// Execution watch object.
 Execution_Watch timer[30];



